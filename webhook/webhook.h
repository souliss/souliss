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
#	include "EthernetUdp.h"
#	include "EthernetServer.h"
#	include "EthernetClient.h"
#	include "Ethernet.h"
#endif

#if(ARDUINO_DHCP)
#	include "Dhcp.h"
#endif

#if(ARDUINO_DNS)
#	include "Dns.h"
#endif

#if(ARDUINO_ETHLIB)
#	include "EthernetUdp.cpp"
#	include "EthernetServer.cpp"
#	include "EthernetClient.cpp"
#	include "Ethernet.cpp"
#endif

#if(ARDUINO_DHCP)
#	include "Dns.cpp"
#endif

#if(ARDUINO_DNS)
#	include "Dhcp.cpp"
#endif

#endif