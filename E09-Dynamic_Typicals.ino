/*PLEASE TAKE CARE THIS SKETCH IT'S UNDER DEVELOP 23/07/2015*/

//MIRAR!!: http://stackoverflow.com/questions/28177544/defining-variables-within-if-else-endif-in-arduino-ide
#if 1
__asm volatile ("nop");
#endif

//AÑADIR CAPACITIVOS - HECHO
//Crear COLOR CYCLE para CAPACITIVO CON RGB

//AÑADIR WIFI MANAGER - HECHO

/*  Download WifiManager from https://github.com/tzapu/WiFiManager  */
#define DEBUG 
#include <WiFiClient.h>
#include <EEPROM.h>
#include <ESP8266mDNS.h>
#include <WiFiManager.h>

WiFiManager wifi(0);


//WEB CONFIGURACION --> GUARDAR EEPROM
//http://www.echoecho.com/htmlforms10.htm
//http://www.forosdelweb.com/f4/combobox-con-html-839038/

//LEER EEPROM - PROBAR
//DYNAMIC ADDRESSING - HECHO

//************************NODE CONFIGURATION ********************************
    
    //Enable this to use this node as GW or PEER
    #define GATEWAY    1
    #define PEER       0
    
    //NETWORK ADDRESSING
    #define STATIC     1
    #define DYNAMIC    0
  
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

    #define byte0    5
    #define byte1    1  
    #define byte2    3
    
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
    #define EEPROM_START 65
    #define EEPROM_END 67
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

#if GATEWAY == 1
    #include "conf/Gateway.h"                   // The main node is the Gateway, we have just one node
    #include "conf/DynamicAddressing.h"  
#endif
 
    //#include "conf/DynamicAddressing.h"  //INCLUIR EN GW
//************** Include framework code and libraries *************************
    #include "conf/IPBroadcast.h"
    //#include <SPI.h>
    #include <ESP8266WiFi.h>
    //#include <EEPROM.h>
    #include "Souliss.h"

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

    #define DALLASPIN   4//14     //Se declara el pin donde se conectará la DATA
    #include <OneWire.h>
    #include <DallasTemperature.h>
    OneWire ourWire(DALLASPIN); //Se establece el pin declarado como bus para la comunicación OneWire
    DallasTemperature sensors(&ourWire); //Se instancia la librería DallasTemperature

    //SDA 5  SCL 4  PINS
    #include <SFE_BMP180.h>
    #include <Wire.h>
    #define ALTITUDE 20.0 // Altitude of reading location in meters
    // You will need to create an SFE_BMP180 object, here called "pressure":
    SFE_BMP180 pressure;

byte nonsense_var = 0;  //PROBAR A COMENTAR

#if STATIC == 1
    // Define the network configuration according 
    // to your router settings
    uint8_t ip_address[4]  = {192, 168, 1, 55};
    uint8_t subnet_mask[4] = {255, 255, 255, 0};
    uint8_t ip_gateway[4]  = {192, 168, 1, 1};
    #define Gateway_address 200
    #define myvNet_address  ip_address[3]  
    #define myvNet_subnet   0xFF00
    #define myvNet_supern   Gateway_address
#endif

