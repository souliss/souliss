/**************************************************************************
    Souliss - RGB LED Strip
    
    Control and RGB LED or a Strip using the PWM, fade in/out and flash 
    effects are available as the Android interface.
 
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

#define LEDCONTROL          0               // This is the memory slot for the logic that handle the light
#define LEDINTENSITY        1               // This is the memory slot for the logic that handle the light

void setup()
{   
    Initialize();

    // Get the IP address from DHCP
    GetIPAddress();                          
    SetAsGateway(myvNet_dhcp);                  // Set this node as gateway for SoulissApp                        
                
    Set_DimmableLight(LEDCONTROL);                  // Set a logic to control a LED strip
    
    // Define inputs, outputs pins
    pinMode(2, INPUT);                  // Hardware pulldown required
    pinMode(3, OUTPUT);                 // Power the LED   
}

void loop()
{ 
    // Here we start to play
    EXECUTEFAST() {                     
        UPDATEFAST();   

        // Execute the code every 1 time_base_fast      
        FAST_10ms() {
        
            // Use Pin2 as command
            DigIn(2, Souliss_T1n_ToggleCmd, LEDCONTROL);        
            
            // Execute the logic that handle the LED
            Logic_DimmableLight(LEDCONTROL);

            // Use the output values to control the PWM
            analogWrite(3, mOutput(LEDINTENSITY));

            // Just process communication as fast as the logics
            ProcessCommunication();
        } 

        // Process the other Gateway stuffs
        FAST_GatewayComms();          
    }
    EXECUTESLOW()
    {   
        UPDATESLOW();

        SLOW_10s()  {
        
            // The timer handle timed-on states
            Timer_DimmableLight(LEDCONTROL);                        
        }     
    }       
} 
