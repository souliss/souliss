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
#ifndef TYP_H
#define TYP_H

#include "src/types.h"

// Defines for Typicals
#define Souliss_T11						0x11			// ON/OFF Digital Output with Timer Option
#define Souliss_T12						0x12			// ON/OFF Digital Output with AUTO mode
#define Souliss_T13						0x13			// Digital Input Value
#define Souliss_T14						0x14			// Pulse Digital Output
#define Souliss_T15						0x15			// RGB Light (IR)
#define Souliss_T16						0x16			// RGB Light
#define Souliss_T18						0x18			// ON/OFF Digital Output with pulse output with Timer Option
#define Souliss_T19						0x19			// LED Light
#define Souliss_T21						0x21			// Motorized devices with limit switches
#define Souliss_T22						0x22			// Motorized devices with limit switches and middle position
#define Souliss_T31						0x31			// Temperature control 
#define Souliss_T32						0x32			// Air Conditioner
#define Souliss_T41						0x41			// Anti-theft integration (Main)
#define Souliss_T42						0x42			// Anti-theft integration (Peer)
#define Souliss_T51						0x51
#define Souliss_T52						0x52
#define Souliss_T53						0x53
#define Souliss_T54						0x54
#define Souliss_TRL						0xFF			// Related association, indentify a slot related to a previous one

// General defines for T1n
#define Souliss_T1n_ToogleCmd			0x01		// Toogle Command
#define Souliss_T1n_OnCmd				0x02		// ON Command
#define Souliss_T1n_OffCmd				0x04		// OFF Command
#define Souliss_T1n_AutoCmd				0x08		// AUTO Mode Command
#define Souliss_T1n_BrightUp			0x10		// Increase Light
#define Souliss_T1n_BrightDown			0x20		// Decrease Light
#define Souliss_T1n_Flash				0x21		// Flash Light
#define Souliss_T1n_Set					0x22		// Set a state
#define Souliss_T1n_OnFeedback			0x23		// Report the actual state as ON
#define Souliss_T1n_OffFeedback			0x24		// Report the actual state as OFF
#define Souliss_T1n_Timed				0x30		// Timed ON
#define Souliss_T1n_RstCmd				0x00		// Reset
#define Souliss_T1n_AutoState			0xF0		// AUTO Mode Feedback
#define Souliss_T1n_Coil				0x01		// Output Feedback ON
#define Souliss_T1n_OnCoil				0x01		// Output Feedback ON
#define Souliss_T1n_OffCoil				0x00		// Output Feedback OFF
#define Souliss_T1n_ResetCoil			0xA1		// Pulse Output Coil (Reset)
#define Souliss_T1n_PulseCoil			0xA5		// Pulse Output Coil (Set)
#define Souliss_T1n_TimedOnCoil			0xE1		// Output Feedback ON in Timed Mode
#define Souliss_T1n_TimedOffCoil		0xE0		// Output Feedback OFF in Timed Mode
#define Souliss_T1n_AutoOnCoil			0xF1		// Output Feedback ON in AUTO Mode
#define Souliss_T1n_AutoOffCoil			0xF0		// Output Feedback OFF in AUTO Mode
#define Souliss_T1n_GoodNight			0xF1		// Output Feedback ON in GoodNight Mode

#define Souliss_T1n_BrightValue			0x10		

#define Souliss_T1n_RGBLamp_OnCmd		0x02		// ON Command
#define Souliss_T1n_RGBLamp_OffCmd		0x04		// OFF Command
#define Souliss_T1n_RGBLamp_RstCmd		0x00		// Reset Command
#define Souliss_T1n_RGBLamp_R			0x06		// Red Color
#define Souliss_T1n_RGBLamp_G			0x07		// Green Color
#define Souliss_T1n_RGBLamp_B			0x08		// Blue Color
#define Souliss_T1n_RGBLamp_W			0x09		// White Color
#define Souliss_T1n_RGBLamp_BrightUp	0x10		
#define Souliss_T1n_RGBLamp_BrightDown	0x20
#define Souliss_T1n_RGBLamp_Flash		0xA1
#define Souliss_T1n_RGBLamp_Strobe		0xA2
#define Souliss_T1n_RGBLamp_Fade		0xA3
#define Souliss_T1n_RGBLamp_Smooth		0xA4
#define Souliss_T1n_RGBLamp_R2			0xB1
#define Souliss_T1n_RGBLamp_R3			0xB2
#define Souliss_T1n_RGBLamp_R4			0xB3
#define Souliss_T1n_RGBLamp_R5			0xB4
#define Souliss_T1n_RGBLamp_G2			0xC1
#define Souliss_T1n_RGBLamp_G3			0xC2
#define Souliss_T1n_RGBLamp_G4			0xC3
#define Souliss_T1n_RGBLamp_G5			0xC4
#define Souliss_T1n_RGBLamp_B2			0xD1
#define Souliss_T1n_RGBLamp_B3			0xD2
#define Souliss_T1n_RGBLamp_B4			0xD3
#define Souliss_T1n_RGBLamp_B5			0xD4

