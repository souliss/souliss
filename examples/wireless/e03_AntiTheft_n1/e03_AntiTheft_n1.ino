/**************************************************************************
    Souliss - Anti-theft Integration (Main Node)

    The anti-theft integration allow the handling of sensors distributed in the
    home, reporting if an alarm is raised to a standard anti-theft system.
    Two communication paths are build for the case, one care of data exchanging
    between the nodes involved in the anti-theft and the other is used to provide
    a distributed watch-dog. A fail of one node will result in a fail of the 
    watch-dog and and so rising an alarm to the anti-theft.
    
    A standard anti-theft system can be integrated via its auxiliary inputs or
    equivalent.
    
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
#define Gateway_address     0x6501              // The Gateway node has two address, one on the Ethernet side
                                                // and the other on the wireless one
#define Peer_address_n1     0x6502
#define Peer_address_n2     0x6503
#define myvNet_subnet       0xFF00
#define myvNet_supern       Gateway_address

#define ANTITHEFT               0           // This is the memory slot used for the execution of the anti-theft
#define WATCHDOG                1           // This is the memory slot used for the execution of the watchdog

void setup()
{
    Initialize();

    // Setup the network configuration      
    Souliss_SetAddress(Peer_address_n1, myvNet_subnet, myvNet_supern);                  // Address on the wireless interface    

    // Setup the anti-theft
    Set_T42(ANTITHEFT);
    
    // Define inputs, outputs pins and pullup
    pinMode(2, INPUT);  // Hardware pullup required 
}

void loop()
{ 
    // Here we start to play
    EXECUTEFAST() {                     
        UPDATEFAST();   

        // Process every 510ms the logic that control the curtain
        FAST_510ms() {
            
            // Input from anti-theft sensor
            LowDigIn(2, Souliss_T4n_Alarm, ANTITHEFT);
        
            // Execute the anti-theft logic and report to the main node
            Logic_T42(ANTITHEFT, Gateway_address);
        }
        
        // Process the communication
        FAST_PeerComms();
        
        // Execute the code every 2110ms          
        FAST_2110ms()   {   
        
            // Build a watchdog chain to monitor the nodes
            mInput(ANTITHEFT) = Watchdog(Peer_address_n2, WATCHDOG, Souliss_T4n_Alarm);
        }           
    }
} 
