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
#ifndef SOULISSSKETCHMODE_H
#define SOULISSSKETCHMODE_H

/*************************************/
/*!
	All code is compiled into the sketch, without creating middle
	object files. This let use #define that are globally recognized
	in the whole code, including sub-libraries.
*/

#include <Arduino.h>
#include "tools/types.h"
#include "tools/util.h"
#include "Typicals.h"
#include "GetConfig.h"			// need : ethUsrCfg.h, vNetCfg.h, SoulissCfg.h, MaCacoCfg.h

#include "frame/MaCaco/MaCaco.h"
#include "frame/vNet/vNet.h"

#if(MCU_TYPE == 0x01)	// Atmel AVR Atmega
#	if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
#		define MAXINPIN		69		// Max number of input pins
#	elif defined(__AVR_ATmega1284P__)
#		define MAXINPIN		40		// Max number of input pins
#	elif defined(__AVR_ATmega32U4__)
#		define MAXINPIN		29		// Max number of input pins
#	else
#		define MAXINPIN		29		// Max number of input pins
#	endif
#elif(MCU_TYPE == 0x02)	// Expressif ESP8266
#	define MAXINPIN		29		// Max number of input pins
#elif(MCU_TYPE == 0x03)	// Expressif ESP32
#	define MAXINPIN		29		// Max number of input pins
#endif

void Souliss_SetAddress(U16 addr, U16 subnetmask, U16 mysupernode);
void Souliss_SetLocalAddress(U8 *memory_map, U16 addr);
void Souliss_ResetLocalAddress(U8 *memory_map);
U16 Souliss_GetLocalAddress(U8 *memory_map);
void Souliss_SetRemoteAddress(U8 *memory_map, U16 addr, U8 node);
U8 Souliss_GetTypicals(U8 *memory_map);
U8 Souliss_CommunicationChannel(U16 addr, U8 *memory_map, U8 input_slot, U8 output_slot, U8 numof_slot, U8 subscr_chnl);
U8 Souliss_CommunicationChannels(U8 *memory_map);
void Souliss_BatteryChannels(U8 *memory_map, U16 addr);
void Souliss_HardcodedCommunicationChannel(U16 gateway_addr);
void Souliss_JoinNetwork();
void Souliss_SetIPAddress(U8* ip_address, U8* subnet_mask, U8* ip_gateway);
uint8_t Souliss_GetIPAddress(U8 timeout);
String Souliss_SetAccessPoint(char *ap_name, char *ap_pass);
uint8_t Souliss_ReadIPConfiguration();
void Souliss_SetAddressingServer(U8 *memory_map);
void Souliss_SetDynamicAddressing();
U8 Souliss_DynamicAddressing (U8 *memory_map, const char id[], U8 size);

U8 Souliss_RemoteInput(U16 addr, U8 slot, U8 command);
U8 Souliss_RemoteInputs(U16 addr, U8 firstslot, U8 *commands, U8 numberof);
U8 Souliss_MassiveCommand(U16 addr, U8 typ, U8 command);
U8 Souliss_BroadcastMassiveCommand(U8 typ, U8 command);
U8 Souliss_Publish(U8 *memory_map, U16 message, U8 action);
U8 Souliss_MulticastPublish(U16 multicast_addr, U8 *memory_map, U16 message, U8 action);
U8 Souliss_PublishData(U8 *memory_map, U16 message, U8 action, U8* data, U8 message_len);
U8 Souliss_MulticastPublishData(U16 multicast_addr, U8 *memory_map, U16 message, U8 action, U8* data, U8 message_len);
U8 Souliss_Subscribe(U8 *memory_map, U16 message, U8 action);
U8 Souliss_SubscribeData(U8 *memory_map, U16 message, U8 action, U8* data, U8* len);
U8 Souliss_SubscribeReset(U8 *memory_map);
U8 Souliss_PullData(U16 addr, U8 slot, U8 remote_slot, U8 remote_numbof);
U8 Souliss_CommunicationData(U8 *memory_map, U8 *trigger);
U8 Souliss_Watchdog(U8 *memory_map, U16 chain_address, U8 chain_slot, U8 alarm_command);
U8 Souliss_UnsupportedCommand();

