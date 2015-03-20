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
#ifndef ETH_CFG_H
#define ETH_CFG_H

/**************************************************************************/
/*!
	Select the Ethernet or Wifi controller used for the board, ensure that 
	pinout of the board correspond to the supported shield or board, otherwise 
	change the pinout in the driver configuration.

	Never select more than one driver per time
	        
		Value       Media
        0x0         Disable (Default)
        0x1         Enable
	
		ETH_W5100		-  Wiznet    W5100 			(Wired Ethernet)
		ETH_W5200		-  Wiznet    W5200 			(Wired Ethernet)
		ETH_W5500		-  Wiznet    W5500 			(Wired Ethernet)		
		ETH_ENC28J60	-  Microchip ENC28J60		(Wired Ethernet)
		WIFI_MRF24		-  Microchip MRF24WB0MA 	(WiFi  Ethernet) 
		WIFI_LPT200		-  Authometion HF-LPT200	(WiFi  Ethernet)
*/
/**************************************************************************/
#if(!(QC_ENABLE))					// Define manually only in Detailed Configuration Mode
#	define ETH_W5100  		0
#	define ETH_W5200  		0
#	define ETH_W5500		0
#	define ETH_ENC28J60  	0
#	define WIFI_MRF24		0
#	define WIFI_LPT200		0
#endif
/**************************************************************************/
/*!
    Define the IP port used for the vNet protocol and the other data
	communication carried over Ethernet
*/
/**************************************************************************/
#define ETH_PORT   		230					// Port for vNet nodes
#define USR_PORT   		23000				// Port for vNet user
#define ETH_HTTP   		80					// Port for HTTP server
#define ETH_MODBUS 		502					// Port for Modbus server

/**************************************************************************/
/*!
    Define the base socket number,
		vNet Ethernet RAW 		 - Use socket RAW_SOCK
		vNet UDP/IP       		 - Use sockets UDP_SOCK
		Server/Client TCP/IP     - Use sockets SRV_SOCK1 and SRV_SOCK2
		Virtual RAM on W5x00	 - Use socket SRV_SOCK2
*/
/**************************************************************************/
#define RAW_SOCK   		0
#define UDP_SOCK	   	1
#define SRV_SOCK1   	2
#define SRV_SOCK2   	3

#define ST_SOCK		   	2			// Starting sock number in Arduino Ethernet compatible
									// mode								
									
/**************************************************************************/
/*!
    User Interface Mode
	
	User Interface Mode is an embeed functionality in vNet over IP that allow
	communication with devices that are out of the LAN and/or use two different 
	port (ETH_PORT, USR_PORT) while writing and receiving vNet frames.
		
	Using the User Mode, the entiere IP address is stored to allow
	communication over NAT.
	
	This functionality must be enabled if is required the communication with
	the Android application or any direct user interface.
	
	vNet addresses in the range 0x0100 - 64FF use User Mode (if enabled)
	
*/
/**************************************************************************/
#define UMODE_ENABLE   		1
#define UMODE_USERS   		5
#define	UMODE_SENDS			3

/**************************************************************************/
/*!
    Define the maximum waiting time while a TCP connection is estabilished
*/
/**************************************************************************/
#define ETH_WAIT   1000

/**************************************************************************/
/*!
    Define the max number of cycle that the socket may wait for incoming data
*/
/**************************************************************************/
#define ESTABILISHED_WAIT   10

/**************************************************************************/
/*!
    Useful definition   
*/
/**************************************************************************/
#define ETH_FAIL		   0
#define ETH_SUCCESS		   1

/**************************************************************************/
/*!
    Arduino Ethernet Library Compatibility
	
	This option include class as per official Arduino Ethernet Library, this
	allow user to customize their Souliss boards using external code developed
	for standard Arduinos.
	You can enable this option only for Wiznet W5100/W5200 (this include the 
	official Arduino Ethernet Shield) and not for controller based on others,
	even if supported in Souliss.
		
	Before using this option, please note the followings:
		- The sockets number 0 and 1 are always used by vNet drivers
		- Arduino IP configuration overrides the vNet ones
			
	In order to avoid misconfiguration, most of vNet methods used for IP init
	are automatically disabled, you must use the standard Arduino ones.
	
	Warning: Adding the Arduino libraries will increase the amount of RAM used,
	generally the libraries based on Arduino Ethernet library are based on ASCII
	protocol and strings that is an additional load on RAM usage.
	
	Enable the general Arduino Ethernet Classes
	
		Value       ARDUINO_ETHLIB
        0x0         Disable (Default)
        0x1         Enable
		
	Enable the DHCP support	
		
		Value       ARDUINO_DHCP
        0x0         Disable (Default)
        0x1         Enable

	Enable the DNS support	
		
		Value       ARDUINO_DNS
        0x0         Disable (Default)
        0x1         Enable			
*/
/**************************************************************************/
#if(!(QC_ENABLE))					// Define manually only in Detailed Configuration Mode
#	define 	ARDUINO_ETHLIB		0x0
#	define	ARDUINO_DHCP		0x0
#	define	ARDUINO_DNS			0x0
#endif

