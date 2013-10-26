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
	
	Originally developed by Dario Di Maio
	
***************************************************************************/
/*!
    \file 
    \ingroup

*/
/**************************************************************************/
#ifndef VNET_ENC28J60_H
#define VNET_ENC28J60_H

#include "Arduino.h"
#include "GetConfig.h"				// need : ethUsrCfg.h
#include "src/types.h"

#include "frame/vNet/stack/uIP/uip.h"
#include "frame/vNet/stack/uIP/uip_arch.h"
#include "frame/vNet/stack/uIP/uip_arp.h"

#include "frame/vNet/drivers/ethENC28J60/vNetCallBack.h"
#include "frame/vNet/drivers/ethENC28J60/src/ENC28J60.h"
#include "frame/vNet/drivers/ethENC28J60/src/nic.h"
#include "frame/vNet/drivers/ethENC28J60/ServerClient.h"
#include "frame/vNet/tools/UserMode.h"

// Application state option
#define	NONE			0x00
#define	RESET			0x00
#define	SOCKETSERVER	0x10
#define	SOCKETCLIENT	0x20
#define CLOSECLIENT		0x01
#define	SENDDATA		0x02
#define	SENDANDCLOSE	0x03

#define	uIP_DELAY	5

typedef struct
{
	U8 base_ip[4];			// Base IP address for address translation
	U8 ip[4];				// IP address
	U8 subnetmask[4];		// Subnetmask
	U8 gateway[4];			// Gateway address
	
} TCPIP;

typedef struct
{
	U8 *data;				// Pointer to the data output buffer
	U8 datalen;				// Data lenght	
} OutBuffer;

void vNet_Init_M1();
void vNet_SetAddress_M1(uint16_t addr);
uint8_t vNet_Send_M1(uint16_t addr, uint8_t *data, uint8_t len);
uint8_t vNet_DataSize_M1();
uint8_t vNet_DataAvailable_M1();
uint8_t vNet_RetrieveByte_M1(uint8_t *data);
uint8_t vNet_RetrieveData_M1(uint8_t *data);
uint8_t vNet_BufferedData_M1();
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
