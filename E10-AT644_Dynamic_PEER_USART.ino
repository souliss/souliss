// NODO HABITACION OCIO, GW BASADO EN ESP conectado a ATMEGA644 por USART.
/**************** ESP12 CODE *****************************/
	/*
// Configure the framework
#include "bconf/MCU_ESP8266.h"              // Load the code directly on the ESP8266
#include "conf/Gateway.h"                   // The main node is the Gateway, we have just one node
#include "conf/DynamicAddressing.h"
#include "conf/usart.h"                      // USART 
#include "conf/IPBroadcast.h"

// **** Define the WiFi name and password ****
#define WIFICONF_INSKETCH
#define WiFi_SSID               "mywifi"
#define WiFi_Password           "mypassword"    

// Include framework code and libraries
#include <ESP8266WiFi.h>
#include <EEPROM.h>
#include "Souliss.h"

void setup()
{   
    Initialize();

    // Connect to the WiFi network and get an address from DHCP
    GetIPAddress();                           
    SetAsGateway(myvNet_dhcp);       // Set this node as gateway for SoulissApp  

    // This is the vNet address for this node, used to communicate with other
    // nodes in your Souliss network
    SetAddress(0xD001, 0xFF00, 0x0000);
    SetAddressingServer();
}

void loop()
{
    

    FAST_GatewayComms(); 
}
	*/  
/**************** USART NODE ELEMENTS ********************/
	/*      
    -Atmel ATXXX
      - Receptor IR
      - RGB  (PWM x 3)
      - Tiras Led x 3 (PWM Opcional)
      - Botones x ?
      - DHT11 x 2 - Interior Exterior
      - LDR
	*/

/**************** CONFIGURATION **************************/
	/******************* IR REMOTE   *************************/

#include <IRLib.h>
int RECV_PIN = 3;
IRrecv My_Receiver(RECV_PIN);
IRdecode My_Decoder;
unsigned int Buffer[RAWBUF];

/*#include <IRremote.h>
int RECV_PIN = 3;
IRrecv irrecv(RECV_PIN);
decode_results results;
*/

int remote_mode=0;
	/******************* IR BUTTONS DEFINITIONS **************/
//This values are long values returned by IRremote library, to get your values enable DEBUG and see results on Serial Port
#define DEBUG_IR  1
#define	B0	16748655
#define	B1	16758855
#define	B2	16775175
#define	B3	16756815
#define	B4	16750695
#define	B5	16767015
#define	B6	16746615
#define	B7	16754775
#define	B8	16771095
#define	B9	16730295
#define	B10	16738455
#define	B11	16757325
#define	B12	16712445
#define	B13	16724685
#define	B14	16720095
#define	B15	16711935
#define	B16	16732335
#define	B17	16742535
#define	B18	16740495
#define	B19	16734375
#define	B20	16726215
#define	B21	16722135
#define	B22	16773135
#define	B23	16724175
#define	B_Repeat	4294967295
	/******************* SOULISS - CONFIGURATION  ************/

#define MaCaco_DEBUG_INSKETCH
#define MaCaco_DEBUG  		0

#define VNET_DEBUG_INSKETCH
#define VNET_DEBUG  		0

#define SOULISS_DEBUG_INSKETCH
#define SOULISS_DEBUG  		0

#define interface 0

#include <dht.h>

// Configure the framework
#include "bconf/StandardArduino.h"          // Use a standard Arduino
#include "conf/usart.h"                     // USART 

// Include framework code and libraries
#include <SPI.h>
#include "Souliss.h"
	/******************* PIN CONFIG  *************************/
//CORRECTION OF A BUG ON PIN DEFINITION ON SANGUINO BOOTLOADER
#define A_0 29

//PIN definitions  
#define ButtonC_pin     0
//#define irLed_pin       3 //PWM

#define Relay0_pin      10
#define Relay1_pin      11

#define Led0_pin        15//12 //PWM
#define Led1_pin        29//13 //PWM
#define Led2_pin        30//14 //PWM
#define Led3_pin        31//15 //PWM

//#define Button0_pin     16
//#define Button1_pin     17
#define Button2_pin     18
#define Button3_pin     19
#define Button4_pin     20
#define Button5_pin     21

