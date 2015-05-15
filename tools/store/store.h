/**************************************************************************
	Souliss

	Modified by Dario Di Maio
	
***************************************************************************/

#ifndef SSSTORE_H
#define SSSTORE_H

#if(MCU_TYPE == 0x01)	// Atmel AVR Atmega
#	include "tools/store/mcu_avr/store.h"
#elif(MCU_TYPE == 0x02)	// Expressif ESP8266
#endif

#endif
