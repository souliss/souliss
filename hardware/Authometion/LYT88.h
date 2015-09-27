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

#define	BRIGHT_STEP						5
#define	BRIGHT_DEFAULT					LYT_MedBright

#define	LYT_MAXNUM						0x0A			// Maximum number of LYT groups
#define	LYT_LoweredCommandRepetition	0x06
#define	LYT_StandardCommandRepetition	PROTOCOL_COMMAND_REPETITION

typedef struct
{
	U8	addr_a;
	U8	addr_b;
	U8	slot;
	U8	set;
	U8	answer_timeout;
} LYT_struct;
					
void Souliss_SetLYTLamps(U8 *memory_map, U8 slot);
U8 Souliss_Logic_LYTLamps(U8 *memory_map, U8 slot, U8 *trigger);					
void Souliss_LYTState(U8* memory_map, U8 slot, U8* trigger);
void Souliss_LYTStateRequest();
void SetLYT(U8 index, U8 addr_a, U8 addr_b, U8 slot);
U8 FindLYT(U8 slot);
void InitLYT();
			
// Define short cuts for often used variables, for more info read about SpeakEasy	
#define	SetLYTLamps(slot)			Souliss_SetLYTLamps(memory_map, slot)
#define LogicLYTLamps(slot)			Souliss_Logic_LYTLamps(memory_map, slot, &data_changed)
#define	LYTSleepTimer(slot)			Souliss_LYTLamps_Timer(memory_map, slot)			
#define LYTState(slot)				Souliss_LYTState(memory_map, slot, &data_changed)
#define LYTStateRequest				Souliss_LYTStateRequest


#endif
