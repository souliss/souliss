/**************************************************************************
	Souliss
    Copyright (C) 2013  Veseo

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
#ifndef SPEAKEASY_H
#define SPEAKEASY_H

#include "Typicals.h"

U8 memory_map[MaCaco_MEMMAP];				// define the shared memory map
U8 data_changed = 1;						// flag

/************** Scheduler ******************/
#define time_base_fast		10				// Time cycle in milliseconds
#define time_base_slow		10000			// Time cycle in milliseconds
#define num_phases			1000			// Number of phases

U16 phase_speedy=0, phase_fast=0, phase_slow=0;
unsigned long tmr_fast=0, tmr_slow=0;


#define EXECUTEFAST()	if((millis()-tmr_fast) > time_base_fast )
#define UPDATEFAST()	tmr_fast = millis();	\
						phase_fast = (phase_fast + 1) % num_phases


#define	FAST_x10ms(n)	if (!(phase_fast % n))
#define	FAST_10ms()		if (!(phase_fast % 1))
#define	FAST_30ms()		if (!(phase_fast % 3))
#define	FAST_50ms()		if (!(phase_fast % 5))
#define	FAST_70ms()		if (!(phase_fast % 7))
#define	FAST_90ms()		if (!(phase_fast % 9))
#define	FAST_110ms()	if (!(phase_fast % 11))
#define FAST_210ms()	if (!(phase_fast % 21))
#define	FAST_510ms()	if (!(phase_fast % 51))
#define	FAST_710ms()	if (!(phase_fast % 71))
#define	FAST_910ms()	if (!(phase_fast % 91))
#define	FAST_1110ms()	if (!(phase_fast % 111))
#define	FAST_2110ms()	if (!(phase_fast % 211))
#define	FAST_7110ms()	if (!(phase_fast % 711))
#define	FAST_9110ms()	if (!(phase_fast % 911))
#define	FAST_11110ms()	if (!(phase_fast % 1111))
#define	FAST_21110ms()	if (!(phase_fast % 2111))
#define	FAST_71110ms()	if (!(phase_fast % 7111))
#define	FAST_91110ms()	if (!(phase_fast % 9111))

#define	SHIFT_10ms(n)		if ((phase_fast % 1)==n)
#define	SHIFT_30ms(n)		if ((phase_fast % 3)==n)
#define	SHIFT_50ms(n)		if ((phase_fast % 5)==n)
#define	SHIFT_70ms(n)		if ((phase_fast % 7)==n)
#define	SHIFT_90ms(n)		if ((phase_fast % 9)==n)
#define	SHIFT_110ms(n)		if ((phase_fast % 11)==n)
#define SHIFT_210ms(n)		if ((phase_fast % 21)==n)
#define SHIFT_310ms(n)		if ((phase_fast % 31)==n)
#define	SHIFT_510ms(n)		if ((phase_fast % 51)==n)
#define	SHIFT_710ms(n)		if ((phase_fast % 71)==n)
#define	SHIFT_910ms(n)		if ((phase_fast % 91)==n)
#define	SHIFT_1110ms(n)		if ((phase_fast % 111)==n)
#define	SHIFT_2110ms(n)		if ((phase_fast % 211)==n)
#define	SHIFT_7110ms(n)		if ((phase_fast % 711)==n)
#define	SHIFT_9110ms(n)		if ((phase_fast % 911)==n)
#define	SHIFT_11110ms(n)	if ((phase_fast % 1111)==n)
#define	SHIFT_21110ms(n)	if ((phase_fast % 2111)==n)
#define	SHIFT_71110ms(n)	if ((phase_fast % 7111)==n)
#define	SHIFT_91110ms(n)	if ((phase_fast % 9111)==n)

#define EXECUTESLOW()	else if((millis()-tmr_slow) > time_base_slow )
#define UPDATESLOW()	tmr_slow = millis();	\
						phase_slow = (phase_slow + 1) % num_phases

