/**************************************************************************
    Souliss - Analog Measure
    
    An analog value is acquired throught Souliss and available at the user
    interfaces as half-precision floating point. All calculation shall be 
    performed as standard floating point and at the end converted in half
    precision using :
        void AnalogIn(U8 pin, U8 slot);
    
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

#define ANALOGDAQ               0           // This is the memory slot used for the execution of the logic
#define DEADBAND                0.05        // Deadband value 5%  

void setup()
{   
    Initialize();

    // Get the IP address from DHCP
    GetIPAddress();                          
    SetAsGateway(myvNet_dhcp);                  // Set this node as gateway for SoulissApp  
    Set_AnalogIn(ANALOGDAQ);                    // Set an analog input value
}

void loop()
{ 
    // Here we start to play
    EXECUTEFAST()
    {                     
        UPDATEFAST();   
        
        // Execute the logic to report the analog value to the user interface         
        FAST_110ms()
        {
            // Compare the acquired input with the stored one, send the new value to the
            // user interface if the difference is greater than the dead-band
            Read_AnalogIn(ANALOGDAQ);
        }
        
        // Process data communication
        FAST_GatewayComms();
    }
    EXECUTESPEEDY()
    {
        UPDATESPEEDY();
      
        // Execute the code every 3 times of free MCU       
        SPEEDY_x(3)
        {
        
            // Acquire data from the microcontroller ADC
            AnalogIn(A0, ANALOGDAQ, 0.09, 0);   // The raw data is 0-1024, scaled as 0-100% without bias (100 / 1024 = 0.09)
        } 
        
    }       
} 
