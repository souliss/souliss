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

#define USART_BAUD				115200	

// General
#define L200_SUCCESS			0x01
#define L200_FAIL				0x00
#define	L200_UDP				0x10
#define	L200_TCP				0x20

// Frame
#define	L200_MAXPAYLOAD			VNET_MAX_FRAME
#define	L200_OVERHEAD			7					// Additional bytes out of the frame
#define L200_TYPECODE			7					// Biggest code size (SNDUDP, RCVUDP, CMDAT, GETMYIP)
#define L200_IPSIZE				15					// ASCII size of 255.255.255.255
#define L200_PORTSIZE			5					// ASCII size of 65536
#define	L200_HEADER_LEN			(L200_OVERHEAD+L200_TYPECODE+L200_IPSIZE+L200_PORTSIZE)
#define	L200_FRAME_LEN			(VNET_MAX_FRAME+L200_OVERHEAD+L200_TYPECODE+L200_IPSIZE+L200_PORTSIZE)

// Boot
#define L200_READYPIN			3
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

uint8_t lpt200_init();
void getip(uint8_t * addr);
uint8_t sendto(const uint8_t * buf, uint16_t len, uint8_t * addr, uint16_t port); 
uint8_t dataaval();
uint8_t getlen();
uint8_t recvfrom(uint8_t * buf, uint16_t len, uint8_t * addr, uint16_t *port);

#endif