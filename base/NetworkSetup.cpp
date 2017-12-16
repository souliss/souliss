/**************************************************************************
	Souliss
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

#include "Souliss.h"
#include "Typicals.h"
#include <Arduino.h>

#include "tools/store/store.h"
#include "tools/types.h"
#include "GetConfig.h"			// need : ethUsrCfg.h, vNetCfg.h, SoulissCfg.h, MaCacoCfg.h
#include "Souliss.h"

bool FirstInit = {false}, addrsrv = {false};
U16 keyidval=0;
uint8_t	myvNet_dhcp=0;

/**************************************************************************/
/*!
	Set and init network configuration
*/	
/**************************************************************************/
void Souliss_SetAddress(U16 addr, U16 subnetmask, U16 mysupernode)
{
	// Set configuration parameters dedicated to the relevant interface
	if(vNet_GetMedia(addr) == 1)			
	{
	#if(VNET_MEDIA1_ENABLE)
		// Media 1 - Ethernet
		eth_SetBaseIP((uint8_t *)DEFAULT_BASEIPADDRESS);
		eth_SetSubnetMask((uint8_t *)DEFAULT_SUBMASK);
		eth_SetGateway((uint8_t *)DEFAULT_GATEWAY);
	#endif
	}
	else if(vNet_GetMedia(addr) == 2)		
	{
		// Media 2 - Chibiduino Wireless
	}
	else if(vNet_GetMedia(addr) == 3)
	{
		#if((VNET_MEDIA3_ENABLE) && !(VNET_MEDIA1_ENABLE))
			// Media 1 - Ethernet
			eth_SetBaseIP((uint8_t *)DEFAULT_BASEIPADDRESS);
			eth_SetSubnetMask((uint8_t *)DEFAULT_SUBMASK);
			eth_SetGateway((uint8_t *)DEFAULT_GATEWAY);
		#endif		
	}
	else if(vNet_GetMedia(addr) == 4)
	{
	}
	else if(vNet_GetMedia(addr) == 5)
	{
	#if(VNET_MEDIA5_ENABLE)	
		// These information are not available in the driver itself, so are passed directly 
		// in order to calculate a collision avoidance index
		vNet_SetCollisionAvoidanceIndex_M5(addr, subnetmask);
	#endif	
	}
	
	// Set configuration general parameters
    if(!FirstInit)
	{
		vNet_Init();											// Init vNet
		FirstInit = true;
	}
	
	vNet_SetAddress(addr, vNet_GetMedia(addr));					// Set vNet Address
	vNet_SetSubnetMask(subnetmask, vNet_GetMedia(addr));		// Set vNet Subnetmask
	vNet_SetMySuperNode(mysupernode, vNet_GetMedia(addr));		// Set vNet Supernode
	
}

/**************************************************************************/
/*!
	Set the addresses of local nodes into the memory map, shall be used
	for gateway node
*/	
/**************************************************************************/
void Souliss_SetLocalAddress(U8 *memory_map, U16 addr)
{
	// Set the remote address of a node into the vNet
	*(memory_map+MaCaco_ADDRESSES_s)   = C16TO8L(addr);
	*(memory_map+MaCaco_ADDRESSES_s+1) = C16TO8H(addr);
	
}

/**************************************************************************/
/*!
	Reset the addresses of local nodes into the memory map, shall be used
	to have the node acting not as a gateway
*/	
/**************************************************************************/
void Souliss_ResetLocalAddress(U8 *memory_map)
{
	// Set the remote address of a node into the vNet
	*(memory_map+MaCaco_ADDRESSES_s)   = 0;
	*(memory_map+MaCaco_ADDRESSES_s+1) = 0;
	
}

/**************************************************************************/
/*!
	Get the addresses of local nodes from the memory map, identify if a node
	has been set as gateway at runtime
*/	
/**************************************************************************/
U16 Souliss_GetLocalAddress(U8 *memory_map)
{
	return C8TO16(memory_map);
}

/**************************************************************************/
/*!
	Set the addresses of other nodes into the network
*/	
/**************************************************************************/
void Souliss_SetRemoteAddress(U8 *memory_map, U16 addr, U8 node)
{
	// Set the remote address of a node into the vNet
	*(memory_map+MaCaco_ADDRESSES_s+node*2)   = C16TO8L(addr);
	*(memory_map+MaCaco_ADDRESSES_s+node*2+1) = C16TO8H(addr);	
}

