/**************************************************************************
	Souliss - vNet Virtualized Network
    Copyright (C) 2011  Veseo

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

#include "GetConfig.h"				// need : ethUsrCfg.h
#include "vNetDriver_eth.h"
#include "ESP8266WiFi.h"

#include "frame/vNet/tools/UserMode.c"

#if (VNET_DEBUG)
	#define VNET_LOG LOG.print
#endif

#define	VNET_M3_HEADER		1
#define	VNET_M3_APPEND		2

// This device always works with both Media1 and Media3 active, so there is no need to process
// incoming data on Media3 as they are broadcast and can be read from Media1.
void vNet_Init_M3() {}
uint8_t vNet_DataAvailable_M3() {return 0;}
uint8_t vNet_RetrieveData_M3(uint8_t *data) {return 0;}

uint16_t vNetM3_address=0;							// Node address for the media
uint16_t vNetM3_srcaddr=0;							// Node address from incoming frame
uint8_t  vNetM3_isdata =0;							// Flag if Media3 has incoming data
oFrame vNetM3_oFrame;	

WiFiUDP udp;										// WiFi UDP Socket
extern ESP8266WiFiClass WiFi;

unsigned long start_time;

uint8_t vNetM1_header;								// Header for output frame
oFrame vNetM1_oFrame;								// Data structure for output frame
inframe dataframe;									// Data structure for incoming UDP frame

TCPIP stack;
IPAddress ipSend;		

uint8_t dlen=0;										// Number of processed data

U8 local_buffer[VNET_MAX_FRAME];					// The WiFi library doesn't support oFrame
													// so we need to load the data into a buffer
													// this result into a waste of RAM, compared
													// to the serialization done with oFrames but
													// with the ESP8266 RAM isn't a too big concern.

													
													
/**************************************************************************/
/*!
    The ESP8266 cannot be initialized at this stage, because this Init is
	called after the address assignment. This is incompatible with the use
	of DHCP
*/
/**************************************************************************/
void vNet_Init_M1() {}

/**************************************************************************/
/*!
	Set the vNet address and all the network parameters
*/
/**************************************************************************/
void vNet_SetAddress_M1(uint16_t addr)
{
	uint8_t ip_addr[4], mac_addr[6];
	
	// Translate and set the address
	eth_vNettoIP(addr, &ip_addr[0]);
	eth_SetIPAddress(&ip_addr[0]);
		
	udp.begin(ETH_PORT);								// Start listen on socket

	// Include debug functionalities, if required
	#if(VNET_DEBUG)
	IPAddress lIP;

	// Print IP address 
	lIP = WiFi.localIP();
    VNET_LOG("(IP)<");
	for(U8 i=0; i<4; i++)
	{
		VNET_LOG(lIP[i],HEX);
		VNET_LOG(",");
	}
	VNET_LOG(">\r\n");
	#endif	
	
}

void vNet_SetAddress_M3(uint16_t addr)
{
	// Locally store the address
	vNetM3_address=addr;	
	oFrame_Define(&vNetM3_oFrame);
	oFrame_Set((uint8_t*)(&vNetM3_address), 0, VNET_M3_APPEND, 0, 0);
}

/**************************************************************************/
/*!
	Send a message via UDP/IP
*/
/**************************************************************************/
uint8_t vNet_Send_M1(uint16_t addr, oFrame *frame, uint8_t len)
{
	uint8_t ip_addr[4];
	uint16_t vNet_port;

	// Define the standard vNet port
	vNet_port = ETH_PORT;
	
	// Define the IP address to be used
	if((addr == VNET_ADDR_BRDC) ||  (addr == VNET_ADDR_wBRDC) ||  (addr == VNET_ADDR_nBRDC) || ((addr > VNET_ADDR_L_M3) && (addr < VNET_ADDR_H_M3)))
	{
		// Set the IP broadcast address
		for(U8 i=0;i<4;i++)
			ip_addr[i]=0xFF;
	}	
	else	
	{
		// Verify the User Mode	
		#if(UMODE_ENABLE)
		if ((addr & 0xFF00) != 0x0000)
		{	
			// The first byte is the User Mode Index, if in range 0x01 - 0x64
			// a standard client/server connection is used with the user interface
			// this give routing and NATting passthrough
			if(!UserMode_Get(addr, &ip_addr[0], (uint8_t*)(&vNet_port)))
			{		
				// Flag the error
				oFrame_Reset();
				
				return ETH_FAIL;
			}	
		}
		else
		#endif
			eth_vNettoIP(addr, &ip_addr[0]);	// Get the IP address
	}
		
	// Build a frame with len of payload as first byte
	vNetM1_header = len+1;
	oFrame_Define(&vNetM1_oFrame);
	oFrame_Set(&vNetM1_header, 0, 1, 0, frame);

	ipSend = ip_addr;
	udp.beginPacket(ipSend, vNet_port);
	
	// Get data from the oFrame and load them into a local buffer
	U8* l_buff = local_buffer;
	while(oFrame_Available())
		*l_buff++ = oFrame_GetByte();
	
	// Load data into the WiFi library buffer
	udp.write(local_buffer, vNetM1_header) == 0;
	
	// Flag data as ready to be send, then will be processed by the IP stack
	// of the WiFi library
	if(!udp.endPacket())
	{
		oFrame_Reset();		// Free the frame
		
		// Restart the socket
		udp.begin(ETH_PORT);
		
		return ETH_FAIL;	// If data sent fail, return
	}
	
	// At this stage data are processed or socket is failed, so we can
	// securely reset the oFrame
	oFrame_Reset();		
	
	// Flag the socket in listening mode
	udp.begin(ETH_PORT);
	
	return ETH_SUCCESS;
}

