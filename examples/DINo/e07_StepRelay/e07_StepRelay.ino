/**************************************************************************
    Souliss - Step Relay
    
    Control a step relay and use a feedback to get the current state, this
	allow the control of a light from multiple sources that may also not
	be under the control of Souliss.

	The step relay must have an auxiliary contact (or any other type of
	feedback to provide its original state back to the board.
	
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

// Define a step relay
#define STEPPER1          0

void setup()
{   
    // Init the board
    InitDINo();

    // Setup the network configuration
    //
    Souliss_SetIPAddress(ip_address, subnet_mask, ip_gateway);
    SetAsGateway((U16)ip_address[3]);                                       // Last byte of the IP address is the vNet address
    
    // Define the logics for this node
    Set_StepRelay(STEPPER1);
    
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
        
        FAST_50ms() {   // We process the logic and relevant input and output every 50 milliseconds

			// Read the command input and the actual state from the step relay	
			DigIn(IN2, Souliss_T1n_ToggleCmd, STEPPER1);
			DigIn2State(IN1, Souliss_T1n_OnFeedback, Souliss_T1n_OffFeedback, STEPPER1);
			
			// Process the logic and give an output command
			Logic_StepRelay(STEPPER1);   
			DigOut(RELAY1, Souliss_T1n_Coil, STEPPER1);
        } 

		FAST_x10ms(10) {
			// The output coil to the step relay is active for five cycle, so approximately five times 100 ms
			// use a different value to match your requirements
			Timer_StepRelay(STEPPER1);
		}

        // Here we process all communication with other nodes
        FAST_GatewayComms();    
    }
    
    EXECUTESLOW() { 
        UPDATESLOW();
    }
} 