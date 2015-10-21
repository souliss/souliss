/**************************************************************************
	Souliss Support for Authometion LYT88
    Copyright (C) 2014  Veseo
	
***************************************************************************/

/**************************************************************************
	Souliss Home Automation
    Copyright (C) 2014  Veseo

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
	
	Originally developed by Dario Di Maio
	
***************************************************************************/
/*!
    \file 
    \ingroup
*/
#include "Typicals.h"
#include "GetConfig.h"
#include "LYT8266.h"

#define	PIN_RED		13
#define	PIN_GREEN	12
#define	PIN_BLUE	14
#define	PIN_WHITE	2
#define	PIN_ENABLE	15

#define FADETIME	5
#define	FADEENABLE	1

// Store the old value 
U8 _red=0, _green=0, _blue=0, _white=0;

/**************************************************************************
/*!
	Init LYT
*/	
/**************************************************************************/
void InitLYT()
{
	// Switch pins as outputs
	pinMode(PIN_RED		,OUTPUT);
	pinMode(PIN_GREEN	,OUTPUT);
	pinMode(PIN_BLUE	,OUTPUT);
	pinMode(PIN_WHITE	,OUTPUT);	
	pinMode(PIN_ENABLE	,OUTPUT);	

	// Set the PWM frequency
	analogWriteFreq(500);

	// Switch all outputs off	
	analogWrite(PIN_RED, 	0);
	analogWrite(PIN_GREEN, 	0);
	analogWrite(PIN_BLUE, 	0);	
	analogWrite(PIN_WHITE, 	0);	

	// Enable outputs
	digitalWrite(PIN_ENABLE, HIGH);	
}

/**************************************************************************
/*!
	Turn ON the LEDs
*/	
/**************************************************************************/
void LYTOn()
{
	// Enable outputs
	digitalWrite(PIN_RED, HIGH);	
	digitalWrite(PIN_GREEN, HIGH);
	digitalWrite(PIN_BLUE, HIGH);	
}

/**************************************************************************
/*!
	Turn OFF all the LEDs (this goes directly to the I/O, don't use in your 
	sketch)
*/	
/**************************************************************************/
void LYTOff()
{
	// Fade off and turn off the LEDs
	while((_red > 0) || (_green > 0) || (_blue > 0) || (_white > 0))
	{
		// Reduce the values till zero
		if(_red) 	_red--;
		if(_green) 	_green--;
		if(_blue) 	_blue--;
		if(_white) 	_white--;		

		analogWrite(PIN_RED,  _red);
		analogWrite(PIN_GREEN,_green); 
		analogWrite(PIN_BLUE, _blue);
		analogWrite(PIN_WHITE,_white); 

		// Delay to have a visible fade
		delay(FADETIME);
	}

}

/**************************************************************************
/*!
	Turn ON the White LED and set the brightness (this goes directly to the 
	I/O, don't use in your sketch)
*/	
/**************************************************************************/
void LYTWhite(U8 brightness, U8 fade_on=0)
{
	// The RGB and W LED cannot be ON at same time
	_red 	= 0;
	_green 	= 0;
	_blue 	= 0;
	analogWrite(PIN_RED,  	_red);
	analogWrite(PIN_GREEN,	_green); 
	analogWrite(PIN_BLUE, 	_blue);	
	
	// Set the output
	while(_white != brightness)
	{
		if(_white < brightness)			_white++;	// Increase the brightness
		else if(_white > brightness)	_white--;	// Decrease the brightness

		analogWrite(PIN_WHITE, _white*4);

		// Delay to have a visible fade
		if(fade_on) delay(FADETIME);
	}
}

