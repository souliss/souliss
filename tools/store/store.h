/**************************************************************************
	Souliss

	Modified by Dario Di Maio
	
***************************************************************************/

#ifndef STORE_H
#define STORE_H

#include "tools/types.h"

#define	STORE__DEFAULTID	0xABCD
						
#define STORE__INDEX		0												// The first byte allocated in the EEPROM
#define STORE_USERSPACE		40												// Bytes reserver for users

#define STORE__ID_s			0												// Identifier (2 bytes)
#define	STORE__ADDR_s		2												// Address M1 to M5 (2 bytes per address)
#define	STORE__ADDR_f		(STORE__ADDR_s+2*VNET_MEDIA_NUMBER-1)			// Address M1 to M5 (2 bytes per address)
#define	STORE__PADDR_s		(STORE__ADDR_f+1)								// First peer address (for gateway)
#define STORE__PADDR_f		(STORE__PADDR_s+2*MaCaco_NODES-1)				// Last peer address  (for gateway)
#define	STORE__GATEWAY_s	(STORE__PADDR_f+1)								// Gateway/Peer mode
#define	STORE__GATEWAY_f	(STORE__GATEWAY_s+1)
#define	STORE__DHCPEN_s		(STORE__GATEWAY_f+1)							// DHCP Enabled / Disabled
#define	STORE__DHCPEN_f		(STORE__DHCPEN_s+1)
#define	STORE__IPADDR_s		(STORE__DHCPEN_f+1)								// IPv4 Address, Subnet mask and IPv4 Gateway
#define	STORE__IPADDR_f		(STORE__IPADDR_s+4)
#define	STORE__IPSUBN_s		(STORE__IPADDR_f+1)								// IPv4 Address, Subnet mask and IPv4 Gateway
#define	STORE__IPSUBN_f		(STORE__IPSUBN_s+4)
#define	STORE__IPGTWY_s		(STORE__IPSUBN_f+1)								// IPv4 Address, Subnet mask and IPv4 Gateway
#define	STORE__IPGTWY_f		(STORE__IPGTWY_s+4)
#define	STORE__WIFISSID_s	(STORE__IPGTWY_f+1)								// WiFi SSID
#define	STORE__WIFISSID_f	(STORE__WIFISSID_s+32)
#define	STORE__WIFIPSW_s	(STORE__WIFISSID_f+1)							// WiFi Password
#define	STORE__WIFIPSW_f	(STORE__WIFIPSW_s+64)
#define STORE__USERMODE_s	(STORE__WIFIPSW_f+1)							// User Mode Addresses
#define STORE__USERMODE_f	(STORE__USERMODE_s+UMODE_USERS*2)
#define STORE__USERSPACE_s	(STORE__USERMODE_f+1)
#define STORE__USERSPACE_f	(STORE__USERSPACE_s+STORE_USERSPACE)
#define	STORE__USABLESIZE	(STORE__USERSPACE_f+1)
#define	STORE__SIZE			(STORE__INDEX+STORE__USABLESIZE)

#if(DYNAMICADDRESSING)
#	define	FIRST_EEPROM_BYTE	(STORE__INDEX+STORE__USERSPACE_s)
#else
#	define	FIRST_EEPROM_BYTE	(STORE__INDEX+1)	
#endif

#define User_Store_8bit(addr,val)	Store_8bit(FIRST_EEPROM_BYTE+addr,val)	
#define User_Return_8bit(addr)		Return_8bit(FIRST_EEPROM_BYTE+addr)
#define User_Store_16bit(addr,val)	Store_16bit(FIRST_EEPROM_BYTE+addr,val)	
#define User_Return_16bit(addr)		Return_16bit(FIRST_EEPROM_BYTE+addr)

void Store_Clear();

//--void Store_8bit(uint8_t addr, uint8_t store_val);
//--uint16_t Return_8bit(uint8_t addr);
//--void Store_16bit(uint8_t addr, uint16_t store_val);
//--uint16_t Return_16bit(uint8_t addr);
void Store_8bit(int addr, uint8_t store_val);
uint16_t Return_8bit(int addr);
void Store_16bit(int addr, uint16_t store_val);
uint16_t Return_16bit(int addr);

void Store_ID(uint16_t id);
uint16_t Return_ID();
void Store_Address(uint16_t address, uint8_t media);
uint16_t Return_Addresses(uint8_t media);
void Store_PeerAddresses(uint8_t *addresses, uint8_t n_addresses);
void Return_PeerAddresses(uint8_t *addresses, uint8_t n_addresses);
void Store_UserModeAddresses(uint16_t *addresses, uint8_t n_addresses);
void Return_UserModeAddresses(uint16_t *addresses, uint8_t n_addresses);
//--uint16_t Return_SinglePeerAddresses(uint8_t n_addr);
uint16_t Return_SinglePeerAddresses(int n_addr);
void Store_GatewayMode(uint8_t mode);
uint8_t Return_GatewayMode();
void Store_DHCPMode(uint8_t mode);
uint8_t Return_DHCPMode();
void Store_StaticIPAddress(uint8_t *ipaddr);
void Return_StaticIPAddress(uint8_t *ipaddr);
void Store_StaticIPSubnet(uint8_t *subnetmask);
void Return_StaticIPSubnet(uint8_t *subnetmask);
void Store_StaticIPGateway(uint8_t *gateway);
void Return_StaticIPGateway(uint8_t *gateway);
void Store_SSID(String string);
String Read_SSID();
void Store_Password(String string);
String Read_Password();
void Store_Commit();

#endif