#define	SLOW_x10s(n)	if (!(phase_slow % n))
#define	SLOW_10s()		if (!(phase_slow % 1))
#define	SLOW_50s()		if (!(phase_slow % 5))
#define	SLOW_70s()		if (!(phase_slow % 7))
#define	SLOW_90s()		if (!(phase_slow % 9))
#define	SLOW_110s()		if (!(phase_slow % 11))
#define	SLOW_210s()		if (!(phase_slow % 21))
#define	SLOW_310s()		if (!(phase_slow % 31))
#define	SLOW_510s()		if (!(phase_slow % 51))
#define	SLOW_710s()		if (!(phase_slow % 71))
#define	SLOW_15m()		if (!(phase_slow % 91))
#define	SLOW_30m()		if (!(phase_slow % 181))
#define	SLOW_1h()		if (!(phase_slow % 361))
#define	SLOW_2h()		if (!(phase_slow % 721))
#define	SLOW_4h()		if (!(phase_slow % 1441))
#define	SLOW_halfday()	if (!(phase_slow % 4321))
#define	SLOW_1day()		if (!(phase_slow % 8641))

#define	SHIFT_SLOW_10s(n)		if ((phase_slow % 1)==n)
#define	SHIFT_SLOW_50s(n)		if ((phase_slow % 5)==n)
#define	SHIFT_SLOW_70s(n)		if ((phase_slow % 7)==n)
#define	SHIFT_SLOW_90s(n)		if ((phase_slow % 9)==n)
#define	SHIFT_SLOW_110s(n)		if ((phase_slow % 11)==n)

#define EXECUTESPEEDY() else
#define UPDATESPEEDY()	phase_speedy = (phase_speedy + 1) % num_phases
#define	SPEEDY_x(n)		if (!(phase_speedy % n))


/***************************************/

/************** Typicals ******************/
#define	Set_SimpleLight(slot)		Souliss_SetT11(memory_map, slot)
#define	Logic_SimpleLight(slot)		Souliss_Logic_T11(memory_map, slot, &data_changed)
#define Timer_SimpleLight(slot)		Souliss_T11_Timer(memory_map, slot)
#define	Set_T11(slot)				Souliss_SetT11(memory_map, slot)
#define	Logic_T11(slot)				Souliss_Logic_T11(memory_map, slot, &data_changed)
#define Timer_T11(slot)				Souliss_T11_Timer(memory_map, slot)
#define Set_LightsGroup(firstSlot, lastSlot) 	Souliss_SetT11Group(memory_map, firstSlot, lastSlot)
#define Timer_LightsGroup(firstSlot, lastSlot) 	Souliss_T11Group_Timer(memory_map, firstSlot, lastSlot)
#define Logic_LightsGroup(firstSlot, lastSlot) 	Souliss_Logic_T11Group(memory_map, firstSlot, lastSlot, &data_changed)

#define	Set_AutoLight(slot)			Souliss_SetT12(memory_map, slot)
#define	Logic_AutoLight(slot) 		Souliss_Logic_T12(memory_map, slot, &data_changed)
#define Timer_AutoLight(slot)		Souliss_T12_Timer(memory_map, slot)
#define	Set_T12(slot)				Souliss_SetT12(memory_map, slot)
#define	Logic_T12(slot)			 	Souliss_Logic_T12(memory_map, slot, &data_changed)
#define Timer_T12(slot)				Souliss_T12_Timer(memory_map, slot)

#define	Set_DigitalInput(slot)		Souliss_SetT13(memory_map, slot)
#define	Logic_DigitalInput(slot)	Souliss_Logic_T13(memory_map, slot, &data_changed)
#define	Set_T13(slot)				Souliss_SetT13(memory_map, slot)
#define	Logic_T13(slot)				Souliss_Logic_T13(memory_map, slot, &data_changed)

#define Set_PulseOutput(slot)		Souliss_SetT14(memory_map, slot)
#define Logic_PulseOutput(slot)		Souliss_Logic_T14(memory_map, slot, &data_changed)
#define Set_T14(slot)				Souliss_SetT14(memory_map, slot)
#define Logic_T14(slot)				Souliss_Logic_T14(memory_map, slot, &data_changed)

#define	Set_IrDA_Lamp(slot)			Souliss_SetT15(memory_map, slot)
#define	Logic_IrDA_Lamp(slot)		Souliss_Logic_T15(memory_map, slot, &data_changed)
#define	Set_T15(slot)				Souliss_SetT15(memory_map, slot)
#define	Logic_T15(slot)				Souliss_Logic_T15(memory_map, slot, &data_changed)

#define Set_LED_Strip(slot)			Souliss_SetT16(memory_map, slot)
#define Logic_LED_Strip(slot)		Souliss_Logic_T16(memory_map, slot, &data_changed)
#define Timer_LED_Strip(slot)		Souliss_T16_Timer(memory_map, slot)
#define Set_T16(slot)				Souliss_SetT16(memory_map, slot)
#define Logic_T16(slot)				Souliss_Logic_T16(memory_map, slot, &data_changed)
#define Timer_T16(slot)				Souliss_T16_Timer(memory_map, slot)