void setup()
{
    Serial.begin(115200);
        
    wifi.autoConnect("Souliss");
        
    if(EEPROM_CONFIG()) WiFi.mode(WIFI_STA);
    
    SLOT_CONFIG();
    PINS_CONFIG();
/***************** INITIALIZE NETWORK AND SENSORS *********************/
    Initialize();
    
    if(GATEWAY && DYNAMIC) {
        GetIPAddress();                           
        SetAsGateway(myvNet_dhcp);       // Set this node as gateway for SoulissApp  
        SetAddress(0xAB01, 0xFF00, 0x0000);
    }
    
    if(GATEWAY && STATIC){
        Souliss_SetIPAddress(ip_address, subnet_mask, ip_gateway);
        SetAsGateway(myvNet_address); 
        SetAddress(0xAB01, 0xFF00, 0x0000);
    }
    
    if(PEER && DYNAMIC){
        GetIPAddress();
        SetAddress(0xAB02, 0xFF00, 0x0000); //Antitheft & Temp/Hum esp12
        //SetDynamicAddressing(); 
        //GetAddress();    
    }
    
    if(PEER && STATIC){
        Souliss_SetIPAddress(ip_address, subnet_mask, ip_gateway);
        SetAddress(0x00CB, 0xFF00, 0x00C8); //Antitheft & Temp/Hum esp12
    }
    


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
        
    //#if defined (MaCaco_DEBUG || VNET_DEBUG)
        LOG("Init");
    //#endif
    
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
                    CapSense(LEDPWM0,Souliss_T1n_ToggleCmd,Souliss_T1n_BrightToggle, CAP0P, thresold, 1500);
                    CapSense(LEDPWM1,Souliss_T1n_ToggleCmd,Souliss_T1n_BrightToggle, CAP1P, thresold, 1500);
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
                    CapSense(LEDRGB,Souliss_T1n_ToggleCmd,Souliss_T1n_BrightToggle,CAP0P, 3, 1500);
                    CapSense(LEDRGB,Souliss_T1n_ToggleCmd,Souliss_T1n_BrightToggle,CAP1P, 3, 1500);
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
              
        // Here we handle here the communication with Android
       #if GATEWAY == 1
            FAST_GatewayComms();
       #endif
        
       #if PEER == 1
            FAST_PeerComms();        
       #endif
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
            #if PEER == 1
                SLOW_PeerJoin();
            #endif    
      } 
      #if PEER == 1   
        START_PeerJoin(); 
      #endif        
}    
   	

bool EEPROM_CONFIG(){
    
    //EEPROM CONFIGURATION READ.
    LOG("EEPROM DATA: ");
    
    bool DATA_OK = true;
    //FOR TESTING PURPORSES
    if(EEPROM.read(EEPROM_START+0) == byte0){
      
    }else{ 
        EEPROM.write(EEPROM_START+0, byte0); 
        Serial.println("BYTE0_WRITED"); 
        DATA_OK = false; 
    }
    if(EEPROM.read(EEPROM_START+1) == byte1){
      
    }else{ 
        EEPROM.write(EEPROM_START+1, byte1); 
        Serial.println("BYTE1_WRITED"); 
        DATA_OK = false; 
    }
    if(EEPROM.read(EEPROM_START+2) == byte2){
      
    }else{ 
        EEPROM.write(EEPROM_START+2, byte2); 
        Serial.println("BYTE2_WRITED"); 
        DATA_OK = false; 
    }
    
    if (!DATA_OK) EEPROM.commit();      
    
    for (int i = EEPROM_START; i<=EEPROM_END; i++) {
      configuration[i] = EEPROM.read(i);
      if(configuration[i] > 7)
          return 0;
      LOG(configuration[i]);
      LOG("\t");
    }
    LOG("\r\n");
    
    // DHT LDR DALLAS OPTIONS:
    switch (configuration[EEPROM_START]) {
        case 0:
            DHT_SENSOR = false;
            LDR_SENSOR = false;
            DALLAS_SENSOR = false;  
            break;
        case 1:
            DHT_SENSOR = true;
            LDR_SENSOR = false;
            DALLAS_SENSOR = false;  
            break;
        case 2:
            DHT_SENSOR = false;
            LDR_SENSOR = true;
            DALLAS_SENSOR = false;  
            break;
        case 3:
            DHT_SENSOR = false;
            LDR_SENSOR = false;
            DALLAS_SENSOR = true;  
            break;
        case 4:
            DHT_SENSOR = true;
            LDR_SENSOR = true;
            DALLAS_SENSOR = false;  
            break;
        case 5:
            DHT_SENSOR = true;
            LDR_SENSOR = false;
            DALLAS_SENSOR = true;  
            break;
        case 6:
            DHT_SENSOR = false;
            LDR_SENSOR = true;
            DALLAS_SENSOR = true;  
            break;
        case 7:
            DHT_SENSOR = true;
            LDR_SENSOR = true;
            DALLAS_SENSOR = true;  
            break;            
    }
    
    LOG(DHT_SENSOR);
    LOG(LDR_SENSOR);
    LOG(DALLAS_SENSOR);
    LOG(" DLD (DHT-LDR-DALLAS)");
    LOG("\r\n");
    
    // PWM PIR RGB OPTIONS:
    switch (configuration[EEPROM_START+1]) {
        case 0:
            PWM_MODE = false;
            PIR_MODE = false;
            RGB_MODE = false;
            break;
        case 1:
            PWM_MODE = true;
            PIR_MODE = false;
            RGB_MODE = false;
            break;
        case 2:
            PWM_MODE = false;
            PIR_MODE = true;
            RGB_MODE = false;
            break;
        case 3:
            PWM_MODE = false;
            PIR_MODE = false;
            RGB_MODE = true;
            break;
    }
    LOG("PPR (PWM-PIR-RGB)");
    LOG(PWM_MODE);
    LOG(PIR_MODE);
    LOG(RGB_MODE);
    LOG(" PPR (PWM-PIR-RGB)");
    LOG("\r\n");
    
    // CAPACITIVE RELAY BMP180 OPTIONS
    switch (configuration[EEPROM_START+2]) {
        case 0:
            CAPACITIVE = false;
            RELAY = false;
            BMP180 = false;
            break;
        case 1:
            CAPACITIVE = true;
            RELAY = false;
            BMP180 = false;
            break;
        case 2:
            CAPACITIVE = false;
            RELAY = true;
            BMP180 = false;
            break;
        case 3:
            CAPACITIVE = false;
            RELAY = false;
            BMP180 = true;
            break;
    }
    LOG("CRB (CAP-RELAY-BMP180)");
    LOG(CAPACITIVE);
    LOG(RELAY);
    LOG(BMP180);
    LOG(" CRB (CAP-RELAY-BMP180)");
    LOG("\r\n");
    
    return 1;

}

