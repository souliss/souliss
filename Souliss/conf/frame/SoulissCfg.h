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
	The openHAB Server is an TCP or UDP server listening for HTTP URLs and 
	enable data transfer using a XML string.
	
	Enabled in TCP mode, the node can be polled using a standard HTTP URL
	(connection in TCP on port 80) this allow data forcing and retriving.
	Enabled in UDP mode, the node once polled send data every time that
	new data are available.
	
	Data are available in XML format.
	
	The openHAB Server is supported only for Ethernet based nodes equipped with
	Wiznet W5100/W5200 controller.
	
        Value       
        0x0         Disable (Default)
        0x1         Enable in TCP
		0x2			Enable in UDP
*/
/**************************************************************************/
#if(!(QC_ENABLE))					// Define manually only in Detailed Configuration Mode
#	define OPENHAB  	0
#endif

#define	OPENHAB_DEBUG	0
					
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
#define	DYNAMICADDR_GATEWAY		0x0000

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
	Souliss_AnalogIn2State configuration parameters
*/
/**************************************************************************/
#define AIN2S_BOTTOM  300
#define AIN2S_TOP     700
	
#endif
							  