#define Set_StepRelay(slot)			Souliss_SetT18(memory_map, slot)
#define Logic_StepRelay(slot)		Souliss_Logic_T18(memory_map, slot, &data_changed)
#define Timer_StepRelay(slot)		Souliss_T18_Timer(memory_map, slot)
#define Set_T18(slot)				Souliss_SetT18(memory_map, slot)
#define Logic_T18(slot)				Souliss_Logic_T18(memory_map, slot, &data_changed)
#define Timer_T18(slot)				Souliss_T18_Timer(memory_map, slot)

#define Set_DimmableLight(slot)		Souliss_SetT19(memory_map, slot)
#define Logic_DimmableLight(slot)	Souliss_Logic_T19(memory_map, slot, &data_changed)
#define Timer_DimmableLight(slot)	Souliss_T19_Timer(memory_map, slot)
#define Set_T19(slot)				Souliss_SetT19(memory_map, slot)
#define Logic_T19(slot)				Souliss_Logic_T19(memory_map, slot, &data_changed)
#define Timer_T19(slot)				Souliss_T19_Timer(memory_map, slot)

#define Set_MultipleInputs(slot)	Souliss_SetT1A(memory_map, slot)
#define Logic_MultipleInputst(slot)	Souliss_Logic_T1A(memory_map, slot, &data_changed)
#define Timer_MultipleInputs(slot)	Souliss_T1A_Timer(memory_map, slot)
#define Set_T1A(slot)				Souliss_SetT1A(memory_map, slot)
#define Logic_T1A(slot)				Souliss_Logic_T1A(memory_map, slot, &data_changed)
#define Timer_T1A(slot)				Souliss_T1A_Timer(memory_map, slot)

#define Set_GarageDoor(slot)		Souliss_SetT21(memory_map, slot)
#define Logic_GarageDoor(slot)		Souliss_Logic_T21(memory_map, slot, &data_changed)
#define Timer_GarageDoor(slot)		Souliss_T21_Timer(memory_map, slot)
#define Set_T21(slot)				Souliss_SetT21(memory_map, slot)
#define Logic_T21(slot)				Souliss_Logic_T21(memory_map, slot, &data_changed)
#define Timer_T21(slot)				Souliss_T21_Timer(memory_map, slot)

#define	Set_Windows(slot)			Souliss_SetT22(memory_map, slot)
#define	Logic_Windows(slot)			Souliss_Logic_T22(memory_map, slot, &data_changed)
#define	Timer_Windows(slot)			Souliss_T22_Timer(memory_map, slot)
#define	Set_T22(slot)				Souliss_SetT22(memory_map, slot)
#define	Logic_T22(slot)				Souliss_Logic_T22(memory_map, slot, &data_changed)
#define	Timer_T22(slot)				Souliss_T22_Timer(memory_map, slot)

#define Set_Thermostat(slot)		Souliss_SetT31(memory_map, slot)
#define Logic_Thermostat(slot)		Souliss_Logic_T31(memory_map, slot, &data_changed)
#define Set_T31(slot)				Souliss_SetT31(memory_map, slot)
#define Logic_T31(slot)				Souliss_Logic_T31(memory_map, slot, &data_changed)

#define	Set_IrDA_Aircon(slot)		Souliss_SetT32(memory_map, slot)
#define	Logic_IrDA_Aircon(slot)		Souliss_Logic_T32(memory_map, slot, &data_changed)
#define	Set_T32(slot)				Souliss_SetT32(memory_map, slot)
#define	Logic_T32(slot)				Souliss_Logic_T32(memory_map, slot, &data_changed)

#define	Set_Antitheft_Main(slot)	Souliss_SetT41(memory_map, slot)
#define	Logic_Antitheft_Main(slot)	Souliss_Logic_T41(memory_map, slot, &data_changed)
#define	Timer_Antitheft_Main(slot)	Souliss_T41_Timer(memory_map, slot)
#define	Set_T41(slot)				Souliss_SetT41(memory_map, slot)
#define	Logic_T41(slot)				Souliss_Logic_T41(memory_map, slot, &data_changed)
#define	Timer_T41(slot)				Souliss_T41_Timer(memory_map, slot)

