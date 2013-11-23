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
	
	Originally developed by Dario Di Maio
	
***************************************************************************/
/*!
    \file 
    \ingroup

*/
/**************************************************************************/
#ifndef VNET_ETHUDP_H
#define VNET_ETHUDP_H

#include "Arduino.h"
#include "GetConfig.h"				// need : ethUsrCfg.h

#include "src/w5x00.h"
#include "src/socket.h"
#include "frame/vNet/tools/UserMode.h"

#include "frame/vNet/drivers/ethW5x00/ServerClient.h"


typedef struct
{
	U8 base_ip[4];			// Base IP address for address translation
	U8 ip[4];				// IP address
	U8 subnetmask[4];		// Subnetmask
	U8 gateway[4];			// Gateway address
} TCPIP;

typedef struct
{
	U8  ip[4];				// Source IP address
	U8  len;				// Data lenght
	U16 port;				// IP Port
} inframe;

void vNet_Init_M1();
void vNet_Begin_M1(uint8_t sock);
void vNet_Stop_M1(uint8_t sock);
void vNet_SetAddress_M1(uint16_t addr);
uint8_t vNet_Send_M1(uint16_t addr, oFrame* frame, uint8_t len);
uint8_t vNet_DataSize_M1();
uint8_t vNet_DataAvailable_M1();
uint8_t vNet_RetrieveData_M1(uint8_t *data);
uint16_t vNet_GetSourceAddress_M1();
void eth_vNettoMAC(const uint16_t addr, uint8_t *mac_addr);
void eth_vNettoIP(const uint16_t addr, uint8_t *ip_addr);
void eth_IPtovNet(uint16_t *addr, const uint8_t *ip_addr);
void eth_SetBaseIP(uint8_t *ip_addr);
void eth_SetIPAddress(uint8_t *ip_addr);
void eth_SetSubnetMask(uint8_t *submask);
void eth_SetGateway(uint8_t *gateway);
void eth_GetIP(uint8_t *ip_addr);
void eth_GetBaseIP(uint8_t *ip_addr);
void eth_GetSubnetMask(uint8_t *submask);
void eth_GetGateway(uint8_t *gateway);


#endif
