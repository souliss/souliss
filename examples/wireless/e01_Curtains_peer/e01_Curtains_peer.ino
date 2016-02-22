/**************************************************************************
    Souliss - Curtains
    
    It control a curtain with two devices: one device act on the relays
    that drive the motor and get the limit switches, the other has one 
    hardwired pushbutton for opening and closing the door. The curtain can 
    be controller via Android (or any equivalent direct user interface) 
    connecting to bridge node.
    
    Ensure to use limit switches to protect the motor once the door is completely
    closed or open, if limit switches are not used the motor should be self-protected.

    Run this code on one of the following boards:
      - Chibiduino v1
      
    As option you can run the same code on the following, just changing the
    relevant configuration file at begin of the sketch
      - Moteino
      
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

#define CURTAIN_NODE1           0           
#define CURTAIN_NODE2           0           

void setup()
{
    Initialize();

    // Setup the network configuration      
    Souliss_SetAddress(Peer_address, myvNet_subnet, myvNet_supern);                 // Address on the wireless interface    
    
    // The logic that handles windows and curtains is T22, you can as alternative use Set_Windows()
    Set_T22(CURTAIN_NODE2);
    
    // Define inputs (hardware pulldown required) and outputs pins
    pinMode(2, INPUT);                              // Open Command from pushbutton
    pinMode(3, INPUT);                              // Close Command from pushbutton
    pinMode(8, OUTPUT);                             // Open Relay
    pinMode(9, OUTPUT);                             // Close Relay
}


void loop()
{   
    // Here we start to play
    EXECUTEFAST() {                     
        UPDATEFAST();   

        // Process every 510ms the logic that control the curtain
        FAST_510ms() {
            // Use OPEN and CLOSE Commands
            DigIn(2, Souliss_T2n_OpenCmd_Local, CURTAIN_NODE2);
            DigIn(3, Souliss_T2n_CloseCmd_Local, CURTAIN_NODE2);
            
            // Run the logic
            Logic_T22(CURTAIN_NODE2);
            
            // Control the output relays, the stop is with both the relays not active.
            // If you need to active the relays on stop, use nDigOut instead.
            DigOut(8, Souliss_T2n_Coil_Open, CURTAIN_NODE2);    
            DigOut(9, Souliss_T2n_Coil_Close, CURTAIN_NODE2);   
                
        } 

        // Process the communication
        FAST_PeerComms();
    
        // Define the hold time of the outputs, by default the timer hold the relays for 16 times, so:
        // 221x10x16ms that is about 35 seconds. Change the parameter inside FAST_x10ms() to change this time.
        FAST_x10ms(211) {                 
            Timer_T22(CURTAIN_NODE2);
        }
    }
} 