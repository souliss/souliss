/**************************************************************************
	Souliss - EEPROM Store and Read
    Copyright (C) 2015  Veseo

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
	Modified by Dario Cdj for Notify Functions
	
***************************************************************************/
/*!
    \file 
    \ingroup

*/
/**************************************************************************/
#include <stdbool.h>
#include <stdint.h>
#include "tools/types.h"
#include "tools/store/store.h"

// Init the use EEPROM
void Store_Init()
{
	#if(MCU_TYPE == 0x01)	// Atmel AVR Atmega
	#elif((MCU_TYPE == 0x02) || (MCU_TYPE == 0x03))	// Expressif ESP8266 or ESP32
		EEPROM.begin(STORE__SIZE);
	#endif	
}	

// Remove the ID, this identify a not initialized EEPROM
void Store_Clear()
{
#if(MCU_TYPE == 0x01)	// Atmel AVR Atmega	
	for(uint16_t i=0;i<EEPROM.length();i++)
		//--EEPROM.update(i, 0);
    EEPROM.update(STORE__INDEX+i, 0);
#elif((MCU_TYPE == 0x02) || (MCU_TYPE == 0x03))	// Expressif ESP8266 or ESP32
	for(uint16_t i=0;i<STORE__USABLESIZE;i++)
		//--EEPROM.write(i, 0);
    EEPROM.write(STORE__INDEX+i, 0);
#endif	
}

//--void Store_8bit(uint8_t addr, uint8_t store_val)
void Store_8bit(int addr, uint8_t store_val)
{	
#if(MCU_TYPE == 0x01)	// Atmel AVR Atmega	
	EEPROM.update(STORE__INDEX+addr, store_val);
#elif((MCU_TYPE == 0x02) || (MCU_TYPE == 0x03))	// Expressif ESP8266 or ESP32
	EEPROM.write(STORE__INDEX+addr, store_val);	
#endif		
}

//--uint16_t Return_8bit(uint8_t addr)
uint16_t Return_8bit(int addr)
{
	return EEPROM.read(STORE__INDEX+addr);
}

//--void Store_16bit(uint8_t addr, uint16_t store_val)
void Store_16bit(int addr, uint16_t store_val)
{	
#if(MCU_TYPE == 0x01)	// Atmel AVR Atmega	
	EEPROM.update(STORE__INDEX+addr,   C16TO8L(store_val));
	EEPROM.update(STORE__INDEX+addr+1, C16TO8H(store_val));
#elif((MCU_TYPE == 0x02) || (MCU_TYPE == 0x03))	// Expressif ESP8266 or ESP32
	EEPROM.write(STORE__INDEX+addr,   C16TO8L(store_val));
	EEPROM.write(STORE__INDEX+addr+1, C16TO8H(store_val));
#endif		
}

//--uint16_t Return_16bit(uint8_t addr)
uint16_t Return_16bit(int addr)
{
	uint8_t val_L = EEPROM.read(STORE__INDEX+addr);
	uint8_t val_H = EEPROM.read(STORE__INDEX+addr+1);

	return ((U16)(val_H << 8) | (U16)(val_L));
}

//--void Store_String(uint8_t addr, String string)
void Store_String(int addr, String string)
{
	char  charBuf[string.length()+1];
	string.toCharArray(charBuf, string.length()+1);

	for (int t=  0; t<sizeof(charBuf);t++)
	{
		EEPROM.write(addr + t,charBuf[t]);
	}
}

//--String Return_String(uint8_t addr, uint8_t maxlenght)
String Return_String(int addr, uint8_t maxlenght)
{
	byte counter=0;
	char rChar;
	String retString = "";

	while (counter < maxlenght)
	{
		rChar = EEPROM.read(addr + counter);
		if (rChar == 0) break;
		if (counter > 51) break;
		counter++;
		retString.concat(rChar);

	}
	return retString;
}

/***** User Space *****/


// Store the node ID, a valid node ID identify an EEPROM with proper values
void Store_ID(uint16_t id)
{
	Store_16bit(STORE__ID_s, id);
}

// Read the node ID, a valid node ID identify an EEPROM with proper values
uint16_t Return_ID()
{
	return Return_16bit(STORE__ID_s);
}

// Store a vNet address
void Store_Address(uint16_t address, uint8_t media)
{
	//--Store_16bit(STORE__ADDR_s+2*(media-1), address);
  Store_16bit(STORE__ADDR_s+2*(int)(media-1), address);
}

// Return a vNet address
uint16_t Return_Addresses(uint8_t media)
{
	//--return Return_16bit(STORE__ADDR_s+2*(media-1));
  return Return_16bit(STORE__ADDR_s+2*(int)(media-1));
}

// Store all the peer addresses
void Store_PeerAddresses(uint8_t *addresses, uint8_t n_addresses)
{
	//--for(uint8_t i=0; i<n_addresses; i++)
  for(int i=0; i<n_addresses; i++)
		Store_16bit(STORE__PADDR_s+2*i, C8TO16(addresses+i));
}

