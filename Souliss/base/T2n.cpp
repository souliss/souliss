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
