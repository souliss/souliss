/**************************************************************************
	Souliss

	Modified by Dario Di Maio
	
***************************************************************************/

#ifndef FLOAT16_H
#define FLOAT16_H

void Store_Clear();
void Store_8bit(uint8_t addr, uint16_t store_val);
uint16_t Return_8bit(uint8_t addr);
void Store_16bit(uint8_t addr, uint16_t store_val);
uint16_t Return_16bit(uint8_t addr);
void Store_ID(uint16_t id);
uint16_t Return_ID();
void Store_Address(unit16_t address, uint8_t media);
uint16_t Return_Addresses(uint8_t media);
void Store_PeerAddresses(uint16_t *addresses, uint8_t n_addresses);
void Return_PeerAddresses(uint16_t *addresses, uint8_t n_addresses);
uint16_t Return_SinglePeerAddresses(uint8_t n_addr);

#endif