// General defines for T2n
#define Souliss_T2n_CloseCmd			0x01		// Close Command		
#define Souliss_T2n_OpenCmd				0x02		// Open Command
#define Souliss_T2n_StopCmd				0x04		// Stop Command
#define Souliss_T2n_ToogleCmd			0x08		// Toogle Command
#define Souliss_T2n_RstCmd				0x00		// Reset Command
#define Souliss_T2n_Timer_Val			0x1F		// Timer set value
#define Souliss_T2n_Timer_Off			0x10		// Timer expired value
#define Souliss_T2n_LimSwitch_Close		0x08		// Close Feedback from Limit Switch
#define Souliss_T2n_LimSwitch_Open		0x10		// Open Feedback from Limit Switch
#define Souliss_T2n_NoLimSwitch			0x20		// No Limit Switch
#define Souliss_T2n_Coil_Close			0x01		// Closing
#define Souliss_T2n_Coil_Open			0x02		// Opening
#define Souliss_T2n_Coil_Stop			0x03		// Stopped
#define Souliss_T2n_Coil_Off			0x00		

// General defines for T3n
#define Souliss_T3n_InSetPoint			0x01		// Increase Setpoint Command
#define Souliss_T3n_OutSetPoint			0x02		// Decrease Setpoint Command
#define Souliss_T3n_AsMeasured			0x03		// Setpoint equal to actual measure
#define Souliss_T3n_Cooling				0x04		// Set cooling mode
#define Souliss_T3n_Heating				0x05		// Set heating mode
#define Souliss_T3n_FanOff				0x06		// Heating / Cooling Fan Off
#define Souliss_T3n_FanLow				0x07		// Heating / Cooling Fan Low
#define Souliss_T3n_FanMed				0x08		// Heating / Cooling Fan Medium
#define Souliss_T3n_FanHigh				0x09		// Heating / Cooling Fan High
#define Souliss_T3n_FanAuto				0x0A		// Heating / Cooling Fan Automatic
#define Souliss_T3n_FanManual			0x0B		// Heating / Cooling Fan Manual
#define Souliss_T3n_DeadBand			0.05		// Percentage Deadband
#define Souliss_T3n_ThMed				0.10		// Threshold for medium speed
#define Souliss_T3n_ThHigh				0.15		// Threshold for high speed

#define Souliss_T3n_RstCmd				0x0000
#define Souliss_T3n_HeatingOn			0x02		// Heating Active
#define Souliss_T3n_CoolingOn			0x03		// Cooling Active
#define Souliss_T3n_FanOn1				0x08		// Fan 1 Running
#define Souliss_T3n_FanOn2				0x10		// Fan 2 Running
#define Souliss_T3n_FanOn3				0x20		// Fan 3 Running
#define Souliss_T3n_FanAutoState		0x40		// Fan set in Automatic
#define Souliss_T3n_HeatingMode			0x80		// State set as heating
#define Souliss_T3n_CoolingMode			0x80		// State set as cooling

#define Souliss_T3n_AirCon_OnCmd		0xF0 
#define Souliss_T3n_AirCon_OffCmd 		0xFC
#define Souliss_T3n_AirCon_RstCmd		0x00
#define Souliss_T3n_AirCon_T16C			0x0F
#define Souliss_T3n_AirCon_T17C			0x07
#define Souliss_T3n_AirCon_T18C			0x0B
#define Souliss_T3n_AirCon_T19C			0x03
#define Souliss_T3n_AirCon_T20C			0x0D
#define Souliss_T3n_AirCon_T21C			0x05
#define Souliss_T3n_AirCon_T22C			0x09
#define Souliss_T3n_AirCon_T23C			0x01
#define Souliss_T3n_AirCon_T24C			0x0E
#define Souliss_T3n_AirCon_T25C			0x06
#define Souliss_T3n_AirCon_T26C			0x0A
#define Souliss_T3n_AirCon_T27C			0x02
#define Souliss_T3n_AirCon_T28C			0x0C
#define Souliss_T3n_AirCon_T29C			0x04
#define Souliss_T3n_AirCon_T30C			0x08
#define Souliss_T3n_AirCon_Normal		0x71
#define Souliss_T3n_AirCon_Eco			0x01
#define Souliss_T3n_AirCon_Turbo		0x11
#define Souliss_T3n_AirCon_Auto			0x0F 
#define Souliss_T3n_AirCon_Dry			0x0B
#define Souliss_T3n_AirCon_Fan			0x03
#define Souliss_T3n_AirCon_Heat			0x0D
#define Souliss_T3n_AirCon_Cool			0x07
#define Souliss_T3n_AirCon_Fan_Auto		0x07
#define Souliss_T3n_AirCon_Fan_High		0x02
#define Souliss_T3n_AirCon_Fan_Med		0x06
#define Souliss_T3n_AirCon_Fan_Low		0x05
#define Souliss_T3n_AirCon_Opt1			0x2D
#define Souliss_T3n_AirCon_Opt2			0x77

