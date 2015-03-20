/*******************************************************************
    Copyright (C) 2009 FreakLabs
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions
    are met:

    1. Redistributions of source code must retain the above copyright
       notice, this list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright
       notice, this list of conditions and the following disclaimer in the
       documentation and/or other materials provided with the distribution.
    3. Neither the name of the the copyright holder nor the names of its contributors
       may be used to endorse or promote products derived from this software
       without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS'' AND
    ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
    ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
    FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
    DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
    OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
    HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
    LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
    OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
    SUCH DAMAGE.

    Originally written by Christopher Wang aka Akiba.
    Please post support questions to the FreakLabs forum.

*******************************************************************/

/**************************************************************************
	Souliss - vNet Virtualized Network
	
	Modified by Dario Di Maio
	
***************************************************************************/

#include <avr/pgmspace.h>
#include "Arduino.h"
#include "chb.h"
#include "chb_drvr.h"
#include "chb_buf.h"
#include "chb_spi.h"

#if (CHB_RX_POLLING_MODE)
   #include "chb_rx_poll.h"
   extern bool rx_intp_flag;
#else
   #include "chb_rx_int.h"
#endif

/**************************************************************************/
/*!
    Retrieve the state from the radio's state machine register.
*/
/**************************************************************************/
static U8 chb_get_state()
{
    return chb_reg_read(TRX_STATUS) & 0x1f;
}

/**************************************************************************/
/*!
    Retrieve the status of the last received frame. 
*/
/**************************************************************************/
static U8 chb_get_status()
{
    return chb_reg_read(TRX_STATE) >> CHB_TRAC_STATUS_POS;
}

/**************************************************************************/
/*!
    Perform a delay of the specified amount of microseconds.
*/
/**************************************************************************/
static void chb_delay_us(U16 usec)
{
    do
    {
        delayMicroseconds(usec);
    } while (--usec);
}

/**************************************************************************/
/*!
    Read the radio's registers for the specified address. Interrupts are disabled
    for the duration of the reading to prevent an interrupt from interfering
    with the operation.
*/
/**************************************************************************/
U8 chb_reg_read(U8 addr)
{
    U8 val = 0;
	
    /* Add the register read command to the register address. */
    addr |= CHB_SPI_CMD_RR;

    CHB_ENTER_CRIT();
    CHB_SPI_ENABLE();

    /*Send Register address and read register content.*/
    val = chb_xfer_byte(addr);
    val = chb_xfer_byte(0);

    CHB_SPI_DISABLE();
    CHB_LEAVE_CRIT();

    return val;
	
/*    
	U8 val = 0;
	
    // Add the register read command to the register address. 
    addr |= CHB_SPI_CMD_RR;

    CHB_ENTER_CRIT();
    CHB_SPI_ENABLE();

    // Send Register address and read register content.
    val = chb_xfer_byte(addr);
    val = chb_xfer_byte(val);

    CHB_SPI_DISABLE();
    CHB_LEAVE_CRIT();

    return val;

*/
}

/**************************************************************************/
/*! 
    Read a 16-bit register. This actually does two consecutive 8-bit reads
    of contiguous registers. It's mostly used to retrieve the PAN ID or the
    short address of the node.
*/
/**************************************************************************/
U16 chb_reg_read16(U8 addr)
{
    U8 i;
    U16 val = 0;

    for (i=0; i<2; i++)
    {
        addr |= chb_reg_read(addr + i) << (8 * i);
    }
    return val;
}

/**************************************************************************/
/*!
    Write to the radio's registers at the specified address with the specified
    value.
*/
/**************************************************************************/
void chb_reg_write(U8 addr, U8 val)
{
    U8 dummy; 
	
    /* Add the Register Write command to the address. */
    addr |= 0xC0;

    CHB_ENTER_CRIT();
    CHB_SPI_ENABLE();

    /*Send Register address and write register content.*/
    dummy = chb_xfer_byte(addr);
    dummy = chb_xfer_byte(val);

    CHB_SPI_DISABLE();
    CHB_LEAVE_CRIT();
}

/**************************************************************************/
/*!
    Write 16-bits to the radio's registers. This does two consecutive 8-bit
    writes to contiguous memory addresses. 
*/
/**************************************************************************/
void chb_reg_write16(U8 addr, U16 val)
{
    U8 i;

    for (i=0; i<2; i++)
    {
        chb_reg_write(addr + i, val >> (8 * i));
    }
}

