// NODO HABITACION OCIO, GW BASADO EN ESP conectado a ATMEGA644 por USART.
/*
  Distribucion del NODO:
    -ESP12:
      - 
      
    -Atmel ATXXX
      - Receptor IR
      - RGB  (PWM x 3)
      - Tiras Led x 3 (PWM Opcional)
      - Botones x ?
      - DHT11 x 2 - Interior Exterior
      - LDR
*/





/****************************IR REMOTE ************************************/


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
/******************************** BUTTONS DEFINITIONS *************************/
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

/**************************************************************************
	Souliss - Bridge
	
	It handle two lights located on two different boards and act them togheter
	if receive a command from the push button. 
	
	Read Temperature - Humidity

	It use Souliss Speak Easy to make write the sketch even easier.
	
	CONFIGURATION IS MANDATORY BEFORE COMPILING

	PIN 2 - Pushbutton
	PIN 3 - irLed
	PIN 4 - Pushbutton
	PIN 8 - Led
	PIN 9 - LedPWM
	PIN 6 - DHT11
	PIN 7 - DHT11
	
	ENC28J60:
		CS  - pin 10
		SI  - PIN 11
		SO  - PIN 12
		SCK - PIN 13
		
		ENC CONNECTOR:
		   *-*-*-*
		   -     -
		   -     -SO 
		SI -     -SCK
		CS -     -
		VCC-     -GND
	
	******************** Configuration Parameters *********************
	
		INSKETCH
		Modify files:
			GetConfig.h - Add lines after line 396
				#	elif(QC_BOARDTYPE == 0x41)
                                #		define	ETH_W5100				1	
                                #		define	COMMS_MODEL				0x01
                                #		define	BOARD_MODEL				0x02
                                #		define 	VNET_SUPERNODE  		1
                                #		define VNET_MEDIA1_ENABLE  		1
                                #		define VNET_MEDIA2_ENABLE  		0
                                #		define VNET_MEDIA3_ENABLE  		0
                                #		define VNET_MEDIA4_ENABLE  		0
                                #		define VNET_MEDIA5_ENABLE  		1
                                #	elif(QC_BOARDTYPE == 0x42)
                                #		define	ETH_ENC28J60			1	
                                #		define	COMMS_MODEL				0x01
                                #		define	BOARD_MODEL				0x02
                                #		define 	VNET_SUPERNODE  		1
                                #		define VNET_MEDIA1_ENABLE  		1
                                #		define VNET_MEDIA2_ENABLE  		0
                                #		define VNET_MEDIA3_ENABLE  		0
                                #		define VNET_MEDIA4_ENABLE  		0
                                #		define VNET_MEDIA5_ENABLE  		1
                                #	elif(QC_BOARDTYPE == 0x43)	
                                #		define	COMMS_MODEL				0x01
                                #		define	BOARD_MODEL				0x02
                                #		define VNET_MEDIA1_ENABLE  		0
                                #		define VNET_MEDIA2_ENABLE  		0
                                #		define VNET_MEDIA3_ENABLE  		0
                                #		define VNET_MEDIA4_ENABLE  		0
                                #		define VNET_MEDIA5_ENABLE  		1
                                #	endif
			QuickCfg.h  - Add info files after line 103:
                  		0x41		Arduino Ethernet (or Ethernet Shield) with USART 
                  		0x42		Arduino with ENC28J60 Ethernet Shield and USART
		                0x43		Arduino with USART

                                    -  Edit line 181
				#if(QC_ENABLE && !defined(DHTSENSOR_INSKETCH))

                  
		
***************************************************************************/
#define MaCaco_DEBUG_INSKETCH
#define MaCaco_DEBUG  		1

#define VNET_DEBUG_INSKETCH
#define VNET_DEBUG  		1

#define SOULISS_DEBUG_INSKETCH
#define SOULISS_DEBUG  		0

