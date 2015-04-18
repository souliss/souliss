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
#ifndef STORE_H
#define STORE_H

#include <stdbool.h>
#include <stdint.h>
#include "tools/types.h"

#define	STORE__DEFAULTID	0xABBA											
#define STORE__INDEX		0												// The first byte allocated in the EEPROM

#define STORE__ID_s			0												// Identifier (2 bytes)
#define	STORE__ADDR_s		2												// Address M1 to M5 (2 bytes per address)
#define	STORE__ADDR_f		(STORE__ADDR_s+2*MEDIA_NUMBER-1)				// Address M1 to M5 (2 bytes per address)
#define	STORE__PADDR_s		STORE__ADDR_f									// First peer address (for gateway)
#define STORE__PADDR_f		(STORE__PADDR_s+2*MaCaco_NODES-1)				// Last peer address  (for gateway)

#if(DYNAMICADDRESSING)
#	define	FIRST_EEPROM_BYTE	(STORE__INDEX+STORE__PADDR_f+1)
#else
#	define	FIRST_EEPROM_BYTE	(STORE__INDEX+1)	
#endif

#define	Store_Init()		{}			

#define User_Store_8bit(addr,val)	Store_8bit(FIRST_EEPROM_BYTE+addr,val)	
#define User_Return_8bit(addr)		Return_8bit(FIRST_EEPROM_BYTE+addr)
#define User_Store_16bit(addr,val)	Store_16bit(FIRST_EEPROM_BYTE+addr,val)	
#define User_Return_16bit(addr)		Return_16bit(FIRST_EEPROM_BYTE+addr)

// Remove the ID, this identify a not initialized EEPROM
void Store_Clear()
{
	EEPROM.write(STORE__INDEX+STORE__ID_s  , 0);
	EEPROM.write(STORE__INDEX+STORE__ID_s+1, 0);	
}

void Store_8bit(uint8_t addr, uint16_t store_val)
{	
	EEPROM.update(STORE__INDEX+addr, store_val);
}

uint16_t Return_8bit(uint8_t addr)
{
	return EEPROM.read(STORE__INDEX+addr);
}

void Store_16bit(uint8_t addr, uint16_t store_val)
{	
	// Point the ID as a byte
	uint8_t *val = (uint8_t*)(&store_val);

	EEPROM.update(STORE__INDEX+addr, *val++);
	EEPROM.update(STORE__INDEX+addr+1, *val);
}

uint16_t Return_16bit(uint8_t addr)
{
	uint16_t store_val=0;
	uint8_t *val=(uint8_t*)&store_val;

	*val++ = EEPROM.read(STORE__INDEX+addr);
	*val = EEPROM.read(STORE__INDEX+addr+1);

	return store_val;
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
uint8_t Return_Addresses(uint8_t media)
{
	return Return_16bit(STORE__ADDR_s+2*(media-1));
}

// Store all the peer addresses
void Store_PeerAddresses(uint16_t *addresses, uint8_t n_addresses)
{
	for(uint8_t i=0; i<n_addresses; i++)
		Store_16bit(STORE__ADDR_s+2*i, addresses[i]);
}

// Store all the peer addresses
void Return_PeerAddresses(uint16_t *addresses, uint8_t n_addresses)
{
	for(uint8_t i=0; i<n_addresses; i++)
		addresses[i] = Return_16bit(STORE__ADDR_s+2*i);
}

#endif