/**************************************************************************/
/*!
    Write 64-bits to the radio's registers. This does 8 consecutive 8-bit
    writes to contiguous memory addresses. This is only used to write the
    IEEE address in case it is used. The default for Chibi is just to use
    16-bit addressing.
*/
/**************************************************************************/
void chb_reg_write64(U8 addr, U8 *val)
{
    U8 i;

    for (i=0; i<8; i++)
    {
        chb_reg_write(addr + i, *(val + i));
    }
}

/**************************************************************************/
/*!
    Perform a read/modify/write to a register. This function first reads
    the specified register, then masks off the bits that will be modified.
    It then changes the modified bits and then writes it back to the register.
    It's used to modify registers without changing bits that are outside
    of the mask. 
*/
/**************************************************************************/
void chb_reg_read_mod_write(U8 addr, U8 val, U8 mask)
{
    U8 tmp;

    tmp = chb_reg_read(addr);
    val &= mask;                // mask off stray bits from val
    tmp &= ~mask;               // mask off bits in reg val
    tmp |= val;                 // copy val into reg val
    chb_reg_write(addr, tmp);   // write back to reg
}

/**************************************************************************/
/*!
    Write data to the frame FIFO of the radio. This is where the data to
    be transmitted gets written. The frame data is written in two separate bursts.
    The first burst is to write the frame header and the second burst is
    to write the frame payload. Makes things easier this way.
*/
/**************************************************************************/
void chb_frame_write(oFrame *frame)
{
    U8 i, dummy;

/*
    // dont allow transmission longer than max frame size
    if ((hdr_len + data_len) > 127)
    {
        return;
    }
*/

    // initiate spi transaction
    CHB_ENTER_CRIT();
    CHB_SPI_ENABLE(); 

    // send fifo write command
    dummy = chb_xfer_byte(CHB_SPI_CMD_FW);

	// Define the frame and write into the fifo
	oFrame_Define(frame);
	while(oFrame_Available())
		dummy = chb_xfer_byte(oFrame_GetByte());
	
	/*
    // write hdr contents to fifo
    for (i=0; i<hdr_len; i++)
    {
        dummy = chb_xfer_byte(*hdr++);
    }

    // write data contents to fifo
    for (i=0; i<data_len; i++)
    {
        dummy = chb_xfer_byte(*data++);
    }
	*/ 
	
    // terminate spi transaction
    CHB_SPI_DISABLE(); 
    CHB_LEAVE_CRIT();
}

/**************************************************************************/
/*!
    Read the data from the frame FIFO. When data arrives at the radio, it will
    get stored in the frame FIFO. The data will get read out from this function
    and stored in the ring buffer. 
*/
/**************************************************************************/
U8 chb_frame_read()
{
	bool flag = false;
    U8 i, len, data;

    CHB_ENTER_CRIT();
    CHB_SPI_ENABLE();

    /*Send frame read command and read the length.*/
    chb_xfer_byte(CHB_SPI_CMD_FR);
    len = chb_xfer_byte(0);

    // check the length of the frame to make sure its within the correct size limits
    if ((len >= CHB_MIN_FRAME_LENGTH) && (len <= CHB_MAX_FRAME_LENGTH))
    {
        // check to see if there is room to write the frame in the buffer. if not, then drop it
        if (len < (CHB_BUF_SZ - chb_buf_get_len()))
        {
            chb_buf_write(len);

            for (i=0; i<len; i++)
            {
                data = chb_xfer_byte(0);
                chb_buf_write(data);
            }

			flag = true;
        }
        else
        {
            // this frame will overflow the buffer. toss the data and do some housekeeping
            pcb_t *pcb = chb_get_pcb();

            // read out the data and throw it away
            for (i=0; i<len; i++)
            {
                data = chb_xfer_byte(0);
            }

            // Increment the overflow stat, and print a message.
            pcb->overflow++;
        }
					
    }

    CHB_SPI_DISABLE();
    CHB_LEAVE_CRIT();
	
	if(flag)
		return RADIO_SUCCESS;
	else
		return 0;
	
}

