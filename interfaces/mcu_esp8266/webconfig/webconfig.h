/* 
  ESP_WebConfig 

  Copyright (c) 2015 John Lassen. All rights reserved.
  This is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.
  This software is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.
  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

  Latest version: 1.1.3  - 2015-07-20
  Changed the loading of the Javascript and CCS Files, so that they will successively loaded and that only one request goes to the ESP.

  -----------------------------------------------------------------------------------------------
  History

  Version: 1.1.2  - 2015-07-17
  Added URLDECODE for some input-fields (SSID, PASSWORD...)

  Version  1.1.1 - 2015-07-12
  First initial version to the public
*/

/***
	
	Modified by Juan Pinto and Lesjaw Ardi to be used with Souliss

***/
#ifndef WEBCONFIG_H
#define WEBCONFIG_H

#define	HTTP_PORT			80
#define	DNS_PORT			53

#define SET_TRUE			1
#define SET_FALSE			0	

#define	IPABYTE_1			192
#define	IPABYTE_2			168
#define	IPABYTE_3			127
#define	IPABYTE_4			111

#define SUBBYTE_1			255
#define SUBBYTE_2			255
#define SUBBYTE_3			255
#define SUBBYTE_4			0

#define IPADDR_DEFAULT		IPAddress(IPABYTE_1, IPABYTE_2, IPABYTE_3, IPABYTE_4)
#define IPSUBN_DEFAULT		IPAddress(SUBBYTE_1, SUBBYTE_2, SUBBYTE_3, SUBBYTE_4)

struct strConfig {
	String ssid;
	String password;
	byte  IP[4];
	byte  Netmask[4];
	byte  Gateway[4];
	boolean dhcp;
	boolean rst;
	boolean RuntimeGateway; //test node mode as gateway or peer
}   config;


void defaultWebConfig();
void startWebServer();
void disableWebServer();
void runWebServer();
void WriteConfig();
boolean ReadConfig();

#endif
