/**************************************************************************
	Souliss - RGB LED Strip
	
	Control and RGB LED or a Strip using the PWM, fade in/out and flash 
	effects are available as the Android interface.
 
	Run this code on one of the following boards:
	  - Arduino Ethernet (W5100) 
	  -	Arduino with Ethernet Shield (W5100)
	  
	As option you can run the same code on the following, just changing the
	relevant configuration file at begin of the sketch
	  -	Arduino with ENC28J60 Ethernet Shield
	  - Arduino with W5200 Ethernet Shield
	  - Arduino with W5500 Ethernet Shield 
	
***************************************************************************/


// Configure the framework
#include "bconf/StandardArduino.h"			// Use a standard Arduino
#include "conf/ethW5100.h"					// Ethernet through Wiznet W5100
#include "conf/Gateway.h"					// The main node is the Gateway, we have just one node
#include "conf/Webhook.h"					// Enable DHCP and DNS

// Include framework code and libraries
#include <SPI.h>
#include "Souliss.h"

#define LEDCONTROL			0				// This is the memory slot for the logic that handle the light
#define LEDRED				1				// This is the memory slot for the logic that handle the light
#define LEDGREEN			2				// This is the memory slot for the logic that handle the light
#define LEDBLUE				3				// This is the memory slot for the logic that handle the light

// This sketch will use DHCP, but a generic IP address is defined in case
// the DHCP will fail. Generally this IP address will not be used and doesn't
// need to be used in SoulissApp
IPAddress ip(192, 168, 1, 77);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);
#define myvNet_address	ip[3]

void setup()
{	
	Initialize();

	// Network configuration
	if(Ethernet.begin()==0)						// Get an address via DHCP     
        Ethernet.begin(ip, gateway, subnet);	// If DHCP fails, use a manual address
	else
        ip = Ethernet.localIP();				         
        		
	SetAsGateway(myvNet_address);				// Set this node as gateway for SoulissApp	
	Set_LED_Strip(LEDCONTROL);					// Set a logic to control a LED strip
	
	// Define inputs, outputs pins
	pinMode(2, INPUT);					// Hardware pulldown required
	pinMode(3, OUTPUT);					// Power the LED
	pinMode(5, OUTPUT);					// Power the LED
	pinMode(6, OUTPUT);					// Power the LED	
}

void loop()
{ 
	// Here we start to play
	EXECUTEFAST() {						
		UPDATEFAST();	

		// Execute the code every 1 time_base_fast		
		FAST_10ms() {
		
			// Use Pin2 as command
			DigIn(2, Souliss_T1n_ToggleCmd, LEDCONTROL);		
			
			// Execute the logic that handle the LED
			Logic_LED_Strip(LEDCONTROL);

			// Use the output values to control the PWM
			analogWrite(3, mOutput(LEDRED));
			analogWrite(5, mOutput(LEDGREEN));
			analogWrite(6, mOutput(LEDBLUE));

			// Just process communication as fast as the logics
			ProcessCommunication();
		}			
	}
	EXECUTESLOW()
	{	
		UPDATESLOW();

		SLOW_10s() 	{
		
			// The timer handle timed-on states
			Timer_LED_Strip(LEDCONTROL);						
		} 	  
	}		
} 