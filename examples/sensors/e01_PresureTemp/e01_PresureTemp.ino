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
#include <Wire.h>
#include <Adafruit_BMP085.h>

/*** All configuration includes should be above this line ***/ 
#include "Souliss.h"

#define PRESSURE0             0               // Leave 2 slots for T58
#define TEMP0                 2               // Leave 2 slots for T52

// BPM180 sensor
Adafruit_BMP085 bmp;

void setup()
{   
    Initialize();

    // Get the IP address from DHCP
    GetIPAddress();                          
    SetAsGateway(myvNet_dhcp);                  // Set this node as gateway for SoulissApp                        

    Set_Pressure(PRESSURE0);
    Set_Temperature(TEMP0);
}

void loop()
{ 
    // Here we start to play
    EXECUTEFAST() {                     
        UPDATEFAST();   

        // Execute the code every 1 time_base_fast      
        FAST_10ms() {
          
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

            Logic_Pressure(PRESSURE0);
            Logic_Temperature(TEMP0);
                     
        }
        
        SLOW_50s()  {
          float pressure = bmp.readPressure()/100;
          float temperature = bmp.readTemperature();
          ImportAnalog(PRESSURE0, &pressure);
          ImportAnalog(TEMP0, &temperature);
        }   
    }       
} 
