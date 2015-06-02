// Configure the framework
#include "bconf/MCU_ESP8266.h"              // Load the code directly on the ESP8266
#include "conf/Gateway.h"                   // The main node is the Gateway, we have just one node
#include "conf/IPBroadcast.h"

// Define the WiFi name and password
#define WIFICONF_INSKETCH
#define WiFi_SSID               "SSID"
#define WiFi_Password           "PASSWORD"    

// Include framework code and libraries
#include <ESP8266WiFi.h>
#include <EEPROM.h>
#include "Souliss.h"

// This identify the number of the SLOT logic
#define DALLAS              0

#define DALLASPIN 4 //Se declara el pin donde se conectará la DATA

#include <OneWire.h>
#include <DallasTemperature.h>
OneWire ourWire(DALLASPIN); //Se establece el pin declarado como bus para la comunicación OneWire
DallasTemperature sensors(&ourWire); //Se instancia la librería DallasTemperature

#define Debug Serial        //Change to Serial1 if you want to use the GPIO2 to TX

void setup()
{   
    Initialize();
    
    Serial.begin(115200);

    sensors.begin(); //Se inician los sensores DS18B20
    // Connect to the WiFi network and get an address from DHCP
    GetIPAddress();      
    SetAsGateway(myvNet_dhcp); 
    // This is the vNet address for this node, used to communicate with other
	// nodes in your Souliss network
    SetAddress(0xAB01, 0xFF00, 0x0000);
    //Example of Peer Definition
    //SetAsPeerNode(0xAB02, 1);

    Set_Temperature(DALLAS);
}

void loop()
{ 
    EXECUTEFAST() {                     
        UPDATEFAST();   

        FAST_910ms()    {
          // Acquire temperature from the microcontroller ADC
          sensors.requestTemperatures(); //Prepara el sensor para la lectura
          float dallas = sensors.getTempCByIndex(0);
          Souliss_ImportAnalog(memory_map, DALLAS, &dallas);
        }      

        FAST_2110ms()
        {
           Logic_Temperature(DALLAS);
        } 
         // Here we handle here the communication with Android
        FAST_GatewayComms();                                        
    }
} 
