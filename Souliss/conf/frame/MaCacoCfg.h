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
			0x21	Read state request with subscription,
			0x31	Read state answer,
			0x22	Read typical logic request,
			0x32	Read typical logic answer,
			0x33	Force input values,
			0x34	Force input values by typical logic,	
			0x25	Nodes healthy request,
			0x35	Nodes healthy answer.
			0x26	Database structure request,
			0x36	Database structure answer.
			0x27	Read state request without subscription,
			0x37	Read state answer without subscription,
			0x28	Discover a gateway node request (broadcast),
			0x38	Discover a gateway node answer (broadcast),
			0x29	Dynamic addressing request (broadcast),
			0x39	Dynamic addressing answer (broadcast),
			0x2A	Subnet request (broadcast)
			0x3A    Subnet answer (broadcast)
			0x2B    Join a network gateway (broadcast)
			0x3C    Set an IP address at runtime (broadcast)
			
		Unstructured Functional Codes:
			0x71	Force input values by typical logic (broadcast or multicast),
			0x72	Send an Action Message (broadcast or multicast)
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
#define MaCaco_HEALTYREQ		0x25	// Nodes healthy request,
#define MaCaco_HEALTYANS		0x35	// Nodes healthy answer.
#define MaCaco_DBSTRUCTREQ		0x26	// Database structure request,
#define MaCaco_DBSTRUCTANS		0x36	// Database structure answer.
#define MaCaco_DATAREQ			0x27	// Read state request without subscription,
#define MaCaco_DATAANS			0x37	// Read state answer without subscription,
#define MaCaco_DISCOVERREQ		0x28	// Discover a gateway node request (broadcast)
#define MaCaco_DISCOVERANS		0x38	// Discover a gateway node answer (broadcast)
#define	MaCaco_DINADDRESSREQ	0x29	// Dynamic addressing request (broadcast)
#define	MaCaco_DINADDRESSANS	0x39	// Dynamic addressing answer (broadcast)
#define	MaCaco_SUBNETREQ		0x2A	// Subnet request (broadcast)
#define	MaCaco_SUBNETANS		0x3A	// Subnet answer (broadcast)
#define	MaCaco_JOINNETWORK		0x2B	// Join a network gateway (broadcast)
#define	MaCaco_SETIP			0x3C	// Set an IP address at runtime (broadcast)

#define	MaCaco_FORCETYP			0x71	// Force input values by typical logic (from a peer)
#define	MaCaco_ACTIONMSG		0x72	// Send an Action Message

#define MaCaco_FUNCODE_ERR 		0x00
#define MaCaco_FUNCODE_OK  		0x01

#define	MaCaco_FUNCODE_NO (17+20+2)

const int MaCaco_funcode[MaCaco_FUNCODE_NO] = {0x01, 0x11, 0x02, 0x12,
											   0x08, 0x18, 0x09, 0x19, 
											   0x05, 0x15,
											   0x13, 0x14, 0x16, 0x17,
											   0x83, 0x84, 0x85,
											   0x21, 0x31, 0x22, 0x32,
											   0x33, 0x34, 0x25, 0x35,
											   0x26, 0x36, 0x27, 0x37,
											   0x28, 0x38, 0x29, 0x39,
											   0x2A, 0x3A, 0x2B, 0x3C,
											   0x71, 0x72};
		  
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
		all nodes, but user interface based on MaCaco may accept larger frame.
		For this reason the maximum payload size is not taken in account while
		sending data, but only while receiving data.
		
		As a process protocol, the frame used to store MaCaco frames shall be
		able to contains also overhead from bottom level.
		
*/
/**************************************************************************/							 
#define MaCaco_HEADER	5
#define MaCaco_PAYLOAD	(VNET_MAX_PAYLOAD-MaCaco_HEADER)
#define MaCaco_FRAME	(VNET_MAX_FRAME)
		  
