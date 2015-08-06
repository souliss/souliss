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
#include "Iotuino.h"
#include "Typicals.h"
#include "GetConfig.h"
#include "LYT.h"

#define	BRIGHT_STEP		0

LYT_struct LYT[LYT_MAXNUM];								// Each RGB light use 
LYTWiFi myLYTWiFi;										// Define a class to control LYT bulbs

void InitLYT()
{
	myLYTWiFi.vfInitialize(PL1167_CS_PIN);
	vfISRInit();
	myLYTWiFi.vfSetLocalChannel(PL1167_DEFAULT_RADIO_TRASMISSION,0);
}

/**************************************************************************
/*!
	Set LYT values
*/	
/**************************************************************************/
void SetLYT(U8 index, U8 addr_a, U8 addr_b, U8 slot)
{
	if(index < LYT_MAXNUM)
	{
		LYT[index].addr_a = addr_a;
		LYT[index].addr_b = addr_b;
		LYT[index].slot;	
	}
	
	// Set the LYT
	myLYTWiFi.vfSetSyncWord(addr_a, addr_b, C_MSBYTE_SYNCWORD0, C_LSBYTE_SYNCWORD0);	
	
}

/**************************************************************************
/*!
	Returns the index in the LYT structure starting from the assigned slot
*/	
/**************************************************************************/
U8 FindLYT(U8 slot)
{
	U8 i=0;
	
	for(i=0;i<LYT_MAXNUM;i++)
		if(LYT[i].slot == slot)
			break;
	
	return i;
	
}

/**************************************************************************
/*!
	Turn the relevant light bulb ON
*/	
/**************************************************************************/
void LYTOn(U8 slot)
{
	// Get the index of the LYT logic typicals
	uint8_t index =	FindLYT(slot);

	// Send turn ON command
	myLYTWiFi.ui8fSwitchOnAndCheck(LYT[index].addr_a,LYT[index].addr_b, 1, BRIGHT_STEP);      	
}

/**************************************************************************
/*!
	Turn the relevant light bulb OFF
*/	
/**************************************************************************/
void LYTOff(U8 slot)
{
	// Get the index of the LYT logic typicals
	uint8_t index =	FindLYT(slot);
	
	// Send turn OFF command
	myLYTWiFi.ui8fSwitchOffAndCheck(LYT[index].addr_a,LYT[index].addr_b, 1, BRIGHT_STEP);  
}

/**************************************************************************
/*!
	Set white
*/	
/**************************************************************************/
void LYTSetWhite(U8 slot)
{
	// Get the index of the LYT logic typicals
	uint8_t index =	FindLYT(slot);
	
	// Send turn OFF command
	myLYTWiFi.###(LYT[index].addr_a,LYT[index].addr_b, 1, BRIGHT_STEP);  
}

/**************************************************************************
/*!
	Set bright
*/	
/**************************************************************************/
void LYTSetBright(U8 bright, U8 slot)
{
	// Get the index of the LYT logic typicals
	uint8_t index =	FindLYT(slot);
	
	// Send turn OFF command
	myLYTWiFi.ui8fSetBrightnessValueAndCheck(LYT[index].addr_a,LYT[index].addr_b, bright, 1, BRIGHT_STEP);  
}

/**************************************************************************
/*!
	Set the color as RGB
*/	
/**************************************************************************/
void LYTSetColorRGB(U8 R, U8 G, U8 B, U8 slot)
{
	// Get the index of the LYT logic typicals
	uint8_t index =	FindLYT(slot);
	
	// Send turn OFF command
	myLYTWiFi.ui8fSetRGBValuesAndCheck(LYT[index].addr_a,LYT[index].addr_b, R, G, B, 1, BRIGHT_STEP);  	
}	

