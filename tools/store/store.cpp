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
	#elif(MCU_TYPE == 0x02)	// Expressif ESP8266
		EEPROM.begin(STORE__SIZE);
	#endif	
}	

// Remove the ID, this identify a not initialized EEPROM
void Store_Clear()
{
#if(MCU_TYPE == 0x01)	// Atmel AVR Atmega	
	for(uint8_t i=0;i<EEPROM.length();i++)
		EEPROM.update(i, 0);
#elif(MCU_TYPE == 0x02)	// Expressif ESP8266
	for(uint8_t i=0;i<STORE__USABLESIZE;i++)
		EEPROM.write(i, 0);
#endif	
}

void Store_8bit(uint8_t addr, uint8_t store_val)
{	
#if(MCU_TYPE == 0x01)	// Atmel AVR Atmega	
	EEPROM.update(STORE__INDEX+addr, store_val);
#elif(MCU_TYPE == 0x02)	// Expressif ESP8266
	EEPROM.write(STORE__INDEX+addr, store_val);	
#endif		
}

uint16_t Return_8bit(uint8_t addr)
{
	return EEPROM.read(STORE__INDEX+addr);
}

void Store_16bit(uint8_t addr, uint16_t store_val)
{	
#if(MCU_TYPE == 0x01)	// Atmel AVR Atmega	
	EEPROM.update(STORE__INDEX+addr,   C16TO8L(store_val));
	EEPROM.update(STORE__INDEX+addr+1, C16TO8H(store_val));
#elif(MCU_TYPE == 0x02)	// Expressif ESP8266
	EEPROM.write(STORE__INDEX+addr,   C16TO8L(store_val));
	EEPROM.write(STORE__INDEX+addr+1, C16TO8H(store_val));
#endif		
}

uint16_t Return_16bit(uint8_t addr)
{
	uint8_t val_L = EEPROM.read(STORE__INDEX+addr);
	uint8_t val_H = EEPROM.read(STORE__INDEX+addr+1);

	return ((U16)(val_H << 8) | (U16)(val_L));
}

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
	Store_16bit(STORE__ADDR_s+2*(media-1), address);
}

// Return a vNet address
uint16_t Return_Addresses(uint8_t media)
{
	return Return_16bit(STORE__ADDR_s+2*(media-1));
}

// Store all the peer addresses
void Store_PeerAddresses(uint8_t *addresses, uint8_t n_addresses)
{
	for(uint8_t i=0; i<n_addresses; i++)
		Store_16bit(STORE__PADDR_s+2*i, C8TO16(addresses+i));
}

// Return all the peer addresses
void Return_PeerAddresses(uint8_t *addresses, uint8_t n_addresses)
{
	for(uint8_t i=0; i<n_addresses; i++)
	{
		addresses[i]   = C16TO8L(Return_16bit(STORE__PADDR_s+2*i));
		addresses[i+1] = C16TO8H(Return_16bit(STORE__PADDR_s+2*i));
	}
		
}

// Return single the peer addresses
uint16_t Return_SinglePeerAddresses(uint8_t n_addr)
{
	return Return_16bit(STORE__PADDR_s+2*n_addr);
}

// Commit to EEPROM
void Store_Commit()
{
	#if(MCU_TYPE == 0x01)	// Atmel AVR Atmega	
	#elif(MCU_TYPE == 0x02)	// Expressif ESP8266
	EEPROM.commit();	
	#endif
}