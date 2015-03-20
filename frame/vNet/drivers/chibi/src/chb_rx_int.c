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
/*!
    \file 
    \ingroup
*/
/**************************************************************************/
#include "Arduino.h"
#include "chb.h"
#include "chb_drvr.h"
#include "chb_spi.h"

/**************************************************************************/
/*!
    This the main ISR for the Chibi radio stack. It doesn't use a standard
    IRQ pin. Instead it uses a general purpose IO configured as an edge-detecting
    interrupt pin. That means that this interrupt needs to qualify the pin
    as a positive rising edge before executing the actual service routine.
*/
/**************************************************************************/
ISR(CHB_RADIO_IRQ)
{
    U8 dummy, state, pinval, intp_src = 0;
    pcb_t *pcb = chb_get_pcb();

    CHB_ENTER_CRIT();

    // get the pin's value to check whether it was a rising or falling edge.
    pinval = PINB & _BV(PINB6);

    // we'll only enter the ISR if the interrupt is a positive edge.  
    if (pinval)
    {
        /*Read Interrupt source.*/
        CHB_SPI_ENABLE();

        /*Send Register address and read register content.*/
        dummy = chb_xfer_byte(IRQ_STATUS | CHB_SPI_CMD_RR);
        intp_src = chb_xfer_byte(0);

        CHB_SPI_DISABLE();

        while (intp_src)
        {
            /*Handle the incomming interrupt. Prioritized.*/
            if ((intp_src & CHB_IRQ_RX_START_MASK))
            {
                intp_src &= ~CHB_IRQ_RX_START_MASK;
            }
            else if (intp_src & CHB_IRQ_TRX_END_MASK)
            {
                state = chb_get_state();

                if ((state == RX_ON) || (state == RX_AACK_ON) || (state == BUSY_RX_AACK))
                {
                    // get the ed measurement
                    pcb->ed = chb_reg_read(PHY_ED_LEVEL);

                    // get the crc
                    pcb->crc = (chb_reg_read(PHY_RSSI) & (1<<7)) ? 1 : 0;

                    // if the crc is not valid, then do not read the frame and set the rx flag
                    if (pcb->crc)
                    {
                        // get the data
                        chb_frame_read();
                        pcb->rcvd_xfers++;
                        pcb->data_rcv = true;
                    }
                }
				pcb->trx_end = true; 
                intp_src &= ~CHB_IRQ_TRX_END_MASK;
				
                //while (chb_set_state(RX_STATE) != RADIO_SUCCESS);
				while (chb_get_state() != RX_STATE)
                {
                    chb_set_state(RX_STATE);
                }
            }
            else if (intp_src & CHB_IRQ_TRX_UR_MASK)
            {
                intp_src &= ~CHB_IRQ_TRX_UR_MASK;
                pcb->underrun++;
            }
            else if (intp_src & CHB_IRQ_PLL_UNLOCK_MASK)
            {
                intp_src &= ~CHB_IRQ_PLL_UNLOCK_MASK;
            }
            else if (intp_src & CHB_IRQ_PLL_LOCK_MASK)
            {
                intp_src &= ~CHB_IRQ_PLL_LOCK_MASK;
            }
            else if (intp_src & CHB_IRQ_BAT_LOW_MASK)
            {
                intp_src &= ~CHB_IRQ_BAT_LOW_MASK;
                pcb->battlow++;
            }
            else
            {
            }
        }
    }

    CHB_LEAVE_CRIT();
}
