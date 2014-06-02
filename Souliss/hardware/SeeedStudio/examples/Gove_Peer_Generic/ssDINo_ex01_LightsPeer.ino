/**************************************************************************

	
***************************************************************************/

// Souliss can run over different hardware, here we define the board that we are using,
// the parameters and the available configuration are listed in QuickCfg.h file located
// in /conf folder.
//
// Define an Arduino using the USART as communication interface, the USART is used for 
// the communication via XBee radio.
#include "bconf/inSketch.h"			
#define	QC_BOARDTYPE	0x43	// Arduino with USART 	
#define	QC_GATEWAYTYPE	0x00	// Disabled
#define	QC_INTERFACE	0x00	// Disabled

#include "Souliss.h"
#include "SpeakEasy.h"
#include <SPI.h>

void setup()
{	
	
	// This board (peer) request an address to the gateway one at runtine, no need
	// to configure any parameter here
	SetDynamicAddressing();
}

void loop()
{ 
	// Here we start to play
	EXECUTEFAST() {						
		UPDATEFAST();	
		
		FAST_50ms() {	// We process the logic and relevant input and output every 50 milliseconds
		
		} 
		
		// Here we process all communication with other nodes
		FAST_PeerComms();
		
		// At first runs, we look for a gateway to join
		START_PeerJoin();
		
		// Periodically check if the peer node has joined the gateway
		FAST_1110ms() {
			if(JoinInProgress())	// If join is in progress, toogle the LED at every turn
				ToogleLED();
			else
				TurnOnLED();		// Once completed, turn ON
		}		
		
	}
	
	EXECUTESLOW() {	
		UPDATESLOW();
		
		// Here we periodically check for a gateway to join
		SLOW_PeerJoin();		
	}
} 