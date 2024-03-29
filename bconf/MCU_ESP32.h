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
#ifndef bconf_ssMCUESP32_H
#define bconf_ssMCUESP32_H

// Expressif ESP32 Board
#define MCU_TYPE_INSKETCH
#define	MCU_TYPE					3

#define BOARD_MODEL_INSKETCH
#define	COMMS_MODEL_INSKETCH
#define VNET_MEDIA_INSKETCH
#define ETH_INSKETCH	

#define	BOARD_MODEL					9		
#define	COMMS_MODEL					7
#define ETH_W5100  					0
#define ETH_W5200  					0
#define ETH_W5500					0
#define ETH_ENC28J60  				0
#define WIFI_MRF24					0
#define WIFI_ESP8266				0
#define WIFI_ESP32					1
#define ETH_ESP32					0

// The ESP32 works always with two media, it use Media 3 to
// remove dependence between vNet address and IP address from DHCP
#define VNET_MEDIA1_ENABLE  		1
#define VNET_MEDIA3_ENABLE  		1

#define USARTDRIVER_INSKETCH
#define USART_TXENABLE				0
#define USART_TXENPIN				2
#define	USARTDRIVER					Serial

#endif
