/**************************************************************************
	Souliss - vNet Virtualized Network
    
	Porting in vNet for Souliss project by Dario Di Maio
	
***************************************************************************/

/******************************************************************************

  Filename:		spi.h
  Description:	SPI bus configuration for the WiShield 1.0

 ******************************************************************************

  TCP/IP stack and driver for the WiShield 1.0 wireless devices

  Copyright(c) 2009 Async Labs Inc. All rights reserved.

  This program is free software; you can redistribute it and/or modify it
  under the terms of version 2 of the GNU General Public License as
  published by the Free Software Foundation.

  This program is distributed in the hope that it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
  more details.

  You should have received a copy of the GNU General Public License along with
  this program; if not, write to the Free Software Foundation, Inc., 59
  Temple Place - Suite 330, Boston, MA  02111-1307, USA.

  Contact Information:
  <asynclabs@asynclabs.com>

   Author               Date        Comment
  ---------------------------------------------------------------
   AsyncLabs			05/01/2009	Initial version
   AsyncLabs			05/29/2009	Adding support for new library

 *****************************************************************************/
 /* Modifyed by Daniele Denaro  2012*/


#ifndef WIFSPI_H_
#define WIFSPI_H_

#define USE_DIG0_INTR	  //used in other files

#define ZG2100_ISR_DISABLE()	(EIMSK &= ~(0x01))     
#define ZG2100_ISR_ENABLE()		(EIMSK |= 0x01)
#define ZG2100_ISR_GET(X)		(X = EIMSK)
#define ZG2100_ISR_SET(X)		(EIMSK = X)

#define BIT0                     0x01
#define BIT1                     0x02
#define BIT2                     0x04
#define BIT3                     0x08
#define BIT4                     0x10
#define BIT5                     0x20
#define BIT6                     0x40
#define BIT7                     0x80

// Define SPI pins and register based on used micro, in most of cases the Slave Select
// SS pin is not used due to microcontroller in Master mode. In that case, SS shall be
// an output, otherwise the micro may goes in Slave mode and this result in a broke
// communication. A way to avoid problems is use the SS as CS that shall be an output.


#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)

#elif defined(__AVR_ATmega1284P__) || defined(__AVR_ATmega644P__)

#elif defined(__AVR_ATmega32U4__)
// Arduino Leonardo and ATmega32U4 boards
#	define SPI0_SS_BIT              BIT0		// Not required, must be set as output             
#	define SPI0_SS_DDR              DDRB
#	define SPI0_SS_PORT             PORTB
#	define SPI0_SCLK_BIT            BIT1     	// ATmega32U4 PB1 - In ICSP     
#	define SPI0_SCLK_DDR            DDRB
#	define SPI0_SCLK_PORT           PORTB
#	define SPI0_MOSI_BIT            BIT2        // ATmega32U4 PB2 - In ICSP
#	define SPI0_MOSI_DDR            DDRB
#	define SPI0_MOSI_PORT           PORTB
#	define SPI0_MISO_BIT            BIT3        // ATmega32U4 PB3 - In ICSP
#	define SPI0_MISO_DDR            DDRB
#	define SPI0_MISO_PORT           PORTB

// Define Chip Select for Olimex AVR-T32U4 or OLIMEXINO-32U4 using MOD-WIFI (UEXT) with Microchip MRF24WB0MA
#	if((BOARD_MODEL == 0x04 || BOARD_MODEL == 0x05) && (COMMS_MODEL == 0x05))
#		define ZG2100_CS_BIT            BIT7        // ATmega32U4 PC7 - UEXT Chip Select   
#		define ZG2100_CS_DDR            DDRC
#		define ZG2100_CS_PORT           PORTC
#	else
#		define ZG2100_CS_BIT            BIT6        // ATmega32U4  PB6 - Arduino pin 10   
#		define ZG2100_CS_DDR            DDRB
#		define ZG2100_CS_PORT           PORTB
#	endif

#	define SPI0_Init()	DDRB  |= SPI0_SS_BIT|SPI0_SCLK_BIT|SPI0_MOSI_BIT;\
										DDRB  &= ~SPI0_MISO_BIT;\
										PORTB = SPI0_SS_BIT;\
										SPCR  = 0x50;\
										SPSR  = 0x01

#else
// Standard ATmega328
#	define SPI0_SS_BIT              BIT2		// Not required used as CS             
#	define SPI0_SS_DDR              DDRB
#	define SPI0_SS_PORT             PORTB
#	define SPI0_SCLK_BIT            BIT5     	// ATmega328 PB5 - Arduino pin 13         
#	define SPI0_SCLK_DDR            DDRB
#	define SPI0_SCLK_PORT           PORTB
#	define SPI0_MOSI_BIT            BIT3        // ATmega328  PB3 - Arduino pin 11    
#	define SPI0_MOSI_DDR            DDRB
#	define SPI0_MOSI_PORT           PORTB
#	define SPI0_MISO_BIT            BIT4        // ATmega328  PB4 - Arduino pin 12    
#	define SPI0_MISO_DDR            DDRB
#	define SPI0_MISO_PORT           PORTB

// Define Chip Select for Olimex OLIMEXINO-328 using MOD-WIFI (UEXT) with Microchip MRF24WB0MA
#	if((BOARD_MODEL ==0x06) && (COMMS_MODEL == 0x05))
#		define ZG2100_CS_BIT            BIT7        // ATmega328  PD7 - UEXT Chip Select
#		define ZG2100_CS_DDR            DDRD
#		define ZG2100_CS_PORT           PORTD
#	else
#		define ZG2100_CS_BIT            BIT2        // ATmega328  PB2 - Arduino pin 10   
#		define ZG2100_CS_DDR            DDRB
#		define ZG2100_CS_PORT           PORTB
#	endif

#	define SPI0_Init()	DDRB  |= SPI0_SS_BIT|SPI0_SCLK_BIT|SPI0_MOSI_BIT;\
										DDRB  &= ~SPI0_MISO_BIT;\
										PORTB = SPI0_SS_BIT;\
										SPCR  = 0x50;\
										SPSR  = 0x01
#endif

#define SPI0_WaitForReceive()
#define SPI0_RxData()				(SPDR)

#define SPI0_TxData(Data)			(SPDR = Data)
#define SPI0_WaitForSend()			while( (SPSR & 0x80)==0x00 )        

#define SPI0_SendByte(Data)			SPI0_TxData(Data);SPI0_WaitForSend()
#define SPI0_RecvBute()				SPI0_RxData()

#define ZG2100_SpiInit				SPI0_Init
#define ZG2100_SpiSendData			SPI0_SendByte
#define ZG2100_SpiRecvData			SPI0_RxData

#define ZG2100_CSInit()				(ZG2100_CS_DDR  |= ZG2100_CS_BIT)
#define ZG2100_CSon()				(ZG2100_CS_PORT |= ZG2100_CS_BIT)
#define ZG2100_CSoff()				(ZG2100_CS_PORT &= ~ZG2100_CS_BIT)    

#endif /* WIFSPI_H_ */
