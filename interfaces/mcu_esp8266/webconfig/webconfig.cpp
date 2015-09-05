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

#include "webconfig.h"

ESP8266WebServer server(HTTP_PORT);	// The Webserver
int AdminTimeOutCounter = 0;		// Counter for Disabling the AdminMode
boolean AdminEnabled = true;		// Enable Admin Mode for a given Time

#include "src/root.h"
#include "src/admin.h"
#include "src/script.js.h"
#include "src/style.css.h"
#include "src/main.h"
#include "src/netconfig.h"

// Apply default configuration
void defaultWebConfig()
{
	config.ssid = "MYSSID";
	config.password = "MYPASSWORD";
	config.dhcp = true;
	config.RuntimeGateway = true;
	config.IP[0] = IPABYTE_1;config.IP[1] = IPABYTE_2;config.IP[2] = IPABYTE_3;config.IP[3] = IPABYTE_4;
	config.Netmask[0] = SUBBYTE_1;config.Netmask[1] = SUBBYTE_2;config.Netmask[2] = SUBBYTE_3;config.Netmask[3] = SUBBYTE_4;
	config.Gateway[0] = IPABYTE_1;config.Gateway[1] = IPABYTE_2;config.Gateway[2] = IPABYTE_3;config.Gateway[3] = IPABYTE_4;
}

// Start the webserver
void startWebServer()
{
	// Read the configuration
	ReadConfig();
	
	// Setup the webserver
	server.onNotFound ( []() {server.send ( 200, "text/html",  reinterpret_cast<const __FlashStringHelper *>(PAGE_main));   }  );
	server.on ( "/", []() {server.send ( 200, "text/html",  reinterpret_cast<const __FlashStringHelper *>(PAGE_main));   }  );	
	server.on ( "/admin/processMain", processMain);
	server.on ( "/admin/filldynamicdata", filldynamicdata );	
	server.on ( "/favicon.ico",   []() { server.send ( 200, "text/html", "" );   }  );
	server.on ( "/admin.html", []() { server.send ( 200, "text/html",  reinterpret_cast<const __FlashStringHelper *>(PAGE_AdminMainPage));   }  );
	server.on ( "/config.html", send_network_configuration_html );
	server.on ( "/main.html", processMain  );
	server.on ( "/main.html", []() { server.send ( 200, "text/html", PAGE_main );  } );
	server.on ( "/style.css", []() { server.send ( 200, "text/plain", reinterpret_cast<const __FlashStringHelper *>( PAGE_Style_css ));  } );
	server.on ( "/microajax.js", []() { server.send ( 200, "text/plain",  reinterpret_cast<const __FlashStringHelper *>(PAGE_microajax_js ));  } );
	server.on ( "/admin/values", send_network_configuration_values_html );
	server.on ( "/admin/connectionstate", send_connection_state_values_html );
	server.on ( "/admin/rstvalues", send_reset_values_html);
	
	// Start the webserver
	server.begin();
}

// Disable the webserver
void disableWebServer()
{
	// How to stop the webserver?
}

// Process the communication for the webserver
void runWebServer()
{
	server.handleClient();      
}

// Write actual configuration in the EEPROM
void WriteConfig()
{

	// Store the node ID
	Store_ID(STORE__DEFAULTID);
    
	// Store DHCP mode (Enabled or Disabled)
	if(config.dhcp)	Store_DHCPMode(SET_TRUE);
	else			Store_DHCPMode(SET_FALSE);

	// Store IP Address
	Store_StaticIPAddress(config.IP);

	// Store IP Subnet
	Store_StaticIPSubnet(config.Netmask);

	// Store IP Gateway
	Store_StaticIPGateway(config.Gateway);

	// Store Gateway/Peer mode
	if(config.RuntimeGateway)	Store_GatewayMode(SET_TRUE);
	else						Store_GatewayMode(SET_FALSE);
	
	// Store WiFi SSID and Password
	Store_SSID(config.ssid);
	Store_Password(config.password);

	// Commit changes
	Store_Commit();
}

// Read actual configuration from EEPROM
boolean ReadConfig()
{
	// If no configuration has been stored, use default values
	if(Return_ID()!=STORE__DEFAULTID)
	{
		defaultWebConfig();
		return false;
	}

	// By default reset it OFF
	config.rst = 0;
	
	// Read DHCP Mode 
	if(Return_DHCPMode()) 	config.dhcp = true;
	else					config.dhcp = false;
	
	// Read IP Address
	Return_StaticIPAddress(config.IP);

	// Read IP Subnet
	Return_StaticIPSubnet(config.Netmask);

	// Read IP Gateway
	Return_StaticIPGateway(config.Gateway);

	// Read Gateway/Peer Mode 
	if(Return_GatewayMode()) config.RuntimeGateway = true;
	else					 config.RuntimeGateway = false;

	// Read WiFi SSID and Password
	config.ssid = Read_SSID();
	config.password = Read_Password();

	return true;
}