#define Relay2_pin      22
#define Relay3_pin      23

#define RGB_Red_pin     12//24 //PWM
#define RGB_Green_pin   13//25 //PWM
#define RGB_Blue_pin    14//26 //PWM

//#define Led4_pin        28 
//#define Led5_pin        27 

#define SensorLDR_pin   A_0//29//24//29//A5
#define Sensor1_pin     25//30//A6
#define Sensor2_pin     26//31//A7
	/******************* SLOT DEFINITIONS  *******************/


#define TEMPERATURE		0			// Identify the temperature logic
#define HUMIDITY		2			// Identify the humidaty logic
dht DHT;

#define TEMPERATURE2		4			// Identify the temperature logic
#define HUMIDITY2		6			// Identify the humidaty logic
dht DHT2;

#define LDR                     8

#define LED0		       10
#define LED1		       11
#define LED2		       12
#define LED3		       13
//#define LED4		       14
//#define LED5		       15

#define LEDRGB_0	       16				
#define LEDRED_0	       17			
#define LEDGREEN_0	       18
#define LEDBLUE_0	       19

#define RELAY0                 20
#define RELAY1                 21
#define RELAY2                 22
#define RELAY3                 23
	/******************* BLOCKS CONFIGURATION  ***************/
// 0 to DISABLE 1 TO ENABLE "BLOCKS"
#define leds       1
#define botones    0
#define rgb        1
#define relays     1
#define sensors    1
	/******************* COLORES  ****************************/
#define Rojo          0xFF,0x00,0x00
#define Verde         0x00,0xFF,0x00
#define Azul          0x00,0x00,0xFF
#define Naranja       0xFF,0x5F,0x00
#define Amarillo      0xFF,0xFF,0x00
#define AzulCielo     0x00,0xFF,0xFF
#define AzulOscuro    0x00,0x5F,0xFF
#define Morado        0xFF,0x00,0xFF
#define MoradoSuave   0xFF,0x5F,0xFF
#define Blanco        0xFF,0xFF,0xFF
	/******************* COLORS for setBrightColor FUNCTION **/
#define Red           1
#define Green         2
#define Blue          3
	/******************* KLUX TABLE FOR GET_LUX FUCNTION *****/
//NUEVA FUNCION PARA LEER LDR Y PASARLO A KLUX:

// Light calibration data
// out[] holds the values wanted in lux/10
static const unsigned int out[] = { 0,1,10,30,40,60,80,170,230,340,470,700,1500,2100,4500,11500 };
        // in[] holds the measured analogRead() values for defined light levels
        // note: the in array should have increasing values
static const unsigned int in[]  = { 30,200,650,810,850,880,910,950,960,970,980,990,1000,1005,1010,1015 };

