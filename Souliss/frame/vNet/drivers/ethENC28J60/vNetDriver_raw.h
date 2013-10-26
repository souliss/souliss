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
#ifndef VNET_RAWENC28J60_H
#define VNET_RAWENC28J60_H

#include "Arduino.h"
#include "src/types.h"

#include "GetConfig.h"				// need : ethUsrCfg.h
#include "frame/vNet/drivers/ethENC28J60/src/ENC28J60.h"

#define	ETH_PREAMBLE			0xAC
#define	ETH_PREAMBLE_LEN		6
#define	ETH_POSTAMBLE			0xCA
#define	ETH_POSTAMBLE_LEN		6
#define	ETH_MAX_RETRIES			4

#define	ETH_HEADER_LEN			14
#define	ETH_FRAME_LEN			(VNET_MAX_FRAME+ETH_HEADER_LEN+ETH_PREAMBLE_LEN+ETH_POSTAMBLE_LEN)

#define ETH_MAC_DADDR			0
#define ETH_MAC_SADDR			6
#define ETH_MAC_FLEN			12
#define ETH_VNET_PREAMBLE		14
#define ETH_VNET_PAYLOAD		20

typedef struct
{
	U8  *data;					// Pointer	
	U16 datalen;				// Data lenght	
} Buffer;

void vNet_Init_M3();
void vNet_SetAddress_M3(uint16_t addr);
uint8_t vNet_Send_M3(uint16_t addr, oFrame *frame, uint8_t len);
uint8_t vNet_DataSize_M3();
uint8_t vNet_DataAvailable_M3();
uint8_t vNet_RetrieveData_M3(uint8_t *data);
uint16_t vNet_GetSourceAddress_M3();
void eth_vNettoMAC(const uint16_t addr, uint8_t *mac_addr);

#endif
