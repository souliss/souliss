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
	Modified by Dario Cdj for Notify System integrated (Pushetta, Pushover and Telegram) 

***/

#include "webconfig.h"

#ifndef ASYNCWEBSERVER
ESP8266WebServer server(HTTP_PORT);	// The Webserver
#else
AsyncWebServer server(HTTP_PORT);	// The Webserver
#endif

int AdminTimeOutCounter = 0;		// Counter for Disabling the AdminMode
boolean AdminEnabled = true;		// Enable Admin Mode for a given Time

#include "src/root.h"
#include "src/admin.h"
#include "src/script.js.h"
#include "src/style.css.h"
#include "src/main.h"
#include "src/netconfig.h"
#include "src/notify.h"

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
#ifndef ASYNCWEBSERVER
	server.onNotFound ( []() {server.send ( 200, "text/html",  reinterpret_cast<const __FlashStringHelper *>(PAGE_main));   }  );
	server.on ( "/", []() {server.send ( 200, "text/html",  reinterpret_cast<const __FlashStringHelper *>(PAGE_main));   }  );	
#else
	server.onNotFound ( [](AsyncWebServerRequest *request) {request->send ( 200, "text/html",  reinterpret_cast<const __FlashStringHelper *>(PAGE_main));   }  );
	server.on ( "/", [](AsyncWebServerRequest *request) {request->send ( 200, "text/html",  reinterpret_cast<const __FlashStringHelper *>(PAGE_main));   }  );	
#endif	
	server.on ( "/admin/processMain", processMain);
	server.on ( "/admin/filldynamicdata", filldynamicdata );	
#ifndef ASYNCWEBSERVER
	server.on ( "/favicon.ico",   []() { server.send ( 200, "text/html", "" );   }  );
	server.on ( "/admin.html", []() { server.send ( 200, "text/html",  reinterpret_cast<const __FlashStringHelper *>(PAGE_AdminMainPage));   }  );
#else
	server.on ( "/favicon.ico",   [](AsyncWebServerRequest *request) { request->send ( 200, "text/html", "" );   }  );
	server.on ( "/admin.html", [](AsyncWebServerRequest *request) { request->send ( 200, "text/html",  reinterpret_cast<const __FlashStringHelper *>(PAGE_AdminMainPage));   }  );
#endif
	server.on ( "/config.html", send_network_configuration_html );
	server.on ( "/notify.html", send_notify_settings_html );
	
	server.on ( "/main.html", processMain  );
#ifndef ASYNCWEBSERVER
	server.on ( "/main.html", []() { server.send ( 200, "text/html", PAGE_main );  } );
	server.on ( "/style.css", []() { server.send ( 200, "text/plain", reinterpret_cast<const __FlashStringHelper *>( PAGE_Style_css ));  } );
	server.on ( "/microajax.js", []() { server.send ( 200, "text/plain",  reinterpret_cast<const __FlashStringHelper *>(PAGE_microajax_js ));  } );
#else
	server.on ( "/main.html", [](AsyncWebServerRequest *request) { request->send ( 200, "text/html", PAGE_main );  } );
	server.on ( "/style.css", [](AsyncWebServerRequest *request) { request->send ( 200, "text/plain", reinterpret_cast<const __FlashStringHelper *>( PAGE_Style_css ));  } );
	server.on ( "/microajax.js", [](AsyncWebServerRequest *request) { request->send ( 200, "text/plain",  reinterpret_cast<const __FlashStringHelper *>(PAGE_microajax_js ));  } );
#endif	
	server.on ( "/admin/values", send_network_configuration_values_html );
	server.on ( "/admin/notifyvalues", send_notify_settings_values_html );
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
#ifndef ASYNCWEBSERVER
	server.handleClient();      
#endif
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
	
	// Store Pushetta 
	Store_Pushetta_ApiKey 		(pushetta.pushettaapikey);
	Store_Pushetta_Channel 		(pushetta.pushettachannel);
	if(pushetta.pushettaenabled)	Store_PushettaEnabled(SET_TRUE);
	else							Store_PushettaEnabled(SET_FALSE);

	// Store Pushover 
	Store_Pushover_ApiToken  	(pushover.pushoverapitoken);
	Store_Pushover_UserKey 		(pushover.pushoveruserkey);
	Store_Pushover_Device  		(pushover.pushoverdevice);
	Store_Pushover_Sound   		(pushover.pushoversound);
	if(pushover.pushoverenabled)	Store_PushoverEnabled(SET_TRUE);
	else							Store_PushoverEnabled(SET_FALSE);

	// Store Telegram 
	Store_Telegram_BOTtoken 	(telegram.telegrambottoken);
	Store_Telegram_ChatGroup 	(telegram.telegramchatgroup);
	Store_Telegram_ChatID 		(telegram.telegramchatid);
	if(telegram.telegramenabled)	Store_TelegramEnabled(SET_TRUE);
	else							Store_TelegramEnabled(SET_FALSE);

	
	
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

	// Read Notify Settings
	pushetta.pushettaapikey = Read_Pushetta_ApiKey();
	pushetta.pushettachannel= Read_Pushetta_Channel();
	if(Return_PushettaEnabled()) pushetta.pushettaenabled = true;
	else						 pushetta.pushettaenabled = false;
	
	pushover.pushoverapitoken = Read_Pushover_ApiToken();
	pushover.pushoveruserkey = Read_Pushover_UserKey();
	pushover.pushoverdevice = Read_Pushover_Device();
	pushover.pushoversound = Read_Pushover_Sound();
	if(Return_PushoverEnabled()) pushover.pushoverenabled = true;
	else						 pushover.pushoverenabled = false;
	
	telegram.telegrambottoken = Read_Telegram_BOTtoken();
	telegram.telegramchatgroup = Read_Telegram_ChatGroup();
	telegram.telegramchatid = Read_Telegram_ChatID();
	if(Return_TelegramEnabled()) telegram.telegramenabled = true;
	else						 telegram.telegramenabled = false;
		
	return true;
}
