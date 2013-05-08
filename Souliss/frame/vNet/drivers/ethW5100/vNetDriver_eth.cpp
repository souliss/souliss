/**************************************************************************
	Souliss Home Automation - vNet Virtualized Network
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

#include "frame/vNet/drivers/ethW5100/src/w5100.cpp"
#include "frame/vNet/drivers/ethW5100/src/socket.cpp"
#include "frame/vNet/tools/UserMode.c"

#include "frame/vNet/drivers/ethW5100/ServerClient.cpp"


unsigned long start_time;

U8 vNetM1_header;									// Header for output frame
oFrame vNetM1_oFrame;								// Data structure for output frame
inframe dataframe;									// Data structure for incoming UDP frame

TCPIP stack;

/**************************************************************************/
/*!
    Init the W5100 chip
*/
/**************************************************************************/
void vNet_Init_M1()
{
	W5100.init();									// Init the chip
}

/**************************************************************************/
/*!
	If the listening socket is not used, set in "LISTEN"
*/
/**************************************************************************/
void vNet_Begin_M1(uint8_t sock)
{
	if(W5100.readSnSR(sock) == SnSR::CLOSED)
	{
		socket(sock, SnMR::UDP, ETH_PORT, 0);		// Open the socket
	}
}

/**************************************************************************/
/*!
	Disconnect the socket
*/
/**************************************************************************/
void vNet_Stop_M1(uint8_t sock)
{
	close(sock);
}

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
	
	// Set the MAC Address	
	#if(AUTO_MAC)
		eth_vNettoMAC(addr, mac_addr);
		W5100.setMACAddress(mac_addr);
	#else
		W5100.setMACAddress((uint8_t*)MAC_ADDRESS);
	#endif
	
	// Set the IP
	W5100.setIPAddress(stack.ip);
	W5100.setGatewayIp(stack.gateway);
	W5100.setSubnetMask(stack.subnetmask);
	
	vNet_Begin_M1(UDP_SOCK);								// Start listen on socket

}

/**************************************************************************/
/*!
	Send a messagge via TCP/IP
*/
/**************************************************************************/
uint8_t vNet_Send_M1(uint16_t addr, oFrame *frame, uint8_t len)
{
	uint8_t s, ip_addr[4];
	uint16_t count = 0, vNet_port;
			
	// Broadcast is not supported
	if(addr == 0xFFFF)
		return ETH_FAIL;

	// Define the standard vNet port
	vNet_port = ETH_PORT;

	// Verify the User Mode	
	#if(UMODE_ENABLE)
	if ((addr & 0xFF00) != 0x0000)
	{
		// The first byte is the User Mode Index, if not zero
		// the User Mode shall be used
		
		UserMode_Get(addr, &ip_addr[0]);
		
		// Use the User Mode
		vNet_port = USR_PORT;
	}
	else
	#endif
		eth_vNettoIP(addr, &ip_addr[0]);	// Get the IP address

	// Build a frame with len of payload as first byte
	vNetM1_header = len+1;
	oFrame_Define(&vNetM1_oFrame);
	oFrame_Set(&vNetM1_header, 0, 1, 0, frame);

	// In case of user interface, data are sent more than once	
	#if(UMODE_ENABLE)
	if ((addr & 0xFF00) != 0x0000)
		s = UMODE_SENDS;
	else
	#endif
		s = 1;
	
	// Send data s times
	while(s)
	{
		oFrame Send_oFrame;
		oFrame_Copy(&Send_oFrame, &vNetM1_oFrame);
		
		// Send data
		//if(!sendto(UDP_SOCK, (U8*)&vNetM1_oFrame, 0, &ip_addr[0], vNet_port))
		if(!sendto(UDP_SOCK, (U8*)&Send_oFrame, 0, &ip_addr[0], vNet_port))
			return ETH_FAIL;	// If data sent fail, return
		
		s--;
	}
	
	return ETH_SUCCESS;
}

/**************************************************************************/
/*!
	Read the W5100 incoming buffer size
*/
/**************************************************************************/
uint8_t vNet_DataSize_M1()
{
	return W5100.getRXReceivedSize(UDP_SOCK);
}

/**************************************************************************/
/*!
	Manage the socket status, it close or reset a socket. If incoming data
	are available, return the data lenght
*/
/**************************************************************************/
uint8_t vNet_DataAvailable_M1()
{
	// Get the incoming lenght
	dataframe.len = W5100.getRXReceivedSize(UDP_SOCK);
	
	// If the incoming size is bigger than the UDP header
	//if((dataframe.len >= 8)
	if((dataframe.len >= 8) && ((dataframe.len <= (VNET_HEADER_SIZE + VNET_MAX_PAYLOAD))))
		return ETH_SUCCESS;
	
	// Discard
	dataframe.len = 0;
	
	// Reset the socket
	close(UDP_SOCK);
	while(W5100.readSnSR(UDP_SOCK) != SnSR::CLOSED);
	
	// Init the socket
	socket(UDP_SOCK, SnMR::UDP, ETH_PORT, 0);		// Open the socket

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
	
	if(!recvfrom(UDP_SOCK, data, dataframe.len, dataframe.ip, (uint16_t*)(&dataframe.port)))
		return ETH_FAIL;
		
	// Verify the incoming address, is a not conventional procedure at this layer
	// but is required to record the IP address in case of User Mode addresses
	#if(UMODE_ENABLE)
	if(((*(U16*)&data_pnt[5]) & 0xFF00) != 0x0000)
		UserMode_Record((*(U16*)&data_pnt[5]), dataframe.ip);
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
    Get the base IP address
*/
/**************************************************************************/
void eth_GetBaseIP(uint8_t *ip_addr)
{
		ip_addr = stack.base_ip;
}

/**************************************************************************/
/*!
    Get the Subnet mask
*/
/**************************************************************************/
void eth_GetSubnetMask(uint8_t *submask)
{
	submask = stack.subnetmask;
}

/**************************************************************************/
/*!
    Get the Gateway
*/
/**************************************************************************/
void eth_GetGateway(uint8_t *gateway)
{
	gateway = stack.gateway;
}
