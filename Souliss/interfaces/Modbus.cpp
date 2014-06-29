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

/** 

	This interface runs either with MaCaco PERSISTANCE mode enabled or disabled
	isn't designed to run with MaCaco LASTIN mode

**/

#include "Typicals.h"
#include "Souliss.h"
#include "frame/MaCaco/MaCaco.h"
#include "frame/vNet/vNet.h"

#include "Modbus.h"

U8 l=0, sock;
U8 modbusin[MODBUS_MAXFRAME];						// Data buffer for incoming data
U8 modbusdata[4+MODBUS_OUTFRAME];					// Data buffer for output data (plus a float size)
oFrame Modbus_oFrame;								// Modbus output frame	

U8 modbustranfer_id;								// Data to be transferred to this id
U8 modbustranfer_slot;								// Data to be transferred to this slot
U8 modbustranfer_var;								// Data to be transferred to this val

extern oFrame vNetM1_oFrame;						// Data structure for output frame

#if(MODBUS_RTU)
	U8	modbuscrc[2];								// CRC buffer
	oFrame Modbus_CRC;								// Modbus CRC	

	#define	MODBUS_ANSWER_HEADER	MODBUS_RTU_ANSWER_HEADER
	#define	MODBUS_REPLY_HEADER		MODBUS_RTU_REPLY_HEADER

	#define	modbusframe ((ModbusRTU *)modbusin)				// Pointer to the data buffer
	#define	modbusreply ((ModbusRTUReply *)modbusin)		// Pointer to the data buffer	
	
#elif(MODBUS_TCP)

	#define	MODBUS_ANSWER_HEADER	MODBUS_TCP_ANSWER_HEADER
	#define	MODBUS_REPLY_HEADER		MODBUS_TCP_REPLY_HEADER

	#define	modbusframe ((ModbusTCP *)modbusin)				// Pointer to the data buffer
	#define	modbusreply ((ModbusTCPReply *)modbusin)		// Pointer to the data buffer	
#endif	

#if (MODBUS_DEBUG && !(MODBUS_RTU))
	#define MODBUS_LOG Serial.print
#endif

/**************************************************************************
/*!
	Calculate the CRC16 for the data frame, used only in case of Modbus RTU
*/	
/**************************************************************************/
#if(MODBUS_RTU)
uint16_t crc16(oFrame *frame)
{
    uint8_t j, len;
    uint16_t crc;
	oFrame tmpframe;
	
	// Make a copy of the output frame
	oFrame_Copy(&tmpframe, frame);
	tmpframe.next=0;
	// Use the temporary output frame	
	oFrame_Define(&tmpframe);
	len = oFrame_GetLenght();
	//len=-2;						// We want only the lenght of data, not the CRC
	
    crc = 0xFFFF;
    while (len--) 
	{
		crc = crc ^ (oFrame_GetByte());
		for (j=0; j < 8; j++) 
		{
			if (crc & 0x0001)
				crc = (crc >> 1) ^ 0xA001;
			else
				crc = crc >> 1;
		}
    }

	// Set back the old frame before return
	oFrame_Define(frame);	
	
    return (crc << 8  | crc >> 8);
}
#endif

/**************************************************************************
/*!
	Setup the Modbus TCP or RTU 
*/	
/**************************************************************************/
U8 ModbusInit(U8 *memory_map)
{
#if(MODBUS_RTU)
	// The Serial class and the USART interrupt are automatically set by the
	// Arduino IDE, so isn't impossible use a dedicated code for serial data.
	Serial.begin(MODBUS_RTU_BAUD);
#elif(MODBUS_TCP && ((ETH_W5100 || ETH_W5200 || ETH_W5500) || ETH_W5200 || ETH_W5500))
	// Set a listening Modbus port
	srv_listen(MODBUS_TCP_PORT); 
#elif(MODBUS_TCP && (ETH_ENC28J60 || WIFI_MRF24))
	// Set a listening Modbus port
	srv_listen(MODBUS_TCP_PORT); 
#endif	

	// Set an internal subscription in order to collect data from other
	// nodes in the network
	MaCaco_InternalSubcription(memory_map);
}

/**************************************************************************
/*!
	Look for available data as Modbus TCP or RTU
*/	
/**************************************************************************/
U8 ModbusAvailable()
{
#if(MODBUS_RTU)
	l=0;

	// From the USART we get one byte per time, so before has to be identified
	// if there is an incoming Modbus frame	
	while(Serial.available())
		modbusin[l++] = Serial.read();
	
	// Verify if the incoming frame is a Modbus one
	if(l == MODBUS_RTU_ANSWER_HEADER+2)								// RTU frame with checksum
	{	
		// Verify the ID and the FC type to identify the frame
		if(modbusframe->unitid == MODBUS_ID)
			return MODBUS_SUCCESS;
		else
			return MODBUS_FAIL;
	}

	return MODBUS_FAIL;
	
#elif(MODBUS_TCP && (ETH_W5100 || ETH_W5200 || ETH_W5500))

	// Set the socket
	srvcln_setsocket(SRV_SOCK2);

	// Look for available data on the socket
	return srvcln_dataavailable(MODBUS_TCP_PORT);
	
#elif(MODBUS_TCP && (ETH_ENC28J60 || WIFI_MRF24))

	// Look for available data on the listening port
	sock = srvcln_getsocket(MODBUS_TCP_PORT);
	
	// If there are available incoming data
	return srvcln_dataavailable(MODBUS_TCP_PORT);			
	
#endif

}

