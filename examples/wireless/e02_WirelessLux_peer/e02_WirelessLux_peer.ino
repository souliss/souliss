/**************************************************************************
    Souliss - Wireless Lux
    
    In this example there are two nodes (bridge and wireless) either with
    two light that can be controlled via Android (or any other user interface)
    and pushbuttons.
    
    Run this code on one of the following boards:
      - Arduino with Nordic nRF24L01 or nRF24L01+
      
    As option you can run the same code on the following, just changing the
    relevant configuration file at begin of the sketch
      - Chibiduino v1
      - Moteino
      
***************************************************************************/

// Let the IDE point to the Souliss framework
#include "SoulissFramework.h"

// Configure the framework
#include "bconf/StandardArduino.h"          // Use a standard Arduino
#include "conf/nRF24L01.h"

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

#define LIGHT1_NODE1            0            
#define LIGHT2_NODE1            1            
#define LIGHT1_NODE2            0            
#define LIGHT2_NODE2            1            
void setup()
{   
    Initialize();
    
    // Set network parameters
    Souliss_SetAddress(Peer_address, myvNet_subnet, myvNet_supern);                 // Address on the wireless interface    

    // Set the typical logic to handle the lights
    Set_T11(LIGHT1_NODE1);
    Set_T11(LIGHT2_NODE1);
    
    // Define inputs (hardware pulldown required) and outputs pins
    pinMode(2, INPUT);                  // Toggle command for light 1
    pinMode(3, INPUT);                  // Toggle command for light 2
    pinMode(4, OUTPUT);                 // Power to relay coil for light 1
    pinMode(5, OUTPUT);                 // Power to relay coil for light 2
}

void loop()
{   
    // Here we start to play
    EXECUTEFAST() {                     
        UPDATEFAST();   

        // Process every 510ms the logic that control the lights
        FAST_510ms() {
            // Use Pin2 as ON/OFF command
            DigIn(2, Souliss_T1n_ToggleCmd, LIGHT1_NODE1);  
            
            // Use Pin3 as ON/OFF command
            DigIn(3, Souliss_T1n_ToggleCmd, LIGHT2_NODE1);      
            
            // Execute the logic
            Logic_T11(LIGHT1_NODE1);
            Logic_T11(LIGHT2_NODE1);

            // Use Pin4 as output on the electrical load
            DigOut(4, Souliss_T1n_Coil, LIGHT1_NODE1);  

            // Use Pin5 as output on the electrical load
            DigOut(5, Souliss_T1n_Coil, LIGHT2_NODE1);                      
        } 
        
        // Here we process the communication
        FAST_PeerComms();       
    }
    
    EXECUTESLOW() {
        UPDATESLOW();

        SLOW_10s() {        // We handle the light timer with a 10 seconds base time
            Timer_T11(LIGHT1_NODE1);
            Timer_T11(LIGHT2_NODE1);
        }       
    }           
} 

