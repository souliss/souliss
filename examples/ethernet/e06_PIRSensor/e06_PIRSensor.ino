/**************************************************************************
    Souliss - Light with PIR Sensor
    
    This example handle two lights as previous example but add an external
    event (like a PIR sensor) to time-on the light in the active state.
    
    If the device is set in AUTO mode, the external event drive the output
    state, is always possible to control manually the light.    

    Run this code on one of the following boards:
      - Arduino Ethernet (W5100) 
      - Arduino with Ethernet Shield (W5100)
      
    As option you can run the same code on the following, just changing the
    relevant configuration file at begin of the sketch
      - Arduino with ENC28J60 Ethernet Shield
      - Arduino with W5200 Ethernet Shield
      - Arduino with W5500 Ethernet Shield
        
***************************************************************************/

// Let the IDE point to the Souliss framework
#include "SoulissFramework.h"

// Configure the framework
#include "bconf/StandardArduino.h"          // Use a standard Arduino
#include "conf/ethW5100.h"                  // Ethernet through Wiznet W5100
#include "conf/Gateway.h"                   // The main node is the Gateway, we have just one node
#include "conf/Webhook.h"                   // Enable DHCP and DNS

// Include framework code and libraries
#include <SPI.h>

/*** All configuration includes should be above this line ***/ 
#include "Souliss.h"

#define LIGHT1_NODE1            0            
#define LIGHT2_NODE1            1                    
#define LIGHT_ON_CYCLE          10          // Light ON for 10 cycles if triggered by a PIR sensor

void setup()
{   
    Initialize();

    // Get the IP address from DHCP
    GetIPAddress();                          
    SetAsGateway(myvNet_dhcp);       // Set this node as gateway for SoulissApp  

    // Set the typical logic to handle the lights
    Set_AutoLight(LIGHT1_NODE1);
    Set_AutoLight(LIGHT2_NODE1);
    
    // Define inputs, outputs pins
    pinMode(2, INPUT);                  // Hardware pulldown required
    pinMode(3, INPUT);                  // Hardware pulldown required   
    pinMode(8, OUTPUT);                 // Power to relay coil for light 1
    pinMode(9, OUTPUT);                 // Power to relay coil for light 2
}

void loop()
{   
    // Here we start to play
    EXECUTEFAST() {                     
        UPDATEFAST();   
        
        // Execute the code every 3 time_base_fast      
        FAST_90ms() {
        
            // Use Pin2 and Pin 3 as ON/OFF command
            DigIn(2, LIGHT_ON_CYCLE, LIGHT1_NODE1); 
            DigIn(3, LIGHT_ON_CYCLE, LIGHT2_NODE1);     
            
            // Execute the logic
            Logic_AutoLight(LIGHT1_NODE1);
            Logic_AutoLight(LIGHT2_NODE1);

            // Use Pin8 and Pin 9 as output on the electrical load
            nDigOut(8, Souliss_T1n_Coil, LIGHT1_NODE1); 
            nDigOut(9, Souliss_T1n_Coil, LIGHT2_NODE1);                     
        } 
        
        // Process data communication
        FAST_GatewayComms();
        
    }
    
    EXECUTESLOW()   {   
        UPDATESLOW();

        SLOW_10s()  {          
        
            // The timer handle timed-on states
            Timer_AutoLight(LIGHT1_NODE1);
            Timer_AutoLight(LIGHT2_NODE1);                      
        }     
    }       
} 