#define NODESIZE_INSKETCH
#define MaCaco_NODES			10//5//45//30												// Number of remote nodes
#define MaCaco_SLOT			24//24//35												// Number of slot

#define NRF24PINS_INSKETCH
#define	NRF24_RADIOEN		1					// Chip Enable Pin
#define	NRF24_SPICS		2					// SPI Chip Select Pin
/*  26/03/15
#include "bconf/inSketch.h"
#define	QC_BOARDTYPE				0x45	//Arduino with ENC28J60 Ethernet Shield and nRF
#define	QC_GATEWAYTYPE				0x01	//Gateway
#define	QC_INTERFACE				0x00	//Gateway
*/
#define interface 0

#include <dht.h>

// Configure the framework
#include "bconf/StandardArduino.h"			// Use a standard Arduino
#include "conf/ethENC28J60.h"					// Ethernet through Wiznet W5100
#include "conf/nRF24L01.h"
#include "conf/Gateway.h"
#include "conf/DynamicAddressing.h" 
#include <EEPROM.h>					// The main node is the Gateway
#include <SPI.h>
#include "Souliss.h"

// Define the network configuration according to your router settings
uint8_t ip_address[4]  = {192, 168, 1, 200};
uint8_t subnet_mask[4] = {255, 255, 255, 0};
uint8_t ip_gateway[4]  = {192, 168, 1, 1};
#define	Gateway_address	0x6501				// The Gateway node has two address, one on the Ethernet side
											// and the other on the wireless one
//#define	Peer_address	0x6502
#define myvNet_address	ip_address[3]		// The last byte of the IP address (77) is also the vNet address
#define	myvNet_subnet	0xFF00
#define	myvNet_supern	Gateway_address

// Define the network configuration
//#define bridge_address	        0x6501
//#define eth_bridge_address	ip_address[3]
#define peer1_address		0x00C9  //ESP WIFI ENTRADA
#define peer2_address		0x00CA  //ESP WIFI DORMIR
#define peer3_address		0x00CB  //
#define peer4_address		0x00CC  //
#define peer5_address		0x6502  //NRF DORMIR
#define peer6_address		0x6503  //NRF ENTRADA
//#define peer7_address		0x6506

//#define myvNet_subnet		0xFF00
//#define myvNet_supern		0x0000

/*  26/03/15
#include "Souliss.h"
#include "Typicals.h"
#include "SpeakEasy.h"						// Is a library to code easy Souliss examples
#include <SPI.h>
*/
/************************** PRESSURE ***************************/
// Your sketch must #include this library, and the Wire library.
// (Wire is a standard library included with Arduino.):

#include <SFE_BMP180.h>
#include <Wire.h>

// You will need to create an SFE_BMP180 object, here called "pressure":
SFE_BMP180 pressure;


/*************************************************************/

//#include "extras/RGB_LedLamp.h"				// Include the map for the IR codes
//#include "extras/MyRGB_LedLamp.cpp"

//CORRECTION OF A BUG ON PIN DEFINITION ON SANGUINO BOOTLOADER
#define A_0 29

//USADOS POR ENC28J60  4 5 6 7
//USADOS POR nRF     1 2 5 6 7
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

/*****************  SLOT DEFINITIONS  ***********************/


#define TEMPERATURE		0			// Identify the temperature logic
#define HUMIDITY		2			// Identify the humidaty logic
dht DHT;

#define TEMPERATURE2		4			// Identify the temperature logic
#define HUMIDITY2		6			// Identify the humidaty logic
dht DHT2;
#define PRESSURE0	        8

#define LDR                     10

#define LED0		       12
#define LED1		       13
#define LED2		       14
#define LED3		       15
//#define LED4		       16
//#define LED5		       15

#define LEDRGB_0	       16				
#define LEDRED_0	       17			
#define LEDGREEN_0	       18
#define LEDBLUE_0	       19

