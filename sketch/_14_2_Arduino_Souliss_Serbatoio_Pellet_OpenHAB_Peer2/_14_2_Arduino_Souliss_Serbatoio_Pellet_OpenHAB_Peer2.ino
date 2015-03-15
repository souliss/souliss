

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
uint8_t ip_address[4]  = {192, 168, 1, 23};
uint8_t subnet_mask[4] = {255, 255, 255, 0};
uint8_t ip_gateway[4]  = {192, 168, 1, 1};

#define	Gateway_vNetAddress			18		// The vNet address for an Ethernet IP node is the last 
// number of the IP address
#define	PEER2              			23

//Slot
#define COCLEA 		                0
#define SENS_COCLEA_ROT          	1
#define SENS_PELLET_OK          	2
#define SENS_PELLET_TROPPO_PIENO  	3
#define LIVELLO_PELLET_STOCCAGGIO	4
#define LIVELLO_PELLET_CALDAIA   	6
#define TEMP_SERBATOIO_STOCCAGGIO	8
#define UMID_SERBATOIO_STOCCAGGIO	10
#define TEMP_MOTORE			12


//DeadBand
#define DEADBAND		0.01

//Ntc Termistori
Termistore temp5(5);                               //Thermistor Library (pin n°)
float TM5;                                         //Thermistor Return Value


//DHT22
//Serbatoio Stoccaggio
#define	DHT_id2						2
ssDHT22_Init(29, DHT_id2);


float pellet_stoccaggio;
float pellet_caldaia;
float sommapellet_caldaia;
float sommapellet_stoccaggio;

//HeartBeat
long heartBeatArray[] = {
  50, 100, 15, 1200
};  
int hbeatIndex = 1;    // this initialization is not important
long prevMillis;
int LEDpin = 9;
//HeartBeat


void setup()
{
  Souliss_SetIPAddress(ip_address, subnet_mask, ip_gateway);


  Souliss_SetT12(memory_map, COCLEA);
  Souliss_SetT13(memory_map, SENS_COCLEA_ROT);
  Souliss_SetT13(memory_map, SENS_PELLET_OK);
  Souliss_SetT13(memory_map, SENS_PELLET_TROPPO_PIENO);
  Souliss_SetT51(memory_map, LIVELLO_PELLET_STOCCAGGIO);
  Souliss_SetT51(memory_map, LIVELLO_PELLET_CALDAIA);
  Souliss_SetT52(memory_map, TEMP_SERBATOIO_STOCCAGGIO);
  Souliss_SetT53(memory_map, UMID_SERBATOIO_STOCCAGGIO);
  Souliss_SetT52(memory_map, TEMP_MOTORE);

  // Define inputs, outputs pins
  pinMode(3, INPUT);		         	        // SENS_COCLEA_ROT
  pinMode(18, INPUT);					// SENS_PELLET_OK
  pinMode(19, INPUT);					// SENS_PELLET_TROPPO_PIENO
  pinMode(21, INPUT);		         	        // Sensore Ultrasuoni ECHO, A0 su Arduino Leonardo
  pinMode(20, INPUT);		         	        // Sensore Ultrasuoni ECHO, A1 su Arduino Leonardo


  pinMode(8, OUTPUT);					// COCLEA
  pinMode(2, OUTPUT);			        	// Sensore Ultrasuoni TRIGGER



  ssDHT_Begin(DHT_id2);                                   // SERBATOIO STOCCAGGIO

  //Seriale
  Serial.begin(9600);

  //HeartBeat
  pinMode(LEDpin, OUTPUT);
  //HeartBeat

}

