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
	
	Originally developed by Dario Di Maio
	
***************************************************************************/
/*!
    \file 
    \ingroup
*/
#ifndef SOULISS_H
#define SOULISS_H

#include "src/types.h"
#include "GetConfig.h"			// need : ethUsrCfg.h, vNetCfg.h, SoulissCfg.h, MaCacoCfg.h

#include "frame/MaCaco/MaCaco.h"
#include "frame/vNet/vNet.h"

#if(HTTPSERVER && VNET_MEDIA1_ENABLE && (ETH_W5100 || ETH_W5200))
#	include "gateway/HTTP.h"
#elif(HTTPSERVER && VNET_MEDIA1_ENABLE && ETH_ENC28J60)
#	include "gateway/HTTP_uIP.h"
#endif
	
#if(MODBUS)
#	include "gateway/Modbus.h"
#endif

// Include IO definitions and drivers for supported hardware
#include "hardware/IOdef.h"
#include "src/IEEE754/float16.h"
#include "sensors/sensors.h"

#define MAXINPIN	8		// Max number of input pins
	
/**************************************************************************/
/*!
	Macros
*/
/**************************************************************************/
#define AUX		MaCaco_AUXIN_s
#define	IN		MaCaco_IN_s
#define	OUT		MaCaco_OUT_s
#define Souliss_Auxiliary(memory_map, slot)	memory_map[AUX+slot]	
#define Souliss_Input(memory_map, slot)		memory_map[IN+slot]	
#define Souliss_Output(memory_map, slot)	memory_map[OUT+slot]
#define Souliss_ResetTrigger(data_changed)	data_changed=0;	
	 
void Souliss_SetAddress(U16 addr, U16 subnetmask, U16 mysupernode);
void Souliss_SetLocalAddress(U8 *memory_map, U16 addr);
void Souliss_SetRemoteAddress(U8 *memory_map, U16 addr, U8 node);
U8 Souliss_GetTypicals(U8 *memory_map);
U8 Souliss_CommunicationChannel(U16 addr, U8 *memory_map, U8 input_slot, U8 output_slot, U8 numof_slot, U8 subscr_chnl);
U8 Souliss_CommunicationChannels(U8 *memory_map);
void Souliss_JoinNetwork();
void Souliss_SetAddressingServer(U8 *memory_map);
void Souliss_SetDynamicAddressing();
void Souliss_DynamicAddressing (U8 *memory_map, const char id[], U8 size);
U8 Souliss_RemoteInput(U16 addr, U8 slot, U8 command);
U8 Souliss_RemoteInputs(U16 addr, U8 firstslot, U8 *commands, U8 numberof);
U8 Souliss_CommunicationData(U8 *memory_map, U8 *trigger);
U8 Souliss_Watchdog(U8 *memory_map, U16 chain_address, U8 chain_slot, U8 alarm_command);
U8 Souliss_DigIn(U8 pin, U8 value, U8 *memory_map, U8 slot);
U8 Souliss_LowDigIn(U8 pin, U8 value, U8 *memory_map, U8 slot);
U8 Souliss_DigIn2State(U8 pin, U8 value_state_on, U8 value_state_off, U8 *memory_map, U8 slot);
U8 Souliss_DigInHold(U8 pin, U8 value_state1, U8 value_state2, U8 *memory_map, U8 slot);
U8 Souliss_LowDigInHold(U8 pin, U8 value_state1, U8 value_state2, U8 *memory_map, U8 slot);
void Souliss_ImportAnalog(U8* memory_map, U8 slot, float* analogvalue);
void Souliss_AnalogIn(U8 pin, U8 *memory_map, U8 slot, float scaling, float bias);
void Souliss_DigOut(U8 pin, U8 value, U8 *memory_map, U8 slot);
void Souliss_nDigOut(U8 pin, U8 value, U8 *memory_map, U8 slot);
void Souliss_LowDigOut(U8 pin, U8 value, U8 *memory_map, U8 slot);
void Souliss_nLowDigOut(U8 pin, U8 value, U8 *memory_map, U8 slot);
void Souliss_DigOutToggle(U8 pin, U8 value, U8 *memory_map, U8 slot);
void Souliss_DigOutLessThan(U8 pin, U8 value, U8 deadband, U8 *memory_map, U8 slot);
void Souliss_DigOutGreaterThan(U8 pin, U8 value, U8 deadband, U8 *memory_map, U8 slot);
void Souliss_LinkIO(U8 *memory_map, U8 input_slot, U8 output_slot, U8 *trigger);
void Souliss_LinkOI(U8 *memory_map, U8 input_slot, U8 output_slot);
void Souliss_ResetOutput(U8 *memory_map, U8 slot);
void Souliss_ResetInput(U8 *memory_map, U8 slot);
U8 Souliss_isTrigged(U8 *memory_map, U8 slot);

#endif
