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
#ifndef HTTP_H
#define HTTP_H

#include "GetConfig.h"			// need : ethUsrCfg.h, vNetCfg.h, SoulissCfg.h

#define HTTPPORT		80			// Listening for WebServer
#define HTTP_MAXBYTES	50			// Max lenght for URL request
#define NULLID			0xFF		// This ID value is assumed as Null

#if(HTTPSERVER && VNET_MEDIA1_ENABLE && (ETH_W5100 || ETH_W5200 || ETH_W5500))
void HTTPServer(U8 *memory_map);
#endif

#endif
