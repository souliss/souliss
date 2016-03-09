/**************************************************************************
	Souliss - vNet Virtualized Network
    Copyright (C) 2014  Veseo

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
	
	Originally developed by Dario Di Maio
	
***************************************************************************/
/*!
    \file 
    \ingroup

*/
/**************************************************************************/
#ifndef VNET_ETHBRD_H
#define VNET_ETHBRD_H

#include "Arduino.h"
#include "GetConfig.h"						// need : ethUsrCfg.h

#include "vNetDriver_eth.h"

#define	vNet_Init_M3()						vNet_Init_M1()
#define vNet_Begin_M3						vNet_Begin_M1
#define	vNet_DataAvailable_M3()				vNet_DataAvailable_M1()
#define	vNet_RetrieveData_M3(data)			vNet_RetrieveData_M1(data)

#define	VNET_M3_HEADER		1
#define	VNET_M3_APPEND		2

uint16_t vNetM3_address=0;							// Node address for the media
uint16_t vNetM3_srcaddr=0;							// Node address from incoming frame
uint8_t  vNetM3_isdata =0;							// Flag if Media3 has incoming data
oFrame vNetM3_oFrame;								// Data structure for output frame

extern oFrame vNetM1_oFrame;	
extern uint8_t vNetM1_header;
extern TCPIP stack;

#if (VNET_DEBUG)
	#define VNET_LOG LOG.print	
#endif

/**************************************************************************/
/*!
	Set the vNet address and all the network parameters
*/
/**************************************************************************/
#if(!VNET_MEDIA1_ENABLE)
	void vNet_SetAddress_M3(uint16_t addr)
	{
		uint8_t ip_addr[4], mac_addr[6];

		// Locally store the address
		vNetM3_address=addr;
		oFrame_Define(&vNetM3_oFrame);
		oFrame_Set((uint8_t*)(&vNetM3_address), 0, 1, 0, 0);
		
		// The Wiznet doesn't have embed DHCP support and the scope of this M3
		// is to remove dependency on IP configuration, so we just use a NULL
		// address
		ip_addr[0]=ip_addr[1]=ip_addr[2]=ip_addr[3]=0;
		eth_SetIPAddress(&ip_addr[0]);
		
		// Set the MAC Address	
#       if(AUTO_MAC)
			eth_vNettoMAC(addr, mac_addr);
			enc28j60Init(mac_addr);
#       else
			enc28j60Init((U8 *)&MAC_ADDRESS[0]);
#       endif 

		// Include debug functionalities, if required
		#if(VNET_DEBUG)

		// Print IP address 
		VNET_LOG("(IP)<");
		for(U8 i=0; i<4; i++)
		{
			VNET_LOG(ip_addr[i],HEX);
			VNET_LOG(",");
		}
		
		VNET_LOG(">\r\n");
		#endif	
		
		vNet_Begin_M3(UDP_SOCK);								// Start listen on socket
		
	}
#else
	void vNet_SetAddress_M3(uint16_t addr)
	{
		// Locally store the address
		vNetM3_address=addr;	
		oFrame_Define(&vNetM3_oFrame);
		oFrame_Set((uint8_t*)(&vNetM3_address), 0, 1, 0, 0);
	}
#endif

/**************************************************************************/
/*!
    Get the source address of the most recently received frame
*/
/**************************************************************************/
uint16_t vNet_GetSourceAddress_M3(){return vNetM3_srcaddr;}

/**************************************************************************/
/*!
    The upper layer needs to identify if data are on M1 or M3, and this
	flags are used for that scope
*/
/**************************************************************************/
void  vNet_setIncomingData_M3() {vNetM3_isdata = 1;}
uint8_t  vNet_hasIncomingData_M3() 
{
	if(vNetM3_isdata)
	{
		vNetM3_isdata = 0; 
		return 1;
	}
	return 0;
}	

/**************************************************************************/
/*!
	Send a message via TCP/IP
*/
/**************************************************************************/
uint8_t vNet_Send_M3(uint16_t addr, oFrame *frame, uint8_t len)
{
	uint8_t s, ip_addr[4];
	uint16_t count = 0, vNet_port;

	// Check message length
	if ((len == 0) || (len >= UIP_PAYLOADSIZE))
		return ETH_FAIL;

	// Define the standard vNet port
	vNet_port = ETH_PORT;

	// Set the IP broadcast address
	for(U8 i=0;i<4;i++)
		ip_addr[i]=0xFF;
	
	/***
		Add the whole length as first byte and the node address
		at the end of the frame
	***/
	
	// Add the node address
	oFrame_Define(&vNetM3_oFrame);
	oFrame_Set((uint8_t*)(&vNetM3_address), 0, VNET_M3_APPEND, 0, 0);	
	
	// If the frame is not empty, there are waiting data 	
	oFrame_Define(&vNetM1_oFrame);
	if(oFrame_isBusy())
		return ETH_FAIL;		

	// Add the length as first byte
	vNetM1_header = len+VNET_M3_HEADER+VNET_M3_APPEND;
	oFrame_Define(&vNetM1_oFrame);
	oFrame_Set(&vNetM1_header, 0, 1, 0, frame);

	// Append the address as last, this is contained into a dedicated oFrame
	oFrame_AppendLast(&vNetM3_oFrame);
	
	// Setup the connection, data will be sent using a callback function
	if(!uip_udp_sock((u16_t*)ip_addr, vNet_port, (u16_t)ETH_PORT))
	{		
		// Flag the error
		oFrame_Reset();
		
		return ETH_FAIL;
	}
				
	// Data are processed with the IP stack		
	vNet_uIP();	
	
	// At this stage data are processed or socket is failed, so we can
	// securely reset the oFrame
	oFrame_Reset();	
	
	return ETH_SUCCESS;
}


#endif
