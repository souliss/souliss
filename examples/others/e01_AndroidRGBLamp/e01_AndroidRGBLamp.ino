/**************************************************************************
    Souliss - Android RGB Lamp
    
    Control an RGB Led Lamp using the Android user interface, communication
    between the board and Android is via Ethernet and MaCaco/vNet protocol.
    
    The lamp is controlled via IR led that emulate the original remote, is
    required a dedicated map for the lamp that shall be controlled, in this
    example there is a mapping for NEC codes that shall work for most of the
    devices.
    
    You need an IR led as additional hardware on pin 3
 
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
#include "extras/extras.h"

// Define the network configuration according to your router settings
uint8_t ip_address[4]  = {192, 168, 1, 77};
uint8_t subnet_mask[4] = {255, 255, 255, 0};
uint8_t ip_gateway[4]  = {192, 168, 1, 1};
#define Gateway_address 77
#define Peer_address    78
#define myvNet_address  ip_address[3]       // The last byte of the IP address (77) is also the vNet address
#define myvNet_subnet   0xFF00
#define myvNet_supern   Gateway_address

#define RGBLAMP_SLOT            0           // This is the memory slot used for the execution of the logic in network_address1

void setup()
{   
    Initialize();
    
    // Set network parameters
    Souliss_SetIPAddress(ip_address, subnet_mask, ip_gateway);
    SetAsGateway(myvNet_address);                                   // Set this node as gateway for SoulissApp  
    
    // Set the RGB led lamp typical
    Set_T15(RGBLAMP_SLOT);
    
    // Define outputs pins for IR led
    pinMode(3, OUTPUT);
}

void loop()
{ 
    // Here we start to play
    EXECUTEFAST() {                     
        UPDATEFAST();   

        // Read every 510ms the input state and send it to the other board  
        FAST_510ms() {
            
            // Execute the logic for RGB control
            Logic_T15(RGBLAMP_SLOT);
            
            // Send the command to the lamp via IR
            if(isTrigged(RGBLAMP_SLOT))
            {
                sendRGBRemote(mOutput(RGBLAMP_SLOT));
                ResetTrigger();
            }   
        } 

        // Here we handle here the communication with Android, commands and notification
        FAST_GatewayComms();        
                    
    }
} 