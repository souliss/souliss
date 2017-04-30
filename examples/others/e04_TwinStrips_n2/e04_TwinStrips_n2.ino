/**************************************************************************
    Souliss - Twin RGB LED Strip
    
    Control a couple of RGB LED or a Strip using the RGB Shield and Ethernet 
    Shield from Open Electronics, control bright and color from an Android 
    smartphone and additionaly with an hardware switch.
 
    Brightness and color can be controller from the Android device and even
    be connected to the playing music, intesisty of Red, Green and Blue is
    forced based on the intesity of low, medium and high frequency on the
    music.
    
    If the synchronization logic is set, the first node will force its state
    on the twin one, all commands from Android or the hardware switch are then
    replicated on the twin node.
     
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
#include "conf/Gateway.h"                   // The main node is the Gateway

// Include framework code and libraries
#include <SPI.h>

/*** All configuration includes should be above this line ***/ 
#include "Souliss.h"

// Define the network configuration according to your router settings
uint8_t ip_address[4]  = {192, 168, 1, 78};
uint8_t subnet_mask[4] = {255, 255, 255, 0};
uint8_t ip_gateway[4]  = {192, 168, 1, 1};
#define Gateway_address 77
#define Peer_address    78
#define myvNet_address  ip_address[3]       // The last byte of the IP address (77) is also the vNet address
#define myvNet_subnet   0xFF00
#define myvNet_supern   Gateway_address

#define LEDCONTROL          0               // This is the memory slot for the logic that handle the light
#define LEDRED              1               // This is the memory slot for the logic that handle the light
#define LEDGREEN            2               // This is the memory slot for the logic that handle the light
#define LEDBLUE             3               // This is the memory slot for the logic that handle the light

void setup()
{   
    Initialize();
    
    // Set network parameters
    Souliss_SetIPAddress(ip_address, subnet_mask, ip_gateway);
    SetAsGateway(myvNet_address);                                   // Set this node as gateway for SoulissApp      
    
    // Set the typical logic to use
    Set_T16(LEDCONTROL);
    
    // Define inputs, outputs pins
    pinMode(3, OUTPUT);                 // Power the LED
    pinMode(5, OUTPUT);                 // Power the LED
    pinMode(6, OUTPUT);                 // Power the LED    
}

void loop()
{ 
    // Here we start to play
    EXECUTEFAST() {                     
        UPDATEFAST();   

        // Read every 10ms the input state and send it to the other board   
        FAST_10ms() {
            
            // Use Pin2 as ON/OFF command, use a pulldown resistor (10 k) connected between 
            // Pin2 and GND. Pressing for more than 1,5 seconds will set the GoodNight mode
            // and the light will be progressly be turned off.
            // It get the digital input state and store it in the LEDCONTROL logic
            DigInHold(2, Souliss_T1n_ToggleCmd, 0x255, LEDCONTROL);     
            
            // Use in inputs stored in the LEDCONTROL to control the RGB LED or Strip,
            // it provide automatically fade in and out effect and color change.
            Logic_T16(LEDCONTROL);

            // Use the output from the logic to control the colors via PWM in pins 3, 5 and 6
            //
            // If using a LED Strip wire as
            //  Strip Pin           RGB Shield Pin
            //  +                   R+ or G+ or B+
            //  R                   R-
            //  G                   G-
            //  B                   B-
            //
            // If using an RGB LED wire as
            //  LED Pin             RGB Shield Pin
            //  +                   R+ or G+ or B+
            //  R                   R- with resistor
            //  G                   G- with resistor
            //  B                   B- with resistor
            //          
            // If using separated LEDs for each color
            //  Strip Pin           RGB Shield Pin
            //  + (Red)             R+ with resistor
            //  + (Green)           G+ with resistor
            //  + (Blue)            B+ with resistor
            //  - (Red)             R-
            //  - (Green)           G-
            //  - (Blue)            B-
            //          
            analogWrite(3, mOutput(LEDRED));
            analogWrite(5, mOutput(LEDGREEN));
            analogWrite(6, mOutput(LEDBLUE));

            // Automatically handle the communication
            ProcessCommunication();
        }           
    }
    
    EXECUTESLOW() { 
        UPDATESLOW();

        SLOW_10s() {                
            // Timer associated to the LED logic control
            Timer_T16(LEDCONTROL);                      
        }     
        
    }   
} 