#define RELAY0                 20
#define RELAY1                 21
#define RELAY2                 22
#define RELAY3                 23






// 0 to DISABLE 1 TO ENABLE "BLOCKS"
#define leds       1
#define botones    0
#define rgb        1
#define relays     1
#define sensors    1
#define pressure_sensor   0

/************************************* COLORES ************************/
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

/****************************** COLORS FOR setBrightColor FUNCTION ******************/
#define Red           1
#define Green         2
#define Blue          3

//NUEVA FUNCION PARA LEER LDR Y PASARLO A KLUX:

// Light calibration data
// out[] holds the values wanted in lux/10
static const unsigned int out[] = { 0,1,10,30,40,60,80,170,230,340,470,700,1500,2100,4500,11500 };
        // in[] holds the measured analogRead() values for defined light levels
        // note: the in array should have increasing values
static const unsigned int in[]  = { 30,200,650,810,850,880,910,950,960,970,980,990,1000,1005,1010,1015 };

void setup()
{	
  
/************************ENABLE IR RECEIVER*******************/
        //if(!Cap_Buttons){
          Serial.begin(57600);
          Serial.println("Start");
        //}
          
        //irrecv.enableIRIn(); // Start the receiver
        My_Receiver.enableIRIn(); // Start the receiver
        My_Decoder.UseExtnBuf(Buffer);


/************************ NETWORK SET ************************/ 
        Initialize();
	
	// Set network parameters
	Souliss_SetIPAddress(ip_address, subnet_mask, ip_gateway);							// Address on the Ethernet interface
	SetAsGateway(myvNet_address);	
        SetAddress(0xAB01, 0xFF00, 0x0000);													// Set this node as gateway for SoulissApp	
        SetAddress(0x00C8, 0xFF00, 0x0000);
	
        //11/07/15 //Souliss_SetAddress(Gateway_address, myvNet_subnet, myvNet_supern);					// Address on the wireless interface	
/* 26/03/15        
        //SetAddressingServer();
	// Setup the network configuration
	//
	// The vNet address is 11(hex) that is 17(dec), so the IP address is
	// the DEFAULT_BASEIPADDRESS[] defined in ethUsrCfg.h plus 17 on last 
	// octect. If DEFAULT_BASEIPADDRESS[] = {192, 168, 1, 0} the IP address
	// for the board will be 192.168.1.17
		
        Souliss_SetAddress(eth_bridge_address, myvNet_subnet, myvNet_supern);		
	Souliss_SetAddress(bridge_address, myvNet_subnet, myvNet_supern);

	// Load the address also in the memory_map
	Souliss_SetLocalAddress(memory_map, eth_bridge_address);
	Souliss_SetRemoteAddress(memory_map, peer1_address, 1);	
	Souliss_SetRemoteAddress(memory_map, peer2_address, 2);	
	Souliss_SetRemoteAddress(memory_map, peer3_address, 3);	
	Souliss_SetRemoteAddress(memory_map, peer4_address, 4);
//	Souliss_SetRemoteAddress(memory_map, peer5_address, 5);
*/
        //SetAddress(0xAB01, 0xFF00, 0x0000);
        //SetAsPeerNode(0xAB02, 1);
	// Set the addresses of the remote nodes
	// This node as gateway will get data from the Peer
	SetAsPeerNode(peer1_address, 1);	
	SetAsPeerNode(peer2_address, 2);	
	SetAsPeerNode(peer3_address, 3);	
	SetAsPeerNode(peer4_address, 4);	
	SetAsPeerNode(peer5_address, 5);	
	SetAsPeerNode(peer6_address, 6);

/******************* SET LED STRIPS ********************/
        if (leds){
                Set_SimpleLight(LED0);			// Define a simple LED light logic
                Set_SimpleLight(LED1);			// Define a simple LED light logic
                Set_SimpleLight(LED2);			// Define a simple LED light logic
                Set_SimpleLight(LED3);			// Define a simple LED light logic
                //Set_SimpleLight(LED4);			// Define a simple LED light logic
                //Set_SimpleLight(LED5);			// Define a simple LED light logic
                
        }
        if(rgb) Set_LED_Strip(LEDRGB_0);      
        /****************** SET RELAYS ************************/
        if(relays){
                Set_SimpleLight(RELAY0);			// Define a simple LED light logic
                Set_SimpleLight(RELAY1);			// Define a simple LED light logic
                Set_SimpleLight(RELAY2);			// Define a simple LED light logic        
                Set_SimpleLight(RELAY3);			// Define a simple LED light logic  
        }
        /***************   SET SENSORS **************************/	
        if(sensors){
        	Set_Temperature(TEMPERATURE);		// Define one temperature measure
        	Set_Humidity(HUMIDITY);				// Define one humidity measure
        	
                Set_Temperature(TEMPERATURE2);		// Define one temperature measure
        	Set_Humidity(HUMIDITY2);		// Define one humidity measure
        	
                pinMode(SensorLDR_pin, INPUT);
                Souliss_SetT54(memory_map, LDR);
                
                if(pressure_sensor) 
                Souliss_SetT51(memory_map, PRESSURE0);
        }
                // We connect a pushbutton between 5V and Button*_pin with a pulldown resistor 
        	// between pin2 and GND, the LED is connected to pin9 with a resistor to
        	// limit the current amount
        	
        	//pinMode(irLed_pin, OUTPUT);					// irLed
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
       
        /***************************** SET PRESSURE SENSOR *********************************
        // Initialize the sensor (it is important to get calibration values stored on the device).
        
        if (pressure.begin())
            Serial.println("BMP180 init success");
        else
        {
            // Oops, something went wrong, this is usually a connection problem,
            // see the comments at the top of this sketch for the proper connections.
        
            Serial.println("BMP180 init fail\n\n");
            //while(1); // Pause forever.
        }
        */
        
        if(pressure_sensor){ 
           pressure.begin();	
           delay(1000);
        }
}

