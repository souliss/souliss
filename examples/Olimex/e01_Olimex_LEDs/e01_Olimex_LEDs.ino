/**************************************************************************
    Souliss - LEDs
    
    It control via Android the two embedded LEDs of the OLIMEXINO-328 board,
    this is the simplest example and can be used as starting point with Souliss 
    over Olimex hardware.
    
    The communication goes over Ethernet using MOD-ENC28J60 or MOD-WIFI.
 
    Run this code on one of the following boards:
      - Olimexino328 and MOD-ENC28J60 

    As option you can run the same code on the following, just changing the
    relevant configuration file at begin of the sketch
      - Olimexino328 and MOD-WIFI
      - Olimexino32U4 and MOD-ENC28J60
      - Olimexino32U4 and MOD-WIFI
      - AVR T-32U4 and MOD-ENC28J60
      - AVR T-32U4 and MOD-WIFI   
    
***************************************************************************/

// Let the IDE point to the Souliss framework
#include "SoulissFramework.h"

// Configure the framework
#include "bconf/Olimexino328.h"             // Use a Olimex Olimexino328
#include "bconf/OlimexMOD-ENC28J60.h"       // Ethernet through Microchip ENC28J60
#include "conf/Gateway.h"                   // The main node is the Gateway, we have just one node

// Include framework code and libraries
#include <SPI.h>

/*** All configuration includes should be above this line ***/ 
#include "Souliss.h"

// Define the network configuration according to your router settings
uint8_t ip_address[4]  = {192, 168, 1, 77};
uint8_t subnet_mask[4] = {255, 255, 255, 0};
uint8_t ip_gateway[4]  = {192, 168, 1, 1};
#define Gateway_address 77
#define Peer_address    78
#define myvNet_address  ip_address[3]       // The last byte of the IP address (77) is also the vNet address
#define myvNet_subnet   0xFF00
#define myvNet_supern   Gateway_address

#define SLOT_LED1           0           // This is the memory slot used for the execution of the logic
#define SLOT_LED2           1           // This is the memory slot used for the execution of the logic

void setup()
{   
    Initialize();
    
    // Set network parameters
    Souliss_SetIPAddress(ip_address, subnet_mask, ip_gateway);
    SetAsGateway(myvNet_address);                                   // Set this node as gateway for SoulissApp  
    
    // Set the typical logic to use, T11 is a ON/OFF Digital Output with Timer Option
    Set_T11(SLOT_LED1);
    Set_T11(SLOT_LED2);
    
    // Define LEDs pins as output
    pinMode(9, OUTPUT);
    pinMode(13,OUTPUT);
    
}

void loop()
{ 
    // Here we start to play
    EXECUTEFAST() {                     
        UPDATEFAST();   

        // Read every 510ms the input state and send it to the other board  
        FAST_510ms() {

            // Execute the logic
            Logic_T11(SLOT_LED1);
            Logic_T11(SLOT_LED2);
                                    
            // Set outputs
            DigOut(9, Souliss_T1n_Coil, SLOT_LED1);
            DigOut(13, Souliss_T1n_Coil, SLOT_LED1);
        }       
        
        // Here we handle here the communication with Android, commands and notification
        FAST_GatewayComms();                    
    }
} 