#define	Set_Antitheft_Peer(slot)							Souliss_SetT42(memory_map, slot)
#define	Logic_Antitheft_Peer(slot, main_antitheft_address)	Souliss_Logic_T42(memory_map, slot, &data_changed, main_antitheft_address)
#define	Set_T42(slot)										Souliss_SetT42(memory_map, slot)
#define	Logic_T42(slot, main_antitheft_address)				Souliss_Logic_T42(memory_map, slot, &data_changed, main_antitheft_address)

#define	Set_AnalogIn(slot)			Souliss_SetT51(memory_map, slot)
#define	Read_AnalogIn(slot)			Souliss_Logic_T51(memory_map, slot, mOutputAsFloat(slot)*0.015, &data_changed)
#define	Set_T51(slot)				Souliss_SetT51(memory_map, slot)
#define	Read_T51(slot)				Souliss_Logic_T51(memory_map, slot, mOutputAsFloat(slot)*0.015, &data_changed)

#define	Set_Temperature(slot)		Souliss_SetT52(memory_map, slot)
#define	Logic_Temperature(slot)		Souliss_Logic_T52(memory_map, slot, mOutputAsFloat(slot)*0.015, &data_changed)
#define Logic_Temperature_FixedDB(slot)	Souliss_Logic_T52(memory_map, slot, 0.1, &data_changed) 
#define	Set_T52(slot)				Souliss_SetT52(memory_map, slot)
#define	Logic_T52(slot)				Souliss_Logic_T52(memory_map, slot, mOutputAsFloat(slot)*0.015, &data_changed)

#define	Set_Humidity(slot)			Souliss_SetT53(memory_map, slot)
#define	Logic_Humidity(slot)		Souliss_Logic_T53(memory_map, slot, mOutputAsFloat(slot)*0.015, &data_changed)
#define	Set_T53(slot)				Souliss_SetT53(memory_map, slot)
#define	Logic_T53(slot)				Souliss_Logic_T53(memory_map, slot, mOutputAsFloat(slot)*0.015, &data_changed)

#define	Set_Light(slot)				Souliss_SetT54(memory_map, slot)
#define	Logic_Light(slot)			Souliss_Logic_T54(memory_map, slot, mOutputAsFloat(slot)*0.015, &data_changed)
#define	Set_T54(slot)				Souliss_SetT54(memory_map, slot)
#define	Logic_T54(slot)				Souliss_Logic_T54(memory_map, slot, mOutputAsFloat(slot)*0.015, &data_changed)

#define	Set_Voltage(slot)			Souliss_SetT55(memory_map, slot)
#define	Logic_Voltage(slot)			Souliss_Logic_T55(memory_map, slot, mOutputAsFloat(slot)*0.015, &data_changed)
#define	Set_T55(slot)				Souliss_SetT55(memory_map, slot)
#define	Logic_T55(slot)				Souliss_Logic_T55(memory_map, slot, mOutputAsFloat(slot)*0.015, &data_changed)

#define	Set_Current(slot)			Souliss_SetT56(memory_map, slot)
#define	Logic_Current(slot)			Souliss_Logic_T56(memory_map, slot, mOutputAsFloat(slot)*0.015, &data_changed)
#define	Set_T56(slot)				Souliss_SetT56(memory_map, slot)
#define	Logic_T56(slot)				Souliss_Logic_T56(memory_map, slot, mOutputAsFloat(slot)*0.015, &data_changed)

#define	Set_Power(slot)				Souliss_SetT57(memory_map, slot)
#define	Logic_Power(slot)			Souliss_Logic_T57(memory_map, slot, mOutputAsFloat(slot)*0.015, &data_changed)
#define	Set_T57(slot)				Souliss_SetT57(memory_map, slot)
#define	Logic_T57(slot)				Souliss_Logic_T57(memory_map, slot, mOutputAsFloat(slot)*0.015, &data_changed)

#define	Set_Pressure(slot)			Souliss_SetT58(memory_map, slot)
#define	Logic_Pressure(slot)		Souliss_Logic_T58(memory_map, slot, mOutputAsFloat(slot)*0.015, &data_changed)
#define	Set_T58(slot)				Souliss_SetT58(memory_map, slot)
#define	Logic_T58(slot)				Souliss_Logic_T58(memory_map, slot, mOutputAsFloat(slot)*0.015, &data_changed)

