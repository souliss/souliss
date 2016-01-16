/**************************************************************************
    Souliss - Lights
    
    It handle the MOD-IO2 relays either via GPIOs inputs or using the Android
    interface. Connecting the relays to lights or similar electrical appliance, 
    you can get remote control of them. 
    
    To use MOD-IO2 and a communication board (MOD-ENC28J60 or MOD-WIFI) is
    required a ribbon cable that extend the UEXT bus to both the boards, the signal 
    dedicated for I2C shall not be extended to the communication modules.

    Run this code on one of the following boards:
      - Olimexino328 and MOD-ENC28J60 plus MOD-IO2

    As option you can run the same code on the following, just changing the
    relevant configuration file at begin of the sketch
      - Olimexino328 and MOD-WIFI plus MOD-IO2 or MOD-IO
      - Olimexino32U4 and MOD-ENC28J60 plus MOD-IO2 or MOD-IO
      - Olimexino32U4 and MOD-WIFI plus MOD-IO2 or MOD-IO
      - AVR T-32U4 and MOD-ENC28J60 plus MOD-IO2 or MOD-IO
      - AVR T-32U4 and MOD-WIFI plus MOD-IO2 or MOD-IO
    
***************************************************************************/

// Let the IDE point to the Souliss framework
#include "SoulissFramework.h"

// Configure the framework
#include "bconf/Olimexino328.h"             // Use a Olimex Olimexino328
#include "bconf/OlimexMOD-ENC28J60.h"       // Ethernet through Microchip ENC28J60
#include "bconf/OlimexMOD-IO2.h"            // I/O with Olimex MOD-IO2
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

#define SLOT_RELAY1                 0           // This is the memory slot used for the execution of the logic
#define SLOT_RELAY2                 1           // This is the memory slot used for the execution of the logic

void setup()
{   
    Initialize();
    
    // Set network parameters
    Souliss_SetIPAddress(ip_address, subnet_mask, ip_gateway);
    SetAsGateway(myvNet_address);                                   // Set this node as gateway for SoulissApp  
    
    // Set the typical logic to use, T11 is a ON/OFF Digital Output with Timer Option
    Set_T11(SLOT_RELAY1);
    Set_T11(SLOT_RELAY2);
    
    // Define inputs : GPIO1, GPIO0 - Outputs : GPIO6, GPIO5, the MODIO relays doesn't need an init
    MODIO_Init();
    mIO_PinMode(~(GPIO6 | GPIO5) & (GPIO1 | GPIO0));    

    // Set pullups for input pins
    mIO_Pullup((GPIO1 | GPIO0));        
    
}

void loop()
{ 
    // Here we start to play
    EXECUTEFAST() {                     
        UPDATEFAST();   

        // Read every 510ms the input state and send it to the other board  
        FAST_510ms() {
            
            // Get all inputs states
            m_In();
        
            // Inputs are pulled up at 3.3V, if placed at GND value the input will be trigged
            // and associated to a toggle command
            m_LowDigIn(GPIO0, Souliss_T1n_ToggleCmd, SLOT_RELAY1);
            m_LowDigIn(GPIO1, Souliss_T1n_ToggleCmd, SLOT_RELAY2);                      
            
            // Execute the logic
            Logic_T11(SLOT_RELAY1);
            Logic_T11(SLOT_RELAY2);
                                    
            // Set outputs
            m_SetOut(RELAY1, Souliss_T1n_Coil, SLOT_RELAY1);
            m_SetOut(RELAY2, Souliss_T1n_Coil, SLOT_RELAY2);
            
            // Act on relays based on set outputs
            m_RelayOut();       
            
        }   

        // Here we handle here the communication with Android, commands and notification
        FAST_GatewayComms();                    
        
    }
} 