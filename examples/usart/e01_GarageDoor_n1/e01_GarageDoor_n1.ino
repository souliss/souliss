/**************************************************************************
    Souliss - Garage Door
    
    Control a garage door using two boards connected via USART: one device act on 
	the relays that drive the motor and get the limit switches, the other has one 
    push-button for opening and closing the door. The door can be controlled
    also via Android (download SoulissApp from Play Store).
    
	Connect the boards via USART crossing TX or RX, or through an RS485 transceiver.
	
    Ensure to use limit switches to protect the motor once the door is completely
    closed or open, if limit switches are not used the motor should be self-protected.
        
    Run this code on one of the following boards:
      - Arduino Ethernet (W5100) with RS485 transceiver
      - Arduino with Ethernet Shield (W5100) with RS485 transceiver
      
    As option you can run the same code on the following, just changing the
    relevant configuration file at begin of the sketch
      - Arduino with ENC28J60 Ethernet Shield with RS485 transceiver
      - Arduino with W5200 Ethernet Shield with RS485 transceiver
      - Arduino with W5500 Ethernet Shield with RS485 transceiver
            
***************************************************************************/

// Configure the framework
#include "bconf/StandardArduino.h"          // Use a standard Arduino
#include "conf/ethW5100.h"                  // Ethernet through Wiznet W5100
#include "conf/usart.h"                     // USART RS485
#include "conf/Gateway.h"                   // The main node is the Gateway

/*************/
// Use the following if you are using an RS485 transceiver with 
// transmission enable pin, otherwise delete this section.
//
#define USARTDRIVER_INSKETCH
#define USART_TXENABLE          1
#define USART_TXENPIN           3
#define USARTDRIVER             Serial
/*************/

// Include framework code and libraries
#include <SPI.h>
#include "Souliss.h"

// Define the network configuration according to your router settings
uint8_t ip_address[4]  = {192, 168, 1, 77};
uint8_t subnet_mask[4] = {255, 255, 255, 0};
uint8_t ip_gateway[4]  = {192, 168, 1, 1};
#define myvNet_address  ip_address[3]       // The last byte of the IP address (77) is also the vNet address
// Define the RS485 network configuration
#define myvNet_subnet   0xFF00
#define Gateway_RS485   0xCE01
#define Peer_RS485      0xCE02

#define GARAGEDOOR_NODE1            0                       
#define GARAGEDOOR_NODE2            0   
#define GARAGELIGHT_NODE2           1

#define INPUT_PIN	2 // Use Pin2 as ON/OFF command

void setup()
{   
    Initialize();
    
    // Set network parameters
    Souliss_SetIPAddress(ip_address, subnet_mask, ip_gateway);
    SetAsGateway(myvNet_address);                                   // Set this node as gateway for SoulissApp  
    SetAddress(Gateway_RS485, myvNet_subnet, 0);                    // Set the address on the RS485 bus
    
    // This node as gateway will get data from the Peer
    SetAsPeerNode(Peer_RS485, 1); 
    
    // Define inputs, outputs pins and pulldown
	pinMode(INPUT_PIN, INPUT);  // Hardware pulldown required
    
}

void loop()
{   
    // Here we start to play
    EXECUTEFAST() {                     
        UPDATEFAST();   

        // Read every 510ms the input state and send it to the other board  
        FAST_510ms() {
            
            // Use Pin2 as ON/OFF command
            if(DigIn(INPUT_PIN, Souliss_T2n_ToggleCmd, GARAGEDOOR_NODE1))
            {
                // Send data
                Souliss_RemoteInput(Peer_RS485, GARAGEDOOR_NODE2, mInput(GARAGEDOOR_NODE1));
                Souliss_ResetInput(memory_map, GARAGEDOOR_NODE1);
            }
        } 
        
        // Here we handle here the communication with Android, commands and notification
        // are automatically assigned to MYLEDLOGIC
        FAST_GatewayComms();        
                        
    }
} 
