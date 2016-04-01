/**************************************************************************
	Souliss 
    Copyright (C) 2012  Veseo

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
#ifndef SOULISS_CFG_H
#define SOULISS_CFG_H

/**************************************************************************/
/*!
	The HTTP server is listening on the port number 80 and parse remote 
	commands via HTTP GET request. It doesn't provide any data retrieve 
	mechanism, allows only to send commands over the network.
	
        Value       
        0x0         Disable (Default)
        0x1         Enable

	This is supported on AVR only.
*/
/**************************************************************************/
#ifndef HTTPSERVER_INSKETCH
#	define HTTPSERVER  	0
#endif
							
/**************************************************************************/
/*!
	The XML Server is an TCP server listening for HTTP URLs and 
	enable data transfer using a XML string.
	
	Enabled in TCP mode, the node can be polled using a standard HTTP URL
	(connection in TCP on port 80) this allow data forcing and retrieving.
	
	Data are available in XML format.
	
	The XML Server is supported only for Ethernet based nodes equipped with
	Wiznet W5100/W5200/W5500 controller.
	
        Value       
        0x0         Disable (Default)
        0x1         Enable in TCP (Polling)

	This is supported on AVR only.
*/
/**************************************************************************/
#ifndef XMLSERVER_INSKETCH
#	define	XMLSERVER  			0
#endif

#define	XMLSERVER_DEBUG			0

/**************************************************************************/
/*!
	The Modbus TCP/RTU enable data transfer using standard Modbus protocol,
	an external device can access the data into the node using a standard
	Modbus request. 
	
	That node can access data from all the other in the network acting as a 
	Souliss to Modbus interface.
	
	This feature is intender to get data from a Souliss node, so Souliss is 
	always a Modbus slave. In case of Modbus TCP a dedicated socket is used,
	rather in case of Modbus RTU the internal USART is used.
	
	This code implement only partially the Modbus protocol and doesn't allow
	multidrop configuration and/or bridging in RTU mode.

	This is supported on AVR only.
*/
/**************************************************************************/
#ifndef MODBUS_INSKETCH
#	define	MODBUS					0
#	define 	MODBUS_TCP  			1
#	define 	MODBUS_RTU  			0
#endif

#define	MODBUS_DEBUG				0

#define	MODBUS_ID					1	
#define	MODBUS_RTU_BAUD				115200
#define MODBUS_TCP_PORT				502
		
/**************************************************************************/
/*!
	Enable the WebConfiguration HTTP server on port number 80.
	
        Value       
        0x0         Disable (Default)
        0x1         Enable

	This is supported on ESP8266 only.
*/
/**************************************************************************/
#ifndef WEBCONFIGSERVER_INSKETCH
#	define WEBCONFIGSERVER			0
#endif

	
/**************************************************************************/
/*!
	If dynamic addressing is enabled, nodes get address from the Gateway
	that acts as addressing server
	
        Value       
        0x0         Disable (Default)
        0x1         Enable
*/
/**************************************************************************/
#ifndef DYNAMICADDRESSING_INSKETCH
#	define DYNAMICADDRESSING  	0
#endif

/**************************************************************************/
/*!
	Once enabled, node configuration is stored in the EEPROM, use the constant
	FIRST_EEPROM_BYTE to identify the first usable byte in your EEPROM.
	
        Value       
        0x0         Disable (Default)
        0x1         Enable
*/
/**************************************************************************/
#ifndef USEEEPROM_INSKETCH
#	define USEEEPROM			0
#endif

#define	DYNAMICADDR_IPVNETNODE	0x004D
#define	DYNAMICADDR_IPGATEWAY	0x0000
#define	DYNAMICADDR_GATEWAYNODE	0x0001
#define	DYNAMICADDR_SUBNETMASK	0xFF00
#define	DYNAMICADDR_GATEWAY		0xFF01
#define	DYNAMICADDR_TEMPADDR	0x0033

/**************************************************************************/
/*!
	Watchdog configuration parameters
*/
/**************************************************************************/
#define	WTD_EXPIRE	0x10
#define	WTD_FAILED	0x00
#define	WTD_SET		0x19

/**************************************************************************/
/*!
	These option let customize the sleeping behaviour of the microcontroller
	
	  SLEEP_BODDISALBE 
		
		Disable the Brown-out Detector, this is used to
		trip the microcontroller in case of low supply voltage. 
		Enabling this option will reduce power consumption but may force the
		microcontroller to work at too low voltages, this may result in 
		unpredictable results.

        Value       
        0x0         Disable 
        0x1         Enable  (Default)
		
	  SLEEP_WAKEUPDELAY
		Enable a 10+10 millisecond wakeup delay, this give time to the radio
		and microcontroller to be ready (voltage get stabilized and peripherals
		are back active).		
	
        Value       
        0x0         Disable
        0x1         Enable  (Default)
*/
/**************************************************************************/
#ifndef	SLEEP_ENABLE_INSKETCH
#	define SLEEP_ENABLE  		0
#endif

#define SLEEP_BODDISALBE		0
#define	SLEEP_WAKEUPDELAY		1

/**************************************************************************/
/*!
	Timeout configuration parameters
*/
/**************************************************************************/
#define	TIMEOUT_EXPIRE	0x00
#define	TIMEOUT_SET		0x20
	
/**************************************************************************/
/*!
	Souliss_AnalogIn2State configuration parameters
*/
/**************************************************************************/
#define AIN2S_BOTTOM  300
#define AIN2S_TOP     700

/**************************************************************************/
/*!
	If enabled print debug messages
	
        Value       Status
        0x0         Disable (Default)
        0x1         Enable
*/
/**************************************************************************/
#ifndef	SOULISS_DEBUG_INSKETCH
#	define SOULISS_DEBUG  		0
#endif

/**************************************************************************/
/*!
	Define an object for debug messages, this can be an USART or any other
	type of object that support print method.
*/
/**************************************************************************/
#ifndef SERIALPORT_INSKETCH
#	define LOG 					Serial
#endif

/**************************************************************************/
/*!
	Define the specific print output for debug messages
*/
/**************************************************************************/
#define SOULISS_LOG 			LOG.print

#endif
							  