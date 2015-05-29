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

// Light calibration data
// out[] holds the values wanted in lux/10
#define sizeofarray 9   // Number of items on out and in arrays
static const unsigned int out[] = { 7, 30, 45, 65, 150, 300, 450, 2100, 13000};  // x10  //ULTIMO VALOR REFERENCIA
static const unsigned int in[]  = { 100, 350, 430, 500, 680, 780, 950, 1005, 1024 };  // 0 - 1024

#define Debug Serial        //Change to Serial1 if you want to use the GPIO2 to TX 

void setup()
{   
    Initialize();
    
    Debug.begin(115200);

    // Connect to the WiFi network and get an address from DHCP
    GetIPAddress();      
    SetAsGateway(myvNet_dhcp); 

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
              float ldr_read = get_lux(in, out, sizeofarray)/100.0;  //ORIGINAL
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
 Debug.print("AnalogRead: ");
 Debug.println(val);
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
