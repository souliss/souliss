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
#include <stdio.h>
#include <string.h>

#include "vNetDriver_usart.h"							

#define	PRESTANOME		0

#define	BusIsBusy()		busstate=USART_BUSBUSY
#define	BusFreeSlot()	if(busstate) busstate--
#define	isBusFree()		busstate==USART_BUSFREE
#define	waitBusFree()	delay((myaddress-VNET_ADDR_L_M5)*USART_TOKEN_TIME)

uint8_t usartframe[USART_FRAME_LEN], l=0;	
uint8_t	busstate=USART_BUSBUSY;
uint16_t myaddress=0, in_crc=0;

/**************************************************************************/
/*!
    Init the uIP stack
*/
/**************************************************************************/
void vNet_Init_M5()
{	
	// Start the USART
	Serial.begin(USART_BAUD);
	
	// Set the write mode pin of the RS485
	#if(USART_TXENABLE)
	pinMode(USART_TXENPIN, OUTPUT);
	#endif
}

/**************************************************************************/
/*!
	Set the vNet address 
	
	The USART works in broadcast and has no address check, this method is
	here only for compatiblity with upper vNet layers (that has their own
	addressing).
	
	The address is saved in a local register and is used in the collision
	avoidance
*/
/**************************************************************************/
void vNet_SetAddress_M5(uint16_t addr)
{
	myaddress = addr;
}

/**************************************************************************/
/*!
	Send a messagge via USART
*/
/**************************************************************************/
uint8_t vNet_Send_M5(uint16_t addr, oFrame *frame, uint8_t len)
{
	uint8_t i, j, data;
	uint16_t crc;
	
	// Check message lenght
	if ((len == 0) || (len >= USART_MAXPAYLOAD))
		return USART_FAIL;

	// Set the write mode pin of the RS485
	#if(USART_TXENABLE)
	digitalWrite(USART_TXENPIN, HIGH);
	#endif
	
	// Check if the bus is free
	if(!isBusFree())
	{
		// The bus was used recently, if not yet in use try to get it
		if(!Serial.available())
		{	
			// Send a token to notify that we are willing to use the bus
			for(i=0; i<USART_TOKEN_LENGHT; i++)
				Serial.write(USART_TOKEN);	
			
			// Wait for a given number of token times before proceed,
			// nodes with lower address has major priority.
			// This is a collision avoidance with fixed priority, in
			// a busy network, nodes with lower priority may never 
			// found a slot
			delay((myaddress-VNET_ADDR_L_M5)*USART_TOKEN_TIME);
			
			// Check if someone started while waiting for the slot
			i=0;
			while(Serial.available()) i++;

			// Didn't get the bus, try next time
			if(i)
			{
				BusIsBusy();
				return USART_FAIL;
			}	
		}
		else
			return USART_FAIL;
	}
	
	// Send the preamble
	for(i=0; i<USART_PREAMBLE_LEN; i++)
		Serial.write(USART_PREAMBLE);
		
	// Start sending the data
	oFrame_Define(frame);
	Serial.write(oFrame_GetLenght()+1);			// Send the frame lenght as first byte
	
	while(oFrame_Available())					// Send the frame	
	{	
		// Get the next byte to send
		data = oFrame_GetByte();
		
		// Compute the CRC
		uint16_t crc = 0xFFFF;
		crc = crc ^ (data);
		for (j=0; j<8; j++) 
		{
			if (crc & 0x0001)
				crc = (crc >> 1) ^ 0xA001;
			else
				crc = crc >> 1;
		}		
		
		// Send byte
		Serial.write(data);							
	}
	
	// Send the CRC
	//Serial.write(crc); // Temporary disabled
	
	// Send postamble
	for(i=0; i<USART_POSTAMBLE_LEN; i++)
		Serial.write(USART_POSTAMBLE);
	
	// Reset the write mode pin of the RS485
	#if(USART_TXENABLE)
	digitalWrite(USART_TXENPIN, LOW);
	#endif
	
	return USART_SUCCESS;
}

/**************************************************************************/
/*!
	Read the incoming data size, use this after vNet_DataAvailable_M5()
*/
/**************************************************************************/
uint8_t vNet_DataSize_M5()
{
	return l;
}

