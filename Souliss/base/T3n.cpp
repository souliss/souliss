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
		
		All values shall be in half-precision floating point, automatic
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
				#define Souliss_T3n_SetTemp				0x0C
				#define Souliss_T3n_ShutDown			0x0D
				
		Setpoint and measured values can be provided via analog input acquisition
		using Souliss_AnalogIn method, the setpoint can be modified using the In(+)
		and Out(-) commands, that will decrease of one unit the actual setpoint.
		
		The outputs are available as control state in the SLOT +0 and as used
		measured value and setpoint in next four slots (as per inputs).
		
		The control state bit meaning follow as:
			BIT 0	Not used 
			BIT 1	(0 Heating OFF , 1 Heating ON)
			BIT 2	(0 Cooling OFF , 1 Cooling ON)
			BIT 3	(0 Fan 1 OFF   , 1 Fan 1 ON)
			BIT 4	(0 Fan 2 OFF   , 1 Fan 2 ON)
			BIT 5	(0 Fan 3 OFF   , 1 Fan 3 ON)
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
U8 Souliss_Logic_T31(U8 *memory_map, U8 slot, U8 *trigger)
{
	U8 i_trigger=0;														// Internal trigger
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
			if(abs((in_temp-actual_temp)) > (Souliss_T3n_DeadBand * actual_temp))
				actual_temp = in_temp;													// Set the new temperature value
		
		*(U16*)(memory_map + MaCaco_IN_s + slot + 1) = Souliss_T3n_RstCmd;		// Reset	
	}
	
	// Trig the next change of the state
	i_trigger = Souliss_TRIGGED;	
		
	// Check the actual operational mode (Cooling / Heating)
	if(!(memory_map[MaCaco_OUT_s + slot] & Souliss_T3n_HeatingMode))
	{
		// Heating Mode
		if(((actual_temp-actual_setpnt)) < (-1 * Souliss_T3n_DeadBand * actual_temp))
			memory_map[MaCaco_OUT_s + slot] |= Souliss_T3n_HeatingOn;	// Active the heating 
		else if(((actual_temp-actual_setpnt)) > (Souliss_T3n_DeadBand*actual_temp))
			memory_map[MaCaco_OUT_s + slot] &= ~Souliss_T3n_HeatingOn;	// Stop the heating 
		else
			i_trigger = Souliss_NOTTRIGGED;								// No action, no need for trig	
	}
	else if(memory_map[MaCaco_OUT_s + slot] & Souliss_T3n_CoolingMode)
	{
		// Cooling Mode
		if(((actual_temp-actual_setpnt)) > (Souliss_T3n_DeadBand*actual_temp))
			memory_map[MaCaco_OUT_s + slot] |= Souliss_T3n_CoolingOn;	// Active the cooling 
		else if(((actual_temp-actual_setpnt)) < (-1 * Souliss_T3n_DeadBand * actual_temp))
			memory_map[MaCaco_OUT_s + slot] &= ~Souliss_T3n_CoolingOn;	// Stop the cooling 
		else
			i_trigger = Souliss_NOTTRIGGED;								// No action, no need for trig			
	}

	// Check the fan mode (Manual / Auto)
	if(memory_map[MaCaco_OUT_s + slot] & Souliss_T3n_FanAutoState)
	{
		float deviation = abs(actual_temp-actual_setpnt);
		
		if(deviation > (Souliss_T3n_ThHigh*actual_temp))
			memory_map[MaCaco_OUT_s + slot] |= (Souliss_T3n_FanOn1 | Souliss_T3n_FanOn2 | Souliss_T3n_FanOn3);	// Active Fan1 + Fan2 + Fan3
		else if(deviation > Souliss_T3n_ThMed)
		{
			memory_map[MaCaco_OUT_s + slot] |= (Souliss_T3n_FanOn1 | Souliss_T3n_FanOn2);						// Active Fan1 + Fan2
			memory_map[MaCaco_OUT_s + slot] &= ~Souliss_T3n_FanOn3;
		}
		else if(deviation > (Souliss_T3n_DeadBand*actual_temp))
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
		else if(memory_map[MaCaco_IN_s + slot] == Souliss_T3n_SetTemp)
		{
			// Get the value from the input
			memory_map[MaCaco_OUT_s + slot + 3] = memory_map[MaCaco_IN_s + slot + 3];
			memory_map[MaCaco_OUT_s + slot + 4] = memory_map[MaCaco_IN_s + slot + 4];
			
			memory_map[MaCaco_IN_s + slot] = Souliss_T3n_RstCmd;					// Reset
			return Souliss_TRIGGED;				
		}
		else if(memory_map[MaCaco_IN_s + slot] == Souliss_T3n_ShutDown)
			memory_map[MaCaco_OUT_s + slot] = Souliss_T3n_RstCmd;
		
			
		memory_map[MaCaco_IN_s + slot] = Souliss_T3n_RstCmd;					// Reset
		i_trigger = Souliss_TRIGGED;		
	}		
	
	// Convert the processed values in half precision floating points
	float16((U16*)(memory_map + MaCaco_OUT_s + slot + 1), &actual_temp);
	float16((U16*)(memory_map + MaCaco_OUT_s + slot + 3), &actual_setpnt);
		
	// Update the trigger
	if(i_trigger)
		*trigger = i_trigger;
	
	return i_trigger;		
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
U8 Souliss_Logic_T32(U8 *memory_map, U8 slot, U8 *trigger)
{
	U8 i_trigger=0;														// Internal trigger

	// If there is an input command, process it
	if(memory_map[MaCaco_IN_s + slot] != Souliss_T3n_AirCon_RstCmd)	
	{	
		// If the input command is different from the last executed command, trig a change
		if(memory_map[MaCaco_OUT_s + slot] != memory_map[MaCaco_IN_s + slot])  
		{
			memory_map[MaCaco_AUXIN_s + slot] = Souliss_TRIGGED;
			i_trigger = Souliss_TRIGGED;	// Trig change
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
	
	// Update the trigger
	if(i_trigger)
		*trigger = i_trigger;
	
	return i_trigger;	
}

