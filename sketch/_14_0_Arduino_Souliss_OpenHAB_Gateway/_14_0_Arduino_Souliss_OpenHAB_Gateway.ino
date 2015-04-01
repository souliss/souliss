
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "bconf/inSketch.h"
//#include "bconf/SmallNetwork.h"

#define QC_BOARDTYPE    0x04    // Arduino with Ethernet Shield (W5100)
#define QC_GATEWAYTYPE  0x02    // Gateway 
#define QC_INTERFACE    0x00    // 

#include "Souliss.h"
#include "SpeakEasy.h"	
#include <SPI.h>
#include <Termistore.h>



// Define the network configuration
uint8_t ip_address[4]  = {192, 168, 1, 18};
uint8_t subnet_mask[4] = {255, 255, 255, 0};
uint8_t ip_gateway[4]  = {192, 168, 1, 1};

#define	Gateway_vNetAddress			18		// The vNet address for an Ethernet IP node is the last 
											// number of the IP address
#define	PEER1            			21	
#define	PEER2            			23	
#define	PEER3            			24	



#define	DHT_id1					1		// Identify the sensor, in case of more than one used on the same board

//Slot

#define TEMPERATURE				0			// This is the memory slot used for the execution of the logic in network_address1
#define HUMIDITY				2			// This is the memory slot used for the execution of the logic
#define TEMPERATUREAN			        4			// This is the memory slot used for the execution of the logic in network_address1
#define HUMIDITYAN				6			// This is the memory slot used for the execution of the logic


		


#define DEADBAND				0.05		// Deadband value 5%  
ssDHT22_Init(2, DHT_id1);
ssDHT22_Init(3, DHT_id2);

//HeartBeat
long heartBeatArray[] = {
    50, 100, 15, 1200};
int hbeatIndex = 1;    // this initialization is not important  
long prevMillis;
int LEDpin = 3;
void setup()
{	
	Souliss_SetIPAddress(ip_address, subnet_mask, ip_gateway);
	SetAsGateway(Gateway_vNetAddress);										// Last byte of the IP address is the vNet address
	SetAsPeerNode(PEER1,1);
        SetAsPeerNode(PEER2,2);
        SetAsPeerNode(PEER3,3);

      Souliss_SetT52(memory_map, TEMPERATURE);
      Souliss_SetT53(memory_map, HUMIDITY);
      Souliss_SetT52(memory_map, TEMPERATUREAN);
      Souliss_SetT53(memory_map, HUMIDITYAN);
      

 
  ssDHT_Begin(DHT_id1);
  ssDHT_Begin(DHT_id2);
  
//HeartBeat
pinMode(LEDpin, OUTPUT);
//HeartBeat
}

void loop()
{ 
EXECUTEFAST() {						
		UPDATEFAST();	
		
		FAST_GatewayComms();	

		FAST_110ms() {   // We check the openHAB HTTP/XML interface every 70 milliseconds
			
		}

                FAST_510ms() {
     
	}			
				  
		FAST_1110ms()
		{
                      Souliss_Logic_T52(memory_map, TEMPERATURE, DEADBAND, &data_changed);
		      Souliss_Logic_T53(memory_map, HUMIDITY, DEADBAND, &data_changed);
                      Souliss_Logic_T52(memory_map, TEMPERATUREAN, DEADBAND, &data_changed);
		      Souliss_Logic_T53(memory_map, HUMIDITYAN, DEADBAND, &data_changed);
                }
	}
EXECUTESLOW()
	{	
		UPDATESLOW();

		SLOW_10s()
		{                 
	                 // Read temperature value from DHT sensor and convert from single-precision to half-precision
		      float temperature = ssDHT_readTemperature(DHT_id1);
		      Souliss_ImportAnalog(memory_map, TEMPERATURE, &temperature);
                      float temperaturean = ssDHT_readTemperature(DHT_id2);
		      Souliss_ImportAnalog(memory_map, TEMPERATUREAN, &temperaturean);
			
			// Read humidity value from DHT sensor and convert from single-precision to half-precision
	              float humidity = ssDHT_readHumidity(DHT_id1);
		      Souliss_ImportAnalog(memory_map, HUMIDITY, &humidity);
                      float humidityan = ssDHT_readHumidity(DHT_id2);
		      Souliss_ImportAnalog(memory_map, HUMIDITYAN, &humidityan);
		} 	
		
		
	}
    //HeartBeat
    heartBeat(1.5);    	
    //HeartBeat
} 
//HeartBeat
void heartBeat(float tempo){
    if ((millis() - prevMillis) > (long)(heartBeatArray[hbeatIndex] * tempo)){
        hbeatIndex++;
        if (hbeatIndex > 3) hbeatIndex = 0;

        if ((hbeatIndex % 2) == 0){     // modulo 2 operator will be true on even counts
            digitalWrite(LEDpin, HIGH);
        }
        else{ 
            digitalWrite(LEDpin, LOW); 
        }
        //  Serial.println(hbeatIndex);
        prevMillis = millis();
    }
}
void stopHeatbeat(){
    digitalWrite(LEDpin, LOW); 
}
//HeartBeat
