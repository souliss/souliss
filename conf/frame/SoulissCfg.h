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
*/
/**************************************************************************/
#if(!(QC_ENABLE))					// Define manually only in Detailed Configuration Mode
#	define HTTPSERVER  	0
#endif
							
/**************************************************************************/
/*!
	The XML Server is an TCP or UDP server listening for HTTP URLs and 
	enable data transfer using a XML string.
	
	Enabled in TCP mode, the node can be polled using a standard HTTP URL
	(connection in TCP on port 80) this allow data forcing and retriving.
	Enabled in UDP mode, the node once polled send data every time that
	new data are available.
	
	Data are available in XML format.
	
	The XML Server is supported only for Ethernet based nodes equipped with
	Wiznet W5100/W5200/W5500 controller.
	
        Value       
        0x0         Disable (Default)
        0x1         Enable in TCP (Polling)
		0x2			Enable in UDP (Event-driven)
*/
/**************************************************************************/
#if(!(QC_ENABLE))					// Define manually only in Detailed Configuration Mode
#	define XMLSERVER  	0
#endif

#define	XMLSERVER_DEBUG	0
			
/**************************************************************************/
/*!
	The HTTP server is listening on the port number 80 and parse remote 
	commands via HTTP GET request. It doesn't provide any data retrieve 
	mechanism, allows only to send commands over the network.
	
        Value       
        0x0         Disable (Default)
        0x1         Enable
*/
/**************************************************************************/
#if(!(QC_ENABLE))					// Define manually only in Detailed Configuration Mode
#	define HTTPSERVER  	0
#endif

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
*/
/**************************************************************************/
#if(!(QC_ENABLE))					// Define manually only in Detailed Configuration Mode
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
	The dynamic addressing node act as server for Souliss addresses over
	the network. It works for single media and bridged networks with a single
	bridge acting as addressing server.
	
        Value       
        0x0         Disable (Default)
        0x1         Enable
*/
/**************************************************************************/
#if(!(QC_ENABLE))					// Define manually only in Detailed Configuration Mode
#	define DYNAMICADDRESSING  	0
#endif

#define	DYNAMICADDR_GATEWAYNODE	0x004D
#define	DYNAMICADDR_SUBNETMASK	0xFF00
#define	DYNAMICADDR_GATEWAY		0xFF01

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
	
#endif
							  