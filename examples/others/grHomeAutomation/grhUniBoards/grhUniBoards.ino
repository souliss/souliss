
// BOF preprocessor bug prevent - insert me on top of your arduino-code
#if 1
__asm volatile ("nop");
#endif


/**************************************************************************
	GRHOME - Common Souliss Network Definitions
	Architecture reference:
		|->WiFi-> SoulissApp
		|->eth-> OpenHab on RaspberryPI (192.168.1.2)
		|-----------------------------------------------------------------
		|->eth-> ROW1B1 (gw) ->485-> ROW1B2 ->485-> ROW1B3 ->485-> ROW1B4
		|->eth-> ROW2B1 ->485-> ROW2B2 ->485-> ROW2B3 ->485-> ROW2B4 
		|->eth-> BED1B1
	 	|->eth-> BED2B1 ->485-> BED2B2 ->485-> BED2B3 
	 	|->eth-> LOFTB1
***************************************************************************/

// Define the boards that we want program in order to create the appropriate sketch.
#define	TESTB1

//#define DEBUG

#if defined(DEBUG)
	#define MaCaco_DEBUG_INSKETCH
	#define MaCaco_DEBUG 	1
	#define VNET_DEBUG_INSKETCH
	#define VNET_DEBUG 		1
#endif


#include "grhBoardsSetup.h"

#if defined(DHT_SENSOR)
	#define TEMPERATURE			(DHT_SENSOR+1)
	#define TEMPERATURE_1		(DHT_SENSOR+2)
	#define HUMIDITY			(DHT_SENSOR+3)
	#define HUMIDITY_1			(DHT_SENSOR+4)
#endif

#if defined(BOARD_PRODINO_V2)

	#if defined(SOULISS_GATEWAY)
		#include "bconf/DINo_v2.h"
		#include "conf/Gateway.h"


	#elif defined(SOULISS_BRIDGE)
		#include "bconf/DINo_v2_EthernetBridge_RS485.h"

	#else

		#if defined(RS485_ADDRESS)
			#include "bconf/DINo_v2_RS485.h"
		#elif defined(IP_ADDRESS_4) // no RS485 address defined, so this is a simple IP Peer
			#include "bconf/DINo_v2.h"
		#endif
	
	#endif

#elif defined(BOARD_MEGA_ETH)

	#include "bconf/StandardArduino.h"
	#include "conf/ethW5100.h"
	#include "conf/Gateway.h"

#endif


#include "grhSoulissCommon.h"
#include "Souliss.h"
#include "SPI.h"
#include "grhSoulissCustom.h"
#include "NTC.h"
#include <DHT.h>	

#if defined(BOARD_PRODINO_V2)
	#include "HW_Setup_DINo_v2.h"
#elif defined(BOARD_MEGA_ETH)
	#include "HW_Setup_Mega_2560.h"
#endif


#if defined(DHT_SENSOR)
	DHT dht(ONE_WIRE_PIN, DHT22);
	float th=0;
#endif

SOULISS_GLOBALS;

void setup()
{
	#if defined(BOARD_MEGA_ETH)
		DEFINE_PIN_MODE
	#endif

	#if defined(DEBUG)
    // Open serial communications and wait for port to open:
    Serial.begin(9600);
    while (!Serial) {
      ; // wait for serial port to connect. Needed for Leonardo only. If need debug setup() void.
    }
    Serial.println("Serial started to DEBUG");
	#endif

	#if defined(BOARD_PRODINO_V2)
		DINo_v2_HW_Setup();
	#elif defined(BOARD_MEGA_ETH)
		InitMEGA();
	#endif

	
	#if defined(IP_ADDRESS_4)
		uint8_t ip_address[4]  = {IP_ADDRESS_1, IP_ADDRESS_2, IP_ADDRESS_3, IP_ADDRESS_4};
	  	uint8_t subnet_mask[4] = {255, 255, 255, 0};
	  	uint8_t ip_gateway[4]  = {IP_ADDRESS_1, IP_ADDRESS_2, IP_ADDRESS_3, IP_ADDRESS_4_NETGW};

		Souliss_SetIPAddress(ip_address, subnet_mask, ip_gateway);
	#else
		#if defined(BOARD_PRODINO_V2) // it is a prodino and not using ehternet, shut it off !
			PowerDownEthernet();
		#endif
	#endif


	#if defined(RS485_ADDRESS)
		Souliss_SetAddress(RS485_ADDRESS, SOULISS_SUBNET_MASK, RS485_SUPERNODE);
	#endif


	#if defined(SOULISS_GATEWAY)

		SetAsGateway(IP_ADDRESS_4);

		SetAsPeerNode(IP_ADDRESS_ROW1B1,1);
		SetAsPeerNode(RS485_ADDRESS_ROW1B2,2);
		SetAsPeerNode(RS485_ADDRESS_ROW1B3,3);
		SetAsPeerNode(RS485_ADDRESS_ROW1B4,4);

		SetAsPeerNode(IP_ADDRESS_ROW2B1,5);
		SetAsPeerNode(RS485_ADDRESS_ROW2B2,6);
		SetAsPeerNode(RS485_ADDRESS_ROW2B3,7);
		SetAsPeerNode(RS485_ADDRESS_ROW2B4,8);

		SetAsPeerNode(IP_ADDRESS_BED1B1,9);

		SetAsPeerNode(IP_ADDRESS_BED2B1,10);
		SetAsPeerNode(RS485_ADDRESS_BED2B2,11);
		SetAsPeerNode(RS485_ADDRESS_BED2B3,12);

//		SetAsPeerNode(IP_ADDRESS_LOFTB1,12);

//		SetAsPeerNode(IP_ADDRESS_TESTB1,13);


	#endif

	SOULISS_DEFINE_TYPICALS;

	#if defined(DHT_SENSOR)
		Set_Temperature(TEMPERATURE);
		Set_Humidity(HUMIDITY);
		dht.begin();
	#endif

} 

void loop()
{ 
	EXECUTEFAST() {						
		UPDATEFAST();	
		
		FAST_30ms() 
		{

			SOULISS_READ_INPUTS;
		} 

		FAST_50ms() 
		{
			SOULISS_PROCESS_LOGICS;

			#if defined(DHT_SENSOR)
				Logic_Humidity(HUMIDITY);
				Logic_Temperature(TEMPERATURE);
			#endif

			SOULISS_SET_OUTPUTS;
		}
		
		// Here we process all communication with other nodes
		#if defined(SOULISS_GATEWAY)
			FAST_GatewayComms();		
		#else		
		FAST_PeerComms();

		// At first runs, we look for a gateway to join
		// (not needed if the board is listed in the gateway with SetAsPearNode) 
		START_PeerJoin();
		
		// Periodically check if the peer node has joined the gateway
		FAST_1110ms() {
			if(JoinInProgress())	// If join is in progress, toggle the LED at every turn
				ToggleLED();
			else
				TurnOnLED();		// Once completed, turn ON				
		}
		#endif		

		FAST_2110ms()
		{
			SOULISS_PROCESS_TIMERS;

			#if defined(DHT_SENSOR)
				th = dht.readHumidity();
				ImportAnalog(HUMIDITY, &th);

				th = dht.readTemperature();
				ImportAnalog(TEMPERATURE, &th);	
				
			#endif		
		}		
	}
	
	EXECUTESLOW() {	
		UPDATESLOW();

		#if !defined(SOULISS_GATEWAY)
			// Here we periodically check for a gateway to join
			// (not needed if the board is listed in the gateway with SetAsPearNode)
			SLOW_PeerJoin();		
		#endif


		}
} 

