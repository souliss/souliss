/**************************************************************************
   Souliss - Web Configuration
    
    This example demonstrate a complete web configuration of ESP8266 based
	nodes, the node starts as access point and allow though a web interface
	the configuration of IP and Souliss parameters (Gateway / Peer mode).

	Once configured the IP and Souliss parameters, the node will be recognized
	by SoulissApp and will be available in the Arduino IDE for OTA (Over the Air)
	programming through its IP address.
	
	This example is only supported on ESP8266.
 
***************************************************************************/

// Let the IDE point to the Souliss framework
#include "SoulissFramework.h"

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <EEPROM.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

// Configure the Souliss framework
#include "bconf/MCU_ESP8266.h"              // Load the code directly on the ESP8266
#include "conf/RuntimeGateway.h"            // This node is a Peer and can became a Gateway at runtime
#include "conf/DynamicAddressing.h"         // Use dynamically assigned addresses
#include "conf/WEBCONFinterface.h"          // Enable the WebConfig interface


/*** All configuration includes should be above this line ***/ 
#include "Souliss.h"
   
#define MYLED               0
uint8_t GPIO_POUT = 15;  //GPIO15

void setup()
{
    Initialize();
    
    // Read the IP configuration from the EEPROM, if not available start
    // the node as access point
    if(!ReadIPConfiguration()) 
	{	
		// Start the node as access point with a configuration WebServer
		SetAccessPoint();
		startWebServer();

		// We have nothing more than the WebServer for the configuration
		// to run, once configured the node will quit this.
		while(1)
		{
			yield();
			runWebServer(); 
		}

	}

    if (IsRuntimeGateway())
    {
        // Connect to the WiFi network and get an address from DHCP                      
        SetAsGateway(myvNet_dhcp);       // Set this node as gateway for SoulissApp  
        SetAddressingServer();
    }
    else 
    {
        // This board request an address to the gateway at runtime, no need
        // to configure any parameter here.
        SetDynamicAddressing();  
        GetAddress();
    }
    
    Set_DimmableLight(MYLED);
	
    // Init the OTA
    ArduinoOTA.setHostname("souliss-nodename");    
    ArduinoOTA.begin(); 
}

void loop()
{  
    EXECUTEFAST() {                     
        UPDATEFAST();   
        
        FAST_50ms() {   // We process the logic and relevant input and output every 50 milliseconds
             Logic_DimmableLight(MYLED);                        
             analogWrite(GPIO_POUT, mOutput(MYLED+1)*4);
        }
      
        // Run communication as Gateway or Peer
        if (IsRuntimeGateway())
            FAST_GatewayComms(); 
        else 
            FAST_PeerComms();   
    }

    EXECUTESLOW() {
        UPDATESLOW();

        SLOW_10s() {  // Read temperature and humidity from DHT every 110 seconds  
                Timer_DimmableLight(MYLED);   
        }
        
        // If running as Peer
        if (!IsRuntimeGateway())
            SLOW_PeerJoin();
    } 

    // Look for a new sketch to update over the air
    ArduinoOTA.handle();	
}    
