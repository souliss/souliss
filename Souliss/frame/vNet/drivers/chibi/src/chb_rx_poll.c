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
    Copyright (C) 2011  Veseo

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
	
	Modified by Dario Di Maio
	
***************************************************************************/

/*!
    \file 
    \ingroup


*/
/**************************************************************************/
#include "Arduino.h"
#include "chb.h"
#include "chb_drvr.h"
#include "chb_spi.h"

static U8 intp_src = 0;
bool rx_intp_flag = false;

/**************************************************************************/
/*!
    This is the main IRQ handler for chibi. It's not in an actual ISR and is
    only called after an interrupt occurs.
*/
/**************************************************************************/
static void chb_irq_handler()
{
    U8 dummy, state, pinval;
    pcb_t *pcb = chb_get_pcb();

    // find out who issued the interrupt
    intp_src |= chb_reg_read(IRQ_STATUS);

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
                    if(chb_frame_read())
					{				
						pcb->rcvd_xfers++;
						pcb->data_rcv = true;
					}
					else
						pcb->data_rcv = false;
				}
            }
            intp_src &= ~CHB_IRQ_TRX_END_MASK;
            while (chb_set_state(RX_STATE) != RADIO_SUCCESS);
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
			intp_src = 0;	// Added by Dario to avoid deadlock
        }
    }
	
    rx_intp_flag = false;
    CHB_IRQ_ENABLE();
	

}

/**************************************************************************/
/*!
    This is the function that checks to see if any interrupt occurred. If it
    did, then it will execute the handler and return a flag indicating
    new data is available.
*/
/**************************************************************************/
void chb_rcv_poll()
{
    // if an interrupt occured, handle it
    if (rx_intp_flag)
        chb_irq_handler();

}


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
    pcb_t *pcb = chb_get_pcb();
    U8 state, pinval;

    CHB_ENTER_CRIT();

    // check if the pin is high or low which indicates rising or falling edge
    // interrupt
    pinval = PINB & _BV(PINB6);

    // if it was a rising edge, then flag it as a real interrupt
    if (pinval)
    {		
		// reset the interrupt array status
		intp_src = 0;
	
        // we can only access the SPI if we're transmitting. That's because
        // we're in full control of the SPI bus. Otherwise, the ethernet shield
        // has a bug that blocks other SPI devices when its using it. 
        if (pcb->tx_busy)
        {
            // get the interrupt source
            intp_src |= chb_reg_read(IRQ_STATUS);
			
            //if (intp_src && CHB_IRQ_TRX_END_MASK)			
            if (intp_src & CHB_IRQ_TRX_END_MASK)
            {
				// if these states are detected, the transmission was failed
				// added by Dario
				state = chb_get_state();							
				if ((state == RX_ON) || (state == RX_AACK_ON) || (state == BUSY_RX_AACK))
				{
					// activate the irq_handler
					rx_intp_flag = true;
					CHB_IRQ_DISABLE(); 
					
					goto leave_crit;     
				}			
                
				// if we're transmitting, then signal the end of the transmission
                pcb->tx_end = true;
                pcb->tx_busy = false;
				
				// reset the interrupt
                intp_src &= ~CHB_IRQ_TRX_END_MASK;			
                while (chb_set_state(RX_STATE) != RADIO_SUCCESS);			

				// if other interrupts are still active, let the irq_handler start
				// added by Dario to perform full interrupt management
				if(intp_src)
				{
					rx_intp_flag = true;
					CHB_IRQ_DISABLE(); 
				}			
                
				// we're finished here
				goto leave_crit;
            }
        }

        // if its a receive interrupt, then disable the IRQ on the MCU side.
        // we'll flag the interrupt and process it later outside of the isr
        rx_intp_flag = true;
        CHB_IRQ_DISABLE(); 
    }

	leave_crit:			
    CHB_LEAVE_CRIT();
}