void loop()
{   
	// Here we start to play
	EXECUTEFAST() {						
		UPDATEFAST();	
		FAST_30ms() {
                     
                     if(rgb){
                        Logic_LED_Strip(LEDRGB_0);
			// Use the output values to control the PWM
			analogWrite(RGB_Red_pin, mOutput(LEDRED_0));
			analogWrite(RGB_Green_pin, mOutput(LEDGREEN_0));
			analogWrite(RGB_Blue_pin, mOutput(LEDBLUE_0));
                        
                     }
                }
		/* DEBUG RGB
                        Serial.print(ssOutput(LEDRGB_0)); Serial.print(" /  ");
                        Serial.print(ssOutput(LEDRGB_0+1)); Serial.print(" /  ");
                        Serial.print(ssOutput(LEDRGB_0+2)); Serial.print(" /  ");
                        Serial.print(ssOutput(LEDRGB_0+3)); Serial.println("");    
                }*/
                
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
                        
/*********************** BUTTON ASSIGNATION COLORS CALL FUNCTION *********************/                        
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
                        if(Souliss_IrIn(B1, 0, memory_map, 0)) 
                        { 
                             if (remote_mode == 0) remote_mode=1;
                             else remote_mode=0;
                        }
                        
                        if(Souliss_IrIn(B16, 0, memory_map, LEDRGB_0)) 
                        { 
                            if (remote_mode == 0 ) setColor(LEDRGB_0, Naranja);
                            else setBrightColor(LEDRGB_0, Red, Souliss_T1n_BrightUp);
                        }
                        if(Souliss_IrIn(B17, 0, memory_map, LEDRGB_0)) 
                        { 
                            if (remote_mode == 0) setColor(LEDRGB_0, Azul);
                            else setBrightColor(LEDRGB_0, Green, Souliss_T1n_BrightUp);    
                        }
                        if(Souliss_IrIn(B18, 0, memory_map, LEDRGB_0)) 
                        { 
                            if (remote_mode == 0 ) setColor(LEDRGB_0, Morado);       
                            else setBrightColor(LEDRGB_0, Blue, Souliss_T1n_BrightUp);                            
                        }
                        if(Souliss_IrIn(B20, 0, memory_map, LEDRGB_0)) 
                        { 
                            if (remote_mode == 0 ) setColor(LEDRGB_0, Amarillo);   
                            else setBrightColor(LEDRGB_0, Red, Souliss_T1n_BrightDown);                         
                        }
                        if(Souliss_IrIn(B21, 0, memory_map, LEDRGB_0)) 
                        { 
                            if (remote_mode == 0 ) setColor(LEDRGB_0, Blanco);                            
                            else setBrightColor(LEDRGB_0, Green, Souliss_T1n_BrightDown);
                          
                        }
                        if(Souliss_IrIn(B22, 0, memory_map, LEDRGB_0)) 
                        { 
                            if (remote_mode == 0 ) setColor(LEDRGB_0, MoradoSuave);                      
                            else setBrightColor(LEDRGB_0, Blue, Souliss_T1n_BrightDown);                            
                        }
                      
                        
/*********************** BUTTON ASSIGNATION TO SLOT *********************/
if(botones){ 

                        DigIn(Button3_pin, Souliss_T1n_ToggleCmd, LED0);
                        DigIn(Button4_pin, Souliss_T1n_ToggleCmd, LED1);
                        DigIn(Button2_pin, Souliss_T1n_ToggleCmd, LED2);
                        //ssDigIn(Button5_pin, Souliss_T1n_ToogleCmd, LED5);
                        
                        /*ssDigIn(Button0_pin, Souliss_T1n_ToogleCmd, LED0);
                        ssDigIn(Button1_pin, Souliss_T1n_ToogleCmd, LED1);
                        ssDigIn(Button2_pin, Souliss_T1n_ToogleCmd, LED2);
                        ssDigIn(Button3_pin, Souliss_T1n_ToogleCmd, LED3);
                        ssDigIn(Button4_pin, Souliss_T1n_ToogleCmd, LED4);
                        ssDigIn(Button5_pin, Souliss_T1n_ToogleCmd, LED5);*/
                        
}
 /************************** LOGIC LED STRIPS *************************/                      
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
 
/*************************IR BUTTONS TO RELAYS*****************************/
                        Souliss_IrIn(B8, Souliss_T1n_ToggleCmd, memory_map, RELAY0);
                        Souliss_IrIn(B9, Souliss_T1n_ToggleCmd, memory_map, RELAY1);
                        Souliss_IrIn(B10, Souliss_T1n_ToggleCmd, memory_map, RELAY2);
                        Souliss_IrIn(B11, Souliss_T1n_ToggleCmd, memory_map, RELAY3);
/*************************** LOGIC RELAYS *********************************/
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
		
if(sensors){		
		FAST_510ms() {	// We retrieve data from the node with index 1 (peervNet_address)
			Logic_Temperature(TEMPERATURE);
			Logic_Humidity(HUMIDITY);
		}
		
		FAST_710ms() {	// We retrieve typical (device type connected to the board) for node with index 1
			//ssGetTypicals();
                        Logic_Temperature(TEMPERATURE2);
			Logic_Humidity(HUMIDITY2);
                        
		}
                FAST_1110ms() { // Compare previous and new input measure and provide the updates to SoulissApp
                      
                        Souliss_Logic_T54(memory_map, LDR, 0.05, &data_changed);
                     	
                        
                }
                FAST_2110ms() {
                        if(pressure_sensor) 
                        Souliss_Logic_T51(memory_map, PRESSURE0, 0.001, &data_changed);
                }
}
                //openHABInterface(memory_map);
                FAST_GatewayComms();
                
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
if(rgb)                 Timer_LED_Strip(LEDRGB_0);

if(relays){
                        Souliss_T11_Timer(memory_map, RELAY0);
                        Souliss_T11_Timer(memory_map, RELAY1);
                        Souliss_T11_Timer(memory_map, RELAY2);			
                        Souliss_T11_Timer(memory_map, RELAY3);			
}           
		} 
if(sensors){
		
        SLOW_50s() {  // Read temperature and humidity from DHT every 110 seconds  
// SENSOR PRESSURE
            if(pressure_sensor){
                float pressure_now = get_pressure();
                Serial.print("Presion: ");
                Serial.println(pressure_now,2);
                if(pressure_now < 1050 && pressure_now > 850)    // "IF" to AVOID ERRORS ON READING
        	        Souliss_ImportAnalog(memory_map, PRESSURE0, &pressure_now);
            }        
// SENSOR 1		
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
// SENSOR 2
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
            Serial.print("Time/min: ");
            Serial.println(millis()/60000); 
//LDR SENSOR
            float ldr_read = get_lux(SensorLDR_pin, in, out, 16)/10.0;  //ORIGINAL
            //float ldr_read = get_lux(SensorLDR_pin, in, out, 16)/10.0;  //MEDIDA EN KLUX
            Serial.print("Lux: ");
            Serial.println(ldr_read);
            if (ldr_read == 0) ldr_read = 0.01;
            Souliss_ImportAnalog(memory_map, LDR, &ldr_read);

	}
             
}
	}		
} 