/**************************************************************************/
/*!
	Set an IP address and calculate automatically the vNet address, it
	overwrite all other settings
*/	
/**************************************************************************/
void Souliss_SetIPAddress(U8* ip_address, U8* subnet_mask, U8* ip_gateway)
{	
	// Starting from IP configuration define the vNet ones
	U8 i=0;
	for(i=0; i<4; i++)
	{
		if(DEFAULT_BASEIPADDRESS) 	DEFAULT_BASEIPADDRESS[i]=ip_address[i];
		if(DEFAULT_SUBMASK) 		DEFAULT_SUBMASK[i] = subnet_mask[i];
		if(DEFAULT_GATEWAY) 		DEFAULT_GATEWAY[i] = ip_gateway[i];
	}
	
	U16 vNet_address = (U16)ip_address[i-1];			// The last byte of the IP address is the vNet one
	DEFAULT_BASEIPADDRESS[i-1]=0;						// The BASEIPADDRESS has last byte always zero

	#if((MCU_TYPE == 0x02) || (MCU_TYPE == 0x03))	// Expressif ESP8266 or ESP32
		U8 timeout=20;
	
		// If is the first time that we connect to WiFi.SSID
		if(strcmp(WiFi.SSID().c_str(), WiFi_SSID) || strcmp(WiFi.psk().c_str(), WiFi_Password))
		{
			WiFi.mode(WIFI_STA);
			WiFi.begin(WiFi_SSID, WiFi_Password);
		}
		else
			WiFi.begin();	// WiFi.SSID is a known network, no need to specify it
		
		// Connect
		while ((WiFi.status() != WL_CONNECTED) && timeout)
		{
			timeout--;
			delay(500);
		}

		// Set manually an IP address
		WiFi.config(ip_address, ip_gateway, subnet_mask);

	#endif
	
	// Set the address
	Souliss_SetAddress(vNet_address, DYNAMICADDR_SUBNETMASK, 0);
}

/**************************************************************************/
/*!
	Get IP Address from DHCP
*/	
/**************************************************************************/ 
uint8_t Souliss_GetIPAddress(U8 timeout=20)
{

#if((MCU_TYPE == 0x01) && ARDUINO_DHCP)	// Atmel AVR Atmega

	// Use software based DHCP client
	Ethernet.begin();
	IPAddress ip = Ethernet.localIP();
	
	// The last byte of the IP address is used as vNet address
	myvNet_dhcp = ip[3];	
	return 1;

	/*** This calls Souliss_SetIPAddress directly	***/

#elif((MCU_TYPE == 0x02) || (MCU_TYPE == 0x03))	// Expressif ESP8266 or ESP32
	
	// If is the first time that we connect to WiFi.SSID
	if(strcmp(WiFi.SSID().c_str(), WiFi_SSID) || strcmp(WiFi.psk().c_str(), WiFi_Password))
	{
		WiFi.mode(WIFI_STA);
		WiFi.begin(WiFi_SSID, WiFi_Password);
	}
	else
		WiFi.begin();	// WiFi.SSID is a known network, no need to specify it

	// Connect
	while ((WiFi.status() != WL_CONNECTED) && timeout)
	{
		timeout--;
		delay(500);
	}
	
	// Get the IP network parameters
	IPAddress lIP  = WiFi.localIP();
	IPAddress sMk  = WiFi.subnetMask();
	IPAddress gIP  = WiFi.gatewayIP();
	
	uint8_t i;
	uint8_t ipaddr[4];
	uint8_t subnet[4];
	uint8_t gateway[4];
	
	for(i=0;i<4;i++)
	{
		ipaddr[i]  = lIP[i];
		subnet[i]  = sMk[i];
		gateway[i] = gIP[i];
	}	

	// The last byte of the IP address is used as vNet address
	myvNet_dhcp = ipaddr[3];
	
	// Starting from IP configuration define the vNet ones
	for(i=0; i<4; i++)
	{
		if(DEFAULT_BASEIPADDRESS) 	DEFAULT_BASEIPADDRESS[i]=ipaddr[i];
		if(DEFAULT_SUBMASK) 		DEFAULT_SUBMASK[i] = subnet[i];
		if(DEFAULT_GATEWAY) 		DEFAULT_GATEWAY[i] = gateway[i];
	}
	
	U16 vNet_address = (U16)ipaddr[i-1];			// The last byte of the IP address is the vNet one
	DEFAULT_BASEIPADDRESS[i-1]=0;					// The BASEIPADDRESS has last byte always zero
	
	// Set the address
	Souliss_SetAddress(vNet_address, DYNAMICADDR_SUBNETMASK, 0);

	#if((MCU_TYPE == 0x02) || (MCU_TYPE == 0x03))	// Expressif ESP8266 or ESP32
	if(WiFi.status() != WL_CONNECTED) 
	{
		// Print debug messages
		#if (SOULISS_DEBUG)
		SOULISS_LOG(F("(ss)<WiFi Fail>\r\n"));
		#endif	

		return 0;
	}
	else 
	{
		// Print debug messages
		#if (SOULISS_DEBUG)
		SOULISS_LOG(F("(ss)<WiFi Connected>\r\n"));
		#endif	

		return 1;
	}
	#endif

#endif	
}												

