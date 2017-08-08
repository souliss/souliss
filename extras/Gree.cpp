#include <avr/pgmspace.h>

#include "Souliss.h"
#include "Typicals.h"
#include "Tadiran/tadiran.h"

#define tempmask 						0xF	   //1111
#define funmask 						0xF0   //11110000
#define fanmask 						0x700  //11100000000
#define fanAutoMask						
#define swirlmask 						0x800  //100000000000

/**************************************************************************

*/	
/**************************************************************************/
void sendGree(U8 *memory_map, U8 slot){

	IRsend irsend;
	int temperature = 16;
	int mode = MODE_cold; // 0-Auto | 1-Cold | 2-Dry | 3-Fan | 4-Heat
	int fanspeed = FAN_auto; //0-Auto | 1-Low | 2-Medium | 3-High
	
	
	// the Tairan code generator object
	Tadiran tadiran(mode, fanspeed, temperature, STATE_on);
	
	unsigned long data = memory_map[MaCaco_OUT_s + slot];
	data<<=8;
			
	//retrieve second 2 bytes
	data += memory_map[MaCaco_OUT_s + slot + 1];
	
	
	if (data & 0x8000){//if first bit of input=1 TURN ON

		tadiran.setState(STATE_on);
	}
	 else if (data<<1 & 0x8000) {//if second bit of input=1 SHUT OFF
		tadiran.setState(STATE_off);
		irsend.sendRaw(tadiran.codes, TADIRAN_BUFFER_SIZE, 38);
		memory_map[MaCaco_OUT_s + slot] = 0;	
		memory_map[MaCaco_OUT_s + slot + 1] =  0;	
		return;
	} 
	
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
	
	//Fan has only 24 degrees
	if (fun == Souliss_T3n_AirCon_Fan)
		temperat = 24;
		
	//Dry has only AUTO fan setting
	if (fun == Souliss_T3n_AirCon_Dry)
		fan = 7;
	
	switch(fan)
	{
		case 7:
			tadiran.setFan(FAN_auto);
			break;
		case 5:
			tadiran.setFan(FAN_1);
			break;
		case 6:
			tadiran.setFan(FAN_2);
			break;
		case 2:
			tadiran.setFan(FAN_3);
			break;
	}
	
	switch(fun)
	{
		case 11:
			tadiran.setMode(MODE_dry);
			break;
		case 7:
			tadiran.setMode(MODE_cold);
			break;
		case 3:
			tadiran.setMode(MODE_fan);
			break;
		case 13:
			tadiran.setMode(MODE_heat);
			break;
		case 15:
			tadiran.setMode(MODE_auto);
			break;
	}
	
	switch(temperat)
	{
		case 1:
			tadiran.setTemeprature(23);
			break;
		case 2:
			tadiran.setTemeprature(27);
			break;
		case 3:
			tadiran.setTemeprature(19);
			break;
		case 4:
			tadiran.setTemeprature(29);
			break;
		case 5:
			tadiran.setTemeprature(21);
			break;
		case 6:
			tadiran.setTemeprature(25);
			break;
		case 7:
			tadiran.setTemeprature(17);
			break;
		case 8:
			tadiran.setTemeprature(30);
			break;
		case 9:
			tadiran.setTemeprature(22);
			break;
		case 10:
			tadiran.setTemeprature(26);
			break;
		case 11:
			tadiran.setTemeprature(18);
			break;
		case 12:
			tadiran.setTemeprature(28);
			break;
		case 13:
			tadiran.setTemeprature(20);
			break;
		case 14:
			tadiran.setTemeprature(24);
			break;
		case 15:
			tadiran.setTemeprature(16);
			break;
	}
	
	return tadiran.getTemeprature();
	irsend.sendRaw(tadiran.codes, TADIRAN_BUFFER_SIZE, 38);
}