#define Set_Analog_Setpoint(slot)	Souliss_SetT61(memory_map, slot)
#define	Logic_AnalogIn(slot)		Souliss_Logic_T61(memory_map, slot, &data_changed)
#define Set_T61(slot)				Souliss_SetT61(memory_map, slot)
#define	Logic_T61(slot)				Souliss_Logic_T61(memory_map, slot, &data_changed)

#define Set_Temperature_Setpoint(slot)		Souliss_SetT62(memory_map, slot)
#define	Logic_Temperature_Setpoint(slot)	Souliss_Logic_T62(memory_map, slot, &data_changed)
#define Set_T62(slot)						Souliss_SetT62(memory_map, slot)
#define	Logic_T62(slot)						Souliss_Logic_T62(memory_map, slot, &data_changed)

#define Set_Humidity_Setpoint(slot)				Souliss_SetT63(memory_map, slot)
#define	Logic_Humidity_Setpoint(slot)			Souliss_Logic_T63(memory_map, slot, &data_changed)
#define Set_T63(slot)							Souliss_SetT63(memory_map, slot)
#define	Logic_T63(slot)							Souliss_Logic_T63(memory_map, slot, &data_changed)

#define Set_Light_Setpoint(slot)				Souliss_SetT64(memory_map, slot)
#define	Logic_Light_Setpoint(slot)				Souliss_Logic_T64(memory_map, slot, &data_changed)
#define Set_T64(slot)							Souliss_SetT64(memory_map, slot)
#define	Logic_T64(slot)							Souliss_Logic_T64(memory_map, slot, &data_changed)

#define Set_Voltage_Setpoint(slot)				Souliss_SetT65(memory_map, slot)
#define	Logic_Voltage_Setpoint(slot)			Souliss_Logic_T65(memory_map, slot, &data_changed)
#define Set_T65(slot)							Souliss_SetT65(memory_map, slot)
#define	Logic_T65(slot)							Souliss_Logic_T65(memory_map, slot, &data_changed)

#define Set_Current_Setpoint(slot)				Souliss_SetT66(memory_map, slot)
#define	Logic_Current_Setpoint(slot)			Souliss_Logic_T66(memory_map, slot, &data_changed)
#define Set_T66(slot)							Souliss_SetT66(memory_map, slot)
#define	Logic_T66(slot)							Souliss_Logic_T66(memory_map, slot, &data_changed)

#define Set_Power_Setpoint(slot)				Souliss_SetT67(memory_map, slot)
#define	Logic_Power_Setpoint(slot)				Souliss_Logic_T67(memory_map, slot, &data_changed)
#define Set_T67(slot)							Souliss_SetT67(memory_map, slot)
#define	Logic_T67(slot)							Souliss_Logic_T67(memory_map, slot, &data_changed)

#define Set_Pressure_Setpoint(slot)				Souliss_SetT68(memory_map, slot)
#define	Logic_Pressure_Setpoint(slot)			Souliss_Logic_T68(memory_map, slot, &data_changed)
#define Set_T68(slot)							Souliss_SetT68(memory_map, slot)
#define	Logic_T68(slot)							Souliss_Logic_T68(memory_map, slot, &data_changed)

#define	Watchdog(chain_address, chain_slot, alarm_command)	Souliss_Watchdog(memory_map, chain_address, chain_slot, alarm_command)
/******************************************/

/************** Souliss ******************/
#define myMap				memory_map
#define myData				&data_changed
#define myNode				memory_map, &data_changed

#define SetAddress												Souliss_SetAddress
#define	SetAddressingServer()									Souliss_SetAddressingServer(memory_map)
#define	SetDynamicAddressing()									Souliss_SetDynamicAddressing()
#define	SetIPAddress											Souliss_SetIPAddress
#define GetIPAddress											Souliss_GetIPAddress
#define SetAccessPoint											Souliss_SetAccessPoint
#define ReadIPConfiguration										Souliss_ReadIPConfiguration
#define	IsRuntimeGateway										MaCaco_IsRuntimeGateway
#define	JoinNetwork()											Souliss_JoinNetwork()
#define	JoinAndReset()											Souliss_JoinAndReset()
#define SetUserInterface										UserMode_ManualRecord
#define SetAsGateway(address)									Souliss_SetLocalAddress(memory_map, address)
#define RemoveAsGateway()										Souliss_ResetLocalAddress(memory_map)
#define GetAsGateway()											Souliss_GetLocalAddress(memory_map)
#define	SetAsPeerNode(address, index)							Souliss_SetRemoteAddress(memory_map, address, index)
#define	SetAsBatteryNode(address, index)						Souliss_SetRemoteAddress(memory_map, address, index);	\
																Souliss_BatteryChannels(memory_map, address)

