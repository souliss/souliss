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
		
		A manual reset of the alarm condition is used to let the user identify 
		which of the devices was in alarm.
		
		Hardware and/or Software Command:
			
			Using a two state switch or a software command from user other nodes
				#define Souliss_T4n_Alarm			0x01
				#define	Souliss_T4n_ReArm			0x03

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
	else if((memory_map[MaCaco_IN_s + slot] == Souliss_T4n_ReArm))
		return memory_map[MaCaco_OUT_s + slot] = Souliss_T4n_RstCmd;	
}