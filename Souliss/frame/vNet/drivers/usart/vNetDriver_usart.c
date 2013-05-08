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
#include <stdio.h>
#include <string.h>

#include "vNetDriver_usart.h"

uint8_t buff[VNET_MAX_PAYLOAD];						// Buffer
Buffer buffer;										// Structure for the outgoing data

// If data are available
SIGNAL(USART_RX_vect)
{
	U8 i = (U8)(buffer.w_pnt + 1) % VNET_MAX_PAYLOAD;

	// If there is room in the buffer, store data from the USART
	if (i != buffer.r_pnt) 
	{
		buffer.data[buffer.w_pnt] = UDR0;
		buffer.w_pnt = i;
	}
}

/**************************************************************************/
/*!
    Send a byte over the USART
*/
/**************************************************************************/
void usart_send(U8 data)
{
	while (!(UCSR0A) & (1 << UDRE0));
	UDR0 = data;		
}

/**************************************************************************/
/*!
	Returns the number of bytes into the buffer
*/
/**************************************************************************/
uint8_t usart_datain()
{
	return USART_AREDATAIN;
}

/**************************************************************************/
/*!
	Retrieve one byte from the buffer
*/
/**************************************************************************/
void usart_retrievebyte(U8 *data)
{
	*data = buffer.data[buffer.r_pnt];
	buffer.r_pnt = (U8)(buffer.r_pnt + 1) % VNET_MAX_PAYLOAD;
}
		
/**************************************************************************/
/*!
    Init the uIP stack
*/
/**************************************************************************/
void vNet_Init_M6()
{	
	// Init the buffer
	buffer.data=buff;								
	buffer.w_pnt=0;								
	buffer.r_pnt=0;								
	
	// Init the USART
	UCSR0A = 1 << true;
	UBRR0H = ((F_CPU / 4 / 9600 - 1) / 2) >> 8;
	UBRR0L = ((F_CPU / 4 / 9600 - 1) / 2);

	(_SFR_BYTE(UCSR0B) &= ~_BV(RXEN0));
	(_SFR_BYTE(UCSR0B) &= ~_BV(TXEN0));
	(_SFR_BYTE(UCSR0B) &= ~_BV(RXCIE0));
}

/**************************************************************************/
/*!
	The USART work only in broadcast, the SetAddress method is there only
	for standardization with other drivers
*/
/**************************************************************************/
void vNet_SetAddress_M6(uint16_t addr)
{
}

/**************************************************************************/
/*!
	Send a messagge over the USART, the address is not used at this stage
	of the communication.
*/
/**************************************************************************/
uint8_t vNet_Send_M6(uint16_t addr, uint8_t *data, uint8_t len)
{
	// Check message lenght
	if ((len == 0) || (len >= VNET_MAX_PAYLOAD))
		return ETH_FAIL;
			
	// Broadcast is not supported
	if(addr == 0xFFFF)
		return ETH_FAIL;

	// Send data
	for(U8 i=0;i<len+USART_PREAMBLE+1;i++)
	{
		// Insert the preamble at the begin of the frame, then the lenght and the payload
		if(i<USART_PREAMBLE_LEN)
			usart_send(USART_PREAMBLE);
		else if (i == USART_PREAMBLE_LEN)	
			usart_send(len+1);
		else
			usart_send(data[i]);	
	}	
}

/**************************************************************************/
/*!
	Verify data in the temporary buffer to find out vNet frames
*/
/**************************************************************************/
uint8_t vNet_DataAvailable_M6()
{
	// If the incoming data buffer isn't empty
	uint8_t dlen = USART_AREDATAIN;
	
	// If the incoming data are less than the header, return
	if(dlen < USART_HEADER_LEN)
		return USART_FAIL;
	else
	{
		// If the preable is available, is a vNet data
		if(USART_ISPREAMBLE)
		{
			/* Data are loaded one byte per time, and the frame maybe not completely
			loaded into the buffer, before return that data are available, a check
			for data consistency is required
			*/
			
			// The first byte after the preamble is the frame lenght
			if(buffer.data[buffer.r_pnt+3] <= dlen)
			{
				// Remove the preamble
				buffer.r_pnt = (U8)(buffer.r_pnt + 3) % VNET_MAX_PAYLOAD;
				
				// Return the lenght of the incoming frame
				return buffer.data[buffer.r_pnt];
			}	
			else
			{
				// The data are corrupted, go to the next preamble (if any)
				U8 i = 0;
				while(USART_AREDATAIN && !USART_ISPREAMBLE)
				{
					buffer.r_pnt = (U8)(buffer.r_pnt + 1) % VNET_MAX_PAYLOAD;	
					i++;
				}	
			}
		}
		else	// Incoming data are not vNet frame
		{
			// Use a callback to notify the incoming data to an external listner
			USART_CALLBACK;
		}	
	}
}

/**************************************************************************/
/*!
	Retrieve the complete vNet frame from the incoming buffer
*/
/**************************************************************************/
uint8_t vNet_RetrieveData_M6(uint8_t *data)
{
	// The first byte is the frame lengt
	U8 len = buffer.data[buffer.r_pnt];
	buffer.r_pnt = (U8)(buffer.r_pnt + 1) % VNET_MAX_PAYLOAD;
	
	// There is an error
	if(!len)
	{
		// Init the driver
		vNet_Init_M6();
		return 	USART_FAIL;	
	}
	
	// Retrieve data
	for(U8 i=0;i<len;i++)
	{
		data[i] = buffer.data[buffer.r_pnt];
		buffer.r_pnt = (U8)(buffer.r_pnt + 1) % VNET_MAX_PAYLOAD;
	}
	
	return	USART_SUCCESS;
}

/**************************************************************************/
/*!
    Get the source address of the most recently received frame
*/
/**************************************************************************/
uint16_t vNet_GetSourceAddress_M6()
{
	return 0;
}

