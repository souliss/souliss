/**************************************************************************
	Souliss 
    Copyright (C) 2013  Veseo

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

#ifndef MODIO_H
#define MODIO_H

#include "src/types.h"
#include "src/I2C/I2C.h"
#include "Olimex.h"

// Functional Codes for I2C Communication to MOD-IO
#define	SET_TRIS	0x01
#define SET_LAT		0x02
#define	GET_PORT	0x03
#define SET_PU		0x04
#define GET_AN0		0x10
#define GET_AN1		0x11
#define GET_AN2		0x12
#define GET_AN3		0x13
#define GET_AN7		0x17
#define SET_REL		0x40
#define	RELAY1		0x01
#define	RELAY2		0x02
#define	RELAY3		0x04
#define	RELAY4		0x08
#define	GPIO0		0x01
#define	GPIO1		0x02
#define	GPIO2		0x04
#define	GPIO3		0x08
#define	GPIO4		0x10
#define	GPIO5		0x20
#define	GPIO6		0x40

#define mIO_SetAddr0(addr0)			mIO_cmd[ADDR0] = addr0
#define mIO_SetAddr1(addr1)			mIO_cmd[ADDR1] = addr1
#define mIO_SetAddr2(addr2)			mIO_cmd[ADDR2] = addr2

void MODIO_Init();
void mIO_PinMode(uint8_t gpio_register);
void mIO_Pullup(uint8_t gpio_register);
void mIO_Relay(uint8_t relay_reg);
void mIO_digitalWrite(uint8_t gpio_register);
uint16_t mIO_analogRead(uint8_t pin);
uint8_t mIO_digitalRead(uint8_t pin);

#endif
