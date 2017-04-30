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


/**************************************************************************/
/*!
	Typical 61 : Analog Setpoint

		Store a value as setpoint, this can be used to create custom logic

		The analog values shall be a floating point and must be converted in
		half-precision floating point using Souliss.

		This typical use two (2) memory slots
*/
/**************************************************************************/
U8 Souliss_Logic_T61(U8 *memory_map, U8 slot, U8 *trigger)
{
	// If there is a new values, compare it against the previous one
	if((memory_map[MaCaco_IN_s + slot] != 0) || (memory_map[MaCaco_IN_s + slot + 1] != 0))
	{
		// If the input is not a number (NaN) force the output at same value
		if((C8TO16(memory_map + MaCaco_IN_s + slot)) == 0xFE00)
		{
			*(memory_map + MaCaco_OUT_s + slot)     = 0x00;		// Force at NaN
			*(memory_map + MaCaco_OUT_s + slot + 1) = 0xFE;		// Force at NaN

			return Souliss_NOTTRIGGED;
		}

		// If the new input value is different from the actual output value, store it in the output
		if((C8TO16(memory_map + MaCaco_IN_s + slot)) != (C8TO16(memory_map + MaCaco_OUT_s + slot)))
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

	return Souliss_NOTTRIGGED;
}

/**************************************************************************/
/*!
	Typical 6n : Pre-defined Analog Setpoint

		Souliss support for analog values use a single representation as
		half-duplex floating point, at same time some pre-defined analog
		input typical (pressure, temperature, humidity, ...) are defined
		in order to standardize the ranges and the user interface management.

		The following pre-defined analog inputs are available,
			Souliss_Logic_T61 - Generic
			Souliss_Logic_T62 - Temperature measure (-20, +50) �C
			Souliss_Logic_T63 - Humidity measure (0, 100) %
			Souliss_Logic_T64 - Light Sensor (0, 40) kLux
			Souliss_Logic_T65 - Voltage (0, 400) V
			Souliss_Logic_T66 - Current (0, 25)  A
			Souliss_Logic_T67 - Power (0, 6500)  W
			Souliss_Logic_T68 - Pressure measure (0, 1500) hPa
			Souliss_Logic_T69 - 

		All typicals between T52 and T59 are handled with T51, simply the value
		at user interface side will be clamped into the pre-defined ranges, having
		known unit measure.

		All code for T52 up to T59 is defined using macro in Typicals.h
*/
/**************************************************************************/
void Souliss_SetT6n(U8 *memory_map, U8 slot, U8 typ)
{
	memory_map[MaCaco_TYP_s + slot] = typ;
	memory_map[MaCaco_TYP_s + slot + 1] = Souliss_TRL;

	*(memory_map + MaCaco_IN_s + slot)      = 0x00;	// Use NaN as init value
	*(memory_map + MaCaco_IN_s + slot + 1)  = 0xFE;	// Use NaN as init value

	*(memory_map + MaCaco_OUT_s + slot)     = 0x00;	// Use NaN as init value
	*(memory_map + MaCaco_OUT_s + slot + 1) = 0xFE;	// Use NaN as init value
}