//************************* PINS CONFIGURATION FUNCTION ************************
void PINS_CONFIG(){
  //if(DHT_SENSOR)
      //DHTPIN = 16;     // what pin we're connected to
    
  if(PWM_MODE){
      LEDPWMP0 = 5;//12;      //LED STRIP ON PIN 12
      LEDPWMP1 = 16;//13;      //LED STRIP ON PIN 13
      LEDPWMP2 = 15;      //LED STRIP ON PIN 15
  }
  
  if(PIR_MODE){
      LEDPWMP0 = 5;//12;      //LED STRIP ON PIN 12
      LEDPWMP1 = 16;//13;      //LED STRIP ON PIN 13
      LEDP = 15;      //LED STRIP ON PIN 15
      PIRP = 2;       //LED STRIP ON PIN 2
  }
  
  if(RGB_MODE){
      LEDRP = 5;//12;      //LED STRIP ON PIN 12
      LEDGP = 16;//13;      //LED STRIP ON PIN 13
      LEDBP = 15;      //LED STRIP ON PIN 15
  }
  
  //PIN OPTIONS FOR CAPACITIVE - RELAY OR BMP180
  if(CAPACITIVE){
      //SDA 5  SCL 4  PINS
      CAP0P = 12;//4;
      CAP1P = 14;//5;
  }
  
  if(RELAY){
      RELAY0P = 12;//4;
      RELAY1P = 14;//5;
  }

}

