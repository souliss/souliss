/**************************************************************************
	Souliss
    Copyright (C) 2011  Veseo

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
	
	Originally developed by Dario Di Maio and Alessandro DelPex
	
***************************************************************************/
/*!
    \file 
    \ingroup

*/

#include "frame/MaCaco/MaCaco.h"
#include "Souliss.h"
#include "Arduino.h"

#include "Typicals.h"

/**************************************************************************
/*!
	Define the use of Typical 11 : ON/OFF Digital Output with Timer Option
*/	
/**************************************************************************/
void Souliss_SetT11(U8 *memory_map, U8 slot)
{
	memory_map[MaCaco_TYP_s + slot] = Souliss_T11;
}

/**************************************************************************
/*!
	Typical 11 : ON/OFF Digital Output with Timer Option
	
		Handle one digital output based on hardware and software commands,
		output can be timed out.
		
		This logic can be used for lights, wall socket and all the devices
		that has an ON/OFF behavior.
		
		Hardware Command:
			
			If using a monostable wall switch (press and spring return), 
			each press will toogle the output status.		
				#define Souliss_T1n_ToogleCmd		0x01
			
			If using a bistable wall switch (press without return), the two
			switch position can be associated with the ON and OFF commands
				#define Souliss_T1n_OnCmd			0x02
				#define Souliss_T1n_OffCmd			0x04
					
		Software Commands:		
			
			From any available software interface, these commands will turn
			the light ON and OFF.
				#define Souliss_T1n_OnCmd			0x02
				#define Souliss_T1n_OffCmd			0x04
		
		Timed Output Command:
		
			This logic can be used with a timer to active the output state for
			a predifined amount of time, timer is activated if the input command
			is greater than
				#define Souliss_T1n_Timed			0x30
			
			A timed out command can be sent via software or hardware commands,
			in case of hardware commands is usually associated to a long press
			of a monostable wall switch.
			
			If INPUTVAL is the input value the output will be timed for nCYCLES
			of the associated timer.
				nCYCLES = INPUTVAL - Souliss_T1n_Timed
			
		Command recap, using: 
		-  1(hex) as command, toogle the output 
		-  2(hex) as command, the output move to ON
		-  4(hex) as command, the output move to OFF
		- >30(hex) as command, time the output to ON
		-  0(hex) as command, no action

		Output status,
		-  0(hex) for output OFF,
		-  1(hex) for output ON.
		
*/	
/**************************************************************************/
void Souliss_Logic_T11(U8 *memory_map, U8 slot, U8 *trigger)
{
	// Look for input value, update output. If the output is not set, trig a data
	// change, otherwise just reset the input
	
	if(memory_map[MaCaco_IN_s + slot] > Souliss_T1n_Timed) // Memory value is used as timer
	{	
		if(memory_map[MaCaco_OUT_s + slot] != Souliss_T1n_OnCoil)  
			*trigger = Souliss_TRIGGED;									// Trig change
		
		memory_map[MaCaco_OUT_s + slot] = Souliss_T1n_OnCoil;			// Switch on the output
	}
	else if (memory_map[MaCaco_IN_s + slot] == Souliss_T1n_OffCmd)	// Off Command
	{
		if(memory_map[MaCaco_OUT_s + slot] != Souliss_T1n_OffCoil)  
			*trigger = Souliss_TRIGGED;
	
		memory_map[MaCaco_OUT_s + slot] = Souliss_T1n_OffCoil;			// Switch off the output
		memory_map[MaCaco_IN_s + slot] = Souliss_T1n_RstCmd;			// Reset
	}
	else if (memory_map[MaCaco_IN_s + slot] == Souliss_T1n_OnCmd)
	{
		if(memory_map[MaCaco_OUT_s + slot] != Souliss_T1n_OnCoil)  
			*trigger = Souliss_TRIGGED;	
	
		memory_map[MaCaco_OUT_s + slot] = Souliss_T1n_OnCoil;			// Switch on the output
		memory_map[MaCaco_IN_s + slot] = Souliss_T1n_RstCmd;			// Reset
	}
	else if (memory_map[MaCaco_IN_s + slot] == Souliss_T1n_ToogleCmd)
	{
		if(memory_map[MaCaco_OUT_s + slot] == Souliss_T1n_OnCoil)
			memory_map[MaCaco_OUT_s + slot] = Souliss_T1n_OffCoil;			// Switch OFF the output
		else if(memory_map[MaCaco_OUT_s + slot] == Souliss_T1n_OffCoil)
			memory_map[MaCaco_OUT_s + slot] = Souliss_T1n_OnCoil;			// Switch ON the output		
		
		*trigger = Souliss_TRIGGED;	
		memory_map[MaCaco_IN_s + slot] = Souliss_T1n_RstCmd;			// Reset
	}
}

/**************************************************************************
/*!
	Timer associated to T11
*/	
/**************************************************************************/
void Souliss_T11_Timer(U8 *memory_map, U8 input_slot)
{
	if(memory_map[MaCaco_IN_s + input_slot] > Souliss_T1n_OffCmd)		// Memory value is used as timer
		memory_map[MaCaco_IN_s + input_slot]--;							// Decrease timer
}

/**************************************************************************
/*!
	Define the use of Typical 12 : ON/OFF Digital Output with AUTO mode
*/	
/**************************************************************************/
void Souliss_SetT12(U8 *memory_map, U8 slot)
{
	memory_map[MaCaco_TYP_s + slot] = Souliss_T12;
}

