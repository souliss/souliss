//Changed Webinterface with the new one
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <Ticker.h>
#include <EEPROM.h>
#include <WiFiUdp.h>

//************************NODE CONFIGURATION ********************************
    
/*
    BYTE 1
    0 - NONE
    1 - DHT
    2 - LDR
    3 - DALLAS
    4 - DHT + LDR
    5 - DHT + DALLAS
    6 - LDR + DALLAS
    7 - DHT LDR DALLAS
    
    BYTE 2
    0 - NONE
    1 - PWM_MODE
    2 - PIR_MODE
    3 - RGB_MODE
    
    BYTE 3
    0 - NONE
    1 - CAPACITIVE
    2 - RELAY
    3 - BMP180
    */

   
    //Enable this to use DHT Sensor on PIN 16
    boolean DHT_SENSOR = true;
    
    //Enable this to use LDR Sensor on PIN ADC
    boolean LDR_SENSOR = true;
    
    //Enable this to use DALLAS Sensor on PIN 14
    boolean DALLAS_SENSOR = true;   
    
    // Enable this to use PIN 15 for PWM or PIR (PIR Sensor on PIN 2) 
    boolean PWM_MODE = true;           
    boolean PIR_MODE = false;
    boolean RGB_MODE = false;
    
    //Enable this to use PINS 4, 5 for CAPACITIVE BUTTONS, RELAY1 and RELAY2 or on BMP180 pressure sensor (I2C)
    boolean CAPACITIVE  = true;     
    boolean RELAY = false;          
    boolean BMP180 = false;       

/**************************************************************************
    Souliss - ESP Juan Pinto Configurable Board.
    
    This is the basic example, create a software push-button on Android
    using SoulissApp (get it from Play Store).  
    
    Load this code on ESP8266 board using the porting of the Arduino core
    for this platform.
        
***************************************************************************/
//*********************   DEFINES DEBUGS AND VARIABLES ***************************
    //DEBUG LINES
    #define LOG Serial.print
    
    #define MaCaco_DEBUG_INSKETCH
    #define MaCaco_DEBUG          1
    
    #define VNET_DEBUG_INSKETCH
    #define VNET_DEBUG  	      1
    
    #define DEBUG_PRESSURE        1
    
    #define DEBUG_CAPSENSE        0
    
    #define DEBUG_GETLUX          0

//*********************************************************************************    
    //Autocalibrate Capacitive Sensors ON
    #define AUTOCALIBRATE         1
    uint8_t thresold = 3;
    
    //EEPROM NODE CONFIGURATION VALUES
    #define EEPROM_START 307
    #define EEPROM_END 309
    uint8_t byte0;
    uint8_t byte1;
    uint8_t byte2;
    byte configuration[3];
    
    
    //SLOTS VARIABLES
    uint8_t TEMPERATURE;
    uint8_t HUMIDITY;
    uint8_t LEDPWM0;
    uint8_t LEDPWM1;
    uint8_t LEDPWM2;
    uint8_t LED;
    uint8_t LEDRGB;
    uint8_t LDR;
    uint8_t DALLAS;
    uint8_t CAP0;
    uint8_t CAP1;
    uint8_t RELAY0;
    uint8_t RELAY1;
    uint8_t PRESSURE0;
    uint8_t BMP180TEMP;
   
    #define LIGHT_ON_CYCLE 10    		// Light ON for 10 cycles if triggered by a PIR sensor
    
    //PIN VARIABLES
    byte LEDPWMP0;
    byte LEDPWMP1;
    byte LEDPWMP2;
    byte LEDP;
    byte PIRP;
    byte LEDRP;
    byte LEDGP;
    byte LEDBP;
    byte CAP0P;
    byte CAP1P;
    byte RELAY0P;
    byte RELAY1P;

// Configure the framework
#include "bconf/MCU_ESP8266.h"              // Load the code directly on the ESP8266