#define	HardcodedChannel										Souliss_HardcodedCommunicationChannel
#define	GetTypicals()											Souliss_GetTypicals(memory_map)
#define	CommunicationChannels()									Souliss_CommunicationChannels(memory_map)
#define ProcessCommunication()									Souliss_CommunicationData(memory_map, &data_changed)
#define	DigIn(pin,value,slot)									Souliss_DigIn(pin, value, memory_map, slot)
#define	LowDigIn(pin,value,slot)								Souliss_LowDigIn(pin, value, memory_map, slot)
#define	DigIn2State(pin,value_state_on,value_state_off,slot)	Souliss_DigIn2State(pin, value_state_on, value_state_off, memory_map, slot)
#define	LowDigIn2State(pin,value_state_on,value_state_off,slot)	Souliss_LowDigIn2State(pin, value_state_on, value_state_off, memory_map, slot)
#define	DigInHold(pin, value_state1,value_state2,slot)			Souliss_DigInHold(pin, value_state1, value_state2, memory_map, slot)
#define	LowDigInHold(pin, value_state1,value_state2,slot)		Souliss_LowDigInHold(pin, value_state1, value_state2, memory_map, slot)
#define	DigInHold2(pin, value_state1,value_state2,slot1,slot2)	Souliss_DigInHold2(pin, value_state1, value_state2, memory_map, slot1, slot2)
#define	LowDigInHold2(pin, value_state1,value_state2,slot1,slot2) Souliss_LowDigInHold2(pin, value_state1, value_state2, memory_map, slot1, slot2)

#define	DigKeepHold(pin, value_state1,value_state2,slot)		Souliss_DigKeepHold(pin, value_state1, value_state2, memory_map, slot)
#define	LowDigKeepHold(pin, value_state1,value_state2,slot)		Souliss_LowDigKeepHold(pin, value_state1, value_state2, memory_map, slot)
#define DigOut(pin,value,slot)									Souliss_DigOut(pin, value, memory_map, slot)
#define PulseDigOut(pin,value,slot)								Souliss_PulseDigOut(pin, value, memory_map, slot)
#define nDigOut(pin,value,slot)									Souliss_nDigOut(pin, value, memory_map, slot)
#define LowDigOut(pin,value,slot)								Souliss_LowDigOut(pin, value, memory_map, slot)
#define PulseLowDigOut(pin,value,slot)							Souliss_PulseLowDigOut(pin, value, memory_map, slot)
#define nLowDigOut(pin,value,slot)								Souliss_nLowDigOut(pin, value, memory_map, slot)
#define DigOutToggle(pin,value,slot)							Souliss_DigOutToggle(pin, value, memory_map, slot)
#define	ImportAnalog(slot,analogvalue)							Souliss_ImportAnalog(memory_map, slot, analogvalue)
#define	AnalogIn(pin, slot, conv, bias)							Souliss_AnalogIn(pin, memory_map, slot, conv, bias)
#define isTrigged(slot)											Souliss_isTrigged(memory_map, slot)
#define	RemoteInput												Souliss_RemoteInput
#define	RemoteInputs											Souliss_RemoteInputs
#define	Send													Souliss_RemoteInput
#define	SendData												Souliss_RemoteInputs
#define	ResetInput(slot)										Souliss_ResetInput(memory_map, slot)
#define PullData(addr, slot, remote_slot, remote_numbof)		Souliss_PullData(addr, slot, remote_slot, remote_numbof)
#define	RoutingTable											vNet_SetRoutingTable
#define	DonotRouteTable											vNet_SetDoNotRoutingTable
#define	Init_XMLServer()										XMLSERVERInit(memory_map)
#define	Run_XMLServer()											XMLSERVERInterface(memory_map)
#define	Init_Modbus()											ModbusInit(memory_map)
#define	Run_Modbus()											Modbus(memory_map)
#define	Init_HTTPServer()										{}
#define	Run_HTTPServer()										HTTPServer(memory_map)

