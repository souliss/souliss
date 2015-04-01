

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
uint8_t ip_address[4]  = {192, 168, 1, 21};
uint8_t subnet_mask[4] = {255, 255, 255, 0};
uint8_t ip_gateway[4]  = {192, 168, 1, 1};

#define	Gateway_vNetAddress			18	       // The vNet address for an Ethernet IP node is the last 
											// number of the IP address
#define	PEER1              			21							

//Slot
#define CALDAIA_CMD  		        0      //Pin 7			
#define CALDAIA_ACCESA 		        1      //Pin 2			
#define CALDAIA_ALLARME		        2      //Pin 4			        	

#define TEMP_MANDATA_CALDAIA            10     //Pin AIN 5			        	
#define TEMP_RITORNO_CALDAIA	        12     //Pin AIN 4
#define TEMP_RITORNO_IMPIANTO	        14     //Pin AIN 3
#define TEMP_FUMI        	        16     //Pin AIN 2

		

//DeadBand
#define DEADBAND			0.05
#define DEADBANDNTC			0.05		  



//Ntc Termistori           
Termistore temp2(2);                               //Thermistor Library (pin n°)
float TM2;                                         //Thermistor Return Value                       
Termistore temp3(3);                               //Thermistor Library (pin n°)
float TM3;                                         //Thermistor Return Value                       
Termistore temp4(4);                               //Thermistor Library (pin n°)
float TM4;                                         //Thermistor Return Value                       
Termistore temp5(5);                               //Thermistor Library (pin n°)
float TM5;                                         //Thermistor Return Value 

//HeartBeat
long heartBeatArray[] = {
    50, 100, 15, 1200};
int hbeatIndex = 1;    // this initialization is not important  
long prevMillis;
int LEDpin = 9;
//HeartBeat

void setup()
{	
	Souliss_SetIPAddress(ip_address, subnet_mask, ip_gateway);
	

        Souliss_SetT12(memory_map, CALDAIA_CMD);
	Souliss_SetT13(memory_map, CALDAIA_ACCESA);
        Souliss_SetT13(memory_map, CALDAIA_ALLARME);


        Souliss_SetT52(memory_map, TEMP_MANDATA_CALDAIA);
        Souliss_SetT52(memory_map, TEMP_RITORNO_CALDAIA);
        Souliss_SetT52(memory_map, TEMP_RITORNO_IMPIANTO);
        Souliss_SetT52(memory_map, TEMP_FUMI);

       
        	
	// Define inputs, outputs pins
        pinMode(2, INPUT);					// CALDAIA_ACCESA
        pinMode(4, INPUT);					// CALDAIA_ALLARME   

        
	pinMode(7, OUTPUT);					// CALDAIA_CMD

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

                Souliss_DigIn2State(2,Souliss_T1n_OnCmd,Souliss_T1n_OffCmd,memory_map,CALDAIA_ACCESA);
                Souliss_DigIn2State(4,Souliss_T1n_OnCmd,Souliss_T1n_OffCmd,memory_map,CALDAIA_ALLARME);

                        //Uscite
                        Souliss_DigOut(7, Souliss_T1n_Coil, memory_map, CALDAIA_CMD);              
                        
                // Execute the logic
	        Souliss_Logic_T12(memory_map, CALDAIA_CMD, &data_changed);      
                Souliss_Logic_T13(memory_map, CALDAIA_ACCESA, &data_changed);     
                Souliss_Logic_T13(memory_map, CALDAIA_ALLARME, &data_changed);

          
          
		}	
        FAST_1110ms()
		{
                        Souliss_Logic_T52(memory_map, TEMP_MANDATA_CALDAIA, DEADBANDNTC, &data_changed);
                        Souliss_Logic_T52(memory_map, TEMP_RITORNO_CALDAIA, DEADBANDNTC, &data_changed);
                        Souliss_Logic_T52(memory_map, TEMP_RITORNO_IMPIANTO, DEADBANDNTC, &data_changed);
                        Souliss_Logic_T52(memory_map, TEMP_FUMI, DEADBANDNTC, &data_changed);
                        
                }
        }	
EXECUTESLOW()
	{	
	UPDATESLOW();

	SLOW_10s()
		{                 
                        // Acquire data from Thermistor
                        TM2 = temp2.getTemp();
                        TM3 = temp3.getTemp();
                        TM4 = temp4.getTemp();
                        TM5 = temp5.getTemp();
         
			// Acquire data from the microcontroller ADC
                        Souliss_ImportAnalog(memory_map, TEMP_MANDATA_CALDAIA, &TM5);      
                        Souliss_ImportAnalog(memory_map, TEMP_RITORNO_CALDAIA, &TM4);   
                        Souliss_ImportAnalog(memory_map, TEMP_RITORNO_IMPIANTO, &TM3);
                        Souliss_ImportAnalog(memory_map, TEMP_FUMI, &TM2);
                }
        }
    //HeartBeat
    heartBeat(1.5);     // try changing parameter 	
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