/**************************************************************************
/*!
	Typical 12 : ON/OFF Digital Output with AUTO mode

		Handle one digital output based on hardware and software commands,
		output can be set in AUTO mode. 
		
		This logic can be used for lights, wall socket and all the devices
		that has an ON/OFF behavior, output can be driven by external event
		like a PIR sensor.
		
		Hardware Command:
			
			If using a monostable wall switch (press and spring return), 
			each press will toogle the output status.		
				#define Souliss_T1n_ToogleCmd		0x01
			
			If using a bistable wall switch (press without return), the two
			switch position can be associated with the ON and OFF commands
				#define Souliss_T1n_OnCmd			0x02
				#define Souliss_T1n_OffCmd			0x04
			
			Every time that an ON, OFF or TOOGLE command is received the AUTO
			mode is automatically removed.
			
		Software Commands:		
			
			From any available software interface, these commands will turn
			the light ON and OFF.
				#define Souliss_T1n_OnCmd			0x02
				#define Souliss_T1n_OffCmd			0x04
		
			Every time that an ON, OFF or TOOGLE command is received the AUTO
			mode is automatically removed.
			
		AUTO Mode:
		
			If in AUTO the output is driven by external event, such events has
			no effect if the AUTO mode is not active.
			
			This logic MUST be used with a timer to active the output state for
			a predifined amount of time, once the external event is recognized.
			Once the external event occur, the input of the logic shall be 
			greather than
				#define Souliss_T1n_AutoCmd			0x08
				
			If INPUTVAL is the input value associated to the external event the 
			output will be timed for nCYCLES of the associated timer.
				nCYCLES = INPUTVAL - Souliss_T1n_Timed
			
		Command recap, using: 
		-  1(hex) as command, toogle the output 
		-  2(hex) as command, the output move to ON the mode is reset
		-  4(hex) as command, the output move to OFF the mode is reset
		-  8(hex) as command, the mode is set to AUTO
		- >8(hex) as command, time the output to ON if in AUTO 
		-  0(hex) as command, no action
		
		Output status,
		-  0(hex) for output OFF and mode not in AUTO,
		-  1(hex) for output ON and mode not in AUTO,
		- F0(hex) for output OFF and mode in AUTO,
		- F1(hex) for output ON and mode in AUTO.

*/	
/**************************************************************************/
void Souliss_Logic_T12(U8 *memory_map, U8 slot, U8 *trigger)
{
	// Look for input value, update output. If the output is not set, trig a data
	// change, otherwise just reset the input
	
	if((memory_map[MaCaco_IN_s + slot] > Souliss_T1n_AutoCmd) && (memory_map[MaCaco_OUT_s + slot] & Souliss_T1n_AutoState)) // Memory value is used as timer
	{	
		if((memory_map[MaCaco_OUT_s + slot] & ~Souliss_T1n_AutoState) != Souliss_T1n_OnCoil)  
			*trigger = Souliss_TRIGGED;									// Trig change
		
		memory_map[MaCaco_OUT_s + slot] = (Souliss_T1n_AutoState | Souliss_T1n_OnCoil);			// Switch on the output
	}
	else if (memory_map[MaCaco_IN_s + slot] == Souliss_T1n_OffCmd)	// Off Command
	{
		if((memory_map[MaCaco_OUT_s + slot] & ~Souliss_T1n_AutoState) != Souliss_T1n_OffCoil)  
			*trigger = Souliss_TRIGGED;
	
		// Switch OFF and reset the AUTO mode
		memory_map[MaCaco_OUT_s + slot] = Souliss_T1n_OffCoil;			// Switch off the output and reset the AUTO mode
		memory_map[MaCaco_IN_s + slot] = Souliss_T1n_RstCmd;			// Reset
	}
	else if (memory_map[MaCaco_IN_s + slot] == Souliss_T1n_OnCmd)
	{
		if((memory_map[MaCaco_OUT_s + slot] & ~Souliss_T1n_AutoState) != Souliss_T1n_OnCoil)  
			*trigger = Souliss_TRIGGED;	
		
		// Switch ON and reset the AUTO mode
		memory_map[MaCaco_OUT_s + slot] = Souliss_T1n_OnCoil;			// Switch on the output
		memory_map[MaCaco_IN_s + slot] = Souliss_T1n_RstCmd;			// Reset
	}
	else if (memory_map[MaCaco_IN_s + slot] == Souliss_T1n_ToogleCmd)
	{
		if((memory_map[MaCaco_OUT_s + slot] & ~Souliss_T1n_AutoState) == Souliss_T1n_OnCoil)
			memory_map[MaCaco_OUT_s + slot] = Souliss_T1n_OffCoil;			// Switch OFF the output and reset the AUTO mode
		else if((memory_map[MaCaco_OUT_s + slot] & ~Souliss_T1n_AutoState) == Souliss_T1n_OffCoil)
			memory_map[MaCaco_OUT_s + slot] = Souliss_T1n_OnCoil;			// Switch ON the output	and reset the AUTO mode

		*trigger = Souliss_TRIGGED;										// Trig the state change
		memory_map[MaCaco_IN_s + slot] = Souliss_T1n_RstCmd;			// Reset
	}
	else if (memory_map[MaCaco_IN_s + slot] == Souliss_T1n_AutoCmd)	// Set AUTO state
	{
		// If the logic is not in AUTO, active AUTO mode
		if((memory_map[MaCaco_OUT_s + slot] == (Souliss_T1n_AutoState | Souliss_T1n_Coil)) || ((memory_map[MaCaco_OUT_s + slot] & Souliss_T1n_AutoState) != Souliss_T1n_AutoState))
			memory_map[MaCaco_OUT_s + slot] = (Souliss_T1n_AutoState | Souliss_T1n_OffCoil);			// Switch OFF and set the AUTO mode
		else
			memory_map[MaCaco_OUT_s + slot] = (~Souliss_T1n_AutoState & Souliss_T1n_OffCoil);			// Switch OFF and reset the AUTO mode
			
		*trigger = Souliss_TRIGGED;
		memory_map[MaCaco_IN_s + slot] = Souliss_T1n_RstCmd;			// Reset
	}
	else
		memory_map[MaCaco_IN_s + slot] = Souliss_T1n_RstCmd;			// No recognized command, reset
}

/**************************************************************************
/*!
	Timer associated to T12
*/	
/**************************************************************************/
void Souliss_T12_Timer(U8 *memory_map, U8 input_slot)
{
	if(memory_map[MaCaco_IN_s + input_slot] > Souliss_T1n_AutoCmd)		// Memory value is used as timer
		memory_map[MaCaco_IN_s + input_slot]--;							// Decrease timer
}

/**************************************************************************
/*!
	Define the use of Typical 13 : Digital Input Value
*/	
/**************************************************************************/
void Souliss_SetT13(U8 *memory_map, U8 slot)
{
	memory_map[MaCaco_TYP_s + slot] = Souliss_T13;
}

/**************************************************************************
/*!
	Typical 13 : Digital Input Value
	
		It read read data from input are of the memory map and place the
		values in the output, this logic shall be used to monitor a digital
		value into an user interface.
		
		Hardware Command:
				
			Using a bistable input switch (press without return) the two switch
			position can be associated with the ON and OFF commands
				#define Souliss_T1n_OnCmd			0x02
				#define Souliss_T1n_OffCmd			0x04
			
		Command recap, using: 
		-  2(hex) as command, the output move to ON
		-  4(hex) as command, the output move to OFF
		-  0(hex) as command, no action
		
		Output status,
		-  0(hex) for output OFF,
		-  1(hex) for output ON.

*/	
/**************************************************************************/
void Souliss_Logic_T13(U8 *memory_map, U8 slot, U8 *trigger)
{
	// Look for input value, update output. If the output is not set, trig a data
	// change, otherwise just reset the input
	
	if(memory_map[MaCaco_IN_s + slot] == Souliss_T1n_OnCmd)		// Memory value is used as timer
	{	
		if(memory_map[MaCaco_OUT_s + slot] != Souliss_T1n_OnCoil)  
			*trigger = Souliss_TRIGGED;									// Trig change
		
		memory_map[MaCaco_OUT_s + slot] = Souliss_T1n_OnCoil;	
		memory_map[MaCaco_IN_s + slot] = Souliss_T1n_RstCmd;			// Reset
	}
	else if(memory_map[MaCaco_IN_s + slot] == Souliss_T1n_OffCmd)		// Memory value is used as timer
	{	
		if(memory_map[MaCaco_OUT_s + slot] != Souliss_T1n_OffCoil)  
			*trigger = Souliss_TRIGGED;									// Trig change
		
		memory_map[MaCaco_OUT_s + slot] = Souliss_T1n_OffCoil;	
		memory_map[MaCaco_IN_s + slot] = Souliss_T1n_RstCmd;			// Reset
	}
	
}

/**************************************************************************
/*!
	Define the use of Typical 14 : Pulse Digital Output
*/	
/**************************************************************************/
void Souliss_SetT14(U8 *memory_map, U8 slot)
{
	memory_map[MaCaco_TYP_s + slot] = Souliss_T14;
}

/**************************************************************************
/*!
	Typical 14 : Pulse Digital Output
	
		It control a digital output with a pulse of the duration of one
		execution cycle.
		
		Hardware and/or Software Command:
				
			Using a monostable input switch (press with return) or a software
			command from the user interface to have a pulse on the output
				#define Souliss_T1n_OnCmd			0x02
			
		Command recap, using: 
		-  2(hex) as command, the output move to ON for a cycle
		-  0(hex) as command, no action
		
		Output status,
		-  0(hex) for output OFF,
		-  1(hex) for output ON pulse.

*/	
/**************************************************************************/
void Souliss_Logic_T14(U8 *memory_map, U8 slot, U8 *trigger)
{
	// If the ON command is received
	if (memory_map[MaCaco_IN_s + slot] == Souliss_T1n_OnCmd)	
	{
		// Set the output ON
		memory_map[MaCaco_OUT_s + slot] = Souliss_T1n_OnCoil;
		
		// Reset the command
		memory_map[MaCaco_IN_s + slot] = Souliss_T1n_RstCmd;
		*trigger = Souliss_TRIGGED;
	}
	else	// Set the output OFF
		memory_map[MaCaco_OUT_s + slot] = Souliss_T1n_OffCoil;	

}

