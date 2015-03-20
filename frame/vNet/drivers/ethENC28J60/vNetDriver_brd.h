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
#define	vNet_Send_M3(addr, frame, len)		vNet_Send_M1(addr, frame, len)
#define	vNet_DataAvailable_M3()				vNet_DataAvailable_M1()
#define	vNet_RetrieveData_M3(data)			vNet_RetrieveData_M1(data)

/**************************************************************************/
/*!
	Set the vNet address and all the network parameters
*/
/**************************************************************************/
#if(!VNET_MEDIA1_ENABLE)
	void vNet_SetAddress_M3(uint16_t addr)
	{
		uint8_t ip_addr[4], mac_addr[6];
		
		// Translate and set the address
		eth_vNettoIP(0x00FF, &ip_addr[0]);
		eth_SetIPAddress(&ip_addr[0]);
		
		// Set the MAC Address	
#       if(AUTO_MAC)
                eth_vNettoMAC(addr, mac_addr);
                enc28j60Init(mac_addr);
#       else
                enc28j60Init((U8 *)&MAC_ADDRESS[0]);
#       endif 
		
		// Set the IP
		W5x00.setIPAddress(stack.ip);
		W5x00.setGatewayIp(stack.gateway);
		W5x00.setSubnetMask(stack.subnetmask);
		
		vNet_Begin_M1(UDP_SOCK);								// Start listen on socket

		// Include debug functionalities, if required
		#if(VNET_DEBUG)
		uint8_t addrval[6];
		
		// Print MAC address 
		W5x00.getMACAddress(addrval);
		VNET_LOG("(MAC)<");
		for(U8 i=0; i<6; i++)
		{
			VNET_LOG(addrval[i],HEX);
			VNET_LOG(",");
		}
		VNET_LOG(">\r\n");

		// Print IP address 
		W5x00.getIPAddress(addrval);
		VNET_LOG("(IP)<");
		for(U8 i=0; i<4; i++)
		{
			VNET_LOG(addrval[i],HEX);
			VNET_LOG(",");
		}
		
		VNET_LOG(">\r\n");
		#endif	
		
	}
#else
	void vNet_SetAddress_M3(uint16_t addr){return;}
#endif

// These is not applicable in a broadcast only scenario, so is there only for compatibility
// with upper layers.
uint16_t vNet_GetSourceAddress_M3(){return 0;}

#endif
