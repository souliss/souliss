/**************************************************************************
    Souliss - ESP Juan Pinto Configurable Board.
    
    This is the basic example, create a software push-button on Android
    using SoulissApp (get it from Play Store).  
    
    Load this code on ESP8266 board using the porting of the Arduino core
    for this platform.
        
***************************************************************************/
    #define MaCaco_DEBUG_INSKETCH
    #define MaCaco_DEBUG          1
    
    #define VNET_DEBUG_INSKETCH
    #define VNET_DEBUG  	  1
//Changed Webinterface with the new one
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <Ticker.h>
#include <EEPROM.h>
#include <WiFiUdp.h>

const byte DNS_PORT = 53;
IPAddress apIP(192, 168, 4, 1);
DNSServer dnsServer;

#define ACCESS_POINT_NAME  "Souliss"				
//#define ACCESS_POINT_PASSWORD  "12345678" 

#define LOG Serial

// Configure the framework
#include "bconf/MCU_ESP8266.h"              // Load the code directly on the ESP8266
#include "conf/Gateway.h"                   // The main node is the Gateway, we have just one node
#include "conf/DynamicAddressing.h"  

//************** Include framework code and libraries *************************
#include "conf/IPBroadcast.h"
#include "Souliss.h"
   
    //NEW WEBINTERFACE INCLUDES
    #include "helpers.h"
    #include "global.h"
    //Include the HTML, STYLE and Script "Pages"
    #include "Page_Root.h"
    #include "Page_Admin.h"
    #include "Page_Script.js.h"
    #include "Page_Style.css.h"
    #include "PAGE_NetworkConfiguration.h"
    #include "main.h"
    
    
    #define AdminTimeOut 300  // Defines the Time in Seconds, when the Admin-Mode will be diabled
    
    #define MYLED 0

void setup()
{
    EEPROM.begin(STORE__SIZE);
    LOG.begin(115200);
    delay(500);   
   
/*    for(int i=0;i<STORE__SIZE;i++)
    {	EEPROM.write(i, 255);   }
    EEPROM.commit(); 
*/
    LOG.println("Starting Souliss Node");
	if (!ReadConfig())
	{
		// DEFAULT CONFIG
		config.ssid = "MYSSID";
		config.password = "MYPASSWORD";
		config.dhcp = true;
		config.IP[0] = 192;config.IP[1] = 168;config.IP[2] = 1;config.IP[3] = 100;
		config.Netmask[0] = 255;config.Netmask[1] = 255;config.Netmask[2] = 255;config.Netmask[3] = 0;
		config.Gateway[0] = 192;config.Gateway[1] = 168;config.Gateway[2] = 1;config.Gateway[3] = 1;
		WriteConfig();
		LOG.println("General config applied");
	}
	
	
	if (AdminEnabled)
	{
                LOG.println( "Admin Enabled" );
		LOG.println( "AP mode started" );
                WiFi.mode(WIFI_AP);
                WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
                WiFi.softAP(ACCESS_POINT_NAME);
                LOG.println(WiFi.softAPIP());
                dnsServer.start(DNS_PORT, "*", apIP);

	}
	else
	{
		LOG.println( "STA mode started" );
                WiFi.mode(WIFI_STA);
                LOG.println(WiFi.localIP());
	}
    
    ConfigureWifi();
    
/***************** INITIALIZE NETWORK AND SENSORS *********************/
    Initialize();
    GetIPAddress();
    
  		server.onNotFound ( []() { LOG.println("admin.html"); server.send ( 200, "text/html",  reinterpret_cast<const __FlashStringHelper *>(PAGE_AdminMainPage));   }  );
                server.on ( "/", []() { LOG.println("admin.html"); server.send ( 200, "text/html",  reinterpret_cast<const __FlashStringHelper *>(PAGE_AdminMainPage));   }  );	
                server.on ( "/admin/processMain", processMain);
            	server.on ( "/admin/filldynamicdata", filldynamicdata );	
              	server.on ( "/favicon.ico",   []() { LOG.println("favicon.ico"); server.send ( 200, "text/html", "" );   }  );
              	server.on ( "/admin.html", []() { LOG.println("admin.html"); server.send ( 200, "text/html",  reinterpret_cast<const __FlashStringHelper *>(PAGE_AdminMainPage));   }  );
         	server.on ( "/config.html", send_network_configuration_html );
                server.on ( "/main.html", processMain  );
              	server.on ( "/main.html", []() { server.send ( 200, "text/html", PAGE_main );  } );
              	server.on ( "/style.css", []() { LOG.println("style.css"); server.send ( 200, "text/plain", reinterpret_cast<const __FlashStringHelper *>( PAGE_Style_css ));  } );
              	server.on ( "/microajax.js", []() { LOG.println("microajax.js"); server.send ( 200, "text/plain",  reinterpret_cast<const __FlashStringHelper *>(PAGE_microajax_js ));  } );
              	server.on ( "/admin/values", send_network_configuration_values_html );
              	server.on ( "/admin/connectionstate", send_connection_state_values_html );
              	server.on ( "/admin/rstvalues", send_reset_values_html);
              	server.begin();
              	LOG.println( "HTTP server started" );

    Souliss_Node_Start();
    
    Set_DimmableLight(MYLED);
    
}

void loop()
{ 
     if (AdminEnabled)
	{
		if (AdminTimeOutCounter > AdminTimeOut)
		{
			AdminEnabled = false;
			LOG.println("Admin Mode disabled!");
			WiFi.mode(WIFI_STA);
                        LOG.println( "STA mode started" );
                        LOG.println(WiFi.localIP());
		}
	}
	//if(nowifi) 
        dnsServer.processNextRequest();
        server.handleClient();
        
    //**************************************************************************************************
    EXECUTEFAST() {                     
        UPDATEFAST();   
        
        FAST_50ms() {   // We process the logic and relevant input and output every 50 milliseconds
             Logic_DimmableLight(MYLED);                        
             analogWrite(15, mOutput(MYLED+1)*4);
        }
      
        if (config.NodeMode){
           // Here we handle here the communication with Android as Gateway
            FAST_GatewayComms(); 
        }
        else { 
            // Here we handle here the communication with Android as Peer
            FAST_PeerComms(); 
        }    
    }
    EXECUTESLOW() {
	UPDATESLOW();

            SLOW_10s() {  // Read temperature and humidity from DHT every 110 seconds  
                Timer_DimmableLight(MYLED);   
            }
            if (config.NodeMode){
                //nothing here
            }
            else {
                SLOW_PeerJoin();
            }
       } 
      
       if (config.NodeMode){
           //nothing here
       }
       else {
           START_PeerJoin(); //tell gateway that i am exist
       }   
}    