/**************************************************************************/
/*!
	SHARED MEMORY MAP,
		Is the whole data area that is used for the protocol handling and
		for data sharing. As option the User Mode can be enabled, this 
		reserve an area of the microcontroller RAM for data gathered from
		other nodes in the network.
		
		As User Mode a node can acts as Gateway and pass data from nodes in
		the network the the User Interface. Having a gateway remove from the
		User Interface the need to discover and handle the network, either
		makes easier IP forwarding.
		Data are not stored in the gateway and are moving on an event base,
		so the gateway cannot serve any polling-based protocol unless is 
		activated PERSISTANCE mode.		
		
		You are allowed to modify the number of MaCaco_NODES and MaCaco_SLOT
		according to the below table, the values cannot be exceeded, but all 
		configuration with lower values are valid.
				
		The maximum number of nodes and slots depends on configuration as shown
		in below table:
		
					Nodes		Slots									
					30			35
					45			24			(Default)
					70			4
			
			Due to the maximum addressing space (1 byte) the
			following constrain shall be verified:
			
			1) MaCaco_SLOT < VNET_MAX_PAYLOAD - MaCaco_HEADER
			2) MaCaco_NODES < 255
			3) MaCaco_SLOT  < 255 / 3
			
			You can modify VNET_MAX_PAYLOAD in order to suit your needs, rather
			MaCaco_HEADER shall not be modified.
			
			The MaCaco protocol use (3*MaCaco_NODES + 4*MaCaco_SLOT + MaCaco_QUEUELEN)
			bytes of RAM in case of MaCaco_SUBSCRIBERS selected, if not the 3*MaCaco_NODES
			quantity doesn't apply.
			
		As general requirements, all nodes shall have the same type of configuration
		and size for the shared memory map, this regardless their action as standard
		or gateway node.
		
		As option you can activate PERSISTANCE or LASTIN mode, the former stores all
		incoming data and use a quite high amount of RAM; the latter stores only the
		last incoming data.
		
		Value       Status
        0x0         Disable (Default)
        0x1         Enable
				
*/
/**************************************************************************/							 
#if(!(QC_ENABLE))																// Define manually only in Detailed Configuration Mode
#	define MaCaco_USERMODE		1												// User Mode
#	define MaCaco_SUBSCRIBERS	1												// Enable subscribing to other nodes
#	define MaCaco_PERSISTANCE	0												// Data Persistence (increase RAM usage)
#	define MaCaco_LASTIN		0												// Data Persistence for last incoming values
#endif

#define MaCaco_LOCNODE			0												// Node number for local data (cannot be changed)
#define	MaCaco_QUEUELEN			15												// Define the number of configuration parameters

#if(!defined(NODESIZE_INSKETCH))
#define MaCaco_NODES			45												// Number of remote nodes
#define MaCaco_SLOT				24												// Number of slot
#endif

#define MaCaco_SUBSCRLEN		MaCaco_SLOT										// Length  byte for subscription data
#define MaCaco_TYPLENGHT		MaCaco_SLOT										// Number of slot
#define MaCaco_OUTLENGHT		MaCaco_SLOT										// Number of slot

#define MaCaco_QUEUE_s			0												// First byte of the configuration parameters
#define MaCaco_QUEUE_f			(MaCaco_QUEUE_s+MaCaco_QUEUELEN-1)				// Last  byte of the configuration parameters
#define MaCaco_AUXIN_s			(MaCaco_QUEUE_f+1)								// First byte of the auxiliary inputs
#define MaCaco_AUXIN_f			(MaCaco_AUXIN_s+MaCaco_SLOT-1)					// Last  byte of the auxiliary inputs
#define MaCaco_IN_s				(MaCaco_AUXIN_f+1)								// First byte for input data
#define MaCaco_IN_f				(MaCaco_IN_s+MaCaco_SLOT-1)						// Last  byte for input data
#define MaCaco_TYP_s			(MaCaco_IN_f+1)									// First byte for typical logic definitions
#define MaCaco_TYP_f			(MaCaco_TYP_s+MaCaco_SLOT-1)					// Last  byte for typical logic definitions
#define MaCaco_OUT_s			(MaCaco_TYP_f+1)								// First byte for output data
#define MaCaco_OUT_f			(MaCaco_OUT_s+MaCaco_SLOT-1)					// Last  byte for output data

#define MaCaco_ADDRESSES_s		(MaCaco_OUT_f+1)								// First byte of the addresses for the remote nodes
#define MaCaco_ADDRESSES_f		(MaCaco_ADDRESSES_s+ 2*MaCaco_NODES-1)			// Last  byte of the addresses for the remote nodes
#define MaCaco_HEALTY_s			(MaCaco_ADDRESSES_f+1)							// First byte of the healthy for the remote nodes
#define MaCaco_HEALTY_f			(MaCaco_HEALTY_s+MaCaco_NODES-1)				// Last  byte of the healthy for the remote nodes

#define MaCaco_WRITE_s			(MaCaco_AUXIN_s)								// First writeable data by a remote device
#define MaCaco_WRITE_f			(MaCaco_IN_f)									// Last  writeable data by a remote device

#define	MaCaco_P_TYP_s			(MaCaco_HEALTY_f+1)								// First byte for typical logic definitions in PERSISTANCE mode
#define MaCaco_P_TYP_f			(MaCaco_P_TYP_s+(MaCaco_NODES*MaCaco_SLOT))		// Last byte for typical logic definitions in PERSISTANCE mode
#define	MaCaco_P_OUT_s			(MaCaco_P_TYP_f+1)								// First byte for output data in PERSISTANCE mode
#define MaCaco_P_OUT_f			(MaCaco_P_OUT_s+(MaCaco_NODES*MaCaco_SLOT))		// Last byte for output data in PERSISTANCE mode