void Souliss_Initialize(U8 *memory_map);
U8 Souliss_DigIn(U8 pin, U8 value, U8 *memory_map, U8 slot, bool filteractive);
U8 Souliss_LowDigIn(U8 pin, U8 value, U8 *memory_map, U8 slot, bool filteractive);
U8 Souliss_DigIn2State(U8 pin, U8 value_state_on, U8 value_state_off, U8 *memory_map, U8 slot);
U8 Souliss_AnalogIn2Buttons(U8 pin, U8 value_button1, U8 value_button2, U8 *memory_map, U8 slot);
U8 Souliss_LowDigIn2State(U8 pin, U8 value_state_on, U8 value_state_off, U8 *memory_map, U8 slot);
U8 Souliss_DigInHold(U8 pin, U8 value, U8 value_hold, U8 *memory_map, U8 slot, U16 holdtime);
U8 Souliss_LowDigInHold(U8 pin, U8 value, U8 value_hold, U8 *memory_map, U8 slot, U16 holdtime);
U8 Souliss_DigInHold2(U8 pin, U8 value, U8 value_hold, U8 *memory_map,  U8 slot1, U8 slot2, U16 holdtime);
U8 Souliss_LowDigInHold2(U8 pin, U8 value, U8 value_hold, U8 *memory_map,  U8 slot1, U8 slot2, U16 holdtime);
U8 Souliss_DigKeepHold(U8 pin, U8 value, U8 value_hold, U8 *memory_map, U8 slot, U16 holdtime);
U8 Souliss_LowDigKeepHold(U8 pin, U8 value, U8 value_hold, U8 *memory_map, U8 slot, U16 holdtime);

U8 Souliss_RemoteDigIn(U8 pin, U8 value, U16 addr, U8 slot, bool filteractive);
U8 Souliss_RemoteLowDigIn(U8 pin, U8 value, U16 addr, U8 slot, bool filteractive);
U8 Souliss_RemoteDigIn2State(U8 pin, U8 value_state_on, U8 value_state_off, U16 addr, U8 slot);
U8 Souliss_RemoteAnalogIn2Buttons(U8 pin, U8 value_button1, U8 value_button2, U16 addr, U8 slot);
U8 Souliss_RemoteLowDigIn2State(U8 pin, U8 value_state_on, U8 value_state_off, U16 addr, U8 slot);
U8 Souliss_RemoteDigInHold(U8 pin, U8 value, U8 value_hold, U16 addr, U8 slot, U16 holdtime);
U8 Souliss_RemoteLowDigInHold(U8 pin, U8 value, U8 value_hold, U16 addr, U8 slot, U16 holdtime);

void Souliss_ImportAnalog(U8* memory_map, U8 slot, float* analogvalue);
void Souliss_AnalogIn(U8 pin, U8 *memory_map, U8 slot, float scaling, float bias);
void Souliss_DigOut(U8 pin, U8 value, U8 *memory_map, U8 slot);
void Souliss_PulseDigOut(U8 pin, U8 value, U8 *memory_map, U8 slot);
void Souliss_nDigOut(U8 pin, U8 value, U8 *memory_map, U8 slot);
void Souliss_LowDigOut(U8 pin, U8 value, U8 *memory_map, U8 slot);
void Souliss_PulseLowDigOut(U8 pin, U8 value, U8 *memory_map, U8 slot);
void Souliss_nLowDigOut(U8 pin, U8 value, U8 *memory_map, U8 slot);
void Souliss_DigOutToggle(U8 pin, U8 value, U8 *memory_map, U8 slot);
void Souliss_DigOutLessThan(U8 pin, U8 value, U8 deadband, U8 *memory_map, U8 slot);
void Souliss_DigOutGreaterThan(U8 pin, U8 value, U8 deadband, U8 *memory_map, U8 slot);
void Souliss_LinkIO(U8 *memory_map, U8 input_slot, U8 output_slot, U8 *trigger);

void Souliss_LinkOI(U8 *memory_map, U8 input_slot, U8 output_slot);
void Souliss_ResetOutput(U8 *memory_map, U8 slot);
void Souliss_ResetInput(U8 *memory_map, U8 slot);
U8 Souliss_isTrigged(U8 *memory_map, U8 slot);
float Souliss_SinglePrecisionFloating(U8 *input);
void Souliss_HalfPrecisionFloating(U8 *output, float *input);

#if(MaCaco_PERSISTANCE)
U8 Souliss_Persistence_IsData(U8 *memory_map, U8 id);
void Souliss_Persistence_ClearData(U8 *memory_map, U8 id);
#endif

#if(MaCaco_LASTIN)
U8 Souliss_LastIn_IsData(U8 *memory_map, U8 id);
void Souliss_LastIn_ClearData(U8 *memory_map, U8 id);
U8 Souliss_LastIn_GetData(U8 *memory_map, U8 id, U8 slot);
float Souliss_LastIn_GetAnalog(U8 *memory_map, U8 id, U8 slot);
#endif

