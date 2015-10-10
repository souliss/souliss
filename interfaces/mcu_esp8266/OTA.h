
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
	
	Modified by Dario Di Maio, original code from 
	   https://github.com/esp8266/Arduino
	
***************************************************************************/
/*!
    \file 
    \ingroup
*/

#define OTA_Setup()               WiFiServer TelnetServer(8266); \
                                  WiFiClient Telnet;              \
                                  WiFiUDP OTA
                                  
#define OTA_Initialization()      if(WiFi.waitForConnectResult() == WL_CONNECTED){ \
                                  MDNS.begin(host);                                \
                                  MDNS.addService("arduino", "tcp", 8266);         \
                                  OTA.begin(aport);                                \
                                  TelnetServer.begin();                            \        
                                  TelnetServer.setNoDelay(true);                   \
                                  Serial.print("IP address: ");                    \
                                  Serial.println(WiFi.localIP());}
                                  
