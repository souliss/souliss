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
	
	Developed by Dario Di Maio, based on HardwareSerial by Nicholas Zambetti
	
***************************************************************************/
/*!
    \file 
    \ingroup

*/
/**************************************************************************/
#ifndef VNET_USART232_H
#define VNET_USART232_H

#include "Arduino.h"
#include "src/types.h"

#define	USART_BAUD				9600
#define	USART_SUCCESS			1
#define	USART_FAIL				0

#define USART_PREAMBLE			0xAC
#define	USART_PREAMBLE_LEN		3

#define	USART_HEADER_LEN		4
#define	USART_FRAME_LEN			(USART_HEADER_LEN+VNET_MAX_PAYLOAD)

#define USART_AREDATAIN			((U8)(VNET_MAX_PAYLOAD + buffer.w_pnt - buffer.r_pnt) % VNET_MAX_PAYLOAD)
#define USART_ISPREAMBLE		((buffer.data[buffer.r_pnt] == USART_PREAMBLE) && (buffer.data[buffer.r_pnt+1] == USART_PREAMBLE) && (buffer.data[buffer.r_pnt+2] == USART_PREAMBLE))

#define	USART_CALLBACK			

typedef struct
{
	U8  *data;					// Pointer	
	U8	w_pnt;					// Write Pointer
	U8	r_pnt;					// Read Pointer
} Buffer;

void usart_send(U8 data);
uint8_t usart_datain();
void usart_retrievebyte(U8 *data);
void vNet_Init_M6();
void vNet_SetAddress_M6(uint16_t addr);
uint8_t vNet_Send_M6(uint16_t addr, uint8_t *data, uint8_t len);
uint8_t vNet_DataAvailable_M6();
uint8_t vNet_RetrieveData_M6(uint8_t *data);
uint16_t vNet_GetSourceAddress_M6();

#endif