/**************************************************************************
/*!
	Retrieve data as Modbus TCP or RTU
*/	
/**************************************************************************/
U8 ModbusRetrieve()
{
#if(MODBUS_RTU)
	// For USART data was yet retrieved with the ModbusAvailable() method
	// just swap to little endian
	modbusframe->startingaddress = HTONS(modbusframe->startingaddress);
	modbusframe->registers = HTONS(modbusframe->registers);
	
	return MODBUS_SUCCESS;
#elif(MODBUS_TCP && (ETH_W5100 || ETH_W5200 || ETH_W5500))

	// Look for available data on the listening port
	l = srvcln_dataavailable(MODBUS_TCP_PORT);
	
	// If there are available incoming data
	if (l) 
	{  
		// If the socket is connected and the data size doesn't exceed the buffer
		if (srvcln_connected(MODBUS_TCP_PORT) && (l < MODBUS_MAXFRAME))										// If data are available
		{
			// Retrieve data from the Ethernet controller
			srvcln_retrieve(modbusin, l);	
			
			// Include debug functionalities, if required
#			if(MODBUS_DEBUG)

				MODBUS_LOG("(MBUS)<IN><0x");
				MODBUS_LOG(modbusin[0],HEX);
				for(U8 i=1;i<l;i++)
				{
					MODBUS_LOG(",0x");
					MODBUS_LOG(modbusin[i],HEX);
						
				}
				MODBUS_LOG(">\r\n");
				
#			endif
			
			// Swap from big-endian to little-endian
			modbusframe->transactionid = HTONS(modbusframe->transactionid);
			modbusframe->protocolid = HTONS(modbusframe->protocolid);
			modbusframe->lenght = HTONS(modbusframe->lenght);
			modbusframe->startingaddress = HTONS(modbusframe->startingaddress);
			modbusframe->registers = HTONS(modbusframe->registers);
	
			return l;
		}
	}
	
	return MODBUS_FAIL;
	
#elif(MODBUS_TCP && (ETH_ENC28J60 || WIFI_MRF24))

	// If there are available incoming data
	l = srvcln_dataavailable(MODBUS_TCP_PORT);
	
	if (l)
	{
		// Retrieve data from the Ethernet controller
		srvcln_retrieve(modbusin, MODBUS_MAXFRAME);	
		
			// Include debug functionalities, if required
	#	if(MODBUS_DEBUG)
			MODBUS_LOG("(MBUS)<IN><0x");
			MODBUS_LOG(modbusin[0],HEX);
			for(U8 i=1;i<l;i++)
			{
				MODBUS_LOG(",0x");
				MODBUS_LOG(modbusin[i],HEX);
					
			}
			MODBUS_LOG(">\r\n");			
	#	endif
			
		// Swap from big-endian to little-endian
		modbusframe->transactionid = HTONS(modbusframe->transactionid);
		modbusframe->protocolid = HTONS(modbusframe->protocolid);
		modbusframe->lenght = HTONS(modbusframe->lenght);
		modbusframe->startingaddress = HTONS(modbusframe->startingaddress);
		modbusframe->registers = HTONS(modbusframe->registers);
	
		return l;
	}
	
	return MODBUS_FAIL;
	
#endif
}

