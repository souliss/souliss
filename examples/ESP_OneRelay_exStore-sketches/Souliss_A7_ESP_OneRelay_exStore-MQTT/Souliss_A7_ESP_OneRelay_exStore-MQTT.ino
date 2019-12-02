#define HOST_NAME_INSKETCH
#define HOST_NAME "ESP8266-WiFi-Relay-V3-AdafruitIO"

/**************************************************************************
Sketch: ESP8266 WiFi Relay V3 - Souliss - Web Configuration - AdafruitIO
Author: Tonino Fazio

This example is only supported on ESP8266.

 //Used pins
// pin 12: onboad relay ON
// pin 13: onboad relay OFF
// pin 14: switch
// pin 16: DHT22 - Terperature sensor
***************************************************************************/
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <EEPROM.h>
#include <WiFiUdp.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

// Configure the Souliss framework
#include "bconf/MCU_ESP8266.h"              // Load the code directly on the ESP8266
#include "conf/RuntimeGateway.h"            // This node is a Peer and can became a Gateway at runtime
#include "conf/DynamicAddressing.h"         // Use dynamically assigned addresses
#include "conf/WEBCONFinterface.h"          // Enable the WebConfig interface

#include "Souliss.h"
#include "DHT.h"
//*************************************************************************
//*************************************************************************

/************************* Adafruit.io Setup *********************************/
#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883
#include "user.h"

/************ Global State (you don't need to change this!) ******************/

// Create an ESP8266 WiFiClient class to connect to the MQTT server.
WiFiClient client;

// Store the MQTT server, client ID, username, and password in flash memory.
// This is required for using the Adafruit MQTT library.
const char MQTT_SERVER[]  = AIO_SERVER;
// Set a unique MQTT client ID using the AIO key + the date and time the sketch
// was compiled (so this should be unique across multiple devices for a user,
// alternatively you can manually set this to a GUID or other random value).
const char MQTT_CLIENTID[]  = __TIME__ AIO_USERNAME;
const char MQTT_USERNAME[]  = AIO_USERNAME;
const char MQTT_PASSWORD[]  = AIO_KEY;

// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client mqtt(&client, MQTT_SERVER, AIO_SERVERPORT, MQTT_CLIENTID, MQTT_USERNAME, MQTT_PASSWORD);

/****************************** Feeds ***************************************/
const char TEMPERATURE_FEED[] = AIO_USERNAME "/feeds/temperature";
Adafruit_MQTT_Publish MQTTtemperature = Adafruit_MQTT_Publish(&mqtt, TEMPERATURE_FEED);
const char ONOFF_FEED[] = AIO_USERNAME "/feeds/relay0";
Adafruit_MQTT_Publish MQTTrelay0 = Adafruit_MQTT_Publish(&mqtt, ONOFF_FEED);
/****************************** Subscribe ***************************************/
Adafruit_MQTT_Subscribe MQTTrelay0_Read = Adafruit_MQTT_Subscribe(&mqtt, ONOFF_FEED);

U8 lastVal;

#define SLOT_RELAY_0 0
#define SLOT_TEMPERATURE        1     // This is the memory slot used for the execution of the logic in network_address1
#define SLOT_HUMIDITY        3     // This is the memory slot used for the execution of the logic

#define PIN_LED 2
#define PIN_SWITCH  14
#define PIN_RELAY_ON  12
#define PIN_RELAY_OFF  13
#define PIN_DHT 16
#define PIN_RESET 0

#define DHTTYPE DHT22   // DHT 22 
#define DEADBAND        0.01    // Deadband value 1% 
// Initialize DHT sensor for normal 8mhz Arduino
DHT dht(PIN_DHT, DHTTYPE, 2);


uint8_t MAC_array[6];
char MAC_char[18];

