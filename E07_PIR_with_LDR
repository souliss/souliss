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


// Configure the framework
#include "bconf/StandardArduino.h"          // Use a standard Arduino
#include "conf/ethW5100.h"                  // Ethernet through Wiznet W5100
#include "conf/Gateway.h"                   // The main node is the Gateway, we have just one node
#include "conf/Webhook.h"                   // Enable DHCP and DNS

// Include framework code and libraries
#include <SPI.h>
#include "Souliss.h"

#define PIR                      0            
#define LDR                      1
                  
#define LIGHT_ON_CYCLE          10          // Light ON for 10 cycles if triggered by a PIR sensor

#define LDR_THRESOLD           500

//DEFINICION DE PINES
#define LIGHT_PIN                8
#define BUTTON_PIN               2
#define PIR_PIN                  9
#define LDR_PIN                 A0

void setup()
{   
    Initialize();

    // Get the IP address from DHCP
    GetIPAddress();                          
    SetAsGateway(myvNet_dhcp);       // Set this node as gateway for SoulissApp  

    // Set the typical logic to handle the lights
    Set_AutoLight(PIR);
    Set_T54(LDR);
    
    // Define inputs, outputs pins
    pinMode(LDR_PIN, INPUT);
    pinMode(PIR_PIN, INPUT);
    pinMode(BUTTON_PIN, INPUT);                  // Hardware pulldown required
    pinMode(LIGHT_PIN, OUTPUT);                 // Power to relay coil for light 1
}

void loop()
{   
    // Here we start to play
    EXECUTEFAST() {                     
        UPDATEFAST();   
        
        // Execute the code every 3 time_base_fast      
        FAST_90ms() {
        
            //Auto Mode when LDR <= LDR_THRESOLD
            if(mOutput(PIR) != Souliss_T1n_AutoCmd && mOutputasFloat(LDR) <= LDR_THRESOLD)
                mInput(PIR) = Souliss_T1n_AutoCmd;
            
            //Off Mode when LDR >= LDR_THRESOLD
            if(mOutput(PIR) == Souliss_T1n_AutoCmd && mOutputasFloat(LDR) >= LDR_THRESOLD)
                mInput(PIR) == Souliss_T1n_OffCmd;
                
            // Use BUTTON_PIN to Toggle On/Off
            DigIn(BUTTON_PIN, Souliss_T1n_ToggleCmd, PIR); 
            
            // Use PIR_PIN to enable on Presence detection and Auto_mode ON
            DigIn(PIR_PIN, LIGHT_ON_CYCLE, PIR);
            
            // Execute the logic
            Logic_AutoLight(PIR);
            Logic_T54(LDR);
            
            // Use Pin8 and Pin 9 as output on the electrical load
            nDigOut(LIGHT_PIN, Souliss_T1n_Coil, PIR); 
        } 
        FAST_910ms(){
            AnalogIn(LDR_PIN, LDR, 0.09, 0);
        }
        
        // Process data communication
        FAST_GatewayComms();
        
    }
    
    EXECUTESLOW()   {   
        UPDATESLOW();

        SLOW_10s()  {          
        
            // The timer handle timed-on states
            Timer_AutoLight(PIR);
        }     
    }       
} 

