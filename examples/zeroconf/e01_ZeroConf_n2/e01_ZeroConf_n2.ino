/**************************************************************************
    Souliss - Zero Configuration
    
    This example demonstrate a zero configuration setup, with the following
    network architecture:
    
    HOME-ROUTER                     GATEWAY (n1)    
    ETHERNET        (connect)       ETHERNET    
    
    HOME-ROUTER                     BRIDGE  (n2)    
    ETHERNET        (connect)       ETHERNET    
    
    BRIDGE (n2)                     PEER (n3)
    RS-485 A        (connect)       RS-485 A
    RS-485 B        (connect)       RS-485 B
    RS-485 GND      (optional)      RS-485 GND

    The Gateway gets an IP address via DHCP, the Bridge works in IPBROADCAST
    and doesn't request an IP address but only a vNet address (through the Gateway)
    as last the Peer get the subnet from the Bridge and the vNet address from the
    Gateway.
    
    As startup procedure, 
    1) Turn on the Gateway and waits until SoulissApp detect it
    2) Turn on the Bridge and waits until the LED get solid on
    3) Turn on the Peer and waits until the LED get solid on
    
    At this time the network is up and you can run it.  
    
***************************************************************************/

// Let the IDE point to the Souliss framework
#include "SoulissFramework.h"

#include "bconf/DINo_v2_EthernetBridge_RS485.h"                 // Define the board type
#include "conf/SuperNode.h"                                     // The main node is the SuperNode
#include "conf/DynamicAddressing.h"                             // Use dynamic address

// Include framework code and libraries
#include <SPI.h>
#include <EEPROM.h>

/*** All configuration includes should be above this line ***/ 
#include "Souliss.h"

#define LIGHT1                  0           // This is the memory slot used for the execution of the logic
#define LIGHT2                  1           // This is the memory slot used for the execution of the logic
#define LIGHT3                  2           // This is the memory slot used for the execution of the logic
#define LIGHT4                  3           // This is the memory slot used for the execution of the logic
#define LIGHT_NO                4           
#define ONTIME                  15          // Light is on for ONTIME number of cycles


void setup()
{   
    InitDINo();

    // Setup the network configuration
    //
    
    // Set the inputs
    SetInput1();
    SetInput2();
    SetInput3();
    SetInput4();
    
    // Set the outputs
    SetRelay1();
    SetRelay2();
    SetRelay3();
    SetRelay4();

    // Set the status LED and let it blink for a couple of seconds
    SetLED();
    StartupLED();
    
    // Define Simple Light logics for the relays
    Set_SimpleLight(LIGHT1);
    Set_SimpleLight(LIGHT2);
    Set_SimpleLight(LIGHT3);
    Set_SimpleLight(LIGHT4);    
    
    // This board request an address to the gateway at runtime, no need
    // to configure any parameter here.
    SetDynamicAddressing();
    GetAddress();   
    
} 

void loop()
{ 
    // Here we start to play
    EXECUTEFAST() {                     
        UPDATEFAST();   
        
        FAST_50ms() {   // We process the logic and relevant input and output every 50 milliseconds
        
            DigIn(IN1, Souliss_T1n_ToggleCmd, LIGHT1);          // Read inputs from IN1
            DigIn(IN2, Souliss_T1n_ToggleCmd, LIGHT2);          // Read inputs from IN2
            DigIn(IN3, Souliss_T1n_ToggleCmd, LIGHT3);          // Read inputs from IN3
            DigIn(IN4, Souliss_T1n_ToggleCmd, LIGHT4);          // Read inputs from IN4
        
            Logic_SimpleLight(LIGHT1);                          // Execute the logic for Relay 1
            Logic_SimpleLight(LIGHT2);                          // Execute the logic for Relay 2
            Logic_SimpleLight(LIGHT3);                          // Execute the logic for Relay 3
            Logic_SimpleLight(LIGHT4);                          // Execute the logic for Relay 4
            
            DigOut(RELAY1, Souliss_T1n_Coil, LIGHT1);           // Drive the Relay 1
            DigOut(RELAY2, Souliss_T1n_Coil, LIGHT2);           // Drive the Relay 2
            DigOut(RELAY3, Souliss_T1n_Coil, LIGHT3);           // Drive the Relay 3
            DigOut(RELAY4, Souliss_T1n_Coil, LIGHT4);           // Drive the Relay 4
        } 
        
        // Here we process all communication with other nodes
        FAST_PeerComms();   
        
    }
    
    EXECUTESLOW() { 
        UPDATESLOW();

        SLOW_10s() {  // Process the timer every 10 seconds  
            Timer_SimpleLight(LIGHT1);
            Timer_SimpleLight(LIGHT2);  
            Timer_SimpleLight(LIGHT3);
            Timer_SimpleLight(LIGHT4);  

            // Once the join has been completed, turn the LED solid on
            if(!JoinInProgress())
                TurnOnLED();            
        } 
        
        // Here we periodically check for a gateway to join
        SLOW_PeerJoin();    
    }
} 