/**************************************************************************/
/*!
	Define the use of Typical 15 : RGB Light
*/	
/**************************************************************************/
void Souliss_SetT15(U8 *memory_map, U8 slot)
{
	memory_map[MaCaco_TYP_s + slot] = Souliss_T15;
}

/**************************************************************************
/*!
	Typical 15 : RGB Light

		It map output over inputs matching the user interface commands,
		act as a remote control for the conditioner and require a mapping of
		the typical commands and the expected commands on the conditioner.
		
		Software Commands:
			
			A command is long one byte and indicate one operating mode of the
			RBG light. In the definitions are available the commands that shall 
			be mapped versus the RGB light command map.
			
*/	
/**************************************************************************/
void Souliss_Logic_T15(U8 *memory_map, U8 slot, U8 *trigger)
{
	// Before power OFF store the last state
	if(memory_map[MaCaco_IN_s + slot] == Souliss_T1n_RGBLamp_OffCmd )
		memory_map[MaCaco_AUXIN_s + slot] = memory_map[MaCaco_OUT_s + slot];
	
	// If previosly there was an ON command, set the last state
	if((memory_map[MaCaco_OUT_s + slot] == Souliss_T1n_RGBLamp_OnCmd) && (memory_map[MaCaco_AUXIN_s + slot]))
	{
		memory_map[MaCaco_OUT_s + slot] = memory_map[MaCaco_AUXIN_s + slot];
		*trigger = Souliss_TRIGGED;
	}
	else if(memory_map[MaCaco_IN_s + slot] != Souliss_T1n_RGBLamp_RstCmd) 
	{
		// Use the incoming command
		memory_map[MaCaco_OUT_s + slot] = memory_map[MaCaco_IN_s + slot];
		*trigger = Souliss_TRIGGED;
		
		memory_map[MaCaco_IN_s + slot] = Souliss_T1n_RGBLamp_RstCmd;			// Reset
	}	
	
	/*
		At this stage the requested command is available in the outputs, a mapping between
		these command and what is expected by the air conditioner device shall be used just
		after this method.
	*/	

}

/**************************************************************************
/*!
	Define the use of Typical 16 : RGB LED Strip
*/	
/**************************************************************************/
void Souliss_SetT16(U8 *memory_map, U8 slot)
{
	memory_map[MaCaco_TYP_s + slot] = Souliss_T16;
	memory_map[MaCaco_TYP_s + slot + 1] = Souliss_TRL;
	memory_map[MaCaco_TYP_s + slot + 2] = Souliss_TRL;
	memory_map[MaCaco_TYP_s + slot + 3] = Souliss_TRL;
}

/**************************************************************************
/*!
	Typical 16 : RGB LED Strip

		Handle one RGB LED strip based on software commands, with automatic 
		fade effects and "Good Night" feature. It use four (4) SLOT.					
		
		Hardware Command:
			
			If using a monostable wall switch (press and spring return), 
			each press will toogle the output status.		
				#define Souliss_T1n_ToogleCmd		0x01
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
		-  1(hex) as command, toogle the output 
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
void Souliss_Logic_T16(U8 *memory_map, U8 slot, U8 *trigger)
{
	// Look for input value, update output. If the output is not set, trig a data
	// change, otherwise just reset the input
	
	if (memory_map[MaCaco_IN_s + slot] == Souliss_T1n_ToogleCmd)		// Toogle Command
	{
		// Toogle the actual status of the light
		if(memory_map[MaCaco_OUT_s + slot] == Souliss_T1n_OffCoil)		
			memory_map[MaCaco_IN_s + slot] = Souliss_T1n_OnCmd;			
		else if(memory_map[MaCaco_OUT_s + slot] == Souliss_T1n_OnCmd)
			memory_map[MaCaco_IN_s + slot] = Souliss_T1n_OffCmd;
		else
			memory_map[MaCaco_IN_s + slot] = Souliss_T1n_RstCmd;
	}
	else
	if (memory_map[MaCaco_IN_s + slot] == Souliss_T1n_OffCmd)		// Off Command
	{
		// Trigger the change and save the actual color
		if(memory_map[MaCaco_OUT_s + slot] != Souliss_T1n_OffCoil)  
		{
			// Save the actual color and turn off the output
			for(U8 i=1;i<4;i++)
				memory_map[MaCaco_AUXIN_s + slot + i] = memory_map[MaCaco_OUT_s + slot + i];
				
			memory_map[MaCaco_OUT_s + slot] = Souliss_T1n_OffCoil;		// Switch off the light state
			*trigger = Souliss_TRIGGED;									// Trig the change
		}
		
		// Fade out and turn off the light step wise
		for(U8 i=1;i<4;i++)
			if(memory_map[MaCaco_OUT_s + slot + i]) 
				memory_map[MaCaco_OUT_s + slot + i]--;
		
		// Once is off, reset
		if((memory_map[MaCaco_OUT_s + slot + 1] == 0) && (memory_map[MaCaco_OUT_s + slot + 2] == 0) 
			&& (memory_map[MaCaco_OUT_s + slot + 3] == 0))
			memory_map[MaCaco_IN_s + slot] = Souliss_T1n_RstCmd;		// Reset
				
	}
	else if (memory_map[MaCaco_IN_s + slot] == Souliss_T1n_OnCmd)
	{
		if(memory_map[MaCaco_OUT_s + slot] != Souliss_T1n_OnCoil)  
			*trigger = Souliss_TRIGGED;	
	
		memory_map[MaCaco_OUT_s + slot] = Souliss_T1n_OnCoil;			// Switch on the output
		
		// If there were no color set, use a light white
		if((memory_map[MaCaco_AUXIN_s + slot + 1] == 0) && (memory_map[MaCaco_AUXIN_s + slot + 2] == 0) 
			&& (memory_map[MaCaco_AUXIN_s + slot + 3] == 0))
		{
			for(U8 i=1;i<4;i++)
			{
				memory_map[MaCaco_OUT_s + slot + i] = 0xAA;				// Set a light white
				memory_map[MaCaco_AUXIN_s + slot + i] = memory_map[MaCaco_OUT_s + slot + i];
			}
			
			memory_map[MaCaco_IN_s + slot] = Souliss_T1n_RstCmd;			// Reset	
		}			
		else
		{
			// Fade in to the last color
			for(U8 i=1;i<4;i++)
				if(memory_map[MaCaco_OUT_s + slot + i] < (memory_map[MaCaco_AUXIN_s + slot + i])) 
					memory_map[MaCaco_OUT_s + slot + i]++;			
		}
		
		// Once is on, reset
		if((memory_map[MaCaco_OUT_s + slot + 1] == memory_map[MaCaco_AUXIN_s + slot + 1]) 
			&& (memory_map[MaCaco_OUT_s + slot + 2] == memory_map[MaCaco_AUXIN_s + slot + 2]) 
			&& (memory_map[MaCaco_OUT_s + slot + 3] == memory_map[MaCaco_AUXIN_s + slot + 3]))
		{	
			memory_map[MaCaco_IN_s + slot] = Souliss_T1n_RstCmd;			// Reset
		}	
	}
	else if (memory_map[MaCaco_IN_s + slot] == Souliss_T1n_BrightUp)		// Increase the light value 
	{
		// Increase the light value
		if(memory_map[MaCaco_OUT_s + slot + 1] < 255 - Souliss_T1n_BrightValue) 
			memory_map[MaCaco_OUT_s + slot + 1] += Souliss_T1n_BrightValue;
			
		if(memory_map[MaCaco_OUT_s + slot + 2] < 255 - Souliss_T1n_BrightValue) 
			memory_map[MaCaco_OUT_s + slot + 2] += Souliss_T1n_BrightValue;
			
		if(memory_map[MaCaco_OUT_s + slot + 3] < 255 - Souliss_T1n_BrightValue) 
			memory_map[MaCaco_OUT_s + slot + 3] += Souliss_T1n_BrightValue;
		
		memory_map[MaCaco_IN_s + slot] = Souliss_T1n_RstCmd;			// Reset
	}
	else if (memory_map[MaCaco_IN_s + slot] == Souliss_T1n_BrightDown)				// Decrease the light value
	{
		// Decrease the light value
		if(memory_map[MaCaco_OUT_s + slot + 1] > Souliss_T1n_BrightValue) 
			memory_map[MaCaco_OUT_s + slot + 1] -= Souliss_T1n_BrightValue;

		if(memory_map[MaCaco_OUT_s + slot + 2] > Souliss_T1n_BrightValue) 
			memory_map[MaCaco_OUT_s + slot + 2] -= Souliss_T1n_BrightValue;

		if(memory_map[MaCaco_OUT_s + slot + 3] > Souliss_T1n_BrightValue) 
			memory_map[MaCaco_OUT_s + slot + 3] -= Souliss_T1n_BrightValue;
			
		memory_map[MaCaco_IN_s + slot] = Souliss_T1n_RstCmd;			// Reset
	}	
	else if (memory_map[MaCaco_IN_s + slot] == Souliss_T1n_Flash)					// Turn ON and OFF at each cycle
	{
		// If the light was on
		if(memory_map[MaCaco_OUT_s + slot] == Souliss_T1n_OnCoil)
		{
			// Switch ON and OFF until a new command is received
			if((memory_map[MaCaco_OUT_s + slot + 1] == memory_map[MaCaco_AUXIN_s + slot + 1]) 
				&& (memory_map[MaCaco_OUT_s + slot + 2] == memory_map[MaCaco_AUXIN_s + slot + 2]) 
				&& (memory_map[MaCaco_OUT_s + slot + 3] == memory_map[MaCaco_AUXIN_s + slot + 3]))
			{
				// Set output to zero
				memory_map[MaCaco_OUT_s + slot + 1] = 0;
				memory_map[MaCaco_OUT_s + slot + 2] = 0;
				memory_map[MaCaco_OUT_s + slot + 3] = 0;
			}
			else
			{
				// Set output to previous value
				memory_map[MaCaco_OUT_s + slot + 1] = memory_map[MaCaco_AUXIN_s + slot + 1];
				memory_map[MaCaco_OUT_s + slot + 2] = memory_map[MaCaco_AUXIN_s + slot + 2];
				memory_map[MaCaco_OUT_s + slot + 3] = memory_map[MaCaco_AUXIN_s + slot + 3];	
			}
		}
	}
	else if (memory_map[MaCaco_IN_s + slot] == Souliss_T1n_Set)
	{	
		// Set the new color
		for(U8 i=1;i<4;i++)
		{
			memory_map[MaCaco_OUT_s + slot + i] = memory_map[MaCaco_IN_s + slot + i];
			memory_map[MaCaco_IN_s + slot + i] = Souliss_T1n_RstCmd;
		}
		
		memory_map[MaCaco_OUT_s + slot] = Souliss_T1n_OnCoil;			// Switch on the output
		memory_map[MaCaco_IN_s + slot] = Souliss_T1n_RstCmd;			// Reset		
	}
}

/**************************************************************************
/*!
	Timer associated to T16
*/	
/**************************************************************************/
void Souliss_T16_Timer(U8 *memory_map, U8 slot)
{
	if(memory_map[MaCaco_IN_s + slot] > Souliss_T1n_Timed)		// Memory value is used as timer
	{
		memory_map[MaCaco_OUT_s + slot] = Souliss_T1n_GoodNight;
	
		// Decrease timer and check the expiration
		if((--memory_map[MaCaco_IN_s + slot]) == Souliss_T1n_Timed)		
			memory_map[MaCaco_IN_s + slot] = Souliss_T1n_OffCmd;
		
		// Bright down the light
		for(U8 i=1;i<4;i++)
			if(memory_map[MaCaco_OUT_s + slot + i])
				memory_map[MaCaco_OUT_s + slot + i]--;
			else
				memory_map[MaCaco_OUT_s + slot + i] = 0;
		
		// If there is no more light, declare it off
		if((memory_map[MaCaco_OUT_s + slot + 1] == Souliss_T1n_OffCoil) && (memory_map[MaCaco_OUT_s + slot + 2] == Souliss_T1n_OffCoil) 
			&& (memory_map[MaCaco_OUT_s + slot + 3] == Souliss_T1n_OffCoil))
		{
			memory_map[MaCaco_IN_s + slot] = Souliss_T1n_OffCmd;		// Reset
		}		
	}	
}

