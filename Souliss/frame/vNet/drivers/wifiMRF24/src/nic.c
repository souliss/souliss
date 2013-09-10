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

#include "nic.h"

#if (VNET_DEBUG)
	#define VNET_LOG Serial.print
#endif

void nic_send(void)
{
	if(uip_len < UIP_PAYLOADSIZE)
	{	
		// Transfer the message to the Wifi controller
		zg_set_buf((U8 *)uip_buf, uip_len);
		zg_set_tx_status(1);
	}
	
	// Process the in/out data from the Wifi controller
	zg_drv_process();	
	
}

void nic_connect(void)
{
/*	This part of code is required only if using the interrupt while accessing the SPI
	attachInterrupt(0, zg_isr, LOW);
*/	
	zg_setpolling();

	// Include debug functionalities, if required
	#if(VNET_DEBUG)
	// Print address  
    VNET_LOG("(vNet)WiFi Connecting");
	VNET_LOG("\r\n");
	#endif
	
	// Connect to a Wifi network or in AdHoc mode	
	while(zg_get_conn_state() != 1) {
		zg_drv_process();
	}	
	
	// Include debug functionalities, if required
	#if(VNET_DEBUG)
	// Print address  
    VNET_LOG("(vNet)WiFi Connected");
	VNET_LOG("\r\n");
	#endif	
}

u8_t nic_poll(void)
{
	u16_t packetLength;
	
	// Connection lost, retry	
	if(zg_get_conn_state() != 1) {
	
		// Include debug functionalities, if required
		#if(VNET_DEBUG)
		// Print address  
		VNET_LOG("(vNet)WiFi Connection lost, retry");
		VNET_LOG("\r\n");
		#endif	
	
		while(zg_get_conn_state() != 1){
			zg_drv_process();
		}
	}
	
	// This method place the incoming data directly into the uIP buffer
	packetLength = zg_get_rx_status();	

	// Process the in/out data from the Wifi controller
	zg_drv_process();
	
	// If there are no incoming data
	if(!packetLength)
	  return packetLength = 0;

	// If the lenght exceed the buffer size
	if(packetLength > UIP_BUFSIZE)
		return packetLength = 0; 
	
	// Return the lenght		
	return packetLength;	
}