//************************************** FUNCTIONS **************************************
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
    if (DEBUG_IR) Serial.println(My_Decoder.value);//, HEX);
    //My_Decoder.DumpResults();
  }
    
}

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

void setColor(uint8_t slot, uint8_t R, uint8_t G, uint8_t B)
{
  mInput(slot+1)=R;
  mInput(slot+2)=G;
  mInput(slot+3)=B;
//  ssInput(slot)=Souliss_T1n_Coil;
  mInput(slot)=Souliss_T1n_Set;
}

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


//*************************************************************************
//Lux Calculate lux based on rawADC reading from LDR
//*************************************************************************
/*double get_lux(int RawADC0){
	double Vout=RawADC0*0.0048828125;
	const float dividerResistor = 5; // adjust this to calibrate
	int lux=500/(dividerResistor*((5-Vout)/Vout));//use this equation if the LDR is in the upper part of the divider
	//int lux=(2500/Vout-500)/dividerResistor;
	return lux;
}*/
//ULTIMA OK
/*
float get_lux(int PIN){
  //My calibrated ldr light sensor
   unsigned int lux;
  // out[] holds the values wanted in lux
  
  const unsigned int out[] = {
     0, 1, 10, 100, 300, 400, 600, 800, 1000, 1700, 2300, 3400, 4700};//, 7, 15, 21, 45, 100, 115 };
  const unsigned int out2[] = {
     7, 15, 21, 45, 100, 115 };
//     0, 1, 10, 100, 300, 400, 600, 800, 1000, 1700, 2300, 3400, 4700, 7000, 15000, 21000, 45000, 115000 };
  // in[] holds the measured analogRead() values for defined light levels
  // note: the in array should have increasing values
 

  const unsigned int in[]  = { 
    0, 30, 200, 650, 810, 850, 880, 910, 940, 950, 960, 970, 980};//, 990, 1000, 1005, 1010, 1014, 1015};
  const unsigned int in2[]  = {
    990, 1000, 1005, 1010, 1014, 1015};
//    0, 30, 200, 650, 810, 850, 880, 910, 940, 950, 960, 970, 980, 990, 1000, 1005, 1010, 1015};    
  unsigned int val=analogRead(PIN); // read raw value
  Serial.print(" val: "); Serial.print(val);
  if (val < 990)
    lux = UmultiMap(val, in, out, 13); // calculate lux
  else
    lux = UmultiMap(val, in2, out2, 6); // calculate lux

  Serial.print(" lux: "); Serial.print(lux);

  if (val < 990)
      return lux/1000.00;
  else
      return lux;
}



// note: the _in array should have increasing values
unsigned int UmultiMap(unsigned int val, const unsigned int* _in, const unsigned int* _out, byte size)
{
  // take care the value is within range
  // val = constrain(val, _in[0], _in[size-1]);
  if (val <= _in[0]) return _out[0];
  if (val >= _in[size-1]) return _out[size-1];

  // search right interval
  byte pos = 1;  // _in[0] allready tested
  while(val > _in[pos]) pos++;

  // this will handle all exact "points" in the _in array
  if (val == _in[pos]) return _out[pos];

  // interpolate in the right segment for the rest
  return map(val, _in[pos-1], _in[pos], _out[pos-1], _out[pos]);
}*/
//////////////////////////////////////////////////////////////////////////////
// Calculate lux based on rawADC reading from LDR returns value in lux/10
//////////////////////////////////////////////////////////////////////////////
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