/**************************************************************************/
/*!
    IP Base Configuration 

	The IP address of Ethernet boards is defined as merge of a IP Base Address
	and the vNet address, to get this the DEFAULT_BASEIPADDRESS[] shall always
	have the last byte at zero, that byte is used to define the vNet address.
	
	Below are listed some example of valid and not valid configurations, the
	default configuration match the one used for most of home networks routers.
	
	Example of valid configuration are:
		- IP 192.168. 0.0  / SUBNETMASK 255.255.255.0
		- IP 192.168. 1.0  / SUBNETMASK 255.255.255.0
		- IP 192.168.10.0  / SUBNETMASK 255.255.255.0
	Example of wrong configuration are:	
		- IP 192.168. 0.12  / SUBNETMASK 255.255.255.0	(WRONG)

*/
/**************************************************************************/
#if(!(QC_ENABLE))					// Define manually only in Detailed Configuration Mode
	const uint8_t DEFAULT_BASEIPADDRESS[] = {192, 168, 1, 0};
	const uint8_t DEFAULT_SUBMASK[] 	  = {255, 255, 255, 0};
	const uint8_t DEFAULT_GATEWAY[] 	  = {192, 168, 1, 1};
#endif

/**************************************************************************/
/*!
	IP Broadcasting Configuration
	
	If enabled, nodes use IP broadcast frames instead of unicast ones. This
	remove the need of a bind between the vNet and the IP address, from the 
	vNet point of view, this communication flows as Media3.
	
	Use this option if a node cannot have a static IP address and at same time
	could not use DHCP due to short RAM available.
	
	
		Value       ARDUINO_DHCP
        0x0         Disable (Default)
        0x1         Enable
*/
/**************************************************************************/
#if(!defined(IPBROADCAST_INSKETCH))
#	define IPBROADCAST 		0x00
#endif	
	
/**************************************************************************/
/*!
	MAC Configuration
	
	The MAC address is typically provided with the Ethernet controller board,
	and same address cannot be used on multiple boards. Insert the MAC address
	in the MAC_ADDRESS[] field.
	
	If a MAC address is not available, is suggested the use of a locally MAC
	administred address, that can be inserted manually or automatically.
	
	The MAC address can be automatically assigned to the board if the AUTO_MAC
	option is set. In that case the MAC address will be sum of the MAC_ADDRESS[]
	value plus the vNet address.
	
	If this feature is used, the MAC address must be a local one. Local MAC address
	has the 2nd bit of the first byte set at 1. 
	
	Examples of local MAC address are:
		
		Hex Value		Binary Value
		0x1A 			11010
		0x12			10010
		
		and others.
	
	Only for debugging purpose the MAC_DEBUG option can be used, in that case the 1st
	bit of the first byte will be automatically set to 1. The MAC address will became 
	a broadcast one, message will be available at all the LAN nodes using a sniffer
	like Wireshark.
	
	If using a MAC RAW based communication, all nodes shall have the use the same
	MAC configuration for, otherwise the communication wont work. Debug require 
	AUTO_MAC option enabled.
	
*/
/**************************************************************************/
#if (!defined(MAC_INSKETCH))
	const uint8_t MAC_ADDRESS[] = {0x1A, 0xA6, 0x49, 0x6B, 0x00, 0x01};
#	define AUTO_MAC  		1
#	define MAC_DEBUG	  	0
#endif							

/**************************************************************************/
/*!
	WiFi Configuration
	
	Define the configuration parameters for your wireless network connection,
	these are additional to the standard IP/MAC configuration.
	
	Firstly specify the connection mode using following values, never select 
	more than one driver per time
	        
		Value       Action
        0x0         Disable (Default)
        0x1         Enable
	
		Mode						Description
		Infrastracture				WiFi module connect to a router
		AdHoc						Peer-to-peer connection between two modules
		
	The network protection type suggested is WPA2, if connecting to a router
	refer to router configuration for used protection type. If using an AdHoc
	mode, the configuration parameters set here shall be used by the other
	peer to perform the connection.
	
	The network name and password shall be the one configured into the router
	if using an Infrastracture connection, otherwise can be freely defined
	and then used by the other peer in case of AdHoc connection.
	
*/
/**************************************************************************/
// Connection mode
#if(!(QC_ENABLE))					// Define manually only in Detailed Configuration Mode
#	define WiFi_Infrastracture		1
#	define WiFi_AdHoc				0

// Network protection type
#	define	WiFi_OpenNetwork		0	
#	define 	WiFi_WEP				0					// Please avoid the use of WEP
#	define 	WiFi_WPA				0
#	define	WiFi_WPA2				1

// Network name and password
#	define	WiFi_SSID				"mywifi"
#	define	WiFi_Password			"mypassword"	
#endif

#endif
							  