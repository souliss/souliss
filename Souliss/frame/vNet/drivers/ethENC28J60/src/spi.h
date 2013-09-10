/**************************************************************************
	Souliss - vNet Virtualized Network
    
	Porting in vNet for Souliss project by Dario Di Maio
	
***************************************************************************/

#ifndef ENCSPI_H_
#define ENCSPI_H_

// Define mask for bits
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

// Define Chip Select for Olimex AVR-T32U4 or OLIMEXINO-32U4 using MOD-WIFI or MOD-ENC28J60 (UEXT)
#	if((BOARD_MODEL == 0x04 || BOARD_MODEL == 0x05) && ((COMMS_MODEL == 0x04) || (COMMS_MODEL == 0x05)))
#		define ENC28J60_CS_BIT            BIT7        // ATmega32U4 PC7 - UEXT Chip Select   
#		define ENC28J60_CS_DDR            DDRC
#		define ENC28J60_CS_PORT           PORTC
#	else
#		define ENC28J60_CS_BIT            BIT6        // ATmega32U4  PB6 - Arduino pin 10   
#		define ENC28J60_CS_DDR            DDRB
#		define ENC28J60_CS_PORT           PORTB
#	endif

#	define SPI0_Init()	DDRB  |= SPI0_SS_BIT|SPI0_SCLK_BIT|SPI0_MOSI_BIT;\
										DDRB  &= ~SPI0_MISO_BIT;\
										PORTB = SPI0_SS_BIT

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

// Define Chip Select for Olimex OLIMEXINO-328 using MOD-WIFI or MOD-ENC28J60 (UEXT) 
#	if((BOARD_MODEL == 0x06) && ((COMMS_MODEL == 0x04) || (COMMS_MODEL == 0x05)))
#		define ENC28J60_CS_BIT            BIT7        // ATmega328  PD7 - UEXT Chip Select
#		define ENC28J60_CS_DDR            DDRD
#		define ENC28J60_CS_PORT           PORTD
#	else
#		define ENC28J60_CS_BIT            BIT2        // ATmega328  PB2 - Arduino pin 10   
#		define ENC28J60_CS_DDR            DDRB
#		define ENC28J60_CS_PORT           PORTB
#	endif

#	define SPI0_Init()	DDRB  |= SPI0_SS_BIT|SPI0_SCLK_BIT|SPI0_MOSI_BIT;\
										DDRB  &= ~SPI0_MISO_BIT;\
										PORTB = SPI0_SS_BIT
#endif
/*
#define SPI0_WaitForReceive()
#define SPI0_RxData()				(SPDR)

#define SPI0_TxData(Data)			(SPDR = Data)
#define SPI0_WaitForSend()			while( (SPSR & 0x80)==0x00 )        

#define SPI0_SendByte(Data)			SPI0_TxData(Data);SPI0_WaitForSend()
#define SPI0_RecvByte()				SPI0_RxData()
*/
#define ENC28J60_SpiInit			SPI0_Init
//#define ENC28J60_SpiSendData		SPI0_SendByte
//#define ENC28J60_SpiRecvData		SPI0_RxData

#define ENC28J60_CSInit()			(ENC28J60_CS_DDR  |= ENC28J60_CS_BIT)
#define ENC28J60_CSon()				(ENC28J60_CS_PORT |= ENC28J60_CS_BIT)
#define ENC28J60_CSoff()			(ENC28J60_CS_PORT &= ~ENC28J60_CS_BIT)    

#endif
