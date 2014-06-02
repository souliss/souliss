/**************************************************************************
	Souliss - SeeedStudio Grove (Gateway Node)
	
	This node is a gateway and collect data from all your SeeedStudio Grove
	nodes connected via XBee Series-1 radio. It use a Grove Temperature and 
	Humidity Sensor (based on DHT11) to read data and provide them on Android.
	
	You need to download SoulissApp from Google Play store.
	
	Boards details:
		Run this sketch using a Seeeduino Ethernet (ATmega328 + Wiznet W5100)
		connected to your home router, ensure that a free IP address that ends
		with .77 (example 192.168.1.77) is available and not yet used on your
		LAN network.
		
		Connect a "Grove - Temperature & Humidity Sensor" on the SDA, SCK 
		grove connector.
		
		Only after the upload of the sketch, connect on the other grove connector
		a "Grove - Bee Socket" with a "XBee Pro Chip Antenna S1" over it, then
		hold a reset. This radio will be used to connect with other boards (if any)
	
	Bill of Materials:
		- Seeeduino Ethernet qty. 1	(ARD07061P)
		- Grove Temp&Humi Sensor qty. 1 (SEN11301P)
		- Grove Bee Socket qty. 1 (COM0581P)
		- XBee Pro Chip S1 (WLS03202M)
	
***************************************************************************/

// Souliss can run over different hardware, here we define the board that we are using,
// the parameters and the available configuration are listed in QuickCfg.h file located
// in /conf folder.
//
// Define an Arduino Ethernet (or compatible like Seeeduino Ethernet) using the USART
// as additional communication interface, the USART is used for the communication via
// XBee radio. The node is defined as Gateway because need to collect data from others
// wireless node in your network.
#include "bconf/inSketch.h"			
#define	QC_BOARDTYPE	0x41	// Arduino Ethernet (or Ethernet Shield) with USART 	
#define	QC_GATEWAYTYPE	0x01	// Gateway
#define	QC_INTERFACE	0x00	// Disabled

#include "Souliss.h"
#include "SpeakEasy.h"
#include <SPI.h>

#define	DHT_id1					1			// Identify the sensor, in case of more than one used on the same board			
#define TEMPERATURE				0			// Identify the temperature logic
#define HUMIDITY				2			// Identify the humidaty logic

ssDHT11_Init(A5, DHT_id1);					// Setup the DHT sensor, use ssDHT11_Init or ssDHT22_Init for DHT11 and DHT22

void setup()
{	
	// Set a temperature and humidity measure	
	Set_Temperature(TEMPERATURE);		// Define one temperature measure
	Set_Humidity(HUMIDITY);				// Define one humidity measure
	ssDHT_Begin(DHT_id1);				// Init the DHT sensor

	// We don't assign a fixed IP address to the board, the board will get the base IP
	// information from the first broadcast frame sent out with the Android application.
	// By default the board will get an IP address with .77 as last byte, you can change
	// it in runtime using the Android application SoulissApp.
	SetAddressingServer();	
}

void loop()
{ 
	// Here we start to play
	EXECUTEFAST() {					
		UPDATEFAST();	
					  
		FAST_1110ms() { // Compare previous and new input measure and provide the updates to SoulissApp
			Logic_Temperature(TEMPERATURE);
			Logic_Humidity(HUMIDITY);			
		}
		
		// Here we process all communication with other nodes and Android
		FAST_GatewayComms();	
	}
	
	EXECUTESLOW() {	
		UPDATESLOW();

		SLOW_110s() {  // Read temperature and humidity from DHT every 110 seconds  
			ssGetTemperatureDHT(DHT_id1, TEMPERATURE);
			ssGetHumidityDHT(DHT_id1, HUMIDITY);		
		} 	  
	}
} 