/**************************************************************************
/*!
	Turn ON the R,G,B LEDs and set the brightness (this goes directly to the 
	I/O, don't use in your sketch)
*/	
/**************************************************************************/
void LYTColor(U8 red, U8 green, U8 blue, U8 fade_on=0)
{
	// The RGB and W LED cannot be ON at same time
	_white = 0;
	analogWrite(PIN_WHITE,	_white); 
	
	// Set the output
	while((_red != red) || (_green != green) || (_blue != blue))
	{
		if(_red < red)			_red++;		// Increase the brightness
		else if(_red > red)		_red--;     // Decrease the brightness

		if(_green < green)		_green++;	// Increase the brightness
		else if(_green > green)	_green--;   // Decrease the brightness

		if(_blue < blue)		_blue++;	// Increase the brightness
		else if(_blue > blue)	_blue--;    // Decrease the brightness

		analogWrite(PIN_RED, 	_red*4);
		analogWrite(PIN_GREEN,	_green*4);
		analogWrite(PIN_BLUE, 	_blue*4);

		// Delay to have a visible fade
		if(fade_on) delay(FADETIME);
	}
}

/**************************************************************************
/*!
	Pulse a bit
*/	
/**************************************************************************/
void LYTPulse()
{
	for(U8 i=0;i<3;i++)
	{
		LYTColor(0x50, 0x10, 0x00, FADEENABLE);
		delay(50);
		LYTColor(0x00, 0x00, 0x00, FADEENABLE);		
	}

	delay(3000);
}

/**************************************************************************
/*!
	Define the use of Typical 16 : RGB LED Strip
	
	Modified for LYT Lamps by Authometion
*/	
/**************************************************************************/
void Souliss_SetLYTLamps(U8 *memory_map, U8 slot)
{
	memory_map[MaCaco_TYP_s + slot] = Souliss_T16;
	memory_map[MaCaco_AUXIN_s + slot] = LYT_MedBright;
	memory_map[MaCaco_TYP_s + slot + 1] = Souliss_TRL;
	memory_map[MaCaco_TYP_s + slot + 2] = Souliss_TRL;
	memory_map[MaCaco_TYP_s + slot + 3] = Souliss_TRL;
}

