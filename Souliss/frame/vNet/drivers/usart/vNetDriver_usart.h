/**************************************************************************
	Souliss - vNet Virtualized Network
    Copyright (C) 2013  Veseo

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
#ifndef VNET_USART_H
#define VNET_USART_H

#include "Arduino.h"
#include "src/types.h"
#include "GetConfig.h"				// need : usartUsrCfg.h

#define USART_SUCCESS			0x01
#define USART_FAIL				0x00

#if(USART_BAUD115k2)
#	define USART_BAUD			115200			
#elif(USART_BAUD19k2)
#	define USART_BAUD			19200
#else
#	define USART_BAUD			9600
#endif

#define	USART_HEADERLEN			1
#define USART_CRCLEN			2
#define	USART_PREAMBLE			0xAC
#define	USART_PREAMBLE_LEN		6
#define	USART_POSTAMBLE			0xCA
#define	USART_POSTAMBLE_LEN		6
#define	USART_FRAME_LEN			(VNET_MAX_FRAME+USART_PREAMBLE_LEN+USART_POSTAMBLE_LEN+USART_HEADERLEN+USART_CRCLEN+3*USART_TOKEN_LENGHT)
#define	USART_MAXPAYLOAD		VNET_MAX_PAYLOAD
#define USART_TOKEN				0xAA
#define	USART_TOKEN_LENGHT		3
#define USART_BYTE_TIME			1					// Time in milliseconds required to transmit a byte at slower baud rate
#define USART_TOKEN_TIME		(USART_TOKEN_LENGHT*USART_BYTE_TIME)
#define	USART_BUSBUSY			0x05
#define	USART_BUSRECV			0x09
#define	USART_BUSFREE			0x00

void vNet_Init_M5();
void vNet_SetAddress_M5(uint16_t addr);
uint8_t vNet_Send_M5(uint16_t addr, oFrame *frame, uint8_t len);
uint8_t vNet_DataSize_M5();
uint8_t vNet_DataAvailable_M5();
uint8_t vNet_RetrieveData_M5(uint8_t *data);
uint16_t vNet_GetSourceAddress_M5();

#endif
