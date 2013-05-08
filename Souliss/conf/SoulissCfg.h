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
	Macros
*/
/**************************************************************************/
#define AUX		MaCaco_AUXIN_s
#define	IN		MaCaco_IN_s
#define	OUT		MaCaco_OUT_s
#define Souliss_Auxiliary(memory_map, slot)	memory_map[AUX+slot]	
#define Souliss_Input(memory_map, slot)		memory_map[IN+slot]	
#define Souliss_Output(memory_map, slot)	memory_map[OUT+slot]
#define Souliss_ResetTrigger(data_changed)	data_changed=0;

/**************************************************************************/
/*!
	The HTTP server is listening on the port number 80 and parse remote 
	commands via HTTP GET request. 
	
	The actual state of the node and its connected devices can be retrieved
	via MaCaco, Modbus or JSON (if W5100 is used). Enabling only the HTTP
	doesn't give access to that data.
	
        Value       HTTPSERVER
        0x0         Disable (Default)
        0x1         Enable
*/
/**************************************************************************/
#if(!(QC_ENABLE))					// Define manually only in Detailed Configuration Mode
#	define HTTPSERVER  	0
#endif
							
/**************************************************************************/
/*!
	The JSON Server is an HTTP server listening on the port number 80 that
	enable data transfer using a JSON string.
	
	An external device can access the data into the node using a simple HTTP
	GET request. That node can access data from all the other in the network
	acting as a Souliss to JSON gateway.
	
	The JSON Server is supported only for Ethernet based nodes equipped with
	Wiznet W5100 controller.
	
        Value       JSONSERVER
        0x0         Disable (Default)
        0x1         Enable
*/
/**************************************************************************/
#if(!(QC_ENABLE))					// Define manually only in Detailed Configuration Mode
#	define JSONSERVER  	0
#endif
/**************************************************************************/
/*!
	The Modbus TCP/RTU enable data transfer using standard Modbus protocol,
	an external device can access the data into the node using a standard
	Modbus request. 
	
	That node can access data from all the other in the network acting as a 
	Souliss to Modbus gateway.
	
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
	Watchdog configuration parameters
*/
/**************************************************************************/
#define	WTD_EXPIRE	0x10
#define	WTD_FAILED	0x00
#define	WTD_SET		0x19
											
#endif
							  