/**************************************************************************
/*!
	Typical 16 : RGB LED Strip

	Modified for LYT Lamps by Authometion	
	
		Handle one RGB LED strip based on software commands, with automatic 
		fade effects and "Good Night" feature. It use four (4) SLOT.					
		
		Hardware Command:
			
			If using a monostable wall switch (press and spring return), 
			each press will toggle the output status.		
				#define Souliss_T1n_ToggleCmd		0x01
				#define Souliss_T1n_BrightUp		0x10
				#define Souliss_T1n_BrightDown		0x20
			
			If using a bistable wall switch (press without return), the two
			switch position can be associated with the ON and OFF commands
				#define Souliss_T1n_OnCmd			0x02
				#define Souliss_T1n_OffCmd			0x03
					
		Software Commands:		
			
			From any available software interface, these commands will turn
			the light ON and OFF.
				#define Souliss_T1n_OnCmd			0x02
				#define Souliss_T1n_OffCmd			0x03
				#define Souliss_T1n_BrightUp		0x10
				#define Souliss_T1n_BrightDown		0x20
				#define Souliss_T1n_Flash			0x21
			
			Color can be set using the following command in the first slot,
			and giving the R, B and G values into the next three slots
				#define Souliss_T1n_Set				0x22
			
		Good Night:
			
			The light goes off with a long time fade effect, this feature needs the 
			external timer. Good Night mode is activated using if the input command
			is greater than
				#define Souliss_T1n_Timed			0x30
			
			A Good Night command can be sent via software or hardware commands,
			in case of hardware commands is usually associated to a long press
			of a monostable wall switch.
			
		Command recap, using: 
		-  1(hex) as command, toggle the output 
		-  2(hex) as command, the output move to ON
		-  4(hex) as command, the output move to OFF
		-  10(hex) as command, the light bright is increased
		-  20(hex) as command, the light bright is decreased
		-  21(hex) as command, the light flash on and off at each cycle
		-  22(hex) as command, set the color (needs R, G and B values)
		- >30(hex) as command, time the output to ON
		-  0(hex) as command, no action

		Output status,
		-  0(hex) for output OFF,
		-  1(hex) for output ON.
									
		The next three slots contains the R, G and B values applied to the
		light.		
	
*/	
/**************************************************************************/
U8 Souliss_Logic_LYTLamps(U8 *memory_map, U8 slot, U8 *trigger)
{
	U8 i_trigger=0;														// Internal trigger

	// Look for input value, update output. If the output is not set, trig a data
	// change, otherwise just reset the input
	
	if(memory_map[MaCaco_IN_s + slot] == Souliss_T1n_RstCmd)
		return 0;

	// Set a new color
	if (memory_map[MaCaco_IN_s + slot] == Souliss_T1n_Set)
	{	
		// Set the new color
		for(U8 i=1;i<4;i++)
		{
			memory_map[MaCaco_OUT_s + slot + i] = memory_map[MaCaco_IN_s + slot + i];
			memory_map[MaCaco_AUXIN_s + slot + i] = memory_map[MaCaco_IN_s + slot + i];
			memory_map[MaCaco_IN_s + slot + i] = Souliss_T1n_RstCmd;
		}
		
		// If the color is set as white
		if( ((memory_map[MaCaco_OUT_s + slot + 1]  >= 0xF0) &&
			( memory_map[MaCaco_OUT_s + slot + 2]  >= 0xF0) &&
			( memory_map[MaCaco_OUT_s + slot + 3]  >= 0xF0)) )			
		{
			// Set middle brightness
			memory_map[MaCaco_AUXIN_s + slot] = BRIGHT_DEFAULT;

			LYTWhite(memory_map[MaCaco_AUXIN_s + slot]);
		}		
		else // Set the color
		{
			// Set the brightness value as the high between R, G and B
			memory_map[MaCaco_AUXIN_s + slot] = memory_map[MaCaco_AUXIN_s + slot + 1];
			if(memory_map[MaCaco_AUXIN_s + slot + 2] > memory_map[MaCaco_AUXIN_s + slot])
				memory_map[MaCaco_AUXIN_s + slot] = memory_map[MaCaco_AUXIN_s + slot + 2];
			if(memory_map[MaCaco_AUXIN_s + slot + 3] > memory_map[MaCaco_AUXIN_s + slot])
				memory_map[MaCaco_AUXIN_s + slot] = memory_map[MaCaco_AUXIN_s + slot + 3];

			// Set the color
			LYTColor( 	memory_map[MaCaco_OUT_s + slot + 1], 
						memory_map[MaCaco_OUT_s + slot + 2], 
						memory_map[MaCaco_OUT_s + slot + 3]);					
		}
		
		memory_map[MaCaco_OUT_s + slot] = Souliss_T1n_OnCoil;			// Switch on the output
		memory_map[MaCaco_IN_s + slot] = Souliss_T1n_RstCmd;			// Reset			
	}
	else if (memory_map[MaCaco_IN_s + slot] == Souliss_T1n_ToggleCmd)		// Toggle Command
	{
		// Toggle the actual status of the light
		if(memory_map[MaCaco_OUT_s + slot] == Souliss_T1n_OffCoil)		
			memory_map[MaCaco_IN_s + slot] = Souliss_T1n_OnCmd;			
		else if(memory_map[MaCaco_OUT_s + slot] == Souliss_T1n_OnCoil)
			memory_map[MaCaco_IN_s + slot] = Souliss_T1n_OffCmd;
		else
			memory_map[MaCaco_IN_s + slot] = Souliss_T1n_RstCmd;	
	}
	else if (memory_map[MaCaco_IN_s + slot] == Souliss_T1n_OffCmd)		// Off Command
	{
		memory_map[MaCaco_OUT_s + slot] = Souliss_T1n_OffCoil;		// Switch off the light state

		i_trigger = Souliss_TRIGGED;								// Trig the change

		// Save the actual color
		for(U8 i=1;i<4;i++)
			memory_map[MaCaco_OUT_s + slot + i] = 0;
		
		// Send the off command
		LYTOff();
		
		// Reset the input
		memory_map[MaCaco_IN_s + slot] = Souliss_T1n_RstCmd;		
	}
	else if (memory_map[MaCaco_IN_s + slot] == Souliss_T1n_OnCmd)
	{
		i_trigger = Souliss_TRIGGED;									// Trig the change
	
		memory_map[MaCaco_OUT_s + slot] = Souliss_T1n_OnCoil;			// Switch on the output
		
		// If there were no color set, use a light white
		if((memory_map[MaCaco_AUXIN_s + slot + 1] == 0) && (memory_map[MaCaco_AUXIN_s + slot + 2] == 0) && (memory_map[MaCaco_AUXIN_s + slot + 3] == 0))
		{
			for(U8 i=1;i<4;i++)
				memory_map[MaCaco_AUXIN_s + slot + i] = 0xFF;				// Set a light white

			memory_map[MaCaco_AUXIN_s + slot] = BRIGHT_DEFAULT;				// Set default brightness
		}

		// If the color is set as white
		if( ((memory_map[MaCaco_AUXIN_s + slot + 1]  >= 0xF0) &&
			( memory_map[MaCaco_AUXIN_s + slot + 2]  >= 0xF0) &&
			( memory_map[MaCaco_AUXIN_s + slot + 3]  >= 0xF0))  )			
		{
			// Set the output
			for(U8 i=1;i<4;i++)
				memory_map[MaCaco_OUT_s + slot + i] = memory_map[MaCaco_AUXIN_s + slot + i];

			LYTWhite(memory_map[MaCaco_AUXIN_s + slot], FADEENABLE);		
		}		
		else // Set the color
		{	
			// Get the base brightness
			uint8_t base_bright =  memory_map[MaCaco_AUXIN_s + slot + 1];
			if(memory_map[MaCaco_AUXIN_s + slot + 2] > base_bright)
				base_bright =  memory_map[MaCaco_AUXIN_s + slot + 2];
			if(memory_map[MaCaco_AUXIN_s + slot + 3] > base_bright)
				base_bright =  memory_map[MaCaco_AUXIN_s + slot + 3];

			float r = memory_map[MaCaco_AUXIN_s + slot + 1] + (memory_map[MaCaco_AUXIN_s + slot] - base_bright);
			float g = memory_map[MaCaco_AUXIN_s + slot + 2] + (memory_map[MaCaco_AUXIN_s + slot] - base_bright);
			float b = memory_map[MaCaco_AUXIN_s + slot + 3] + (memory_map[MaCaco_AUXIN_s + slot] - base_bright);

			if(r > (0xF0 - BRIGHT_STEP -1))	memory_map[MaCaco_OUT_s + slot + 1] = (0xF0 - BRIGHT_STEP -1);
			else if(r < 0)					memory_map[MaCaco_OUT_s + slot + 1] = 0;
			else							memory_map[MaCaco_OUT_s + slot + 1] = r;

			if(g > (0xF0 - BRIGHT_STEP -1))	memory_map[MaCaco_OUT_s + slot + 2] = (0xF0 - BRIGHT_STEP -1);
			else if(g < 0)					memory_map[MaCaco_OUT_s + slot + 2] = 0;
			else							memory_map[MaCaco_OUT_s + slot + 2] = g;

			if(b > (0xF0 - BRIGHT_STEP -1))	memory_map[MaCaco_OUT_s + slot + 3] = (0xF0 - BRIGHT_STEP -1);
			else if(b < 0)					memory_map[MaCaco_OUT_s + slot + 3] = 0;
			else							memory_map[MaCaco_OUT_s + slot + 3] = b;
			
			LYTColor(memory_map[MaCaco_OUT_s + slot + 1], memory_map[MaCaco_OUT_s + slot + 2], memory_map[MaCaco_OUT_s + slot + 3], FADEENABLE);					
		}
	
		memory_map[MaCaco_IN_s + slot] = Souliss_T1n_RstCmd;			// Reset	
	}
	else if (memory_map[MaCaco_IN_s + slot] == Souliss_T1n_BrightUp)				// Increase the light value 
	{
		// Increase the brightness
		if(memory_map[MaCaco_AUXIN_s + slot] < (LYT_MaxBright-BRIGHT_STEP))	memory_map[MaCaco_AUXIN_s + slot] = memory_map[MaCaco_AUXIN_s + slot] + BRIGHT_STEP;	// Increase the light value

		// If is white
		if((memory_map[MaCaco_AUXIN_s + slot + 1] >= 0xF0) && (memory_map[MaCaco_AUXIN_s + slot + 2] >= 0xF0) && (memory_map[MaCaco_AUXIN_s + slot + 3] >= 0xF0))
			LYTWhite(memory_map[MaCaco_AUXIN_s + slot], FADEENABLE);
		else	// Otherwise
		{
			// Get the base brightness
			uint8_t base_bright =  memory_map[MaCaco_AUXIN_s + slot + 1];
			if(memory_map[MaCaco_AUXIN_s + slot + 2] > base_bright)
				base_bright =  memory_map[MaCaco_AUXIN_s + slot + 2];
			if(memory_map[MaCaco_AUXIN_s + slot + 3] > base_bright)
				base_bright =  memory_map[MaCaco_AUXIN_s + slot + 3];

			float r = memory_map[MaCaco_AUXIN_s + slot + 1] + (memory_map[MaCaco_AUXIN_s + slot] - base_bright);
			float g = memory_map[MaCaco_AUXIN_s + slot + 2] + (memory_map[MaCaco_AUXIN_s + slot] - base_bright);
			float b = memory_map[MaCaco_AUXIN_s + slot + 3] + (memory_map[MaCaco_AUXIN_s + slot] - base_bright);

			if(r > (0xF0 - BRIGHT_STEP -1))	memory_map[MaCaco_OUT_s + slot + 1] = (0xF0 - BRIGHT_STEP -1);
			else if(r < 0)					memory_map[MaCaco_OUT_s + slot + 1] = 0;
			else							memory_map[MaCaco_OUT_s + slot + 1] = r;

			if(g > (0xF0 - BRIGHT_STEP -1))	memory_map[MaCaco_OUT_s + slot + 2] = (0xF0 - BRIGHT_STEP -1);
			else if(g < 0)					memory_map[MaCaco_OUT_s + slot + 2] = 0;
			else							memory_map[MaCaco_OUT_s + slot + 2] = g;

			if(b > (0xF0 - BRIGHT_STEP -1))	memory_map[MaCaco_OUT_s + slot + 3] = (0xF0 - BRIGHT_STEP -1);
			else if(b < 0)					memory_map[MaCaco_OUT_s + slot + 3] = 0;
			else							memory_map[MaCaco_OUT_s + slot + 3] = b;
			
			LYTColor(memory_map[MaCaco_OUT_s + slot + 1], memory_map[MaCaco_OUT_s + slot + 2], memory_map[MaCaco_OUT_s + slot + 3], FADEENABLE);					
		}				

		memory_map[MaCaco_IN_s + slot] = Souliss_T1n_RstCmd;						// Reset
	}
	else if (memory_map[MaCaco_IN_s + slot] == Souliss_T1n_BrightDown)				// Decrease the light value
	{
		// Decrease the brightness
		if(memory_map[MaCaco_AUXIN_s + slot] > (LYT_MinBright+BRIGHT_STEP))	memory_map[MaCaco_AUXIN_s + slot] = memory_map[MaCaco_AUXIN_s + slot] - BRIGHT_STEP;		// Decrease the light value

		// If is white		
		if((memory_map[MaCaco_AUXIN_s + slot + 1] >= 0xF0) && (memory_map[MaCaco_AUXIN_s + slot + 2] >= 0xF0) && (memory_map[MaCaco_AUXIN_s + slot + 3] >= 0xF0))
			LYTWhite(memory_map[MaCaco_AUXIN_s + slot], FADEENABLE);		
		else	// Otherwise
		{
			// Get the base brightness
			uint8_t base_bright =  memory_map[MaCaco_AUXIN_s + slot + 1];
			if(memory_map[MaCaco_AUXIN_s + slot + 2] > base_bright)
				base_bright =  memory_map[MaCaco_AUXIN_s + slot + 2];
			if(memory_map[MaCaco_AUXIN_s + slot + 3] > base_bright)
				base_bright =  memory_map[MaCaco_AUXIN_s + slot + 3];

			float r = memory_map[MaCaco_AUXIN_s + slot + 1] + (memory_map[MaCaco_AUXIN_s + slot] - base_bright);
			float g = memory_map[MaCaco_AUXIN_s + slot + 2] + (memory_map[MaCaco_AUXIN_s + slot] - base_bright);
			float b = memory_map[MaCaco_AUXIN_s + slot + 3] + (memory_map[MaCaco_AUXIN_s + slot] - base_bright);

			if(r > (0xF0 - BRIGHT_STEP -1))	memory_map[MaCaco_OUT_s + slot + 1] = (0xF0 - BRIGHT_STEP -1);
			else if(r < 0)					memory_map[MaCaco_OUT_s + slot + 1] = 0;
			else							memory_map[MaCaco_OUT_s + slot + 1] = r;

			if(g > (0xF0 - BRIGHT_STEP -1))	memory_map[MaCaco_OUT_s + slot + 2] = (0xF0 - BRIGHT_STEP -1);
			else if(g < 0)					memory_map[MaCaco_OUT_s + slot + 2] = 0;
			else							memory_map[MaCaco_OUT_s + slot + 2] = g;

			if(b > (0xF0 - BRIGHT_STEP -1))	memory_map[MaCaco_OUT_s + slot + 3] = (0xF0 - BRIGHT_STEP -1);
			else if(b < 0)					memory_map[MaCaco_OUT_s + slot + 3] = 0;
			else							memory_map[MaCaco_OUT_s + slot + 3] = b;
			
			LYTColor(memory_map[MaCaco_OUT_s + slot + 1], memory_map[MaCaco_OUT_s + slot + 2], memory_map[MaCaco_OUT_s + slot + 3], FADEENABLE);					
		}				

		memory_map[MaCaco_IN_s + slot] = Souliss_T1n_RstCmd;						// Reset
	}
	else if (memory_map[MaCaco_IN_s + slot] == Souliss_T1n_Flash)					// Flash
		Souliss_UnsupportedCommand();												// Actually not supported
	

	// Update the trigger
	if(i_trigger)
		*trigger = i_trigger;
	
	return i_trigger;	
	
}

