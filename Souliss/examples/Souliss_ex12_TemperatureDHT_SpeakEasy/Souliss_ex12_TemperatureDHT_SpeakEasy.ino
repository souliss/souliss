/**************************************************************************
	Souliss - Temperature and Humidity Measure (Speak Easy)
	
	Read temperature and humidity from a DHT11/21/22 sensor and makes them
	available over an Android device running SoulissApp. It use Souliss
	Speak Easy macro to make write a sketch even simpler.
 
	CONFIGURATION IS MANDATORY BEFORE COMPILING
	
	Run this code on one of the following boards:
	
		Board Conf Code			Board Model
        0x03        			Arduino Ethernet (W5100) 
		0x04					Arduino with Ethernet Shield (W5100)
		0x05					Arduino with ENC28J60 Ethernet Shield	

		Sensor Conf
		0x01				Temperature and humidity DHT11, DHT21, DHT22
		
	******************** Configuration Parameters *********************
	
		Configuration file		Parameter
		QuickCfg.h				#define	QC_ENABLE			0x01
		QuickCfg.h				#define	QC_BOARDTYPE		0x03, 0x04, 0x05
		QuickCfg.h				#define	DHT_SENSOR			0x01

		QuickCfg.h				#define	QC_GATEWAYTYPE		0x01
		
	Is required an additional IP configuration using the following parameters
		QuickCfg.h				const uint8_t DEFAULT_BASEIPADDRESS[] = {...}
		QuickCfg.h				const uint8_t DEFAULT_SUBMASK[]       = {...}
		QuickCfg.h				const uint8_t DEFAULT_GATEWAY[]       = {...}
		
***************************************************************************/
#include "Souliss.h"
#include "SpeakEasy.h"
#include <SPI.h>

// Define the network configuration
//
// In the QuickCfg.h file set DEFAULT_BASEIPADDRESS[] = {192,168,1,0}
#define myvNet_address		0x0011 			// 0x0011 is equal to 17 in decimal
#define myvNet_subnet		0xFF00
#define myvNet_supern		0x0000
// defining myvNet_address as 0x0011 (is hexadecimal, in decimal is uqual to 17) gives
// as IP address 192.168.1.17, use this address in SoulissApp 

#define	DHT_id1					1			// Identify the sensor, in case of more than one used on the same board			
#define TEMPERATURE				0			// Identify the temperature logic
#define HUMIDITY				2			// Identify the humidaty logic

ssDHT11_Init(2, DHT_id1);					// Setup the DHT sensor, use ssDHT11_Init or ssDHT22_Init for DHT11 and DHT22

void setup()
{	
	// Setup the network configuration
	Souliss_SetAddress(myvNet_address, myvNet_subnet, myvNet_supern);		
	SetAsGateway(myvNet_address);		// Set this node as gateway for SoulissApp	
	
	Set_Temperature(TEMPERATURE);		// Define one temperature measure
	Set_Humidity(HUMIDITY);				// Define one humidity measure
	ssDHT_Begin(DHT_id1);				// Init the DHT sensor
}

void loop()
{ 
	// Here we start to play
	EXECUTEFAST() {					
		UPDATEFAST();	
		
		FAST_70ms() {   // We check incoming communication data every 70 milliseconds
			ProcessCommunication();										
		}	
			  
		FAST_1110ms() { // Compare previous and new input measure and provide the updates to SoulissApp
			Logic_Temperature(TEMPERATURE);
			Logic_Humidity(HUMIDITY);			
		}
	}
	
	EXECUTESLOW() {	
		UPDATESLOW();

		SLOW_110s() {  // Read temperature and humidity from DHT every 110 seconds  
			ssGetTemperatureDHT(DHT_id1, TEMPERATURE);
			ssGetHumidityDHT(DHT_id1, HUMIDITY);		
		} 	  
	}
} 