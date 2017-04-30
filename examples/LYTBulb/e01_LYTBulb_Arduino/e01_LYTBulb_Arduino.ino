/**************************************************************************
    Souliss - LYT Light Bulb
    
    This sketch control the Authometion LYT bubls through the Authometion
    shield.
    
    Load this sketch on an Arduino UNO/Duemilanove (use of Leonardo require
    the use of the proper USART configuration) and using the USART bridge it
    to an ESP8266 WiFi module.

    Verify shield's jumpers and select Hardware Usart while using this sketch, 
    remember to remove the jumpers before programming the Arduino. Use pin 10 as
    chip select for the radio.
    
***************************************************************************/

// Let the IDE point to the Souliss framework
#include "SoulissFramework.h"

#include "bconf/StandardArduino.h"              // Use an Arduino board
#include "bconf/LYT88_LEDBulb_Radio.h"          // Define the board type
#include "conf/usart_fast.h"                    // Ethernet through Wiznet W5100

// Include framework code and libraries
#include <SPI.h>

/*** All configuration includes should be above this line ***/ 
#include "Souliss.h"

// Include Authometion libraries
#include <PL1167.h>
#include <Lytwifi.h>
#include <WiFiInterrupt.h>

// These are not used in this sketch, keep it just to pat the compiler
#include <EEPROM.h>
#include <SoftwareSerial.h>

// Define logic slots, multicolor lights use four slots
#define LYTLIGHT1           0                   

void setup()
{   
    Initialize();
    
    // Set network parameters
    SetAddress(0xCE02, 0xFF00, 0xCE01);
    
    // Se the LYT bulbs (index, bulb type, addr_a, addr_b, logic slot), here we have
    // the first lamp (index 0) with default addresses (0, 0). Change these based on
    // the configuration of your lamps.
    InitLYT();
    SetLYT(0, 0, 0, LYTLIGHT1);

    // Define a logic to handle the bulb(s)
    SetLYTLamps(LYTLIGHT1); 
}

void loop()
{ 
    // Here we start to play
    EXECUTEFAST() {                     
        UPDATEFAST();   
        
        // Is an unusual approach, but to get fast response to color change we run the LYT logic and
        // basic communication processing at maximum speed.
        LogicLYTLamps(LYTLIGHT1);       
        ProcessCommunication();
        
        // Here we process all communication with other nodes
        FAST_1110ms() {
            LYTState(LYTLIGHT1);
        }

        FAST_9110ms() {
			LYTSleepTimer(LYTLIGHT1);       // Slowly shut down the lamp
        }
        
    }   
    
    EXECUTESLOW() {
        UPDATESLOW();
        
        SLOW_10s() {
            LYTStateRequest();              // Request the lamp state
        }
    }
} 