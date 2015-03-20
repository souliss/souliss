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
#define SET_REL		0x10
#define	GET_IN		0x20
#define	RELAY1		0x01
#define	RELAY2		0x02
#define	RELAY3		0x04
#define	RELAY4		0x08
#define	IN1			0x01
#define	IN2			0x02
#define	IN3			0x04
#define	IN4			0x08
#define GET_AN1		0x30
#define GET_AN2		0x31
#define GET_AN3		0x32
#define GET_AN4		0x33

#define mIO_SetAddr0(addr0)			mIO_cmd[ADDR0] = addr0
#define mIO_SetAddr1(addr1)			mIO_cmd[ADDR1] = addr1
#define mIO_SetAddr2(addr2)			mIO_cmd[ADDR2] = addr2

void MODIO_Init();
void mIO_Relay(uint8_t relay_reg);
uint16_t mIO_analogRead(uint8_t pin);
uint8_t mIO_digitalRead(uint8_t pin);

#define MODIO_Init_wI2Cpullups()	MODIO_Init(); \
									I2C_pullup(1)
									

#endif
