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

#ifndef HOST_NAME_INSKETCH				
#	define HOST_NAME  		"souliss"
#endif

#define OTA_Setup()             WiFiServer TelnetServer(8266);                      \
                                WiFiClient Telnet;                                  \
                                WiFiUDP OTA

#define OTA_Init()              if(WiFi.waitForConnectResult() == WL_CONNECTED){    \
                                MDNS.begin(HOST_NAME);                              \
                                MDNS.addService("arduino", "tcp", 8266);            \
                                OTA.begin(8266);     	                            \
                                TelnetServer.begin();                               \
                                TelnetServer.setNoDelay(true);}

#define OTA_Process()           if (OTA.parsePacket()) {                            \
                                    IPAddress remote = OTA.remoteIP();              \
                                    int cmd  = OTA.parseInt();                      \
                                    int port = OTA.parseInt();                      \
                                    int size   = OTA.parseInt();                    \
                                    if(!Update.begin(size)) return;                 \
                                    WiFiClient client;                              \
                                    if (client.connect(remote, port)) {             \
                                    uint32_t written;                               \
                                    while(!Update.isFinished()){                    \
                                        written = Update.write(client);             \
                                        if(written > 0) client.print(written, DEC); \
                                    }                                               \
                                    if(Update.end()){                               \
                                        client.println("OK");                       \
                                        ESP.restart();                              \
                                    } else {                                        \
                                        Update.printError(client);                  \
                                    }                                               \
                                    } else {                                        \
                                    }                                               \
                                }                                                   \
                                if (TelnetServer.hasClient()){                      \
                                    if (!Telnet || !Telnet.connected()){            \
                                    if(Telnet) Telnet.stop();                       \
                                    Telnet = TelnetServer.available();              \
                                    } else {                                        \
                                    WiFiClient toKill = TelnetServer.available();   \
                                    toKill.stop();                                  \
                                    }                                               \
                                }                                                   \