//#if GATEWAY == 1
    #include "conf/Gateway.h"                   // The main node is the Gateway, we have just one node
    #include "conf/DynamicAddressing.h"  
//#endif
 
    //#include "conf/DynamicAddressing.h"  //INCLUIR EN GW
//************** Include framework code and libraries *************************
    #include "conf/IPBroadcast.h"
    //#include <SPI.h>
    #include "Souliss.h"
   
   // MY FUNCTIONS LIBRARY
    #include "Functions.h"


    //NEW WEBINTERFACE INCLUDES
    #include "helpers.h"
    #include "global.h"
    //Include the HTML, STYLE and Script "Pages"
    #include "Page_Root.h"
    #include "Page_Admin.h"
    #include "Page_Script.js.h"
    #include "Page_Style.css.h"
    #include "Page_NTPsettings.h"
    #include "Page_Information.h"
    #include "Page_General.h"
    #include "PAGE_NetworkConfiguration.h"
    #include "main.h"
    
    #define ACCESS_POINT_NAME  "Souliss"				
    #define ACCESS_POINT_PASSWORD  "12345678" 
    #define AdminTimeOut 300  // Defines the Time in Seconds, when the Admin-Mode will be diabled

    // ThingSpeak API
    const char* serverTS = "api.thingspeak.com";
    bool oldInputState;
  
  
    #define DHTPIN      13//16     // what pin we're connected to
    // Include and Configure DHT11 SENSOR
    #include "DHT.h"
    #define DHTTYPE DHT11   // DHT 11 
    DHT dht(DHTPIN, DHTTYPE, 15);

    // Light calibration data
    // out[] holds the values wanted in lux/10
    #define SIZEOF 9
    static const unsigned int out[] = { 7, 30, 45, 65, 150, 300, 450, 2100, 13000};  // x10  //ULTIMO VALOR REFERENCIA
    static const unsigned int in[]  = { 100, 350, 430, 500, 680, 780, 950, 1005, 1024 };  // 0 - 1024

    #define DALLASPIN   4//14     //Se declara el pin donde se conectarÃ¡ la DATA
    #include <OneWire.h>
    #include <DallasTemperature.h>
    OneWire ourWire(DALLASPIN); //Se establece el pin declarado como bus para la comunicaciÃ³n OneWire
    DallasTemperature sensors(&ourWire); //Se instancia la librerÃ­a DallasTemperature

    //SDA 5  SCL 4  PINS
    #include <SFE_BMP180.h>
    #include <Wire.h>
    #define ALTITUDE 20.0 // Altitude of reading location in meters
    // You will need to create an SFE_BMP180 object, here called "pressure":
    SFE_BMP180 pressure;