/**************************************************************************/
/*!
	Start the node as an access point
*/	
/**************************************************************************/ 
#if((MCU_TYPE == 0x02) || (MCU_TYPE == 0x03))	// Expressif ESP8266 or ESP32
String Souliss_SetAccessPoint(const char ap_name[32] = "", const char ap_pass[32] = "")
{
	uint8_t i;
	uint8_t ipaddr[4];
	uint8_t gateway[4];		
	
	// Set the access point name with the last 3 bytes of the WiFi MAC address
	char _apname[32];

	if (ap_name[0] == 0) {

		// get the MAC address
		byte mac[6];
	    char _macaddr[18];

	    WiFi.softAPmacAddress(mac);

	    sprintf(_apname, "Souliss_%02X%02X%02X", mac[3],mac[4],mac[5]);

	} else {
		strncpy(_apname, ap_name, 31);
		_apname[31] ='\0';
	}

	
	WiFi.mode(WIFI_AP);
	WiFi.softAP(_apname, ap_pass);

	// Setup the Souliss framework, get the IP network parameters
	IPAddress lIP  = WiFi.softAPIP();


	
	for(i=0;i<4;i++)
	{
		ipaddr[i]  = lIP[i];
		gateway[i] = lIP[i];
	}	

	// The last byte of the IP address is used as vNet address
	myvNet_dhcp = ipaddr[3];
	
	// Starting from IP configuration define the vNet ones
	for(i=0; i<4; i++)
	{
		if(DEFAULT_BASEIPADDRESS) 	DEFAULT_BASEIPADDRESS[i]=ipaddr[i];
		if(DEFAULT_GATEWAY) 		DEFAULT_GATEWAY[i] = gateway[i];
	}
	
	U16 vNet_address = (U16)ipaddr[i-1];			// The last byte of the IP address is the vNet one
	DEFAULT_BASEIPADDRESS[i-1]=0;					// The BASEIPADDRESS has last byte always zero
	
	// Set the address
	Souliss_SetAddress(vNet_address, DYNAMICADDR_SUBNETMASK, 0);

	return String(_apname);	
}
#endif

