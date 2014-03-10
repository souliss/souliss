/**************************************************************************
	Souliss - vNet Virtualized Network
	
	Modified by Dario Di Maio
	
***************************************************************************/

/*******************************************************************
    Copyright (C) 2009 FreakLabs
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions
    are met:

    1. Redistributions of source code must retain the above copyright
       notice, this list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright
       notice, this list of conditions and the following disclaimer in the
       documentation and/or other materials provided with the distribution.
    3. Neither the name of the the copyright holder nor the names of its contributors
       may be used to endorse or promote products derived from this software
       without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS'' AND
    ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
    ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
    FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
    DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
    OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
    HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
    LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
    OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
    SUCH DAMAGE.

    Originally written by Christopher Wang aka Akiba.
    Please post support questions to the FreakLabs forum.

*******************************************************************/
/*!
    \file 
    \ingroup
 
    This is the user configurable file. It contains parameters that users
    can change to suit their needs. 

*/
/**************************************************************************/
#ifndef CHB_USR_CFG_H
#define CHB_USR_CFG_H

/**************************************************************************/
/*!
	Select the wireless controller used for the board
	        
		Value       
        0x0         Disable (Default)
        0x1         Enable
		
		CHIBI_AT86RF230	-  Atmel AT86RF230 Wireless - 2.4 GHz IEEE 802.15.4
*/
/**************************************************************************/
#if(!(QC_ENABLE))					// Define manually only in Detailed Configuration Mode
#	define CHIBI_AT86RF230  	0
#endif
/**************************************************************************/

/**************************************************************************/
/*!
 Normally, when data is received to the radio, the radio will issue an interrupt
 to the microcontroller(MCU). The MCU will then move the data from the radio
 to the MCU memory. This needs to be done as quickly as possible since any other
 frame received by the radio will overwrite the previous one. Hence the data is
 moved inside the interrupt service routine.
 
 In some cases, this is not desired, especially if there are other devices that are
 extremely busy and have tight timing requirements. In this case, RX POLLING MODE
 will simply flag the interrupt and the data will get moved at the next possible chance
 by the MCU. Its called RX POLLING MODE because the interrupt flag is checked
 outside of the interrupt service routine on each iteration of the main loop (polling).
 
 RX POLLING MODE also resolves an issue with the Arduino Ethernet shield where the
 software makes it incompatible with devices that access the SPI bus inside an
 interrupt service routine.
 
        Value       Polling Mode
        0x0         Disable (Default)
        0x1         Enable


*/
/**************************************************************************/
#define CHB_RX_POLLING_MODE  0

/**************************************************************************/
/*!
    Enable the chibiArduino stack to run in promiscuous mode. This should
    only be used to analyze raw packet frames, like for wireshark. 
*/
/**************************************************************************/
#define CHIBI_PROMISCUOUS 0

/**************************************************************************/
/*!
    Max payload determines the largest size that can be transmitted in a single frame.
    If a frame is transmitted that's greater than the max payload, then it
    will be split up into the max payload size and transmitted over multiple frames.

        Integer, byte(s); Range: 1 - 116; Default: 100
        NOTE values over 100 may not work. Please see the documentation.
    
*/
/**************************************************************************/
#define CHB_MAX_PAYLOAD   VNET_MAX_FRAME

/**************************************************************************/
/*!
    This is the position in EEPROM where the 64-bit IEEE address is stored.
    It takes up 8 byte positions in the EEPROM. 

        HEX, EEPROM location; Range: 0x00 - 0x01F8; Default: 0x00
*/
/**************************************************************************/
#define CHB_EEPROM_IEEE_ADDR    0x00

/**************************************************************************/
/*!
    This is the position in the EEPROM where the 16-bit short address is
    stored. It takes up 2 bytes in the EEPROM.
    
        HEX, EEPROM location; Range: 0x00 - 0x1FE; Default 0x09
*/
/**************************************************************************/
#define CHB_EEPROM_SHORT_ADDR   0x09

/**************************************************************************/
/*!
    This is where the SLP_TR pin is defined. The PORT, Data Direction register
    and the position is needed to fully define it.
    
        CHB_SLPTR_PORT default PORTC on the chibiduino
        CHB_SLPTR_DDIR default DDRC on the chibiduino
        CHB_SLPTR_PIN default 2 on the chibiduino.
*/
/**************************************************************************/
#define CHB_SLPTR_PORT       PORTC
#define CHB_SLPTR_DDIR       DDRC
#define CHB_SLPTR_PIN        2

/**************************************************************************/
/*!
    This is where the SPI Chip Select pin is defined. The PORT, Data Direction register
    and the position is needed to fully define it.
    
        CHB_SPI_CS_PORT default PORTC on the chibiduino
        CHB_SPI_CS_DDIR default DDRC on the chibiduino
        CHB_SPI_CS_PIN default 3 on the chibiduino.
*/
/**************************************************************************/
#define CHB_SPI_CS_PORT PORTC
#define CHB_SPI_CS_DDIR DDRC
#define CHB_SPI_CS_PIN  3                 // PC.3 - SPI Chip Select (SSEL)


/**************************************************************************/
/*!
    This is where the IRQ vector is defined. The IRQ vector will be based
    on the interrupt being used for the radio.
    
        CHB_RADIO_IRQ default PCINT0_vect on the chibiduino
*/
/**************************************************************************/
#define CHB_RADIO_IRQ       PCINT0_vect
    
