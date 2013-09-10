/**************************************************************************
	Souliss
    Copyright (C) 2012  Veseo

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

#include "GetConfig.h"			// need : ethUsrCfg.h, vNetCfg.h, SoulissCfg.h

#ifndef MODBUS_H
#define MODBUS_H

#define MODBUS_BROADCAST_ADDRESS 				0

#define	MODBUS_FAIL								0
#define	MODBUS_SUCCESS							1
#define	MODBUS_EXCEPTION						2
#define	MODBUS_MAXFRAME							16
#define MODBUS_REGISTERS						MaCaco_SLOT							// Number of registers for the output frame
#define	MODBUS_OUTFRAME							2*MODBUS_REGISTERS					// Number of bytes for the output frame

// Modbus Addresses Mapping for Registers
#define	REGISTER_MODBUS_START_ADDRESS			0x0000		
#define	REGISTER_MODBUS_END_ADDRESS				0x02FF
#define	REGISTER_MODBUS_MAX_AREA_SIZE			0x00FF
#define	REGISTER_INPUT_START_ADDRESS			0x0000
#define	REGISTER_INPUT_END_ADDRESS				0x00FF
#define	REGISTER_TYP_START_ADDRESS				0x0100
#define	REGISTER_TYP_END_ADDRESS				0x01FF
#define	REGISTER_OUTPUT_START_ADDRESS			0x0200
#define	REGISTER_OUTPUT_END_ADDRESS				0x02FF

// Modbus Addresses Mapping for Coils
#define	COILS_MODBUS_START_ADDRESS				0x0000		
#define	COILS_MODBUS_END_ADDRESS				0x2FF0
#define COILS_MODBUS_MAX_AREA_SIZE				0x0FF0
#define	COILS_INPUT_START_ADDRESS				0x0000
#define	COILS_INPUT_END_ADDRESS					0x0FF0
#define	COILS_TYP_START_ADDRESS					0x1000
#define	COILS_TYP_END_ADDRESS					0x1FF0
#define	COILS_OUTPUT_START_ADDRESS				0x2000
#define	COILS_OUTPUT_END_ADDRESS				0x2FF0

// Supported function codes 
#define MODBUS_READ_COIL			    		0x01
#define MODBUS_READ_DISCRETE_INPUTS	    		0x02
#define MODBUS_READ_HOLDING_REGISTERS    		0x03
#define MODBUS_READ_INPUT_REGISTERS   	 		0x04
#define MODBUS_WRITE_SINGLE_COIL  				0x05
#define MODBUS_WRITE_SINGLE_REGISTERS  			0x06

#define MODBUS_FIRST_FC							0x01
#define	MODBUS_LAST_FC							0x06

// Protocol exceptions
#define MODBUS_EXCEPTION_ILLEGAL_FUNCTION     	0x1
#define MODBUS_EXCEPTION_ILLEGAL_DATA_ADDRESS 	0x2
#define MODBUS_EXCEPTION_ILLEGAL_DATA_VALUE   	0x3

// Headers
#define	MODBUS_TCP_ANSWER_HEADER				12
#define MODBUS_RTU_ANSWER_HEADER				6
#define	MODBUS_TCP_REPLY_HEADER					9
#define MODBUS_RTU_REPLY_HEADER					3

// Used to swap from little-endian to big-endian and viceversa, the AVRs use little
// endian rapresentation, rather Modbus and IP are big-endian
#ifndef HTONS
#	define HTONS(n) ((((U16)((n) & 0xff)) << 8) | (((n) & 0xff00) >> 8))
#endif

typedef struct
{
	U16	transactionid;			// Transaction ID
	U16	protocolid;				// Protocol ID
	U16 lenght;					// Frame lenght to end
	U8  unitid;					// Unit ID
	U8  functionalcode;			// Functional Code
	U16	startingaddress;		// Starting address
	U16	registers;				// Number of register
} ModbusTCP;

typedef struct
{
	U8  unitid;					// Unit ID
	U8  functionalcode;			// Functional Code
	U16	startingaddress;		// Starting address
	U16	registers;				// Number of register
} ModbusRTU;

typedef struct
{
	U16	transactionid;			// Transaction ID
	U16	protocolid;				// Protocol ID
	U16 lenght;					// Frame lenght to end
	U8  unitid;					// Unit ID
	U8  functionalcode;			// Functional Code
	U8	bytecount;				// Byte count
} ModbusTCPReply;

typedef struct
{
	U8  unitid;					// Unit ID
	U8  functionalcode;			// Functional Code
	U8	bytecount;				// Byte count
} ModbusRTUReply;


U8 ModbusAvailable();
U8 ModbusRetrieve();
U8 ModbusVerify();
U8 ModbusReply(U8 *memory_map);
void ModbusSend();
void ModbusEcho();
U8 ModbusExecption();

// Methods for public use
U8 ModbusInit();
U8 Modbus(U8 *memory_map);
void ModbusRemoteInput(U8 *memory_map);

#endif
