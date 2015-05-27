# Souliss_ESP_Examples
First steps for using Arduino IDE + ESP

(Thank You Dario)
This is the procedure from the Arduino/ESP8266 Github

•Install Arduino 1.6.4 from the Arduino website.
•Start Arduino and open Perferences window.
•Enter http://arduino.esp8266.com/package_esp8266com_index.json into Additional Board Manager URLs field. You can add multiple URLs, separating them with commas.
•Open Boards Manager from Tools > Board menu and install esp8266 platform (and don't forget to select your ESP8266 board from Tools > Board menu after installation).

Once done, install manually (not from the library manager) souliss friariello
https://github.com/souliss/souliss/archive/friariello.zip

-------------------------------------------------------------------------------------------

This page is very usefull, most of the info explained here is related to the readme.md
https://github.com/esp8266/Arduino

To use this integration and examples you need to download some libraries.

DHT Integration:
-Library: https://github.com/adafruit/DHT-sensor-library

-Example: 


 // Configure the framework
#include "bconf/MCU_ESP8266.h"              // Load the code directly on the ESP8266
#include "conf/Gateway.h"                   // The main node is the Gateway, we have just one node
#include "conf/DynamicAddressing.h"         // Use dynamic addressing
//#include "conf/DisableEEPROM.h"


// Define the WiFi name and password
#define WIFICONF_INSKETCH
#define WiFi_SSID               "SSID"
#define WiFi_Password           "PASSWORD"    


// Include framework code and libraries
//#include <SPI.h>
#include <ESP8266WiFi.h>
#include <EEPROM.h>
#include "Souliss.h"


// Include and Configure DHT11 SENSOR
#include "DHT.h"
#define DHTPIN 13     // what pin we're connected to
#define DHTTYPE DHT11   // DHT 11 
DHT dht(DHTPIN, DHTTYPE, 15);


// This identify the number of the LED logic
#define TEMPERATURE         0
#define HUMIDITY            2


void setup()
{   
    Initialize();
    
    Serial.begin(115200); 
    Serial.println("DHTxx test!");
    dht.begin();
    
    // Connect to the WiFi network and get an address from DHCP
    Setup_ESP8266();                           
    SetAsGateway(myvNet_esp8266);       // Set this node as gateway for SoulissApp  
    SetAddressingServer();
    // This node will serve all the others in the network providing an address
    
    Set_Temperature(TEMPERATURE);
    Set_Humidity(HUMIDITY);
}


void loop()
{ 
    // Here we start to play
    EXECUTEFAST() {                     
        UPDATEFAST();   
        
        FAST_2110ms()
        {
           Logic_Temperature(TEMPERATURE);
           Logic_Humidity(HUMIDITY);
        }
              
        // Here we handle here the communication with Android
        FAST_GatewayComms();                                        
    }
    EXECUTESLOW() {
 UPDATESLOW();


            SLOW_10s() {  
   // Read temperature and humidity from DHT every 10 seconds  
           float h = dht.readHumidity();
              // Read temperature as Celsius
              float t = dht.readTemperature();
              
              // Check if any reads failed and exit early (to try again).
              if (isnan(h) || isnan(t) || isnan(f)) {
                Serial.println("Failed to read from DHT sensor!");
                //return;
              }
            
              Serial.print("Humidity: "); 
              Serial.print(h);
              Serial.print(" %\t");
              Serial.print("Temperature: "); 
              Serial.print(t);
              Serial.print(" *C ");


              
             Souliss_ImportAnalog(memory_map, TEMPERATURE, &t);
            Souliss_ImportAnalog(memory_map, HUMIDITY, &h); 
            } 
    }
}    




-Wiring. You can power from 3.3v directly the DHT and connect DATA to any PIN. 

 

Dallas Integration

- Library: https://github.com/milesburton/Arduino-Temperature-Control-Library

In most cases OneWire library isn't on the Arduino IDE or have an Older version, be sure to download the ESP one: https://github.com/esp8266/Arduino/tree/esp8266/libraries/OneWire

- Example: 



// Configure the framework
#include "bconf/MCU_ESP8266.h"              // Load the code directly on the ESP8266
#include "conf/Gateway.h"                   // The main node is the Gateway, we have just one node
#include "conf/DynamicAddressing.h"         // Use dynamic addressing


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


#include <OneWire.h>
#include <DallasTemperature.h>
#define DALLASPIN 4 //Se declara el pin donde se conectará la DATA
OneWire ourWire(DALLASPIN); //Se establece el pin declarado como bus para la comunicación OneWire
DallasTemperature sensors(&ourWire); //Se instancia la librería DallasTemperature


void setup()
{   
    Initialize();
    
    Serial.begin(115200);


    sensors.begin(); //Se inician los sensores DS18B20
    // Connect to the WiFi network and get an address from DHCP
    Setup_ESP8266();                           
    SetAsGateway(myvNet_esp8266);       // Set this node as gateway for SoulissApp  


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




- Wiring. You can power with 3.3v. You need a 4k7 resistor from VCC to DATA PIN, you can use any pin.

 

EmonLib Integration

Library: https://github.com/openenergymonitor/EmonLib

https://github.com/arduino/Arduino/tree/master/hardware/arduino/avr/libraries/SoftwareSerial





Due the limitations on the ADC pin I'll use an ATTINY to get the data and send via SoftwareSerial to the ESP, so first steps needs to be done before.

Load the sketch to Attiny:

 

•Start Arduino and Load the Arduino as ISP example to an Arduino (any of them)
•Connect to the Attiny following this scheme:
•
​Open Perferences window.
•Enter https://raw.githubusercontent.com/damellis/attiny/ide-1.6.x-boards-manager/package_damellis_attiny_index.json into Additional Board Manager URLs field. You can add multiple URLs, separating them with commas. Note: I've an error if I put the esp and the attiny separated by comma, but you can add the attiny and when its programmed leave only the ESP one.
•Open Boards Manager from Tools > Board menu and install attiny platform (and don't forget to select your Attiny 1mhz Internal Clock board from Tools > Board menu after installation).
•Select on Tools Arduino as ISP 
•Load this sketch to the Attiny:
•Sketch Attiny: 


#include <SoftwareSerial.h>
#include "EmonLib.h"             // Include Emon Library 
EnergyMonitor emon1;             // Create an instance 


SoftwareSerial TinySerial(3, 4); // RX, TX
#define LED 1
#define CT  A1


void setup()  
{
  // Open serial communications and let us know we are connected
 TinySerial.begin(9600);  // SET TO 1200 ON RECEIVER, I DONT KNOW WHY
 pinMode(LED, OUTPUT);
 emon1.current(CT, 10);       // Current: input pin, calibration.
}


void loop()
{
  float current = emon1.calcIrms(1480);
  TinySerial.print(current);
  delay(100);
  digitalWrite(LED,!digitalRead(LED));
}





The Current Sensor I use is SCT-013-000 and connecto to the PIN A1 of the Attiny using this scheme:


​Where RVD = 10kohm and Burden is 220ohm.

3.3v on vIn for Attiny is ok and does not affect to the meassure. I added a Activity led on pin 1 of Attiny, but this is optional.

Serial TX from the Attiny is on Pin 4 and must be connected to the RX from ESP.

Now on ESP we need to use for debug purposes the Serial1 (TX on GPIO2) and the Standard Serial Pins to receive data from Attiny, only the RX pin is used.  Note: This Serial1 only trasmit.

 

Due the ESP GPIO2 must be connected to GND to load sketches and the RX must be released from the Attiny before load a sketch I added a jumper, I'll use the same jumper to connect both, with this solution we avoid possible problems when we are loading sketches to the ESP.

 

And finally this is the example for ESP that reads the meassurement from attiny and push to Souliss

ESP example



 // Configure the framework
#include "bconf/MCU_ESP8266.h"              // Load the code directly on the ESP8266
#include "conf/Gateway.h"                   // The main node is the Gateway, we have just one node
#include "conf/DynamicAddressing.h"         // Use dynamic addressing


// Define the WiFi name and password
#define WIFICONF_INSKETCH
#define WiFi_SSID               "SSID"
#define WiFi_Password           "PASSWORD"    

// Include framework code and libraries
#include <ESP8266WiFi.h>
#include <EEPROM.h>
#include "Souliss.h"

// This identify the number of the LED logic
#define CURRENT             0
#define WATTS               2

void setup()
{   
    Initialize();
    
    Serial.begin(9600); 
    Serial1.begin(115200);

    // Connect to the WiFi network and get an address from DHCP
    Setup_ESP8266();                           
    SetAsGateway(myvNet_esp8266);       // Set this node as gateway for SoulissApp  

    // This node will serve all the others in the network providing an address
    
    Set_Current(CURRENT);
    Set_Power(WATTS);
}


void loop()
{ 
    
    EXECUTEFAST() {                     
        UPDATEFAST();   
        
        FAST_510ms()    {
           Logic_Current(CURRENT);
           Logic_Power(WATTS);
        }
        FAST_710ms()    {
           float current = 0;
           if(Serial.available()){
               float current = Serial.parseFloat();
               Serial1.println(current);
               
               Souliss_ImportAnalog(memory_map, CURRENT, &current); 
               float watt = current*230; 
               Souliss_ImportAnalog(memory_map, WATTS, &watt);
           }
        }
        // Here we handle here the communication with Android
        FAST_GatewayComms();                                        
    }
}    

 


 

Lux Integration

-Library:  No library needed, just a function to map values on example.

-Wiring: ADC pin from ESP have a range from 0 to 1v, so we need to use a voltage divider to down the range from 3.3v to 1, I tried with 220ohm and 100ohm resistors (1.031v) and works well.

-Scheme:  (Sorry by this, I explain better if needed)

3.3v --- 220ohm ---- LDR ---- 100 ohm ----- GND

                             LDR ----  -  ADC ----- 10kohm ------ GND

- Example:



// Configure the framework
#include "bconf/MCU_ESP8266.h"              // Load the code directly on the ESP8266
#include "conf/Gateway.h"                   // The main node is the Gateway, we have just one node
#include "conf/DynamicAddressing.h"         // Use dynamic addressing


// Define the WiFi name and password
#define WIFICONF_INSKETCH
#define WiFi_SSID               "SSID"
#define WiFi_Password           "PASSWORD"    


// Include framework code and libraries
#include <ESP8266WiFi.h>
#include <EEPROM.h>
#include "Souliss.h"


// This identify the number of the LED logic
#define LDR                 0


#define SensorLDR_pin   A0


//NUEVA FUNCION PARA LEER LDR Y PASARLO A KLUX:


// Light calibration data
// out[] holds the values wanted in lux/10
static const unsigned int out[] = { 7, 30, 45, 65, 150, 300, 450, 2100, 13000};  // x10  //ULTIMO VALOR REFERENCIA
static const unsigned int in[]  = { 100, 350, 430, 500, 680, 780, 950, 1005, 1024 };  // 0 - 1024


void setup()
{   
    Initialize();
    
    Serial.begin(115200);


    // Connect to the WiFi network and get an address from DHCP
    Setup_ESP8266();                           
    SetAsGateway(myvNet_esp8266);       // Set this node as gateway for SoulissApp  


    // This node will serve all the others in the network providing an address
    Set_T54(LDR);
}


void loop()
{ 
    
    EXECUTEFAST() {                     
        UPDATEFAST();   
        
        FAST_2110ms()
        {
           Logic_T54(LDR);
        } 
        
        FAST_7110ms()
        { 
              float ldr_read = get_lux(in, out, 16)/10.0;  //ORIGINAL
              if (ldr_read == 0) ldr_read = 0.01;
              Souliss_ImportAnalog(memory_map, LDR, &ldr_read);
        }
        
        // Here we handle here the communication with Android
        FAST_GatewayComms();                                        
    }
}    
    
   
//////////////////////////////////////////////////////////////////////////////
// Calculate lux based on rawADC reading from LDR returns value in lux/10
//////////////////////////////////////////////////////////////////////////////
int get_lux(const unsigned int* _in, const unsigned int* _out, byte size)
{
 
        // take care the value is within range
 // val = constrain(val, _in[0], _in[size-1]);


 int val = analogRead(A0);
        Serial.print("AnalogRead: ");
        Serial.println(val);
 if (val <= _in[0]) return _out[0];
 if (val >= _in[size-1]) return _out[size-1];


 // search right interval
 byte pos = 1;  // _in[0] allready tested
 while(val > _in[pos]) pos++;


 // this will handle all exact "points" in the _in array
 if (val == _in[pos]) return _out[pos];


 // interpolate in the right segment for the rest
 return map(val, _in[pos-1], _in[pos], _out[pos-1], _out[pos]);
}







PWM Integration

Library: No library needed, just use the last core from the Boards Managed

Note: analogWrite(PIN, VALUE*4) Value on ESP It's 0-1024 range instead the 0-255 from the Arduino, just add *4  to send the right value.

Example:



// Configure the framework
#include "bconf/MCU_ESP8266.h"              // Load the code directly on the ESP8266
#include "conf/Gateway.h"                   // The main node is the Gateway, we have just one node
#include "conf/DynamicAddressing.h"         // Use dynamic addressing


// Define the WiFi name and password
#define WIFICONF_INSKETCH
#define WiFi_SSID               "SSID"
#define WiFi_Password           "PASSWORD"    


// Include framework code and libraries
#include <ESP8266WiFi.h>
#include <EEPROM.h>
#include "Souliss.h"


// This identify the number of the SLOT logic
#define LEDPWM              9


//PWM pin
#define LEDPWMP 5


void setup()
{   
    Initialize();
    Serial.begin(115200);
   // Connect to the WiFi network and get an address from DHCP
    Setup_ESP8266();                           
    SetAsGateway(myvNet_esp8266);       // Set this node as gateway for SoulissApp  


    // This node will serve all the others in the network providing an address
    Set_DimmableLight(LEDPWM);
    
    pinMode(LEDPWMP, OUTPUT);


}


void loop()
{ 
    
    EXECUTEFAST() {                     
        UPDATEFAST();   
        
        FAST_50ms() {   // We process the logic and relevant input and output every 50 milliseconds
            Logic_DimmableLight(LEDPWM);                        
            analogWrite(LEDPWMP, mOutput(LEDPWM+1)*4);
        }


        // Here we handle here the communication with Android
        FAST_GatewayComms();                                        
    }
    EXECUTESLOW() {
 UPDATESLOW();


            SLOW_10s() {  // Read temperature and humidity from DHT every 110 seconds  
                Timer_DimmableLight(LEDPWM);              
            }            


    }
}    




I hope this can help many people to get your nodes working easily.
