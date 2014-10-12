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
#include "src/RFM69.h"
#include "src/RFM69.cpp"
#include "SPI.h"

// Define the operating frequency
#if(RFM69_315MHZ)
#	define	FREQUENCY	RF69_315MHZ
#elif(RFM69_433MHZ)
#	define	FREQUENCY	RF69_433MHZ
#elif(RFM69_868MHZ)
#	define	FREQUENCY	RF69_868MHZ
#elif(RFM69_915MHZ)
#	define	FREQUENCY	RF69_915MHZ
#endif

RFM69 radio(RF69_SPI_CS, RF69_IRQ_PIN, RFM69_HIGHPOWER, RF69_IRQ_NUM);

/**************************************************************************/
/*!
    Init the RFM69 radio, there is nothing to do here. This is just
	for compatibility with the upper layers
*/
/**************************************************************************/
void vNet_Init_M2()
{
}

/**************************************************************************/
/*!
	Set the pipe address for the radio
*/
/**************************************************************************/
void vNet_SetAddress_M2(uint16_t addr)
{
	// Set the radio address
	radio.initialize(FREQUENCY, (uint8_t)(addr & 0x00FF), RFM69_NETID);
	
	// Enable the high power transmission
	#if(RFM69_HIGHPOWER)
	radio.setHighPower();
	#endif
	
	// Set the encryption key
	radio.encrypt(RFM69_ENCRYPTKEY);
	
	// The RFM69 radio has only 1 byte as addressing space, this doesn't match the 2 byte
	// addressing space of vNet. So we set the radio in promiscuous mode, the vNet layer 
	// will filter data addressed to the node.
	radio.promiscuous(true);
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

	// Send out the oFrame, doesn't need to specify the length
    if(radio.sendWithRetry((uint8_t)(addr & 0x00FF), frame, 0))
		return RFM69_SUCC;
	else
	{
		oFrame_Reset();		// Free the frame
	
		return RFM69_FAIL;
	}	
}	

/**************************************************************************/
/*!
	Check for incoming data
*/
/**************************************************************************/
uint8_t vNet_DataAvailable_M2()
{
	return radio.receiveDone();
}

/**************************************************************************/
/*!
	Retrieve data from the radio
*/
/**************************************************************************/
uint8_t vNet_RetrieveData_M2(uint8_t *data)
{
	uint8_t* data_pnt = data;

	uint8_t len = radio.DATALEN;
	if(len) memmove(data, (uint8_t *)radio.DATA, len);
	else	
		return RFM69_FAIL;
	
	// The RFM69 support small payloads, that could be cut just before sending
	// at this stage we verify the original length and fill the missing with zeros.
	
	uint8_t	original_len = *(data);			// First byte is the expected length of
											// the vNet frame
	// Fill in the missing bytes
	if(original_len > len)
		for(uint8_t i=0; i<(original_len-len); i++)
			*(data+len+i) = 0;
	
	// Reply with an ACK (only if it wasn't a broadcast)
	if(radio.ACKRequested())
		radio.sendACK();
	
	return RFM69_SUCC;
}

/**************************************************************************/
/*!
	Actually isn't possible to get the source address for the last received
	frame. This can affect multiple hop routing.
*/
/**************************************************************************/
uint16_t vNet_GetSourceAddress_M2()
{
	return 0;
}