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
#ifndef VNET_CFG_H
#define VNET_CFG_H

/**************************************************************************/
/*!
    Max payload determines the largest size that can be transmitted in a 
	single frame. Sum of max payload and header should comply with max size
	allowed by used base protocol.    
*/
/**************************************************************************/
#ifndef VNET_MAX_PAYLOAD_INSKETCH
#	define VNET_MAX_PAYLOAD   46
#endif

/**************************************************************************/
/*!
    Header size for vNet frames    
*/
/**************************************************************************/
#define VNET_HEADER_SIZE	6

/**************************************************************************/
/*!
    Overhead space reserved for additional bytes from media driver   
*/
/**************************************************************************/
#define VNET_OVERHEAD_SIZE	10

/**************************************************************************/
/*!
    Number of multicast groups that can be subscribed   
*/
/**************************************************************************/
#define VNET_MULTICAST_SIZE	5

/**************************************************************************/
/*!
    Max frame length  
*/
/**************************************************************************/
#define VNET_MAX_FRAME   (VNET_MAX_PAYLOAD+VNET_HEADER_SIZE+VNET_OVERHEAD_SIZE)

/**************************************************************************/
/*!
    Size for routing table    
*/
/**************************************************************************/
#define VNET_ROUTING_TABLE		   10

/**************************************************************************/
/*!
    Size for bridging table    
*/
/**************************************************************************/
#define VNET_BRIDGING_TABLE		   5

/**************************************************************************/
/*!
    This is maximum number of times that vNet_DataAvailable() can returns
	without having data for the upper layers. 
*/
/**************************************************************************/
#ifndef	VNET_RESETTIME_INSKETCH
#define VNET_RESETTIME			0x000AFC80

// Force a node reset using the MCU watchdog, shall be configured as 
// #define VNET_HARDRESET  while(1)
// this will prevent the MCU to reset the watchdog and will force a reboot.
//
// By default this is empty and does nothing, please note that if you haven't
// a watchdog enabled, this will hang your node and you will need to manually
// reebot it.
#define VNET_HARDRESET	
#endif

/**************************************************************************/
/*!
    If this option is enabled the Gateway doesn't accept new incoming 
	connection from user interfaces after an elapsed time (from boot)
*/
/**************************************************************************/
#ifndef	VNET_USERLOCKDOWN_INSKETCH
#	define VNET_USERLOCKDOWN		0
#endif

/**************************************************************************/
/*!
    Useful definition   
*/
/**************************************************************************/
#define VNET_FAIL				   	0
#define VNET_DATA_FAIL			   	0
#define VNET_DATA_SUCCESS		   	1
#define VNET_SUCCESS				1
#define VNET_DATA_ROUTED		   	255



/**************************************************************************/
/*!
    To avoid network flood on gateway with multiple network interfaces a
	delay time of VNET_BROADCAST_DELAY milliseconds is used.
	
        Value       VNET_BROADCAST_DEFAULT
        0x0         Disable
        0x1         Enable (Default)
*/
/**************************************************************************/
#define VNET_BROADCAST_DEFAULT		1

#define	VNET_BROADCAST_ENABLE		1
#define	VNET_BROADCAST_DISABLE		0
#define	VNET_BROADCAST_DELAY		300		// Delay in milliseconds

/**************************************************************************/
/*!
	If enabled print the header and payload of incoming, outgoing and routed
	frames.
	
        Value       Status
        0x0         Disable (Default)
        0x1         Enable
*/
/**************************************************************************/
#ifndef	VNET_DEBUG_INSKETCH
#	define VNET_DEBUG  		0
#endif								
/**************************************************************************/
/*!
	If enabled allow broadcast support for incoming frames, to work properly
	the same should be supported also by the communication media drivers. 
	
        Value       Status
        0x0         Disable 
        0x1         Enable	(Default)
*/
/**************************************************************************/
#define VNET_BRDCAST  		1								

/**************************************************************************/
/*!
	If enabled prevent broadcast messages to loop, this is useful in network
	where there are loops between SUPERNODE nodes.
	
        Value       Status
        0x0         Disable (Default)
        0x1         Enable
*/
/**************************************************************************/
#define VNET_LOOPS  		0								

/**************************************************************************/
/*!
	If enabled hold the code just after the re-broadcast, this result in a
	delay before the frame is passed to the upper layers.
	
        Value       Status
        0x0         Disable (Default)
        0x1         Enable
*/
/**************************************************************************/
#ifndef VNET_BRDDELAY_INSKETCH
#	define VNET_BRDDELAY_ENABLE		0
#endif

#define	VNET_BRDDELAY				1000
								