/**************************************************************************
/*!
	Parse the incoming 
*/	
/**************************************************************************/
U8 ModbusVerify()
{
	U8 byteqty, bytenumber;

	// Verify the Unit ID
	if((modbusframe->unitid != MODBUS_ID) && (modbusframe->unitid != MODBUS_BROADCAST_ADDRESS))
	{
		// Prepare an execption answer
		modbusreply->unitid = MODBUS_ID;
		modbusreply->functionalcode += 0x80;
		modbusreply->bytecount = MODBUS_EXCEPTION_ILLEGAL_DATA_VALUE;

		// Print a log message
#		if(MODBUS_DEBUG)
			MODBUS_LOG("(MBUS)<IN>ILLEGAL_DATA_VALUE");
			MODBUS_LOG(">\r\n");		
#		endif
	
		return MODBUS_EXCEPTION;
	}
		
	// Verify the functional code
	if((modbusframe->functionalcode < MODBUS_FIRST_FC) || (modbusframe->functionalcode > MODBUS_LAST_FC))
	{
		// Prepare an execption answer
		modbusreply->functionalcode += 0x80;
		modbusreply->bytecount = MODBUS_EXCEPTION_ILLEGAL_FUNCTION;

		// Print a log message
#		if(MODBUS_DEBUG)
			MODBUS_LOG("(MBUS)<IN>ILLEGAL_FUNCTION");
			MODBUS_LOG(">\r\n");		
#		endif
			
		return MODBUS_EXCEPTION;
	}

	// Perfor checks based on the functional code requested
	if((modbusframe->functionalcode == MODBUS_READ_COIL) || (modbusframe->functionalcode == MODBUS_READ_DISCRETE_INPUTS) || (modbusframe->functionalcode == MODBUS_WRITE_SINGLE_COIL))
	{
		// Verify the starting address
		if(!(modbusframe->startingaddress >= COILS_MODBUS_START_ADDRESS) && !(modbusframe->startingaddress <= COILS_MODBUS_END_ADDRESS))
		{
			// Prepare an execption answer
			modbusreply->functionalcode += 0x80;
			modbusreply->bytecount = MODBUS_EXCEPTION_ILLEGAL_DATA_ADDRESS;

			// Print a log message
		#	if(MODBUS_DEBUG)
				MODBUS_LOG("(MBUS)<IN>ILLEGAL_DATA_ADDRESS 1a");
				MODBUS_LOG(">\r\n");			
		#	endif
			
			return MODBUS_EXCEPTION;		
		}

		// Verify the number of bytes requested against the memory area
		if((modbusframe->functionalcode == MODBUS_READ_COIL) && (modbusframe->registers > 8*(MaCaco_NODES*MaCaco_SLOT)))
		{
			// Prepare an execption answer
			modbusreply->functionalcode += 0x80;
			modbusreply->bytecount = MODBUS_EXCEPTION_ILLEGAL_DATA_ADDRESS;

			// Print a log message
		#	if(MODBUS_DEBUG)		
				MODBUS_LOG("(MBUS)<IN>ILLEGAL_DATA_ADDRESS 2a");
				MODBUS_LOG(">\r\n");			
		#	endif
					
			return MODBUS_EXCEPTION;		
		}	

		// Verify the reading address request
		if((modbusframe->functionalcode == MODBUS_READ_COIL) || (modbusframe->functionalcode == MODBUS_READ_DISCRETE_INPUTS))
		{
			// Calculate the bit number and the byte offset into the memory_map from the requested bit
			bytenumber  = (modbusframe->startingaddress)/8;
				
			if(modbusframe->registers%8)
				byteqty = 1 + modbusframe->registers / 8;
			else
				byteqty = modbusframe->registers / 8;		

			
			// Verify the maximum number of byte that can be requested
			if(byteqty > (MaCaco_NODES*MaCaco_SLOT))
			{
				// Prepare an execption answer
				modbusreply->functionalcode += 0x80;
				modbusreply->bytecount = MODBUS_EXCEPTION_ILLEGAL_DATA_ADDRESS;

				// Print a log message
			#	if(MODBUS_DEBUG)		
				MODBUS_LOG("(MBUS)<IN>ILLEGAL_DATA_ADDRESS 3a");
				MODBUS_LOG(">\r\n");			
			#	endif
								
				return MODBUS_EXCEPTION;
			}
			
			// Verify for each group of the memory map if the address is within the memory size
			if((modbusframe->startingaddress >= COILS_INPUT_START_ADDRESS) && (modbusframe->startingaddress <= COILS_INPUT_END_ADDRESS))
			{
				if((bytenumber + byteqty) > (MaCaco_NODES*MaCaco_SLOT))
				{
					// Prepare an execption answer
					modbusreply->functionalcode += 0x80;
					modbusreply->bytecount = MODBUS_EXCEPTION_ILLEGAL_DATA_ADDRESS;

					// Print a log message
				#		if(MODBUS_DEBUG)
						MODBUS_LOG("(MBUS)<IN>ILLEGAL_DATA_ADDRESS 4a");
						MODBUS_LOG(">\r\n");
				#		endif
								
					return MODBUS_EXCEPTION;		
				}	
			}
			else if((modbusframe->startingaddress >= COILS_TYP_START_ADDRESS) && (modbusframe->startingaddress <= COILS_TYP_END_ADDRESS))
			{
				if((bytenumber + byteqty + MaCaco_G_TYP_s) > MaCaco_G_TYP_f)
				{
					// Prepare an execption answer
					modbusreply->functionalcode += 0x80;
					modbusreply->bytecount = MODBUS_EXCEPTION_ILLEGAL_DATA_ADDRESS;

					// Print a log message
				#	if(MODBUS_DEBUG)
						MODBUS_LOG("(MBUS)<IN>ILLEGAL_DATA_ADDRESS 5a");
						MODBUS_LOG(">\r\n");
				#	endif
								
					return MODBUS_EXCEPTION;		
				}	
			}		
			else if((modbusframe->startingaddress >= COILS_OUTPUT_START_ADDRESS) && (modbusframe->startingaddress <= COILS_OUTPUT_END_ADDRESS))
			{
				if((bytenumber + byteqty + MaCaco_G_OUT_s) > MaCaco_G_OUT_f)
				{
					// Prepare an execption answer
					modbusreply->functionalcode += 0x80;
					modbusreply->bytecount = MODBUS_EXCEPTION_ILLEGAL_DATA_ADDRESS;

					// Print a log message
				#	if(MODBUS_DEBUG)
						MODBUS_LOG("(MBUS)<IN>ILLEGAL_DATA_ADDRESS 6a");
						MODBUS_LOG(">\r\n");
				#	endif
								
					return MODBUS_EXCEPTION;		
				}
			}		
		}

		// Verify the writing addresses request
		if(modbusframe->functionalcode == MODBUS_WRITE_SINGLE_COIL) 
		{	
			// Calculate the bit number and the byte offset into the memory_map from the requested bit
			bytenumber  = (modbusframe->startingaddress)/8;
				
			if(modbusframe->registers%8)
				byteqty = 1 + modbusframe->registers / 8;
			else
				byteqty = modbusframe->registers / 8;		

					
			// Verify that the requested address is writeble
			if(!(modbusframe->startingaddress >= COILS_MODBUS_START_ADDRESS) && !(modbusframe->startingaddress <= COILS_MODBUS_END_ADDRESS))
			{
				// Prepare an execption answer
				modbusreply->functionalcode += 0x80;
				modbusreply->bytecount = MODBUS_EXCEPTION_ILLEGAL_DATA_ADDRESS;

				// Print a log message
			#	if(MODBUS_DEBUG)
					MODBUS_LOG("(MBUS)<IN>ILLEGAL_DATA_ADDRESS 7a");
					MODBUS_LOG(">\r\n");
			#	endif
								
				return MODBUS_EXCEPTION;		

			}

			// Verify that the requested address is writeble
			if(!(modbusframe->startingaddress >= COILS_INPUT_START_ADDRESS) || !(modbusframe->startingaddress <= COILS_INPUT_END_ADDRESS))
			{
				// Prepare an execption answer
				modbusreply->functionalcode += 0x80;
				modbusreply->bytecount = MODBUS_EXCEPTION_ILLEGAL_DATA_ADDRESS;

				// Print a log message
			#	if(MODBUS_DEBUG)
					MODBUS_LOG("(MBUS)<IN>ILLEGAL_DATA_ADDRESS 8a");
					MODBUS_LOG(">\r\n");
			#	endif
								
				return MODBUS_EXCEPTION;		

			}
		/*	
			// Verify that requested register allow writing operations
			if((bytenumber + MaCaco_IN_s) > MaCaco_WRITE_f)
			{
				// Prepare an execption answer
				modbusreply->functionalcode += 0x80;
				modbusreply->bytecount = MODBUS_EXCEPTION_ILLEGAL_DATA_ADDRESS;

				// Print a log message
			#	if(MODBUS_DEBUG)
					MODBUS_LOG("(MBUS)<IN>ILLEGAL_DATA_ADDRESS 9a");
					MODBUS_LOG(">\r\n");
			#	endif
						
				return MODBUS_EXCEPTION;		
			}
		*/
		
		}		
	}
	else if((modbusframe->functionalcode == MODBUS_READ_HOLDING_REGISTERS) || (modbusframe->functionalcode == MODBUS_READ_INPUT_REGISTERS) || (modbusframe->functionalcode == MODBUS_WRITE_SINGLE_REGISTERS))
	{	
		// Verify the starting address
		if(!(modbusframe->startingaddress >= REGISTER_MODBUS_START_ADDRESS) && !(modbusframe->startingaddress <= REGISTER_MODBUS_END_ADDRESS))
		{
			// Prepare an execption answer
			modbusreply->functionalcode += 0x80;
			modbusreply->bytecount = MODBUS_EXCEPTION_ILLEGAL_DATA_ADDRESS;

			// Print a log message
		#	if(MODBUS_DEBUG)
				MODBUS_LOG("(MBUS)<IN>ILLEGAL_DATA_ADDRESS 1b");
				MODBUS_LOG(">\r\n");
		#	endif
			
			return MODBUS_EXCEPTION;		
		}

		// Verify the number of bytes requested
		if((modbusframe->functionalcode == MODBUS_READ_HOLDING_REGISTERS) && (modbusframe->registers > (MODBUS_REGISTERS)))
		{
			// Prepare an execption answer
			modbusreply->functionalcode += 0x80;
			modbusreply->bytecount = MODBUS_EXCEPTION_ILLEGAL_DATA_ADDRESS;

			// Print a log message
		#	if(MODBUS_DEBUG)
				MODBUS_LOG("(MBUS)<IN>ILLEGAL_DATA_ADDRESS 2b");
				MODBUS_LOG(">\r\n");
		#	endif
					
			return MODBUS_EXCEPTION;		
		}	

		// Verify the reading address request
		if((modbusframe->functionalcode == MODBUS_READ_HOLDING_REGISTERS) || (modbusframe->functionalcode == MODBUS_READ_INPUT_REGISTERS))
		{
			// Calculate the bit number and the byte offset into the memory_map from the requested bit
			bytenumber  = modbusframe->startingaddress;
			byteqty = modbusframe->registers;		
			
			// Verify the maximum number of byte that can be requested
			if(byteqty > MODBUS_REGISTERS)
			{
				// Prepare an execption answer
				modbusreply->functionalcode += 0x80;
				modbusreply->bytecount = MODBUS_EXCEPTION_ILLEGAL_DATA_ADDRESS;

				// Print a log message
			#	if(MODBUS_DEBUG)
				MODBUS_LOG("(MBUS)<IN>ILLEGAL_DATA_ADDRESS 3b");
				MODBUS_LOG(">\r\n");
			#	endif
								
				return MODBUS_EXCEPTION;
			}
			
			// Verify for each group of the memory map if the address is within the memory size
			if((modbusframe->startingaddress >= REGISTER_INPUT_START_ADDRESS) && (modbusframe->startingaddress <= REGISTER_INPUT_END_ADDRESS))
			{
				if((bytenumber + byteqty) > (MaCaco_NODES*MaCaco_SLOT))
				{
					// Prepare an execption answer
					modbusreply->functionalcode += 0x80;
					modbusreply->bytecount = MODBUS_EXCEPTION_ILLEGAL_DATA_ADDRESS;

					// Print a log message
				#		if(MODBUS_DEBUG)
						MODBUS_LOG("(MBUS)<IN>ILLEGAL_DATA_ADDRESS 4b");
						MODBUS_LOG(">\r\n");
				#		endif
								
					return MODBUS_EXCEPTION;		
				}	
			}
			else if((modbusframe->startingaddress >= REGISTER_TYP_START_ADDRESS) && (modbusframe->startingaddress <= REGISTER_TYP_END_ADDRESS))
			{
				if((bytenumber + byteqty + MaCaco_G_TYP_s) > MaCaco_G_TYP_f)
				{
					// Prepare an execption answer
					modbusreply->functionalcode += 0x80;
					modbusreply->bytecount = MODBUS_EXCEPTION_ILLEGAL_DATA_ADDRESS;

					// Print a log message
				#	if(MODBUS_DEBUG)
						MODBUS_LOG("(MBUS)<IN>ILLEGAL_DATA_ADDRESS 5b");
						MODBUS_LOG(">\r\n");
				#	endif
								
					return MODBUS_EXCEPTION;		
				}	
			}		
			else if((modbusframe->startingaddress >= REGISTER_OUTPUT_START_ADDRESS) && (modbusframe->startingaddress <= REGISTER_OUTPUT_END_ADDRESS))
			{
				if((bytenumber + byteqty + MaCaco_G_OUT_s) > MaCaco_G_OUT_f)
				{
					// Prepare an execption answer
					modbusreply->functionalcode += 0x80;
					modbusreply->bytecount = MODBUS_EXCEPTION_ILLEGAL_DATA_ADDRESS;
	
					// Print a log message
				#	if(MODBUS_DEBUG)	
						MODBUS_LOG("(MBUS)<IN>ILLEGAL_DATA_ADDRESS 6b");
						MODBUS_LOG(">\r\n");		
				#	endif
								
					return MODBUS_EXCEPTION;		
				}
			}		
		}

		// Verify the writing addresses request
		if(modbusframe->functionalcode == MODBUS_WRITE_SINGLE_REGISTERS) 
		{
			// Calculate the bit number and the byte offset into the memory_map from the requested bit
			bytenumber  = modbusframe->startingaddress;
			byteqty = modbusframe->registers;				
		
			// Verify that requested register are inputs
			if(!(modbusframe->startingaddress >= REGISTER_MODBUS_START_ADDRESS) && !(modbusframe->startingaddress <= REGISTER_MODBUS_END_ADDRESS))
			{
				// Prepare an execption answer
				modbusreply->functionalcode += 0x80;
				modbusreply->bytecount = MODBUS_EXCEPTION_ILLEGAL_DATA_ADDRESS;

				// Print a log message
			#	if(MODBUS_DEBUG)
					MODBUS_LOG("(MBUS)<IN>ILLEGAL_DATA_ADDRESS 7b");
					MODBUS_LOG(">\r\n");
			#	endif
						
				return MODBUS_EXCEPTION;		
			}
		
			// Verify that requested register are inputs
			if(!(modbusframe->startingaddress >= REGISTER_INPUT_START_ADDRESS) || !(modbusframe->startingaddress <= REGISTER_INPUT_END_ADDRESS))
			{
				// Prepare an execption answer
				modbusreply->functionalcode += 0x80;
				modbusreply->bytecount = MODBUS_EXCEPTION_ILLEGAL_DATA_ADDRESS;

				// Print a log message
			#	if(MODBUS_DEBUG)
					MODBUS_LOG("(MBUS)<IN>ILLEGAL_DATA_ADDRESS 8b");
					MODBUS_LOG(">\r\n");
			#	endif
						
				return MODBUS_EXCEPTION;		
			}		
		
		/*
			// Verify that requested register allow writing operations
			if((bytenumber + MaCaco_IN_s) > MaCaco_WRITE_f)
			{	
				// Prepare an execption answer
				modbusreply->functionalcode += 0x80;
				modbusreply->bytecount = MODBUS_EXCEPTION_ILLEGAL_DATA_ADDRESS;

				// Print a log message
			#	if(MODBUS_DEBUG)
					MODBUS_LOG("(MBUS)<IN>ILLEGAL_DATA_ADDRESS 9b");
					MODBUS_LOG(">\r\n");
			#	endif
						
				return MODBUS_EXCEPTION;		
			}
		*/
		
		}
	}
	
	return MODBUS_SUCCESS;
	
}