/**************************************************************************/    
/*!
    This is where the interrupt configuration code is. This may be different
    based on the chip or type of interrupt being used. 
*/
/**************************************************************************/    
// enable rising edge interrupt on IRQ0
#define CFG_CHB_INTP() do               \
            {                           \
                PCMSK0 |= _BV(PCINT6);  \
                PCICR |= _BV(PCIE0);    \
            }                           \ 
            while(0)

/**************************************************************************/
/*!
    This is the code to enable and disable the interrupts on the MCU side.
    This is only used when we're in RX_POLLING_MODE where the interrupt needs
    to be turned off on the MCU side until the data can be retrieved. It also
    is a workaround for a nasty bug on the Wiznet W5100 chips where the
    SPI interface is not released properly. Hence, the interrupts are turned
    off until the SPI bus is free and the data can be retrieved without collision.
*/
/**************************************************************************/
#define CHB_IRQ_DISABLE() do {PCMSK0 &= ~_BV(PCINT6);} while(0)
#define CHB_IRQ_ENABLE() do {PCMSK0 |= _BV(PCINT6);} while(0)


/**************************************************************************/
/*!
    The default channel for the radio to start on. The 802.15.4 channels go from
    channel 11 to 26. Channels that don't conflict with 802.11 (Wi-Fi) are
    channels 15, 20, and 26 in North America and Europe. 
    
        Integer; Range: 11 - 26; Default: 11
*/
/**************************************************************************/
#define CHB_CHANNEL     20

/**************************************************************************/
/*!
    This is the default PAN ID (network ID) of all devices on the network.
    Only devices sharing the same PAN ID can communicate with each other,
    unless the radio is set to promiscuous mode. The PAN ID is a 16-bit value.
    
        HEX; Range: 0x0000 - 0xFFFF; Default: 0x1234
*/
/**************************************************************************/
#define CHB_PAN_ID  0xAEFF

/**************************************************************************/
/*!
    This is the value that gets written into the radio's register. Each value
    corresponds to a different transmit power in dBm. The mapping is as follows:
 
        Value       Power (dBm)
        0x0         +3.0    (Default)
        0x1         +2.6
        0x2         +2.1
        0x3         +1.6
        0x4         +1.1
        0x5         +0.5
        0x6         -0.2
        0x7         -1.2
        0x8         -2.2
        0x9         -3.2
        0xA         -4.2
*/
/**************************************************************************/
#define CHB_TX_PWR  0x0

/**************************************************************************/
/*!
    This is the number of times the radio will auto-retry a transmission.
    The auto-retry is triggered when an ACK isn't received within the ACK
    timeout period. This is typically ~1 msec. 
    
        Integer, count; Range: 0 - 15; Default: 3
*/
/**************************************************************************/
#define CHB_MAX_FRAME_RETRIES   3

/**************************************************************************/
/*!
    This is the number of times the radio will attempt a transmission when
    the channel is busy. The radio first checks if the channel is clear.
    If it's occupied by another node, then it will back-off for a random
    period and try again. If it exceeds the MAX CSMA RETRIES, it will assume
    the channel is blocked and return an error status.
    
        Integer, count; Range: 0 - 9; Default: 4
*/
/**************************************************************************/
#define CHB_MAX_CSMA_RETRIES    4

/**************************************************************************/
/*!
    This is the minimum backoff exponent. When the channel is busy, the radio
    will wait at least 2^MIN_BE symbol periods before another attempt at
    transmission.
    
        Integer, exponent; Range: 0 - 3; Default: 0
*/
/**************************************************************************/
#define CHB_MIN_BE  0

/**************************************************************************/
/*!
    This is the clear channel assessment mode used to determine whether a
    channel is clear or not. There are generally two ways to assess a busy
    channel. One is to do an energy detection by sampling the energy level
    of the channel. If its below the threshold, then its clear. The other
    way is to check for a 2.4 GHz carrier signal.
 
        Value       Mode
        0           Reserved
        1           Mode 1, Energy above threshold      (Default)
        2           Mode 2, Carrier sense only
        3           Mode 3, Carrier sense with energy above threshold 
*/
/**************************************************************************/
#define CHB_CCA_MODE    0x1

/**************************************************************************/
/*!
    This is the energy detection threshold for the clear channel assessment.
    A channel is considered busy when the energy in the channel is:
    RSSI_BASE_VAL + 2 • CCA_ED_THRES [dBm]
 
    where RSSI_BASE_VAL = -91 dBm for the AT86RF230
    
        Integer, byte; Range: 0x01 - 0x0F; Default: 0x07
*/
/**************************************************************************/
#define CHB_CCA_ED_THRES    0x7

/**************************************************************************/
/*!
    These are the seeds to start off the pseudorandom number generator
    for the backoff timer. They can be defined to be anything and will
    determine the random number sequence for the backoffs. 
*/
/**************************************************************************/
#define CHB_CSMA_SEED0  0
#define CHB_CSMA_SEED1  0

/**************************************************************************/
/*!
    Define the max wait time for the last ack, time is milli seconds
*/
/**************************************************************************/
#define CHB_WAIT   1000

/**************************************************************************/
/*!
    Useful definition   
*/
/**************************************************************************/
#define CHB_SUCC		1
#define CHB_FAIL		0

#endif