float get_pressure(){
  boolean DEBUG_PRESSURE = 0;
  int ALTITUDE = 20; // Altitude of reading location in meters
  
  char status;
  double T,P,p0,a;

  // Loop here getting pressure readings every 10 seconds.

  // If you want sea-level-compensated pressure, as used in weather reports,
  // you will need to know the altitude at which your measurements are taken.
  // We're using a constant called ALTITUDE in this sketch:
  
  if(DEBUG_PRESSURE){
    Serial.println();
    Serial.print("provided altitude: ");
    Serial.print(ALTITUDE,0);
    Serial.print(" meters, ");
    Serial.print(ALTITUDE*3.28084,0);
    Serial.println(" feet");
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
        Serial.print("temperature: ");
        Serial.print(T,2);
        Serial.print(" deg C, ");
        Serial.print((9.0/5.0)*T+32.0,2);
        Serial.println(" deg F");
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
            Serial.print("absolute pressure: ");
            Serial.print(P,2);
            Serial.print(" mb, ");
            Serial.print(P*0.0295333727,2);
            Serial.println(" inHg");
          }
          // The pressure sensor returns abolute pressure, which varies with altitude.
          // To remove the effects of altitude, use the sealevel function and your current altitude.
          // This number is commonly used in weather reports.
          // Parameters: P = absolute pressure in mb, ALTITUDE = current altitude in m.
          // Result: p0 = sea-level compensated pressure in mb

          p0 = pressure.sealevel(P,ALTITUDE); // we're at 1655 meters (Boulder, CO)
          if(DEBUG_PRESSURE){
            Serial.print("relative (sea-level) pressure: ");
            Serial.print(p0,2);
            Serial.print(" mb, ");
            Serial.print(p0*0.0295333727,2);
            Serial.println(" inHg");
          }
          // On the other hand, if you want to determine your altitude from the pressure reading,
          // use the altitude function along with a baseline pressure (sea-level or other).
          // Parameters: P = absolute pressure in mb, p0 = baseline pressure in mb.
          // Result: a = altitude in m.

          a = pressure.altitude(P,p0);
          if(DEBUG_PRESSURE){
            Serial.print("computed altitude: ");
            Serial.print(a,0);
            Serial.print(" meters, ");
            Serial.print(a*3.28084,0);
            Serial.println(" feet");
          }
          return p0; 
        }
        else if(DEBUG_PRESSURE) Serial.println("error retrieving pressure measurement\n");
      }
      else if(DEBUG_PRESSURE) Serial.println("error starting pressure measurement\n");
    }
    else if(DEBUG_PRESSURE) Serial.println("error retrieving temperature measurement\n");
  }
  else if(DEBUG_PRESSURE) Serial.println("error starting temperature measurement\n");
 
}




