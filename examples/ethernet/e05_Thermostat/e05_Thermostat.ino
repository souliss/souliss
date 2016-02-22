/**************************************************************************
    Souliss - Thermostat
    
    Control the heating or cooling using a temperature measure and a
    setpoint, based on the percentage distance from the setpoint runs the
    fans.
    
    The temperature measure is acquired through Souliss and available at the
    user interfaces as half-precision floating point. All calculation shall be 
    performed as standard floating point and at the end converted in half
    precision using :
        void Souliss_AnalogIn(U8 pin, U8 *memory_map, U8 slot, 
                                float scaling, float bias);
    
    Is suggested a scaling to the vRef of the AVR's ADC, using an external
    amplification circuit, in order to use the whole resolution.
    
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

#define ANALOGDAQ               0           // This is the memory slot used for the execution of the logic in network_address1
#define DEADBAND                0.05        // Deadband value 5%  

void setup()
{   
    Initialize();

    // Get the IP address from DHCP
    GetIPAddress();                          
    SetAsGateway(myvNet_dhcp);          // Set this node as gateway for SoulissApp  
    Set_Thermostat(ANALOGDAQ);          // Set a logic for the thermostat
    
    // Define output pins
    pinMode(2, OUTPUT);     // Heater (ON/OFF)
    pinMode(3, OUTPUT);     // Fan 1
    pinMode(4, OUTPUT);     // Fan 2
    pinMode(5, OUTPUT);     // Fan 3
}

void loop()
{ 
    // Here we start to play
    EXECUTEFAST() {                     
        UPDATEFAST();   
        
        // Execute the code every 11 time_base_fast       
        FAST_110ms() {  
            // The logic is executed faster than the data acquisition just to have a 
            // prompt answer to the user commands, but the real rate is defined by
            // the temperature measure rate.
        
            // Compare the setpoint from the user interface with the measured temperature
            Logic_Thermostat(ANALOGDAQ);
            
            // Start the heater and the fans
            nDigOut(2, Souliss_T3n_HeatingOn, ANALOGDAQ);   // Heater
            nDigOut(3, Souliss_T3n_FanOn1   , ANALOGDAQ);   // Fan1
            nDigOut(4, Souliss_T3n_FanOn2   , ANALOGDAQ);   // Fan2
            nDigOut(5, Souliss_T3n_FanOn3   , ANALOGDAQ);   // Fan3
            
            // We are not handling the cooling mode, if enabled by the user, force it back
            // to disable
            if(mOutput(ANALOGDAQ) & Souliss_T3n_CoolingOn)
                mOutput(ANALOGDAQ) &= ~Souliss_T3n_CoolingOn;
        }
        
        // Execute the code every 101 time_base_fast          
        FAST_910ms()    {
        
            // Acquire temperature from the microcontroller ADC
            AnalogIn(A0, ANALOGDAQ+1, 0.04, -10); // The raw data is 0-1024, scaled as -10 to +30 Celsius
        }       
        
        // Process data communication
        FAST_GatewayComms();
    }       
} 
