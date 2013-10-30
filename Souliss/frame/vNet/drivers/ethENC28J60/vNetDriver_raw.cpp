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
#include <stdio.h>
#include <string.h>

#include "vNetDriver_raw.h"
#if(!VNET_MEDIA1_ENABLE)
#	include "frame/vNet/drivers/ethENC28J60/src/ENC28J60.c"
#endif

#if(VNET_MEDIA1_ENABLE)
	extern	uint8_t mac_addr[6];					// Source MAC address
#else
	uint8_t mac_addr[6];	
#endif

uint8_t ethraw_frame[ETH_FRAME_LEN];				// Buffer for outgoing data to EN29J60
Buffer ethstr;		

/**************************************************************************/
/*!
    Init the uIP stack
*/
/**************************************************************************/
void vNet_Init_M3()
{	
	ethstr.data=ethraw_frame;						// Init the buffer pointer	
}

/**************************************************************************/
/*!
	Set the vNet address
*/
/**************************************************************************/
void vNet_SetAddress_M3(uint16_t addr)
{
#if(!VNET_MEDIA1_ENABLE)
	// Set the MAC Address	
#	if(AUTO_MAC)
		eth_vNettoMAC(addr, mac_addr);
		enc28j60Init(mac_addr);
#	else
		enc28j60Init((U8 *)&MAC_ADDRESS[0]);
#	endif	
#endif
}

/**************************************************************************/
/*!
	Send a messagge via Ethernet
*/
/**************************************************************************/
uint8_t vNet_Send_M3(uint16_t addr, oFrame *frame, uint8_t len)
{
	uint8_t *data;

	// Check message lenght
	if ((len == 0) || (len >= VNET_MAX_PAYLOAD))
		return ETH_FAIL;

	// Build the Ethernet header	
	memset(ethstr.data+ETH_MAC_DADDR, 0xFF, 6);								// Load broadcast Ethernet address as destination address
	
	// Load source Ethernet address
	#if(AUTO_MAC)
		memcpy(ethstr.data+ETH_MAC_SADDR, mac_addr, 6);
	#else
		memcpy(ethstr.data+ETH_MAC_SADDR, MAC_ADDRESS, 6);
	#endif
	
	*(uint16_t*)(ethstr.data+ETH_MAC_FLEN)=ETH_HEADER_LEN+len+1;			// Message Lenght

	// Insert preamble (used only in Ethernet-RAW mode)
	memset(ethstr.data+ETH_VNET_PREAMBLE, ETH_PREAMBLE, ETH_PREAMBLE_LEN);	// Load preamble values
	
	// Build a frame with len of payload as first byte
	*(ethstr.data+ETH_VNET_PAYLOAD) = len+1;
	
	// Load payload into the buffer
	data = ethstr.data+ETH_VNET_PAYLOAD+1;	
	oFrame_Define(frame);
	while(oFrame_Available())
		memset(data++, oFrame_GetByte(), 1);
	
	// Insert postamble (used only in Ethernet-RAW mode)
	memset(ethstr.data+ETH_VNET_PAYLOAD+1+len, ETH_POSTAMBLE, ETH_POSTAMBLE_LEN);	// Load postamble values

	// Frame lenght
	ethstr.datalen = len+ETH_HEADER_LEN+ETH_PREAMBLE_LEN+ETH_POSTAMBLE_LEN+1;
	
	// Send data
	enc28j60PacketSend(ethstr.datalen, ethstr.data);
	
	// Reset the buffer pointers
	ethstr.datalen=0;
	
	return ETH_SUCCESS;
}

/**************************************************************************/
/*!
	Read the incoming data size, use this after vNet_DataAvailable_M3()
*/
/**************************************************************************/
uint8_t vNet_DataSize_M3()
{
	return ethstr.datalen;
}