/**************************************************************************/
/*!
    Read directly from the SRAM on the radio. This is only used for debugging
    purposes.
*/
/**************************************************************************/
#ifdef CHB_DEBUG
void chb_sram_read(U8 addr, U8 len, U8 *data)
{
    U8 i, dummy;

    CHB_ENTER_CRIT();
    CHB_SPI_ENABLE();

    /*Send SRAM read command.*/
    dummy = chb_xfer_byte(CHB_SPI_CMD_SR);

    /*Send address where to start reading.*/
    dummy = chb_xfer_byte(addr);

    for (i=0; i<len; i++)
    {
        *data++ = chb_xfer_byte(0);
    }

    CHB_SPI_DISABLE();
    CHB_LEAVE_CRIT();
}

/**************************************************************************/
/*!
    Write to the SRAM of the radio. This is only used for debug purposes.
*/
/**************************************************************************/
void chb_sram_write(U8 addr, U8 len, U8 *data)
{    
    U8 i, dummy;

    CHB_ENTER_CRIT();
    CHB_SPI_ENABLE();

    /*Send SRAM write command.*/
    dummy = chb_xfer_byte(CHB_SPI_CMD_SW);

    /*Send address where to start writing to.*/
    dummy = chb_xfer_byte(addr);

    for (i=0; i<len; i++)
    {
        dummy = chb_xfer_byte(*data++);
    }

    CHB_SPI_DISABLE();
    CHB_LEAVE_CRIT();
}
#endif

/**************************************************************************/
/*!
    Set the TX/RX state machine state. Some manual manipulation is required 
    for certain operations. Check the datasheet for more details on the state 
    machine and manipulations.
*/
/**************************************************************************/
static U8 chb_set_state(U8 state)
{
    U8 curr_state, delay;

    // if we're sleeping then don't allow transition
    if (CHB_SLPTR_PORT & _BV(CHB_SLPTR_PIN))
    {
        return RADIO_WRONG_STATE;
    }

    // if we're in a transition state, wait for the state to become stable
    curr_state = chb_get_state();
    if ((curr_state == BUSY_TX_ARET) || (curr_state == BUSY_RX_AACK) || (curr_state == BUSY_RX) || (curr_state == BUSY_TX))
    {
        while (chb_get_state() == curr_state);
    }

    // At this point it is clear that the requested new_state is:
    // TRX_OFF, RX_ON, PLL_ON, RX_AACK_ON or TX_ARET_ON.
    // we need to handle some special cases before we transition to the new state
    switch (state)
    {
    case TRX_OFF:
        /* Go to TRX_OFF from any state. */
        CHB_SLPTR_DISABLE();
        chb_delay_us(TIME_NOCLK_TO_WAKE);
        chb_reg_read_mod_write(TRX_STATE, CMD_FORCE_TRX_OFF, 0x1f);
        chb_delay_us(TIME_CMD_FORCE_TRX_OFF);
        break;

    case TX_ARET_ON:
        if (curr_state == RX_AACK_ON)
        {
            /* First do intermediate state transition to PLL_ON, then to TX_ARET_ON. */
            chb_reg_read_mod_write(TRX_STATE, CMD_PLL_ON, 0x1f);
            chb_delay_us(TIME_STATE_TRANSITION_PLL_ACTIVE);
        }
        break;

    case RX_AACK_ON:
        if (curr_state == TX_ARET_ON)
        {
            /* First do intermediate state transition to RX_ON, then to RX_AACK_ON. */
            chb_reg_read_mod_write(TRX_STATE, CMD_PLL_ON, 0x1f);
            chb_delay_us(TIME_STATE_TRANSITION_PLL_ACTIVE);
        }
        break;
    }

    /* Now we're okay to transition to any new state. */
    chb_reg_read_mod_write(TRX_STATE, state, 0x1f);

    /* When the PLL is active most states can be reached in 1us. However, from */
    /* TRX_OFF the PLL needs time to activate. */
    delay = (curr_state == TRX_OFF) ? TIME_TRX_OFF_TO_PLL_ACTIVE : TIME_STATE_TRANSITION_PLL_ACTIVE;
    chb_delay_us(delay);

    if (chb_get_state() == state)
    {
        return RADIO_SUCCESS;
    }
    return RADIO_TIMED_OUT;
}

