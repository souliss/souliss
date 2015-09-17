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
#define	LYTLIGHT0			0	
#define	LIGHTINTESITY		4
#define	TEMPERATURE			6
#define	HUMIDITY			8

#define	LowMood									0xFF01,0x01
#define	FullLight								0xFF01,0x02

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
    // the first lamp (index 0) with default addresses (0, 1). Change these based on
    // the configuration of your lamps.
    InitLYT();
    SetLYT(0, 0, 1, LYTLIGHT0);

    // Define a logic to handle the bulb(s)
    SetLYTLamps(LYTLIGHT0); 
	Set_Light(LIGHTINTESITY);
	Set_Temperature(TEMPERATURE);
	Set_Humidity(HUMIDITY);
}

void loop()
{ 
	// Here we start to play
	EXECUTEFAST() {						
		UPDATEFAST();	       

		LogicLYTLamps(LYTLIGHT0);
        ProcessCommunication();
		
		// Listen for topic published by other nodes
		FAST_110ms() {
			
			// Set LowMood
			if(subscribe(LowMood))
			{
				mInput(LYTLIGHT0) = Souliss_T1n_OffCmd;
				LogicLYTLamps(LYTLIGHT0);

				// Turn on one light
				LYTOn(LYTLIGHT0);
				delay(50);

				mInput(LYTLIGHT0+1) = 0x64;
				mInput(LYTLIGHT0+2) = 0x23;
				mInput(LYTLIGHT0+3) = 0x00;
				mInput(LYTLIGHT0) = Souliss_T1n_Set;		
				LogicLYTLamps(LYTLIGHT0);				
			}

			// Set Full Light
			if(subscribe(FullLight))
			{
				mInput(LYTLIGHT0) = Souliss_T1n_OnCmd;
				LogicLYTLamps(LYTLIGHT0);
				delay(50);

				mInput(LYTLIGHT0+1) = 0xFF;
				mInput(LYTLIGHT0+2) = 0xFF;
				mInput(LYTLIGHT0+3) = 0xFF;
				mInput(LYTLIGHT0) = Souliss_T1n_Set;		
				LogicLYTLamps(LYTLIGHT0);				
			}


		}

		FAST_1110ms() {
			// Check for a state update
			LYTState(LYTLIGHT0);			
		}

		FAST_510ms() {	
	  
			// If the capacitive button has been press
			if(digitalRead(8))	{
				
				// Toggle between increasing and decreasing
				if(toggleswitch)	toggleswitch=0;
				else				toggleswitch=1;
				
				// Set the color as white
				mInput(LYTLIGHT0+1) = 0xF0;
				mInput(LYTLIGHT0+2) = 0xF0;
				mInput(LYTLIGHT0+3) = 0xF0;
				mInput(LYTLIGHT0) = Souliss_T1n_Set;		
				LogicLYTLamps(LYTLIGHT0);
				
				while(digitalRead(8))
				{
					// Select if bright up or down
					if(toggleswitch) 
						mInput(LYTLIGHT0) = Souliss_T1n_BrightUp;
					else
						mInput(LYTLIGHT0) = Souliss_T1n_BrightDown;

					// Drive the LYT bulb
					LogicLYTLamps(LYTLIGHT0);
					delay(250);	
				}	
			}	
		}

		// Process analogue values
		FAST_9110ms() {
			Logic_Light(LIGHTINTESITY);
			Logic_Humidity(HUMIDITY);
			Logic_Temperature(TEMPERATURE);

			// Request the actual state of the lamp
			LYTStateRequest();
		}
	}	
	
	EXECUTESLOW() {
		UPDATESLOW();
		
		// Slowly shut down the lamp
		SLOW_10s() {
			
			// Read the light intensity
			AnalogIn(A0, LIGHTINTESITY, 0.1, 0);
		
			// Read the humidity
			th = dht.readHumidity();
			ImportAnalog(HUMIDITY, &th);
			
			// Read the temperature
			th = dht.readTemperature();
			ImportAnalog(TEMPERATURE, &th);			

            LYTSleepTimer(LYTLIGHT0);       // Slowly shut down the lamp
        }
	}
} 