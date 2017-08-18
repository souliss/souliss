/**************************************************************************
    Souliss - Typicals examples on Expressif ESP8266


***************************************************************************/


#include "SoulissFramework.h"
#include <ESP8266mDNS.h>
#include "bconf/MCU_ESP8266.h"              // Load the code directly on the ESP8266
#include "conf/Gateway.h"
#include "conf/IPBroadcast.h"

// **** Define the WiFi name and password ****
#define WIFICONF_INSKETCH
#define WiFi_SSID               "yourSSID"
#define WiFi_Password           "yourPASSWORD"


// Include framework code and libraries
#include <ESP8266WiFi.h>
#include "Souliss.h"

//*************************************************************************
#define thisGW_address  0xAB20
#define myvNet_subnet 0xFF00
//*************************************************************************

#define SLOT_T1A                  0

short val_State_T1A;
short short_Input_val;

void setup()
{
  Serial.begin(115200);
  Serial.println("Start");

  Initialize();
  Serial.println("GetIPAddress");
  GetIPAddress();
  SetAsGateway(myvNet_dhcp);
  SetAddress(thisGW_address, myvNet_subnet, 0x0000);

  Serial.println("Set Typicals T1A");
  Souliss_SetT1A(memory_map, SLOT_T1A); //Typical 1A : Digital pass through

  Serial.println("Going to loop");
}

void loop()
{
  EXECUTEFAST() {
    UPDATEFAST();

    FAST_510ms() {

      memory_map[MaCaco_IN_s + SLOT_T1A] = short_Input_val++;

      Souliss_Logic_T1A(memory_map, SLOT_T1A, &data_changed);

      val_State_T1A =  mOutput(SLOT_T1A);

      Serial.print("State T1A: ");
      Serial.println(val_State_T1A);

//print state of bits
      Serial.print("Bits: ");
      for (int i=0;i<=7;i++){
      Serial.print(getBit(i, val_State_T1A));  
      Serial.print(" ");
      }
      Serial.println("");
    }

    FAST_GatewayComms();
  }
}
short getBit(int n, short shortVal) {
  short short_PositionVal;
  switch (n) {
    case 0:
    short_PositionVal=1;
      break;
    case 1:
    short_PositionVal=2;
      break;
    case 2:
    short_PositionVal=4;
      break;
    case 3:
    short_PositionVal=8;
      break;
    case 4:
    short_PositionVal=16;
      break;
    case 5:
    short_PositionVal=32;
      break;
    case 6:
    short_PositionVal=64;
      break;
    case 7:
    short_PositionVal=128;
      break;
  }
  if ((shortVal & short_PositionVal) > 0){
    return 1;
  }
  else return 0;
}