#if(MCU_TYPE == 0x01) 	// Atmel AVR Atmega
#	if(HTTPSERVER && VNET_MEDIA1_ENABLE && (ETH_W5100 || ETH_W5200 || ETH_W5500))
#		include "interfaces/mcu_avr/HTTP.h"
#	elif(HTTPSERVER && VNET_MEDIA1_ENABLE && ETH_ENC28J60)
#		include "interfaces/mcu_avr/HTTP_uIP.h"
#	elif(ARDUINO_ETHLIB && VNET_MEDIA1_ENABLE && (ETH_W5100 || ETH_W5200 || ETH_W5500))
#		include	"webhook/webhook.h"
#	elif(XMLSERVER && VNET_MEDIA1_ENABLE && (ETH_W5100 || ETH_W5200 || ETH_W5500 || ETH_ENC28J60))
#		include "interfaces/mcu_avr/XMLServer.h"
#	elif(MODBUS)
#		include "interfaces/mcu_avr/Modbus.h"
#	endif
#elif(MCU_TYPE == 0x02)	// Expressif ESP8266
#	if(WEBCONFIGSERVER)
#		include "interfaces/mcu_esp8266/webconfig/webconfig.h"
#	endif
#elif(MCU_TYPE == 0x03)	// Expressif ESP32
#	if(WEBCONFIGSERVER)
#		include "interfaces/mcu_esp32/webconfig/webconfig.h"
#	endif

#endif

// Include IO definitions and other tools
#include "hardware/IOdef.h"
#include "tools/IEEE754/float16.h"

// Some bytes in the EEPROM are reserved
#if(USEEEPROM)
#	include "tools/store/store.h"
#	include "tools/store/store.cpp"
#endif

// Inlcude framework code
#include "frame/vNet/vNet.cpp"
#include "frame/MaCaco/MaCaco.cpp"

// Include IO definitions and other tools
#include "hardware/IOdef.cpp"

// Include methods for half-precision floating points and strings
#include "tools/IEEE754/float16.c"
#include "tools/strings/strings.c"

#if(MCU_TYPE == 0x01) 	// Atmel AVR Atmega
	#if(HTTPSERVER && VNET_MEDIA1_ENABLE && (ETH_W5100 || ETH_W5200 || ETH_W5500))
		#include "interfaces/mcu_avr/HTTP.cpp"
	#elif(HTTPSERVER && VNET_MEDIA1_ENABLE && ETH_ENC28J60)
		#include "interfaces/mcu_avr/HTTP_uIP.cpp"
	#elif((XMLSERVER == 1) && (VNET_MEDIA1_ENABLE && (ETH_W5100 || ETH_W5200 || ETH_W5500)))
	#	include "interfaces/mcu_avr/XMLServer_HTTP.cpp"
	#elif((XMLSERVER == 1) && (VNET_MEDIA1_ENABLE && ETH_ENC28J60))
	#	include "interfaces/mcu_avr/XMLServer_HTTP_uIP.cpp"
	#elif(MODBUS)
	#	include "interfaces/mcu_avr/Modbus.cpp"
	#	endif
#elif(MCU_TYPE == 0x02)	// Expressif ESP8266
	#if(WEBCONFIGSERVER)
	#	include "interfaces/mcu_esp8266/webconfig/webconfig.cpp"
	#endif
#elif(MCU_TYPE == 0x03)	// Expressif ESP32
	#if(WEBCONFIGSERVER)
	#	include "interfaces/mcu_esp32/webconfig/webconfig.cpp"
	#endif
#endif


#if(MCU_TYPE == 0x01)	// ATmega AVR
#elif(MCU_TYPE == 0x02)	// Expressif ESP8266
#elif(MCU_TYPE == 0x03)	// Expressif ESP32
#endif

// Include Souliss code base and typicals
#include "base/Communication.cpp"
#include "base/LocalIO.cpp"
#include "base/RemoteIO.cpp"
#include "base/NetworkSetup.cpp"
#include "base/LastIn.cpp"
#include "base/Persistence.cpp"
#include "base/T1n.cpp"
#include "base/T2n.cpp"
#include "base/T3n.cpp"
#include "base/T4n.cpp"
#include "base/T5n.cpp"
#include "base/T6n.cpp"

#include "tools/types.h"
#include "GetConfig.h"			// need : ethUsrCfg.h, vNetCfg.h, SoulissCfg.h, MaCacoCfg.h

#include "base/PublishSubscribe.h"
#include "base/Sleeping.h"
#include "base/SpeakEasy.h"

#include "frame/MaCaco/MaCaco.h"
#include "frame/vNet/vNet.h"

#include "user/user_config.h"

#endif