#define	MaCaco_L_BUFSIZE		3												// Number of nodes stored in the buffer for LASTIN mode
#define	MaCaco_L_IDX_NULL		255												// Null value for the index in LASTIN mode									
#define MaCaco_L_TYP5n_s		(MaCaco_HEALTY_f+1)								// First byte for last incoming data in case of LASTIN mode
#define MaCaco_L_TYP5n_f		(MaCaco_L_TYP5n_s+(2*MaCaco_NODES))				// First byte for last incoming data in case of LASTIN mode
#define	MaCaco_L_IDX_s			(MaCaco_L_TYP5n_f+1)							// First byte for node index in case of LASTIN mode
#define	MaCaco_L_IDX_f			(MaCaco_L_IDX_s+MaCaco_L_BUFSIZE)				// Last byte for node index in case of LASTIN mode
#define	MaCaco_L_OUT_s			(MaCaco_L_IDX_f+1)								// First byte for the output data in LASTIN mode 
#define	MaCaco_L_OUT_f			(MaCaco_L_OUT_s+(MaCaco_L_BUFSIZE*MaCaco_SLOT))	// Last byte for the output data in LASTIN mode

#define MaCaco_T_MEMMAP			(MaCaco_OUT_f+1)								// Lenght of the whole memory map
#define MaCaco_S_MEMMAP			(MaCaco_HEALTY_f+1)								// Lenght of the whole memory map in case of Subscribers nodes
#define MaCaco_P_MEMMAP			(MaCaco_P_OUT_f+1)								// Lenght of the whole memory map in case of PERSISTANCE mode
#define	MaCaco_L_MEMMAP			(MaCaco_L_OUT_f+1)								// Lenght of the whole memory map in case of LASTIN mode

#if(MaCaco_PERSISTANCE)
#	define	MaCaco_G_TYP_s		MaCaco_P_TYP_s									// Pointer for gateway data in case of PERSISTANCE mode
#	define	MaCaco_G_TYP_f		MaCaco_P_TYP_f
#	define	MaCaco_G_OUT_s		MaCaco_P_OUT_s                                  // Pointer for gateway data in case of PERSISTANCE mode
#	define	MaCaco_G_OUT_f		MaCaco_P_OUT_f
#	define	MaCaco_MEMMAP		(MaCaco_P_MEMMAP+1)								// Lenght
#elif(MaCaco_LASTIN)
#	define	MaCaco_G_OUT_s		MaCaco_L_OUT_s									// Pointer for gateway data in case of LASTIN mode
#	define	MaCaco_MEMMAP		(MaCaco_L_MEMMAP+1)								// Lenght
#elif(MaCaco_SUBSCRIBERS)
#	define	MaCaco_G_TYP_s		MaCaco_TYP_s									// Pointer for gateway data
#	define	MaCaco_G_TYP_f		MaCaco_TYP_f
#	define	MaCaco_G_OUT_s		MaCaco_OUT_s									// Pointer for gateway data
#	define	MaCaco_G_OUT_f		MaCaco_OUT_f
#	define	MaCaco_MEMMAP		(MaCaco_S_MEMMAP+1)								// Lenght
#else
#	define	MaCaco_G_TYP_s		MaCaco_TYP_s									// Pointer for gateway data
#	define	MaCaco_G_TYP_f		MaCaco_TYP_f
#	define	MaCaco_G_OUT_s		MaCaco_OUT_s									// Pointer for gateway data
#	define	MaCaco_G_OUT_f		MaCaco_OUT_f
#	define	MaCaco_MEMMAP		(MaCaco_T_MEMMAP+1)								// Lenght
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
#define MaCaco_INMAXSUBSCR			10
#define MaCaco_OUTMAXSUBSCR			MaCaco_NODES

#define MaCaco_NODATARECEIVED		0x00
#define MaCaco_DATARECEIVED			0x01
#define MaCaco_NODATACHANGED		0x00
#define MaCaco_DATACHANGED			0x01

#define MaCaco_SUBSDECREASE			0x0C
#define MaCaco_SUBSINCREASE			0x50

#define MaCaco_NOSUBSCRANSWER		0x00
#define MaCaco_SUBSCRANSWER			0x01
#define MaCaco_SUBSCRHEALTY			0x01
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
#ifndef	MaCaco_DEBUG_INSKETCH
#	define MaCaco_DEBUG  		0
#endif

#endif