void SLOT_CONFIG(){
  int NEXTSLOT = 0;
  Serial.println("SLOT CONFIG");  
  
  if(DHT_SENSOR){
      TEMPERATURE = NEXTSLOT;
      HUMIDITY = NEXTSLOT + 2;
      NEXTSLOT = HUMIDITY + 2;
      Serial.print("TEMP: ");
      Serial.println(TEMPERATURE);  
      Serial.print("HUMI: ");
      Serial.println(HUMIDITY);      
  }
  
  if(PWM_MODE || PIR_MODE){
      LEDPWM0 = NEXTSLOT;
      LEDPWM1 = NEXTSLOT + 2;
      NEXTSLOT = LEDPWM1 + 2;
      Serial.print("LEDPWM0: ");
      Serial.println(LEDPWM0);  
      Serial.print("LEDPWM1: ");
      Serial.println(LEDPWM1);      
  }
  if(PWM_MODE){
      LEDPWM2 = NEXTSLOT;
      NEXTSLOT = LEDPWM2 + 2;
      Serial.print("LEDPWM2: ");
      Serial.println(LEDPWM2);     
  }
  
  if(PIR_MODE){
      LED = NEXTSLOT;
      NEXTSLOT = LED + 1;
      Serial.print("LED: ");
      Serial.println(LED);       
  }
  
  if(RGB_MODE){
      LEDRGB = NEXTSLOT;
      NEXTSLOT = LEDRGB + 4;
      Serial.print("LEDRGB: ");
      Serial.println(LEDRGB);        
  }
  
  if(LDR_SENSOR){
      LDR = NEXTSLOT;
      NEXTSLOT = LDR + 2;
      Serial.print("LDR: ");
      Serial.println(LDR);        
  }
  
  if(DALLAS_SENSOR){
      DALLAS = NEXTSLOT;
      NEXTSLOT = DALLAS + 2;
      Serial.print("DALLAS: ");
      Serial.println(DALLAS);        
  }
  
  //GPIO 4-5 SLOT DEFINITIONS
  
  if(CAPACITIVE){
      CAP0 = NEXTSLOT; 
      CAP1 = NEXTSLOT + 1;
      NEXTSLOT = CAP1 + 1;
      Serial.print("CAP0: ");
      Serial.println(CAP0);   
      Serial.print("CAP1: ");
      Serial.println(CAP1);             
  }
  
  if(RELAY){
      RELAY0 = NEXTSLOT;
      RELAY1 = NEXTSLOT + 1;
      NEXTSLOT = RELAY1 + 1;
      Serial.print("RELAY0: ");
      Serial.println(RELAY0);   
      Serial.print("RELAY1: ");
      Serial.println(RELAY1);         
  }
  
  if(BMP180){
      PRESSURE0 = NEXTSLOT;  
      BMP180TEMP = NEXTSLOT + 2;
      NEXTSLOT = BMP180TEMP + 2;
      Serial.print("PRESSURE0: ");
      Serial.println(PRESSURE0);   
      Serial.print("BMP180TEMP: ");
      Serial.println(BMP180TEMP);       
  }  
}

uint8_t CapSense(uint8_t slot, uint8_t value, uint8_t value_hold, uint8_t pin, uint8_t thresold_value, int holdtime) {

    int cycles = readCapacitivePin(pin); 
    if(DEBUG_CAPSENSE){
          if(millis()%300==0) Serial.println("Pin\tSlotIn\tOut\tPWM\tcycles\tReturn");
          Serial.print(pin);
          Serial.print("\t");        
          Serial.print(InPin[pin]);
          Serial.print("\t");        
          Serial.print(mInput(slot));
          Serial.print("\t");        
          Serial.print(mOutput(slot));
          Serial.print("\t");
          Serial.print(mOutput(slot+1));
          Serial.print("\t");
          Serial.print(cycles); 
          Serial.print("\t");
          Serial.print(thresold_value); 
          Serial.print("\t");          
          Serial.print(abs(millis()-time)); 
          Serial.print("\t");          

    }
  
    if(cycles > thresold_value && (InPin[pin] == 10 || InPin[pin] == 11)){
               InPin[pin] = PINSET;
    }
        
    if(cycles > thresold_value && (InPin[pin]==PINRESET))
	{
		time = millis();								// Record time
		InPin[pin] = PINSET;
		
		return InPin[pin];
	}
	else if(cycles > thresold_value && (abs(millis()-time) > holdtime) && (InPin[pin]==PINSET || InPin[pin]==PINACTIVE))
	{
		if(AUTOCALIBRATE && (abs(millis()-time) > 15000)){
            thresold = cycles + 2;
            if(DEBUG_CAPSENSE) { 
                Serial.print("Autocalibrate: ");
                Serial.println(thresold);
            }
            return thresold;
        }else{
            InPin[pin] = PINACTIVE;								// Stay there till pushbutton is released
      		// Write timer value in memory map
		    if (memory_map)	memory_map[MaCaco_IN_s + slot] = value_hold;
            return value_hold;
        }
	}
    else if(cycles < thresold_value && (InPin[pin]==PINSET)) {
		InPin[pin] = 10;
        return InPin[pin];
	}
	else if(cycles < thresold_value && (InPin[pin]==PINACTIVE)) {
		InPin[pin] = 11;
        return InPin[pin];
    }
    else if(cycles < thresold_value && InPin[pin] == 10){
        if(mInput(slot) == Souliss_T1n_OffCmd || mInput(slot) == Souliss_T1n_OnCmd){
          	if(mOutput(slot+1) == 0) mOutput(slot) = Souliss_T1n_OffCoil;
            else                     mOutput(slot) = Souliss_T1n_OnCoil;
                memory_map[MaCaco_IN_s + slot] = Souliss_T1n_RstCmd;			// Reset
        }
        else memory_map[MaCaco_IN_s + slot] = value; 
        
        InPin[pin] = PINRESET;     
        return value;
    }
    else if(cycles < thresold_value && InPin[pin] == 11){
          InPin[pin] = PINRESET;
          if(mOutput(slot+1) == 0) mOutput(slot) = Souliss_T1n_OffCoil;
          else                     mOutput(slot) = Souliss_T1n_OnCoil;
          memory_map[MaCaco_IN_s + slot] = Souliss_T1n_RstCmd;			// Reset
    }          
	
    // return MaCaco_NODATACHANGED;

}