/**************************************************************************/
/*!
	Read IP Configuration from EEPROM or equivalent
*/	
/**************************************************************************/ 
#if(USEEEPROM)
uint8_t Souliss_ReadIPConfiguration()
{
	U8 i=0, timeout=20;

	// If a valid configuration hasn't been found
	if(Return_ID()!=STORE__DEFAULTID) 
	{
		// Print debug messages
		#if (SOULISS_DEBUG)
		SOULISS_LOG(F("(ss)<No stored config>\r\n"));
		#endif
		
		return 0;	
	}

	// Setup the SSID and Password
	#if((MCU_TYPE == 0x02) || (MCU_TYPE == 0x03))	// Expressif ESP8266 or ESP32	
	String SSID = Read_SSID();
	String PSW  = Read_Password();

		#if (SOULISS_DEBUG)
		// Print debug messages
		SOULISS_LOG(F("(ss)<Read_SSID>"));
		SOULISS_LOG(SSID);	
		SOULISS_LOG("\r\n");

		SOULISS_LOG(F("(ss)<Read_Password>"));
		SOULISS_LOG(PSW);	
		SOULISS_LOG("\r\n");
		#endif

	#endif

	// If the DHCP Mode is set
	if(Return_DHCPMode())
	{
		// Print debug messages
		#if (SOULISS_DEBUG)
		SOULISS_LOG(F("(ss)<DHCPMode Set>\r\n"));
		#endif		

		#if((MCU_TYPE == 0x01) && ARDUINO_DHCP)	// Atmel AVR Atmega

			// Use software based DHCP client
			Ethernet.begin();
			IPAddress ip = Ethernet.localIP();
			
			// The last byte of the IP address is used as vNet address
			myvNet_dhcp = ip[3];	
			
		#elif((MCU_TYPE == 0x02) || (MCU_TYPE == 0x03))	// Expressif ESP8266 or ESP32
			
			// If is the first time that we connect to WiFi.SSID

			#if (SOULISS_DEBUG)
			// Print debug messages
			SOULISS_LOG(F("(ss)<Read_SSID>"));
			SOULISS_LOG(WiFi.SSID());	
			SOULISS_LOG("\r\n");

			SOULISS_LOG(F("(ss)<Read_Password>"));
			SOULISS_LOG(WiFi.psk());	
			SOULISS_LOG("\r\n");
			#endif

			if(strcmp(WiFi.SSID().c_str(), SSID.c_str()) || strcmp(WiFi.psk().c_str(), PSW.c_str()))
			{
				WiFi.mode(WIFI_STA);
				WiFi.begin(SSID.c_str(), PSW.c_str());
			}
			else
				WiFi.begin();				// WiFi.SSID is a known network, no need to specify it

			// Connect
			while ((WiFi.status() != WL_CONNECTED) && timeout)
			{
				timeout--;
				delay(500);
			}
			
			// Get the IP network parameters
			IPAddress lIP  = WiFi.localIP();
			IPAddress sMk  = WiFi.subnetMask();
			IPAddress gIP  = WiFi.gatewayIP();
			
			uint8_t i;
			uint8_t ipaddr[4];
			uint8_t subnet[4];
			uint8_t gateway[4];
			
			for(i=0;i<4;i++)
			{
				ipaddr[i]  = lIP[i];
				subnet[i]  = sMk[i];
				gateway[i] = gIP[i];
			}	

			// The last byte of the IP address is used as vNet address
			myvNet_dhcp = ipaddr[3];
			
			// Starting from IP configuration define the vNet ones
			for(i=0; i<4; i++)
			{
				if(DEFAULT_BASEIPADDRESS) 	DEFAULT_BASEIPADDRESS[i]=ipaddr[i];
				if(DEFAULT_SUBMASK) 		DEFAULT_SUBMASK[i] = subnet[i];
				if(DEFAULT_GATEWAY) 		DEFAULT_GATEWAY[i] = gateway[i];
			}
			
			U16 vNet_address = (U16)ipaddr[i-1];			// The last byte of the IP address is the vNet one
			DEFAULT_BASEIPADDRESS[i-1]=0;					// The BASEIPADDRESS has last byte always zero
			
			// Set the address
			Souliss_SetAddress(vNet_address, DYNAMICADDR_SUBNETMASK, 0);	
		#endif	
	}
	else					
	{
		// Static IP Address
		uint8_t _ip_address[4];
		uint8_t _subnet_mask[4];
		uint8_t _ip_gateway[4];

		// Read from EEPROM
		Return_StaticIPAddress(_ip_address);
		Return_StaticIPSubnet(_subnet_mask);
		Return_StaticIPGateway(_ip_gateway);

		#if (SOULISS_DEBUG)
		// Print debug messages
		SOULISS_LOG(F("(ss)<IP Addr:><0x"));
		for(U8 i=0;i<4;i++)	
		{
			SOULISS_LOG(_ip_address[i],HEX);
			SOULISS_LOG(F("|0x"));
		}
		SOULISS_LOG(">\r\n");

		SOULISS_LOG(F("(ss)<IP Subn:><0x"));
		for(U8 i=0;i<4;i++)	
		{
			SOULISS_LOG(_subnet_mask[i],HEX);
			SOULISS_LOG(F("|0x"));
		}
		SOULISS_LOG(">\r\n");

		SOULISS_LOG(F("(ss)<IP Gtwy:><0x"));
		for(U8 i=0;i<4;i++)	
		{
			SOULISS_LOG(_ip_gateway[i],HEX);
			SOULISS_LOG(F("|0x"));
		}
		SOULISS_LOG(">\r\n");
		#endif

		// Set IP Address
		for(i=0; i<4; i++)
		{
			if(DEFAULT_BASEIPADDRESS) 	DEFAULT_BASEIPADDRESS[i]=_ip_address[i];
			if(DEFAULT_SUBMASK) 		DEFAULT_SUBMASK[i] = _subnet_mask[i];
			if(DEFAULT_GATEWAY) 		DEFAULT_GATEWAY[i] = _ip_gateway[i];
		}
		
		myvNet_dhcp = (U16)_ip_address[i-1];			// The last byte of the IP address is the vNet one
		DEFAULT_BASEIPADDRESS[i-1]=0;						// The BASEIPADDRESS has last byte always zero

		#if((MCU_TYPE == 0x02) || (MCU_TYPE == 0x03))	// Expressif ESP8266 or ESP32

			#if (SOULISS_DEBUG)
			// Print debug messages
			SOULISS_LOG(F("(ss)<Read_SSID>"));
			SOULISS_LOG(WiFi.SSID());	
			SOULISS_LOG("\r\n");

			SOULISS_LOG(F("(ss)<Read_Password>"));
			SOULISS_LOG(WiFi.psk());	
			SOULISS_LOG("\r\n");
			#endif

			// If is the first time that we connect to WiFi.SSID
			if(strcmp(WiFi.SSID().c_str(), SSID.c_str()) || strcmp(WiFi.psk().c_str(), PSW.c_str()))
			{
				WiFi.mode(WIFI_STA);
				WiFi.begin(SSID.c_str(), PSW.c_str());
			}
			else
				WiFi.begin();				// WiFi.SSID is a known network, no need to specify it

			// Connect
			while ((WiFi.status() != WL_CONNECTED) && timeout)
			{
				timeout--;
				delay(500);
			}
		
			// Set manually an IP address
			WiFi.config(_ip_address, _ip_gateway, _subnet_mask);

		#endif
		
		// Set the address
		Souliss_SetAddress(myvNet_dhcp, DYNAMICADDR_SUBNETMASK, 0);
	}

	#if((MCU_TYPE == 0x02) || (MCU_TYPE == 0x03))	// Expressif ESP8266 or ESP32
	if(WiFi.status() != WL_CONNECTED) 
	{
		// Print debug messages
		#if (SOULISS_DEBUG)
		SOULISS_LOG(F("(ss)<WiFi Fail>\r\n"));
		#endif	

		return 0;
	}
	else 
	{
		// Print debug messages
		#if (SOULISS_DEBUG)
		SOULISS_LOG(F("(ss)<WiFi Connected>\r\n"));
		#endif	

		return 1;
	}
	#endif
}
#endif