/**************************************************************************
/*!
	Reply the incoming request
*/	
/**************************************************************************/
U8 ModbusReply(U8 *memory_map)
{
	// Map the Modbus request to the MaCaco data area
	U8 memory_map_address;	
			
	// Parse the functinal code
	if((modbusframe->functionalcode == MODBUS_READ_COIL) || (modbusframe->functionalcode == MODBUS_READ_DISCRETE_INPUTS))
	{	
		// Map the the Modbus registers with the internal memory
		if((modbusframe->startingaddress >= COILS_INPUT_START_ADDRESS) && (modbusframe->startingaddress <= COILS_INPUT_END_ADDRESS))
			memory_map_address = MaCaco_IN_s;
		else if((modbusframe->startingaddress >= COILS_TYP_START_ADDRESS) && (modbusframe->startingaddress <= COILS_TYP_END_ADDRESS))
			memory_map_address = MaCaco_G_TYP_s;
		else if((modbusframe->startingaddress >= COILS_OUTPUT_START_ADDRESS) && (modbusframe->startingaddress <= COILS_OUTPUT_END_ADDRESS))
			memory_map_address = MaCaco_G_OUT_s;
	
		// Calculate the bit number and the byte offset into the memory_map from the requested bit
		U8 byteqty, bytenumber  = (modbusframe->startingaddress)/8;

		if(modbusframe->registers%8)
			byteqty = 1 + modbusframe->registers / 8;
		else
			byteqty = modbusframe->registers / 8;		
	
		// Build a frame, only the pointer are stored, so it can be changed
		// also after the output frame is build
		
		#if(MODBUS_RTU)
			oFrame_Define(&Modbus_CRC);
			oFrame_Set(0, modbuscrc, 0, 2, 0);
			oFrame_Define(&Modbus_oFrame);
		
			// If the input values are requested
			if(memory_map_address == MaCaco_IN_s)
			{
				// Reply with a full zero payload, input values are not available
				for(U8 i=0; i<byteqty; i++)
					modbusdata[i]=0;
				
				// Set the output frame
				oFrame_Set(modbusin, modbusdata, MODBUS_REPLY_HEADER, byteqty, &Modbus_CRC);
			}
			else
				oFrame_Set(modbusin, (memory_map+memory_map_address+bytenumber), MODBUS_REPLY_HEADER, byteqty, &Modbus_CRC);
		
		#elif(MODBUS_TCP)
			oFrame_Define(&Modbus_oFrame);

			// If the input values are requested
			if(memory_map_address == MaCaco_IN_s)
			{
				// Reply with a full zero payload, input values are not available
				for(U8 i=0; i<byteqty; i++)
					modbusdata[i]=0;
				
				// Set the output frame
				oFrame_Set(modbusin, modbusdata, MODBUS_REPLY_HEADER, byteqty, 0);
			}
			else
				oFrame_Set(modbusin, (memory_map+memory_map_address+bytenumber), MODBUS_REPLY_HEADER, byteqty, 0);		
		#endif
		
		// Prepare the header
		modbusframe->startingaddress = 0;
		modbusreply->bytecount = (byteqty);
		
		#if(MODBUS_RTU)
			// Prepare the CRC
			*(U16*)modbuscrc = HTONS(crc16(&Modbus_oFrame));	
		#elif(MODBUS_TCP)
			modbusreply->lenght = 3+modbusreply->bytecount;			// In lenght is not reported the whole header lenght
		#endif

		// Print a log message
	#	if(MODBUS_DEBUG)
			MODBUS_LOG("(MBUS)<IN>READ_COIL");
			MODBUS_LOG(">\r\n");
	#	endif
						
		return MODBUS_READ_COIL;
	}	
	else if((modbusframe->functionalcode == MODBUS_READ_HOLDING_REGISTERS) || (modbusframe->functionalcode == MODBUS_READ_INPUT_REGISTERS))
	{
		// Map the the Modbus registers with the internal memory
		if((modbusframe->startingaddress >= REGISTER_INPUT_START_ADDRESS) && (modbusframe->startingaddress <= REGISTER_INPUT_END_ADDRESS))
			memory_map_address = 0;
		else if((modbusframe->startingaddress >= REGISTER_TYP_START_ADDRESS) && (modbusframe->startingaddress <= REGISTER_TYP_END_ADDRESS))
			memory_map_address = MaCaco_G_TYP_s + (modbusframe->startingaddress - REGISTER_TYP_START_ADDRESS);
		else if((modbusframe->startingaddress >= REGISTER_OUTPUT_START_ADDRESS) && (modbusframe->startingaddress <= REGISTER_OUTPUT_END_ADDRESS))
			memory_map_address = MaCaco_G_OUT_s + (modbusframe->startingaddress - REGISTER_OUTPUT_START_ADDRESS);
	
		// Modbus data are addressed as words rather Souliss ones as bytes
		// convert them before sending, use big-Endian

		if(memory_map_address == 0)
			for(U8 i=0;i<(modbusframe->registers);i++)	// Input data are not available, fill with zero
				modbusdata[i] = 0;
		else
			for(U8 i=0;i<(modbusframe->registers);i++)
				if((memory_map_address >= MaCaco_G_OUT_s) && (((*(memory_map + (MaCaco_G_TYP_s + (modbusframe->startingaddress - REGISTER_OUTPUT_START_ADDRESS)) + i)) & 0xF0) == Souliss_T5n))
				{
						// If is an analog value, convert it as floating point
						float f_val;
						float* f_val_p = &f_val;
						float32((U16*)(memory_map+memory_map_address+i), &f_val);
						
						// Convert from little to big endian
						HTONF(modbusdata+2*i, f_val_p);

						// The analog values use two slot, so skip the next one
						i++;
				}
				else	// Otherwhise as raw value
					*(U16 *)(&modbusdata[2*i]) = HTONS((U16)(*(memory_map+memory_map_address+i)));

		// Build a frame, only the pointer are stored, so it can be changed
		// also after the output frame is build
		
		#if(MODBUS_RTU)
			oFrame_Define(&Modbus_CRC);
			oFrame_Set(0, modbuscrc, 0, 2, 0);
			oFrame_Define(&Modbus_oFrame);
			oFrame_Set(modbusin, modbusdata, MODBUS_REPLY_HEADER, 2*(modbusframe->registers), &Modbus_CRC);
		#elif(MODBUS_TCP)
			oFrame_Define(&Modbus_oFrame);
			oFrame_Set(modbusin, modbusdata, MODBUS_REPLY_HEADER, 2*(modbusframe->registers), 0);		
		#endif
		
		// Prepare the header
		modbusframe->startingaddress = 0;
		modbusreply->bytecount = 2*(modbusframe->registers);
		
		#if(MODBUS_RTU)
			// Prepare the CRC
			*(U16*)modbuscrc = HTONS(crc16(&Modbus_oFrame));	
		#elif(MODBUS_TCP)
			modbusreply->lenght = 3+modbusreply->bytecount;			// In lenght is not reported the whole header lenght
		#endif
		
		// Print a log message
#		if(MODBUS_DEBUG)
			MODBUS_LOG("(MBUS)<IN>READ_HOLDING_REGISTERS");
			MODBUS_LOG(">\r\n");
#		endif
				
		return MODBUS_READ_HOLDING_REGISTERS;
	}
	else if((modbusframe->functionalcode == MODBUS_WRITE_SINGLE_REGISTERS))
	{
		// Get the index where should be written the data
		U16 index = (modbusframe->startingaddress - REGISTER_INPUT_START_ADDRESS);;
		
		// Calculate the id and slot for the remote node
		modbustranfer_id = index/MaCaco_SLOT;
		modbustranfer_slot = index-(modbustranfer_id*MaCaco_SLOT)-modbustranfer_id;
		
		// Just store the data to be processed at later time, the answer is an echo
		modbustranfer_var = (U8)(modbusframe->registers);
	
		// Is a command local to this node, apply it	
		if(modbustranfer_id==0)
		{
			memory_map[MaCaco_IN_s+modbustranfer_slot] = modbustranfer_var;
			modbustranfer_slot=0;
			modbustranfer_var=0;			
		}
		
		// The answer is just an echo of the incoming message
		#if(MODBUS_RTU)
			oFrame_Define(&Modbus_oFrame);
			oFrame_Set(modbusin, 0, MODBUS_RTU_ANSWER_HEADER+2*(modbusframe->registers)+2, 0, 0);
		#elif(MODBUS_TCP)
			oFrame_Define(&Modbus_oFrame);
			oFrame_Set(modbusin, 0, MODBUS_RTU_ANSWER_HEADER+2*(modbusframe->registers), 0, 0);
			//oFrame_Set(modbusin, 0, MODBUS_REPLY_HEADER+2*(modbusframe->registers), 0, 0);		
		#endif	
		
		// Print a log message
#		if(MODBUS_DEBUG)
			MODBUS_LOG("(MBUS)<IN>WRITE_SINGLE_REGISTERS");
			MODBUS_LOG(">\r\n");
#		endif
					
		return MODBUS_WRITE_SINGLE_REGISTERS;			
	}
	else if((modbusframe->functionalcode == MODBUS_WRITE_SINGLE_COIL))
	{		
		// Calculate the bit number and the byte offset into the memory_map from the requested bit
		U8 bytenumber  = (modbusframe->startingaddress)/8;
		U8 bitnumber = ((modbusframe->startingaddress) - bytenumber*8);

		// Calculate the id and slot for the remote node
		modbustranfer_id = bytenumber/MaCaco_SLOT;
		modbustranfer_slot = bytenumber-(modbustranfer_id*MaCaco_SLOT);
		
		// Just store the data to be processed at later time, the answer is an echo
		modbustranfer_var = 0;
		if(modbusframe->registers)			// In this field is contained a GTZ value if ON else OFF
			modbustranfer_var |= 1 << bitnumber;
		else
			modbustranfer_var &= ~(1 << bitnumber);
		
		// Is a command local to this node, apply it	
		if(modbustranfer_id==0)
		{
			memory_map[MaCaco_IN_s+modbustranfer_slot] = modbustranfer_var;
			modbustranfer_slot=0;
			modbustranfer_var=0;			
		}

		// The answer is just an echo of the incoming message
		#if(MODBUS_RTU)
			oFrame_Define(&Modbus_oFrame);
			oFrame_Set(modbusin, 0, MODBUS_RTU_ANSWER_HEADER+bytenumber+2, 0, 0);
		#elif(MODBUS_TCP)
			oFrame_Define(&Modbus_oFrame);
			oFrame_Set(modbusin, 0, MODBUS_RTU_ANSWER_HEADER+bytenumber, 0, 0);
			//oFrame_Set(modbusin, 0, MODBUS_REPLY_HEADER+2*(modbusframe->registers), 0, 0);		
		#endif	
		
		// Print a log message
#		if(MODBUS_DEBUG)
			MODBUS_LOG("(MBUS)<IN>WRITE_SINGLE_COIL");
			MODBUS_LOG(">\r\n");
#		endif
					
		return MODBUS_WRITE_SINGLE_COIL;		
	}

	// Print a log message
#	if(MODBUS_DEBUG)
		MODBUS_LOG("(MBUS)<IN>REPLY_FAIL");
		MODBUS_LOG(">\r\n");
#	endif
		
	return MODBUS_FAIL;
}

