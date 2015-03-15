

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "bconf/inSketch.h"
//#include "bconf/SmallNetwork.h"
#define	QC_BOARDTYPE				0x04	// Arduino with Ethernet Shield (W5100)
#define	QC_GATEWAYTYPE				0x00	// Disabled
#define	QC_INTERFACE				0x00	// Disabled

#include "Souliss.h"
#include "SpeakEasy.h"						// Is a library to code easy Souliss examples
#include <SPI.h>
#include <Termistore.h>


// Define the network configuration
uint8_t ip_address[4]  = {192, 168, 1, 24};
uint8_t subnet_mask[4] = {255, 255, 255, 0};
uint8_t ip_gateway[4]  = {192, 168, 1, 1};

#define	Gateway_vNetAddress			18		// The vNet address for an Ethernet IP node is the last 
// number of the IP address
#define	PEER3              			24

//Slot
#define TEMPERATURA                     0
#define UMIDITA                  	2
#define CALDAIA                  	4
#define CONZDIZIONATORE         	5
#define TEMPERATURA_T31                 6



//DeadBand
#define DEADBAND	        	0.05


//DHT22
//Serbatoio Stoccaggio
#define	DHT_id1				1
ssDHT22_Init(18, DHT_id1);



void setup()
{
  Souliss_SetIPAddress(ip_address, subnet_mask, ip_gateway);

  
  Souliss_SetT52(memory_map, TEMPERATURA);
  Souliss_SetT53(memory_map, UMIDITA);
  Souliss_SetT31(memory_map, TEMPERATURA_T31);


  // Define inputs, outputs pins
  pinMode(3, OUTPUT);		// Heater
  pinMode(5, OUTPUT);		// Fan 1
  pinMode(6, OUTPUT);		// Fan 2
  pinMode(7, OUTPUT);		// Fan 3



  ssDHT_Begin(DHT_id1);                                   // Temperatura

  //Seriale
  Serial.begin(9600);


}

void loop()
{
  // Here we start to play
  EXECUTEFAST() {
    UPDATEFAST();

    // Here we process all communication with other nodes
    FAST_PeerComms();

    FAST_110ms() {

      
      	// Start the heater and the fans
	Souliss_nDigOut(3, Souliss_T3n_HeatingOn, memory_map, TEMPERATURA_T31);	// Heater
	Souliss_nDigOut(5, Souliss_T3n_FanOn1   , memory_map, TEMPERATURA_T31);	// Fan1
	Souliss_nDigOut(6, Souliss_T3n_FanOn2   , memory_map, TEMPERATURA_T31);	// Fan2
	Souliss_nDigOut(7, Souliss_T3n_FanOn3   , memory_map, TEMPERATURA_T31);	// Fan3

      }
    FAST_1110ms()
      {
      //Aggiorno Valori Grafica
      Souliss_Logic_T52(memory_map, TEMPERATURA, DEADBAND, &data_changed);
      Souliss_Logic_T53(memory_map, UMIDITA, DEADBAND, &data_changed);
     
     
      // Compare the setpoint from the user interface with the measured temperature
      Souliss_Logic_T31(memory_map, TEMPERATURA_T31, &data_changed);
      }
      
      
    }
  EXECUTESLOW()
  {
    UPDATESLOW();

    SLOW_10s()
    {
      //Temperatura DHT22
      float temperature1 = ssDHT_readTemperature(DHT_id1);
      Souliss_ImportAnalog(memory_map, TEMPERATURA, &temperature1);
      Souliss_ImportAnalog(memory_map, TEMPERATURA_T31, &temperature1);
      float humidity1 = ssDHT_readHumidity(DHT_id1);
      Souliss_ImportAnalog(memory_map, UMIDITA, &humidity1);
      
      
      


    }
  
}}
