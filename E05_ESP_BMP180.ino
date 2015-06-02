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


// This identify the number of the LED logic
#define PRESSURE0    0
#define BMP180TEMP    2


// SDA and SCL pins can be configured, you need to edit SFE_BMP180/SFE_BMP180.cpp line 38. Preconfigured at 14, 12.
#include <SFE_BMP180.h>
#include <Wire.h>
#define ALTITUDE 20.0 // Altitude of reading location in meters


// You will need to create an SFE_BMP180 object, here called "pressure":
SFE_BMP180 pressure;

#define Debug Serial        //Change to Serial1 if you want to use the GPIO2 to TX

void setup()
{   
    Initialize();
    Debug.begin(115200);
    // Connect to the WiFi network and get an address from DHCP
                  
    GetIPAddress();     
    SetAsGateway(myvNet_dhcp);       // Set this node as gateway for SoulissApp  

    // This is the vNet address for this node, used to communicate with other
	// nodes in your Souliss network
    SetAddress(0xAB01, 0xFF00, 0x0000);
    //Example of Peer Definition
    //SetAsPeerNode(0xAB02, 1);

    if (pressure.begin())
       Debug.println("BMP180 init success");
    else
     {
              // Oops, something went wrong, this is usually a connection problem,
              // see the comments at the top of this sketch for the proper connections.
        
              Debug.println("BMP180 init fail\n\n");
              
      }
    
    
}


void loop()
{ 
    
    EXECUTEFAST() {                     
        UPDATEFAST();   
        
        // Here we handle here the communication with Android
        FAST_GatewayComms();                                        
    }
    EXECUTESLOW() {
 UPDATESLOW();


            SLOW_10s() {  // Read temperature and humidity from DHT every 110 seconds  


              Souliss_GetPressure_BMP180(PRESSURE0,BMP180TEMP); 
              
            } 
      }
}    
    
   


/***************************************************************************/
/*                         BMP180 I2C READING FUNCTION                     */
/***************************************************************************/
float Souliss_GetPressure_BMP180(uint8_t SLOT_PRESSURE, uint8_t SLOT_TEMPERATURE){
  
  boolean DEBUG_PRESSURE = 0;
 
  char status;
  double T,P,p0,a;


  // Loop here getting pressure readings every 10 seconds.


  // If you want sea-level-compensated pressure, as used in weather reports,
  // you will need to know the altitude at which your measurements are taken.
  // We're using a constant called ALTITUDE in this sketch:
  
  if(DEBUG_PRESSURE){
    Debug.println();
    Debug.print("provided altitude: ");
    Debug.print(ALTITUDE,0);
    Debug.print(" meters, ");
    Debug.print(ALTITUDE*3.28084,0);
    Debug.println(" feet");
  }  
  // If you want to measure altitude, and not pressure, you will instead need
  // to provide a known baseline pressure. This is shown at the end of the sketch.


  // You must first get a temperature measurement to perform a pressure reading.
  
  // Start a temperature measurement:
  // If request is successful, the number of ms to wait is returned.
  // If request is unsuccessful, 0 is returned.


  status = pressure.startTemperature();
  if (status != 0)
  {
    // Wait for the measurement to complete:
    delay(status);


    // Retrieve the completed temperature measurement:
    // Note that the measurement is stored in the variable T.
    // Function returns 1 if successful, 0 if failure.


    status = pressure.getTemperature(T);
    if (status != 0)
    {
      if(DEBUG_PRESSURE){ 
        // Print out the measurement:
        Debug.print("temperature: ");
        Debug.print(T,2);
        Debug.print(" deg C, ");
        Debug.print((9.0/5.0)*T+32.0,2);
        Debug.println(" deg F");
      }    
      // Start a pressure measurement:
      // The parameter is the oversampling setting, from 0 to 3 (highest res, longest wait).
      // If request is successful, the number of ms to wait is returned.
      // If request is unsuccessful, 0 is returned.


      status = pressure.startPressure(3);
      if (status != 0)
      {
        // Wait for the measurement to complete:
        delay(status);


        // Retrieve the completed pressure measurement:
        // Note that the measurement is stored in the variable P.
        // Note also that the function requires the previous temperature measurement (T).
        // (If temperature is stable, you can do one temperature measurement for a number of pressure measurements.)
        // Function returns 1 if successful, 0 if failure.


        status = pressure.getPressure(P,T);
        if (status != 0)
        {
          if(DEBUG_PRESSURE){
            // Print out the measurement:
            Debug.print("absolute pressure: ");
            Debug.print(P,2);
            Debug.print(" mb, ");
            Debug.print(P*0.0295333727,2);
            Debug.println(" inHg");
          }
          // The pressure sensor returns abolute pressure, which varies with altitude.
          // To remove the effects of altitude, use the sealevel function and your current altitude.
          // This number is commonly used in weather reports.
          // Parameters: P = absolute pressure in mb, ALTITUDE = current altitude in m.
          // Result: p0 = sea-level compensated pressure in mb


          p0 = pressure.sealevel(P,ALTITUDE); // we're at 1655 meters (Boulder, CO)
          if(DEBUG_PRESSURE){
            Debug.print("relative (sea-level) pressure: ");
            Debug.print(p0,2);
            Debug.print(" mb, ");
            Debug.print(p0*0.0295333727,2);
            Debug.println(" inHg");
          }
          // On the other hand, if you want to determine your altitude from the pressure reading,
          // use the altitude function along with a baseline pressure (sea-level or other).
          // Parameters: P = absolute pressure in mb, p0 = baseline pressure in mb.
          // Result: a = altitude in m.


          a = pressure.altitude(P,p0);
          if(DEBUG_PRESSURE){
            Debug.print("computed altitude: ");
            Debug.print(a,0);
            Debug.print(" meters, ");
            Debug.print(a*3.28084,0);
            Debug.println(" feet");
          }
          float pressure = p0;
          float temperature = T; 
          Souliss_ImportAnalog(memory_map, SLOT_PRESSURE, &pressure);
          Souliss_ImportAnalog(memory_map, SLOT_TEMPERATURE, &temperature);
          return p0; 
        }
        else if(DEBUG_PRESSURE) Debug.println("error retrieving pressure measurement\n");
      }
      else if(DEBUG_PRESSURE) Debug.println("error starting pressure measurement\n");
    }
    else if(DEBUG_PRESSURE) Debug.println("error retrieving temperature measurement\n");
  }
  else if(DEBUG_PRESSURE) Debug.println("error starting temperature measurement\n");
 
}
