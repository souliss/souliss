/**************************************************************************
    Souliss - Garage Door
    
    Control a garage door using two Ethernet boards: one device act on the 
    relays that drive the motor and get the limit switches, the other has one 
    push-button for opening and closing the door. The door can be controlled
    also via Android (download SoulissApp from Play Store).
    
    Ensure to use limit switches to protect the motor once the door is completely
    closed or open, if limit switches are not used the motor should be self-protected.
        
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

#define GARAGEDOOR_NODE1            0                       
#define GARAGEDOOR_NODE2            0   
#define GARAGELIGHT_NODE2           1

#define INPUTPIN_LIMIT_OPENING 2
#define INPUTPIN_LIMIT_CLOSING 4

#define OUTPUTPIN_LAMP 7
#define OUTPUTPIN_OPENING 8
#define OUTPUTPIN_CLOSING 9

void setup()
{   
    Initialize();
    
    // Set network parameters
    Souliss_SetIPAddress(ip_address, subnet_mask, ip_gateway);      

    // Set the typical logic to handle the garage door and light
    Set_GarageDoor (GARAGEDOOR_NODE2);
    Set_SimpleLight(GARAGELIGHT_NODE2);
    
    // Define inputs, outputs pins
    pinMode(INPUTPIN_LIMIT_OPENING, INPUT);                  // Hardware pulldown required
    pinMode(INPUTPIN_LIMIT_CLOSING, INPUT);                  // Hardware pulldown required

    pinMode(OUTPUTPIN_LAMP, OUTPUT);
    pinMode(OUTPUTPIN_OPENING, OUTPUT);
    pinMode(OUTPUTPIN_CLOSING, OUTPUT);
}

void loop()
{ 
    // Here we start to play
    EXECUTEFAST() {                     
        UPDATEFAST();   

        // Execute every 510ms the logic, the command to open and close the garage door
        // cames directly from SoulissApp or the push-button located on the other node
        FAST_510ms() {
        
            // Use Pin2 and Pin 4 as limit switches
            DigIn(INPUTPIN_LIMIT_OPENING, Souliss_T2n_LimSwitch_Open, GARAGEDOOR_NODE2);
            DigIn(INPUTPIN_LIMIT_CLOSING, Souliss_T2n_LimSwitch_Close, GARAGEDOOR_NODE2);
            
            // Execute the logic for door or gate
            Logic_GarageDoor(GARAGEDOOR_NODE1);
            
            // Use Pin8 and Pin9 to control the open and close command through relays
            DigOut(OUTPUTPIN_OPENING, Souliss_T2n_Coil_Open, GARAGEDOOR_NODE2);
            DigOut(OUTPUTPIN_CLOSING, Souliss_T2n_Coil_Close, GARAGEDOOR_NODE2);
        
        
            // If the door or gate is open / opening, switch ON the light
            if((mOutput(GARAGEDOOR_NODE2) == Souliss_T2n_Coil_Open) || (mOutput(GARAGEDOOR_NODE2) == Souliss_T2n_LimSwitch_Open))
                mInput(GARAGELIGHT_NODE2) = Souliss_T1n_OnCmd;
            else if((mOutput(GARAGEDOOR_NODE2) == Souliss_T2n_Coil_Close) || (mOutput(GARAGEDOOR_NODE2) == Souliss_T2n_LimSwitch_Close))
                mInput(GARAGELIGHT_NODE2) = Souliss_T1n_OffCmd;
            
            // Execute the logic for light
            Logic_SimpleLight(GARAGELIGHT_NODE2);
            
            // Use Pin7 to control the light
            DigOut(OUTPUTPIN_LAMP, Souliss_T1n_OnCoil, GARAGELIGHT_NODE2);
        }

        // Process the communication, this include the command that are coming from SoulissApp
        // or from the push-button located on the other node
        FAST_PeerComms();
        
    }
    
    EXECUTESLOW() { 
        UPDATESLOW();

        SLOW_10s() {               
            // Time out commands if no limit switches are received
            Timer_GarageDoor(GARAGEDOOR_NODE1);
        } 
    }   
    
} 