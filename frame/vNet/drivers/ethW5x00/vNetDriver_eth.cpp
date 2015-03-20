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

#if(ETH_W5100 || ETH_W5200)
#	include "src/w5x00.cpp"
#elif(ETH_W5500)
#	include "src/w5500.cpp"
#endif

#include "frame/vNet/drivers/ethW5x00/src/socket.cpp"
#include "frame/vNet/tools/UserMode.c"

#include "frame/vNet/drivers/ethW5x00/ServerClient.cpp"

#if (VNET_DEBUG)
	#define VNET_LOG Serial.print
#endif

unsigned long start_time;

U8 vNetM1_header;									// Header for output frame
oFrame vNetM1_oFrame;								// Data structure for output frame
inframe dataframe;									// Data structure for incoming UDP frame

TCPIP stack;

extern bool addrsrv;

/**************************************************************************/
/*!
    Init the W5x00 chip
*/
/**************************************************************************/
void vNet_Init_M1()
{
	W5x00.init();									// Init the chip
}

/**************************************************************************/
/*!
	If the listening socket is not used, set in "LISTEN"
*/
/**************************************************************************/
void vNet_Begin_M1(uint8_t sock)
{
	if(W5x00.readSnSR(sock) == SnSR::CLOSED)
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
		W5x00.setMACAddress(mac_addr);
	#else
		W5x00.setMACAddress((uint8_t*)MAC_ADDRESS);
	#endif
	
	// Set the IP
	W5x00.setIPAddress(stack.ip);
	W5x00.setGatewayIp(stack.gateway);
	W5x00.setSubnetMask(stack.subnetmask);
	
	vNet_Begin_M1(UDP_SOCK);								// Start listen on socket

	// Include debug functionalities, if required
	#if(VNET_DEBUG)
	uint8_t addrval[6];
	
	// Print MAC address 
	W5x00.getMACAddress(addrval);
    VNET_LOG("(MAC)<");
	for(U8 i=0; i<6; i++)
	{
		VNET_LOG(addrval[i],HEX);
		VNET_LOG(",");
	}
	VNET_LOG(">\r\n");

	// Print IP address 
	W5x00.getIPAddress(addrval);
    VNET_LOG("(IP)<");
	for(U8 i=0; i<4; i++)
	{
		VNET_LOG(addrval[i],HEX);
		VNET_LOG(",");
	}
	
	VNET_LOG(">\r\n");
	#endif	
	
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
	if((addr == 0xFFFF) || ((addr > VNET_ADDR_L_M3) && (addr < VNET_ADDR_H_M3)))
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
	if(!sendto(UDP_SOCK, (uint8_t*)&vNetM1_oFrame, 0, &ip_addr[0], vNet_port))
	{
		oFrame_Reset();		// Free the frame
		
		// Restart the socket
		vNet_Stop_M1(UDP_SOCK);
		vNet_Begin_M1(UDP_SOCK);
		
		return ETH_FAIL;	// If data sent fail, return
	}
	
	// At this stage data are processed or socket is failed, so we can
	// securely reset the oFrame
	oFrame_Reset();		
		
	return ETH_SUCCESS;
}

/**************************************************************************/
/*!
	Read the W5x00 incoming buffer size
*/
/**************************************************************************/
uint8_t vNet_DataSize_M1()
{
	return W5x00.getRXReceivedSize(UDP_SOCK);
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
	dataframe.len = W5x00.getRXReceivedSize(UDP_SOCK);
	
	// If the incoming size is bigger than the UDP header
	if((dataframe.len >= 8) && (dataframe.len <= VNET_MAX_FRAME))
		return ETH_SUCCESS;
	
	// Discard
	dataframe.len = 0;
	
	// Reset the socket
	close(UDP_SOCK);
	while(W5x00.readSnSR(UDP_SOCK) != SnSR::CLOSED);
	
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
	// but is required to record the IP address in case of User Mode addresses (0x0100 - 0x64FF)
	#if(UMODE_ENABLE)
	uint16_t umrec = ((*(U16*)&data_pnt[5]) & 0xFF00);
	if((umrec != 0x0000) && (umrec <= VNET_ADDR_H_M1))
		UserMode_Record((*(U16*)&data_pnt[5]), dataframe.ip, (uint8_t *)(&dataframe.port));	
	#endif
	
	// If the addressing server is used, get the subnet from the first broadcast
	// message received. This trick is due to the missing DHCP support in vNet/IP
	// and is supposed to work in most, but not all cases (fixed address will be needed
	// in case of failure)
	#if(UMODE_ENABLE && DYNAMICADDRESSING)
	if(addrsrv == true)
	{
		// Use the incoming IP address to get the subnet to use, we are not changing
		// the part of the IP address related to the vNet address
		uint8_t i;
		for(i=0;i<4;i++)
		{
			// Remove the subnet bits from the actual address
			stack.base_ip[i] &= ~stack.subnetmask[i];
			stack.ip[i] 	 &= ~stack.subnetmask[i];
			stack.gateway[i] &= ~stack.subnetmask[i];
			
			// Get the subnet bits from the incoming adress
			stack.base_ip[i] |= (dataframe.ip[i] & stack.subnetmask[i]);	
			stack.ip[i] 	 |= (dataframe.ip[i] & stack.subnetmask[i]);				
			stack.gateway[i] |= (dataframe.ip[i] & stack.subnetmask[i]);				
		}

		// Reset the IP drivers
		W5x00.setIPAddress(stack.ip);
		W5x00.setGatewayIp(stack.gateway);
		W5x00.setSubnetMask(stack.subnetmask);
		
		vNet_Begin_M1(UDP_SOCK);								// Start listen on socket

		addrsrv = false;
	}
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