// Setup the libraries for Over The Air Update
OTA_Setup();
void setup()
{
  Serial.begin(115200);
  Serial.println(F("begin OK"));
  //delay 15 seconds
  delay(15000);
  Initialize();
  check_for_reset_now();
  
  Serial.println(F("init  OK"));
  // Read the IP configuration from the EEPROM, if not available start
  // the node as access point
  if (!ReadIPConfiguration())
  {
    // Start the node as access point with a configuration WebServer
    SetAccessPoint();
    startWebServer();
    while (1)
    {
      yield();
      runWebServer();
    }
  }

  if (IsRuntimeGateway())
  {
    // Connect to the WiFi network and get an address from DHCP
    SetAsGateway(myvNet_dhcp);       // Set this node as gateway for SoulissApp
    SetAddressingServer();
    Serial.println(F("SetAsGateway OK"));
  }
  else
  {
    // This board request an address to the gateway at runtime, no need
    // to configure any parameter here.
    SetDynamicAddressing();
    GetAddress();
    Serial.println(F("SetAsPeer OK"));
  }

  //*************************************************************************
  //*************************************************************************
  Set_SimpleLight(SLOT_RELAY_0);
  mOutput(SLOT_RELAY_0) = Souliss_T1n_OnCoil; //Set output to ON, then first execution of DigIn2State cause a change state to OFF.

  Set_Temperature(SLOT_TEMPERATURE);
  Set_Humidity(SLOT_HUMIDITY);
  pinMode(PIN_DHT, INPUT);
  dht.begin();

  pinMode(PIN_SWITCH, INPUT_PULLUP);    // SWITCH
  digitalWrite(PIN_RELAY_ON, LOW);
  pinMode(PIN_RELAY_ON, OUTPUT);    // Relay ON
  digitalWrite(PIN_RELAY_OFF, LOW);
  pinMode(PIN_RELAY_OFF, OUTPUT);    // Relay OFF
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_RESET, INPUT); //10 seconds to gnd for eeprom reset

  // Init the OTA
  OTA_Init();


  // Setup MQTT subscription for onoff feed.
  mqtt.subscribe(&MQTTrelay0_Read);
}

void loop()
{
  // Look for a new sketch to update over the air
  OTA_Process();

  EXECUTEFAST() {
    UPDATEFAST();
    FAST_210ms() {
      check_if_joined();
    }
    FAST_2110ms() {
      check_if_reset();
      activity();
    }
    FAST_9110ms() {
      // Ensure the connection to the MQTT server is alive (this will make the first
      // connection and automatically reconnect when disconnected).  See the MQTT_connect
      // function definition further below.
      MQTT_connect();
    }

    FAST_1110ms() {
      // this is our 'wait for incoming subscription packets' busy subloop
      Adafruit_MQTT_Subscribe *subscription;
      while ((subscription = mqtt.readSubscription(1000))) {
        if (subscription == &MQTTrelay0_Read) {
          Serial.print(F("Got: "));
          Serial.println((char *)MQTTrelay0_Read.lastread);
          Serial.print("MQTTrelay0_Read.lastread: "); Serial.println((char*) MQTTrelay0_Read.lastread);

          if (!strcmp( (char *)MQTTrelay0_Read.lastread, "ON")) {
            Serial.println("Set Souliss Relay ON");
            mInput(SLOT_RELAY_0) = Souliss_T1n_OnCmd;
          } else if (!strcmp( (char *)MQTTrelay0_Read.lastread, "OFF")) {
            Serial.println("Set Souliss Relay OFF");
            mInput(SLOT_RELAY_0) = Souliss_T1n_OffCmd;
          }
        }
      }
    }

    FAST_50ms() {
      DigIn2State(PIN_SWITCH, Souliss_T1n_ToggleCmd, Souliss_T1n_ToggleCmd, SLOT_RELAY_0);
      Logic_SimpleLight_MQTT(MQTTrelay0, SLOT_RELAY_0, &data_changed);
      Logic_SimpleLight(SLOT_RELAY_0);
      PulseLowDigOut(PIN_RELAY_ON, Souliss_T1n_OnCoil, SLOT_RELAY_0);
      PulseLowDigOut(PIN_RELAY_OFF, Souliss_T1n_OffCoil, SLOT_RELAY_0);
    }

    FAST_510ms() {
      Logic_Temperature(SLOT_TEMPERATURE);
      Logic_Humidity(SLOT_HUMIDITY);
    }

    // Run communication as Gateway or Peer
    if (IsRuntimeGateway())
      FAST_GatewayComms();
    else
      FAST_PeerComms();
  }

  EXECUTESLOW() {
    UPDATESLOW();
    SLOW_10s() {  // Process the timer every 10 seconds
      Timer_SimpleLight(SLOT_RELAY_0);
      // Read temperature value from DHT sensor and convert from single-precision to half-precision
      float temperature = dht.readTemperature();
      ImportAnalog(SLOT_TEMPERATURE, &temperature);

      // Read humidity value from DHT sensor and convert from single-precision to half-precision
      float humidity = dht.readHumidity();
      ImportAnalog(SLOT_HUMIDITY, &humidity);

      // MQTT Publish
      Serial.print(F("\nSending temperature val "));
      Serial.print(temperature);
      Serial.print("...");
      if (! MQTTtemperature.publish(temperature)) {
        Serial.println(F("Failed"));
      } else {
        Serial.println(F("OK!"));
      }
    }
    // If running as Peer
    if (!IsRuntimeGateway())
      SLOW_PeerJoin();
  }

}

