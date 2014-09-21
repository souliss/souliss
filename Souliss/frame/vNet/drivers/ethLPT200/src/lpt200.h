/**************************************************************************
	Souliss - vNet Virtualized Network
    Copyright (C) 2014  Veseo

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
#include <stdio.h>
#include <string.h>

#ifndef VNET_LPT200_H
#define VNET_LPT200_H

#include "Arduino.h"
#include "SerialPort.h"

#define USART_BAUD				19200	

// General
#define L200_SUCCESS			0x01
#define L200_FAIL				0x00
#define	L200_UDP				0x10
#define	L200_TCP				0x20
#define	L200_JUNK				0x05				// Number of cycle that data can hold in the buffer

// Commands for LPT200 Wifi Module
#define	SNDUDP					0x01
#define	RCVUDP					0x02
#define	CMDAT					0x03
#define	GETMYIP					0x04
#define	FRAMESTART				0x23
#define	FRAMESTOP				0x24
#define	FRAMESPACE				0x2C

// Frame
#define	L200_MAXPAYLOAD			(VNET_MAX_FRAME)
#define L200_HEADER_LEN			25
#define	L200_USARTBUFFER		192

// Boot
#define	L200_BOOTTIME			10000
#define	L200_WAITTIME			1000

/**************************************************************************/
/*!
	If enabled print the header and payload of incoming, outgoing and routed
	frames.
	
        Value       Status
        0x0         Disable (Default)
        0x1         Enable
		
	This enable the Arduino SoftwareSerial library and need an external USB
	to USART converter (like the one used for programming Arduino Ethernet).
	
	In your sketch you should add the following lines
		[ before setup() ] 	SoftwareSerial myUSARTDRIVER(5, 6);	
		
		[in setup()	]		myUSARTDRIVER.begin(9600);

	The TX and RX pins from the external converter shall be reversed, so TX 
	goes on pin 5 (RX from SoftwareSerial) and RX to 6.
*/
/**************************************************************************/
#define LPT200_DEBUG  			0

// The name of the class that refers to the USART, change it accordingly to the used device
//#ifndef USARTDRIVER_INSKETCH
//#	define	USARTDRIVER	Serial				
//#endif

uint8_t lpt200_init();
void getip(uint8_t * addr);
uint8_t sendUDP(const uint8_t * buf, uint16_t len, uint8_t * addr, uint16_t port); 
uint8_t dataaval();
uint8_t getlen();
uint8_t recvfrom(uint8_t * buf, uint16_t len, uint8_t * addr, uint16_t *port);
uint8_t recv(uint8_t * buf, uint16_t len);

#endif