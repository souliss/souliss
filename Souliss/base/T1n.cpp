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
	memory_map[MaCaco_TYP_s + slot + 1] = Souliss_TRL;	
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
			
		It use two memory slots	
			
*/	
/**************************************************************************/
void Souliss_Logic_T15(U8 *memory_map, U8 slot, U8 *trigger)
{
	// Before power OFF store the last state
	if(memory_map[MaCaco_IN_s + slot] == Souliss_T1n_RGBLamp_OffCmd )
		memory_map[MaCaco_AUXIN_s + slot + 1] = memory_map[MaCaco_OUT_s + slot];
	
	// If previosly there was an ON command, set the last state
	if((memory_map[MaCaco_OUT_s + slot] == Souliss_T1n_RGBLamp_OnCmd) && (memory_map[MaCaco_AUXIN_s + slot + 1]))
	{
		memory_map[MaCaco_OUT_s + slot] = memory_map[MaCaco_AUXIN_s + slot + 1];
		
		// Flag the change in the state
		memory_map[MaCaco_AUXIN_s + slot] = Souliss_TRIGGED;
		*trigger = Souliss_TRIGGED;
	}
	else if(memory_map[MaCaco_IN_s + slot] != Souliss_T1n_RGBLamp_RstCmd) 
	{
		// Use the incoming command
		memory_map[MaCaco_OUT_s + slot] = memory_map[MaCaco_IN_s + slot];
		
		// Flag the change in the state
		memory_map[MaCaco_AUXIN_s + slot] = Souliss_TRIGGED;
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
		else if(memory_map[MaCaco_OUT_s + slot] == Souliss_T1n_OnCoil)
			memory_map[MaCaco_IN_s + slot] = Souliss_T1n_OffCmd;
		else
			memory_map[MaCaco_IN_s + slot] = Souliss_T1n_RstCmd;
		
		// Trig the change
		*trigger = Souliss_TRIGGED;	
	}
	else if (memory_map[MaCaco_IN_s + slot] == Souliss_T1n_OffCmd)		// Off Command
	{
		// Trigger the change and save the actual color
		if(memory_map[MaCaco_OUT_s + slot] != Souliss_T1n_OffCoil)  
		{				
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
			else if((memory_map[MaCaco_OUT_s + slot + 1] == 0)
				&& (memory_map[MaCaco_OUT_s + slot + 2]  == 0) 
				&& (memory_map[MaCaco_OUT_s + slot + 3]  == 0))
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
			memory_map[MaCaco_AUXIN_s + slot + i] = memory_map[MaCaco_OUT_s + slot + i];
			memory_map[MaCaco_IN_s + slot + i] = Souliss_T1n_RstCmd;
		}
		
		memory_map[MaCaco_AUXIN_s + slot] = Souliss_T1n_Timed;			// Set a timer for the state notification
		memory_map[MaCaco_OUT_s + slot] = Souliss_T1n_OnCoil;			// Switch on the output
		memory_map[MaCaco_IN_s + slot] = Souliss_T1n_RstCmd;			// Reset		
	}
	else
	{	// There is no command
		
		if(memory_map[MaCaco_AUXIN_s + slot] > Souliss_T1n_Set)			// Decrese the timer value
			memory_map[MaCaco_AUXIN_s + slot]--;
		else if(memory_map[MaCaco_AUXIN_s + slot] > 0)					// If we not getting new commands, the burst		
		{																// is done, send the actual state 
			memory_map[MaCaco_AUXIN_s + slot] = 0;
			*trigger = Souliss_TRIGGED;									
		}	
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
	Define the use of Typical 18 : N/OFF Digital Output with pulse output 
	with Timer Option
*/	
/**************************************************************************/
void Souliss_SetT18(U8 *memory_map, U8 slot)
{
	memory_map[MaCaco_TYP_s + slot] = Souliss_T18;
}

/**************************************************************************
/*!
	Typical 18 : ON/OFF Digital Output
	
		Handle one digital output based on hardware and software commands,
		output can be timed out.
		
		This logic can be used for lights, wall socket and all the devices
		that has an ON/OFF behavior. It has a pulsed output, and can be 
		used with bistable relay or similar devices.
		
		The actual state shall be reported with an external digital input
		such as an auxiliary contact or a current measure.
		
		Hardware Command:
			
			If using a monostable wall switch (press and spring return), 
			each press will toogle the output status.		
				#define Souliss_T1n_ToogleCmd		0x01
			
			If using a bistable wall switch (press without return), the two
			switch position can be associated with the ON and OFF commands
				#define Souliss_T1n_OnCmd			0x02
				#define Souliss_T1n_OffCmd			0x04
				
			The actual state shall be reported using these two feedbacks in
			relevant INPUT slot.
				#define Souliss_T1n_OnFeedback			0x23		
				#define Souliss_T1n_OffFeedback			0x24		
					
		Software Commands:		
			
			From any available software interface, these commands will turn
			the light ON and OFF.
				#define Souliss_T1n_OnCmd			0x02
				#define Souliss_T1n_OffCmd			0x04
			
		Command recap, using: 
		-  1(hex) as command, toogle the output 
		-  2(hex) as command, the output move to ON
		-  4(hex) as command, the output move to OFF
		-  0(hex) as command, no action

		Output status,
		-  0(hex) for state OFF,
		-  1(hex) for state ON,
		- A1(hex) for pulsed output.
		
*/	
/**************************************************************************/
void Souliss_Logic_T18(U8 *memory_map, U8 slot, U8 *trigger)
{
	// Look for input value, update output. If the output is not set, trig a data
	// change, otherwise just reset the input
	
	if ((memory_map[MaCaco_IN_s + slot] == Souliss_T1n_OffFeedback) &&
			(memory_map[MaCaco_OUT_s + slot] == Souliss_T1n_ResetCoil))	// If the pulse is expired then
	{                                                                   // set the State Feedback as OFF
		if(memory_map[MaCaco_AUXIN_s + slot] != Souliss_T1n_OffFeedback)  
			*trigger = Souliss_TRIGGED;
	
		memory_map[MaCaco_OUT_s + slot] = Souliss_T1n_OffFeedback;		// State Feedback as OFF
		memory_map[MaCaco_AUXIN_s + slot] = Souliss_T1n_OffFeedback;		// Store the actual State Feedback	
		memory_map[MaCaco_IN_s + slot] = Souliss_T1n_RstCmd;			// Reset
	}
	else if ((memory_map[MaCaco_IN_s + slot] == Souliss_T1n_OnFeedback) &&
			(memory_map[MaCaco_OUT_s + slot] == Souliss_T1n_ResetCoil))	// If the pulse is expired then
	{                                                                   // set the State Feedback as OFF
		if(memory_map[MaCaco_OUT_s + slot] != Souliss_T1n_OnFeedback)  
			*trigger = Souliss_TRIGGED;	
	
		memory_map[MaCaco_OUT_s + slot] = Souliss_T1n_OnFeedback;		// State Feedback as OFF
		memory_map[MaCaco_AUXIN_s + slot] = Souliss_T1n_OnFeedback;		// Store the actual State Feedback	
		memory_map[MaCaco_IN_s + slot] = Souliss_T1n_RstCmd;			// Reset
	}
	else if ((memory_map[MaCaco_IN_s + slot] == Souliss_T1n_ToogleCmd))			
	{
		memory_map[MaCaco_OUT_s + slot] = Souliss_T1n_PulseCoil;		// Change output state
	}
	else if ((memory_map[MaCaco_IN_s + slot] == Souliss_T1n_OnCmd))			
	{
		if (memory_map[MaCaco_AUXIN_s + slot] == Souliss_T1n_OffFeedback)
			memory_map[MaCaco_OUT_s + slot] = Souliss_T1n_PulseCoil;	// Change output state if previously at OFF
	}
	else if ((memory_map[MaCaco_IN_s + slot] == Souliss_T1n_OffCmd))			
	{
		if (memory_map[MaCaco_AUXIN_s + slot] == Souliss_T1n_OnFeedback)
			memory_map[MaCaco_OUT_s + slot] = Souliss_T1n_PulseCoil;	// Change output state if previously at ON
	}	
	else if (memory_map[MaCaco_OUT_s + slot] > Souliss_T1n_ResetCoil)
		memory_map[MaCaco_OUT_s + slot]--;
}

/**************************************************************************
/*!
	Define the use of Typical 19 : RGB LED Strip
*/	
/**************************************************************************/
void Souliss_SetT19(U8 *memory_map, U8 slot)
{
	memory_map[MaCaco_TYP_s + slot] = Souliss_T19;
	memory_map[MaCaco_TYP_s + slot + 1] = Souliss_TRL;
}

/**************************************************************************
/*!
	Typical 19 : Single Color LED Strip

		Handle one single color LED strip based on software commands, with automatic 
		fade effects and "Good Night" feature. It use two (2) SLOT.					
		
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
			
			Intensity can be set using the following command, followed by the
			value to be forced
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
		-  22(hex) as command, set the intensity
		- >30(hex) as command, time the output to ON
		-  0(hex) as command, no action

		Output status,
		-  0(hex) for output OFF,
		-  1(hex) for output ON.
									
		The next three slots contains the R, G and B values applied to the
		light.		
	
*/	
/**************************************************************************/
void Souliss_Logic_T19(U8 *memory_map, U8 slot, U8 *trigger)
{
	// Look for input value, update output. If the output is not set, trig a data
	// change, otherwise just reset the input
	
	if (memory_map[MaCaco_IN_s + slot] == Souliss_T1n_ToogleCmd)		// Toogle Command
	{
		// Toogle the actual status of the light
		if(memory_map[MaCaco_OUT_s + slot] == Souliss_T1n_OffCoil)		
			memory_map[MaCaco_IN_s + slot] = Souliss_T1n_OnCmd;			
		else if(memory_map[MaCaco_OUT_s + slot] == Souliss_T1n_OnCoil)
			memory_map[MaCaco_IN_s + slot] = Souliss_T1n_OffCmd;
		else
			memory_map[MaCaco_IN_s + slot] = Souliss_T1n_RstCmd;
	}
	else if (memory_map[MaCaco_IN_s + slot] == Souliss_T1n_OffCmd)		// Off Command
	{
		// Trigger the change and save the actual color
		if(memory_map[MaCaco_OUT_s + slot] != Souliss_T1n_OffCoil)  
		{				
			memory_map[MaCaco_OUT_s + slot] = Souliss_T1n_OffCoil;		// Switch off the light state
			*trigger = Souliss_TRIGGED;									// Trig the change
		}
		
		// Fade out and turn off the light step wise
			if(memory_map[MaCaco_OUT_s + slot + 1]) 
				memory_map[MaCaco_OUT_s + slot + 1]--;
		
		// Once is off, reset
		if((memory_map[MaCaco_OUT_s + slot + 1] == 0))
			memory_map[MaCaco_IN_s + slot] = Souliss_T1n_RstCmd;		// Reset
	}
	else if (memory_map[MaCaco_IN_s + slot] == Souliss_T1n_OnCmd)
	{
		if(memory_map[MaCaco_OUT_s + slot] != Souliss_T1n_OnCoil)  
			*trigger = Souliss_TRIGGED;	
	
		memory_map[MaCaco_OUT_s + slot] = Souliss_T1n_OnCoil;			// Switch on the output
		
		// If there were no color set, use a light white
		if((memory_map[MaCaco_AUXIN_s + slot + 1] == 0))
		{
			memory_map[MaCaco_OUT_s + slot + 1] = 0xAA;				// Set a light white
			memory_map[MaCaco_AUXIN_s + slot + 1] = memory_map[MaCaco_OUT_s + slot + 1];

			memory_map[MaCaco_IN_s + slot] = Souliss_T1n_RstCmd;			// Reset	
		}			
		else
		{
			// Fade in to the last color
			if(memory_map[MaCaco_OUT_s + slot + 1] < (memory_map[MaCaco_AUXIN_s + slot + 1])) 
				memory_map[MaCaco_OUT_s + slot + 1]++;			
		}
		
		// Once is on, reset
		if((memory_map[MaCaco_OUT_s + slot + 1] == memory_map[MaCaco_AUXIN_s + slot + 1]))
			memory_map[MaCaco_IN_s + slot] = Souliss_T1n_RstCmd;			// Reset
	}
	else if (memory_map[MaCaco_IN_s + slot] == Souliss_T1n_BrightUp)		// Increase the light value 
	{
		// Increase the light value
		if(memory_map[MaCaco_OUT_s + slot + 1] < 255 - Souliss_T1n_BrightValue) 
			memory_map[MaCaco_OUT_s + slot + 1] += Souliss_T1n_BrightValue;
		
		memory_map[MaCaco_IN_s + slot] = Souliss_T1n_RstCmd;			// Reset
	}
	else if (memory_map[MaCaco_IN_s + slot] == Souliss_T1n_BrightDown)				// Decrease the light value
	{
		// Decrease the light value
		if(memory_map[MaCaco_OUT_s + slot + 1] > Souliss_T1n_BrightValue) 
			memory_map[MaCaco_OUT_s + slot + 1] -= Souliss_T1n_BrightValue;
			
		memory_map[MaCaco_IN_s + slot] = Souliss_T1n_RstCmd;			// Reset
	}	
	else if (memory_map[MaCaco_IN_s + slot] == Souliss_T1n_Flash)					// Turn ON and OFF at each cycle
	{
		// If the light was on
		if(memory_map[MaCaco_OUT_s + slot] == Souliss_T1n_OnCoil)
		{
			// Switch ON and OFF until a new command is received
			if((memory_map[MaCaco_OUT_s + slot + 1] == memory_map[MaCaco_AUXIN_s + slot + 1]))
				memory_map[MaCaco_OUT_s + slot + 1] = 0;										// Set output to zero
			else if((memory_map[MaCaco_OUT_s + slot + 1] == 0))
				memory_map[MaCaco_OUT_s + slot + 1] = memory_map[MaCaco_AUXIN_s + slot + 1];	// Set output to previous value
		}
	}
	else if (memory_map[MaCaco_IN_s + slot] == Souliss_T1n_Set)
	{	
		// Set the new color
		memory_map[MaCaco_OUT_s + slot + 1] = memory_map[MaCaco_IN_s + slot + 1];
		memory_map[MaCaco_AUXIN_s + slot + 1] = memory_map[MaCaco_OUT_s + slot + 1];
		memory_map[MaCaco_IN_s + slot + 1] = Souliss_T1n_RstCmd;
		
		memory_map[MaCaco_AUXIN_s + slot] = Souliss_T1n_Timed;			// Set a timer for the state notification		
		memory_map[MaCaco_OUT_s + slot] = Souliss_T1n_OnCoil;			// Switch on the output
		memory_map[MaCaco_IN_s + slot] = Souliss_T1n_RstCmd;			// Reset		
	}
	else
	{	// There is no command
		
		if(memory_map[MaCaco_AUXIN_s + slot] > Souliss_T1n_Set)			// Decrese the timer value
			memory_map[MaCaco_AUXIN_s + slot]--;
		else if(memory_map[MaCaco_AUXIN_s + slot] > 0)					// If we not getting new commands, the burst		
		{																// is done, send the actual state 
			memory_map[MaCaco_AUXIN_s + slot] = 0;
			*trigger = Souliss_TRIGGED;									
		}	
	}	
}

/**************************************************************************
/*!
	Timer associated to T19
*/	
/**************************************************************************/
void Souliss_T19_Timer(U8 *memory_map, U8 slot)
{
	if(memory_map[MaCaco_IN_s + slot] > Souliss_T1n_Timed)		// Memory value is used as timer
	{
		memory_map[MaCaco_OUT_s + slot] = Souliss_T1n_GoodNight;
	
		// Decrease timer and check the expiration
		if((--memory_map[MaCaco_IN_s + slot]) == Souliss_T1n_Timed)		
			memory_map[MaCaco_IN_s + slot] = Souliss_T1n_OffCmd;
		
		// Bright down the light
		if(memory_map[MaCaco_OUT_s + slot + 1])
			memory_map[MaCaco_OUT_s + slot + 1]--;
		else
			memory_map[MaCaco_OUT_s + slot + 1] = 0;
		
		// If there is no more light, declare it off
		if((memory_map[MaCaco_OUT_s + slot + 1] == Souliss_T1n_OffCoil))
			memory_map[MaCaco_IN_s + slot] = Souliss_T1n_OffCmd;		// Reset	
	}	
}