/**************************************************************************
/*!
	Define the use of Typical 21 : Motorized devices with limit switches
*/	
/**************************************************************************/
void Souliss_SetT21(U8 *memory_map, U8 slot)
{
	memory_map[MaCaco_TYP_s + slot] = Souliss_T21;
}

/**************************************************************************
/*!
	Typical 21 : Motorized devices with limit switches
	
		It handle OPEN / CLOSE devices with alternate direction, an unique
		command is available and toogle the device status to OPEN or CLOSE
		based on the previous state, moving throught the STOP state.
		Is used for garage doors or generally devices that need to be fully
		opened or closed.
		
		Once a command, it remain active until the relevant limit switch is
		recognized or the associated timer is expired. Timer must be used
		for proper release of command.
		Limit switches are not mandatory, but if not used, the device engine
		shall be protected from extracurrent or shall have its own control
		center.
		
		Hardware Limit Switches :
		
			Using bistable switches for identification of open and close position			
				#define Souliss_T2n_LimSwitch_Close	0x08
				#define Souliss_T2n_LimSwitch_Open	0x10

		Hardware and/or Software Command:
			
			Using a monostable wall switch (press and spring return) or a 
			software command from user interface, each press will toogle 
			the output status.		
				#define Souliss_T2n_ToogleCmd		0x04
				
			Optionally are available direct OPEN/CLOSE/STOP commands	
				#define Souliss_T2n_CloseCmd		0x01
				#define Souliss_T2n_OpenCmd			0x02
				#define Souliss_T2n_StopCmd			0x03
				
		Output status:
		- 1(hex) for CLOSE,
		- 2(hex) for OPEN,
		- 3(hex) for STOP.
	
*/	
/**************************************************************************/
void Souliss_Logic_T21(U8 *memory_map, U8 slot, U8 *trigger)
{
	// Look for input value, update output. If the output is not set, trig a data
	// change, otherwise just reset the input
	
	if((memory_map[MaCaco_IN_s + slot] == Souliss_T2n_ToogleCmd) || (memory_map[MaCaco_IN_s + slot] == Souliss_T2n_StopCmd) || (memory_map[MaCaco_IN_s + slot] == Souliss_T2n_OpenCmd) || (memory_map[MaCaco_IN_s + slot] == Souliss_T2n_CloseCmd))
	{
		if(memory_map[MaCaco_IN_s + slot] == Souliss_T2n_ToogleCmd)
		{		
			// Change the output value, between OPEN and CLOSE always OFF is performed	
			if((memory_map[MaCaco_OUT_s + slot] == Souliss_T2n_Coil_Close) || (memory_map[MaCaco_OUT_s + slot] == Souliss_T2n_Coil_Open))
			{
				memory_map[MaCaco_AUXIN_s + slot] = memory_map[MaCaco_OUT_s + slot];	// Save actual state	
				memory_map[MaCaco_OUT_s + slot] = Souliss_T2n_Coil_Stop;				// Off Command
			}
			else if((memory_map[MaCaco_AUXIN_s + slot] == Souliss_T2n_Coil_Close) || (memory_map[MaCaco_OUT_s + slot] == Souliss_T2n_LimSwitch_Close))
				memory_map[MaCaco_OUT_s + slot] = Souliss_T2n_Coil_Open;			// Open Command
			else if((memory_map[MaCaco_AUXIN_s + slot] == Souliss_T2n_Coil_Open) || (memory_map[MaCaco_OUT_s + slot] == Souliss_T2n_LimSwitch_Open))
				memory_map[MaCaco_OUT_s + slot] = Souliss_T2n_Coil_Close;			// Close command
			else
				memory_map[MaCaco_OUT_s + slot] = Souliss_T2n_Coil_Open;			// If state is undefined, Open Command
		}
		else
		{
			// Perform the requested action
			if((memory_map[MaCaco_IN_s + slot] == Souliss_T2n_StopCmd) || (memory_map[MaCaco_OUT_s + slot] == Souliss_T2n_Coil_Close) || (memory_map[MaCaco_OUT_s + slot] == Souliss_T2n_Coil_Open))
				memory_map[MaCaco_OUT_s + slot] = Souliss_T2n_Coil_Stop;				// Stop Command
			else if((memory_map[MaCaco_IN_s + slot] == Souliss_T2n_OpenCmd) && (memory_map[MaCaco_OUT_s + slot] != Souliss_T2n_LimSwitch_Open))
				memory_map[MaCaco_OUT_s + slot] = Souliss_T2n_Coil_Open;			// Open Command
			else if((memory_map[MaCaco_IN_s + slot] == Souliss_T2n_CloseCmd) && (memory_map[MaCaco_OUT_s + slot] != Souliss_T2n_LimSwitch_Close))
				memory_map[MaCaco_OUT_s + slot] = Souliss_T2n_Coil_Close;			// Close command
		}
		
		// If a command was issued, set the timer
		if((memory_map[MaCaco_OUT_s + slot] == Souliss_T2n_Coil_Open) || (memory_map[MaCaco_OUT_s + slot] == Souliss_T2n_Coil_Close))
			memory_map[MaCaco_AUXIN_s + slot] = Souliss_T2n_Timer_Val;			// Set timer value
		
		memory_map[MaCaco_IN_s + slot] = Souliss_T2n_RstCmd;					// Reset
  		*trigger = Souliss_TRIGGED;
	}
	else if((memory_map[MaCaco_IN_s + slot] == Souliss_T2n_LimSwitch_Close) || ((memory_map[MaCaco_OUT_s + slot] == Souliss_T2n_Coil_Close) && (memory_map[MaCaco_AUXIN_s + slot] == Souliss_T2n_Timer_Off)))
	{
		memory_map[MaCaco_OUT_s + slot] = Souliss_T2n_LimSwitch_Close;			// Close Limit Switch
		memory_map[MaCaco_IN_s + slot] = Souliss_T2n_RstCmd;					// Reset
  		*trigger = Souliss_TRIGGED;
	}
	else if((memory_map[MaCaco_IN_s + slot] == Souliss_T2n_LimSwitch_Open) || ((memory_map[MaCaco_OUT_s + slot] == Souliss_T2n_Coil_Open) && (memory_map[MaCaco_AUXIN_s + slot] == Souliss_T2n_Timer_Off)))
	{
		memory_map[MaCaco_OUT_s + slot] = Souliss_T2n_LimSwitch_Open;			// Open Limit Switch
		memory_map[MaCaco_IN_s + slot] = Souliss_T2n_RstCmd;					// Reset
  		*trigger = Souliss_TRIGGED;	
	}
}

