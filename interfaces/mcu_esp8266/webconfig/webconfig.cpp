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

ESP8266WebServer server(80);		// The Webserver
int AdminTimeOutCounter = 0;		// Counter for Disabling the AdminMode
boolean AdminEnabled = true;		// Enable Admin Mode for a given Time





void check_ESPMode()
{
	if (WiFi.status() != WL_CONNECTED);{
	  Serial.print("Connecting to AP");
	  for (int i=0; i < 10; i++){
		(WiFi.status() != WL_CONNECTED);
		delay(1000);
		LOG.println(".");
	  }
	}
	if (WiFi.status() == WL_CONNECTED) {
		LOG.print("Already connect to : ");
		LOG.println(WiFi.SSID());
		LOG.println("Admin Time Out");
		AdminTimeOutCounter=0;
		WiFi.mode(WIFI_STA);
		LOG.println( "STA mode started" );
		LOG.println(WiFi.localIP());
		LOG.println("Souliss update IP Address");
		GetIPAddress(); 
		delay(500);

	}
	else
	{
		LOG.println( "AP mode started coz' No WiFi or No Config" );
                WiFi.mode(WIFI_AP);
                WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
                WiFi.softAP(ACCESS_POINT_NAME);
                LOG.println(WiFi.softAPIP());
                dnsServer.start(DNS_PORT, "*", apIP);
                LOG.println(WiFi.softAPIP());
	}

}

void ConfigureWifi()
{
	LOG.println("Configuring Wifi");
	WiFi.begin (config.ssid.c_str(), config.password.c_str());
	check_ESPMode();

	if (!config.dhcp)
	{
		WiFi.config(IPAddress(config.IP[0],config.IP[1],config.IP[2],config.IP[3] ),  IPAddress(config.Gateway[0],config.Gateway[1],config.Gateway[2],config.Gateway[3] ) , IPAddress(config.Netmask[0],config.Netmask[1],config.Netmask[2],config.Netmask[3] ));
		LOG.println("Souliss update IP Address for STATIC");
		GetIPAddress();
		LOG.println(WiFi.localIP());
	}
       	check_ESPMode();
}

void WriteConfig()
{

	LOG.println("Writing Config");
	EEPROM.write(0,'C');
	EEPROM.write(1,'F');
	EEPROM.write(2,'G');
	EEPROM.write(3,config.rst);
        
	EEPROM.write(16,config.dhcp);
	
	EEPROM.write(32,config.IP[0]);
	EEPROM.write(33,config.IP[1]);
	EEPROM.write(34,config.IP[2]);
	EEPROM.write(35,config.IP[3]);

	EEPROM.write(36,config.Netmask[0]);
	EEPROM.write(37,config.Netmask[1]);
	EEPROM.write(38,config.Netmask[2]);
	EEPROM.write(39,config.Netmask[3]);

	EEPROM.write(40,config.Gateway[0]);
	EEPROM.write(41,config.Gateway[1]);
	EEPROM.write(42,config.Gateway[2]);
	EEPROM.write(43,config.Gateway[3]);
	
	EEPROM.write(50,config.NodeMode);

	WriteStringToEEPROM(100,config.ssid);
	WriteStringToEEPROM(132,config.password);

	EEPROM.commit();
	
}
boolean ReadConfig()
{

	LOG.println("Reading Configuration");
	if (EEPROM.read(0) == 'C' && EEPROM.read(1) == 'F'  && EEPROM.read(2) == 'G' )
	{
  		LOG.println("Configuration Found!");
		config.rst = EEPROM.read(3);
                
		config.dhcp = 	EEPROM.read(16);

		config.IP[0] = EEPROM.read(32);
		config.IP[1] = EEPROM.read(33);
		config.IP[2] = EEPROM.read(34);
		config.IP[3] = EEPROM.read(35);
		config.Netmask[0] = EEPROM.read(36);
		config.Netmask[1] = EEPROM.read(37);
		config.Netmask[2] = EEPROM.read(38);
		config.Netmask[3] = EEPROM.read(39);
		config.Gateway[0] = EEPROM.read(40);
		config.Gateway[1] = EEPROM.read(41);
		config.Gateway[2] = EEPROM.read(42);
		config.Gateway[3] = EEPROM.read(43);
		
		config.NodeMode = EEPROM.read(50);
		
		config.ssid = ReadStringFromEEPROM(100);
		config.password = ReadStringFromEEPROM(132);
		
		return true;
	
	}
	else
	{
		LOG.println("Configuration NOT FOUND!!!!");
		return false;
	}
}