/**************************************************************************/
/*!
	Return the last available data size
*/
/**************************************************************************/
uint8_t vNet_DataSize_M1()
{
	return dataframe.len;
}

/**************************************************************************/
/*!
    Get the source address of the most recently received frame
*/
/**************************************************************************/
uint16_t vNet_GetSourceAddress_M3(){return vNetM3_srcaddr;}

/**************************************************************************/
/*!
    The upper layer needs to identify if data are on M1 or M3, and this
	flags are used for that scope
*/
/**************************************************************************/
void  vNet_setIncomingData_M3() {vNetM3_isdata = 1;}
uint8_t  vNet_hasIncomingData_M3() 
{
	if(vNetM3_isdata)
	{
		vNetM3_isdata = 0; 
		return 1;
	}
	return 0;
}	

/**************************************************************************/
/*!
	Send a message via UDP/IP
*/
/**************************************************************************/
uint8_t vNet_Send_M3(uint16_t addr, oFrame *frame, uint8_t len)
{
	uint8_t ip_addr[4];
	uint16_t vNet_port;
	
	// Define the standard vNet port
	vNet_port = ETH_PORT;
	
	// Set the IP broadcast address
	for(U8 i=0;i<4;i++)
		ip_addr[i]=DEFAULT_BASEIPADDRESS[i];
	ip_addr[3]=0xFF;
	
	/***
		Add the whole length as first byte and the node address
		at the end of the frame
	***/

	// Add the node address
	oFrame_Define(&vNetM3_oFrame);
	oFrame_Set((uint8_t*)(&vNetM3_address), 0, VNET_M3_APPEND, 0, 0);
	
	// Add the length as first byte
	vNetM1_header = len+VNET_M3_HEADER+VNET_M3_APPEND;
	oFrame_Define(&vNetM1_oFrame);
	oFrame_Set(&vNetM1_header, 0, 1, 0, frame);

	// Append the address as last, this is contained into a dedicated oFrame
	oFrame_AppendLast(&vNetM3_oFrame);
	
	ipSend = ip_addr;
	udp.beginPacket(ipSend, vNet_port);
	
	// Get data from the oFrame and load them into a local buffer
	U8* l_buff = local_buffer;
	while(oFrame_Available())
		*l_buff++ = oFrame_GetByte();

	// Load data into the WiFi library buffer
	udp.write(local_buffer, vNetM1_header) == 0;
	
	// Flag data as ready to be send, then will be processed by the IP stack
	// of the WiFi library
	if(!udp.endPacket())
	{
		oFrame_Reset();		// Free the frame
		
		// Restart the socket
		udp.begin(ETH_PORT);
		
		return ETH_FAIL;	// If data sent fail, return
	}
	
	// At this stage data are processed or socket is failed, so we can
	// securely reset the oFrame
	oFrame_Reset();		
	udp.begin(ETH_PORT);
	
	return ETH_SUCCESS;
}

/**************************************************************************/
/*!
	Manage the socket status, it close or reset a socket. If incoming data
	are available, return the data length
*/
/**************************************************************************/
uint8_t vNet_DataAvailable_M1()
{
	// Get the incoming length
	dataframe.len = udp.parsePacket();
	
	// If the incoming size is bigger than the UDP header
	if((dataframe.len >= 8) && (dataframe.len <= VNET_MAX_FRAME))
		return ETH_SUCCESS;
	
	// Discard
	dataframe.len = 0;

	// Init the socket
	udp.begin(ETH_PORT);		// Open the socket

	return ETH_FAIL;

}

