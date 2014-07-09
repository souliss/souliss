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
	Define the use of Typical 51 : Analog input, half-precision floating point
*/	
/**************************************************************************/
void Souliss_SetT51(U8 *memory_map, U8 slot)
{
	memory_map[MaCaco_TYP_s + slot] = Souliss_T51;
	memory_map[MaCaco_TYP_s + slot + 1] = Souliss_TRL;
	
	*(U16*)(memory_map + MaCaco_IN_s + slot)  = 0xFE00;	// Use NaN as init value
	*(U16*)(memory_map + MaCaco_OUT_s + slot) = 0xFE00;	// Use NaN as init value
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
		// If the input is not a number (NaN) force the output at same value
		if((*(U16*)(memory_map + MaCaco_IN_s + slot)) == 0xFE00)	
		{
			*(U16*)(memory_map + MaCaco_OUT_s + slot) = 0xFE00;		// Force at NaN
			return 0;
		}
		
		// Values are actually stored as half-precision floating point, this doesn't
		// allow math operation, so convert them back to single-precision
		float32((U16*)(memory_map + MaCaco_IN_s + slot), &m_in);
		float32((U16*)(memory_map + MaCaco_OUT_s + slot), &m_out);
		
		// If previously set as NaN or if there is a change greater than the deadband, update the output
		if((((*(U16*)(memory_map + MaCaco_IN_s + slot)) != (*(U16*)(memory_map + MaCaco_OUT_s + slot))) && 
				*(U16*)(memory_map + MaCaco_OUT_s + slot) == 0xFE00) || 
				(abs(m_in - m_out) > abs(deadband*m_in)))
		{
			// Store the new value
			memory_map[MaCaco_OUT_s + slot] = memory_map[MaCaco_IN_s + slot];
			memory_map[MaCaco_OUT_s + slot + 1] = memory_map[MaCaco_IN_s + slot + 1];
					
			memory_map[MaCaco_IN_s + slot] = 0;			// Reset
			memory_map[MaCaco_IN_s + slot + 1] = 0;		// Reset
			
			// Trigger
			*trigger = Souliss_TRIGGED;		
			return Souliss_TRIGGED;	
		}
	}

	return 0;	
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
			Souliss_Logic_T54 - Light Sensor (0, 40) kLux
			Souliss_Logic_T55 - Voltage (0, 400) V
			Souliss_Logic_T56 - Current (0, 25)  A
			Souliss_Logic_T57 - Power (0, 6500)  W
			Souliss_Logic_T58 - Pressure measure (0, 1500) hPa 
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

	*(U16*)(memory_map + MaCaco_IN_s + slot)  = 0xFE00;	// Use NaN as init value
	*(U16*)(memory_map + MaCaco_OUT_s + slot) = 0xFE00;	// Use NaN as init value
}