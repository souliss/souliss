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

#include "Souliss.h"
#include "frame/MaCaco/MaCaco.h"
#include "frame/vNet/vNet.h"

#include "Modbus.h"

#if(MaCaco_PASSTHROUGH)
#	waring	Modbus cannot work with Passthough mode in MaCaco, remove it
#endif

U8 l=0, sock;
U8 modbusin[MODBUS_MAXFRAME];						// Data buffer for incoming data
U8 modbusdata[MODBUS_OUTFRAME];						// Data buffer for output data 
oFrame Modbus_Additional_oFrame;							// Modbus output frame for data relevant the local node
oFrame Modbus_oFrame;						// Modbus output frame for data relevant the remote nodes

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
U8 ModbusInit()
{
#if(MODBUS_RTU)
	// The Serial class and the USART interrupt are automatically set by the
	// Arduino IDE, so isn't impossible use a dedicated code for serial data.
	Serial.begin(MODBUS_RTU_BAUD);
#elif(MODBUS_TCP && ETH_W5100)
	// Set a listening Modbus port
	srv_listen(MODBUS_TCP_PORT); 
#elif(MODBUS_TCP && (ETH_ENC28J60 || WIFI_MRF24))
	// Set a listening Modbus port
	srv_listen(MODBUS_TCP_PORT); 
#endif	
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
	
#elif(MODBUS_TCP && ETH_W5100)

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
#elif(MODBUS_TCP && ETH_W5100)

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
				if((bytenumber + byteqty + MaCaco_IN_s) > MaCaco_TYP_f)
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
				if((bytenumber + byteqty + MaCaco_TYP_s) > MaCaco_TYP_f)
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
				if((bytenumber + byteqty + MaCaco_OUT_s) > MaCaco_OUT_f)
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
				if((bytenumber + byteqty + MaCaco_IN_s) > MaCaco_IN_f)
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
				if((bytenumber + byteqty + MaCaco_TYP_s) > MaCaco_TYP_f)
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
				if((bytenumber + byteqty + MaCaco_OUT_s) > MaCaco_OUT_f)
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
	U8 memory_map_address, byteqty, bytenumber;	
			
	// Parse the functional code
	if((modbusframe->functionalcode == MODBUS_READ_COIL) || (modbusframe->functionalcode == MODBUS_READ_DISCRETE_INPUTS))
	{	
		// Map the the Modbus registers with the internal memory
		if((modbusframe->startingaddress >= COILS_INPUT_START_ADDRESS) && (modbusframe->startingaddress <= COILS_INPUT_END_ADDRESS))
		{
			// Start from inputs
			memory_map_address = MaCaco_IN_s;
			
			// Calculate the bit number into the memory_map
			bytenumber  = (modbusframe->startingaddress - COILS_INPUT_START_ADDRESS)/8;			
		}
		else if((modbusframe->startingaddress >= COILS_TYP_START_ADDRESS) && (modbusframe->startingaddress <= COILS_TYP_END_ADDRESS))
		{
			// Start from typicals
			memory_map_address = MaCaco_TYP_s;

			// Calculate the bit number into the memory_map			bytenumber  = (modbusframe->startingaddress - COILS_TYP_START_ADDRESS)/8;				
		}
		else if((modbusframe->startingaddress >= COILS_OUTPUT_START_ADDRESS) && (modbusframe->startingaddress <= COILS_OUTPUT_END_ADDRESS))
		{
			// Start from outputs
			memory_map_address = MaCaco_OUT_s;
			
			// Calculate the bit number into the memory_map			bytenumber  = (modbusframe->startingaddress - COILS_OUTPUT_START_ADDRESS)/8;				
		}

		if(modbusframe->registers%8)
			byteqty = 1 + modbusframe->registers / 8;
		else
			byteqty = modbusframe->registers / 8;		
	
		// Build a frame, only the pointer are stored, so it can be changed
		// also after the output frame is build
		#if(MODBUS_RTU)
			oFrame_Define(&Modbus_CRC);
			oFrame_Set(0, modbuscrc, 0, 2, 0);

			// Indentify if requested data are local or remote
			if((bytenumber + byteqty < MaCaco_SLOT))
			{
				// Requested data are all contained into the local node
				oFrame_Define(&Modbus_oFrame);
				oFrame_Set(0, (memory_map+memory_map_address+bytenumber), 0, byteqty, &Modbus_CRC);
			}
			else if (bytenumber < MaCaco_SLOT)
			{
				// Requested data are contained either in local and remote nodes
						
				// Append an oFrame for data from remote nodes
				oFrame_Define(&Modbus_Additional_oFrame);
				oFrame_Set(0, (memory_map+memory_map_address+bytenumber+(MaCacoUserMode_IN_s-MaCaco_IN_s)), 0, byteqty, &Modbus_CRC);				
				
				// Build an oFrame for data from local nodes
				oFrame_Define(&Modbus_oFrame);
				oFrame_Set(modbusin, (memory_map+memory_map_address+bytenumber), MODBUS_REPLY_HEADER, byteqty, &Modbus_Additional_oFrame);
			}
			else
			{
				// Requested data are all contained into the remote nodes
				oFrame_Define(&Modbus_oFrame);
				oFrame_Set(0, (memory_map+memory_map_address+bytenumber), 0, byteqty, &Modbus_CRC);				
			}
		#elif(MODBUS_TCP)
			// Indentify if requested data are local or remote
			if((bytenumber + byteqty < MaCaco_SLOT))
			{
				// Requested data are all contained into the local node
				oFrame_Define(&Modbus_oFrame);
				oFrame_Set(0, (memory_map+memory_map_address+bytenumber), 0, byteqty, 0);
			}
			else if (bytenumber < MaCaco_SLOT)
			{
				// Requested data are contained either in local and remote nodes
						
				// Append an oFrame for data from remote nodes
				oFrame_Define(&Modbus_Additional_oFrame);
				oFrame_Set(0, (memory_map+memory_map_address+bytenumber+(MaCacoUserMode_IN_s-MaCaco_IN_s)), 0, byteqty - (MaCaco_SLOT - bytenumber), 0);				
				
				// Build an oFrame for data from local nodes
				oFrame_Define(&Modbus_oFrame);
				oFrame_Set(modbusin, (memory_map+memory_map_address+bytenumber), MODBUS_REPLY_HEADER, (MaCaco_SLOT - bytenumber), &Modbus_Additional_oFrame);
			}
			else
			{
				// Requested data are all contained into the remote nodes
				oFrame_Define(&Modbus_oFrame);
				oFrame_Set(0, (memory_map+memory_map_address+bytenumber), 0, byteqty, 0);				
			}		
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
		{
			// Calculate the byte number into the memory_map		
			bytenumber = (modbusframe->startingaddress - REGISTER_INPUT_START_ADDRESS);
			
			// Start from inputs
			memory_map_address = MaCaco_IN_s + bytenumber;
		}
		else if((modbusframe->startingaddress >= REGISTER_TYP_START_ADDRESS) && (modbusframe->startingaddress <= REGISTER_TYP_END_ADDRESS))
		{
			// Calculate the byte number into the memory_map			
			bytenumber = (modbusframe->startingaddress - REGISTER_TYP_START_ADDRESS);
			
			// Start from typicals
			memory_map_address = MaCaco_TYP_s + bytenumber;
		}
		else if((modbusframe->startingaddress >= REGISTER_OUTPUT_START_ADDRESS) && (modbusframe->startingaddress <= REGISTER_OUTPUT_END_ADDRESS))
		{
			// Calculate the byte number into the memory_map			
			bytenumber = (modbusframe->startingaddress - REGISTER_OUTPUT_START_ADDRESS);
			
			// Start from outputs
			memory_map_address = MaCaco_OUT_s + bytenumber;
		}
		
		// Modbus data are 2 bytes, rather MaCaco data is 1 byte. So is mandatory
		// load the MaCaco data into a buffer, copying them as 2 bytes data.
		
		for(U8 i=0;i<(modbusframe->registers);i++)
			if(bytenumber + i < MaCaco_SLOT)	// Data relevant the local node
				*(U16 *)(&modbusdata[2*i]) = HTONS((U16)(*(memory_map+memory_map_address+i)));
			else								// Data relevant the remote node
				*(U16 *)(&modbusdata[2*i]) = HTONS((U16)(*(memory_map+memory_map_address+i+(MaCacoUserMode_IN_s-MaCaco_IN_s))));
				
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
		// Calculate the address
		memory_map_address = MaCaco_IN_s + (modbusframe->startingaddress - REGISTER_INPUT_START_ADDRESS);

		// Just copy the data, the answer is an echo
		memory_map[memory_map_address] = (U8)(modbusframe->registers);
		
		// The answer is just an echo of the incoming message
		#if(MODBUS_RTU)
			oFrame_Define(&Modbus_oFrame);
			oFrame_Set(modbusin, 0, MODBUS_RTU_ANSWER_HEADER+2*(modbusframe->registers)+2, 0, 0);
		#elif(MODBUS_TCP)
			oFrame_Define(&Modbus_oFrame);
			oFrame_Set(modbusin, 0, MODBUS_RTU_ANSWER_HEADER+2*(modbusframe->registers), 0, 0);		
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
		
	
		if(modbusframe->registers)			// In this field is contained a GTZ value if ON else OFF
			memory_map[MaCaco_IN_s+bytenumber] |= 1 << bitnumber;
		else
			memory_map[MaCaco_IN_s+bytenumber] &= ~(1 << bitnumber);

		// The answer is just an echo of the incoming message
		#if(MODBUS_RTU)
			oFrame_Define(&Modbus_oFrame);
			oFrame_Set(modbusin, 0, MODBUS_RTU_ANSWER_HEADER+bytenumber+2, 0, 0);
		#elif(MODBUS_TCP)
			oFrame_Define(&Modbus_oFrame);
			oFrame_Set(modbusin, 0, MODBUS_RTU_ANSWER_HEADER+bytenumber, 0, 0);	
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
		
#elif(MODBUS_TCP && ETH_W5100)
	
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

#elif(MODBUS_TCP && ETH_W5100)
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

#elif(MODBUS_TCP && ETH_W5100)

	// Send data
	srvcln_send(modbusin, MODBUS_TCP_REPLY_HEADER); 
	
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

/**************************************************************************
/*!
	Translate a Modbus write request in a vNet frame for a remote node, this
	function is required for the handling of write requests that are not
	relevant the node it-self.
	
	In case of Modbus, after a write request an answer to the Modbus Master is
	required, so in the same cycle the write request cannot be forwarded to
	a remote node. The request is so loaded into the memory map and then parsed
	with this function.
	
	In case of MaCaco, a command that is not relevant to the node it-self is 
	redirected without be loaded into the memory map, because MaCaco doesn't
	need a confirmation message for commands.
*/	
/**************************************************************************/
void ModbusRemoteInput(U8 *memory_map)
{
	U16 i;

	// Go throught all the inputs that are not relevant the local node
	for(i=MaCacoUserMode_IN_s;i<MaCacoUserMode_IN_f;i++)
	{
		// Look in the input area for a waiting command
		if(*(memory_map+i))
		{
			// Calculate the id and slot for the remote node
			U8 id = (i-MaCaco_IN_s)/MaCaco_SLOT; 
			U8 slot = (i-MaCaco_IN_s)-id*MaCaco_SLOT;
			
			// Send the remote command
			Souliss_RemoteInput(*(U16*)(memory_map+MaCaco_ADDRESSES_s+2*id), slot, *(memory_map+i));		

			// Reset the input area
			*(memory_map+i)=0;
			
			// No more than one command parsed at time
			break;
		}
	}	
}