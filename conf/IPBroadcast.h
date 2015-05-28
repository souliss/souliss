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
#ifndef bconf_IPBROADCAST_H
#define bconf_IPBROADCAST_H

#if(MCU_TYPE == 0x01)	// Atmel AVR Atmega
#	if  (MaCaco_USERMODE && VNET_MEDIA1_ENABLE)	// If is a gateway
#		define VNET_MEDIA3_ENABLE  			1
#	elif(VNET_MEDIA1_ENABLE)
#		define	VNET_MEDIA1_ENABLE  		0
#		define	VNET_MEDIA3_ENABLE  		1
#	else
#		define	VNET_MEDIA3_ENABLE  		0
#		define	IPBROADCAST					0
#	endif
#elif(MCU_TYPE == 0x02)	// Expressif ESP8266
#		define VNET_MEDIA3_ENABLE  			1
#endif

#endif