/**************** SETUP **********************************/
void setup(){	
	/*************************** ENABLE IR RECEIVER  *****************/
	My_Receiver.enableIRIn(); // Start the receiver
	My_Decoder.UseExtnBuf(Buffer);
	/*************************** NETWORK SET *************************/ 
	Initialize();
	
	// Set network parameters
	SetAddress(0xD002, 0xFF00, 0xD001); 
	/*************************** SET LED STRIPS  *********************/
	if (leds){
		Set_SimpleLight(LED0);			// Define a simple LED light logic
		Set_SimpleLight(LED1);			// Define a simple LED light logic
		Set_SimpleLight(LED2);			// Define a simple LED light logic
		Set_SimpleLight(LED3);			// Define a simple LED light logic
		//Set_SimpleLight(LED4);			// Define a simple LED light logic
		//Set_SimpleLight(LED5);			// Define a simple LED light logic
	}
	if(rgb) Set_LED_Strip(LEDRGB_0);      
	/*************************** SET RELAYS **************************/
	if(relays){
		Set_SimpleLight(RELAY0);			// Define a simple LED light logic
		Set_SimpleLight(RELAY1);			// Define a simple LED light logic
		Set_SimpleLight(RELAY2);			// Define a simple LED light logic        
		Set_SimpleLight(RELAY3);			// Define a simple LED light logic  
    }
	/*************************** SET SENSORS *************************/	
    if(sensors){
		Set_Temperature(TEMPERATURE);		// Define one temperature measure
		Set_Humidity(HUMIDITY);				// Define one humidity measure
        	
		Set_Temperature(TEMPERATURE2);		// Define one temperature measure
		Set_Humidity(HUMIDITY2);		// Define one humidity measure
        	
		pinMode(SensorLDR_pin, INPUT);
		Souliss_SetT54(memory_map, LDR);
                
    }
	/*************************** PIN CONFIGURATION *******************/        
	// We connect a pushbutton between 5V and Button*_pin with a pulldown resistor 
	// between pin2 and GND, the LED is connected to pin9 with a resistor to
	// limit the current amount
        if(botones){
            //pinMode(Button0_pin, INPUT);					// Hardware pulldown required
			//pinMode(Button1_pin, INPUT);					// Hardware pulldown required
			pinMode(Button2_pin, INPUT);					// Hardware pulldown required
			pinMode(Button3_pin, INPUT);					// Hardware pulldown required
			pinMode(Button4_pin, INPUT);					// Hardware pulldown required
			pinMode(Button5_pin, INPUT);					// Hardware pulldown required
        }
        if(leds){
			pinMode(Led0_pin, OUTPUT);					// Power the LED
			pinMode(Led1_pin, OUTPUT);					// Power the LED
			pinMode(Led2_pin, OUTPUT);					// Power the LedPWM
			pinMode(Led3_pin, OUTPUT);					// Power the LED
			//pinMode(Led4_pin, OUTPUT);					// Power the LED
			//pinMode(Led5_pin, OUTPUT);					// Power the LedPWM
        }
        if(rgb){
			pinMode(RGB_Red_pin, OUTPUT);					// Power the LED
			pinMode(RGB_Green_pin, OUTPUT);					// Power the LED
			pinMode(RGB_Blue_pin, OUTPUT);					// Power the LedPWM
        }
        if(relays){
			digitalWrite(Relay0_pin, HIGH);
			digitalWrite(Relay1_pin, HIGH);
			digitalWrite(Relay2_pin, HIGH);
			digitalWrite(Relay3_pin, HIGH);
        
            pinMode(Relay0_pin, OUTPUT);					// Power the LedPWM
			pinMode(Relay1_pin, OUTPUT);					// Power the LedPWM
			pinMode(Relay2_pin, OUTPUT);					// Power the LedPWM
			pinMode(Relay3_pin, OUTPUT);					// Power the LedPWM
        }
}

