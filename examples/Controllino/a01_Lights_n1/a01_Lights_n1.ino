/**************************************************************************
    Souliss - Controllino Gateway Ethernet
    
    This example show the use of a Controlino Maxi Arduino PLC
		as Souliss Gateway conntected through ethernet port
		
		Prerequisite: Support for Controllino boards correctly installed in Arduino IDE

		As option you can run the same code on the following, just changing the
    relevant configuration file at begin of the sketch
      - Controllino Maxi
      - Controllino Mega
            
***************************************************************************/
#include <Controllino.h>                    // Include Controllino names

// Configure the framework
#include "bconf/ControllinoMaxi.h"          // Use a controllino maxi
#include "conf/Gateway.h"                   // The main node is the Gateway

// Include framework code and libraries
#include <SPI.h>
#include "Souliss.h"

// Define the network configuration according to your router settings
#define Gateway_address 77
uint8_t ip_address[4]  = {192, 168, 1, Gateway_address};
uint8_t subnet_mask[4] = {255, 255, 255, 0};
uint8_t ip_gateway[4]  = {192, 168, 1, 1};
#define Peer_address    78
#define myvNet_address  ip_address[3]       // The last byte of the IP address (77) is also the vNet address
#define myvNet_subnet   0xFF00
#define myvNet_supern   Gateway_address

#define LIGHT1                  0           // This is the memory slot used for the execution of the logic
#define LIGHT2                  1           
#define LIGHT3                  2           
#define LIGHT4                  3           

void setup()
{   
	Initialize();
	
	// Setup Controllino pins
	pinMode(CONTROLLINO_RELAY_00, OUTPUT);
	pinMode(CONTROLLINO_RELAY_01, OUTPUT);
	pinMode(CONTROLLINO_RELAY_02, OUTPUT);
	pinMode(CONTROLLINO_RELAY_03, OUTPUT);

	pinMode(CONTROLLINO_SCREW_TERMINAL_DIGITAL_IN_00, INPUT);
	pinMode(CONTROLLINO_SCREW_TERMINAL_DIGITAL_IN_01, INPUT);
	pinMode(CONTROLLINO_SCREW_TERMINAL_DIGITAL_IN_02, INPUT);
	pinMode(CONTROLLINO_SCREW_TERMINAL_DIGITAL_IN_03, INPUT);
	
	// Set network parameters
	Souliss_SetIPAddress(ip_address, subnet_mask, ip_gateway);
	SetAsGateway(myvNet_address);                                   // Set this node as gateway for SoulissApp  
	
	// This node as gateway will get data from the Peer
	SetAsPeerNode(Peer_address, 1); 
	
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

		// Read every 510ms the input state and send it to the other board  
		FAST_510ms() {			
			DigIn(CONTROLLINO_SCREW_TERMINAL_DIGITAL_IN_00, Souliss_T1n_ToggleCmd, LIGHT1);          // Read inputs from CONTROLLINO_SCREW_TERMINAL_DIGITAL_IN_00
			DigIn(CONTROLLINO_SCREW_TERMINAL_DIGITAL_IN_01, Souliss_T1n_ToggleCmd, LIGHT2);          // Read inputs from CONTROLLINO_SCREW_TERMINAL_DIGITAL_IN_01
			DigIn(CONTROLLINO_SCREW_TERMINAL_DIGITAL_IN_03, Souliss_T1n_ToggleCmd, LIGHT3);          // Read inputs from CONTROLLINO_SCREW_TERMINAL_DIGITAL_IN_02
			DigIn(CONTROLLINO_SCREW_TERMINAL_DIGITAL_IN_04, Souliss_T1n_ToggleCmd, LIGHT4);          // Read inputs from CONTROLLINO_SCREW_TERMINAL_DIGITAL_IN_03

			Logic_SimpleLight(LIGHT1);                          // Execute the logic for Relay 1
			Logic_SimpleLight(LIGHT2);                          // Execute the logic for Relay 2
			Logic_SimpleLight(LIGHT3);                          // Execute the logic for Relay 3
			Logic_SimpleLight(LIGHT4);                          // Execute the logic for Relay 4
			
			DigOut(CONTROLLINO_RELAY_00, Souliss_T1n_Coil, LIGHT1);           // Drive the Relay 1
			DigOut(CONTROLLINO_RELAY_01, Souliss_T1n_Coil, LIGHT2);           // Drive the Relay 2
			DigOut(CONTROLLINO_RELAY_02, Souliss_T1n_Coil, LIGHT3);           // Drive the Relay 3
			DigOut(CONTROLLINO_RELAY_03, Souliss_T1n_Coil, LIGHT4);           // Drive the Relay 4		
		} 
		
		// Here we handle here the communication with Android, commands and notification
		// are automatically assigned to MYLEDLOGIC
		FAST_GatewayComms();
	}
} 