// Return all the peer addresses
void Return_PeerAddresses(uint8_t *addresses, uint8_t n_addresses)
{
	//--for(uint8_t i=0; i<n_addresses; i++)
  for(int i=0; i<n_addresses; i++)
	{
		addresses[i]   = C16TO8L(Return_16bit(STORE__PADDR_s+2*i));
		addresses[i+1] = C16TO8H(Return_16bit(STORE__PADDR_s+2*i));
	}
		
}

// Store all the UserMode addresses
void Store_UserModeAddresses(uint16_t *addresses, uint8_t n_addresses)
{
	//--for(uint8_t i=0; i<n_addresses; i++)
  for(int i=0; i<n_addresses; i++)
		Store_16bit(STORE__USERMODE_s+2*i, *(addresses+i));
}

// Return all the UserMode addresses
void Return_UserModeAddresses(uint16_t *addresses, uint8_t n_addresses)
{
	//for(uint8_t i=0; i<n_addresses; i++)
  for(int i=0; i<n_addresses; i++)
	{
		addresses[i]   = Return_16bit(STORE__USERMODE_s+2*i);
	}
		
}

// Return single the peer addresses
//--uint16_t Return_SinglePeerAddresses(uint8_t n_addr)
uint16_t Return_SinglePeerAddresses(int n_addr)
{
  return Return_16bit(STORE__PADDR_s+2*n_addr);
}

// Store the Gateway mode (Gateway if true, otherwise Peer)
void Store_GatewayMode(uint8_t mode)
{
	Store_8bit(STORE__GATEWAY_s, mode);
}

// Read the Gateway mode (Gateway if true, otherwise Peer)
uint8_t Return_GatewayMode()
{
	return Return_8bit(STORE__GATEWAY_s);
}

// Store the DHCP mode (DHCP if true, otherwise static address)
void Store_DHCPMode(uint8_t mode)
{
	Store_8bit(STORE__DHCPEN_s, mode);
}

// Read the DHCP mode (DHCP if true, otherwise static address)
uint8_t Return_DHCPMode()
{
	return Return_8bit(STORE__DHCPEN_s);
}

// Store the static IP address
void Store_StaticIPAddress(uint8_t *ipaddr)
{
	for(uint8_t i=0; i<4; i++)
		//--Store_8bit(STORE__IPADDR_s+i, *(ipaddr+i));   
    Store_8bit(STORE__IPADDR_s+(int)i, *(ipaddr+i));
}

// Return the static IP address
void Return_StaticIPAddress(uint8_t *ipaddr)
{
	//--for(uint8_t i=0; i<4; i++)
  for(int i=0; i<4; i++)
		ipaddr[i]   = Return_8bit(STORE__IPADDR_s+i);
}

// Store the static IP subnet
void Store_StaticIPSubnet(uint8_t *subnetmask)
{
	for(uint8_t i=0; i<4; i++)
		//--Store_8bit(STORE__IPSUBN_s+i, *(subnetmask+i));
    Store_8bit(STORE__IPSUBN_s+(int)i, *(subnetmask+i));
}

// Return the static IP subnet
void Return_StaticIPSubnet(uint8_t *subnetmask)
{
	//--for(uint8_t i=0; i<4; i++)
  for(int i=0; i<4; i++)
		subnetmask[i]   = Return_8bit(STORE__IPSUBN_s+i);
}

// Store the static IP gateway
void Store_StaticIPGateway(uint8_t *gateway)
{
	for(uint8_t i=0; i<4; i++)
		//--Store_8bit(STORE__IPGTWY_s+i, *(gateway+i));
    Store_8bit(STORE__IPGTWY_s+(int)i, *(gateway+i));
}

// Return the static IP gateway
void Return_StaticIPGateway(uint8_t *gateway)
{
	//--for(uint8_t i=0; i<4; i++)
  for(int i=0; i<4; i++)
		gateway[i]   = Return_8bit(STORE__IPGTWY_s+i);
}

// Store the SSID
void Store_SSID(String string)
{
	Store_String(STORE__WIFISSID_s, string);
}

// Return the SSID
String Read_SSID()
{
	return Return_String(STORE__WIFISSID_s, 32);
}

// Store the Password
void Store_Password(String string)
{
	Store_String(STORE__WIFIPSW_s, string);
}

// Return the Password
String Read_Password()
{
	return Return_String(STORE__WIFIPSW_s, 64);
}

// Store the Pushetta APIKEY
void Store_Pushetta_ApiKey(String string)
{
	Store_String(STORE__PUSHETTAAPIKEY_s, string);
}
// Return the Pushetta APIKEY
String Read_Pushetta_ApiKey()
{
	return Return_String(STORE__PUSHETTAAPIKEY_s, 40);
}
// Store the Pushetta CHANNEL
void Store_Pushetta_Channel(String string)
{
	Store_String(STORE__PUSHETTACHANNEL_s, string);
}

// Return the Pushetta CHANNEL
String Read_Pushetta_Channel()
{
	return Return_String(STORE__PUSHETTACHANNEL_s, 20);
}

// Store the Pushetta Enabled 
void Store_PushettaEnabled(uint8_t mode)
{
	Store_8bit(STORE__PUSHETTAENABLED_s, mode);
}

