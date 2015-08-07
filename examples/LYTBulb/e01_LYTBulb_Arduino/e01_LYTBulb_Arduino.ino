/**************************************************************************
    Souliss - LYT Light Bulb
    
    This sketch control the Authometion LYT bubls through the Authometion
    shield.
    
    Load this sketch on an Arduino UNO/Duemilanove (use of Leonardo require
    the use of the proper USART configuration) and using the USART bridge it
    to an ESP8266 WiFi module.

    Verify shield's jumpers and select Hardware Usart while using this sketch, 
    remember to remove the jumpers before programming the Arduino.
    
***************************************************************************/
#include "bconf/StandardArduino.h"              // Use an Arduino board
#include "bconf/LYT88_LEDBulb_Radio.h"          // Define the board type
#include "conf/usart.h"                         // Ethernet through Wiznet W5100

// Include framework code and libraries
#include <SPI.h>
#include "Souliss.h"

// Include Authometion libraries
#include <PL1167.h>
#include <Lytwifi.h>
#include <WiFiInterrupt.h>

// There are not used in this sketch, keep it just to pat the compiler
#include <EEPROM.h>
#include <SoftwareSerial.h>

// Define network parameters
#define PEER_USART          0xCE02
#define GATEWAY_USART       0xCE01
#define SUBNETMASK          0xFF00

// Define logic slots, multicolor lights use four slots
#define LYTLIGHT1           0                   

void setup()
{   
    Initialize();
    
    // Set network parameters
    SetAddress(PEER_USART, SUBNETMASK, GATEWAY_USART);
    
    // Se the LYT bulbs (index, bulb type, addr_a, addr_b, logic slot), here we have
    // the first lamp (index 0) with default addresses (0, 0). Change these based on
    // the configuration of your lamps.
    SetLYT(0, 0, 0, LYTLIGHT1);

    // Define a logic to handle the bulb(s)
    SetLYTLamps(LYTLIGHT1); 
}

void loop()
{ 
    // Here we start to play
    EXECUTEFAST() {                     
        UPDATEFAST();   
        
        FAST_10ms() {   // We process the logic and relevant input and output every 110 milliseconds
            ProcessCommunication();
            LogicLYTLamps(LYTLIGHT1);       
        } 
        
        // Here we process all communication with other nodes
        FAST_PeerComms();    
    }   
    
    EXECUTESLOW() {
        UPDATESLOW();
        
        SLOW_10s() {
            LYTState(LYTLIGHT1);            // Verify the lamp state
            LYTSleepTimer(LYTLIGHT1);       // Slowly shut down the lamp
        }
    }
} 