/**************** LOOP ***********************************/
void loop(){   
	EXECUTEFAST() {						
		UPDATEFAST();	
		FAST_30ms() {    
/********************* RGB LOGIC - OUTPUT - DEBUG ************************/                     
    	if(rgb){
				Logic_LED_Strip(LEDRGB_0);
				// Use the output values to control the PWM
				analogWrite(RGB_Red_pin, mOutput(LEDRED_0));
				analogWrite(RGB_Green_pin, mOutput(LEDGREEN_0));
				analogWrite(RGB_Blue_pin, mOutput(LEDBLUE_0));
    	}
    
		// DEBUG RGB
		//	Serial.print(ssOutput(LEDRGB_0)); Serial.print(" /  ");
		//	Serial.print(ssOutput(LEDRGB_0+1)); Serial.print(" /  ");
		//	Serial.print(ssOutput(LEDRGB_0+2)); Serial.print(" /  ");
		//	Serial.print(ssOutput(LEDRGB_0+3)); Serial.println("");    
		} //RGB LOGIC - OUTPUT - DEBUG
                
		FAST_50ms() {	// We process the logic and relevant input and output every 50 milliseconds
/*********************** IR BUTTON ASSIGNATION TO SLOT *********************/    
			read_IR(); // READ IR CODES     
			Souliss_IrIn(B12, Souliss_T1n_ToggleCmd, memory_map, LED0);
			Souliss_IrIn(B5, Souliss_T1n_ToggleCmd, memory_map, LED1);
			Souliss_IrIn(B6, Souliss_T1n_ToggleCmd, memory_map, LED2);                        
			Souliss_IrIn(B7, Souliss_T1n_ToggleCmd, memory_map, LEDRGB_0);
			if(Souliss_IrIn(B2, 0xFF, memory_map, LEDRGB_0)) Serial.println("//Sleep 255");
                     
			Souliss_IrIn(B4, Souliss_T1n_ToggleCmd, memory_map, LED3);
			//Souliss_IrIn(B13, Souliss_T1n_ToogleCmd, memory_map, LED4);
			//Souliss_IrIn(B14, Souliss_T1n_ToogleCmd, memory_map, LED5); 

			Souliss_IrIn(B15, Souliss_T1n_BrightUp, memory_map, LEDRGB_0);
			Souliss_IrIn(B19, Souliss_T1n_BrightDown, memory_map, LEDRGB_0);
/*********************** IR BUTTON ASSIGNATION COLORS CALL FUNCTION *********************/                        
			//Buttons_to_Colors();   
			/************************************* COLORES ************************/
			/*
			#define Rojo          0xFF,0x00,0x00
			#define Verde         0x00,0xFF,0x00
			#define Azul          0x00,0x00,0xFF
			#define Naranja       0xFF,0x5F,0x00
			#define Amarillo      0xFF,0xFF,0x00
			#define AzulCielo     0x00,0xFF,0xFF
			#define AzulOscuro    0x00,0x5F,0xFF
			#define Morado        0xFF,0x00,0xFF
			#define MoradoSuave   0xFF,0x5F,0xFF
			#define Blanco        0xFF,0xFF,0xFF
			*/
			
			if(Souliss_IrIn(B1, 0, memory_map, 0)){ 
				//Button to change between Modes
				if (remote_mode == 0) remote_mode=1;
				else remote_mode=0;
			}
			if(Souliss_IrIn(B16, 0, memory_map, LEDRGB_0)){ 
				if (remote_mode == 0 ) setColor(LEDRGB_0, Naranja);
				else setBrightColor(LEDRGB_0, Red, Souliss_T1n_BrightUp);
			}
			if(Souliss_IrIn(B17, 0, memory_map, LEDRGB_0)){ 
				if (remote_mode == 0) setColor(LEDRGB_0, Azul);
				else setBrightColor(LEDRGB_0, Green, Souliss_T1n_BrightUp);    
			}
			if(Souliss_IrIn(B18, 0, memory_map, LEDRGB_0)){ 
				if (remote_mode == 0 ) setColor(LEDRGB_0, Morado);       
				else setBrightColor(LEDRGB_0, Blue, Souliss_T1n_BrightUp);                            
			}
			if(Souliss_IrIn(B20, 0, memory_map, LEDRGB_0)){ 
				if (remote_mode == 0 ) setColor(LEDRGB_0, Amarillo);   
				else setBrightColor(LEDRGB_0, Red, Souliss_T1n_BrightDown);                         
			}
			if(Souliss_IrIn(B21, 0, memory_map, LEDRGB_0)){ 
				if (remote_mode == 0 ) setColor(LEDRGB_0, Blanco);                            
				else setBrightColor(LEDRGB_0, Green, Souliss_T1n_BrightDown);
			}
			if(Souliss_IrIn(B22, 0, memory_map, LEDRGB_0)){ 
				if (remote_mode == 0 ) setColor(LEDRGB_0, MoradoSuave);                      
				else setBrightColor(LEDRGB_0, Blue, Souliss_T1n_BrightDown);                            
			}
/*********************** PUSH BUTTON ASSIGNATION TO SLOT ***************/
			if(botones){ 
				DigIn(Button3_pin, Souliss_T1n_ToggleCmd, LED0);
				DigIn(Button4_pin, Souliss_T1n_ToggleCmd, LED1);
				DigIn(Button2_pin, Souliss_T1n_ToggleCmd, LED2);
				//ssDigIn(Button5_pin, Souliss_T1n_ToogleCmd, LED5);
			}
/*********************** LOGIC LED STRIPS ******************************/                      
 			if(leds){
				Logic_SimpleLight(LED0);
				DigOut(Led0_pin, Souliss_T1n_Coil, LED0);

				Logic_SimpleLight(LED1);
				DigOut(Led1_pin, Souliss_T1n_Coil, LED1);

				Logic_SimpleLight(LED2);
				DigOut(Led2_pin, Souliss_T1n_Coil, LED2);

				Logic_SimpleLight(LED3);
				DigOut(Led3_pin, Souliss_T1n_Coil, LED3);

				//Logic_SimpleLight(LED4);
				//ssDigOut(Led4_pin, Souliss_T1n_Coil, LED4);

				//Logic_SimpleLight(LED5);
				//ssDigOut(Led5_pin, Souliss_T1n_Coil, LED5);     
 			}
/*********************** IR BUTTONS TO RELAYS***************************/
			Souliss_IrIn(B8, Souliss_T1n_ToggleCmd, memory_map, RELAY0);
			Souliss_IrIn(B9, Souliss_T1n_ToggleCmd, memory_map, RELAY1);
			Souliss_IrIn(B10, Souliss_T1n_ToggleCmd, memory_map, RELAY2);
			Souliss_IrIn(B11, Souliss_T1n_ToggleCmd, memory_map, RELAY3);
/*********************** LOGIC RELAYS **********************************/
			if(relays){
				Logic_SimpleLight(RELAY0);
				LowDigOut(Relay0_pin, Souliss_T1n_Coil, RELAY0);				
				Logic_SimpleLight(RELAY1);
				LowDigOut(Relay1_pin, Souliss_T1n_Coil, RELAY1);				
				Logic_SimpleLight(RELAY2);
				LowDigOut(Relay2_pin, Souliss_T1n_Coil, RELAY2);				
				Logic_SimpleLight(RELAY3);
				LowDigOut(Relay3_pin, Souliss_T1n_Coil, RELAY3);				
			}
		}//END FAST_50
		
/*********************** SENSORS LOGIC *********************************/
	if(sensors){		
		FAST_510ms() {	// We retrieve data from the node with index 1 (peervNet_address)
			Logic_Temperature(TEMPERATURE);
			Logic_Humidity(HUMIDITY);
		}
		
		FAST_710ms() {	// We retrieve typical (device type connected to the board) for node with index 1
			Logic_Temperature(TEMPERATURE2);
			Logic_Humidity(HUMIDITY2);
		}
    		FAST_1110ms() { // Compare previous and new input measure and provide the updates to SoulissApp
			Souliss_Logic_T54(memory_map, LDR, 0.05, &data_changed);
    		}
	}
/*********************** PEER COMMS *******************************/
  	FAST_PeerComms();
  	START_PeerJoin();
	}
	
	EXECUTESLOW() {
		UPDATESLOW();
		SLOW_10s() {		// We handle the light timer with a 10 seconds base time
			if(leds){
				Souliss_T11_Timer(memory_map, LED0);
				Souliss_T11_Timer(memory_map, LED1);
				Souliss_T11_Timer(memory_map, LED2);
				Souliss_T11_Timer(memory_map, LED3);
				//Souliss_T11_Timer(memory_map, LED4);
				//Souliss_T11_Timer(memory_map, LED5);                        
			}
			if(rgb) Timer_LED_Strip(LEDRGB_0);

			if(relays){
				Souliss_T11_Timer(memory_map, RELAY0);
				Souliss_T11_Timer(memory_map, RELAY1);
				Souliss_T11_Timer(memory_map, RELAY2);			
				Souliss_T11_Timer(memory_map, RELAY3);			
			}           
		} 
			if(sensors){
				SLOW_50s() {  // Read temperature and humidity from DHT every 110 seconds  
					//SENSOR 1		
					DHT.read11(Sensor1_pin);	
					float temperature = 0;
					if (DHT.temperature < 65) temperature = DHT.temperature;   // "IF" to AVOID ERRORS ON READING
					Souliss_ImportAnalog(memory_map, TEMPERATURE, &temperature);
					// Read humidity value from DHT sensor and convert from single-precision to half-precision
					float humidity = 0;
					if (DHT.humidity < 100) humidity = DHT.humidity;   // "IF" to AVOID ERRORS ON READING
					Souliss_ImportAnalog(memory_map, HUMIDITY, &humidity);	
				} 	
				SLOW_70s() {  // Read temperature and humidity from DHT every 110 seconds  
					//SENSOR 2
					DHT2.read11(Sensor2_pin);
					float temperature2 = 0;
					if (DHT2.temperature < 65) temperature2 = DHT2.temperature; // "IF" to AVOID ERRORS ON READING
					Souliss_ImportAnalog(memory_map, TEMPERATURE2, &temperature2);
			
					// Read humidity value from DHT sensor and convert from single-precision to half-precision
					float humidity2 = 0;
					if (DHT2.humidity < 100) humidity2 = DHT2.humidity; // "IF" to AVOID ERRORS ON READING
					Souliss_ImportAnalog(memory_map, HUMIDITY2, &humidity2);	      
				}
        			SLOW_90s() {  // Read temperature and humidity from DHT every 110 seconds 
          				//Serial.print("Time/min: ");
          				//Serial.println(millis()/60000); 
					//LDR SENSOR
					float ldr_read = get_lux(SensorLDR_pin, in, out, 16)/10.0;  //ORIGINAL
					//float ldr_read = get_lux(SensorLDR_pin, in, out, 16)/10.0;  //MEDIDA EN KLUX
					//Serial.print("Lux: ");
					//Serial.println(ldr_read);
					if (ldr_read == 0) ldr_read = 0.01;
					Souliss_ImportAnalog(memory_map, LDR, &ldr_read);
				}
             
			}
	}		
} 