void loop()
{
  // Here we start to play
  EXECUTEFAST() {
    UPDATEFAST();

    // Here we process all communication with other nodes
    FAST_PeerComms();

    FAST_510ms() {   // We check incoming communication data every 510 milliseconds

      //Ingressi
      Souliss_DigIn2State(3, Souliss_T1n_OnCmd, Souliss_T1n_OffCmd, memory_map, SENS_COCLEA_ROT);
      Souliss_DigIn2State(18, Souliss_T1n_OnCmd, Souliss_T1n_OffCmd, memory_map, SENS_PELLET_OK);
      Souliss_DigIn2State(19, Souliss_T1n_OnCmd, Souliss_T1n_OffCmd, memory_map, SENS_PELLET_TROPPO_PIENO);



      //Uscite
      Souliss_DigOut(8, Souliss_T1n_Coil, memory_map, COCLEA);


      // Execute the logic
      Souliss_Logic_T12(memory_map, COCLEA, &data_changed);
      Souliss_Logic_T13(memory_map, SENS_COCLEA_ROT, &data_changed);
      Souliss_Logic_T13(memory_map, SENS_PELLET_OK, &data_changed);
      Souliss_Logic_T13(memory_map, SENS_PELLET_TROPPO_PIENO, &data_changed);


    }
    FAST_1110ms()
    {
      //Aggiorno Valori Grafica
      Souliss_Logic_T51(memory_map, LIVELLO_PELLET_STOCCAGGIO, DEADBAND, &data_changed);
      Souliss_Logic_T51(memory_map, LIVELLO_PELLET_CALDAIA, DEADBAND, &data_changed);
      Souliss_Logic_T52(memory_map, TEMP_SERBATOIO_STOCCAGGIO, DEADBAND, &data_changed);
      Souliss_Logic_T53(memory_map, UMID_SERBATOIO_STOCCAGGIO, DEADBAND, &data_changed);
      Souliss_Logic_T52(memory_map, TEMP_MOTORE, DEADBAND, &data_changed);

    }
  }
  EXECUTESLOW()
  {
    UPDATESLOW();

    SLOW_10s()
    {

      //Acquisizione NTC
      TM5 = temp5.getTemp();
      Souliss_ImportAnalog(memory_map, TEMP_MOTORE, &TM5);


      //Temperatura DHT22 Serbatoio Stoccaggio
      float temperature1 = ssDHT_readTemperature(DHT_id2);
      Souliss_ImportAnalog(memory_map, TEMP_SERBATOIO_STOCCAGGIO, &temperature1);
      float humidity1 = ssDHT_readHumidity(DHT_id2);
      Souliss_ImportAnalog(memory_map, UMID_SERBATOIO_STOCCAGGIO, &humidity1);




      //HC-SR04
      digitalWrite(2, LOW);
      delayMicroseconds(5);
      digitalWrite(2, HIGH);
      delayMicroseconds(50);
      digitalWrite(2, LOW);
      // Conversione Segnale da Sensore
      float distance = pulseIn(21, HIGH);
      pellet_stoccaggio = ((distance / 20) * -1) + 110.0;
      sommapellet_stoccaggio = sommapellet_stoccaggio + pellet_stoccaggio;
      Serial.print (sommapellet_stoccaggio,DEC);
      Serial.print ("   ");

      digitalWrite(2, LOW);
      delayMicroseconds(5);
      digitalWrite(2, HIGH);
      delayMicroseconds(50);
      digitalWrite(2, LOW);
      float distance1 = pulseIn(20, HIGH);
      pellet_caldaia = ((distance1 / 20) * -1) + 110.0;
      sommapellet_caldaia = sommapellet_caldaia + pellet_caldaia;
      Serial.println (sommapellet_caldaia,DEC);
    }
     SLOW_110s()
    { 
     //Trasferico valore mediato 
      pellet_stoccaggio = sommapellet_stoccaggio/10;
      Serial.print ("Pellet Stoccaggio   ");      
      Serial.println (pellet_stoccaggio,DEC);
      Souliss_ImportAnalog(memory_map, LIVELLO_PELLET_STOCCAGGIO, &pellet_stoccaggio);
      sommapellet_stoccaggio=0;
     //Trasferico valore mediato 
      pellet_caldaia = sommapellet_caldaia/10; 
      Serial.print ("Pellet Caldaia   ");  
      Serial.println (pellet_caldaia,DEC);
      Souliss_ImportAnalog(memory_map, LIVELLO_PELLET_CALDAIA, &pellet_caldaia); 
      sommapellet_caldaia=0;
      
    }
  
}  
  
  //HeartBeat
  heartBeat(1.5);
  //HeartBeat
}

//HeartBeat
void heartBeat(float tempo) {
  if ((millis() - prevMillis) > (long)(heartBeatArray[hbeatIndex] * tempo)) {
    hbeatIndex++;
    if (hbeatIndex > 3) hbeatIndex = 0;

    if ((hbeatIndex % 2) == 0) {    // modulo 2 operator will be true on even counts
      digitalWrite(LEDpin, HIGH);
    }
    else {
      digitalWrite(LEDpin, LOW);
    }
    //  Serial.println(hbeatIndex);
    prevMillis = millis();
  }
}
void stopHeatbeat() {
  digitalWrite(LEDpin, LOW);
}
//HeartBeat

