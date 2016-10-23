/**************************************************************************
	Souliss 
    Copyright (C) 2016  Veseo

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
	
	Originally developed by Plamen Kovandjiev and Dimitar Antonov
	
***************************************************************************/
/*!
    \file 
    \ingroup
*/

#ifndef _KMPDINOWIFIESP_H
#define _KMPDINOWIFIESP_H

#include <Arduino.h>

// Defines
#define RELAY_COUNT  4
#define OPTOIN_COUNT 4

#define CS 0x0F

#define READ_CMD  0x41
#define WRITE_CMD 0x40

#define IODIR   0x00
#define IPOL    0x01
#define GPINTEN 0x02
#define DEFVAL  0x03
#define INTCON  0x04
#define IOCON   0x05
#define GPPU    0x06
#define INTF    0x07
#define INTCAP  0x08
#define GPIO    0x09
#define OLAT    0x0A

// Relay pins
#define REL1PIN 0x04
#define REL2PIN 0x05
#define REL3PIN 0x06
#define REL4PIN 0x07

// Opto input pins
#define IN1PIN 0x00
#define IN2PIN 0x01
#define IN3PIN 0x02
#define IN4PIN 0x03

// RS485 enable
#define RS485PIN 0x10

// User defined pins
#define IN1				0
#define IN2				1
#define IN3				2
#define IN4				3

#define RELAY1			0
#define RELAY2			1
#define RELAY3			2
#define RELAY4			3	

/**
 * @brief Grove connector first pin GPIO4/SDA.
 */
#define	GROVE_PIN1	4 

/**
 * @brief Grove connector first pin GPIO5/SCL.
 */
#define	GROVE_PIN2	5 

/**
 * @brief Relay pins.
 */
const uint8_t RELAY_PINS[RELAY_COUNT] =
{ REL1PIN, REL2PIN, REL3PIN, REL4PIN };

/**
 * @brief Input pins.
 */
const int OPTOIN_PINS[OPTOIN_COUNT] =
{ IN1PIN, IN2PIN, IN3PIN, IN4PIN };

uint8_t  _expTxData[16];
uint8_t  _expRxData[16];

// User Methods
#define	InitDINo()	DINoWroom_init()
#define	dRead		DINoWroom_ExpanderGetPin
#define	dWrite		DINoWroom_ExpanderSetPin
#define	aRead		analogRead
#define	aWrite		analogWrite

#endif