/**************************************************************************/
/*!
	If data are available store in the temporary buffer
*/
/**************************************************************************/
uint8_t vNet_DataAvailable_M5()
{
	uint8_t i=0;
	
	#if(PRESTANOME)	
	Serial.println("1");
	#endif

	if(l == USART_FRAME_LEN-1)
		l=0;		// Buffer is full just before retrieve data, remove junk
	
	// From the USART we get one byte per time, so before has to be identified
	// if there is an incoming Modbus frame	
	while(Serial.available() && (l < USART_FRAME_LEN)){	
		usartframe[l++] = Serial.read();
	
	#if(PRESTANOME)	
		uint8_t k=l-1;
		Serial.print("(0x");
		Serial.print(usartframe[k],HEX);
		Serial.print("),");
	#endif	
	}
	
	#if(PRESTANOME)	
	Serial.println("2");
	#endif	
	
	// If there are no incoming data
	if((l == 0))
	{
		BusFreeSlot();			// Note that there was a free slot
		return USART_FAIL;
	}

	#if(PRESTANOME)	
	Serial.println("3");
	#endif
	
	// There were bytes on the bus
	BusIsBusy();
	
	// If the lenght exceed the buffer size
	if(l > USART_FRAME_LEN-1)
	{
		l = 0;
		return USART_FAIL;
	}

	#if(PRESTANOME)	
	Serial.println("4");
	#endif
	
	// Analyze the retreived frame to findout a vNet message
	for(i=0;i<l-USART_PREAMBLE_LEN;i++)
	{
	
	#if(PRESTANOME)	
	Serial.print("<0x");
	Serial.print(usartframe[i],HEX);
	Serial.print(">,");
	#endif
	
		// Look for vNet preamble used in vNet for Ethernet Raw mode 
		if( (usartframe[i]   == USART_PREAMBLE) &&
			(usartframe[i+1] == USART_PREAMBLE) &&
			(usartframe[i+2] == USART_PREAMBLE) &&
			(usartframe[i+3] == USART_PREAMBLE) &&
			(usartframe[i+4] == USART_PREAMBLE) &&				
			(usartframe[i+5] == USART_PREAMBLE))
		{
			// There is a preamble, look for postamble
				
			// If is a valid vNet message, after the preamble there is the frame lenght
			uint8_t vNetLen = usartframe[i+USART_PREAMBLE_LEN];
			
			#if(PRESTANOME)	
			Serial.println("4a");
			Serial.print("vNetLen ");
			Serial.println(vNetLen,HEX);
			
			for(uint8_t j=0;j<USART_FRAME_LEN;j++){
				Serial.print("[0x");
				Serial.print(usartframe[j],HEX);
				Serial.print("],");
			}
			#endif
			
			if((vNetLen)&& ((i+USART_PREAMBLE_LEN+vNetLen) < USART_FRAME_LEN) &&
				(usartframe[i+USART_PREAMBLE_LEN+vNetLen] == USART_POSTAMBLE) &&
				(usartframe[i+USART_PREAMBLE_LEN+vNetLen+1] == USART_POSTAMBLE) &&
				(usartframe[i+USART_PREAMBLE_LEN+vNetLen+2] == USART_POSTAMBLE) &&
				(usartframe[i+USART_PREAMBLE_LEN+vNetLen+3] == USART_POSTAMBLE) &&
				(usartframe[i+USART_PREAMBLE_LEN+vNetLen+4] == USART_POSTAMBLE) &&
				(usartframe[i+USART_PREAMBLE_LEN+vNetLen+5] == USART_POSTAMBLE))
			{

				// The frame is a valid vNet message, remove the preamble
				memcpy(usartframe, &usartframe[i+USART_PREAMBLE_LEN], vNetLen);	
				
				// Save the transmitter crc
				in_crc = *(uint16_t*)(usartframe+i+USART_PREAMBLE_LEN+vNetLen);		
				
			#if(PRESTANOME)	
			Serial.println("5");				
			#endif		
			
				return vNetLen;			// Return message lenght
			}
			else
			{
				// If we are here, the frame was a token or corrupted
				BusIsBusy();	// Bus busy
				
			#if(PRESTANOME)		
			Serial.println("6");		
			#endif
			
				// No valid message
				l=0;
				return USART_FAIL;		
			}
		}
	}	
	
	#if(PRESTANOME)	
	Serial.println("7");
	#endif
	
	return USART_FAIL;
}

/**************************************************************************/
/*!
	Retrieve the complete vNet frame from the incoming buffer
*/
/**************************************************************************/
uint8_t vNet_RetrieveData_M5(uint8_t *data)
{
	if(l)
	{
		// Retrieve the first byte of the message
		uint8_t len=*(usartframe);

		// Retrieve the complete message
		if(len > 0 && len <= l)
		{	
			uint8_t k=len;
			uint16_t crc;
			crc = 0xFFFF;
			while (k--) 
			{
				crc = crc ^ (oFrame_GetByte());
				for (uint8_t j=0; j < 8; j++) 
				{
					if (crc & 0x0001)
						crc = (crc >> 1) ^ 0xA001;
					else
						crc = crc >> 1;
				}
			}		
			
			// Frame is corrupted
			if(in_crc != (crc << 8  | crc >> 8));
				//return USART_FAIL;
			
			memcpy(data, usartframe+1, len);
			l = 0;										// Reset the lenght
		}
		else
		{
			l = 0;										// Reset the lenght
			return ETH_FAIL;							// Data corrupted
		}
		
		// Return lenght of the data
		return len;
	}
	
	// No available data
	l=0;
	return ETH_FAIL;
}

/**************************************************************************/
/*!
    Get the source address of the most recently received frame
*/
/**************************************************************************/
uint16_t vNet_GetSourceAddress_M5()
{
	return 0;
}