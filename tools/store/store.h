/**************************************************************************
	Souliss

	Modified by Dario Di Maio
	
***************************************************************************/

#ifndef STORE_H
#define STORE_H

#include "tools/types.h"

#define	STORE__DEFAULTID	0xABBA											
#define STORE__INDEX		0												// The first byte allocated in the EEPROM

#define STORE__ID_s			0												// Identifier (2 bytes)
#define	STORE__ADDR_s		2												// Address M1 to M5 (2 bytes per address)
#define	STORE__ADDR_f		(STORE__ADDR_s+2*VNET_MEDIA_NUMBER-1)			// Address M1 to M5 (2 bytes per address)
#define	STORE__PADDR_s		(STORE__ADDR_f+1)								// First peer address (for gateway)
#define STORE__PADDR_f		(STORE__PADDR_s+2*MaCaco_NODES-1)				// Last peer address  (for gateway)
#define	STORE__USABLESIZE	(STORE__PADDR_f)
#define	STORE__SIZE			(STORE__INDEX+STORE__PADDR_f)

#if(DYNAMICADDRESSING)
#	define	FIRST_EEPROM_BYTE	(STORE__INDEX+STORE__PADDR_f+1)
#else
#	define	FIRST_EEPROM_BYTE	(STORE__INDEX+1)	
#endif

#define User_Store_8bit(addr,val)	Store_8bit(FIRST_EEPROM_BYTE+addr,val)	
#define User_Return_8bit(addr)		Return_8bit(FIRST_EEPROM_BYTE+addr)
#define User_Store_16bit(addr,val)	Store_16bit(FIRST_EEPROM_BYTE+addr,val)	
#define User_Return_16bit(addr)		Return_16bit(FIRST_EEPROM_BYTE+addr)

void Store_Clear();
void Store_8bit(uint8_t addr, uint8_t store_val);
uint16_t Return_8bit(uint8_t addr);
void Store_16bit(uint8_t addr, uint16_t store_val);
uint16_t Return_16bit(uint8_t addr);
void Store_ID(uint16_t id);
uint16_t Return_ID();
void Store_Address(uint16_t address, uint8_t media);
uint16_t Return_Addresses(uint8_t media);
void Store_PeerAddresses(uint8_t *addresses, uint8_t n_addresses);
void Return_PeerAddresses(uint8_t *addresses, uint8_t n_addresses);
uint16_t Return_SinglePeerAddresses(uint8_t n_addr);
void Store_Commit();

#endif
