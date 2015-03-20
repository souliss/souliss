/**************************************************************************
	Souliss 
    Copyright (C) 2014

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
		
	Originally developed by Fulvio Spelta and Dario Di Maio
	
***************************************************************************/
/*!
    \file 
    \ingroup
*/
#ifndef XMLSERVER_H
#define XMLSERVER_H

#include "GetConfig.h"						// need : ethUsrCfg.h, vNetCfg.h, SoulissCfg.h

// XML over TCP/HTTP	(polling)
#define HTTPPORT		80					// Listening for WebServer
#define	HTTP_REQBYTES	50					// Max lenght for URL request
#define HTTP_BUFBYTES	(20+12*MaCaco_SLOT)	// Max lenght for data buffer

// XML over UDP			(event based) require MaCaco LASTIN mode
#define	XML_REQBYTES	HTTP_REQBYTES		// Max lenght for URL request
#define	XML_BUFBYTES	HTTP_BUFBYTES		// Max lenght for data buffer
#define	udpXML_inPORT	290					// Listening UDP port
#define	udpXML_outPORT	2900				// Destination UDP port

#define NULLID			0xFF				// This ID value is assumed as Null

#define	MAXVALUES		5					// Number of values into a single URL, comma separated

#define	CMDBUFF_NO		5
#define	CMDBUFF_HEADER	(5*CMDBUFF_NO)		// Lenght of the header structure
#define	CMDBUFF_VALS	(5*CMDBUFF_NO)		// Lenght of the vals area

typedef struct
{
	U16	address;							// Node Address
	U8  slot;								// Node Slot
} CommandBufferHeader;

#if (XMLSERVER_DEBUG)
	#define XMLSERVER_LOG Serial.print
#endif

#if(XMLSERVER && VNET_MEDIA1_ENABLE && ( ETH_W5100 || ETH_W5200 || ETH_W5500 || ETH_ENC28J60))
void XMLSERVERInit(U8 *memory_map);
void XMLSERVERInterface(U8 *memory_map);

// Get back compatibility with previous APIs
#define openHABInit XMLSERVERInit
#define openHABInterface XMLSERVERInterface

#endif
#endif