/**************************************************************************/
/*!
	Retrieve the complete vNet frame from the incoming buffer
*/
/**************************************************************************/
uint8_t vNet_RetrieveData_M1(uint8_t *data)
{
	uint8_t *data_pnt;

	data_pnt=data;
	
	// Get the data from the WiFi library buffer
	udp.read(data,dataframe.len);
	
	// Get source port and IP address
	dataframe.port = udp.remotePort();	
	IPAddress t_ip = udp.remoteIP();
	for(U8 i=0; i<4; i++)
		dataframe.ip[i] = t_ip[i];

	// Verify the incoming address, is a not conventional procedure at this layer
	// but is required to record the IP address in case of User Mode addresses (0x0100 - 0x64FF)
	#if(UMODE_ENABLE)
	uint16_t umrec = (C8TO16(data_pnt+5) & 0xFF00);
	if((umrec != 0x0000) && (umrec <= VNET_ADDR_H_M1))
		UserMode_Record(C8TO16(data_pnt+5), dataframe.ip, (uint8_t *)(&dataframe.port));	
	#endif
	
	// The first byte is the length
	dlen = *data_pnt;
	
	#if(VNET_MEDIA3_ENABLE)
	// Frames from Media 3 has additional bytes at the end
	if((dlen-*(data_pnt+1)) > 1)
	{
		vNetM3_srcaddr = C8TO16(data_pnt+(dlen-VNET_M3_APPEND));
		
		// Remove the header and skip the last two bytes
		data_pnt++;
		dlen = dlen-(VNET_M3_APPEND+VNET_M3_HEADER);	

		// At the upper layer there is no way to identify if data are on M1
		// or on M3, so this flag is used
		vNet_setIncomingData_M3();		
	}
	else
	{
		// Remove the header
		data_pnt++;
		dlen--;		
	}
	#else
		// Remove the header
		data_pnt++;
		dlen--;			
	#endif
	
	// Prepare data from the top level
	memmove(data, data_pnt, dlen);
	
	// Return the number of processed bytes
	return dlen;
}

/**************************************************************************/
/*!
    Get the source address of the most recently received frame
*/
/**************************************************************************/
uint16_t vNet_GetSourceAddress_M1()
{
	uint16_t addr;
	
	// Address translation	
	eth_IPtovNet(&addr, dataframe.ip);

	return addr;
}

/**************************************************************************/
/*!
    Translate a vNet address (2 bytes) in MAC address (6 bytes)
*/
/**************************************************************************/
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

/**************************************************************************/
/*!
    Translate a vNet address (2 bytes) in IP address (4 bytes)
*/
/**************************************************************************/
void eth_vNettoIP(const uint16_t addr, uint8_t *ip_addr)
{
	uint8_t *vNet_addr;
	vNet_addr = (uint8_t *)&addr;
	
	ip_addr[3] = stack.base_ip[3]+*vNet_addr++;
	ip_addr[2] = stack.base_ip[2]+*vNet_addr;
	ip_addr[1] = stack.base_ip[1];
	ip_addr[0] = stack.base_ip[0];

}

/**************************************************************************/
/*!
    Translate an  IP address (4 bytes) in vNet address (2 bytes)
*/
/**************************************************************************/
void eth_IPtovNet(uint16_t *addr, const uint8_t *ip_addr)
{
	uint8_t *vNet_addr;
	vNet_addr = (uint8_t *)addr;
	
	*vNet_addr++ = ip_addr[3] - stack.base_ip[3];
	*vNet_addr = ip_addr[2] - stack.base_ip[2];

}

/**************************************************************************/
/*!
    Set the base IP address
*/
/**************************************************************************/
void eth_SetBaseIP(uint8_t *ip_addr)
{
	uint8_t i;
	for(i=0;i<4;i++)
		stack.base_ip[i] = *ip_addr++;
}

/**************************************************************************/
/*!
    Set the IP address
*/
/**************************************************************************/
void eth_SetIPAddress(uint8_t *ip_addr)
{
	uint8_t i;
	for(i=0;i<4;i++)
		stack.ip[i] = *ip_addr++;
}

/**************************************************************************/
/*!
    Set the Subnet mask
*/
/**************************************************************************/
void eth_SetSubnetMask(uint8_t *submask)
{
	uint8_t i;
	for(i=0;i<4;i++)
		stack.subnetmask[i] = *submask++;
}

/**************************************************************************/
/*!
    Set the Gateway
*/
/**************************************************************************/
void eth_SetGateway(uint8_t *gateway)
{
	uint8_t i;
	for(i=0;i<4;i++)
		stack.gateway[i] = *gateway++;
}

/**************************************************************************/
/*!
    Get the IP address
*/
/**************************************************************************/
void eth_GetIP(uint8_t *ip_addr)
{
	*(ip_addr+0) = stack.ip[0];
	*(ip_addr+1) = stack.ip[1];
	*(ip_addr+2) = stack.ip[2];
	*(ip_addr+3) = stack.ip[3];	
}

/**************************************************************************/
/*!
    Get the base IP address
*/
/**************************************************************************/
void eth_GetBaseIP(uint8_t *ip_addr)
{
	*(ip_addr+0) = stack.base_ip[0];
	*(ip_addr+1) = stack.base_ip[1];
	*(ip_addr+2) = stack.base_ip[2];
	*(ip_addr+3) = stack.base_ip[3];	
}

/**************************************************************************/
/*!
    Get the Subnet mask
*/
/**************************************************************************/
void eth_GetSubnetMask(uint8_t *submask)
{
	*(submask+0) = stack.subnetmask[0];
	*(submask+1) = stack.subnetmask[1];
	*(submask+2) = stack.subnetmask[2];
	*(submask+3) = stack.subnetmask[3];		
}

/**************************************************************************/
/*!
    Get the Gateway
*/
/**************************************************************************/
void eth_GetGateway(uint8_t *gateway)
{
	*(gateway+0) = stack.gateway[0];
	*(gateway+1) = stack.gateway[1];
	*(gateway+2) = stack.gateway[2];
	*(gateway+3) = stack.gateway[3];	
}