/**************************************************************************
/*!
	Timer associated to T21, timeout the OPEN/CLOSE commands
*/	
/**************************************************************************/
void Souliss_T21_Timer(U8 *memory_map, U8 slot)
{
	if(memory_map[MaCaco_AUXIN_s + slot] > Souliss_T2n_Timer_Off)				// Memory value is used as timer
		memory_map[MaCaco_AUXIN_s + slot]--;				// Decrease timer
}

/**************************************************************************
/*!
	Define the use of Typical 22 : Motorized devices with limit switches
*/	
/**************************************************************************/
void Souliss_SetT22(U8 *memory_map, U8 slot)
{
	memory_map[MaCaco_TYP_s + slot] = Souliss_T22;
}

/**************************************************************************
/*!
	Typical 22 : Motorized devices with limit switches and middle position
	
		It handle OPEN / CLOSE devices with alternate direction, dedicated
		commands for OPEN, CLOSE and STOP are available. Every state change
		goes throught the STOP command for security reason.
		Is used for curtains or other devices that need an adjustable position.
		
		Once a command, it remain active until the relevant limit switch is
		recognized or the associated timer is expired. Timer must be used
		for proper release of command.
		Limit switches are not mandatory, but if not used, the device engine
		shall be protected from extracurrent or shall have its own control
		center.
		
		Hardware Limit Switches :
		
			Using bistable switches for identification of open and close position			
				#define Souliss_T2n_LimSwitch_Close	0x08
				#define Souliss_T2n_LimSwitch_Open	0x10

		Hardware and/or Software Command:
			
			Using a monostable wall switch (press and spring return) or a 
			software command from user interface, each press will toogle 
			the output status.			
				#define Souliss_T2n_CloseCmd		0x01
				#define Souliss_T2n_OpenCmd			0x02
				#define Souliss_T2n_StopCmd			0x03
				
		Command recap, using: 
		-  1(hex) as command, CLOSE request (stop if opening) 
		-  2(hex) as command, OPEN request (stop if closing)
		-  3(hex) as command, STOP request
		
		Output status:
		- 1(hex) for CLOSE,
		- 2(hex) for OPEN,
		- 3(hex) for STOP.
	
*/	
/**************************************************************************/
void Souliss_Logic_T22(U8 *memory_map, U8 slot, U8 *trigger)
{
	// Look for input value, update output. If the output is not set, trig a data
	// change, otherwise just reset the input
	
	if((memory_map[MaCaco_IN_s + slot] == Souliss_T2n_CloseCmd) || (memory_map[MaCaco_IN_s + slot] == Souliss_T2n_OpenCmd) || (memory_map[MaCaco_IN_s + slot] == Souliss_T2n_StopCmd))
	{
		// Change the output value, between OPEN and CLOSE always OFF is performed	
		if((memory_map[MaCaco_IN_s + slot] == Souliss_T2n_StopCmd) || (memory_map[MaCaco_OUT_s + slot] == Souliss_T2n_Coil_Close) || (memory_map[MaCaco_OUT_s + slot] == Souliss_T2n_Coil_Open))
			memory_map[MaCaco_OUT_s + slot] = Souliss_T2n_Coil_Stop;			// Stop Command
		else if((memory_map[MaCaco_IN_s + slot] == Souliss_T2n_OpenCmd) && (memory_map[MaCaco_OUT_s + slot] != Souliss_T2n_LimSwitch_Open))
			memory_map[MaCaco_OUT_s + slot] = Souliss_T2n_Coil_Open;			// Open Command
		else if((memory_map[MaCaco_IN_s + slot] == Souliss_T2n_CloseCmd) && (memory_map[MaCaco_OUT_s + slot] != Souliss_T2n_LimSwitch_Close))
			memory_map[MaCaco_OUT_s + slot] = Souliss_T2n_Coil_Close;			// Close command
		
		// If a command was issued, set the timer
		if(memory_map[MaCaco_OUT_s + slot] != Souliss_T2n_Coil_Off)
		{
			memory_map[MaCaco_IN_s + slot] = Souliss_T2n_Timer_Val;			// Set timer value
			*trigger = Souliss_TRIGGED;		
		}
		else
			memory_map[MaCaco_IN_s + slot] = Souliss_T2n_RstCmd;					// Reset
	}
	else if((memory_map[MaCaco_IN_s + slot] == Souliss_T2n_LimSwitch_Close) || ((memory_map[MaCaco_OUT_s + slot] == Souliss_T2n_Coil_Close) && (memory_map[MaCaco_IN_s + slot] == Souliss_T2n_Timer_Off)))
	{
		memory_map[MaCaco_OUT_s + slot] = Souliss_T2n_LimSwitch_Close;			// Close Limit Switch
		memory_map[MaCaco_IN_s + slot] = Souliss_T2n_RstCmd;					// Reset
  		*trigger = Souliss_TRIGGED;
	}
	else if((memory_map[MaCaco_IN_s + slot] == Souliss_T2n_LimSwitch_Open) || ((memory_map[MaCaco_OUT_s + slot] == Souliss_T2n_Coil_Open) && (memory_map[MaCaco_IN_s + slot] == Souliss_T2n_Timer_Off)))
	{
		memory_map[MaCaco_OUT_s + slot] = Souliss_T2n_LimSwitch_Open;			// Open Limit Switch
		memory_map[MaCaco_IN_s + slot] = Souliss_T2n_RstCmd;					// Reset
  		*trigger = Souliss_TRIGGED;	
	}
	else if((memory_map[MaCaco_OUT_s + slot] == Souliss_T2n_Coil_Stop) && (memory_map[MaCaco_IN_s + slot] == Souliss_T2n_Timer_Off))
	{
		memory_map[MaCaco_OUT_s + slot] = Souliss_T2n_NoLimSwitch;				// No Limit Switch
		memory_map[MaCaco_IN_s + slot] = Souliss_T2n_RstCmd;					// Reset
  		*trigger = Souliss_TRIGGED;	
	}	
}

