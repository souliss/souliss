/**************************************************************************
    Souliss - Lights
    
    It handle the four inputs and outputs using the Android interface. Connecting 
    relays on the outputs lets control lights or similar electrical appliance, 
    you can get remote control of them.
    
    The Gateway node is connected via Ethernet to the Android (or any other)
    interface and via RS485 to all the peer nodes. 
    Use a twisted pair wire to connect the RS485 port of the Gateway to the
    nearest peer and from that peer to the next up to the end of the chain.
    
    Connection scheme:
    
    HOME-ROUTER                     GATEWAY     
    ETHERNET        (connect)       ETHERNET    

    GATEWAY                         PEER1
    RS-485 A        (connect)       RS-485 A
    RS-485 B        (connect)       RS-485 B
    RS-485 GND      (optional)      RS-485 GND
    
    Run this example on Industruino
    
    Applicable for:
        - Light
        - Other ON/OFF electrical appliance
    
***************************************************************************/

// Let the IDE point to the Souliss framework
#include "SoulissFramework.h"

#include "bconf/Industruino.h"                                   // Define the board type

// Include Industrino libraries
#include "Indio.h"
#include <Wire.h>

// Include framework code and libraries
#include <SPI.h>

/*** All configuration includes should be above this line ***/ 
#include "Souliss.h"

// Define the network configuration
uint8_t ip_address[4]  = {192, 168, 1, 77};
uint8_t subnet_mask[4] = {255, 255, 255, 0};
uint8_t ip_gateway[4]  = {192, 168, 1, 1};
#define Gateway_address         77
#define myvNet_address          ip_address[3]       // The last byte of the IP address (77) is also the vNet address
#define Gateway_RS485_address   0xCE01
#define Peer1_address           0xCE11
#define myvNet_subnet           0xFF00
#define myvNet_supern           Gateway_RS485_address

#define LIGHT1                  0           // This is the memory slot used for the execution of the logic
#define LIGHT2                  1           // This is the memory slot used for the execution of the logic
#define LIGHT3                  2           // This is the memory slot used for the execution of the logic
#define LIGHT4                  3           // This is the memory slot used for the execution of the logic
#define LIGHT_NO                4           
#define ONTIME                  15          // Light is on for ONTIME number of cycles


void setup()
{   
    // Init the board
    Initialize();
    
    // Define the address for the RS485 interface
    Souliss_SetAddress(Peer1_address, myvNet_subnet, Gateway_RS485_address);            
    
    // Set the inputs
    Indio.digitalMode(DI1, INPUT);
    Indio.digitalMode(DI2, INPUT);
    Indio.digitalMode(DI3, INPUT);
    Indio.digitalMode(DI4, INPUT);
    Indio.digitalMode(DO1, OUTPUT);
    Indio.digitalMode(DO2, OUTPUT);
    Indio.digitalMode(DO3, OUTPUT);
    Indio.digitalMode(DO4, OUTPUT);
     
    // Define Simple Light logics for the relays
    Set_SimpleLight(LIGHT1);
    Set_SimpleLight(LIGHT2);
    Set_SimpleLight(LIGHT3);
    Set_SimpleLight(LIGHT4);    
    
}

void loop()
{ 
    // Here we start to play
    EXECUTEFAST() {                     
        UPDATEFAST();   
        
        FAST_50ms() {   // We process the logic and relevant input and output every 50 milliseconds
        
           DigIn(DI1, Souliss_T1n_ToggleCmd, LIGHT1);          // Read inputs from IN1
            DigIn(DI2, Souliss_T1n_ToggleCmd, LIGHT2);          // Read inputs from IN2
            DigIn(DI3, Souliss_T1n_ToggleCmd, LIGHT3);          // Read inputs from IN3
            DigIn(DI4, Souliss_T1n_ToggleCmd, LIGHT4);          // Read inputs from IN4
        
            Logic_SimpleLight(LIGHT1);                          // Execute the logic for Relay 1
            Logic_SimpleLight(LIGHT2);                          // Execute the logic for Relay 2
            Logic_SimpleLight(LIGHT3);                          // Execute the logic for Relay 3
            Logic_SimpleLight(LIGHT4);                          // Execute the logic for Relay 4
            
            DigOut(DO1, Souliss_T1n_Coil, LIGHT1);              // Drive the Relay 1
            DigOut(DO2, Souliss_T1n_Coil, LIGHT2);              // Drive the Relay 2
            DigOut(DO3, Souliss_T1n_Coil, LIGHT3);              // Drive the Relay 3
            DigOut(DO4, Souliss_T1n_Coil, LIGHT4);              // Drive the Relay 4
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
        }     
    }
} 