/**************************************************************************
/*!
	Send the data as Modbus TCP or RTU
*/	
/**************************************************************************/
void ModbusSend()
{
#	if(MODBUS_DEBUG)
		MODBUS_LOG("(MBUS)<SEND>");
		MODBUS_LOG(">\r\n");
#	endif		

#	if(MODBUS_RTU)
		// No action required
#	elif(MODBUS_TCP)
		// Swap again from little-endian to big-endian
		modbusreply->transactionid = HTONS(modbusframe->transactionid);
		modbusreply->protocolid = HTONS(modbusframe->protocolid);
		modbusreply->lenght = HTONS(modbusframe->lenght);	
#	endif

	// Include debug functionalities, if required
#	if(MODBUS_DEBUG)
		U8 i;
		
		MODBUS_LOG("(MBUS)<OUT><0x");
		
		// Print header
		MODBUS_LOG(Modbus_oFrame.header[0],HEX);
		for(i=1;i<Modbus_oFrame.header_len;i++)
		{
			MODBUS_LOG(",0x");
			MODBUS_LOG(Modbus_oFrame.header[i],HEX);			
		}
		
		// Print payload
		MODBUS_LOG(",0x");
		MODBUS_LOG(Modbus_oFrame.payload[0],HEX);
		for(i=1;i<Modbus_oFrame.payload_len;i++)
		{
			MODBUS_LOG(",0x");
			MODBUS_LOG(Modbus_oFrame.payload[i],HEX);			
		}		
		
		MODBUS_LOG(">\r\n");
#	endif

#if(MODBUS_RTU)

	// Send the frame
	while(oFrame_Available())
		Serial.write(oFrame_GetByte());			// Get the next byte
		
#elif(MODBUS_TCP && (ETH_W5100 || ETH_W5200 || ETH_W5500))
	
	// Send data
	srvcln_send((U8*)&Modbus_oFrame, 0);
	
#elif(MODBUS_TCP && (ETH_ENC28J60 || WIFI_MRF24))

	// Get the socket where data shall be sent
	sock = srvcln_getsocket(MODBUS_TCP_PORT);

	// If the frame is not empty, there are waiting data 	
	oFrame_Define(&vNetM1_oFrame);
	if(oFrame_Available())
		return ;			
		
	// Data via uIP need to be loaded into the vNet oFrame	
	oFrame_Set(0, 0, 0, 0, &Modbus_oFrame);
	
	/* Data are not really sent with this method, the outgoing data are placed into
		the dedicated buffer and a connection request to a peer is flagged. Later a
		callback function from uip_input() will act on the ENC28J60 to send data. */
	srv_send(sock, SENDDATA);
	
#endif

}

