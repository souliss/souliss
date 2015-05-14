/**************************************************************************
	Souliss
    Copyright (C) 2014  Veseo

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
#ifndef bconf_ssSTDARDUINO_H
#define bconf_ssSTDARDUINO_H

// Expressif ESP8266 Board
#define MCU_TYPE_INSKETCH
#define	MCU_TYPE			0x02

#define BOARD_MODEL_INSKETCH
#define VNET_MEDIA_INSKETCH
#define ETH_INSKETCH	

#define	BOARD_MODEL					9		
#define	COMMS_MODEL					7
#define ETH_W5100  					0
#define ETH_W5200  					0
#define ETH_W5500					0
#define ETH_ENC28J60  				0
#define WIFI_MRF24					0
#define WIFI_ESP8266				1

#define	Init_ESP8266()	WiFi.begin(WiFi_SSID, WiFi_Password);										\
						while (WiFi.status() != WL_CONNECTED) {										\
						delay(500);}																\
						Souliss_SetIPAddress(WiFi.localIP(), WiFi.subnetMask(), WiFi.gatewayIP());	\
						SetAsGateway(*(WiFi.localIP()+3))
						

#include <ESP8266WiFi.h>


#endif
