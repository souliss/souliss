/**************************************************************************
    Souliss - LYT Light Bulb
    
    This sketch control the Authometion LYT bubls through the Authometion
    shield.
    
    Load this sketch on an ESP8266 WiFi module used to bridge via USART the
    Arduino AVR microcontroller that handle the radio.

    Verify shield's jumpers and select Hardware Usart while using this sketch, 
    remember to remove the jumpers before programming the Arduino. Use pin 10 as
    chip select for the radio.
        
***************************************************************************/

// Include framework code and libraries
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>
#include <WiFiUdp.h>

// Configure the framework
#include "bconf/MCU_ESP8266.h"              // Load the code directly on the ESP8266
#include "conf/Gateway.h"                   // The main node is the Gateway, we have just one node
#include "conf/usart_fast.h"                // USART / RS485 transceiver
#include "conf/DynamicAddressing.h"         // Use dynamically assigned addresses
#include "conf/WEBCONFinterface.h"          // Enable the WebConfig interface

#include "Souliss.h"

#define	PUSH1	0
#define	PUSH2	1
#define	LowMood									0xFF01,0x01
#define	FullLight								0xFF01,0x02

void setup()
{   
    Initialize();

    // Read the IP configuration from the EEPROM and wait for a connection
    while(!ReadIPConfiguration() && (millis() < 60000)) 
    {   
        delay(3000);
    }

    // If the access point is not available, start itself as access point and
    // wait for a configuration
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

    // Set as Gateway
    SetAsGateway(myvNet_dhcp);       // Set this node as gateway for SoulissApp  

    // This is the vNet address for this node, used to communicate with other
    // nodes in your Souliss network
    SetAddress(0xCE01, 0xFF00, 0x0000);
    SetAddress(0xAB01, 0xFF00, 0x0000);
    SetAsPeerNode(0xCE02, 1);

	Set_T14(PUSH1);
	Set_T14(PUSH2);
}

void loop()
{ 
    // Here we start to play
    EXECUTEFAST() {                     
        UPDATEFAST();   
             
        // Process the communication basic at max speed, this allow smooth handling of color and music synch                                
        ProcessCommunication(); 

		FAST_110ms() {

			// LowMood Command
			if(Logic_T14(PUSH1))
				if(mOutput(PUSH1))	publish(LowMood);

			// Full Light Command
			if(Logic_T14(PUSH2))
				if(mOutput(PUSH2))	publish(FullLight);
		}
            
        // Complete the communication tasks at normal rate
        FAST_GatewayComms();


    }
}   