/**************************************************************************
	Souliss

	Modified by Dario Di Maio
	Modified by Dario Cdj for Notify Functions
	
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
#define STORE__PUSHETTAAPIKEY_s (STORE__USERSPACE_f+1)
#define STORE__PUSHETTAAPIKEY_f (STORE__PUSHETTAAPIKEY_s+40)
#define STORE__PUSHETTACHANNEL_s (STORE__PUSHETTAAPIKEY_f+1)
#define STORE__PUSHETTACHANNEL_f (STORE__PUSHETTACHANNEL_s+20)
#define	STORE__PUSHETTAENABLED_s	(STORE__PUSHETTACHANNEL_f+1)								
#define	STORE__PUSHETTAENABLED_f	(STORE__PUSHETTAENABLED_s+1)
#define STORE__PUSHOVERAPITOKEN_s (STORE__PUSHETTAENABLED_f+1)
#define STORE__PUSHOVERAPITOKEN_f (STORE__PUSHOVERAPITOKEN_s+30)
#define STORE__PUSHOVERUSERKEY_s (STORE__PUSHOVERAPITOKEN_f+1)
#define STORE__PUSHOVERUSERKEY_f (STORE__PUSHOVERUSERKEY_s+30)
#define	STORE__PUSHOVERDEVICE_s	(STORE__PUSHOVERUSERKEY_f+1)								
#define	STORE__PUSHOVERDEVICE_f	(STORE__PUSHOVERDEVICE_s+20)
#define	STORE__PUSHOVERSOUND_s	(STORE__PUSHOVERDEVICE_f+1)								
#define	STORE__PUSHOVERSOUND_f	(STORE__PUSHOVERSOUND_s+20)
#define	STORE__PUSHOVERENABLED_s	(STORE__PUSHOVERSOUND_f+1)								
#define	STORE__PUSHOVERENABLED_f	(STORE__PUSHOVERENABLED_s+1)
#define STORE__TELEGRAMBOTTOKEN_s (STORE__PUSHOVERENABLED_f+1)
#define STORE__TELEGRAMBOTTOKEN_f (STORE__TELEGRAMBOTTOKEN_s+45)
#define STORE__TELEGRAMCHATGROUP_s (STORE__TELEGRAMBOTTOKEN_f+1)
#define STORE__TELEGRAMCHATGROUP_f (STORE__TELEGRAMCHATGROUP_s+20)
#define	STORE__TELEGRAMCHATID_s	(STORE__TELEGRAMCHATGROUP_f+1)								
#define	STORE__TELEGRAMCHATID_f	(STORE__TELEGRAMCHATID_s+20)
#define	STORE__TELEGRAMENABLED_s	(STORE__TELEGRAMCHATID_f+1)								
#define	STORE__TELEGRAMENABLED_f	(STORE__TELEGRAMENABLED_s+1)
#define	STORE__NOTIFYMESSAGE_s	(STORE__TELEGRAMENABLED_f+1)								
#define	STORE__NOTIFYMESSAGE_f	(STORE__NOTIFYMESSAGE_s+40)
#define	STORE__SOULISSVNETADDRESS_s	(STORE__NOTIFYMESSAGE_f+1)								
#define	STORE__SOULISSVNETADDRESS_f	(STORE__SOULISSVNETADDRESS_s+2) // 2 bytes
#define	STORE__SOULISSVNETGATEWAY_s	(STORE__SOULISSVNETADDRESS_f+1)								
#define	STORE__SOULISSVNETGATEWAY_f	(STORE__SOULISSVNETGATEWAY_s+2) // 2 bytes
#define	STORE__DELETESUBSCRIPTION_s	(STORE__SOULISSVNETGATEWAY_f+1)								
#define	STORE__DELETESUBSCRIPTION_f	(STORE__DELETESUBSCRIPTION_s+1)
#define	STORE__NODENAME_s	(STORE__DELETESUBSCRIPTION_f+1)								
#define	STORE__NODENAME_f	(STORE__NODENAME_s+30)
#define	STORE__USABLESIZE	(STORE__NODENAME_f+1)
#define	STORE__SIZE			(STORE__INDEX+STORE__USABLESIZE)

#if(DYNAMICADDRESSING)
#	define	FIRST_EEPROM_BYTE	(STORE__INDEX+STORE__DELETESUBSCRIPTION_s)
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
void Store_Pushetta_ApiKey(String string);
String Read_Pushetta_ApiKey();
void Store_Pushetta_Channel(String string);
String Read_Pushetta_Channel();
void Store_PushettaEnabled(uint8_t mode);
uint8_t Return_PushettaEnabled();
void Store_Pushover_ApiToken(String string);
String Read_Pushover_ApiToken();
void Store_Pushover_UserKey(String string);
String Read_Pushover_UserKey();
void Store_Pushover_Device(String string);
String Read_Pushover_Device();
void Store_Pushover_Sound(String string);
String Read_Pushover_Sound();
void Store_PushoverEnabled(uint8_t mode);
uint8_t Return_PushoverEnabled();
void Store_Telegram_BOTtoken(String string);
String Read_Telegram_BOTtoken();
void Store_Telegram_ChatGroup(String string);
String Read_Telegram_ChatGroup();
void Store_Telegram_ChatID(String string);
String Read_Telegram_ChatID();
void Store_TelegramEnabled(uint8_t mode);
uint8_t Return_TelegramEnabled();
void Store_NodeName(String string);
String Read_NodeName();
void Store_NotifyMessage(String string);
String Read_NotifyMessage();
void Store_SoulissVNETAddress(uint16_t value);
uint16_t Read_SoulissVNETAddress();
void Store_SoulissVNETGateway(uint16_t value);
uint16_t Read_SoulissVNETGateway();
void Store_DeleteSubscription(uint8_t mode);
uint8_t Return_DeleteSubscription();

void Store_Commit();

#endif