/**************************************************************************
/*!
	Check the actual state, the code stops until an answer has been received
*/	
/**************************************************************************/
void Souliss_LYTState(U8* memory_map, U8 slot, U8* trigger)
{
	// Get the index of the LYT logic typicals
	uint8_t index =	FindLYT(slot);
	
	// Request data update
	myLYTWiFi.ui8fAskLampInfoStatusAndCheck(LYT[index].addr_a,LYT[index].addr_b);
	
	// Verify the actual ON/OFF state
	if((memory_map[MaCaco_OUT_s + slot] != myLYTWiFi.ReceivedAnswer.AnswerStruct.ui8Answer[0]))
	{
		memory_map[MaCaco_OUT_s + slot] = myLYTWiFi.ReceivedAnswer.AnswerStruct.ui8Answer[0];
		*trigger = MaCaco_DATACHANGED;
	}	
	
	// Verify the actual color
	if(((myLYTWiFi.ReceivedAnswer.AnswerStruct.ui8Answer[4] == 0) && 
	  ((memory_map[MaCaco_OUT_s + slot + 1] != myLYTWiFi.ReceivedAnswer.AnswerStruct.ui8Answer[1])) ||
	  ((memory_map[MaCaco_OUT_s + slot + 2] != myLYTWiFi.ReceivedAnswer.AnswerStruct.ui8Answer[2])) ||
	  ((memory_map[MaCaco_OUT_s + slot + 3] != myLYTWiFi.ReceivedAnswer.AnswerStruct.ui8Answer[3]))))
	 {
		memory_map[MaCaco_OUT_s + slot + 1] = myLYTWiFi.ReceivedAnswer.AnswerStruct.ui8Answer[1];
		memory_map[MaCaco_OUT_s + slot + 2] = myLYTWiFi.ReceivedAnswer.AnswerStruct.ui8Answer[2];
	    memory_map[MaCaco_OUT_s + slot + 3] = myLYTWiFi.ReceivedAnswer.AnswerStruct.ui8Answer[3];
		*trigger = MaCaco_DATACHANGED;		
	 }
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
			
			If INPUTVAL is the input value the output will be timed for nCYCLES
			of the associated timer.
				nCYCLES = INPUTVAL - Souliss_T1n_Timed
			
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
	
	if (memory_map[MaCaco_IN_s + slot] == Souliss_T1n_ToggleCmd)		// Toggle Command
	{
		// Toggle the actual status of the light
		if(memory_map[MaCaco_OUT_s + slot] == Souliss_T1n_OffCoil)		
			memory_map[MaCaco_IN_s + slot] = Souliss_T1n_OnCmd;			
		else if(memory_map[MaCaco_OUT_s + slot] == Souliss_T1n_OnCoil)
			memory_map[MaCaco_IN_s + slot] = Souliss_T1n_OffCmd;
		else
			memory_map[MaCaco_IN_s + slot] = Souliss_T1n_RstCmd;
		
		// Trig the change
		i_trigger = Souliss_TRIGGED;	
	}
	else if (memory_map[MaCaco_IN_s + slot] == Souliss_T1n_OffCmd)		// Off Command
	{
		memory_map[MaCaco_OUT_s + slot] = Souliss_T1n_OffCoil;		// Switch off the light state
		i_trigger = Souliss_TRIGGED;								// Trig the change
		
		// Send the off command
		LYTOff(slot);
		
		// Reset the input
		memory_map[MaCaco_IN_s + slot] = Souliss_T1n_RstCmd;		
	}
	else if (memory_map[MaCaco_IN_s + slot] == Souliss_T1n_OnCmd)
	{
		i_trigger = Souliss_TRIGGED;									// Trig the change
	
		memory_map[MaCaco_OUT_s + slot] = Souliss_T1n_OnCoil;			// Switch on the output
		
		// If there were no color set, use a light white
		if((memory_map[MaCaco_AUXIN_s + slot + 1] == 0) && (memory_map[MaCaco_AUXIN_s + slot + 2] == 0) 
			&& (memory_map[MaCaco_AUXIN_s + slot + 3] == 0))
		{
			for(U8 i=1;i<4;i++)
			{
				memory_map[MaCaco_OUT_s + slot + i] = 0xFF;				// Set a light white
				memory_map[MaCaco_AUXIN_s + slot + i] = memory_map[MaCaco_OUT_s + slot + i];
			}
			
			memory_map[MaCaco_IN_s + slot] = Souliss_T1n_RstCmd;			// Reset	
		}			
		else
		{
			// Use the last color
			for(U8 i=1;i<4;i++)
				memory_map[MaCaco_OUT_s + slot + i] = (memory_map[MaCaco_AUXIN_s + slot + i]);
							
		}

		// Turn the lamp on
		LYTOn(slot);
		
		// If the color is set as white
		if( ((memory_map[MaCaco_OUT_s + slot + 1] >= 0xF0) &&
			(memory_map[MaCaco_OUT_s + slot + 2]  >= 0xF0) &&
			(memory_map[MaCaco_OUT_s + slot + 3]  >= 0xF0)) )			
		{
			LYTSetBright(memory_map[MaCaco_AUXIN_s + slot], slot);		
		}		
		else // Set the color
			LYTSetColorRGB(memory_map[MaCaco_OUT_s + slot + 1], memory_map[MaCaco_OUT_s + slot + 2], memory_map[MaCaco_OUT_s + slot + 3], slot);		
		
		memory_map[MaCaco_IN_s + slot] = Souliss_T1n_RstCmd;			// Reset	
	}
	else if (memory_map[MaCaco_IN_s + slot] == Souliss_T1n_BrightUp)		// Increase the light value 
	{
		if((memory_map[MaCaco_OUT_s + slot + 1] < 0xF0) || (memory_map[MaCaco_OUT_s + slot + 2] < 0xF0) || (memory_map[MaCaco_OUT_s + slot + 3] < 0xF0))
		{
			LYTSetColorRGB(++memory_map[MaCaco_OUT_s + slot + 1], ++memory_map[MaCaco_OUT_s + slot + 2], ++memory_map[MaCaco_OUT_s + slot + 3], slot);				
		}
		else
		{
			// Increase the light value
			if(memory_map[MaCaco_AUXIN_s + slot] < LYT_MaxBright) 
				memory_map[MaCaco_AUXIN_s + slot]++;
			
			// Set bright
			LYTSetBright(memory_map[MaCaco_AUXIN_s + slot], slot);		
		}
		
		memory_map[MaCaco_IN_s + slot] = Souliss_T1n_RstCmd;			// Reset
	}
	else if (memory_map[MaCaco_IN_s + slot] == Souliss_T1n_BrightDown)				// Decrease the light value
	{

		if((memory_map[MaCaco_OUT_s + slot + 1] < 0xF0) || (memory_map[MaCaco_OUT_s + slot + 2] < 0xF0) || (memory_map[MaCaco_OUT_s + slot + 3] < 0xF0))
		{
			LYTSetColorRGB(--memory_map[MaCaco_OUT_s + slot + 1], --memory_map[MaCaco_OUT_s + slot + 2], --memory_map[MaCaco_OUT_s + slot + 3], slot);				
		}
		else
		{		
			// Decrease the light value
			if(memory_map[MaCaco_AUXIN_s + slot] > LYT_MinBright) 
				memory_map[MaCaco_AUXIN_s + slot]--;
				
			// Set bright
			LYTSetBright(memory_map[MaCaco_AUXIN_s + slot], slot);
		}
		
		memory_map[MaCaco_IN_s + slot] = Souliss_T1n_RstCmd;			// Reset
	}	
	else if (memory_map[MaCaco_IN_s + slot] == Souliss_T1n_Flash)					// Turn ON and OFF at each cycle
	{
		// If the light was on
		if(memory_map[MaCaco_OUT_s + slot] == Souliss_T1n_OnCoil)
		{
			// Set the output state
			memory_map[MaCaco_OUT_s + slot] = Souliss_T1n_OffCoil;
			
			// Send the command
			LYTOn(slot);
		}
		else
		{
			// Set the output state
			memory_map[MaCaco_OUT_s + slot] = Souliss_T1n_OnCoil;
			
			// Send the command
			LYTOff(slot);
		}		
	}
	else if (memory_map[MaCaco_IN_s + slot] == Souliss_T1n_Set)
	{	
		// Set the new color
		for(U8 i=1;i<4;i++)
		{
			memory_map[MaCaco_OUT_s + slot + i] = memory_map[MaCaco_IN_s + slot + i];
			memory_map[MaCaco_AUXIN_s + slot + i] = memory_map[MaCaco_OUT_s + slot + i];
			memory_map[MaCaco_IN_s + slot + i] = Souliss_T1n_RstCmd;
		}

		// If the color is set as white
		if( ((memory_map[MaCaco_OUT_s + slot + 1] >= 0xF0) &&
			(memory_map[MaCaco_OUT_s + slot + 2]  >= 0xF0) &&
			(memory_map[MaCaco_OUT_s + slot + 3]  >= 0xF0)) )			
		{
			LYTSetWhite(slot);
		}		
		else // Set the color
			LYTSetColorRGB(memory_map[MaCaco_OUT_s + slot + 1], memory_map[MaCaco_OUT_s + slot + 2], memory_map[MaCaco_OUT_s + slot + 3], slot);
		
		memory_map[MaCaco_OUT_s + slot] = Souliss_T1n_OnCoil;			// Switch on the output
		memory_map[MaCaco_IN_s + slot] = Souliss_T1n_RstCmd;			// Reset		
	}

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
	if(memory_map[MaCaco_IN_s + slot] > Souliss_T1n_Timed)		// Memory value is used as timer
	{
		if(memory_map[MaCaco_OUT_s + slot] != Souliss_T1n_GoodNight)
		{
			// Set the good night mode
			memory_map[MaCaco_OUT_s + slot] = Souliss_T1n_GoodNight;
		}
		
		// Decrease timer and check the expiration
		if((--memory_map[MaCaco_IN_s + slot]) == Souliss_T1n_Timed)		
			memory_map[MaCaco_IN_s + slot] = Souliss_T1n_OffCmd;
		
	}	
}