/**************************************************************************
/*!
	Send back the input data as echo in Modbus TCP or RTU
*/	
/**************************************************************************/
void ModbusEcho()
{
#	if(MODBUS_DEBUG)
		MODBUS_LOG("(MBUS)<ECHO>");
		MODBUS_LOG(">\r\n");
#	endif	

#if(MODBUS_RTU)

	// Swap again from little-endian to big-endian
	modbusframe->startingaddress = HTONS(modbusframe->startingaddress);
	modbusframe->registers = HTONS(modbusframe->registers);
	
#elif(MODBUS_TCP)

	// Swap again from little-endian to big-endian
	modbusframe->transactionid = HTONS(modbusframe->transactionid);
	modbusframe->protocolid = HTONS(modbusframe->protocolid);
	modbusframe->lenght = HTONS(modbusframe->lenght);
	modbusframe->startingaddress = HTONS(modbusframe->startingaddress);
	modbusframe->registers = HTONS(modbusframe->registers);

	// Define the Modbus output frame where data are stored
	oFrame_Define(&Modbus_oFrame);
	oFrame_Set(modbusin, 0, HTONS(modbusframe->lenght)+6, 0, 0);		// Leght doesn't contain the Modbus TCP transaction data	
#endif
	
	// Include debug functionalities, if required
#	if(MODBUS_DEBUG)
		U8 i;
		MODBUS_LOG("(MBUS)<OUT><0x");
		
		// Print header
		MODBUS_LOG(Modbus_oFrame.header[0],HEX);
		for(i=1;i<Modbus_oFrame.header_len;i++)
		{
			MODBUS_LOG(",0x");
			MODBUS_LOG(Modbus_oFrame.header[i],HEX);			
		}
		
		// Print payload
		MODBUS_LOG(",0x");
		MODBUS_LOG(Modbus_oFrame.payload[0],HEX);
		for(i=1;i<Modbus_oFrame.payload_len;i++)
		{
			MODBUS_LOG(",0x");
			MODBUS_LOG(Modbus_oFrame.payload[i],HEX);			
		}		
		
		MODBUS_LOG(">\r\n");
#	endif

#if(MODBUS_RTU)

	// Send the frame
	while(oFrame_Available())
		Serial.write(oFrame_GetByte());			// Get the next byte

#elif(MODBUS_TCP && (ETH_W5100 || ETH_W5200 || ETH_W5500))
	// Send data
	srvcln_send((U8*)&Modbus_oFrame, 0);
	
#elif(MODBUS_TCP && (ETH_ENC28J60 || WIFI_MRF24))

	// Get the socket where data shall be sent
	sock = srvcln_getsocket(MODBUS_TCP_PORT);

	// If the frame is not empty, there are waiting data 	
	oFrame_Define(&vNetM1_oFrame);
	if(oFrame_Available())
		return ;
	
	// Data via uIP need to be loaded into the vNet oFrame
	oFrame_Set(0, 0, 0, 0, &Modbus_oFrame);
	
	/* Data are not really sent with this method, the outgoing data are placed into
		the dedicated buffer and a connection request to a peer is flagged. Later a
		callback function from uip_input() will act on the ENC28J60 to send data. */
	srv_send(sock, SENDDATA);
	
#endif
}

