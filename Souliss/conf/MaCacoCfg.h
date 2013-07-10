/**************************************************************************
	Souliss - MaCaco Communication Protocol
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
/**************************************************************************/
#ifndef MACACO_PROTOCOL_DEF_H
#define MACACO_PROTOCOL_DEF_H

/**************************************************************************/
/*!
				
*/
/**************************************************************************/	
#define MaCaco_PORT				23
#define Macaco_MAXRETRY			3
/**************************************************************************/
/*!
	FUNCTIONAL CODES,
		Define the type and direction of datas between peers, user can 
		add custom functional code to suite additional needs.
		
		Custom functional code can fit in ranges:
			0x41 - 0x49 Group 1 Request
			0x51 - 0x59 Group 1 Answer
			0x61 - 0x69 Group 2 Request
			0x71 - 0x79 Group 2 Answer
			
			At each request functional code, should be associated an answer
			that share the last four bit of the functional code.
			E.g.:	For custom request 0x43, the associated custom answer should
					be 0x53.
		
		Standard Functional Codes:
			0x01	Read request for digital values,
			0x11	Read answer for digital values,
			0x02	Read request for analog values,
			0x12	Read answer for analog values,
			0x05	Subscription request,
			0x15	Subscription answer,
			0x13	Force back a register value,
			0x14	Force a register value,
			0x16	Force a register value (bit-wise AND),
			0x17	Force a register value (bit-wise OR),
			0x08	Ping request,
			0x18	Ping answer,
			0x09	Trace root request,
			0x19	Trace root answer,
			0x83	Functional code not supported,
			0x84	Data out of range,
			0x85	Subscription refused.

		Gateway Functional Codes:
			0x21	Read state request (subscription),
			0x31	Read state answer  (subscription),
			0x22	Read state request (subscription),
			0x32	Read state answer  (subscription),			
			0x33	Force input values request,
			0x34	Force input values answer,
			0x25	Force input values by typical logic request,
			0x35	Force input values by typical logic answer.	
			
*/
/**************************************************************************/
#define MaCaco_READREQDIG		0x01	// Read request for digital values,
#define MaCaco_READANSDIG		0x11	// Read answer  for digital values,
#define MaCaco_READREQANA		0x02	// Read request for analog values,
#define MaCaco_READANSANA		0x12	// Read answer for analog values,
#define MaCaco_SUBSCRREQ		0x05	// Subscription request,
#define MaCaco_SUBSCRANS		0x15	// Subscription answer,
#define MaCaco_FORCEBACK		0x13	// Force back a register value,
#define MaCaco_FORCEREGSTR		0x14	// Force register value,
#define MaCaco_FORCEREGAND		0x16	// Force register value (bit-wise AND),
#define MaCaco_FORCEREGOR		0x17	// Force register value (bit-wise OR),
#define MaCaco_PINGREQ			0x08	// Ping request,
#define MaCaco_PINGANS			0x18	// Ping answer,
#define MaCaco_TRACEREQ			0x09	// Trace root request (not implemented),
#define MaCaco_TRACEANS			0x19	// Trace root answer  (not implemented),
#define MaCaco_ERR83			0x83	// Functional code not supported,
#define MaCaco_ERR84			0x84	// Data out of range,
#define MaCaco_ERR85			0x85	// Subscription refused

#define MaCaco_STATEREQ			0x21	// Read state request with subscription,
#define MaCaco_STATEANS			0x31	// Read state answer,
#define MaCaco_TYPREQ			0x22	// Read typical logic request,
#define MaCaco_TYPANS			0x32	// Read typical logic answer,
#define MaCaco_FORCE			0x33	// Force input values,
#define MaCaco_TYP				0x34	// Force input values by typical logic,	
#define MaCaco_HEALTYREQ		0x25	// Nodes healty request,
#define MaCaco_HEALTYANS		0x35	// Nodes healty answer.
#define MaCaco_DBSTRUCTREQ		0x26	// Database structure request,
#define MaCaco_DBSTRUCTANS		0x36	// Database structure answer.
#define MaCaco_DATAREQ			0x27	// Read state request without subscription,
#define MaCaco_DATAANS			0x37	// Read state answer without subscription,