/**************** FUNCTIONS ******************************/
	/******************** READ IR ******************************/
void read_IR() {
    /*if (irrecv.decode(&results)) {
        irrecv.resume(); // Receive the next value
        if (DEBUG_IR) Serial.println(results.value);//, HEX);
    }*/
  if (My_Receiver.GetResults(&My_Decoder)) {
    //Restart the receiver so it can be capturing another code
    //while we are working on decoding this one.
    My_Receiver.resume(); 
    My_Decoder.decode();
    //if (DEBUG_IR) Serial.println(My_Decoder.value);//, HEX);
    //My_Decoder.DumpResults();
  }
    
}
	/******************** SOULISS IRIN *************************/
uint8_t Souliss_IrIn(long remote_button, uint8_t value_state1, uint8_t *memory_map, uint8_t slot) {

    if (My_Decoder.value == remote_button)  
    //if (results.value == remote_button)
     {
                memory_map[MaCaco_IN_s + slot] = value_state1;
                //results.value = 0;
                My_Decoder.value = 0;
		return MaCaco_DATACHANGED;
     }
     else return MaCaco_NODATACHANGED;
}
	/******************** SET COLOR ****************************/
void setColor(uint8_t slot, uint8_t R, uint8_t G, uint8_t B)
{
  mInput(slot+1)=R;
  mInput(slot+2)=G;
  mInput(slot+3)=B;
//  ssInput(slot)=Souliss_T1n_Coil;
  mInput(slot)=Souliss_T1n_Set;
}
	/******************** SET BRIGHTCOLOR **********************/
void setBrightColor(uint8_t slot, uint8_t color, uint8_t value_state)  //color:  Red=1 Green=2 Blue=3
{
  for (int i = 1; i<=3; i++)
  {
    if ( i == color ) 
    { 
      if (value_state == Souliss_T1n_BrightUp && mOutput(slot+color) +10 <= 254) { mInput(slot+color) = mOutput(slot+color) +10; }
      if (value_state == Souliss_T1n_BrightDown && mOutput(slot+color) -10 >= 0) { mInput(slot+color) = mOutput(slot+color) -10; }
    }
    else 
    {
      mInput(slot+i)=mOutput(slot+i);      
    }
  }
  mInput(slot)=Souliss_T1n_Set;
  
}
	/******************** GET LUX FUNCTION *********************/
// Calculate lux based on rawADC reading from LDR returns value in lux/10

int get_lux(unsigned int pin, const unsigned int* _in, const unsigned int* _out, byte size)
{
	
        // take care the value is within range
	// val = constrain(val, _in[0], _in[size-1]);
	int val = analogRead(pin);
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
