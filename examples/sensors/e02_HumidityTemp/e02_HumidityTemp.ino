/**************************************************************************
    Souliss - DHTxx
    
    Sensor
      - DHTxx
      // Connect pin 1 (on the left) of the sensor to +5V
      // NOTE: If using a board with 3.3V logic like an Arduino Due connect pin 1
      // to 3.3V instead of 5V!
      // Connect pin 2 of the sensor to whatever your DHTPIN is
      // Connect pin 4 (on the right) of the sensor to GROUND
      // Connect a 10K resistor from pin 2 (data) to pin 1 (power) of the sensor      
     
    Run this code on one of the following boards:
      - Arduino Ethernet (W5100) 
      - Arduino with Ethernet Shield (W5100)
      
    As option you can run the same code on the following, just changing the
    relevant configuration file at begin of the sketch
      - Arduino with ENC28J60 Ethernet Shield
      - Arduino with W5200 Ethernet Shield
      - Arduino with W5500 Ethernet Shield 
      
***************************************************************************/

// Let the IDE point to the Souliss framework
#include "SoulissFramework.h"

// Configure the framework
#include "bconf/StandardArduino.h"          // Use a standard Arduino
#include "conf/ethW5100.h"                  // Ethernet through Wiznet W5100
#include "conf/Gateway.h"                   // The main node is the Gateway, we have just one node
#include "conf/Webhook.h"                   // Enable DHCP and DNS

// Include framework code and libraries
#include <SPI.h>
#include <DHT.h>

// Include sensor libraries (from Adafruit) Uncomment whatever type you're using!
//#define DHTTYPE DHT11   // DHT 11
#define DHTTYPE DHT22     // DHT 22 (AM2302), AM2321
//#define DHTTYPE DHT21   // DHT 21 (AM2301)

#define DHTPIN  2         // what digital pin we're connected to

/*** All configuration includes should be above this line ***/ 
#include "Souliss.h"

#define HUMIDITY              0               // Leave 2 slots for T58
#define TEMP0                 2               // Leave 2 slots for T52

// DHT sensor
DHT dht(DHTPIN, DHTTYPE); // for ESP8266 use dht(DHTPIN, DHTTYPE, 11)

void setup()
{   
    Initialize();

    // Get the IP address from DHCP
    GetIPAddress();                          
    SetAsGateway(myvNet_dhcp);                  // Set this node as gateway for SoulissApp

    dht.begin();                                // initialize temperature sensor

    Set_Humidity(HUMIDITY);
    Set_Temperature(TEMP0);
}

void loop()
{ 
    // Here we start to play
    EXECUTEFAST() {                     
        UPDATEFAST();   

        // Execute the code every 1 time_base_fast      
        FAST_10ms() {
          
            // Just process communication as fast as the logics
            ProcessCommunication();
        } 

        // Process the other Gateway stuffs
        FAST_GatewayComms();          
    }
    EXECUTESLOW()
    {   
        UPDATESLOW();

        SLOW_10s()  {

            Logic_Humidity(HUMIDITY);
            Logic_Temperature(TEMP0);
                     
        }
        
        SLOW_50s()  {
          float humidity = dht.readHumidity();
          float temperature = dht.readTemperature(false);
          if (!isnan(humidity) || !isnan(temperature)) {
            ImportAnalog(HUMIDITY, &humidity);
            ImportAnalog(TEMP0, &temperature);
          }
        }   
    }       
} 