void setup()
{
    EEPROM.begin(STORE__SIZE);
    Serial.begin(115200);
    delay(500);    

    Serial.println("Starting ES8266");
	if (!ReadConfig())
	{
		// DEFAULT CONFIG
		config.ssid = "MYSSID";
		config.password = "MYPASSWORD";
		config.dhcp = true;
		config.IP[0] = 192;config.IP[1] = 168;config.IP[2] = 1;config.IP[3] = 100;
		config.Netmask[0] = 255;config.Netmask[1] = 255;config.Netmask[2] = 255;config.Netmask[3] = 0;
		config.Gateway[0] = 192;config.Gateway[1] = 168;config.Gateway[2] = 1;config.Gateway[3] = 1;
		config.ntpServerName = "0.id.pool.ntp.org";
		config.Update_Time_Via_NTP_Every =  0;
		config.timezone = +7;
		config.daylight = true;
		config.DeviceName = "Not Named";
		config.AutoTurnOff = false;
		config.AutoTurnOn = false;
		config.TurnOffHour = 0;
		config.TurnOffMinute = 0;
		config.TurnOnHour = 0;
		config.TurnOnMinute = 0;
                config.NodeMode = true;
                config.tsAPI = "";
		WriteConfig();
		Serial.println("General config applied");
	}
	
	
	if (AdminEnabled)
	{
		Serial.println( "AP mode started" );
                WiFi.mode(WIFI_AP_STA);
		WiFi.softAP( ACCESS_POINT_NAME , ACCESS_POINT_PASSWORD);
                Serial.println(WiFi.localIP());
	}
	else
	{
		Serial.println( "STA mode started" );
                WiFi.mode(WIFI_STA);
                Serial.println(WiFi.localIP());
	}
    
    ConfigureWifi();
     
    EEPROM_CONFIG();
    SLOT_CONFIG();
    PINS_CONFIG();
/***************** INITIALIZE NETWORK AND SENSORS *********************/
    Initialize();
    GetIPAddress();
    
  
        server.on ( "/main.html", processMain);
	server.on ( "/admin/filldynamicdata", filldynamicdata );
	
	server.on ( "/favicon.ico",   []() { Serial.println("favicon.ico"); server.send ( 200, "text/html", "" );   }  );


	server.on ( "/admin.html", []() { Serial.println("admin.html"); server.send ( 200, "text/html", PAGE_AdminMainPage );   }  );
	server.on ( "/config.html", send_network_configuration_html );
	server.on ( "/info.html", []() { Serial.println("info.html"); server.send ( 200, "text/html", PAGE_Information );   }  );
	server.on ( "/ntp.html", send_NTP_configuration_html  );
	server.on ( "/general.html", send_general_html  );
//	server.on ( "/main.html", []() { server.send ( 200, "text/html", PAGE_EXAMPLE );  } );
	server.on ( "/style.css", []() { Serial.println("style.css"); server.send ( 200, "text/plain", PAGE_Style_css );  } );
	server.on ( "/microajax.js", []() { Serial.println("microajax.js"); server.send ( 200, "text/plain", PAGE_microajax_js );  } );
	server.on ( "/admin/values", send_network_configuration_values_html );
	server.on ( "/admin/connectionstate", send_connection_state_values_html );
	server.on ( "/admin/infovalues", send_information_values_html );
	server.on ( "/admin/ntpvalues", send_NTP_configuration_values_html );
	server.on ( "/admin/generalvalues", send_general_configuration_values_html);
	server.on ( "/admin/devicename",     send_devicename_value_html);
 	server.onNotFound ( []() { Serial.println("Page Not Found"); server.send ( 400, "text/html", "Page not Found" );   }  );
	server.begin();
	Serial.println( "HTTP server started" );
	tkSecond.attach(1,Second_Tick);
	UDPNTPClient.begin(2390);  // Port for NTP receive   
    
    Souliss_Node_Start();
    
    //**************************** SENSORS INITIALIZE *****************************
    if(DHT_SENSOR){
        dht.begin();
    }
    
    if(DALLAS_SENSOR){
        sensors.begin(); //Se inician los sensores DS18B20
    }
    
//**************************** SETUP SLOTS *****************************
    if(DHT_SENSOR){
        Set_Temperature(TEMPERATURE);
        Set_Humidity(HUMIDITY);
    }
    
    if(PWM_MODE || PIR_MODE){
        Set_DimmableLight(LEDPWM0);
        Set_DimmableLight(LEDPWM1);  
    }
    
    if(PWM_MODE){
        Set_DimmableLight(LEDPWM2);
    }
    
    if(PIR_MODE){
        Set_AutoLight(LED);
    }
    
    if(RGB_MODE){
        Set_LED_Strip(LEDRGB);
    }        
    
    if(RELAY){
        Set_SimpleLight(RELAY0);
        Set_SimpleLight(RELAY1);
    }
    
    if(LDR_SENSOR){
        Set_T54(LDR);
    }
    
    if(DALLAS_SENSOR){
        Set_Temperature(DALLAS);
    }        

    if(BMP180){
        Set_T51(PRESSURE0);
        Set_Temperature(BMP180TEMP);
    }
  
    if(RELAY){
        digitalWrite(RELAY0P,LOW);
        digitalWrite(RELAY1P,LOW);
        pinMode(RELAY0P, OUTPUT);                 
        pinMode(RELAY1P, OUTPUT);
    }
    
    if(PWM_MODE || PIR_MODE){
        pinMode(LEDPWMP0, OUTPUT);
        pinMode(LEDPWMP1, OUTPUT);        
    }
    
    if(PWM_MODE){
        pinMode(LEDPWMP2, OUTPUT);
    }
        
    if(PIR_MODE){
        pinMode(PIRP, INPUT);
        pinMode(LEDP, OUTPUT);
        digitalWrite(LEDP,LOW);
    }
    
    if(RGB_MODE){
            pinMode(LEDRP, OUTPUT);
            pinMode(LEDGP, OUTPUT);
            pinMode(LEDBP, OUTPUT);
    }
        
        
    if(BMP180){
        if (pressure.begin())
            LOG(F("BMP180 init success\r\n"));
        else
        {
            // Oops, something went wrong, this is usually a connection problem,
            // see the comments at the top of this sketch for the proper connections.
            LOG(F("BMP180 init fail\r\n"));
        }
    }
    
}

