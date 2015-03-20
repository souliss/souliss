#include <avr/pgmspace.h>
#include "src/IRremote_light/IRremoteInt_light.h"
#include "src/IRremote_light/IRremote_light.h"
#include "src/IRremote_light/IRremote_light.cpp"

#include "Souliss.h"
#include "Typicals.h"

//These are Samsung MH026FB specific
#define tempmask 						0xF
#define funmask 						0xF0
#define fanmask 						0x700
#define swirlmask 						0x800

const byte charSet[] PROGMEM = { 0x0, 0x8, 0x4, 0xc, 0x2, 0xA , 0x6, 0xE, 0x1, 0x9,0x5, 0xD, 0x3,0xB,0x7,0xF };
	
//IR_Remote lite library
IRsend irsend;

/**
Counts number of 1 in a long
*/
int NumberOfSetBits(long i)
{
    i = i - ((i >> 1) & 0x55555555);
    i = (i & 0x33333333) + ((i >> 2) & 0x33333333);
    return (((i + (i >> 4)) & 0x0F0F0F0F) * 0x01010101) >> 24;
}

/**
 Reverse the bit in a byte via a lookup table
 Bit reversal lookup table
 0100 --> 0010
 //TODO use progmem
*/
char revbits(int n)
{

	return pgm_read_byte(&charSet[n]); 
	//return reversed[n];
}

/**
	Compute Samsung crappy checksum
	Command is split between data and data2
	
	 Samsung checksum
	 1. Count the number of 1 bits in all the bytes except #2 (checksum)
	 2. Compute count mod 15. If the value is 0, use 15 instead.
	 3. Take the value from 2, flip the 4 bits, and reverse the 4 bits.
	 4. The checksum is Bn where n is the value from the previous step.

	 Note that step 2 is mod 15, not mod 16 (which you might expect). I
	 don't know why 15 is used as a special case instead of 0.

	For step 3, the results in order are F, 7, B, 3, D, 5, 9, 1, E, 6, A,
	2, C, 4, 8, 0. Note that these are exactly the same as the values you
	got for byte 5 (temperature). This suggests that the reversed /
	inverted bits are more "fundamental". 
	
	Thanks to Ken Shirriff
*/
long computeChecksum(long data, long data2){
	char temp;
	temp = NumberOfSetBits(data2);
	temp +=NumberOfSetBits(data);
	temp = temp%15;
	if (temp == 0)
		temp = 15;
	#ifdef DEBUG
	Serial.print("NUMBITS SET:");
	Serial.println(temp, HEX);
	#endif
	temp = ~temp;
	temp -=0xF0;
	#ifdef DEBUG
	Serial.print("INVERTED:");
	Serial.println(temp, HEX);
	Serial.print("REVERSED:");
	Serial.println(revbits(temp), HEX);
	#endif
	
	return 0xB0 + revbits(temp);
}

/**************************************************************************
/*!
	Remap Souliss AirCon data with device specific one (Samsung MH026FB).
	You'll have to re-implement this function to support your
	Air Conditioner. Souliss INPUT Data is:
	 
	 0xABCD
	 A = 4 bit mask (XYZT X= powerON, Y PowerOFF, ionizer (airclean) , pwrsave)
	 B = Fan/Swirl (1bit switch + 3bits for four possible fan values)
	 C = Function (auto,cool,dry,fan,heat)
	 D = Temperature (from 16 to 30, see .h)
	
 	 ---------------------------------------------------------------
	 OUTPUT (Samsung specific)
	 Codice dati da inviare: 7F       XX       80       71       F    1    7    7    F0
							 const    check    swirl    const    ion  tem  fan  fun  const
	 DEFAULT				 00000000 00000000 00000000 01110001 0000 0000 0000 0000 00000000

*/	
/**************************************************************************/
void sendMH026FB(U8 *memory_map, U8 slot){

	unsigned long data = memory_map[MaCaco_OUT_s + slot];
	data<<=8;
			
	//retrieve second 2 bytes
	data += memory_map[MaCaco_OUT_s + slot + 1];

	long part1=0;
	long part2=0;
	
	irsend.enableIROut(38);
	
	if (data & 0x8000){//if first bit of input=0 TURN ON

		//3 bursts: 2 constants and go on with command
		irsend.sendSamsung(0xBFB20FFF, 0xFFFFF0);
		irsend.sendSamsung(0x7FB40FFF, 0xFFFFFF);
		
	}
	else if (data<<1 & 0x8000) {//if second bit of input=0 SHUT OFF
		//send a generic POWEROFF command, then exit
		irsend.sendSamsung(0xBFB20FFF, 0xFFFFFC);
		irsend.sendSamsung(0x7FB40FFF, 0xFFFFFF);
		irsend.sendSamsung(0x7FB08A71, 0xF84FFC);
		memory_map[MaCaco_OUT_s + slot] = 0;	
		memory_map[MaCaco_OUT_s + slot + 1] =  0;	
		return;
	}
	else {
		//Comando da due bursts, primo costante
		irsend.sendSamsung(0xBFB20FFFUL ,0xFFFFF0UL );
	}
		//starting values
		part1 =  0x7f000000;
		part2 =  0x0000F0;
		//last part1 const (andrebbe 11 -> turbo) e 01->Powersave
		unsigned int powerMode = Souliss_T3n_AirCon_Normal;
		//FAN is bit 2,3,4 of 2nd BYTE of INPUT
		char fan = (data & fanmask)>>8;
		//FUN = 3rd byte of INPUT
		char fun = (data & funmask)>>4;

		//TEMP = 4th byte of INPUT
		long temperat = data & tempmask;
		//swirl OFF by default
		unsigned int swirl = 0x80;
		
		//swirl (if deflector has to swing)
		if (data & swirlmask){
			swirl=0x8A;
		}
		
		//Echo bit Toggle, only in cool mode
		if ((data<<3 & 0x8000) && (fun == Souliss_T3n_AirCon_Cool))
			powerMode = Souliss_T3n_AirCon_Eco;
		
		//PART2 
		
		//OPT if terzo bit = 1 IONIZER
		if (data<<2 & 0x8000)
			part2 += 0x700000;//Ion ON
		else
			part2 += 0xF00000;//F = Ion OFF
		
		
		//Fan has only 24 degrees
		if (fun == Souliss_T3n_AirCon_Fan)
			temperat = Souliss_T3n_AirCon_T24C;
			
		//Dry has only AUTO fan setting
		if (fun == Souliss_T3n_AirCon_Dry)
			fan = Souliss_T3n_AirCon_Auto;
			
		//Auto mode adjust: fan and options are fixed
		if (fun == Souliss_T3n_AirCon_Auto){
			fan = 0x4;
			//swirl=0x8A;
			powerMode = Souliss_T3n_AirCon_Normal;
		}
		
		part1 += swirl<<8;
		part1 += powerMode;
		//temperature on sixth byte
		part2 += temperat<<16;
		
		part2 += fan<<12;
		part2 += fun<<8;
		
		//has to be last call
		part1 += computeChecksum(part1, part2)<<16;
		
		//send command
		irsend.sendSamsung(part1, part2);
}