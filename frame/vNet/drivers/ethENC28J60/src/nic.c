/**************************************************************************
	Souliss - vNet Virtualized Network
    Copyright (C) 2012  Veseo

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
	
	Developed by Dario Di Maio
	
***************************************************************************/
/*!
    \file 
    \ingroup

*/
/**************************************************************************/

/*****************************************************************************
*  Module Name:       NIC Driver Interface for uIP-AVR Port
*  
*  Created By:        Louis Beaudoin (www.embedded-creations.com)
*
*  Original Release:  November 16, 2003 
*
*
*****************************************************************************/

#include "nic.h"

#define IP_TCP_HEADER_LENGTH 40
#define TOTAL_HEADER_LENGTH (IP_TCP_HEADER_LENGTH+ETHERNET_HEADER_LENGTH)


void nic_send(void)
{
	if(uip_len<=UIP_BUFSIZE)
	{	
		// Transfer the message to the Ethernet controller
		enc28j60PacketSend(uip_len, (U8 *)uip_buf);
	}
}

u8_t nic_poll(void)
{
	u16_t packetLength;

	packetLength = enc28j60PacketReceive(UIP_BUFSIZE, (uint8_t*)uip_buf);	

	// If there are no incoming data
	if(!packetLength)
	  return packetLength = 0;

	// If the lenght exceed the buffer size
	if(packetLength > UIP_BUFSIZE)
		return packetLength = 0; 
	
	// Return the lenght		
	return packetLength;	
}