/**************************************************************************
/*!
	Send an execption as Modbus TCP or RTU
*/	
/**************************************************************************/
U8 ModbusExecption()
{
#	if(MODBUS_DEBUG)
		MODBUS_LOG("(MBUS)<EXECPTION>");
		MODBUS_LOG(">\r\n");
#	endif		

#if(MODBUS_RTU)


#elif(MODBUS_TCP)

	// Swap again from little-endian to big-endian
	modbusframe->transactionid = HTONS(modbusframe->transactionid);
	modbusframe->protocolid = HTONS(modbusframe->protocolid);
	modbusframe->lenght = HTONS(0x03);									// Exceptions has fixed lenght

	// Define the Modbus output frame where data are stored
	oFrame_Define(&Modbus_oFrame);
	oFrame_Set(modbusin, 0, HTONS(modbusframe->lenght)+6, 0, 0);		// Leght doesn't contain the Modbus TCP transaction data	
#endif

	// Include debug functionalities, if required
#	if(MODBUS_DEBUG)
		U8 i;
		MODBUS_LOG("(MBUS)<OUT><0x");
		
		// Print header
		MODBUS_LOG(Modbus_oFrame.header[0],HEX);
		for(i=1;i<Modbus_oFrame.header_len;i++)
		{
			MODBUS_LOG(",0x");
			MODBUS_LOG(Modbus_oFrame.header[i],HEX);			
		}
		
		// Print payload
		MODBUS_LOG(",0x");
		MODBUS_LOG(Modbus_oFrame.payload[0],HEX);
		for(i=1;i<Modbus_oFrame.payload_len;i++)
		{
			MODBUS_LOG(",0x");
			MODBUS_LOG(Modbus_oFrame.payload[i],HEX);			
		}		
		
		MODBUS_LOG(">\r\n");
#	endif

#if(MODBUS_RTU)

	// Send the header as execption
	Serial.write(modbusin, MODBUS_RTU_REPLY_HEADER);

#elif(MODBUS_TCP && (ETH_W5100 || ETH_W5200 || ETH_W5500))

	// Send data
	srvcln_send(modbusin, MODBUS_TCP_REPLY_HEADER); 
	
	// Close the connection
	//srvcln_stop();
	
#elif(MODBUS_TCP && (ETH_ENC28J60 || WIFI_MRF24))
	
	// Load and send data
	srvcln_load(modbusin, MODBUS_TCP_REPLY_HEADER); 
	srv_send(sock, SENDDATA); 
	
#endif
}

