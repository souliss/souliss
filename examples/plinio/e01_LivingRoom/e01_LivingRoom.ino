/**************************************************************************
	Souliss - IoTuino LYT Light Bulb
	
	This sketch control the Authometion LYT bubls through Souliss, use an
	Arduino Ethernet (or with Ethernet shield) and an LYT Shield.
	
	>>>>>>>>
		This node control the main light of the living room, based on LYT
		lamp bulb with control of brightness and color.
	<<<<<<<<
	
***************************************************************************/
#include "bconf/StandardArduino.h"				// Use an Arduino board
#include "bconf/IoTuino_RadioOnly_W5100.h"		// Define the board type
#include "conf/ethW5100.h"						// Ethernet through Wiznet W5100
#include "conf/Gateway.h"						// Define a gateway

// Include framework code and libraries
#include <SPI.h>
#include "Souliss.h"
#include "DHT.h"

// Define logic slots, multicolor lights use four slots
#define LYTLIGHT1			0					
#define	LIGHTINTESITY		4
#define	TEMPERATURE			6
#define	HUMIDITY			8

// Define the network configuration
uint8_t ip_address[4]  = {192, 168, 3, 88};
uint8_t subnet_mask[4] = {255, 255, 255, 0};
uint8_t ip_gateway[4]  = {192, 168, 3, 1};

uint8_t toggleswitch = 0;

DHT dht(A1, DHT11);
float th=0;

void setup()
{	
	// Set the inputs
	pinMode(8,INPUT);	
	pinMode(A0, INPUT);
	dht.begin();
	
	// Init the IoTuino radio using pin9 as chip select for the SPI communication
	InitIoTuino_RadioOnly(9);

	// Setup the IP configuration
	//
	// The vNet address will be equal to the last byte of the IP address
	// with the most significant one at zero, so in this case 0x0011.
	// Board reply to ping at 192.168.1.17
	Souliss_SetIPAddress(ip_address, subnet_mask, ip_gateway);
	SetAsGateway((U16)ip_address[3]);
	
	// Se the LYT bulbs (index, bulb type, addr_a, addr_b, logic slot)
	SetLYT(0, 2, 0, 1, LYTLIGHT1);
	Set_Light(LIGHTINTESITY);
	Set_Temperature(TEMPERATURE);
	Set_Humidity(HUMIDITY);
	
	// Define a logic to handle the bulb(s)
	SetLYTLamps(LYTLIGHT1);	
}

void loop()
{ 
	// Here we start to play
	EXECUTEFAST() {						
		UPDATEFAST();	
		
		FAST_10ms() {	// We process the logic and relevant input and output every 110 milliseconds
			ProcessCommunication();
			
			// Drive the LYT bulb
			LogicLYTLamps(LYTLIGHT1);		
		} 
		
	FAST_510ms() {	
	  
		// If the capacitive button has been press
		if(digitalRead(8))	{
			
			// Toggle between increasing and decreasing
			if(toggleswitch)	toggleswitch=0;
			else				toggleswitch=1;
			
			// Set the color as white
			mInput(LYTLIGHT1+1) = 0xF0;
			mInput(LYTLIGHT1+2) = 0xF0;
			mInput(LYTLIGHT1+3) = 0xF0;
			mInput(LYTLIGHT1) = Souliss_T1n_Set;		
			LogicLYTLamps(LYTLIGHT1);
			
			while(digitalRead(8))
			{
				// Select if bright up or down
				if(toggleswitch) mInput(LYTLIGHT1) = Souliss_T1n_BrightUp;
				else			 mInput(LYTLIGHT1) = Souliss_T1n_BrightDown;

				// Drive the LYT bulb
				LogicLYTLamps(LYTLIGHT1);
				delay(250);	
			}	
    }	

	// Process analogue values
	FAST_9110ms() {
		
		Logic_Light(LIGHTINTESITY);
		Logic_Humidity(HUMIDITY);
		Logic_Temperature(TEMPERATURE);
		
	}
	
}
		// Here we process all communication with other nodes
		FAST_GatewayComms();	
	}	
	
	EXECUTESLOW() {
		UPDATESLOW();
		
		// Slowly shut down the lamp
		SLOW_10s() {
			LYTSleepTimer(LYTLIGHT1);
			
			// Read the light intensity
			AnalogIn(A0, LIGHTINTESITY, 0.1, 0);
		
			// Read the humidity
			th = dht.readHumidity();
			ImportAnalog(HUMIDITY, &th);
			
			// Read the temperature
			th = dht.readTemperature();
			ImportAnalog(TEMPERATURE, &th);			
		}
	}
} 