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

#include "GetConfig.h"				// need : ethUsrCfg.h
#include "vNetDriver_eth.h"

#include "frame/vNet/stack/uIP/uip.c"
#include "frame/vNet/stack/uIP/uip_arch.c"
#include "frame/vNet/stack/uIP/uip_arp.c"

#include "frame/vNet/drivers/ethENC28J60/vNetCallBack.c"
#include "frame/vNet/drivers/ethENC28J60/src/ENC28J60.c"
#include "frame/vNet/drivers/ethENC28J60/src/nic.c"

#include "frame/vNet/drivers/ethENC28J60/ServerClient.c"
#include "frame/vNet/tools/UserMode.c"

#define ETHBUF ((struct uip_eth_hdr *)&uip_buf[0])

extern struct uip_conn uip_conns[UIP_CONNS];			// State for uIP connections

uint8_t *appdata;										// Pointer of the incoming packet
uint8_t *ripadrr;										// Pointer of remote IP address
uint8_t vnetlenght=0;									// Lenght of the incoming packet
uint16_t portnumber=0;									// Destination port number of the incoming packet
uint16_t sportnumber=0;									// Source port number of the incoming packet
uint8_t arptimer=0;										// ARP table timeout						
uint8_t usedsock=MAX_SOCK_NUM;							// Socket used at last shift
uint8_t mac_addr[6];									// MAC Address
uint8_t rawdata=0;										// Flag if there are raw data (not UDP/IP, TCP/IP, ICMP, ARP)

U8 vNetM1_header;										// Header for output frame
oFrame vNetM1_oFrame;									// Data structure for output frame

TCPIP stack;											// Structure for IP definitions

extern bool addrsrv;

/**************************************************************************/
/*!
    Init the uIP stack
*/
/**************************************************************************/
void vNet_Init_M1()
{	
	// Init the uIP stack
	uip_init();

	// Init the ARP table
	uip_arp_init();	
}

/**************************************************************************/
/*!
	Start listening the vNet used ports
*/
/**************************************************************************/
void vNet_Begin_M1(uint8_t sock)
{
	uip_udp_listen(ETH_PORT);
}

/**************************************************************************/
/*!
	Set the vNet address and all the network parameters
*/
/**************************************************************************/
void vNet_SetAddress_M1(uint16_t addr)
{
	uint8_t ip_addr[4];
	
	// Translate and set the address
	eth_vNettoIP(addr, &ip_addr[0]);
	eth_SetIPAddress(&ip_addr[0]);
	
	// Set the MAC Address	
	#if(AUTO_MAC)
		eth_vNettoMAC(addr, mac_addr);
		nic_init(mac_addr);
	#else
		eth_vNettoMAC(0, mac_addr);
		nic_init(mac_addr);
	#endif
	
	vNet_Begin_M1(0);								// Start listen on socket

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

	// Check message lenght
	if ((len == 0) || (len >= UIP_PAYLOADSIZE))
		return ETH_FAIL;
	
	// If the frame is not empty, there are waiting data 	
	oFrame_Define(&vNetM1_oFrame);
	if(oFrame_isBusy())
		return ETH_FAIL;		

	// Build a frame with len of payload as first byte
	vNetM1_header = len+1;
	oFrame_Set(&vNetM1_header, 0, 1, 0, frame);
	
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
			// this give routing and NATting pass through
			UserMode_Get(addr, &ip_addr[0], (uint8_t*)(&vNet_port));
		}
		else
		#endif
			eth_vNettoIP(addr, &ip_addr[0]);	// Get the IP address
	}
	
	// Setup the connection, data will be sent using a callback function
	if(!uip_udp_sock((u16_t*)ip_addr, vNet_port, (u16_t)ETH_PORT))
	{		
		// Flag the error
		oFrame_Reset();
		
		return ETH_FAIL;
	}
				
	// Data are processed with the IP stack		
	vNet_uIP();	
	
	// At this stage data are processed or socket is failed, so we can
	// securely reset the oFrame
	oFrame_Reset();	
	
	return ETH_SUCCESS;
}