#define RemoteDigIn												Souliss_RemoteDigIn
#define RemoteLowDigIn											Souliss_RemoteLowDigIn
#define RemoteDigIn2State										Souliss_RemoteDigIn2State
#define RemoteAnalogIn2Buttons									Souliss_RemoteAnalogIn2Buttons
#define RemoteLowDigIn2State									Souliss_RemoteLowDigIn2State
#define RemoteDigInHold											Souliss_RemoteDigInHold
#define RemoteLowDigInHold										Souliss_RemoteLowDigInHold

#define Persistence_IsData(id)									Souliss_Persistence_IsData(memory_map, id)
#define Persistence_ClearData(id)								Souliss_Persistence_ClearData(memory_map, id)

#define LastIn_IsData(id)										Souliss_LastIn_IsData(memory_map, id)
#define LastIn_ClearData(id)									Souliss_LastIn_ClearData(memory_map, id)
#define LastIn_GetData(id, slot) 								Souliss_LastIn_GetData(memory_map, id, slot)
#define LastIn_GetAnalog(id, slot)								Souliss_LastIn_GetAnalog(memory_map, id, slot)

/*****************************************/

/************* Let be lazy ***************/
#define	FAST_GatewayComms()						FAST_30ms()    						\
													ProcessCommunication();			\
												FAST_1110ms() 						\
													CommunicationChannels();		\
												FAST_2110ms() 						\
													GetTypicals()

#define	FAST_BridgeComms()						FAST_30ms()    						\
													ProcessCommunication();			\


#define	FAST_PeerComms()						FAST_70ms()    						\
													ProcessCommunication();

#define	START_PeerJoin()						FAST_71110ms()						\
												{									\
													if(!MaCaco_IsSubscribed())		\
													{								\
														JoinNetwork();				\
													}								\
												}

#define	JoinInProgress()						(!MaCaco_IsSubscribed())

#define	SLOW_PeerJoin()							SLOW_510s() {						\
													JoinNetwork();					\
												}

#define	GetAddress()							Souliss_DynamicAddressing_FirstBoot (memory_map);						\
												for(uint8_t n=0;n<VNET_MEDIA_NUMBER;n++) {								\
													while(Souliss_DynamicAddressing (memory_map, __TIME__, 9)) {		\
														for(U16 i=0; i<1000; i++) {										\
															if(Souliss_CommunicationData(memory_map, &data_changed))	\
																break;													\
															delay(10);	}												\
														}													\
													while(!MaCaco_IsSubscribed()) {										\
														Souliss_JoinAndReset();											\
														for(U16 i=0; i<1000; i++) {										\
															if(Souliss_CommunicationData(memory_map, &data_changed))	\
																break;													\
															delay(10);	}												\
													}}



#define WaitSubscription()						while(!MaCaco_IsSubscribed())	{	\
													ProcessCommunication();			\
													delay(100);}

#define	aMinuteToSleep()						for(uint8_t s=0; s<255; s++)	{	\
													ProcessCommunication();			\
													delay(200);}

/*****************************************/

/*****************************************/
/*!
	Macros
*/
/*****************************************/
#define AUX							MaCaco_AUXIN_s
#define	IN							MaCaco_IN_s
#define	TYP							MaCaco_TYP_s
#define	OUT							MaCaco_OUT_s
#define mAuxiliary(slot)			memory_map[AUX+slot]
#define mInput(slot)				memory_map[IN+slot]
#define	mTypical(slot)				memory_map[TYP+slot]
#define mOutput(slot)				memory_map[OUT+slot]
#define	mOutputAsFloat(slot)		Souliss_SinglePrecisionFloating(memory_map+MaCaco_OUT_s+slot)

#define	pTYP						MaCaco_P_TYP_s
#define	pOUT						MaCaco_P_OUT_s
#define	pTypical(node,slot)			memory_map[pTYP+slot+(node*MaCaco_SLOT)]
#define	pOutput(node,slot)			memory_map[pOUT+slot+(node*MaCaco_SLOT)]
#define	pOutputAsFloat(node,slot)	Souliss_SinglePrecisionFloating(memory_map+MaCaco_P_OUT_s+slot+(node*MaCaco_SLOT))

#define SetTrigger()				(data_changed=1);
#define ResetTrigger()				(data_changed=0);
#define	Initialize()				Souliss_Initialize(memory_map)
#define	isTrigger()					data_changed

#define SetInput(slot, value) 		mInput(slot) = value

#define	ReqTyp						MaCaco_reqtyp

#endif