/**************************************************************************/
/*!
	Define a gateway node as server for dynamic addressing, media without
	an address will get one automatically.
	
	Is allowed to set the IP address manually or via DHCP, that address
	will not be overwritten by this method.
*/	
/**************************************************************************/
void Souliss_SetAddressingServer(U8 *memory_map)
{
	// Assign a node for each media
	U8 i=0;	
	if(vnet_media_en[i] && !vNet_GetAddress(i+1))
	{
		// If the IP address hasn't been assigned manually or via DHCP, flag the addressing server and get the
		// IP configuration from broadcast frames
		addrsrv = true;
	
		// The last byte of the IP address is set, this will match the vNet address of the node
		Souliss_SetAddress(DYNAMICADDR_IPVNETNODE, DYNAMICADDR_SUBNETMASK, DYNAMICADDR_IPGATEWAY);		
		Souliss_SetLocalAddress(memory_map, DYNAMICADDR_IPVNETNODE);
	}

	// Only if an address hasn't been assigned	
	for(i=1; i<VNET_MEDIA_NUMBER; i++)
		if(vnet_media_en[i] && !vNet_GetAddress(i+1))
			Souliss_SetAddress((vnet_addr_l[i] | DYNAMICADDR_GATEWAYNODE), DYNAMICADDR_SUBNETMASK, ((vnet_addr_l[i] & DYNAMICADDR_SUBNETMASK) | DYNAMICADDR_GATEWAY));	
		
	// If previously we got the network addresses, recover them from EEPROM
	#if(USEEEPROM)
	if(Return_ID()==STORE__DEFAULTID)
		Return_PeerAddresses((memory_map + MaCaco_ADDRESSES_s + 2), MaCaco_NODES - 1);
	
		#if (SOULISS_DEBUG)
		// Print debug messages
		SOULISS_LOG(F("(ss)<rPddr>"));
		SOULISS_LOG(F("<|0x"));
		for(i=0;i<MaCaco_NODES;i++)
		{	
			SOULISS_LOG(C8TO16(memory_map + MaCaco_ADDRESSES_s+2*i),HEX);
			SOULISS_LOG(F("|0x"));
		}			
		SOULISS_LOG(">\r\n");
		#endif

	#endif	
}