/**************************************************************************
/*!
	Timer associated to T16
*/	
/**************************************************************************/
void Souliss_LYTLamps_Timer(U8 *memory_map, U8 slot)
{
	if(memory_map[MaCaco_IN_s + slot] > Souliss_T1n_Timed)	
	{
		// Set the good night mode
		if(memory_map[MaCaco_OUT_s + slot] != Souliss_T1n_GoodNight)
			memory_map[MaCaco_OUT_s + slot] = Souliss_T1n_GoodNight;
		
		memory_map[MaCaco_IN_s + slot] = Souliss_T1n_RstCmd;						// Reset
		memory_map[MaCaco_IN_s + slot + 1] = Souliss_T1n_RstCmd;
		memory_map[MaCaco_IN_s + slot + 2] = Souliss_T1n_RstCmd;
		memory_map[MaCaco_IN_s + slot + 3] = Souliss_T1n_RstCmd;
	}	

	// Decrease brightness and check the expiration
	if((memory_map[MaCaco_OUT_s + slot] == Souliss_T1n_GoodNight) && (memory_map[MaCaco_AUXIN_s + slot] <= (LYT_MinBright+BRIGHT_STEP)))	
		memory_map[MaCaco_IN_s + slot] = Souliss_T1n_OffCmd;	
	else if((memory_map[MaCaco_OUT_s + slot] == Souliss_T1n_GoodNight))	
		memory_map[MaCaco_IN_s + slot] = Souliss_T1n_BrightDown;
	
}

