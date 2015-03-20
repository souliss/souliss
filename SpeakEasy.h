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

#define EXECUTEFAST()	if(abs(millis()-tmr_fast) > time_base_fast)
#define UPDATEFAST()	tmr_fast = millis();	\
						phase_fast = (phase_fast + 1) % num_phases

#define	FAST_10ms()		if (!(phase_fast % 1))	
#define	FAST_30ms()		if (!(phase_fast % 3))						
#define	FAST_50ms()		if (!(phase_fast % 5))
#define	FAST_70ms()		if (!(phase_fast % 7))
#define	FAST_90ms()		if (!(phase_fast % 9))
#define	FAST_110ms()	if (!(phase_fast % 11))
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


#define EXECUTESLOW()	else if(abs(millis()-tmr_slow) > time_base_slow)
#define UPDATESLOW()	tmr_slow = millis();	\
						phase_slow = (phase_slow + 1) % num_phases

#define	SLOW_10s()		if (!(phase_slow % 1))
#define	SLOW_50s()		if (!(phase_slow % 5))
#define	SLOW_70s()		if (!(phase_slow % 7))
#define	SLOW_90s()		if (!(phase_slow % 9))
#define	SLOW_110s()		if (!(phase_slow % 11))
#define	SLOW_510s()		if (!(phase_slow % 51))
#define	SLOW_710s()		if (!(phase_slow % 71))
#define	SLOW_910s()		if (!(phase_slow % 91))
#define	SLOW_1110s()	if (!(phase_slow % 111))
#define	SLOW_2110s()	if (!(phase_slow % 211))
/***************************************/

/************** Typicals ******************/
#define	Set_SimpleLight(slot)		Souliss_SetT11(memory_map, slot)
#define	Logic_SimpleLight(slot)		Souliss_Logic_T11(memory_map, slot, &data_changed)
#define Timer_SimpleLight(slot)		Souliss_T11_Timer(memory_map, slot)

#define	Set_AutoLight(slot)			Souliss_SetT12(memory_map, slot)
#define	Logic_AutoLight(slot) 		Souliss_Logic_T12(memory_map, slot, &data_changed)
#define Timer_AutoLight(slot)		Souliss_T12_Timer(memory_map, slot)

#define	Set_DigitalInput(slot)		Souliss_SetT13(memory_map, slot)
#define	Logic_DigitalInput(slot)	Souliss_Logic_T13(memory_map, slot, &data_changed)

#define Set_PulseOutput(slot)		Souliss_SetT14(memory_map, slot)
#define Logic_PulseOutput(slot)		Souliss_Logic_T14(memory_map, slot, &data_changed)

#define	Set_IrDA_Lamp(slot)			Souliss_SetT15(memory_map, slot)
#define	Logic_IrDA_Lamp(slot)		Souliss_Logic_T15(memory_map, slot, &data_changed)

#define Set_LED_Strip(slot)			Souliss_SetT16(memory_map, slot)
#define Logic_LED_Strip(slot)		Souliss_Logic_T16(memory_map, slot, &data_changed)
#define Timer_LED_Strip(slot)		Souliss_T16_Timer(memory_map, slot)

#define Set_DimmableLight(slot)		Souliss_SetT19(memory_map, slot)
#define Logic_DimmableLight(slot)	Souliss_Logic_T19(memory_map, slot, &data_changed)
#define Timer_DimmableLight(slot)	Souliss_T19_Timer(memory_map, slot)

#define Set_GarageDoor(slot)		Souliss_SetT21(memory_map, slot)
#define Logic_GarageDoor(slot)		Souliss_Logic_T21(memory_map, slot, &data_changed)
#define Timer_GarageDoor(slot)		Souliss_T21_Timer(memory_map, slot)

#define	Set_Windows(slot)			Souliss_SetT22(memory_map, slot)
#define	Logic_Windows(slot)			Souliss_Logic_T22(memory_map, slot, &data_changed)
#define	Timer_Windows(slot)			Souliss_T22_Timer(memory_map, slot)

#define Set_Thermostat(slot)		Souliss_SetT31(memory_map, slot)
#define Logic_Thermostat(slot)		Souliss_Logic_T31(memory_map, slot, &data_changed)

#define	Set_IrDA_Aircon(slot)		Souliss_SetT32(memory_map, slot)
#define	Logic_IrDA_Aircon(slot)		Souliss_Logic_T32(memory_map, slot, &data_changed)

#define	Set_Antitheft_Main(slot)	Souliss_SetT41(memory_map, slot)
#define	Logic_Antitheft_Main(slot)	Souliss_Logic_T41(memory_map, slot, &data_changed)
#define	Timer_Antitheft_Main(slot)	Souliss_T41_Timer(memory_map, slot)

#define	Set_Antitheft_Peer(slot)	Souliss_SetT42(memory_map, slot)
#define	Logic_Antitheft_Peer(slot, main_antitheft_address)	Souliss_Logic_T42(memory_map, slot, &data_changed, main_antitheft_address)

#define	Set_AnalogIn(slot)			Souliss_SetT51(memory_map, slot)
#define	Read_AnalogIn(slot)			Souliss_Logic_T51(memory_map, slot, 0.05, &data_changed)

#define	Set_Temperature(slot)		Souliss_SetT52(memory_map, slot)
#define	Logic_Temperature(slot)		Souliss_Logic_T52(memory_map, slot, 0.05, &data_changed)