/**************************************************************************/
/*!
	Read the TCP incoming buffer size
*/
/**************************************************************************/
uint8_t vNet_DataSize_M1()
{
	return uip_datalen();
}

/**************************************************************************/
/*!
	Manage the socket status, it close or reset a socket. If incoming data
	are available, return the data lenght
*/
/**************************************************************************/
uint8_t vNet_DataAvailable_M1()
{
	/*	If there are incoming data, the uip_input() issue a callback that set 
		the vnetlenght value to the lenght of the vNet frame, then the value
		is returned and the retrieve function provide to get the data from the
		buffer */	
	
	uint8_t dlen = ETH_FAIL;
	
	if(srvcln_dataavailable(ETH_PORT))
	{
		dlen = srvcln_dataavailable(ETH_PORT);
	}
	else if(srvcln_dataavailable(ETH_MODBUS))
	{
		// Modbus frame are not handled at this stage, but requires a dedicated
		// call to Modbus method in the main program
	}
	else if(srvcln_dataavailable(ETH_HTTP))
	{	
		// HTTP frame are not handled at this stage, but requires a dedicated
		// call to HTTP method in the main program
	}
	#if(VNET_MEDIA3_ENABLE)
	else if(rawdata)
	{
		// RAW data are not handled at this stage, but requires a dedicated
		// call to vNet Media3 method
	}	
	#endif
	else		
		vNet_uIP();		// Retrieve and process the incoming data
	
	// Return data lenght, if any
	return dlen;
}

/**************************************************************************/
/*!
	Retrieve the complete vNet frame from the incoming buffer
*/
/**************************************************************************/
uint8_t vNet_RetrieveData_M1(uint8_t *data)
{
	// Retrieve the first byte of the message
	uint8_t len=*appdata;

	// Retrieve the complete message
	if((len>0 && len <= vnetlenght) && len <= VNET_MAX_FRAME)
	{	
		memmove(data, appdata+1, len-1);
		vnetlenght = 0;							// Reset the length
		
		// Verify the incoming address, is a not conventional procedure at this layer
		// but is required to record the IP address in case of User Mode addresses
		#if(UMODE_ENABLE)
		// Get the IP source address for the last frame, ripadrr and sportnumber are processed
		// in vNet_UDP_callback() with a callback from the uIP stack.
		
		// Is an UserMode frame, record the incoming source information
		uint16_t umrec = ((*(U16*)&data[4]) & 0xFF00);
		if((umrec != 0x0000) && (umrec <= VNET_ADDR_H_M1))
		{
			sportnumber = HTONS(sportnumber);										// Swap byte before record the source port
			UserMode_Record((*(U16*)&data[4]), ripadrr, (uint8_t*)&sportnumber);	
		}												
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
				stack.base_ip[i] |= (ripadrr[i] & stack.subnetmask[i]);	
				stack.ip[i] 	 |= (ripadrr[i] & stack.subnetmask[i]);				
				stack.gateway[i] |= (ripadrr[i] & stack.subnetmask[i]);				
			}

			addrsrv = false;
		}
		#endif					
	}
	else
	{
		vnetlenght = 0;
		return ETH_FAIL;
	}
	
	return len;
}

/**************************************************************************/
/*!
	Returns if incoming data were buffered in the ENC28J60 and still have to
	be retrieved
*/
/**************************************************************************/
uint8_t vNet_BufferedData_M1()
{
	return  enc28j60BufferedData();
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
	eth_IPtovNet(&addr, ripadrr);

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
	//vNet_addr = (uint8_t *)&addr;
	*(uint16_t *)vNet_addr = addr; //old
	
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
	uint8_t *vNet_addr, brdcast[4] = {0x00};
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