/*******************************************************NUEVA FUNCION 1 PIN CAPACITIVO ***************************************/
uint8_t readCapacitivePin(int pinToMeasure) {
  pinMode(pinToMeasure, OUTPUT);
  digitalWrite(pinToMeasure, LOW);
  delay(1);
  // Prevent the timer IRQ from disturbing our measurement
  noInterrupts();
  // Make the pin an input with the internal pull-up on
  pinMode(pinToMeasure, INPUT_PULLUP);

  // Now see how long the pin to get pulled up. This manual unrolling of the loop
  // decreases the number of hardware cycles between each read of the pin,
  // thus increasing sensitivity.
  uint8_t cycles = 35;//17;
       if (digitalRead(pinToMeasure)) { cycles =  0;}
  else if (digitalRead(pinToMeasure)) { cycles =  1;}
  else if (digitalRead(pinToMeasure)) { cycles =  2;}
  else if (digitalRead(pinToMeasure)) { cycles =  3;}
  else if (digitalRead(pinToMeasure)) { cycles =  4;}
  else if (digitalRead(pinToMeasure)) { cycles =  5;}
  else if (digitalRead(pinToMeasure)) { cycles =  6;}
  else if (digitalRead(pinToMeasure)) { cycles =  7;}
  else if (digitalRead(pinToMeasure)) { cycles =  8;}
  else if (digitalRead(pinToMeasure)) { cycles =  9;}
  else if (digitalRead(pinToMeasure)) { cycles = 10;}
  else if (digitalRead(pinToMeasure)) { cycles = 11;}
  else if (digitalRead(pinToMeasure)) { cycles = 12;}
  else if (digitalRead(pinToMeasure)) { cycles = 13;}
  else if (digitalRead(pinToMeasure)) { cycles = 14;}
  else if (digitalRead(pinToMeasure)) { cycles = 15;}
  else if (digitalRead(pinToMeasure)) { cycles = 16;}
  else if (digitalRead(pinToMeasure)) { cycles = 17;}
  else if (digitalRead(pinToMeasure)) { cycles = 18;}
  else if (digitalRead(pinToMeasure)) { cycles = 19;}  
  else if (digitalRead(pinToMeasure)) { cycles = 20;}
  else if (digitalRead(pinToMeasure)) { cycles = 21;}
  else if (digitalRead(pinToMeasure)) { cycles = 22;}    
  else if (digitalRead(pinToMeasure)) { cycles = 23;}
  else if (digitalRead(pinToMeasure)) { cycles = 24;}
  else if (digitalRead(pinToMeasure)) { cycles = 25;}
  else if (digitalRead(pinToMeasure)) { cycles = 26;}
  else if (digitalRead(pinToMeasure)) { cycles = 27;}
  else if (digitalRead(pinToMeasure)) { cycles = 28;}
  else if (digitalRead(pinToMeasure)) { cycles = 29;}
  else if (digitalRead(pinToMeasure)) { cycles = 30;}
  else if (digitalRead(pinToMeasure)) { cycles = 31;}
  else if (digitalRead(pinToMeasure)) { cycles = 32;}
  else if (digitalRead(pinToMeasure)) { cycles = 33;}
  else if (digitalRead(pinToMeasure)) { cycles = 34;}
  
  
  
  // End of timing-critical section
  interrupts();

  // Discharge the pin again by setting it low and output
  //  It's important to leave the pins low if you want to 
  //  be able to touch more than 1 sensor at a time - if
  //  the sensor is left pulled high, when you touch
  //  two sensors, your body will transfer the charge between
  //  sensors.
  digitalWrite(pinToMeasure, LOW);
  pinMode(pinToMeasure, OUTPUT);

  return cycles;
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
