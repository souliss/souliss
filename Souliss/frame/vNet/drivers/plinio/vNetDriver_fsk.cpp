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
#include "vNetDriver_fsk.h"
#include "src/plinio.cpp"

uint8_t frame[BFSK_FRAME_LEN];			// Buffers for in/out data
Buffer bfsk_buff;						// Structure for in/out data
										
/**************************************************************************/
/*!
    Init the uIP stack
*/
/**************************************************************************/
void vNet_Init_M4()
{	
	bfsk_buff.data=frame;							// Init the buffer pointer	
	bfsk_buff.datalen=0;							// Init the buffer data lenght	
	bfsk_buff.qualityindex = BFSK_QUALITYMAX;		// Init the buffer quality index
	
	// Init the BFSK
	plinio_init();
}

/**************************************************************************/
/*!
	The BFSK work only in broadcast, the SetAddress method is there
	only for standardization with other drivers
*/
/**************************************************************************/
void vNet_SetAddress_M4(uint16_t addr)
{
}

/**************************************************************************/
/*!
	Send a messagge over the bus
*/
/**************************************************************************/
uint8_t vNet_Send_M4(uint16_t addr, oFrame *frame, uint8_t len)
{
	// Check message lenght
	if ((len == 0) || (len >= VNET_MAX_PAYLOAD))
		return BFSK_VNET_FAIL;

	// Insert preamble
	memset(bfsk_buff.data+BFSK_VNET_PREAMBLE, BFSK_PREAMBLE, BFSK_PREAMBLE_LEN);		// Load preamble values
	
	// Build a frame with len of payload as first byte
	*(bfsk_buff.data+BFSK_PREAMBLE_LEN) = len+1;
	
	// Load payload into the buffer
	U8* data = bfsk_buff.data+BFSK_VNET_PAYLOAD;	
	oFrame_Define(frame);
	while(oFrame_Available())
		memset(data++, oFrame_GetByte(), 1);
	
	// Insert postamble (used only in Ethernet-RAW mode)
	memset(bfsk_buff.data+BFSK_VNET_PAYLOAD+len, BFSK_POSTAMBLE, BFSK_POSTAMBLE_LEN);		// Load postamble values

	// Frame lenght
	bfsk_buff.datalen = len+1+BFSK_PREAMBLE_LEN+BFSK_POSTAMBLE_LEN;

	// Send data
	uint8_t ret = plinio_send(bfsk_buff.data, bfsk_buff.datalen);
			
	bfsk_buff.datalen=0;			// Release the buffer		
	return ret;						
	
}

/**************************************************************************/
/*!
	Read the incoming data size, use this after vNet_DataAvailable_M4()
*/
/**************************************************************************/
uint8_t vNet_DataSize_M4()
{				
	return bfsk_buff.datalen;
}

/**************************************************************************/
/*!
	If data are available store in the temporary buffer
*/
/**************************************************************************/
uint8_t vNet_DataAvailable_M4()
{
	// Run the collision avoidance
	plinio_collisionavoidance();	
	
	// Look for available data	
	uint8_t len = plinio_available();
	if(!len)
		return BFSK_VNET_FAIL;
		
	// If a complete frame is available		
	if(len > (BFSK_PREAMBLE_LEN+BFSK_POSTAMBLE_LEN))
	{
		// Analyze the retreived frame to findout a vNet message
		for(uint8_t i=0;i<len;i++)
		{	
			// Look for vNet preamble used in vNet for Ethernet Raw mode 
			if((plinio_getbyte(i) == BFSK_PREAMBLE) &&
				(plinio_getbyte(i+1) == BFSK_PREAMBLE) &&
				(plinio_getbyte(i+2) == BFSK_PREAMBLE) &&
				(plinio_getbyte(i+3) == BFSK_PREAMBLE) &&
				(plinio_getbyte(i+4) == BFSK_PREAMBLE) &&				
				(plinio_getbyte(i+5) == BFSK_PREAMBLE))
			{
				// There is a preamble, look for postamble
			
				// Incoming data have quality good
				bfsk_buff.qualityindex = BFSK_QUALITYMAX;
				
				// If is a valid vNet message, after the preamble there is the frame lenght
				uint8_t vNetLen = plinio_getbyte(i+BFSK_PREAMBLE_LEN);
				
				// If there is a postamble	
				if((vNetLen)&& (!((i+BFSK_PREAMBLE_LEN+vNetLen+BFSK_POSTAMBLE_LEN) > len)) &&
					(plinio_getbyte(i+BFSK_PREAMBLE_LEN+vNetLen) == BFSK_POSTAMBLE) &&
					(plinio_getbyte(i+BFSK_PREAMBLE_LEN+vNetLen+1) == BFSK_POSTAMBLE) &&
					(plinio_getbyte(i+BFSK_PREAMBLE_LEN+vNetLen+2) == BFSK_POSTAMBLE) &&
					(plinio_getbyte(i+BFSK_PREAMBLE_LEN+vNetLen+3) == BFSK_POSTAMBLE) &&
					(plinio_getbyte(i+BFSK_PREAMBLE_LEN+vNetLen+4) == BFSK_POSTAMBLE) &&
					(plinio_getbyte(i+BFSK_PREAMBLE_LEN+vNetLen+5) == BFSK_POSTAMBLE))
				{
					// The frame is a valid vNet message, remove the preamble
					plinio_remove(i, BFSK_PREAMBLE_LEN);
					
					// Incoming data have quality good
					bfsk_buff.qualityindex = BFSK_QUALITYMAX;
				
					bfsk_buff.datalen = vNetLen;	
					return vNetLen;	
				}
			}
		}	
	}
	
	// The incoming data is not a valid frame
	bfsk_buff.datalen = 0;
	if (bfsk_buff.qualityindex >= BFSK_QUALITYMIN) 
		bfsk_buff.qualityindex--;
	
	// If the buffer is full, clean it up
	if(plinio_full() || (bfsk_buff.qualityindex <= BFSK_QUALITYMIN))
		plinio_init();

	// No valid message
	return BFSK_VNET_FAIL;
}

/**************************************************************************/
/*!
	Retrieve the complete vNet frame from the incoming buffer
*/
/**************************************************************************/
uint8_t vNet_RetrieveData_M4(uint8_t *data)
{
	uint8_t len;
	
	// Retrieve the first byte of the message
	plinio_retrieve(&len, 1);
	len--;											// The first byte is the total lenght itself
	
	// Retrieve the complete message
	if((len > 0) && (len < VNET_MAX_PAYLOAD))
	{		
		// Move data in the buffer of the top layer
		plinio_retrieve(data, len);
		
		// Remove the post amble bytes
		plinio_remove(0, BFSK_POSTAMBLE_LEN);
		
		// Return lenght of the data
		return len;		
	}
	else
	{
		// Data corrupted, init the buffer
		plinio_init();	
		
		// No available data
		return BFSK_VNET_FAIL;		
	}

}


/**************************************************************************/
/*!
    Get the source address of the most recently received frame, not
	applicable for this driver. The method is build only for standardization
	with other drivers
*/
/**************************************************************************/
uint16_t vNet_GetSourceAddress_M4()
{
	return 0;
}