/**************************************************************************/
/*!
	Set the node to retrieve the address dynamically, this cannot be applied
	for IP based nodes
*/	
/**************************************************************************/
void Souliss_SetDynamicAddressing()
{
	U8 i=0;
	for(i=1; i<VNET_MEDIA_NUMBER; i++)
	{
		U8 media = i + 1;							// Addressing in arrays starts from zero
													// rather media definition from one
		if(vnet_media_en[i])
		{
			// Set configuration general parameters
			if(!FirstInit)
			{
				vNet_Init();											// Init vNet
				FirstInit = true;
			}
			
			vNet_SetAddress(vnet_addr_l[i], media);	// Set vNet Address
			vNet_SetSubnetMask(DYNAMICADDR_SUBNETMASK, media);				// Set vNet Subnetmask
			vNet_SetMySuperNode(0x00, media);								// Set vNet Supernode
		}
	}
}

/**************************************************************************/
/*!
	Before proceed to request and address, at first boot, look for a previously
	assigned address.
*/	
/**************************************************************************/
U8 Souliss_DynamicAddressing_FirstBoot (U8 *memory_map)
{
	#if(USEEEPROM)
	// If in the past the node has got an address, we use it again
	if(STORE__DEFAULTID == Return_ID())
	{
		#if (SOULISS_DEBUG)
		// Print debug messages
		SOULISS_LOG(F("(ss)<sID>"));
		SOULISS_LOG(F("<|0x"));
		SOULISS_LOG(Return_ID(),HEX);
		SOULISS_LOG(">\r\n");
		#endif
		
		for(uint8_t i=1; i<=VNET_MEDIA_NUMBER; i++)
		{
			uint16_t raddr = Return_Addresses(i);
			if((raddr) && (i == vNet_GetMedia(raddr)))
			{
				#if (SOULISS_DEBUG)
				// Print debug messages
				SOULISS_LOG(F("(ss)<rAdd>"));
				SOULISS_LOG(F("<|0x"));
				SOULISS_LOG(raddr,HEX);
				SOULISS_LOG(">\r\n");
				#endif				
				
				Souliss_SetAddress(raddr, DYNAMICADDR_SUBNETMASK, ((raddr & DYNAMICADDR_SUBNETMASK) | DYNAMICADDR_GATEWAY));
			}	
		}	
	}
	#endif
	
	return 0;
}
/**************************************************************************/
/*!
	Request an addressing and parse the answer, need an unique identifier
	id that is used while the node hasn't a valid address.
	
	A compiler macro can be used as id, __TIME__ is a 9 byte that indicate
	the time that compiler started. Isn't an absolute identifier but works
	in most of the cases.
*/	
/**************************************************************************/
U8 Souliss_DynamicAddressing (U8 *memory_map, const char id[], U8 size)
{
	U8 i, usedmedia;			
	
	// If no address is set
	if(vNet_MyMediasWithoutAddress(&usedmedia))
	{
		// Generate a a key identifier, this is the ID of the node
		if(!keyidval)
			for(i=0;i<size;i++)
				keyidval+=(i*i)*id[i];	
		
		// Verify if the addressing information are available in the configuration
		// parameters of the memory map
		
		// The first parameter is the keyidval number used to identify my previous request
		U8 *confparameters_p = (memory_map + MaCaco_QUEUE_s);
		if(C8TO16(confparameters_p) == keyidval)
		{
			// The next parameter is the media
			confparameters_p+=sizeof(U16);
			if(*confparameters_p == usedmedia)
			{
				// Load the address
				confparameters_p++;
				U8 proposedsubnet = C8TO16(confparameters_p)>>8;	// Subnet in case of dynamic address (0xFF00) has only one
																	// byte for subnet identification
				
				// If we got a full address
				if(C8TO16(confparameters_p) & ~DYNAMICADDR_SUBNETMASK)
				{
					Souliss_SetAddress(C8TO16(confparameters_p), DYNAMICADDR_SUBNETMASK, ((C8TO16(confparameters_p) & DYNAMICADDR_SUBNETMASK) | DYNAMICADDR_GATEWAY));

					#if(USEEEPROM)
					// Store the node ID
					Store_ID(STORE__DEFAULTID);
					
					// Store the address
					for(i=1; i<=VNET_MEDIA_NUMBER; i++)
						if(vNet_GetAddress(i))
							Store_Address(vNet_GetAddress(i), i);
					
					Store_Commit();
					
						#if (SOULISS_DEBUG)
						// Print debug messages
						SOULISS_LOG(F("(ss)<sID>"));
						SOULISS_LOG(F("<|0x"));
						SOULISS_LOG(Return_ID(),HEX);
						SOULISS_LOG(">\r\n");
					
						SOULISS_LOG(F("(ss)<sAddr>"));
						SOULISS_LOG(F("<|0x"));
						for(uint8_t j=1; j<=VNET_MEDIA_NUMBER; j++)
						{	
							SOULISS_LOG(Return_Addresses(j),HEX);
							SOULISS_LOG(F("|0x"));
						}			
						SOULISS_LOG(">\r\n");
						#endif
		
					#endif
					
					// Configuration data can be now removed
					for(U8 i=0; i<MaCaco_QUEUELEN; i++)
						*(memory_map + MaCaco_QUEUE_s + i) = 0;
					
					return 0;	// Addressing is complete, we can quit
				}					
				else	// Request an address starting from the actual subnet
					MaCaco_send(VNET_ADDR_BRDC, MaCaco_DINADDRESSREQ, keyidval, proposedsubnet, 0, 0);
								
				return 1;				
				
			}	
		}
		
		// Clear the actual configuration parameters, the addressing server will load there
		// the requested address
		for(i=0; i<MaCaco_QUEUELEN; i++)
			*(memory_map + MaCaco_QUEUE_s + i) = 0;

		// Request a new address (if supernode) or a subnet
		#if(VNET_SUPERNODE)
			MaCaco_send(VNET_ADDR_BRDC, MaCaco_DINADDRESSREQ, keyidval, (0xF0 + usedmedia), 0, 0);
		#else
			MaCaco_send(VNET_ADDR_nBRDC, MaCaco_SUBNETREQ, keyidval, (usedmedia), 0, 0);	// this is a non rebroadcastable frame, so it get till the nearest supernode/bridge
		#endif
		
		return 1;
	}

	return 0;
}

/**************************************************************************/
/*!
	Send a request to join a network, shall be periodically processed by nodes
	that requested a dynamic address
*/	
/**************************************************************************/
void Souliss_JoinNetwork()
{
	// Request to join a network only if I've got an address
	if(vNet_GetAddress(vNet_MyMedia()))
		MaCaco_send(0xFFFF, MaCaco_JOINNETWORK, keyidval, 0, 0, 0);
}

/**************************************************************************/
/*!
	Send a request to join a network and request a reset of the subscription, 
	use carefully only at begin on a join
*/	
/**************************************************************************/
void Souliss_JoinAndReset()
{
	// Request to join a network only if I've got an address
	if(vNet_GetAddress(vNet_MyMedia()))
		MaCaco_send(0xFFFF, MaCaco_JOINANDRESET, keyidval, 0, 0, 0);
}

