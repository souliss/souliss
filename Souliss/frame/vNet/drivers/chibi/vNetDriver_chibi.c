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
#include "vNetDriver_chibi.h"

// Including the actual "c" files rather than the headers. The Arduino lib only
// recognizes one source file in the directory so all the source files need to look like 
// they're in the one file.
#include "src/chb.c"
#include "src/chb_buf.c"
#include "src/chb_drvr.c"
#include "src/chb_spi.c"

#if (CHB_RX_POLLING_MODE)
   #include "src/chb_rx_poll.c"
#else
   #include "src/chb_rx_int.c"
#endif

// used to store info about received data
static chb_rx_data_t rx_data;

/**************************************************************************/
/*!
    Init the chibi stack
*/
/**************************************************************************/
void vNet_Init_M2()
{
    chb_init();
}

/**************************************************************************/
/*!
    Set the short address of the wireless node. This is the 16-bit "nickname" 
    of your node and what will be used to identify it. 
*/
/**************************************************************************/
void vNet_SetAddress_M2(uint16_t addr)
{
    chb_set_short_addr(addr);
}

/**************************************************************************/
/*!
    Transmit data to another node wirelessly using Chibi stack.
    Usage: send <addr> <char data array...>
*/
/**************************************************************************/
uint8_t vNet_Send_M2(uint16_t addr, oFrame *frame, uint8_t len)
{
    if(chb_write(addr, frame, len) == CHB_SUCCESS)
		return CHB_SUCC;
	else
	{
		oFrame_Reset();		// Free the frame
	
		return CHB_FAIL;
	}	
}	

/**************************************************************************/
/*!
    This function should be polled in the "loop" portion of the code. It will
    return false if no data has been received or true if data has arrived.
*/
/**************************************************************************/
uint8_t vNet_DataAvailable_M2()
{
    pcb_t *pcb = chb_get_pcb();

#if (CHB_RX_POLLING_MODE)
    // need to poll for received data if we're not retrieving the data 
    // in the ISR
    chb_rcv_poll();
#endif

    return pcb->data_rcv;
}

/**************************************************************************/
/*!
    Retrieves the data from the frame. A char array needs to be passed into
    the function and the data will be copied into it. The character array
    should be at least the size of a maximum sized frame for safety. It can
    be smaller to save RAM but this should be done carefully to avoid buffer
    overflows.
*/
/**************************************************************************/
uint8_t vNet_RetrieveData_M2(uint8_t *data)
{
    // point the data buffer to the buffer that was passed in from arduino ide
    rx_data.data = (U8 *)data;

    // load the data into the buffer and get the length
    rx_data.len = chb_read(&rx_data);
    return rx_data.len;
}

/**************************************************************************/
/*!
    Get the source address of the most recently received frame
*/
/**************************************************************************/
uint16_t vNet_GetSourceAddress_M2()
{
    return rx_data.src_addr;
}

/**************************************************************************/
/*!
    Returns the signal strength recorded from the last received frame.
*/
/**************************************************************************/
uint8_t vNet_GetRSSI_M2()
{
    pcb_t *pcb = chb_get_pcb();
    return pcb->ed;
}

/**************************************************************************/
/*!
    Puts the radio to sleep to save power. It cuts approximately 15 mA of
    current from the system.
*/
/**************************************************************************/
void vNet_Sleep_M2(uint8_t enb)
{
    chb_sleep(enb);
}