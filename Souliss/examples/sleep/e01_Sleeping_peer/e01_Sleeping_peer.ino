/**************************************************************************
	Souliss - Sleeping
	
	A battery operated node (Peer) is activated on press of a pushbutton and/or on
	time base, each time the node wake-ups it sends data to the gateway (that is always
	powered on).
	
	To have low battery consumption you need a device without voltage regulator and other
	hardware that continuously get power even when the microcontroller is sleeping.  
	
	Run this code on one of the following boards:
	  - Arduino with Nordic nRF24L01 or nRF24L01+
	  
	As option you can run the same code on the following, just changing the
	relevant configuration file at begin of the sketch
	  -	Chibiduino v1
	  - Moteino
	  
***************************************************************************/

// Configure the framework
#include "bconf/StandardArduino.h"			// Use a standard Arduino
#include "conf/nRF24L01.h"
#include "conf/Sleep.h"

// Include framework code and libraries
#include <SPI.h>
#include "Souliss.h"

// Define the network configuration according to your router settings
#define	Gateway_address	0x6501				// The Gateway node has two address, one on the Ethernet side
											// and the other on the wireless one
#define	Peer_address	0x6502
#define	myvNet_subnet	0xFF00
#define	myvNet_supern	Gateway_address

#define BATT_LEVEL	0			 

void setup()
{	
	Initialize();
	
	// Set network parameters
	Souliss_SetAddress(Peer_address, myvNet_subnet, myvNet_supern);					// Address on the wireless interface	

	// Set an analog value to measure the battery voltage
	SetT55(memory_map, BATT_LEVEL);

	// Define the Gateway to be associated with, this is an unusual procedure
	// and is applicable on for sleeping nodes
	HardcodedChannel(Gateway_address);
	
	/*****
		The default sleep time is about 30 minutes, you can change this from the
		base/Sleeping.h file, or using the following defines on top of the sketch
		
		#define	SLEEPING_INSKETCH
		#define 	wakePin 	2
		#define	wakePinINT		0
		#define	wakeupTime		0x00FF
		#define	wakeupCycles	5		

		The time between two wake-ups is equal to wakeupTime*8seconds, using 0x00FF
		the node wakes every 34 minutes.
	*****/
	sleepInit(SLEEPMODE_TIMER);
}

void loop()
{   
	// If the node wake-ups then this statement is executed
	if(wasSleeping())
	{	
		// Here we start to play
		EXECUTEFAST() {						
			UPDATEFAST();	

			// Here we process the communication
			FAST_PeerComms();		

			// Check every 510ms how many cycles asleep as expired,
			// using default values, the nodes stays asleep 2.5 seconds
			// every 34 minutes
			FAST_510ms() {

				long vcc = readVcc();
				float vcc_f = (float) vcc;
				float vcc_to_send = vcc_f/1000;
				Souliss_ImportAnalog(memory_map, BATT_LEVEL, &vcc_to_send);
				Souliss_Logic_T55(memory_map, BATT_LEVEL, NODEADBAND, &data_changed);
			
				// Back to sleep
				if(isTimeToSleep())	
				{
					/**************
					Add below the code required to sleep custom devices connected, like:
						- Sensor,
						- Voltage regulator,
						- ...
					**************/
					
					// Sleep microcontroller and radio
					sleepNow();
				}	
			} 
			
		}
	}	
} 

long readVcc() {
	
	// Read 1.1V reference against AVcc
	// set the reference to Vcc and the measurement to the internal 1.1V reference
	#if defined(__AVR_ATmega32U4__) || defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
		ADMUX = _BV(REFS0) | _BV(MUX4) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
	#elif defined (__AVR_ATtiny24__) || defined(__AVR_ATtiny44__) || defined(__AVR_ATtiny84__)
		ADMUX = _BV(MUX5) | _BV(MUX0);
	#elif defined (__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__)
		ADMUX = _BV(MUX3) | _BV(MUX2);
	#else
		ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
	#endif  

	delay(75); // Wait for Vref to settle
	ADCSRA |= _BV(ADSC); // Start conversion
	while (bit_is_set(ADCSRA,ADSC)); // measuring

	uint8_t low  = ADCL; // must read ADCL first - it then locks ADCH  
	uint8_t high = ADCH; // unlocks both

	long result = (high<<8) | low;

	result = 1125300L / result; // Calculate Vcc (in mV); 1125300 = 1.1*1023*1000
	return result; // Vcc in millivolts
}