// General defines for T4n
#define Souliss_T4n_Alarm				0x01		// Alarm Condition Detected (Input)		
#define Souliss_T4n_RstCmd				0x00		
#define Souliss_T4n_ReArm				0x03		// Silence and Arm Command
#define Souliss_T4n_NotArmed			0x04		// Anti-theft not Armed Command
#define Souliss_T4n_Armed				0x05		// Anti-theft Armed Command
#define Souliss_T4n_Antitheft			0x01		// Anti-theft Armed Feedback
#define Souliss_T4n_NoAntitheft			0x00		// Anti-theft not Armed Feedback
#define Souliss_T4n_InAlarm				0x03		// Anti-theft in Alarm

#define Souliss_RstCmd					0x00
#define Souliss_NOTTRIGGED				0x00
#define Souliss_TRIGGED					0x01


void Souliss_SetT11(U8 *memory_map, U8 slot);
void Souliss_Logic_T11(U8 *memory_map, U8 slot, U8 *trigger);
void Souliss_T11_Timer(U8 *memory_map, U8 input_slot);

void Souliss_SetT12(U8 *memory_map, U8 slot);
void Souliss_Logic_T12(U8 *memory_map, U8 slot, U8 *trigger);
void Souliss_T12_Timer(U8 *memory_map, U8 input_slot);

void Souliss_SetT13(U8 *memory_map, U8 slot);
void Souliss_Logic_T13(U8 *memory_map, U8 slot, U8 *trigger);

void Souliss_SetT14(U8 *memory_map, U8 slot);
void Souliss_Logic_T14(U8 *memory_map, U8 slot, U8 *trigger);

void Souliss_SetT15(U8 *memory_map, U8 slot);
void Souliss_Logic_T15(U8 *memory_map, U8 slot, U8 *trigger);

void Souliss_SetT16(U8 *memory_map, U8 slot);
void Souliss_Logic_T16(U8 *memory_map, U8 slot, U8 *trigger);
void Souliss_T16_Timer(U8 *memory_map, U8 input_slot);

void Souliss_SetT18(U8 *memory_map, U8 slot);
void Souliss_Logic_T18(U8 *memory_map, U8 slot, U8 *trigger);

void Souliss_SetT19(U8 *memory_map, U8 slot);
void Souliss_Logic_T19(U8 *memory_map, U8 slot, U8 *trigger);
void Souliss_T19_Timer(U8 *memory_map, U8 input_slot);

void Souliss_SetT21(U8 *memory_map, U8 slot);
void Souliss_Logic_T21(U8 *memory_map, U8 slot, U8 *trigger);
void Souliss_T21_Timer(U8 *memory_map, U8 slot);

void Souliss_SetT22(U8 *memory_map, U8 slot);
void Souliss_Logic_T22(U8 *memory_map, U8 slot, U8 *trigger);
void Souliss_T22_Timer(U8 *memory_map, U8 slot);

void Souliss_SetT31(U8 *memory_map, U8 slot);
void Souliss_Logic_T31(U8 *memory_map, U8 slot, U8 *trigger);

void Souliss_SetT32(U8 *memory_map, U8 slot);
void Souliss_Logic_T32(U8 *memory_map, U8 slot, U8 *trigger);

void Souliss_SetT41(U8 *memory_map, U8 slot);
U8 Souliss_Logic_T41(U8 *memory_map, U8 slot, U8 *trigger);
void Souliss_T41_Timer(U8 *memory_map, U8 slot);

void Souliss_SetT42(U8 *memory_map, U8 slot);
U8 Souliss_Logic_T42(U8 *memory_map, U8 slot, U8 *trigger, U16 main_antitheft_address);

void Souliss_SetT51(U8 *memory_map, U8 slot);
U8 Souliss_Logic_T51(U8 *memory_map, U8 slot, const float deadband, U8 *trigger);

void Souliss_SetT5n(U8 *memory_map, U8 slot, U8 typ);

#define Souliss_SetT52(memory_map, slot) Souliss_SetT5n(memory_map, slot, 0x52)
#define Souliss_SetT53(memory_map, slot) Souliss_SetT5n(memory_map, slot, 0x53)
#define Souliss_SetT54(memory_map, slot) Souliss_SetT5n(memory_map, slot, 0x54)
#define Souliss_SetT55(memory_map, slot) Souliss_SetT5n(memory_map, slot, 0x55)
#define Souliss_SetT56(memory_map, slot) Souliss_SetT5n(memory_map, slot, 0x56)
#define Souliss_SetT57(memory_map, slot) Souliss_SetT5n(memory_map, slot, 0x57)
#define Souliss_SetT58(memory_map, slot) Souliss_SetT5n(memory_map, slot, 0x58)
#define Souliss_SetT59(memory_map, slot) Souliss_SetT5n(memory_map, slot, 0x59)

#define Souliss_Logic_T52 Souliss_Logic_T51
#define Souliss_Logic_T53 Souliss_Logic_T51
#define Souliss_Logic_T54 Souliss_Logic_T51
#define Souliss_Logic_T55 Souliss_Logic_T51
#define Souliss_Logic_T56 Souliss_Logic_T51
#define Souliss_Logic_T57 Souliss_Logic_T51
#define Souliss_Logic_T58 Souliss_Logic_T51
#define Souliss_Logic_T59 Souliss_Logic_T51

#endif



