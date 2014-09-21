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
#include <stdio.h>
#include <string.h>

#include "GetConfig.h"				// need : ethUsrCfg.h
#include "vNetDriver_eth.h"

#include "frame/vNet/drivers/ethLPT200/src/lpt200.c"
#include "frame/vNet/tools/UserMode.c"

U8 vNetM1_header;									// Header for output frame
oFrame vNetM1_oFrame;								// Data structure for output frame
inframe dataframe;									// Data structure for incoming UDP frame

TCPIP stack;

extern bool addrsrv;

/**************************************************************************/
/*!
    Init the LPT200 chip
*/
/**************************************************************************/
void vNet_Init_M1()
{
	// The Init_M1() is called by vNet_Init() and suppose that a valid address
	// is yet available. Since this module works in DHCP only, we have to init
	// the module before the vNet driver.

	// The init shall be called outside before start vNet
}

/**************************************************************************/
/*!
	Set the vNet address and all the network parameters (only for API compatibility)
*/
/**************************************************************************/
void vNet_SetAddress_M1(uint16_t addr)
{
}

/**************************************************************************/
/*!
	Send a messagge via UDP/IP
*/
/**************************************************************************/
uint8_t vNet_Send_M1(uint16_t addr, oFrame *frame, uint8_t len)
{
	uint8_t ip_addr[4];
	uint16_t vNet_port;
	
	// Define the standard vNet port
	vNet_port = ETH_PORT;
	
	// Define the IP address to be used
	if(addr == 0xFFFF)
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
			// this give rounting and NATting passthrough
			UserMode_Get(addr, &ip_addr[0], (uint8_t*)(&vNet_port));
		}
		else
		#endif
			eth_vNettoIP(addr, &ip_addr[0]);	// Get the IP address
	}

	// Build a frame with len of payload as first byte
	vNetM1_header = len+1;
	oFrame_Define(&vNetM1_oFrame);
	oFrame_Set(&vNetM1_header, 0, 1, 0, frame);

	// Send data	
	if(!sendUDP((uint8_t*)&vNetM1_oFrame, 0, &ip_addr[0], vNet_port))
	{
		oFrame_Reset();		// Free the frame
		
		return ETH_FAIL;	// If data sent fail, return
	}

	// At this stage data are processed or socket is failed, so we can
	// securely reset the oFrame
	oFrame_Reset();		
		
	return ETH_SUCCESS;
}

/**************************************************************************/
/*!
	Read the LPT200 incoming buffer size
*/
/**************************************************************************/
uint8_t vNet_DataSize_M1()
{
	return getlen();
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
	dataframe.len = dataaval();
	
	// If the incoming size is bigger than the UDP header
	if((dataframe.len >= 8) && (dataframe.len <= VNET_MAX_FRAME))
		return ETH_SUCCESS;
		
	// Discard
	dataframe.len = 0;

	return ETH_FAIL;

}

/**************************************************************************/
/*!
	Retrieve the complete vNet frame from the incoming buffer
*/
/**************************************************************************/
uint8_t vNet_RetrieveData_M1(uint8_t *data)
{
	uint8_t *data_pnt, d_addr[4];
	uint16_t d_port=0;

	data_pnt=data;
	
	if(!recvUDP(data, VNET_MAX_FRAME, dataframe.ip, (uint16_t*)(&dataframe.port), d_addr, &d_port))
		return ETH_FAIL;
				
	// Verify the incoming address, is a not conventional procedure at this layer
	// but is required to record the IP address in case of User Mode addresses
	#if(UMODE_ENABLE)
	if(((*(U16*)&data_pnt[5]) & 0xFF00) != 0x0000)
		UserMode_Record((*(U16*)&data_pnt[5]), dataframe.ip, (uint8_t *)(&dataframe.port));	
	#endif
	
	// Remove the header
	data_pnt++;
	dataframe.len--;
	memmove(data, data_pnt, dataframe.len);

	return dataframe.len;
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
    Set the Subnet mask (only for API compatibility)
*/
/**************************************************************************/
void eth_SetSubnetMask(uint8_t *submask)
{
}

/**************************************************************************/
/*!
    Set the Gateway (only for API compatibility)
*/
/**************************************************************************/
void eth_SetGateway(uint8_t *gateway)
{
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
    Get the Subnet mask (only for API compatibility)
*/
/**************************************************************************/
void eth_GetSubnetMask(uint8_t *submask)
{	
}

/**************************************************************************/
/*!
    Get the Gateway (only for API compatibility)
*/
/**************************************************************************/
void eth_GetGateway(uint8_t *gateway)
{	
}
