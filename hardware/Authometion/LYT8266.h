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
#ifndef LYT_H
#define LYT_H

#define	LYT_MaxBright					0xFF
#define	LYT_MedBright					0x50
#define	LYT_MinBright					0x00

#define	BRIGHT_STEP						15
#define	BRIGHT_DEFAULT					LYT_MedBright
					
void InitLYT();
void LYTOn();
void LYTOff();
void LYTWhite(U8 brightness, U8 fade_on);
void LYTColor(U8 red, U8 green, U8 blue, U8 fade_on);
void LYTPulse();
void Souliss_SetLYTLamps(U8 *memory_map, U8 slot);
U8 Souliss_Logic_LYTLamps(U8 *memory_map, U8 slot, U8 *trigger);
void Souliss_LYTLamps_Timer(U8 *memory_map, U8 slot);
void Souliss_SetOff(U8 *memory_map, U8 slot, U8 *trigger);
void Souliss_IncreaseBrightness(U8 *memory_map, U8 slot, U8 *trigger);
void Souliss_DecreaseBrightness(U8 *memory_map, U8 slot, U8 *trigger);
void Souliss_SetWhite(U8 *memory_map, U8 slot, U8 *trigger, U8 brightness);
void Souliss_SetColor(U8 *memory_map, U8 slot, U8 *trigger, U8 red, U8 green, U8 blue);
			
// Define short cuts for often used variables, for more info read about SpeakEasy	
#define	SetLYTLamps(slot)			Souliss_SetLYTLamps(memory_map, slot)
#define LogicLYTLamps(slot)			Souliss_Logic_LYTLamps(memory_map, slot, &data_changed)
#define	LYTSleepTimer(slot)			Souliss_LYTLamps_Timer(memory_map, slot)
#define	SetOff(slot)				Souliss_SetOff(memory_map, U8 slot, &data_changed)
#define IncreaseBrightness(slot)	Souliss_IncreaseBrightness(memory_map, slot, &data_changed)
#define DecreaseBrightness(slot)	Souliss_DecreaseBrightness(memory_map, slot, &data_changed)
#define SetWhite(slot, brightness)	Souliss_SetWhite(memory_map, slot, &data_changed, brightness)
#define SetColor(slot, r, g, b)		Souliss_SetColor(memory_map, slot,  &data_changed, r, g, b)

#endif