/**************************************************************************/
/*!
	A SuperNode is a device with routing and bridging capability, if Enable 
	message from Nodes of the own subnet can be sent out to other subnet.

	SuperNode address should be the first usable address of the subnet. 
	
        Value       SuperNode
        0x0         Disable (Default)
        0x1         Enable
*/
/**************************************************************************/
#ifndef VNET_SUPERNODE_INSKETCH
#	define VNET_SUPERNODE  	0
#endif

/**************************************************************************/
/*!
	A Node or a Supernode can have one or more media interfaces active at
	same time. A device (if Supernode) automatically  act  as a bridge between
	the active media.
	        
		Value       Media
        0x0         Disable (Default)
        0x1         Enable
	
		Communication Media 1	-	Ethernet UDP/IP 
		Communication Media 2	-	Wireless - 2.4 GHz IEEE 802.15.4 
		Communication Media 3	-	IP Broadcast only
		Communication Media 4	-	Not used
		Communication Media 5	-	USART with collision avoidance 	
*/
/**************************************************************************/
#define VNET_MEDIA_NUMBER   5		// Number of total media

#define VNET_MEDIA1_ID  0
#define VNET_MEDIA2_ID  1
#define VNET_MEDIA3_ID  2
#define VNET_MEDIA4_ID  3
#define VNET_MEDIA5_ID  4

#ifndef VNET_MEDIA_INSKETCH
#	define VNET_MEDIA1_ENABLE  0
#	define VNET_MEDIA2_ENABLE  0
#	define VNET_MEDIA3_ENABLE  0
#	define VNET_MEDIA4_ENABLE  0
#	define VNET_MEDIA5_ENABLE  0


	const U16 vnet_media_en[VNET_MEDIA_NUMBER] = {VNET_MEDIA1_ENABLE,  // Media 1
												  VNET_MEDIA2_ENABLE,  // Media 2
												  VNET_MEDIA3_ENABLE,  // Media 3
												  VNET_MEDIA4_ENABLE,  // Media 4
												  VNET_MEDIA5_ENABLE   // Media 5	
												};
#endif

/**************************************************************************/
/*!
	Address range for communication media,
		In order to find out the relationship for media and address, the
		following assignment should be followed.

*/
/**************************************************************************/
#define VNET_ADDR_L_M1	   	0x0000			// vNet Low Address for Media 1,  size 4 byte
#define VNET_ADDR_H_M1	   	0x64FF			// vNet High Address for Media 1, size 4 byte

#define VNET_ADDR_L_M2	   	0x6500			// vNet Low Address for Media 2,  size 4 byte
#define VNET_ADDR_H_M2	   	0xAAFF			// vNet High Address for Media 2, size 4 byte

#define VNET_ADDR_L_M3	   	0xAB00			// vNet Low Address for Media 3,  size 4 byte
#define VNET_ADDR_H_M3	   	0xBBFF			// vNet High Address for Media 3, size 4 byte

#define VNET_ADDR_L_M4	   	0xBC00			// vNet Low Address for Media 4,  size 4 byte
#define VNET_ADDR_H_M4	   	0xCCFF			// vNet High Address for Media 4, size 4 byte

#define VNET_ADDR_L_M5	   	0xCE00			// vNet Low Address for Media 5,  size 4 byte
#define VNET_ADDR_H_M5	   	0xDFFF			// vNet High Address for Media 5, size 4 byte

#define	VNET_ADDR_L_MLC		0xF100			// First MULTICAST address
#define	VNET_ADDR_H_MLC		0xFCFF			// Last  MULTICAST address

#define	VNET_ADDR_nBRDC		0xFDFF			// This is a BROADCAST address that cannot be re-broadcasted
#define	VNET_ADDR_wBRDC		0xFEFF			// This is a BROADCAST address that cannot be re-broadcasted over wireless
#define	VNET_ADDR_BRDC		0xFFFF			// This is a BROADCAST address

#define	VNET_ADDR_NULL		0xFF00			// This is a NULL address, frames to that address are discarded

const U16 vnet_addr_l[VNET_MEDIA_NUMBER] = {VNET_ADDR_L_M1,  // Media 1
											VNET_ADDR_L_M2,  // Media 2
											VNET_ADDR_L_M3,  // Media 3
											VNET_ADDR_L_M4,  // Media 4
											VNET_ADDR_L_M5   // Media 5
											};
							  
const U16 vnet_addr_h[VNET_MEDIA_NUMBER] = {VNET_ADDR_H_M1, // Media 1
											VNET_ADDR_H_M2, // Media 2
											VNET_ADDR_H_M3, // Media 3
											VNET_ADDR_H_M4, // Media 4
											VNET_ADDR_H_M5  // Media 5
											};
											
#endif
							  