/**************************************************************************
/*!
	Timer associated to T22, timeout the OPEN/CLOSE commands
*/	
/**************************************************************************/
void Souliss_T22_Timer(U8 *memory_map, U8 slot)
{
	if(memory_map[MaCaco_IN_s + slot] > Souliss_T2n_Timer_Off)				// Memory value is used as timer
		memory_map[MaCaco_IN_s + slot]--;									// Decrease timer
}

/**************************************************************************
/*!
	Define the use of Typical 31 : Temperature control
*/	
/**************************************************************************/
void Souliss_SetT31(U8 *memory_map, U8 slot)
{
	memory_map[MaCaco_TYP_s + slot] = Souliss_T31;
	memory_map[MaCaco_TYP_s + slot + 1] = Souliss_TRL;
	memory_map[MaCaco_TYP_s + slot + 2] = Souliss_TRL;
	memory_map[MaCaco_TYP_s + slot + 3] = Souliss_TRL;
	memory_map[MaCaco_TYP_s + slot + 4] = Souliss_TRL;
}

/**************************************************************************
/*!
	Typical 31 : Temperature control with cooling and heating mode

		It compare an internal setpoint with a measured value to control
		a digital output for cooling or heating. Actual temperature and
		desired setpoint are stored and available for user interface or
		any other node in the network.
		
		The difference is caluclated as measured values (MV) less setpoint
		(SP) D = MV - SP, the output goes on based on the sign of this
		difference and on the selected mode (cooling / heating).
		
		This typical use five (5) memory slot, arranged as follow
			Temperature Control User Commands (IN / OUT) SLOT +0  
			Temperature Measured Value (IN / OUT) SLOT +1, SLOT +2
			Temperature Setpoint Value (IN / OUT) SLOT +3, SLOT +4
		
		all values shall be in half-precision floating point, automatic
		conversion is done if using Souliss_AnalogIn
		
		Hardware and/or Software Command:
			
			Using a monostable wall switch (press and spring return) or a 
			software command from user interface, to increase or decrease
			the setpoint.
				#define Souliss_T3n_InSetPoint			0x01
				#define Souliss_T3n_OutSetPoint			0x02
				#define Souliss_T3n_AsMeasured			0x03
				#define Souliss_T3n_Cooling				0x04
				#define Souliss_T3n_Heating				0x05
				#define Souliss_T3n_FanOff				0x06
				#define Souliss_T3n_FanLow				0x07
				#define Souliss_T3n_FanMed				0x08
				#define Souliss_T3n_FanHigh				0x09
				#define Souliss_T3n_FanAuto				0x0A
				#define Souliss_T3n_FanManual			0x0B
				
		Setpoint and measured values can be provided via analog input acquisition
		using Souliss_AnalogIn method, the setpoint can be modified using the In(+)
		and Out(-) commands, that will decrease of one unit the actual setpoint.
		
		The outputs are available as control state in the SLOT +0 and as used
		measured value and setpoint in next four slots (as per inputs).
		
		The control state bit meaning follow as:
			BIT 0	Not used 
			BIT 1	(0 Heating OFF , 1 Heating ON)
			BIT 2	(0 Cooling OFF , 1 Cooling ON)
			BIT 3	(0 Fan 1 OFF   , Fan 1 ON)
			BIT 4	(0 Fan 2 OFF   , Fan 2 ON)
			BIT 5	(0 Fan 3 OFF   , Fan 3 ON)
			BIT 6	(0 Manual Mode , 1 Automatic Mode for Fan) 
			BIT 7	(0 Heating Mode, 1 Cooling Mode)		
			
		Using the Souliss_nDigOut method matching with following state defines
		let control heating, cooling and fans.
				#define Souliss_T3n_HeatingOn			0x02
				#define Souliss_T3n_CoolingOn			0x03
				#define Souliss_T3n_FanOn1				0x08
				#define Souliss_T3n_FanOn2				0x10
				#define Souliss_T3n_FanOn3				0x20	
*/	
/**************************************************************************/
void Souliss_Logic_T31(U8 *memory_map, U8 slot, U8 *trigger)
{
	float actual_temp, actual_setpnt, in_temp, in_setpnt;

	// Convert the stored values in single precision floating points
	float32((U16*)(memory_map + MaCaco_IN_s + slot + 1), &in_temp);
	float32((U16*)(memory_map + MaCaco_IN_s + slot + 3), &in_setpnt);
	float32((U16*)(memory_map + MaCaco_OUT_s + slot + 1), &actual_temp);
	float32((U16*)(memory_map + MaCaco_OUT_s + slot + 3), &actual_setpnt);
	
	// Store actual value as difference with requested setpoint
	if(*(U16*)(memory_map + MaCaco_IN_s + slot + 1) != Souliss_T3n_RstCmd)
	{		
		// If there is a too small change in the new temperature
		if(abs((in_temp-actual_temp)/actual_temp) > Souliss_T3n_DeadBand)
			actual_temp = in_temp;													// Set the new temperature value

		*(U16*)(memory_map + MaCaco_IN_s + slot + 1) = Souliss_T3n_RstCmd;		// Reset	
	}
	
	// Trig the next change of the state
	*trigger = Souliss_TRIGGED;	
		
	// Check the actual operational mode (Cooling / Heating)
	if(!(memory_map[MaCaco_OUT_s + slot] & Souliss_T3n_HeatingMode))
	{
		// Heating Mode
		if(((actual_temp-actual_setpnt)/actual_temp) < -Souliss_T3n_DeadBand)
			memory_map[MaCaco_OUT_s + slot] |= Souliss_T3n_HeatingOn;	// Active the heating 
		else if(((actual_temp-actual_setpnt)/actual_temp) > Souliss_T3n_DeadBand)
			memory_map[MaCaco_OUT_s + slot] &= ~Souliss_T3n_HeatingOn;	// Stop the heating 
		else
			*trigger = Souliss_NOTTRIGGED;								// No action, no need for trig	
	}
	else if(memory_map[MaCaco_OUT_s + slot] & Souliss_T3n_CoolingMode)
	{
		// Cooling Mode
		if(((actual_temp-actual_setpnt)/actual_temp) > Souliss_T3n_DeadBand)
			memory_map[MaCaco_OUT_s + slot] |= Souliss_T3n_CoolingOn;	// Active the cooling 
		else if(((actual_temp-actual_setpnt)/actual_temp) < -Souliss_T3n_DeadBand)
			memory_map[MaCaco_OUT_s + slot] &= ~Souliss_T3n_CoolingOn;	// Stop the cooling 
		else
			*trigger = Souliss_NOTTRIGGED;								// No action, no need for trig			
	}

	// Check the fan mode (Manual / Auto)
	if(memory_map[MaCaco_OUT_s + slot] & Souliss_T3n_FanAutoState)
	{
		float deviation = abs((actual_temp-actual_setpnt)/actual_temp);
		
		if(deviation > Souliss_T3n_ThHigh)
			memory_map[MaCaco_OUT_s + slot] |= (Souliss_T3n_FanOn1 | Souliss_T3n_FanOn2 | Souliss_T3n_FanOn3);	// Active Fan1 + Fan2 + Fan3
		else if(deviation > Souliss_T3n_ThMed)
		{
			memory_map[MaCaco_OUT_s + slot] |= (Souliss_T3n_FanOn1 | Souliss_T3n_FanOn2);						// Active Fan1 + Fan2
			memory_map[MaCaco_OUT_s + slot] &= ~Souliss_T3n_FanOn3;
		}
		else if(deviation > Souliss_T3n_DeadBand)
		{
			memory_map[MaCaco_OUT_s + slot] |= (Souliss_T3n_FanOn1);											// Active Fan1
			memory_map[MaCaco_OUT_s + slot] &= ~(Souliss_T3n_FanOn2 | Souliss_T3n_FanOn3);
		}
		else
			memory_map[MaCaco_OUT_s + slot] &= ~(Souliss_T3n_FanOn1 | Souliss_T3n_FanOn2 | Souliss_T3n_FanOn3);	// Stop Fan1 + Fan2 + Fan3
	}
	
	// If new setpoint is available, store it
	if(memory_map[MaCaco_IN_s + slot] != Souliss_T3n_RstCmd)
	{	
		// Check out the setpoint request
		if(memory_map[MaCaco_IN_s + slot] == Souliss_T3n_InSetPoint)
			actual_setpnt++;														// Increase of 1 degree
		else if(memory_map[MaCaco_IN_s + slot] == Souliss_T3n_OutSetPoint)
			actual_setpnt--;														// Decrease of 1 degree
		else if(memory_map[MaCaco_IN_s + slot] == Souliss_T3n_AsMeasured)
			actual_setpnt = actual_temp;											// As actual temperature
		else if(memory_map[MaCaco_IN_s + slot] == Souliss_T3n_Cooling)
			memory_map[MaCaco_OUT_s + slot] |= Souliss_T3n_CoolingMode;				// Set Cooling Mode
		else if(memory_map[MaCaco_IN_s + slot] == Souliss_T3n_Heating)
			memory_map[MaCaco_OUT_s + slot] |= Souliss_T3n_HeatingMode;				// Set Heating Mode
		else if(memory_map[MaCaco_IN_s + slot] == Souliss_T3n_FanAuto)
			memory_map[MaCaco_OUT_s + slot] |= Souliss_T3n_FanAutoState;			// Set Fan in Automatic Mode
		else if(memory_map[MaCaco_IN_s + slot] == Souliss_T3n_FanManual)
			memory_map[MaCaco_OUT_s + slot] &= ~Souliss_T3n_FanAutoState;			// Set Fan in Manual Mode				
		else if(memory_map[MaCaco_IN_s + slot] == Souliss_T3n_FanOff)
			memory_map[MaCaco_OUT_s + slot] &= ~(Souliss_T3n_FanOn1 | Souliss_T3n_FanOn2 | Souliss_T3n_FanOn3);	// Stop Fan1 + Fan2 + Fan3
		else if(memory_map[MaCaco_IN_s + slot] == Souliss_T3n_FanLow)
		{
			memory_map[MaCaco_OUT_s + slot] |= (Souliss_T3n_FanOn1);											// Active Fan1
			memory_map[MaCaco_OUT_s + slot] &= ~(Souliss_T3n_FanOn2 | Souliss_T3n_FanOn3);
		
		}
		else if(memory_map[MaCaco_IN_s + slot] == Souliss_T3n_FanMed)
		{
			memory_map[MaCaco_OUT_s + slot] |= (Souliss_T3n_FanOn1 | Souliss_T3n_FanOn2);						// Active Fan1 + Fan2
			memory_map[MaCaco_OUT_s + slot] &= ~Souliss_T3n_FanOn3;
		
		}
		else if(memory_map[MaCaco_IN_s + slot] == Souliss_T3n_FanHigh)		
			memory_map[MaCaco_OUT_s + slot] |= (Souliss_T3n_FanOn1 | Souliss_T3n_FanOn2 | Souliss_T3n_FanOn3);	// Active Fan1 + Fan2 + Fan3

			
		memory_map[MaCaco_IN_s + slot] = Souliss_T3n_RstCmd;					// Reset
		*trigger = Souliss_TRIGGED;		
	}		
	
	// Convert the processed values in half precision floating points
	float16((U16*)(memory_map + MaCaco_OUT_s + slot + 1), &actual_temp);
	float16((U16*)(memory_map + MaCaco_OUT_s + slot + 3), &actual_setpnt);
		
}

