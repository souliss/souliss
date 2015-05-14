/**************************************************************************
	Souliss
    Copyright (C) 2015  Veseo

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
	
	Original developed by Dario Di Maio
	
***************************************************************************/

/*!
    \file 
    \ingroup

*/ 

#include <ESP8266WiFi.h>
#include "GetConfig.h"				// need : ethUsrCfg.h
#include <Souliss.h>

uint8_t	myvNet_esp8266=0;

/**************************************************************************
/*!
	Special setup method for ESP8266
*/	
/**************************************************************************/ 
void Setup_ESP8266()
{
	// Setup the SSID and Password
	WiFi.begin(WiFi_SSID, WiFi_Password);
	
	// Connect
	while (WiFi.status() != WL_CONNECTED) 	
		delay(500);
	
	// Get the IP network parameters
	IPAddress lIP  = WiFi.localIP();
	IPAddress sMk  = WiFi.subnetMask();
	IPAddress gIP  = WiFi.gatewayIP();
	
	uint8_t ipaddr[4];
	uint8_t subnet[4];
	uint8_t gateway[4];
	
	for(uint8_t i=0;i<4;i++)
	{
		ipaddr[i]  = lIP[i];
		subnet[i]  = sMk[i];
		gateway[i] = gIP[i];
	}	

	// The last byte of the IP address is used as vNet address
	myvNet_esp8266 = ipaddr[3];
	
	// Set the values in the vNet stack
	Souliss_SetIPAddress(ipaddr, subnet, gateway);
}						

