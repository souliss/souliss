/**************************************************************************
    Souliss - Control RGB LED Strip
    
    It control via Android an RGB LED or Strip using the MOD-RGB extension
    module, the communication goes over Ethernet using MOD-ENC28J60 or 
    MOD-WIFI.
    
    This code allow the manual control of the LED using a color picker, or
    drive the LED color using the music played by your smartphone.

    To use MOD-RGB and a communication board (MOD-ENC28J60 or MOD-WIFI)
    is required a ribbon cable that extend the UEXT bus to both the boards,
    the signal dedicated for I2C shall not be extended to the communication
    modules.

    Run this code on one of the following boards:
      - Olimexino328 and MOD-ENC28J60 plus MOD-RGB

    As option you can run the same code on the following, just changing the
    relevant configuration file at begin of the sketch
      - Olimexino328 and MOD-WIFI plus MOD-RGB
      - Olimexino32U4 and MOD-ENC28J60 plus MOD-RGB
      - Olimexino32U4 and MOD-WIFI plus MOD-RGB
      - AVR T-32U4 and MOD-ENC28J60 plus MOD-RGB
      - AVR T-32U4 and MOD-WIFI plus MOD-RGB
    
***************************************************************************/

// Let the IDE point to the Souliss framework
#include "SoulissFramework.h"

// Configure the framework
#include "bconf/Olimexino328.h"             // Use a Olimex Olimexino328
#include "bconf/OlimexMOD-ENC28J60.h"       // Ethernet through Microchip ENC28J60
#include "bconf/OlimexMOD-RGB.h"            // Output for LED Strip with Olimex MOD-RGB
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

#define SLOT_RGB_STATE      0                   // This is the memory slot used for the execution of the logic
#define SLOT_RGB_COLOR      SLOT_RGB_STATE+1    // This is the memory slot used for the execution of the logic

void setup()
{   
    Initialize();
    
    // Set network parameters
    Souliss_SetIPAddress(ip_address, subnet_mask, ip_gateway);
    SetAsGateway(myvNet_address);                                   // Set this node as gateway for SoulissApp  
    
    // Set the typical logic to use, T16 is a color picker and music analyzer
    Set_T16(SLOT_RGB_STATE);
    
    // Init the MOD-RGB module
    mrgb_Init();
    
}

void loop()
{ 
    // Here we start to play
    EXECUTEFAST() {                     
        UPDATEFAST();   

        // Read every 50ms the input state and send it to the other board   
        FAST_50ms() {
            // Execute the logic
            Logic_T16(SLOT_RGB_STATE);
                                    
            // Set outputs
            mrgb_SetColor(&mOutput(SLOT_RGB_COLOR));
        }   
    }

    EXECUTESLOW() { 
        UPDATESLOW();

        // Execute the timer to handle the timed-on states
        SLOW_10s() {    

            Timer_T16(SLOT_RGB_STATE);                      
        }     
    }   
} 