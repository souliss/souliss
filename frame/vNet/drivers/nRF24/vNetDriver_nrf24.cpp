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

#include "src/RF24.cpp"
#include "SPI.h"

RF24 radio(NRF24_RADIOEN, NRF24_SPICS);

/**************************************************************************/
/*!
    Init the nRF24L01 radio
*/
/**************************************************************************/
void vNet_Init_M2()
{
	radio.begin();
	radio.enableDynamicPayloads();
}

/**************************************************************************/
/*!
	Set the pipe address for the radio
*/
/**************************************************************************/
void vNet_SetAddress_M2(uint16_t addr)
{
	// Set the pipe address for broadcast
	radio.openReadingPipe(0, (NRF24_PIPE | ((uint64_t)VNET_ADDR_BRDC)));
	radio.setAutoAck(0, false);
	
	// Set the pipe address for unicast	
	radio.openReadingPipe(1, (NRF24_PIPE | ((uint64_t)addr)));
	radio.setAutoAck(1, true);
	
	// Start listening
	radio.startListening();
}

/**************************************************************************/
/*!
	Send data out
*/
/**************************************************************************/
uint8_t vNet_Send_M2(uint16_t addr, oFrame *frame, uint8_t len)
{
	if(len > VNET_MAX_PAYLOAD)
		return VNET_DATA_FAIL;

	// Set the pipe address of the destination node
	radio.openWritingPipe((NRF24_PIPE | ((uint64_t)addr)));

	// Before write, stop listening pipe
	radio.stopListening();
	
	// Send out the oFrame, doesn't need to specify the lenght
    if(radio.write(frame, 0))
	{
		// Listening back
		radio.startListening();
		
		return NRF24_SUCC;
	}	
	else
	{
		oFrame_Reset();		// Free the frame
		radio.startListening();
	
		return NRF24_FAIL;
	}	
}	

/**************************************************************************/
/*!
	Check for incoming data
*/
/**************************************************************************/
uint8_t vNet_DataAvailable_M2()
{
	return radio.available();
}

/**************************************************************************/
/*!
	Retrieve data from the radio
*/
/**************************************************************************/
uint8_t vNet_RetrieveData_M2(uint8_t *data)
{
	uint8_t* data_pnt = data;

	uint8_t len = radio.getDynamicPayloadSize();
	uint8_t state = radio.read(data, len);
	
	if(!state)	return 0;					// Just skip out
	
	// The nRF24L01 support small payloads and it could be cutted just before sending
	// at this stage we verify the original lenght and fill the missing with zeros.
	
	uint8_t	original_len = *(data);			// First byte is the expected lenght of
											// the vNet frame
	// Fill in the missing bytes
	if(original_len > len)
		for(uint8_t i=0; i<(original_len-len); i++)
			*(data+len+i) = 0;
			
	return state;
}

/**************************************************************************/
/*!
	Actually isn't possible to get the source address for the last received
	frame. Node based on nRF24 cannot identify automatically routing paths.
*/
/**************************************************************************/
uint16_t vNet_GetSourceAddress_M2()
{
	return 0;
}