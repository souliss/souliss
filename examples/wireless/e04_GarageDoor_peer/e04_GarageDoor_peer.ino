/**************************************************************************
    Souliss - Garage Door
    
    Control a garage door using two Ethernet boards: one device act on the 
    relays that drive the motor and get the limit switches, the other has one 
    push-button for opening and closing the door. The door can be controlled
    also via Android (download SoulissApp from Play Store).
    
    Ensure to use limit switches to protect the motor once the door is completely
    closed or open, if limit switches are not used the motor should be self-protected.
        
    Run this code on one of the following boards:
      - Chibiduino v1 2.4 GHz
      

***************************************************************************/

// Let the IDE point to the Souliss framework
#include "SoulissFramework.h"

// Configure the framework
#include "bconf/Chibiduino_v1.h"            // Use a Chibiduino 2.4 GHz wireless board

// Include framework code and libraries
#include <SPI.h>

/*** All configuration includes should be above this line ***/ 
#include "Souliss.h"

// Define the network configuration according to your router settings
#define Gateway_address 0x6501              // The Gateway node has two address, one on the Ethernet side
                                            // and the other on the wireless one
#define Peer_address    0x6502
#define myvNet_subnet   0xFF00
#define myvNet_supern   Gateway_address

#define GARAGEDOOR_NODE1            0                       
#define GARAGEDOOR_NODE2            0   
#define GARAGELIGHT_NODE2           1

void setup()
{   
    Initialize();
    
    // Setup the network configuration      
    Souliss_SetAddress(Peer_address, myvNet_subnet, myvNet_supern);                 // Address on the wireless interface    
    
    // Set the typical logic to handle the garage door and light
    Set_T22 (GARAGEDOOR_NODE2);
    Set_SimpleLight(GARAGELIGHT_NODE2);
    
    // Define inputs, outputs pins
    pinMode(2, INPUT);                  // Hardware pulldown required
    pinMode(3, INPUT);                  // Hardware pulldown required

    pinMode(7, OUTPUT); 
    pinMode(8, OUTPUT);
    pinMode(9, OUTPUT);
}

void loop()
{ 
    // Here we start to play
    EXECUTEFAST() {                     
        UPDATEFAST();   

        // Execute every 510ms the logic, the command to open and close the garage door
        // cames directly from SoulissApp or the push-button located on the other node
        FAST_510ms() {
        
            // Use Pin3 and Pin 4 as limit switches
            DigIn(2, Souliss_T2n_LimSwitch_Open, GARAGEDOOR_NODE2);
            DigIn(3, Souliss_T2n_LimSwitch_Close, GARAGEDOOR_NODE2);            
            
            // Execute the logic for door or gate
            Logic_T22(GARAGEDOOR_NODE2);
            
            // Use Pin8 and Pin9 to control the open and close command through relays
            DigOut(8, Souliss_T2n_Coil_Open, GARAGEDOOR_NODE2); 
            DigOut(9, Souliss_T2n_Coil_Close, GARAGEDOOR_NODE2);                
        
        
            // If the door or gate is open / opening, switch ON the light
            if((mOutput(GARAGEDOOR_NODE2) == Souliss_T2n_Coil_Open) || (mOutput(GARAGEDOOR_NODE2) == Souliss_T2n_LimSwitch_Open))
                mInput(GARAGELIGHT_NODE2) = Souliss_T1n_OnCmd;
            else if((mOutput(GARAGEDOOR_NODE2) == Souliss_T2n_Coil_Close) || (mOutput(GARAGEDOOR_NODE2) == Souliss_T2n_LimSwitch_Close))
                mInput(GARAGELIGHT_NODE2) = Souliss_T1n_OffCmd;
            
            // Execute the logic for light
            Logic_SimpleLight(GARAGELIGHT_NODE2);
            
            // Use Pin7 to control the light
            DigOut(7, Souliss_T1n_OnCoil, GARAGELIGHT_NODE2);           
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
