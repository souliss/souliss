/**************************************************************************
	Souliss - IoTuino LYT Light Bulb
	
	This sketch control the Authometion LYT bubls through Souliss, use an
	Arduino Ethernet (or with Ethernet shield) and an LYT Shield.
	
	>>>>>>>>
		This node control the main light of the living room, based on LYT
		lamp bulb with control of brightness and color.
	<<<<<<<<
	
***************************************************************************/
#include "bconf/StandardArduino.h"              // Use an Arduino board
#include "bconf/LYT88_LEDBulb_Radio.h"          // Define the board type
#include "conf/usart_fast.h"                    // Ethernet through Wiznet W5100

// Include framework code and libraries
#include <SPI.h>
#include "Souliss.h"

// Include Authometion libraries
#include <PL1167.h>
#include <Lytwifi.h>
#include <WiFiInterrupt.h>
#include "DHT.h"

// There are not used in this sketch, keep it just to pat the compiler
#include <EEPROM.h>
#include <SoftwareSerial.h>

// Define logic slots, multicolor lights use four slots
#define LYTLIGHT1			0	
#define LYTLIGHT2			4	
#define LYTLIGHT3			8	
#define	LIGHTINTESITY		9
#define	TEMPERATURE			11
#define	HUMIDITY			13

uint8_t toggleswitch = 0;

DHT dht(A1, DHT11);
float th=0;

void setup()
{	
    Initialize();	

	// Set the inputs
	pinMode(8,INPUT);	
	pinMode(A0, INPUT);
	dht.begin();

    // Set network parameters
    SetAddress(0xCE02, 0xFF00, 0xCE01);
	
    // Se the LYT bulbs (index, bulb type, addr_a, addr_b, logic slot), here we have
    // the first lamp (index 0) with default addresses (0, 0). Change these based on
    // the configuration of your lamps.
    InitLYT();
    SetLYT(0, 0, 1, LYTLIGHT1);
    SetLYT(1, 0, 2, LYTLIGHT2);
    SetLYT(2, 0, 3, LYTLIGHT3);

    // Define a logic to handle the bulb(s)
    SetLYTLamps(LYTLIGHT1); 
    SetLYTLamps(LYTLIGHT2); 
    SetLYTLamps(LYTLIGHT3); 

	Set_Light(LIGHTINTESITY);
	Set_Temperature(TEMPERATURE);
	Set_Humidity(HUMIDITY);
}

void loop()
{ 
	// Here we start to play
	EXECUTEFAST() {						
		UPDATEFAST();	
		
        // Is an unsual approach, but to get fast response to color change we run the LYT logic and
        // basic communication processing at maximum speed.
        LogicLYTLamps(LYTLIGHT1);       
        LogicLYTLamps(LYTLIGHT2);  
        LogicLYTLamps(LYTLIGHT3);  
        ProcessCommunication();
        
        // Here we process all communication with other nodes
        FAST_1110ms() {
            LYTState(LYTLIGHT1);
            LYTState(LYTLIGHT2);
            LYTState(LYTLIGHT3);
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
		FAST_PeerComms();	
	}	
	
	EXECUTESLOW() {
		UPDATESLOW();
		
		// Slowly shut down the lamp
		SLOW_x10s(15) {
			
			// Read the light intensity
			AnalogIn(A0, LIGHTINTESITY, 0.1, 0);
		
			// Read the humidity
			th = dht.readHumidity();
			ImportAnalog(HUMIDITY, &th);
			
			// Read the temperature
			th = dht.readTemperature();
			ImportAnalog(TEMPERATURE, &th);			
		}

        SLOW_x10s(9) {
            LYTStateRequest(LYTLIGHT1);     // Request the lamp state
            LYTSleepTimer(LYTLIGHT1);       // Slowly shut down the lamp
        }

        SLOW_x10s(11) {
            LYTStateRequest(LYTLIGHT1);     // Request the lamp state
            LYTSleepTimer(LYTLIGHT1);       // Slowly shut down the lamp
        }

        SLOW_x10s(13) {
            LYTStateRequest(LYTLIGHT1);     // Request the lamp state
            LYTSleepTimer(LYTLIGHT1);       // Slowly shut down the lamp
        }

	}
} 