/**************************************************************************/
/*! 
    Sets the IEEE address of the radio.
*/
/**************************************************************************/
/*void chb_set_ieee_addr(U8 *addr)
{
    chb_eeprom_write(CHB_EEPROM_IEEE_ADDR, addr, 8); 
    chb_reg_write64(IEEE_ADDR_0, addr); 
}*/

/**************************************************************************/
/*!
    Retrieves the IEEE address of the radio.
*/
/**************************************************************************/
/*void chb_get_ieee_addr(U8 *addr)
{
    chb_eeprom_read(CHB_EEPROM_IEEE_ADDR, addr, 8);
}*/

/**************************************************************************/
/*!
    Sets the 16-bit short address of the radio.
*/
/**************************************************************************/
void chb_set_short_addr(U16 addr)
{
    U8 *addr_ptr = (U8 *)&addr;
    pcb_t *pcb = chb_get_pcb();

//    chb_eeprom_write(CHB_EEPROM_SHORT_ADDR, addr_ptr, 2);
    chb_reg_write16(SHORT_ADDR_0, addr);
    pcb->src_addr = addr;
}

/**************************************************************************/
/*!
    Retrieves the 16-bit short address of the radio.
*/
/**************************************************************************/
U16 chb_get_short_addr()
{
    //U8 addr[2];
	U16 addr;												// Added by Dario
	pcb_t *pcb = chb_get_pcb();								// Added by Dario
	addr = pcb->src_addr;									// Added by Dario
//    chb_eeprom_read(CHB_EEPROM_SHORT_ADDR, addr, 2);
//    return *(U16 *)addr;
	return addr;											// Added by Dario
}

/**************************************************************************/
/*!
    Set the channel for the radio. 
*/
/**************************************************************************/
U8 chb_set_channel(U8 channel)
{
    U8 state;
    
    chb_reg_read_mod_write(PHY_CC_CCA, channel, 0x1f); 

    // add a delay to allow the PLL to lock if in active mode.
    state = chb_get_state();
    if ((state == RX_ON) || (state == PLL_ON))
    {
        chb_delay_us(TIME_PLL_LOCK);
    }

    return ((chb_reg_read(PHY_CC_CCA) & 0x1f) == channel) ? RADIO_SUCCESS : RADIO_TIMED_OUT;
}

/**************************************************************************/
/*!
    Returns the current channel for the radio.
*/
/**************************************************************************/
U8 chb_get_channel()
{
    return (chb_reg_read(PHY_CC_CCA) & 0x1f);
}

/**************************************************************************/
/*!
    Transmits a frame via the radio. It loads the data into the fifo,
    initiates a transmission attempt and returns the status of the transmission
    attempt.
*/
/**************************************************************************/
//U8 chb_tx(U8 *hdr, U8 *data, U8 len)
U8 chb_tx(oFrame *frame)
{
#if (CHB_RX_POLLING_MODE)
    if (rx_intp_flag == true)		// Added by Dario to avoid writing operation during
		return 0;					// data reading from SPI
#endif	

    U8 state = chb_get_state();
    pcb_t *pcb = chb_get_pcb();

	if ((state == BUSY_TX) || (state == BUSY_TX_ARET))
        return 0;					// Modified by Dario to manage the return properly

    // transition to the Transmit state
    if(chb_set_state(TX_ARET_ON) != RADIO_SUCCESS)
		return 0;
	//chb_set_state(TX_ARET_ON);


    // write frame to buffer. first write header into buffer (add 1 for len byte), then data. 
    //chb_frame_write(hdr, CHB_HDR_SZ + 1, data, len);
    chb_frame_write(frame);

    //Do frame transmission. 
    pcb->trx_end = false;
    chb_reg_read_mod_write(TRX_STATE, CMD_TX_START, 0x1F);

    // wait for the transmission to end, signalled by the TRX END flag
#if (CHB_RX_POLLING_MODE)
	// In case of polling, if an interrupt occur during the execution of
	// this subroutine, the ISR is placed in hold and the tx_end flag
	// will never be set. The timeout avoid deadlock.
	
	unsigned long start = millis();
	while ((!pcb->tx_end) && (millis() - start < CHB_WAIT));
    pcb->tx_end = false;
#else
	while (!pcb->trx_end);
	pcb->trx_end = false;
	
#endif	
 
    // check the status of the transmission
    return chb_get_status();
}