#define MaCaco_FUNCODE_ERR 		0x00
#define MaCaco_FUNCODE_OK  		0x01

#define	MaCaco_FUNCODE_NO (17+12)

const int MaCaco_funcode[MaCaco_FUNCODE_NO] = {0x01, 0x11, 0x02, 0x12,
											   0x08, 0x18, 0x09, 0x19, 
											   0x05, 0x15,
											   0x13, 0x14, 0x16, 0x17,
											   0x83, 0x84, 0x85,
											   0x21, 0x31, 0x22, 0x32,
											   0x33, 0x34, 0x25, 0x35,
											   0x26, 0x36, 0x27, 0x37};
		  
/**************************************************************************/
/*!
	FRAME HEADER LENGHT,
		The frame header of the protocol is defined as follow,	
			Functional Code, 1 byte
			Putin,			 2 byte
			Start offset,	 1 byte		
			Number of,		 1 byte
		
		Functional Code, is the code used to identify the type of request,
		Putin, is the address of the register where data should be placed,
		Start offset, is the offset of the first requested byte,
		Number of, is the number of requested bytes.
		
	PAYLOAD
		The payload value is used to size the incoming data buffer and shall
		be equal or less than the payload accepted by the communication 
		driver.
		
		Generally size of input and output message shall be consistent over
		all nodes, but some dedicated nodes may accept bigger payload for
		incoming data, this is the case of the user interfaces based on MaCaco.
		For this reason the payload size is not taken in account while sending
		data, but only while receiving data.
		
*/
/**************************************************************************/							 
#define MaCaco_HEADER	5
#define MaCaco_PAYLOAD	50
		  
/**************************************************************************/
/*!
	SHARED MEMORY MAP,
		Is the whole data area that is used for the protocol handling and
		for data sharing. If extended it contains data of the node itself 
		and from remote nodes over the network, otherwise contains only the
		data for the devices connected to the board.
		
		Value       Status
        0x0         Disable (Default)
        0x1         Enable
				
*/
/**************************************************************************/							 
#define MaCaco_EXTENDED_MAP		1

#define	MaCaco_NODES			10												// Number of remote nodes
#define MaCaco_LOCNODE			0												// Node number for local data
#define	MaCaco_CONFPARAM		5												// Define the number of configuration parameters
#define MaCaco_SLOT				8												// Number of slot
#define MaCaco_SUBSCRLEN		MaCaco_SLOT										// Lenght  byte for subscription data
#define MaCaco_TYPLENGHT		MaCaco_SLOT										// Number of slot
#define MaCaco_OUTLENGHT		MaCaco_SLOT										// Number of slot

#define MaCaco_ADDRESSES_s		0												// First byte of the addresses for the remote nodes
#define MaCaco_ADDRESSES_f		(2*MaCaco_NODES-1)								// Last byte of the addresses for the remote nodes
#define MaCaco_HEALTY_s			(MaCaco_ADDRESSES_f+1)							// First byte of the healty for the remote nodes
#define MaCaco_HEALTY_f			(MaCaco_HEALTY_s+MaCaco_NODES-1)				// Last byte of the healty for the remote nodes
#define MaCaco_CONF_s			(MaCaco_HEALTY_f+1)								// First byte of the configuration parameters
#define MaCaco_CONF_f			(MaCaco_CONF_s+MaCaco_CONFPARAM-1)				// Last byte of the configuration parameters
#define MaCaco_AUXIN_s			(MaCaco_CONF_f+1)								// First byte of the auxiliary inputs
#define MaCaco_AUXIN_f			(MaCaco_AUXIN_s+MaCaco_SLOT-1)					// Last byte of the auxiliary inputs

