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

#define SLOT_T16                  0 //it take four slot
#define     PIN_RBG_INPUT 5
#define     PIN_RBG_R 0
#define     PIN_RBG_G 2
#define     PIN_RBG_B 4

#define SLOT_T19                  4 //it take two slot
#define     PIN_W_INPUT 1
#define     PIN_W 3



void setup()
{
  Initialize();
  GetIPAddress();
  SetAsGateway(myvNet_dhcp);
  SetAddress(thisGW_address, myvNet_subnet, 0x0000);
  //*************************************************************************
  //*************************************************************************
  // Define inputs, outputs pins
  pinMode(PIN_RBG_INPUT, INPUT_PULLUP);          // Hardware pulldown required
  pinMode(PIN_RBG_R, OUTPUT);         // Power the LED
  pinMode(PIN_RBG_G, OUTPUT);         // Power the LED
  pinMode(PIN_RBG_B, OUTPUT);         // Power the LED
  Set_LED_Strip(SLOT_T16);

  pinMode(PIN_W_INPUT, INPUT_PULLUP);
  pinMode(PIN_W, OUTPUT);         // Power the LED
  Set_DimmableLight(SLOT_T19);

  Serial.begin(115200);
  Serial.println("Start");
  Serial.println("Going to loop");
}

void loop()
{
  EXECUTEFAST() {
    UPDATEFAST();

    FAST_510ms() {

      DigIn2State(PIN_RBG_INPUT, Souliss_T1n_OnCmd, Souliss_T1n_OffCmd, SLOT_T16);
      Logic_LED_Strip(SLOT_T16);

      short valR =  mOutput(SLOT_T16 + 1);
      short valG =  mOutput(SLOT_T16 + 2);
      short valB =  mOutput(SLOT_T16 + 3);


      Serial.print("R: ");
      Serial.print(valR);
      Serial.print(" G: ");
      Serial.print(valG);
      Serial.print(" B: ");
      Serial.println(valB);

      analogWrite(PIN_RBG_R, valR);
      analogWrite(PIN_RBG_G, valG);
      analogWrite(PIN_RBG_B, valB);


      DigIn2State(PIN_W_INPUT, Souliss_T1n_OnCmd, Souliss_T1n_OffCmd, SLOT_T19);
      Logic_DimmableLight(SLOT_T19);
      short valW = mOutput(SLOT_T19 + 1);

      Serial.print("W: ");
      Serial.println(valW);

      analogWrite(PIN_W, valW);


    }
    FAST_2110ms() {
      Timer_DimmableLight(SLOT_T16);
      Timer_LED_Strip(SLOT_T19);
    }


    FAST_GatewayComms();
  }

  EXECUTESLOW() {
    UPDATESLOW();
    SLOW_10s() {

    }
  }
}