/**************************************************************************
/*!
	Define the use of Typical 32 : Air Conditioner
*/	
/**************************************************************************/
void Souliss_SetT32(U8 *memory_map, U8 slot)
{
	memory_map[MaCaco_TYP_s + slot] = Souliss_T32;
	memory_map[MaCaco_TYP_s + slot + 1] = Souliss_TRL;
}

/**************************************************************************
/*!
	Typical 32 : Air Conditioner

		It map output over inputs matching the user interface commands,
		the logic is handles by the air conditioner itself. This typical
		is a remote control for the conditioner and require a mapping of
		the typical commands and the expected commands on the conditioner.
		
		Software Commands:
			
			A command is long two byte and is composed by the several option
			of a general air conditioner, each command contains the whole
			setup of the conditioner including: temperature, fan and options.
			
			Each group is composed by 4 bits, 
			
			0xABCD
				Group A - General
					(OnCmd, OffCmd, Ionizer, PwrSave)
				
				Group B - Fan / Swirl (3 bits for options, 1 bit activation)
					(Auto/High/Med/Low, Fan ON/OFF) 
				
				Group C - Function
					(Auto/Cool/Dry/Fan/Heat)
				
				Group D - Temperature
					(from 16°C to 30°C, encoded) 
	
			In the definitions are available the commands that shall be mapped
			versus the air conditioner.
			
			The mapping to the IR Led Emitter shall be filtered using the method
			U8 Souliss_isTrigged(U8 *memory_map, U8 slot) to ensure that command
			are sent just once.
	
		This typical use two (2) memory slots, both contains the air conditioner
		commands.
			
*/	
/**************************************************************************/
void Souliss_Logic_T32(U8 *memory_map, U8 slot, U8 *trigger)
{
	// If there is an input command, process it
	if(memory_map[MaCaco_IN_s + slot] != Souliss_T3n_AirCon_RstCmd)	
	{	
		// If the input command is different from the last executed command, trig a change
		if(memory_map[MaCaco_OUT_s + slot] != memory_map[MaCaco_IN_s + slot])  
		{
			memory_map[MaCaco_AUXIN_s + slot] = Souliss_TRIGGED;
			*trigger = Souliss_TRIGGED;	// Trig change
		}
		
		// Load the command in the output slot
		memory_map[MaCaco_OUT_s + slot] = memory_map[MaCaco_IN_s + slot];	
		memory_map[MaCaco_OUT_s + slot + 1] = memory_map[MaCaco_IN_s + slot + 1];	
		
		/*
		At this stage the requested command is available in the outputs, a mapping between
		these command and what is expected by the air conditioner device shall be used just
		after this method.
		*/
		
		memory_map[MaCaco_IN_s + slot] = Souliss_T3n_AirCon_RstCmd;			// Reset
			
	}	
}

/**************************************************************************
/*!
	Define the use of Typical 41 : Anti-theft Integration (Main)
*/	
/**************************************************************************/
void Souliss_SetT41(U8 *memory_map, U8 slot)
{
	memory_map[MaCaco_TYP_s + slot] = Souliss_T41;
}