/**************************************************************************
/*!
	Switch the typical OFF
*/	
/**************************************************************************/
void Souliss_SetOff(U8 *memory_map, U8 slot, U8 *trigger)
{
	memory_map[MaCaco_IN_s + slot] = Souliss_T1n_OffCmd;
	Souliss_Logic_LYTLamps(memory_map, slot, trigger);
}

/**************************************************************************
/*!
	Increase the brightness
*/	
/**************************************************************************/
void Souliss_IncreaseBrightness(U8 *memory_map, U8 slot, U8 *trigger)
{
	memory_map[MaCaco_IN_s + slot] = Souliss_T1n_BrightUp;
	Souliss_Logic_LYTLamps(memory_map, slot, trigger);
}

/**************************************************************************
/*!
	Decrease the brightness
*/	
/**************************************************************************/
void Souliss_DecreaseBrightness(U8 *memory_map, U8 slot, U8 *trigger)
{
	memory_map[MaCaco_IN_s + slot] = Souliss_T1n_BrightDown;
	Souliss_Logic_LYTLamps(memory_map, slot, trigger);
}

/**************************************************************************
/*!
	Set White and its brightness
*/	
/**************************************************************************/
void Souliss_SetWhite(U8 *memory_map, U8 slot, U8 *trigger, U8 brightness)
{
	memory_map[MaCaco_IN_s + slot]     = Souliss_T1n_OnCmd;
	memory_map[MaCaco_AUXIN_s + slot + 1] = 0xFF;
	memory_map[MaCaco_AUXIN_s + slot + 2] = 0xFF;
	memory_map[MaCaco_AUXIN_s + slot + 3] = 0xFF;
	memory_map[MaCaco_AUXIN_s + slot]  = brightness;

	Souliss_Logic_LYTLamps(memory_map, slot, trigger);	
}