#define	Set_Humidity(slot)			Souliss_SetT53(memory_map, slot)
#define	Logic_Humidity(slot)		Souliss_Logic_T53(memory_map, slot, 0.05, &data_changed)

#define	Set_Light(slot)				Souliss_SetT54(memory_map, slot)
#define	Logic_Light(slot)			Souliss_Logic_T54(memory_map, slot, 0.05, &data_changed)

#define	Set_Voltage(slot)			Souliss_SetT55(memory_map, slot)
#define	Logic_Voltage(slot)			Souliss_Logic_T55(memory_map, slot, 0.05, &data_changed)

#define	Set_Current(slot)			Souliss_SetT56(memory_map, slot)
#define	Logic_Current(slot)			Souliss_Logic_T56(memory_map, slot, 0.05, &data_changed)

#define	Set_Power(slot)				Souliss_SetT57(memory_map, slot)
#define	Logic_Power(slot)			Souliss_Logic_T57(memory_map, slot, 0.05, &data_changed)

#define	Set_Pressure(slot)			Souliss_SetT58(memory_map, slot)
#define	Logic_Pressure(slot)		Souliss_Logic_T58(memory_map, slot, 0.05, &data_changed)

/******************************************/

/************** Souliss ******************/
#define myMap	memory_map
#define myData	&data_changed
#define myNode	memory_map, &data_changed


#define	SetAddressingServer()									Souliss_SetAddressingServer(memory_map)
#define	SetDynamicAddressing()									Souliss_SetDynamicAddressing()
#define	ssDynamicAddressing()									Souliss_DynamicAddressing (memory_map, __TIME__, 9)
#define	ssJoinNetwork()											Souliss_JoinNetwork()
#define SetAsGateway(address)									Souliss_SetLocalAddress(memory_map, address)
#define	SetAsPeerNode(address, index)							Souliss_SetRemoteAddress(memory_map, address, index)
#define	ssGetTypicals()											Souliss_GetTypicals(memory_map)
#define	ssCommunicationChannels()								Souliss_CommunicationChannels(memory_map)
#define ProcessCommunication()									Souliss_CommunicationData(memory_map, &data_changed)
#define	ssDigIn(pin,value,slot)									Souliss_DigIn(pin, value, memory_map, slot)
#define	ssLowDigIn(pin,value,slot)								Souliss_LowDigIn(pin, value, memory_map, slot)
#define	ssDigIn2State(pin,value_state_on,value_state_off,slot)	Souliss_DigIn2State(pin, value_state_on, value_state_off, memory_map, slot)
#define	ssDigInHold(pin, value_state1,value_state2,slot)		Souliss_DigInHold(pin, value_state1, value_state2, memory_map, slot)
#define	ssLowDigInHold(pin, value_state1,value_state2,slot)		Souliss_LowDigInHold(pin, value_state1, value_state2, memory_map, slot)
#define ssDigOut(pin,value,slot)								Souliss_DigOut(pin, value, memory_map, slot)
#define ssnDigOut(pin,value,slot)								Souliss_nDigOut(pin, value, memory_map, slot)
#define ssLowDigOut(pin,value,slot)								Souliss_LowDigOut(pin, value, memory_map, slot)
#define ssnLowDigOut(pin,valueslot)								Souliss_nLowDigOut(pin, value, memory_map, slot)
#define	ssImportAnalog(slot,analogvalue)						Souliss_ImportAnalog(memory_map, slot, analogvalue)
#define	ssAnalogIn(pin, slot)									Souliss_AnalogIn(pin, memory_map, slot, 0.05, 0)
#define isTrigged(slot)											Souliss_isTrigged(memory_map, slot)


#define	ssGetTemperatureDHT(sensor, slot)	float tmpdhtval = ssDHT_readTemperature(sensor);	\
											Souliss_ImportAnalog(memory_map, slot, &tmpdhtval)
											
#define	ssGetHumidityDHT(sensor, slot)		float hmtdhtval = ssDHT_readHumidity(sensor);		\
											Souliss_ImportAnalog(memory_map, slot, &hmtdhtval)													

#define ssInput(slot)						memory_map[IN+slot]
#define ssOutput(slot)						memory_map[OUT+slot]

#define	Initialize()						MaCaco_init(memory_map)

/*****************************************/

/************* Let be lazy ***************/	
#define	FAST_GatewayComms()						FAST_30ms()    						\
													ProcessCommunication();			\
												FAST_1110ms() 						\
													ssCommunicationChannels();		\
												FAST_2110ms() 						\
													ssGetTypicals()
										
#define	FAST_BridgeComms()						FAST_30ms()    						\
													ProcessCommunication();			\
													
			
#define	FAST_PeerComms()						FAST_70ms()    						\
													ProcessCommunication();			
													
#define	START_PeerJoin()						FAST_71110ms()						\
												{									\
													if(!MaCaco_IsSubscribed())		\
													{								\
														ssDynamicAddressing();		\
														ssJoinNetwork();			\
													}								\
												}
												
#define	JoinInProgress()						(!MaCaco_IsSubscribed())												

#define	SLOW_PeerJoin()							SLOW_510s() {						\
													ssDynamicAddressing();			\
													ssJoinNetwork();				\
												}

						
/*****************************************/


#endif