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
      - Chibiduino v1 with Ethernet Shield (W5100)
      
    As option you can run the same code on the following, just changing the
    relevant configuration file at begin of the sketch
      - Chibiduino v1 with ENC28J60/W5200/W5500 Ethernet Shield
      - Moteino with W5100/W5200/W5500/ENC28J60 Ethernet Shield
      
***************************************************************************/

// Let the IDE point to the Souliss framework
#include "SoulissFramework.h"

// Configure the framework
#include "bconf/Chibiduino_v1.h"            // Use a Chibiduino 2.4 GHz wireless board
#include "conf/ethW5100.h"                  // Ethernet through Wiznet W5100
#include "conf/Gateway.h"                   // The main node is the Gateway

// Include framework code and libraries
#include <SPI.h>

/*** All configuration includes should be above this line ***/ 
#include "Souliss.h"

// Define the network configuration according to your router settings
uint8_t ip_address[4]  = {192, 168, 1, 77};
uint8_t subnet_mask[4] = {255, 255, 255, 0};
uint8_t ip_gateway[4]  = {192, 168, 1, 1};
#define Gateway_address     0x6501              // The Gateway node has two address, one on the Ethernet side
                                                // and the other on the wireless one
#define Peer_address_n1     0x6502
#define Peer_address_n2     0x6503
#define myvNet_address      ip_address[3]       // The last byte of the IP address (77) is also the vNet address
#define myvNet_subnet       0xFF00
#define myvNet_supern       Gateway_address

#define ANTITHEFT               0           // This is the memory slot used for the execution of the anti-theft
#define WATCHDOG                1           // This is the memory slot used for the execution of the watchdog

void setup()
{
    Initialize();
    
    // Set network parameters
    Souliss_SetIPAddress(ip_address, subnet_mask, ip_gateway);                          // Address on the Ethernet interface
    SetAsGateway(myvNet_address);                                                       // Set this node as gateway for SoulissApp  
    Souliss_SetAddress(Gateway_address, myvNet_subnet, myvNet_supern);                  // Address on the wireless interface    
    
    // This node as gateway will get data from the Peer
    SetAsPeerNode(Peer_address_n1, 1);      
    SetAsPeerNode(Peer_address_n2, 2);      

    // Setup the anti-theft
    Set_T41(ANTITHEFT);

    // Define inputs (hardware pulldown required) and outputs pins
    pinMode(2, INPUT);  
    pinMode(7, OUTPUT);     
    pinMode(8, OUTPUT); 
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
        
            // Execute the anti-theft logic
            Logic_T41(ANTITHEFT);
        
            // Set the Pin7 if the anti-theft is activated
            nDigOut(7, Souliss_T4n_Antitheft, ANTITHEFT);           
        
            // Set the Pin8 if the alarm is raised
            LowDigOut(8, Souliss_T4n_InAlarm, ANTITHEFT);
        }
        
        // This node does just networking, bridging the Peer node to the Ethernet network
        FAST_GatewayComms();        
        
        // Execute the code every 2110ms          
        FAST_2110ms()   {   
        
            // Build a watchdog chain to monitor the nodes
            mInput(ANTITHEFT) = Watchdog(Peer_address_n1, WATCHDOG, Souliss_T4n_Alarm);
        }       
        
    }
} 