/**************************************************************************
/*!
	Typical 41 : Anti-theft integration (Main)
	
		It active an output when an input from a sensor or a chain of nodes
		detect an intrusion or any other anti-theft cause.
		
		The chain can be build usign the anti-theft peers (typical 42) that
		write an alarm condition into the anti-theft main node. Is mandatory
		that main and peer nodes in the anti-theft chain use the same slot.
		
		Additionally a chain of watch-dog could be used as input for the
		anti-theft logics, so that a missing node in the chain will be addressed
		as anti-theft activation cause.
		
		Hardware and/or Software Command:
			
			Using a two state switch or a software command
				#define Souliss_T4n_Alarm			0x01
				#define Souliss_T4n_ReArm			0x03
				#define Souliss_T4n_NotArmed		0x04
				#define Souliss_T4n_Armed			0x05
						
*/	
/**************************************************************************/
U8 Souliss_Logic_T41(U8 *memory_map, U8 slot, U8 *trigger)
{
	// Active or deactive the anti-theft
	if((memory_map[MaCaco_IN_s + slot] == Souliss_T4n_Armed) || (memory_map[MaCaco_IN_s + slot] == Souliss_T4n_ReArm))
	{
		memory_map[MaCaco_IN_s + slot] = Souliss_T4n_RstCmd;			
		memory_map[MaCaco_OUT_s + slot] = Souliss_T4n_Antitheft;	// Activate the anti-theft
		*trigger = Souliss_TRIGGED;	
	}	
	else if (memory_map[MaCaco_IN_s + slot] == Souliss_T4n_NotArmed)
	{
		memory_map[MaCaco_IN_s + slot] = Souliss_T4n_RstCmd;					
		memory_map[MaCaco_OUT_s + slot] = Souliss_T4n_NoAntitheft;	// Deactivate the anti-theft
		*trigger = Souliss_TRIGGED;	
	}	
	
	// If the anti-theft is set and there is an alarm, set the output	
	if((memory_map[MaCaco_OUT_s + slot] == Souliss_T4n_Antitheft) && (memory_map[MaCaco_IN_s + slot] == Souliss_T4n_Alarm))
	{
		memory_map[MaCaco_OUT_s + slot] = Souliss_T4n_InAlarm;
		memory_map[MaCaco_IN_s + slot] = Souliss_T4n_RstCmd;
		*trigger = Souliss_TRIGGED;	
	}

	return memory_map[MaCaco_OUT_s + slot];	
}


/**************************************************************************
/*!
	Timer associated to T41, delayed activation of the main anti-theft
*/	
/**************************************************************************/
void Souliss_T41_Timer(U8 *memory_map, U8 slot)
{
	if(memory_map[MaCaco_IN_s + slot] > Souliss_T4n_Armed)				// Memory value is used as timer
		memory_map[MaCaco_IN_s + slot]--;								// Decrease timer
}	


/**************************************************************************
/*!
	Define the use of Typical 42 : Anti-theft Integration (Peer)
*/	
/**************************************************************************/
void Souliss_SetT42(U8 *memory_map, U8 slot)
{
	memory_map[MaCaco_TYP_s + slot] = Souliss_T42;
}

/**************************************************************************
/*!
	Typical 42 : Anti-theft integration (Peer)
	
		It active an output when an input from a sensor or a chain of nodes
		detect an intrusion or any other anti-theft cause.
		
		The chain can be build usign the anti-theft peers (typical 42) that
		write an alarm condition into the anti-theft main node. Is mandatory
		that main and peer nodes in the anti-theft chain use the same slot.
		
		Additionally a chain of watch-dog could be used as input for the
		anti-theft logics, so that a missing node in the chain will be addressed
		as anti-theft activation cause.
		
		Hardware and/or Software Command:
			
			Using a two state switch or a software command from user other nodes
				#define Souliss_T4n_Alarm			0x01

*/	
/**************************************************************************/
U8 Souliss_Logic_T42(U8 *memory_map, U8 slot, U8 *trigger, U16 main_antitheft_address)
{
	// If the anti-theft is set and there is an alarm, set the output	
	if((memory_map[MaCaco_IN_s + slot] == Souliss_T4n_Alarm))
	{
		memory_map[MaCaco_OUT_s + slot] = Souliss_T4n_Alarm;
		memory_map[MaCaco_IN_s + slot] = Souliss_T4n_RstCmd;

		return MaCaco_send(main_antitheft_address, MaCaco_FORCEREGSTR, 0x00, MaCaco_IN_s + slot, 1, &memory_map[MaCaco_OUT_s + slot]);
	}
	else
		return memory_map[MaCaco_OUT_s + slot] = Souliss_T4n_RstCmd;	
}

/**************************************************************************
/*!
	Define the use of Typical 51 : Analog input, half-precision floating point
*/	
/**************************************************************************/
void Souliss_SetT51(U8 *memory_map, U8 slot)
{
	memory_map[MaCaco_TYP_s + slot] = Souliss_T51;
	memory_map[MaCaco_TYP_s + slot + 1] = Souliss_TRL;
}

/**************************************************************************
/*!
	Typical 51 : Analog input, half-precision floating point
	
		The inputs value, coming from an analog source is stored and notified
		to the user interface or any other subscriber if the data are changing
		out of an user defined threshold.
	
		The analog values shall be a floating point and must be converted in
		half-precision floating point using Souliss.
	
		This typical use two (2) memory slots
*/	
/**************************************************************************/
U8 Souliss_Logic_T51(U8 *memory_map, U8 slot, const float deadband, U8 *trigger)
{
	float m_in, m_out;
	
	// If there is a new values, compare it against the previous one
	if((memory_map[MaCaco_IN_s + slot] != 0) || (memory_map[MaCaco_IN_s + slot + 1] != 0))
	{	
		// Values are actually stored as half-precision floating point, this doesn't
		// allow math operation, so convert them back to single-precision
		float32((U16*)(memory_map + MaCaco_IN_s + slot), &m_in);
		float32((U16*)(memory_map + MaCaco_OUT_s + slot), &m_out);
			
		if(abs((m_in - m_out)/m_in) > deadband)
		{
			// Store the new value
			memory_map[MaCaco_OUT_s + slot] = memory_map[MaCaco_IN_s + slot];
			memory_map[MaCaco_OUT_s + slot + 1] = memory_map[MaCaco_IN_s + slot + 1];
					
			memory_map[MaCaco_IN_s + slot] = 0;			// Reset
			memory_map[MaCaco_IN_s + slot + 1] = 0;		// Reset
			
			// Trigger
			*trigger = Souliss_TRIGGED;		
		}
	}	
}

/**************************************************************************
/*!
	Typical 5n : Pre-defined Analog Input	
	
		Souliss support for analog values use a single rapresentation as
		half-duplex floating point, at same time some pre-defined analog
		input typical (pressure, temperature, humidity, ...) are defined
		in order to standardize the ranges and the user interface management.
		
		The following pre-defined analog inputs are available,
			Souliss_Logic_T52 - Temperature measure (-20, +50) °C
			Souliss_Logic_T53 - Humidity measure (0, 100) %
			Souliss_Logic_T54 - Pressure measure (0, 1500) hPa
			Souliss_Logic_T55 - Voltage (0, 400) V
			Souliss_Logic_T56 - Current (0, 25)  A
			Souliss_Logic_T57 - Power (0, 6500)  W
			Souliss_Logic_T58 - 
			Souliss_Logic_T59 - 
			
		All typicals between T52 and T59 are handled with T51, simply the value
		at user interface side will be clamped into the pre-defined ranges, having
		known unit measure.		
		
		All code for T52 up to T59 is defined using macro in Typicals.h			
*/	
/**************************************************************************/
void Souliss_SetT5n(U8 *memory_map, U8 slot, U8 typ)
{
	memory_map[MaCaco_TYP_s + slot] = typ;
	memory_map[MaCaco_TYP_s + slot + 1] = Souliss_TRL;
}