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
#define LYTLIGHT1			4	
#define LYTLIGHT2			5	
#define LYTLIGHT3			6	
#define	LIGHTINTESITY		7
#define	TEMPERATURE			9
#define	HUMIDITY			11

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
    // the first lamp (index 0) with default addresses (0, 0). Change these based on
    // the configuration of your lamps.
    InitLYT();
    SetLYT(0, 0, 0, LYTLIGHT0);
    SetLYT(1, 0, 1, LYTLIGHT1);
    SetLYT(2, 0, 2, LYTLIGHT2);
    SetLYT(3, 0, 3, LYTLIGHT3);

    // Define a logic to handle the bulb(s)
    SetLYTLamps(LYTLIGHT0); 
	Set_T11(LYTLIGHT1);
	Set_T11(LYTLIGHT2);
	Set_T11(LYTLIGHT3);
	Set_Light(LIGHTINTESITY);
	Set_Temperature(TEMPERATURE);
	Set_Humidity(HUMIDITY);
}

void loop()
{ 
	// Here we start to play
	EXECUTEFAST() {						
		UPDATEFAST();	
		
		// Handle bubls color and brightness
        if(LogicLYTLamps(LYTLIGHT0))
		{
			// Align the single bulb state
			if(mOutput(LYTLIGHT0))
			{
				mInput(LYTLIGHT1) = Souliss_T1n_OnCmd;
				mInput(LYTLIGHT2) = Souliss_T1n_OnCmd;
				mInput(LYTLIGHT3) = Souliss_T1n_OnCmd; 
			}
			else
			{
				mInput(LYTLIGHT1) = Souliss_T1n_OffCmd;
				mInput(LYTLIGHT2) = Souliss_T1n_OffCmd;
				mInput(LYTLIGHT3) = Souliss_T1n_OffCmd; 				
			}
		}
        
        ProcessCommunication();

		// Turn ON/OFF the bulbs
        FAST_710ms() {
			if(Logic_T11(LYTLIGHT1))
				if(mOutput(LYTLIGHT1))	LYTOn(LYTLIGHT1);
				else					LYTOff(LYTLIGHT1);

			if(Logic_T11(LYTLIGHT2))
				if(mOutput(LYTLIGHT2))	LYTOn(LYTLIGHT2);
				else					LYTOff(LYTLIGHT2);

			if(Logic_T11(LYTLIGHT3))
				if(mOutput(LYTLIGHT3))	LYTOn(LYTLIGHT3);
				else					LYTOff(LYTLIGHT3);

			// If all are ON or all are OFF, align the RGB typical
			if(mOutput(LYTLIGHT1) && mOutput(LYTLIGHT2) && mOutput(LYTLIGHT3))
				mOutput(LYTLIGHT0) = Souliss_T1n_OnCoil;
			else if(!mOutput(LYTLIGHT1) && !mOutput(LYTLIGHT2) && !mOutput(LYTLIGHT3))
				mOutput(LYTLIGHT0) = Souliss_T1n_OffCoil;
         }
		
		// Listen for topic published by other nodes
		FAST_110ms() {
			
			// Set LowMood
			if(subscribe(LowMood))
			{
				mInput(LYTLIGHT0) = Souliss_T1n_OffCmd;
				mInput(LYTLIGHT1) = Souliss_T1n_OnCmd;
				mInput(LYTLIGHT2) = Souliss_T1n_OffCmd;
				mInput(LYTLIGHT3) = Souliss_T1n_OffCmd;
				LogicLYTLamps(LYTLIGHT0);

				// Turn on one light
				LYTOn(LYTLIGHT1);
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

            LYTSleepTimer(LYTLIGHT1);       // Slowly shut down the lamp
            LYTSleepTimer(LYTLIGHT2);       // Slowly shut down the lamp
            LYTSleepTimer(LYTLIGHT3);       // Slowly shut down the lamp
        }
	}
} 