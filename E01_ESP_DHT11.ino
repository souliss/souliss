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

// Include and Configure DHT11 SENSOR
#include "DHT.h"
#define DHTPIN 13     // what pin we're connected to
#define DHTTYPE DHT11   // DHT 11 
DHT dht(DHTPIN, DHTTYPE, 15);

// This identify the number of the LED logic
#define TEMPERATURE         0
#define HUMIDITY            2

#define Debug Serial        //Change to Serial1 if you want to use the GPIO2 to TX

void setup()
{   
    Initialize();
    
    Debug.begin(115200); 
    Debug.println("DHTxx test!");
    dht.begin();
    
    // Connect to the WiFi network and get an address from DHCP
    GetIPAddress();     
    SetAsGateway(myvNet_dhcp);
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
              if (isnan(h) || isnan(t) {
                Debug.println("Failed to read from DHT sensor!");
              }
            
              Debug.print("Humidity: "); 
              Debug.print(h);
              Debug.print(" %\t");
              Debug.print("Temperature: "); 
              Debug.print(t);
              Debug.print(" *C ");

              Souliss_ImportAnalog(memory_map, TEMPERATURE, &t);
              Souliss_ImportAnalog(memory_map, HUMIDITY, &h); 
            } 
    }
}    