void loop()
{ 
     if (AdminEnabled)
	{
		if (AdminTimeOutCounter > AdminTimeOut)
		{
			 AdminEnabled = false;
			 Serial.println("Admin Mode disabled!");
			 WiFi.mode(WIFI_STA);
                         Serial.println( "STA mode started" );
                         Serial.println(WiFi.localIP());
		}
	}
	if (config.Update_Time_Via_NTP_Every  > 0 )
	{
		if (cNTP_Update > 5 && firstStart)
		{
			NTPRefresh();
			cNTP_Update =0;
			firstStart = false;
		}
		else if ( cNTP_Update > (config.Update_Time_Via_NTP_Every * 60) )
		{

			NTPRefresh();
			cNTP_Update =0;
		}
	}

	if(DateTime.minute != Minute_Old)
	{
		 Minute_Old = DateTime.minute;
		 if (config.AutoTurnOn)
		 {
			 if (DateTime.hour == config.TurnOnHour && DateTime.minute == config.TurnOnMinute)
			 {
				  Serial.println("SwitchON");
			 }
		 }


		 Minute_Old = DateTime.minute;
		 if (config.AutoTurnOff)
		 {
			 if (DateTime.hour == config.TurnOffHour && DateTime.minute == config.TurnOffMinute)
			 {
				  Serial.println("SwitchOff");
			 }
		 }
	}
	server.handleClient();
              
              

	if (Refresh)  
	{
		Refresh = false;
		 //Serial.println("Refreshing...");
		 Serial.printf("FreeMem:%d %d:%d:%d %d.%d.%d \n",ESP.getFreeHeap() , DateTime.hour,DateTime.minute, DateTime.second, DateTime.year, DateTime.month, DateTime.day);
                 Serial.print(EEPROM.read(307)); Serial.print("\t");
                 Serial.print(EEPROM.read(308)); Serial.print("\t");
                 Serial.print(EEPROM.read(309)); Serial.print("\t");
                 Serial.println(STORE__SIZE);                 
	}

    //**************************************************************************************************
    EXECUTEFAST() {                     
        UPDATEFAST();   
        
        FAST_50ms() {   // We process the logic and relevant input and output every 50 milliseconds
            if(RELAY){
                Logic_SimpleLight(RELAY0);
                DigOut(RELAY0P, Souliss_T1n_Coil,RELAY0);
            
                Logic_SimpleLight(RELAY1);
                DigOut(RELAY1P, Souliss_T1n_Coil,RELAY1);
            }

            if(PWM_MODE || PIR_MODE){
                if(CAPACITIVE){
                    CapSense(LEDPWM0,Souliss_T1n_ToggleCmd,Souliss_T1n_BrightSwitch, CAP0P, thresold, 1500);
                    CapSense(LEDPWM1,Souliss_T1n_ToggleCmd,Souliss_T1n_BrightSwitch, CAP1P, thresold, 1500);
                    if(DEBUG_CAPSENSE) Serial.println("");
                }                
                
                Logic_DimmableLight(LEDPWM0);                        
                analogWrite(LEDPWMP0, mOutput(LEDPWM0+1)*4);
                                
                Logic_DimmableLight(LEDPWM1);                        
                analogWrite(LEDPWMP1, mOutput(LEDPWM1+1)*4);
            }

            if(PIR_MODE){
                DigIn(PIRP, LIGHT_ON_CYCLE, LED);
                Logic_AutoLight(LED);
                nDigOut(LEDP, Souliss_T1n_Coil,LED);
            }
            
            if(PWM_MODE){
                Logic_DimmableLight(LEDPWM2);                        
                analogWrite(LEDPWMP2, mOutput(LEDPWM2+1)*4);
            }
            
            if(RGB_MODE){
                if(CAPACITIVE){
                    CapSense(LEDRGB,Souliss_T1n_ToggleCmd,Souliss_T1n_BrightSwitch,CAP0P, 3, 1500);
                    CapSense(LEDRGB,Souliss_T1n_ToggleCmd,Souliss_T1n_BrightSwitch,CAP1P, 3, 1500);
                }  
                Logic_LED_Strip(LEDRGB);
                analogWrite(LEDRP, mOutput(LEDRGB+1)*4);
                analogWrite(LEDGP, mOutput(LEDRGB+2)*4);
                analogWrite(LEDBP, mOutput(LEDRGB+3)*4);
            }
        }
        
        FAST_910ms()    {
            if(DALLAS){ 
                  // Acquire temperature from the microcontroller ADC
                  sensors.requestTemperatures(); //Prepara el sensor para la lectura
                  float dallas = sensors.getTempCByIndex(0);
                  Souliss_ImportAnalog(memory_map, DALLAS, &dallas);
            }      
        }
        FAST_2110ms()
        {
            if(DHT_SENSOR){
                  Logic_Temperature(TEMPERATURE);
                  Logic_Humidity(HUMIDITY);
            }
            
            if(LDR_SENSOR){
                  Logic_T54(LDR);
            }
            
            if(DALLAS_SENSOR){
                  Logic_Temperature(DALLAS);
            }                  
            
            if(BMP180){
                  Read_T51(PRESSURE0);
//              Logic_Pressure(PRESSURE0);  
                  Logic_Temperature(BMP180TEMP);
            }
        } 
        
        FAST_7110ms()
        { 
            if(LDR_SENSOR){
                float ldr_read = Souliss_GetLux(in, out, SIZEOF)/10.0;  //ORIGINAL
                if(DEBUG_GETLUX){
                    LOG("ldrlead: "); 
                    LOG(ldr_read);
                }
                if (ldr_read == 0) ldr_read = 0.01;
                Souliss_ImportAnalog(memory_map, LDR, &ldr_read);
            }
        }
        
        FAST_x10ms(300){
            //#if defined (MaCaco_DEBUG || VNET_DEBUG)
            /*  if(DHT_SENSOR){  
                LOG("Hum: "); 
                LOG(mOutputAsFloat(TEMPERATURE));
                LOG(" %\t");
                LOG("Temp: "); 
                LOG(mOutputAsFloat(HUMIDITY));
                LOG(" *C\t");
              }
              
              if(DALLAS_SENSOR){
                LOG("Dallas: ");
                LOG(mOutputAsFloat(DALLAS));
                LOG(" *C\t");             
              }
              
              if(LDR_SENSOR){
                LOG("Lux: ");
                LOG(mOutputAsFloat(LDR));
                LOG(" lux\t"); 
              }
              
              if(BMP180){
                LOG("Pressure: ");  
                LOG(mOutputAsFloat(PRESSURE0)); 
                LOG(" mb\t");  
                LOG("TempBMP180: ");  
                LOG(mOutputAsFloat(BMP180TEMP)); 
                LOG(" *C");           
              }
                LOG("\r\n");
            //#endif*/
        }
              
      
        if (config.NodeMode){
           // Here we handle here the communication with Android as Gateway
            FAST_GatewayComms();    
        }
        else { 
            // Here we handle here the communication with Android as Peer
            FAST_PeerComms(); 
        }    
    }
    EXECUTESLOW() {
	UPDATESLOW();

            SLOW_10s() {  // Read temperature and humidity from DHT every 110 seconds  
                if(RELAY){
                    Timer_SimpleLight(RELAY0);
                    Timer_SimpleLight(RELAY1);
                }
                
                if(PIR_MODE || PWM_MODE){
                    Timer_DimmableLight(LEDPWM0);              
                    Timer_DimmableLight(LEDPWM1);              
                }
                
                if(PIR_MODE){
                    Timer_AutoLight(LED);
                }
                
                if(PWM_MODE){
                    Timer_DimmableLight(LEDPWM2);                          
                }
                
                if(RGB_MODE){
                    Timer_LED_Strip(LEDRGB);
                }
            }            
            SLOW_x10s(2) {

                if(DHT_SENSOR){
                    Souliss_GetDHT(TEMPERATURE, HUMIDITY, true);  
                }
                
                if(BMP180){
                    Souliss_GetPressure_BMP180(PRESSURE0,BMP180TEMP);	
                }    
                
            } //SLOW_x10s(2) 
           
                SLOW_PeerJoin();
            
      } 
      
        START_PeerJoin(); 
          
}    
   	
   
/***************************************************************************/
// Calculate lux based on rawADC reading from LDR returns value in lux/10
/***************************************************************************/
int Souliss_GetLux(const unsigned int* _in, const unsigned int* _out, byte size)
{
	//boolean DEBUG_GETLUX = 1;
        // take care the value is within range
	// val = constrain(val, _in[0], _in[size-1]);
        
	int val = analogRead(A0);

        //DEBUG
        if(DEBUG_GETLUX){
          LOG("AnalogRead: ");
          LOG(val);
          LOG("\r\n");
        }
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

/***************************************************************************/
/*                            DHT READING FUNCTION                         */
/***************************************************************************/
void Souliss_GetDHT(uint8_t SLOT_TEMPERATURE, uint8_t SLOT_HUMIDITY, boolean Celsius){
        float h = dht.readHumidity();
        // Read temperature as Celsius
        float t = dht.readTemperature();
        // Read temperature as Fahrenheit
        float f = dht.readTemperature(true);
              
        // Check if any reads failed and exit early (to try again).
        if (isnan(h) || isnan(t) || isnan(f)) {
            LOG("Failed to read from DHT sensor!\r\n");
        }
        // Compute heat index
        // Must send in temp in Fahrenheit!
        /*float hi = dht.computeHeatIndex(f, h);
        float hic = (f - 32) / 1.8;
        Souliss_ImportAnalog(memory_map, HEAT, &hic);*/
  	    
        if(Celsius) Souliss_ImportAnalog(memory_map, SLOT_TEMPERATURE, &t);
        else        Souliss_ImportAnalog(memory_map, SLOT_TEMPERATURE, &f);
        
  	    Souliss_ImportAnalog(memory_map, SLOT_HUMIDITY, &h);	
          
}

/***************************************************************************/
/*                         BMP180 I2C READING FUNCTION                     */
/***************************************************************************/
float Souliss_GetPressure_BMP180(uint8_t SLOT_PRESSURE, uint8_t SLOT_TEMPERATURE){
  
  char status;
  double T,P,p0,a;

  // Loop here getting pressure readings every 10 seconds.

  // If you want sea-level-compensated pressure, as used in weather reports,
  // you will need to know the altitude at which your measurements are taken.
  // We're using a constant called ALTITUDE in this sketch:
  
  if(DEBUG_PRESSURE){
    LOG(F("provided altitude: "));
    LOG(ALTITUDE,0);
    LOG(F(" meters, "));
    LOG(ALTITUDE*3.28084,0);
    LOG(F(" feet\r\n"));
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
        LOG(F("temperature: "));
        LOG(T,2);
        LOG(F(" deg C, "));
        LOG((9.0/5.0)*T+32.0,2);
        LOG(F(" deg F\r\n"));
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
            LOG(F("absolute pressure: "));
            LOG(P,2);
            LOG(F(" mb, "));
            LOG(P*0.0295333727,2);
            LOG(F(" inHg\r\n"));
          }
          // The pressure sensor returns abolute pressure, which varies with altitude.
          // To remove the effects of altitude, use the sealevel function and your current altitude.
          // This number is commonly used in weather reports.
          // Parameters: P = absolute pressure in mb, ALTITUDE = current altitude in m.
          // Result: p0 = sea-level compensated pressure in mb

          p0 = pressure.sealevel(P,ALTITUDE); // we're at 1655 meters (Boulder, CO)
          if(DEBUG_PRESSURE){
            LOG(F("relative (sea-level) pressure: "));
            LOG(p0,2);
            LOG(F(" mb, "));
            LOG(p0*0.0295333727,2);
            LOG(F(" inHg\r\n"));
          }
          // On the other hand, if you want to determine your altitude from the pressure reading,
          // use the altitude function along with a baseline pressure (sea-level or other).
          // Parameters: P = absolute pressure in mb, p0 = baseline pressure in mb.
          // Result: a = altitude in m.

          a = pressure.altitude(P,p0);
          if(DEBUG_PRESSURE){
            LOG(F("computed altitude: "));
            LOG(a,0);
            LOG(F(" meters, "));
            LOG(a*3.28084,0);
            LOG(F(" feet\r\n"));
          }
          float pressure = p0;
          float temperature = T; 
          Souliss_ImportAnalog(memory_map, SLOT_PRESSURE, &pressure);
          Souliss_ImportAnalog(memory_map, SLOT_TEMPERATURE, &temperature);
          return p0; 
        }
        else if(DEBUG_PRESSURE) LOG(F("error retrieving pressure measurement\n"));
      }
      else if(DEBUG_PRESSURE) LOG(F("error starting pressure measurement\n"));
    }
    else if(DEBUG_PRESSURE) LOG(F("error retrieving temperature measurement\n"));
  }
  else if(DEBUG_PRESSURE) LOG(F("error starting temperature measurement\n"));
 
}

// send data to ThingSpeak.com
void sendInputState(bool inputState){
 Serial.println(inputState); 
 Serial.println(config.tsAPI);

 WiFiClient client;
 if(!client.connect(serverTS,80)) {  //   "184.106.153.149" or api.thingspeak.com
    Serial.println("Connection Fail!");
 }
 else {
    String postStr = config.tsAPI;
           postStr +="&field1=";
           postStr += String(inputState);
           postStr += "\r\n";
 
     client.print("POST /update HTTP/1.1\n");
     client.print("Host: api.thingspeak.com\n");
     client.print("Connection: close\n");
     client.print("X-THINGSPEAKAPIKEY: "+config.tsAPI+"\n");
     client.print("Content-Type: application/x-www-form-urlencoded\n");
     client.print("Content-Length: ");
     client.print(postStr.length());
     client.print("\n\n");
     client.print(postStr);
     Serial.println("Send Relay State to Thingspeak");
 
 }
  client.stop();
  Serial.println("Client to send Stoped");
}


