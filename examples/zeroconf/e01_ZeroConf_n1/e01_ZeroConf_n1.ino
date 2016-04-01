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

#include "bconf/StandardArduino.h"          // Use a standard Arduino
#include "conf/ethW5100.h"                  // Ethernet through Wiznet W5100
#include "conf/Gateway.h"                   // The main node is the Gateway, we have just one node
#include "conf/Webhook.h"                   // Enable DHCP and DNS
#include "conf/DynamicAddressing.h"         // Use dynamic address

// Include framework code and libraries
#include <SPI.h>
#include <EEPROM.h>

/*** All configuration includes should be above this line ***/ 
#include "Souliss.h"

#define LIGHT1                  0           // This is the memory slot used for the execution of the logic
#define LIGHT2                  1           
#define LIGHT3                  2           
#define LIGHT4                  3           

void setup()
{   
    Initialize();

    // Get the IP address from DHCP
    GetIPAddress();                          
    SetAsGateway(myvNet_dhcp);      // Set this node as gateway for SoulissApp  
    
    Set_SimpleLight(LIGHT1);        // Define a simple LED light logic
    
    // We connect a pushbutton between 5V and pin2 with a pulldown resistor 
    // between pin2 and GND, the LED is connected to pin9 with a resistor to
    // limit the current amount
    pinMode(2, INPUT);                  // Hardware pulldown required
    pinMode(9, OUTPUT);                 // Power the LED
    
    // This node will serve all the others in the network providing an address
    SetAddressingServer();
}

void loop()
{ 
    // Here we start to play
    EXECUTEFAST() {                     
        UPDATEFAST();   
        
        FAST_50ms() {   // We process the logic and relevant input and output every 50 milliseconds
            DigIn(2, Souliss_T1n_ToggleCmd, LIGHT1);            // Use the pin2 as ON/OFF toggle command
            Logic_SimpleLight(LIGHT1);                          // Drive the LED as per command
            DigOut(9, Souliss_T1n_Coil, LIGHT1);                // Use the pin9 to give power to the LED according to the logic
        } 
              
        // Here we handle here the communication with Android, commands and notification
        // are automatically assigned to LIGHT1
        FAST_GatewayComms();                                        
        
    }
} 