/**************************************************************************
/*!
	Process the Modbus protocol (either TCP or RTU)
*/	
/**************************************************************************/
U8 Modbus(U8 *memory_map)
{		
	U8 parsed, fcode;

	// If there is any previous write to be processed, do it now
	if(modbustranfer_id)
	{
		// Data has been parsed at a later cycle, so just send the value
		Souliss_RemoteInput(*(U16*)(memory_map+MaCaco_ADDRESSES_s+2*modbustranfer_id), modbustranfer_slot, modbustranfer_var);	
			
		// Reset	
		modbustranfer_id=0;
		modbustranfer_slot=0;
		modbustranfer_var=0;
		
		// No new incoming data
		return MODBUS_FAIL;
	}
	
	// If an incoming data is available
	if(ModbusAvailable())
	{
		// Retrieve data
		if(ModbusRetrieve())
		{
			// Verify the frame
			parsed = ModbusVerify();

			// Send an answer or an execption
			if(parsed == MODBUS_SUCCESS)
			{
				fcode = ModbusReply(memory_map);
				
				// Build and send the reply frame
				if((fcode == MODBUS_READ_HOLDING_REGISTERS) || (fcode == MODBUS_READ_COIL))
					ModbusSend();
				else if ((fcode == MODBUS_WRITE_SINGLE_REGISTERS) || (fcode == MODBUS_WRITE_SINGLE_COIL))
					ModbusEcho();
				
			}
			else if(parsed == MODBUS_EXCEPTION)
				ModbusExecption();					// Send the execption frame
					
			return parsed;	
		}	
	}
	else
		return MODBUS_FAIL;
}