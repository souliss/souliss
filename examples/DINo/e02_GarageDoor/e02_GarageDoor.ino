/**************************************************************************
    Souliss - Garage Door
    
    It control a garage door acting on the relays that drive the motor and 
    get the limit switches, open and close command are only available via
    Android (or any other) interface.
    
    This logics is fine if act on device with over current protection and 
    obstacle detection, if not limit switches are mandatory.
    
    Applicable for:
        - Garage doors,
        - Motorized Gates.
    
    Only one command input is provided, it toggle the output within three
    states. The available output states are:
            - Close,
            - Open,
            - Stop.     
    Every change from Close to Open or vice-versa goes through Stop state.

    During opening and if Open, the light is automatically turned ON.
    
    After a command is issued, the device wait for a limit switch to declare
    the final state, if limit switch doesn't came within a configurable time
    the final state is declared based on the issued command.
    
    The Local Device has
        - Limit Switch Open on input 1
        - Limit Switch Close on input 2
        - Open Coil on relay 1
        - Close Coil on relay 2
        - Light on relay 3

***************************************************************************/

// Let the IDE point to the Souliss framework
#include "SoulissFramework.h"

#include "bconf/DINo_v2.h"                  // Define the board type
#include "conf/Gateway.h"                   // The main node is the Gateway, we have just one node

// Include framework code and libraries
#include <SPI.h>

/*** All configuration includes should be above this line ***/ 
#include "Souliss.h"

// Define the network configuration
uint8_t ip_address[4]  = {192, 168, 1, 77};
uint8_t subnet_mask[4] = {255, 255, 255, 0};
uint8_t ip_gateway[4]  = {192, 168, 1, 1};

#define GARAGEDOOR                  0           // This identify the number of the logic
#define GARAGELIGHT                 1           // This identify the number of the logic

void setup()
{   
    // Init the board
    InitDINo();

    // Setup the network configuration
    //
    Souliss_SetIPAddress(ip_address, subnet_mask, ip_gateway);
    SetAsGateway((U16)ip_address[3]);                                       // Last byte of the IP address is the vNet address
    
    // Define the logics for this node
    Set_GarageDoor(GARAGEDOOR);
    Set_SimpleLight(GARAGELIGHT);
    
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
}


void loop()
{ 
    // Here we start to play
    EXECUTEFAST() {                     
        UPDATEFAST();   
        
        FAST_110ms() {   // We check incoming communication data every 110 milliseconds
            // If the door or gate is open / opening, switch ON the light
            if((mOutput(GARAGEDOOR) == Souliss_T2n_Coil_Open) || (mOutput(GARAGEDOOR) == Souliss_T2n_LimSwitch_Open))
                mInput(GARAGELIGHT) = Souliss_T1n_OnCmd;
            else if((mOutput(GARAGEDOOR) == Souliss_T2n_Coil_Close) || (mOutput(GARAGEDOOR) == Souliss_T2n_LimSwitch_Close))
                mInput(GARAGELIGHT) = Souliss_T1n_OffCmd;
            
            // Execute the logic for light
            Logic_SimpleLight(GARAGELIGHT);
            
            // Use Relay3 as output on the electrical load (light)
            DigOut(RELAY3, Souliss_T1n_OnCoil, GARAGELIGHT);            
        }   

        FAST_510ms() {   // We check incoming communication data every 510 milliseconds
            // Use input 1 and input 2 as limit switches
            DigIn(IN1, Souliss_T2n_LimSwitch_Open, GARAGEDOOR);
            DigIn(IN2, Souliss_T2n_LimSwitch_Close, GARAGEDOOR);            
            
            // Execute the logic for door or gate
            Logic_GarageDoor(GARAGEDOOR);
            
            // Use Relay1 and Relay2 for the garagedoor 
            DigOut(RELAY1, Souliss_T2n_Coil_Open, GARAGEDOOR);  
            DigOut(RELAY2, Souliss_T2n_Coil_Close, GARAGEDOOR);             
        }       
        
        // Here we process all communication with other nodes
        FAST_GatewayComms();    
        
    }
    
    EXECUTESLOW()
    {   
        UPDATESLOW();

        SLOW_10s() {               
            // The timer value is set by second state of DigIn2State
            Timer_GarageDoor(GARAGEDOOR);
        } 
    }   
} 