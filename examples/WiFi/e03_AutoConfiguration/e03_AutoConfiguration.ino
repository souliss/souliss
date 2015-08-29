/**************************************************************************
        
***************************************************************************/

#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>
#include <WiFiUdp.h>

// Configure the Souliss framework
#include "bconf/MCU_ESP8266.h"              // Load the code directly on the ESP8266
#include "conf/RuntimeGateway.h"            // This node is a Peer and can became a Gateway at runtime
#include "conf/DynamicAddressing.h"         // Use dynamically assigned addresses
#include "conf/WEBCONFinterface.h"          // Enable the WebConfig interface

#include "Souliss.h"
   
#define MYLED               0

void setup()
{
    Initialize();
    
    // Read the IP configuration from the EEPROM, if not available start
    // the node as access point
    if(!ReadIPConfiguration())  startAccessPoint();

    // Start the configuration WebServer
    startWebServer();

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
    
}

void loop()
{  
    EXECUTEFAST() {                     
        UPDATEFAST();   
        
        FAST_50ms() {   // We process the logic and relevant input and output every 50 milliseconds
             Logic_DimmableLight(MYLED);                        
             analogWrite(15, mOutput(MYLED+1)*4);
        }

        // Process the WebConfig and DNS servers
        FAST_510ms() {
            dnsServer.processNextRequest();
            server.handleClient();      
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
}    
