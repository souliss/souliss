/**************************************************************************
    Souliss - LightsGroup
    
    It handle the four relays either via IN1 input or using the
    Android interface. Connecting the relays to lights or similar electrical 
    appliance, you can get remote control of them.
    
    This example show the usage of LightsGroup to increase lightning in a room
    with more than one light from a single monostable switch.

    Hold the switch to turn all lights on in sequence. 

    Applicable for:
        - Light
        - Other ON/OFF electrical appliance
    
***************************************************************************/

// Let the IDE point to the Souliss framework
#include "SoulissFramework.h"

#include "bconf/DINo_v2.h"                  // Define the board type
#include "conf/Gateway.h"                   // The main node is the Gateway, we have just one node

#include <SPI.h>

/*** All configuration includes should be above this line ***/ 
#include "Souliss.h"

#define LIGHT_1			0
#define LIGHT_2			1
#define LIGHT_3			2		
#define LIGHT_4			3

// static IP address
uint8_t ip_address[4]  = {192, 168, 1, 77};
uint8_t subnet_mask[4] = {255, 255, 255, 0};
uint8_t ip_gateway[4]  = {192, 168, 1, 1};

void setup()
{
    // Init the board
    InitDINo();
    
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
    
    // Set and turn ON the status LED
    SetLED();
    TurnOnLED();

	Souliss_SetIPAddress(ip_address, subnet_mask, ip_gateway);

	Set_LightsGroup(LIGHT_1, LIGHT_4);
}

void loop()
{ 
	EXECUTEFAST() {						
		UPDATEFAST();	
		
		FAST_30ms() 
		{
			Souliss_DigInHoldSteps(IN1, memory_map, LIGHT_1, LIGHT_4, 500);
		} 

		FAST_50ms() 
		{
			Logic_LightsGroup(LIGHT_1, LIGHT_1);
			DigOut(RELAY1, Souliss_T1n_Coil, LIGHT_1);
			DigOut(RELAY2, Souliss_T1n_Coil, LIGHT_2);
			DigOut(RELAY3, Souliss_T1n_Coil, LIGHT_3);
			DigOut(RELAY4, Souliss_T1n_Coil, LIGHT_4); 
		}
		
		FAST_2110ms()
		{
			Timer_LightsGroup(LIGHT_1, LIGHT_4);
		}

        // Here we process all communication with other nodes
        FAST_GatewayComms();    		
	}
	
	EXECUTESLOW() 
	{	
		UPDATESLOW();
	}
} 