// Read the Pushetta Enabled (If Enabled or not)
uint8_t Return_PushettaEnabled()
{
	return Return_8bit(STORE__PUSHETTAENABLED_s);
}

// Store the Pushover APITOKEN
void Store_Pushover_ApiToken(String string)
{
	Store_String(STORE__PUSHOVERAPITOKEN_s, string);
}
// Return the Pushover APIKEY
String Read_Pushover_ApiToken()
{
	return Return_String(STORE__PUSHOVERAPITOKEN_s, 30);
}

// Store the Pushover USERKEY
void Store_Pushover_UserKey(String string)
{
	Store_String(STORE__PUSHOVERUSERKEY_s, string);
}
// Return the Pushover USERKEY
String Read_Pushover_UserKey()
{
	return Return_String(STORE__PUSHOVERUSERKEY_s, 30);
}

// Store the Pushover DEVICE
void Store_Pushover_Device(String string)
{
	Store_String(STORE__PUSHOVERDEVICE_s, string);
}

// Return the Pushover DEVICE
String Read_Pushover_Device()
{
	return Return_String(STORE__PUSHOVERDEVICE_s, 20);
}
// Store the Pushover SOUND
void Store_Pushover_Sound(String string)
{
	Store_String(STORE__PUSHOVERSOUND_s, string);
}

// Return the Pushover SOUND
String Read_Pushover_Sound()
{
	return Return_String(STORE__PUSHOVERSOUND_s, 20);
}

// Store the Pushover Enabled 
void Store_PushoverEnabled(uint8_t mode)
{
	Store_8bit(STORE__PUSHOVERENABLED_s, mode);
}

// Read the Pushover Enabled (If Enabled or not)
uint8_t Return_PushoverEnabled()
{
	return Return_8bit(STORE__PUSHOVERENABLED_s);
}

// Store the Telegram BOT TOKEN
void Store_Telegram_BOTtoken(String string)
{
	Store_String(STORE__TELEGRAMBOTTOKEN_s, string);
}
// Return the Telegram BOT TOKEN
String Read_Telegram_BOTtoken()
{
	return Return_String(STORE__TELEGRAMBOTTOKEN_s, 45);
}

// Store the Telegram CHAT GROUP
void Store_Telegram_ChatGroup(String string)
{
	Store_String(STORE__TELEGRAMCHATGROUP_s, string);
}
// Return the Telegram CHAT GROUP
String Read_Telegram_ChatGroup()
{
	return Return_String(STORE__TELEGRAMCHATGROUP_s, 20);
}

// Store the Telegram CHAT ID
void Store_Telegram_ChatID(String string)
{
	Store_String(STORE__TELEGRAMCHATID_s, string);
}

// Return the Telegram CHAT ID
String Read_Telegram_ChatID()
{
	return Return_String(STORE__TELEGRAMCHATID_s, 20);
}

// Store the Telegram Enabled 
void Store_TelegramEnabled(uint8_t mode)
{
	Store_8bit(STORE__TELEGRAMENABLED_s, mode);
}

// Read the Telegram Enabled (If Enabled or not)
uint8_t Return_TelegramEnabled()
{
	return Return_8bit(STORE__TELEGRAMENABLED_s);
}
// Store the Notify Message
void Store_NotifyMessage(String string)
{
	Store_String(STORE__NOTIFYMESSAGE_s, string);
}

// Return the Notify Message
String Read_NotifyMessage()
{
	return Return_String(STORE__NOTIFYMESSAGE_s, 40);
}

// Store the Souliss Address for Hardcoded Peer
void Store_SoulissVNETAddress(uint16_t address)
{
	//Store_16bit(STORE__SOULISSVNETADDRESS_s, address+2);
	Store_16bit(STORE__SOULISSVNETADDRESS_s, address);
}

// Return the Souliss Address for Hardcoded Peer
uint16_t Read_SoulissVNETAddress()
{
	return Return_16bit(STORE__SOULISSVNETADDRESS_s);
}

// Store the Souliss Address for Hardcoded Peer
void Store_SoulissVNETGateway(uint16_t address)
{
	//Store_16bit(STORE__SOULISSVNETGATEWAY_s, address+2);
	Store_16bit(STORE__SOULISSVNETGATEWAY_s, address);
}

// Return the Souliss Address for Hardcoded Peer
uint16_t Read_SoulissVNETGateway()
{
	return Return_16bit(STORE__SOULISSVNETGATEWAY_s);
}

// Store the Delete Subscription Flag  
void Store_DeleteSubscription(uint8_t mode)
{
	Store_8bit(STORE__DELETESUBSCRIPTION_s, mode);
}

// Read the Delete Subscription (If Enabled or not)
uint8_t Return_DeleteSubscription()
{
	return Return_8bit(STORE__DELETESUBSCRIPTION_s);
}

// Commit to EEPROM
void Store_Commit()
{
	#if(MCU_TYPE == 0x01)	// Atmel AVR Atmega	
	#elif((MCU_TYPE == 0x02) || (MCU_TYPE == 0x03))	// Expressif ESP8266 or ESP32
	EEPROM.commit();	
	#endif
}