/**************************************************************************
/*!
	Set Color
*/	
/**************************************************************************/
void Souliss_SetColor(U8 *memory_map, U8 slot, U8 *trigger, U8 red, U8 green, U8 blue)
{
	// Set ON command
	memory_map[MaCaco_IN_s + slot]     = Souliss_T1n_OnCmd;

	// Set color
	memory_map[MaCaco_AUXIN_s + slot + 1] = red;
	memory_map[MaCaco_AUXIN_s + slot + 2] = green;
	memory_map[MaCaco_AUXIN_s + slot + 3] = blue;

	// Get bright value from color that has been set
	memory_map[MaCaco_AUXIN_s + slot] = memory_map[MaCaco_AUXIN_s + slot + 1];
	if(memory_map[MaCaco_AUXIN_s + slot + 2] > memory_map[MaCaco_AUXIN_s + slot])
		memory_map[MaCaco_AUXIN_s + slot] = memory_map[MaCaco_AUXIN_s + slot + 2];
	if(memory_map[MaCaco_AUXIN_s + slot + 3] > memory_map[MaCaco_AUXIN_s + slot])
		memory_map[MaCaco_AUXIN_s + slot] = memory_map[MaCaco_AUXIN_s + slot + 3];

	// Execute the logic
	Souliss_Logic_LYTLamps(memory_map, slot, trigger);	
}