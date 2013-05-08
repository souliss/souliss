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
#ifndef VNET_BFSKPLINIO_H
#define VNET_BFSKPLINIO_H

#include "Arduino.h"
#include "src/types.h"
#include "src/plinio.h"

#define	BFSK_PREAMBLE			0xAC
#define	BFSK_PREAMBLE_LEN		6
#define	BFSK_POSTAMBLE			0xCA
#define	BFSK_POSTAMBLE_LEN		6

#define	BFSK_HEADER_LEN			6
#define	BFSK_FRAME_LEN			(VNET_MAX_PAYLOAD+BFSK_HEADER_LEN+BFSK_PREAMBLE_LEN+BFSK_POSTAMBLE_LEN)

#define BFSK_VNET_PREAMBLE		0
#define BFSK_VNET_PAYLOAD		7

#define	BFSK_VNET_FAIL			0
#define	BFSK_VNET_SUCCESS		1

#define	BFSK_QUALITYMAX			0xFF
#define	BFSK_QUALITYMIN			0x10

typedef struct
{
	U8  *data;					// Pointer	
	U16 datalen;				// Data lenght
	U8	qualityindex;			// Quality index
} Buffer;

void vNet_Init_M4();
void vNet_SetAddress_M4(uint16_t addr);
uint8_t vNet_Send_M4(uint16_t addr, uint8_t *data, uint8_t len);
uint8_t vNet_DataSize_M4();
uint8_t vNet_DataAvailable_M4();
uint8_t vNet_RetrieveData_M4(uint8_t *data);
uint16_t vNet_GetSourceAddress_M4();

#endif
