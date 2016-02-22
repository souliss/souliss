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
#define Gateway_address 0x6501              // The Gateway node has two address, one on the Ethernet side
                                            // and the other on the wireless one
#define Peer_address    0x6502
#define myvNet_address  ip_address[3]       // The last byte of the IP address (77) is also the vNet address
#define myvNet_subnet   0xFF00
#define myvNet_supern   Gateway_address

#define CURTAIN_NODE1           0           
#define CURTAIN_NODE2           0           

void setup()
{
    Initialize();
    
    // Set network parameters
    Souliss_SetIPAddress(ip_address, subnet_mask, ip_gateway);                          // Address on the Ethernet interface
    SetAsGateway(myvNet_address);                                                       // Set this node as gateway for SoulissApp  
    Souliss_SetAddress(Gateway_address, myvNet_subnet, myvNet_supern);                  // Address on the wireless interface    

    // This node as gateway will get data from the Peer
    SetAsPeerNode(Peer_address, 1); 
}

void loop()
{ 
    // Here we start to play
    EXECUTEFAST() {                     
        UPDATEFAST();   

        // This node does just networking, bridging the Peer node to the Ethernet network
        FAST_GatewayComms();            
    }   
}