/**************************************************************************/
/*!
    Initialize the radio registers.
*/
/**************************************************************************/
static void chb_radio_init()
{
    U8 ieee_addr[8];

    // disable intps while we config the radio
    chb_reg_write(IRQ_MASK, 0);

    // force transceiver off while we configure the intps
    chb_reg_read_mod_write(TRX_STATE, CMD_FORCE_TRX_OFF, 0x1F);
    chb_delay_us(TIME_P_ON_TO_TRX_OFF);

    // set radio cfg parameters
    // **note** uncomment if these will be set to something other than default
    //chb_reg_read_mod_write(XAH_CTRL_0, CHB_MAX_FRAME_RETRIES << CHB_MAX_FRAME_RETRIES_POS, 0xF << CHB_MAX_FRAME_RETRIES_POS);
    //chb_reg_read_mod_write(XAH_CTRL_0, CHB_MAX_CSMA_RETRIES << CHB_MAX_CSMA_RETIRES_POS, 0x7 << CHB_MAX_CSMA_RETIRES_POS);
    //chb_reg_read_mod_write(CSMA_SEED_1, CHB_MIN_BE << CHB_MIN_BE_POS, 0x3 << CHB_MIN_BE_POS);     
    //chb_reg_read_mod_write(CSMA_SEED_1, CHB_CSMA_SEED1 << CHB_CSMA_SEED1_POS, 0x7 << CHB_CSMA_SEED1_POS);
    //chb_ret_write(CSMA_SEED0, CHB_CSMA_SEED0);     
    //chb_reg_read_mod_write(PHY_CC_CCA, CHB_CCA_MODE << CHB_CCA_MODE_POS,0x3 << CHB_CCA_MODE_POS);
    //chb_reg_write(CCA_THRES, CHB_CCA_ED_THRES);
    //chb_reg_read_mod_write(PHY_TX_PWR, CHB_TX_PWR, 0xf);

    // set the channel
    chb_set_channel(CHB_CHANNEL);

    // set autocrc mode
    chb_reg_read_mod_write(PHY_TX_PWR, 1 << CHB_AUTO_CRC_POS, 1 << CHB_AUTO_CRC_POS);

    // set transceiver's fsm state
    chb_set_state(RX_STATE);

    // set pan ID
    chb_reg_write16(PAN_ID_0, CHB_PAN_ID);

    // set short addr
    // NOTE: Possibly get this from EEPROM
    chb_reg_write16(SHORT_ADDR_0, chb_get_short_addr());

    // set long addr
    // NOTE: Possibly get this from EEPROM
    //chb_get_ieee_addr(ieee_addr);						// Modified by Dario
    //chb_reg_write64(IEEE_ADDR_0, ieee_addr);			// Modified by Dario
	chb_reg_write64(IEEE_ADDR_0, 0x00000000);			// Added by Dario
	
    // do a read of the interrupt register to clear the interrupt bits
    chb_reg_read(IRQ_STATUS);

    // re-enable intps while we config the radio
    chb_reg_write(IRQ_MASK, 0xc);

    // enable mcu intp pin on INT6 for rising edge
    CFG_CHB_INTP();
}

/**************************************************************************/
/*!
    Initialize the complete driver.
*/
/**************************************************************************/
void chb_drvr_init()
{
    // config SPI for at86rf230 access
    chb_spi_init();

    // configure IOs
    CHB_SLPTR_DDIR |= (_BV(CHB_SLPTR_PIN));

    // config radio
    chb_radio_init();
}

/**************************************************************************/
/*!
    Enable or disable the radio's sleep mode.
*/
/**************************************************************************/
void chb_sleep(U8 enb)
{
    if (enb)
    {
        // first we need to go to TRX OFF state
        chb_set_state(TRX_OFF);

        // then we need to make SLP_TR pin an input. this enables the external
        // pullup on that pin. we do this so that we won't need to go through
        // a voltage divider which drains current. we want to avoid that when
        // we sleep
        CHB_SLPTR_DDIR &= ~(_BV(CHB_SLPTR_PIN));
    }
    else
    {
        // make sure the SLPTR pin is low first
        CHB_SLPTR_PORT &= ~(_BV(CHB_SLPTR_PIN));

        // make the SLPTR pin an output
        CHB_SLPTR_DDIR |= _BV(CHB_SLPTR_PIN);

        // Turn the transceiver back on
        chb_set_state(RX_STATE);
    }
}