// Function to connect and reconnect as necessary to the MQTT server.
// Should be called in the loop function and it will take care if connecting.
void MQTT_connect() {
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }
  Serial.print("Connecting to MQTT... ");

  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
    Serial.println(mqtt.connectErrorString(ret));
    Serial.println("Retrying MQTT connection...");
    mqtt.disconnect();
  }
  Serial.println("MQTT Connected!");
}

void Logic_SimpleLight_MQTT(Adafruit_MQTT_Publish MQTTrelayX, U8 slot, U8 *trigger) {

  if (mOutput(slot) != lastVal) {
    Serial.print(F("\nSending relay state. "));
    if (mOutput(slot) == Souliss_T1n_OnCoil) {
      if (! MQTTrelayX.publish("ON")) {
        Serial.println(F("Failed to write ON"));
      } else {
        Serial.println(F("Write ON. OK!"));
        lastVal = Souliss_T1n_OnCoil;
      }
    } else if (mOutput(slot) == Souliss_T1n_OffCoil) {
      if (!MQTTrelayX.publish("OFF")) {
        Serial.println(F("Failed to write OFF"));
      } else {
        Serial.println(F("Write OFF. OK!"));
        lastVal = Souliss_T1n_OffCoil;
      }
    }
  }
}
U8 led_status = 0;
U8 joined = 0;
void check_if_joined() {
  if (JoinInProgress() && joined == 0) {
    joined = 0;
    if (led_status == 0) {
      digitalWrite(PIN_LED, HIGH);
      led_status = 1;
    } else {
      digitalWrite(PIN_LED, LOW);
      led_status = 0;
    }
  } else {
    joined = 1;
    digitalWrite(PIN_LED, HIGH);
  }
}

unsigned long time_start = 0;
unsigned long time_for_reset = 10000;
void check_if_reset() {
  if (!digitalRead(PIN_RESET)) {
    if (abs(millis() - time_start) > time_for_reset)
    {
      Serial.println(F("Reset"));
      Store_Init();
      Store_Clear();
      Store_Commit();
      Serial.println(F("OK"));
      time_start = millis();
    }
  } else {
    time_start = millis();
  }
}

void check_for_reset_now() {
  if (!digitalRead(PIN_RESET)) {
      Serial.println("");
      Serial.println(F("Reset"));
      Store_Init();
      Store_Clear();
      Store_Commit();
      Serial.println(F("OK"));
    }
}

U8 activity_led_status = 0;
void activity() {
  if (activity_led_status == 0) {
    digitalWrite(PIN_LED, HIGH);
    activity_led_status = 1;
  } else {
    digitalWrite(PIN_LED, LOW);
    activity_led_status = 0;
  }
}

