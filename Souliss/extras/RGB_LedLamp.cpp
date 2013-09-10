#include "src/IRremote_light/IRremoteInt_light.h"
#include "src/IRremote_light/IRremote_light.h"
#include "src/IRremote_light/IRremote_light.cpp"

#include "Typicals.h"

//IR_Remote lite library
IRsend ts;

/**************************************************************************
/*!
	Send infrared codes to the RGB device, these mapping is device dependent
	and shall be updated based on the used device.
	
	These are standard RGB remote commands, based on NEC IR protocol
*/	
/**************************************************************************/
void sendRGBRemote(U8 mappedVal)
{
	ts.enableIROut(38);				//38Khz IR freq.
	
	switch(mappedVal)
	{
		case Souliss_T1n_RGBLamp_OnCmd:
			ts.sendNEC(0xFFE01F,32);
		break;
		
		case Souliss_T1n_RGBLamp_OffCmd:
			ts.sendNEC(0xFF609F,32);
		break;
		
		case Souliss_T1n_RGBLamp_BrightUp:
			ts.sendNEC(0xFFA05F,32);
		break;
		
		case Souliss_T1n_RGBLamp_BrightDown:
			ts.sendNEC(0xFF20DF,32);
		break;
		
		case Souliss_T1n_RGBLamp_R:
			ts.sendNEC(0xFF906F,32);
		break;	
		
		case Souliss_T1n_RGBLamp_G:
			ts.sendNEC(0xFF10EF,32);
		break;		
		
		case Souliss_T1n_RGBLamp_B:
			ts.sendNEC(0xFF50AF,32);
		break;	
		
		case Souliss_T1n_RGBLamp_W:
			ts.sendNEC(0xFFD02F,32);
		break;	
		
		case Souliss_T1n_RGBLamp_Flash:
			ts.sendNEC(0xFFF00F,32);
		break;	
		
		case Souliss_T1n_RGBLamp_Strobe:
			ts.sendNEC(0xFFE817,32);
		break;	
		
		case Souliss_T1n_RGBLamp_Fade:
			ts.sendNEC(0xFFD827,32);
		break;	
		
		case Souliss_T1n_RGBLamp_Smooth:
			ts.sendNEC(0xFFC837,32);
		break;	
		
		case Souliss_T1n_RGBLamp_R2:
			ts.sendNEC(0xFFB04F,32);
		break;	
		
		case Souliss_T1n_RGBLamp_R3:
			ts.sendNEC(0xFFA857,32);
		break;	
		
		case Souliss_T1n_RGBLamp_R4:
			ts.sendNEC(0xFF9867,32);
		break;	
		
		case Souliss_T1n_RGBLamp_R5:
			ts.sendNEC(0xFF8877,32);
		break;	
		
		case Souliss_T1n_RGBLamp_G2:
			ts.sendNEC(0xFF30CF,32);
		break;		
		
		case Souliss_T1n_RGBLamp_G3:
			ts.sendNEC(0xFF28D7,32);
		break;	
		
		case Souliss_T1n_RGBLamp_G4:
			ts.sendNEC(0xFF18E7,32);
		break;	
		
		case Souliss_T1n_RGBLamp_G5:
			ts.sendNEC(0xFF08F7,32);
		break;	
		
		case Souliss_T1n_RGBLamp_B2:
			ts.sendNEC(0xFF708F,32);
		break;	
		
		case Souliss_T1n_RGBLamp_B3:
			ts.sendNEC(0xFF6897,32);
		break;	
		
		case Souliss_T1n_RGBLamp_B4:
			ts.sendNEC(0xFF58A7,32);
		break;	
		
		case Souliss_T1n_RGBLamp_B5:
			ts.sendNEC(0xFF48B7,32);
		break;						
	}	
}