/**************************************************************************/
/*!
	If data are available store in the temporary buffer
*/
/**************************************************************************/
uint8_t vNet_DataAvailable_M3()
{
	uint8_t i;
	
	// Read data from the buffer
	#if(VNET_MEDIA1_ENABLE)
		ethstr.data = (U8 *)uip_buf;
		ethstr.datalen = rawdata;
	#else
		ethstr.datalen = enc28j60PacketReceive(ETH_FRAME_LEN, ethstr.data);	
	#endif
	
	// If there are no incoming data
	if(!ethstr.datalen)
	{
		#if(VNET_MEDIA1_ENABLE)
		rawdata = 0;
		#endif
		
		ethstr.datalen = 0;
		return ETH_FAIL;
	}
	
	// If the lenght exceed the buffer size
	if(ethstr.datalen > ETH_FRAME_LEN)
	{
		#if(VNET_MEDIA1_ENABLE)
		rawdata = 0;
		#endif
		
		ethstr.datalen = 0;
		return ETH_FAIL;
	}
	
	// Analyze the retreived frame to findout a vNet message
	for(i=0;i<ethstr.datalen-ETH_PREAMBLE_LEN;i++)
	{
		// Look for vNet preamble used in vNet for Ethernet Raw mode 
		if((ethstr.data[i] == ETH_PREAMBLE) &&
			(ethstr.data[i+1] == ETH_PREAMBLE) &&
			(ethstr.data[i+2] == ETH_PREAMBLE) &&
			(ethstr.data[i+3] == ETH_PREAMBLE) &&
			(ethstr.data[i+4] == ETH_PREAMBLE) &&				
			(ethstr.data[i+5] == ETH_PREAMBLE))
		{
			// There is a preamble, look for postamble
				
			// If is a valid vNet message, after the preamble there is the frame lenght
			uint8_t vNetLen = ethstr.data[i+ETH_PREAMBLE_LEN];
				
			if((vNetLen)&& ((i+ETH_PREAMBLE_LEN+vNetLen) < ETH_FRAME_LEN) &&
				(ethstr.data[i+ETH_PREAMBLE_LEN+vNetLen] == ETH_POSTAMBLE) &&
				(ethstr.data[i+ETH_PREAMBLE_LEN+vNetLen+1] == ETH_POSTAMBLE) &&
				(ethstr.data[i+ETH_PREAMBLE_LEN+vNetLen+2] == ETH_POSTAMBLE) &&
				(ethstr.data[i+ETH_PREAMBLE_LEN+vNetLen+3] == ETH_POSTAMBLE) &&
				(ethstr.data[i+ETH_PREAMBLE_LEN+vNetLen+4] == ETH_POSTAMBLE) &&
				(ethstr.data[i+ETH_PREAMBLE_LEN+vNetLen+5] == ETH_POSTAMBLE))
			{

				// The frame is a valid vNet message, remove the preamble
				memcpy(ethstr.data, &ethstr.data[i+ETH_PREAMBLE_LEN], ethstr.datalen);
				ethstr.datalen-=(i+ETH_PREAMBLE_LEN);		
					
				return ethstr.datalen;			// Return message lenght
			}
			else
			{
				// No valid message
				#if(VNET_MEDIA1_ENABLE)
				rawdata = 0;
				#endif
				
				ethstr.datalen = 0;
				return ETH_FAIL;	
			}
		}
	}	
	
	// No valid message
	#if(VNET_MEDIA1_ENABLE)
	rawdata = 0;
	#endif
		
	ethstr.datalen = 0;
	return ETH_FAIL;
}

/**************************************************************************/
/*!
	Retrieve the complete vNet frame from the incoming buffer
*/
/**************************************************************************/
uint8_t vNet_RetrieveData_M3(uint8_t *data)
{
	if(ethstr.datalen)
	{
		// Retrieve the first byte of the message
		uint8_t len=*(ethstr.data);

		// Retrieve the complete message
		if(len>0 && len <= ethstr.datalen)
		{	
			memcpy(data, ethstr.data+1, len-1);
			ethstr.datalen = 0;							// Reset the lenght
		}
		else
		{
			ethstr.datalen = 0;							// Reset the lenght
			return ETH_FAIL;							// Data corrupted
		}
		
		// Return lenght of the data
		return len;
	}
	
	// No available data
	return ETH_FAIL;
}

/**************************************************************************/
/*!
    Get the source address of the most recently received frame
*/
/**************************************************************************/
uint16_t vNet_GetSourceAddress_M3()
{
	return 0;
}

/**************************************************************************/
/*!
    Translate a vNet address (2 bytes) in MAC address (6 bytes)
*/
/**************************************************************************/
#if(!VNET_MEDIA1_ENABLE)
void eth_vNettoMAC(const uint16_t addr, uint8_t *mac_addr)
{
	uint8_t *vNet_addr;
	vNet_addr = (uint8_t *)&addr;
	
	mac_addr[5] = MAC_ADDRESS[5]+*vNet_addr++;
	mac_addr[4] = MAC_ADDRESS[4]+*vNet_addr;	
	mac_addr[3] = MAC_ADDRESS[3];
	mac_addr[2] = MAC_ADDRESS[2];	
	mac_addr[1] = MAC_ADDRESS[1];
	mac_addr[0] = MAC_ADDRESS[0];
	
	#if(MAC_DEBUG)
		// Set the MAC address as broadcast one
		mac_addr[0] |= 0x01;
	#endif
}
#endif