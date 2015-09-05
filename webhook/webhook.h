/**************************************************************************
	Souliss
    Copyright (C) 2013  Veseo

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
#ifndef WEBHOOK_H
#define WEBHOOK_H

#if(ARDUINO_ETHLIB)
#	include "ethW5x00/EthernetUdp.h"
#	include "ethW5x00/EthernetServer.h"
#	include "ethW5x00/EthernetClient.h"
#	include "ethW5x00/Ethernet.h"
#endif

#if(ARDUINO_DHCP)
#	include "ethW5x00/Dhcp.h"
#endif

#if(ARDUINO_DNS)
#	include "ethW5x00/Dns.h"
#endif

#if(ARDUINO_ETHLIB)
#	include "ethW5x00/EthernetUdp.cpp"
#	include "ethW5x00/EthernetServer.cpp"
#	include "ethW5x00/EthernetClient.cpp"
#	include "ethW5x00/Ethernet.cpp"
#endif

#if(ARDUINO_DHCP)
#	include "ethW5x00/Dns.cpp"
#endif

#if(ARDUINO_DNS)
#	include "ethW5x00/Dhcp.cpp"
#endif

#endif