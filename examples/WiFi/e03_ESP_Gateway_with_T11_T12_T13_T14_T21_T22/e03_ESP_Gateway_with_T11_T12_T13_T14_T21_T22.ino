/**************************************************************************
    Souliss - Typicals examples on Expressif ESP8266

      T11: ON/OFF Digital Output with Timer Option
      T12: ON/OFF Digital Output with AUTO mode
      T13: Digital Input Value
      T14: Pulse Digital Output
      T21: Motorized devices with limit switches
      T22: Motorized devices with limit switches and middle position
***************************************************************************/

#include "SoulissFramework.h"
#include <ESP8266mDNS.h>
#include "bconf/MCU_ESP8266.h"              // Load the code directly on the ESP8266
#include "conf/Gateway.h"
#include "conf/IPBroadcast.h"

// **** Define the WiFi name and password ****
#define WIFICONF_INSKETCH
#define WiFi_SSID               "SSID"
#define WiFi_Password           "WiFipassword"


// Include framework code and libraries
#include <ESP8266WiFi.h>
#include "Souliss.h"

//*************************************************************************
#define thisGW_address  0xAB20
#define myvNet_subnet 0xFF00
//*************************************************************************

#define LIGHT_ON_CYCLE 2
#define slotT11 0
#define slotT12 1
#define slotT13 2
#define slotT14 3
#define slotT21 4
#define slotT22 6


#define INPUTPIN_LIMIT_OPENING_T21 3
#define INPUTPIN_LIMIT_CLOSING_T21 1
#define INPUTPIN_LIMIT_OPENING_T22 0
#define INPUTPIN_LIMIT_CLOSING_T22 2
#define pinT12 4
#define pinT13 5


void setup()
{
  Serial.begin(9600);
  Serial.println("Start");
  Initialize();
  GetIPAddress();
  SetAsGateway(myvNet_dhcp);
  SetAddress(thisGW_address, myvNet_subnet, 0x0000);

  //*************************************************************************
  //*************************************************************************
  Set_SimpleLight(slotT11);
  Set_AutoLight(slotT12);
  Set_DigitalInput(slotT13);
  Set_PulseOutput(slotT14);
  Set_GarageDoor (slotT21);
  Set_Windows (slotT22);
  pinMode(pinT12, INPUT_PULLUP);
  pinMode(pinT13, INPUT_PULLUP);
  // Define inputs, outputs pins
  pinMode(INPUTPIN_LIMIT_OPENING_T21, INPUT_PULLUP);                  // Hardware pulldown required
  pinMode(INPUTPIN_LIMIT_CLOSING_T21, INPUT_PULLUP);
  pinMode(INPUTPIN_LIMIT_OPENING_T22, INPUT_PULLUP);                  // Hardware pulldown required
  pinMode(INPUTPIN_LIMIT_CLOSING_T22, INPUT_PULLUP);
}

void loop()
{
  EXECUTEFAST() {
    UPDATEFAST();

    FAST_510ms() {
      DigIn2State(pinT13, Souliss_T1n_OnCmd, Souliss_T1n_OffCmd,  slotT13);
      DigIn(pinT12,  Souliss_T1n_AutoCmd + LIGHT_ON_CYCLE, slotT12);
      LowDigIn(INPUTPIN_LIMIT_OPENING_T21, Souliss_T2n_LimSwitch_Open, slotT21);
      LowDigIn(INPUTPIN_LIMIT_CLOSING_T21, Souliss_T2n_LimSwitch_Close, slotT21);
      LowDigIn(INPUTPIN_LIMIT_OPENING_T22, Souliss_T2n_LimSwitch_Open, slotT22);
      LowDigIn(INPUTPIN_LIMIT_CLOSING_T22, Souliss_T2n_LimSwitch_Close, slotT22);

      Logic_SimpleLight(slotT11); //input PIN not connected
      Logic_AutoLight(slotT12); //input PIN -> pinT12
      Logic_DigitalInput(slotT13); //input PIN -> pinT13
      Logic_PulseOutput(slotT14); //input PIN not connected
      Logic_GarageDoor(slotT21); //input PIN -> INPUTPIN_LIMIT_OPENING_T21 and INPUTPIN_LIMIT_CLOSING_T21
      Logic_Windows(slotT22); //input PIN -> INPUTPIN_LIMIT_OPENING_T22 and INPUTPIN_LIMIT_CLOSING_T22

    }

    FAST_1110ms() {
      Serial.println(digitalRead(pinT13));
      Timer_AutoLight(slotT12);
      Timer_GarageDoor(slotT21);
      Timer_Windows(slotT22);
    }

    FAST_GatewayComms();
  }

  EXECUTESLOW() {
    UPDATESLOW();
    SLOW_10s() {
    }
  }
}