#if(MaCaco_EXTENDED_MAP)
	#define MaCaco_IN_s			(MaCaco_AUXIN_f+1)								// First byte for input data
	#define MaCaco_IN_f			(MaCaco_IN_s+(MaCaco_NODES*MaCaco_SLOT)-1)		// Last  byte for input data
	#define MaCaco_TYP_s		(MaCaco_IN_f+1)									// First byte for typical logic definitions
	#define MaCaco_TYP_f		(MaCaco_TYP_s+(MaCaco_NODES*MaCaco_SLOT)-1)		// Last  byte for typical logic definitions
	#define MaCaco_OUT_s		(MaCaco_TYP_f+1)								// First byte for output data
	#define MaCaco_OUT_f		(MaCaco_OUT_s+(MaCaco_NODES*MaCaco_SLOT)-1)		// Last  byte for output data
#else
	#define MaCaco_IN_s			(MaCaco_AUXIN_f+1)								// First byte for input data
	#define MaCaco_IN_f			(MaCaco_IN_s+MaCaco_SLOT-1)						// Last  byte for input data
	#define MaCaco_TYP_s		(MaCaco_IN_f+1)									// First byte for typical logic definitions
	#define MaCaco_TYP_f		(MaCaco_TYP_s+MaCaco_SLOT-1)					// Last  byte for typical logic definitions
	#define MaCaco_OUT_s		(MaCaco_TYP_f+1)								// First byte for output data
	#define MaCaco_OUT_f		(MaCaco_OUT_s+MaCaco_SLOT-1)					// Last  byte for output data
#endif

#define MaCaco_WRITE_s			(MaCaco_AUXIN_s)								// First writeble data by a remote device
#define MaCaco_WRITE_f			(MaCaco_IN_f)									// Last  writeble data by a remote device
#define MaCaco_EXT_WRITE_s		(MaCaco_TYP_s+1)								// First writeble data by a remote device in case of extended memory map
#define MaCaco_EXT_WRITE_f		(MaCaco_OUT_f)									// Last  writeble data by a remote device in case of extended memory map

#if(MaCaco_EXTENDED_MAP)
	#define MaCaco_MEMMAP		(MaCaco_OUT_f+1)								// Lenght of the whole memory map
#else
	#define MaCaco_MEMMAP		(MaCaco_EXT_WRITE_f+1)							// Lenght of the whole extended memory map
#endif

/**************************************************************************/
/*!
	SUBSCRIBE MODE,
		Devices allow subscription mode in order to share data only when a
		change occour and not at every CPU cycle.
		
		The following parameters allow tuning for subscription healty status
		 
		Meaning of protocol stack is a bit different when data are received
		as subscription of one or more devices, beucase the subscribed area 
		is a fixed subarea or the shared memory map, defined via SUBSCRINDX
		and SUBSCRLEN.
		
		The stack startoffset has no longer meaning, so is used to indetify
		the subscription channel of subscribed device.
		Each device can allow up to MaCaco_MAXSUBSCR different subscriptor 
		devices.
*/
/**************************************************************************/							 
#define MaCaco_MAXSUBSCR			10

#define MaCaco_NODATARECEIVED		0x00
#define MaCaco_DATARECEIVED			0x01
#define MaCaco_NODATACHANGED		0x00
#define MaCaco_DATACHANGED			0x01

#define MaCaco_SUBSDECREASE			0x01
#define MaCaco_SUBSINCREASE			0x10

#define MaCaco_NOSUBSCRANSWER		0x00
#define MaCaco_SUBSCRANSWER			0x01
#define MaCaco_SUBSCRHEALTY			0x10
#define MaCaco_SUBINITHEALTY		0x25
#define MaCaco_SUBMAXHEALTY			0xFF

/**************************************************************************/
/*!
	If enabled print the header incoming and outgoing MaCaco frames
	
        Value       Status
        0x0         Disable (Default)
        0x1         Enable
*/
/**************************************************************************/
#define MaCaco_DEBUG  		0

#endif



