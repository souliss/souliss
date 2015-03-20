/**************************************************************************
	Souliss - vNet Virtualized Network
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
#include <stdio.h>
#include <string.h>

#include "vNet.h"
#include "vNet_Internal.h"

// Frame structure code
#include "tools/oFrame.c"

#if (VNET_MEDIA1_ENABLE)
	// Driver for Wiznet W5100 / W5200
	#if (ETH_W5100 || ETH_W5200 || ETH_W5500)
		#include "drivers/ethW5x00/vNetDriver_eth.cpp"	
	#endif
	
	// Driver for Microchip EN28J60
	#if (ETH_ENC28J60)
		#include "drivers/ethENC28J60/vNetDriver_eth.cpp"	
	#endif

	// Driver for Microchip MRF2WB0MA
	#if (WIFI_MRF24)
		#include "drivers/ethMRF24/vNetDriver_eth.cpp"	
	#endif	

	// Driver for MAuthometion HF-LPT200
	#if (WIFI_LPT200)
		#include "drivers/ethLPT200/vNetDriver_eth.cpp"	
	#endif		
#endif
	
#if (VNET_MEDIA2_ENABLE)
	#if(CHIBI_AT86RF230)
		#include "drivers/chibi/vNetDriver_chibi.c"	
	#endif	
	
	#if(NRF24)
		#include "drivers/nRF24/vNetDriver_nrf24.cpp"
	#endif	
	
	#if(HOPERF_RFM69)
		#include "drivers/RFM69/vNetDriver_rfm69.cpp"
	#endif		
#endif
	
#if (VNET_MEDIA3_ENABLE)
	// Driver for Wiznet W5100 / W5200 / W5500 (broadcast only)
	#if ((ETH_W5100 || ETH_W5200 || ETH_W5500) && !VNET_MEDIA1_ENABLE)
		#include "drivers/ethW5x00/vNetDriver_eth.cpp"	
		#include "drivers/ethW5x00/vNetDriver_brd.h"
	#else
		#include "drivers/ethW5x00/vNetDriver_brd.h"
	#endif
	
	// Driver for Microchip EN28J60 (broadcast only)
	#if ((ETH_ENC28J60) && (!VNET_MEDIA1_ENABLE))
		#include "drivers/ethENC28J60/vNetDriver_eth.cpp"
		#include "drivers/ethENC28J60/vNetDriver_brd.h"	
	#else
		#include "drivers/ethENC28J60/vNetDriver_brd.h"		
	#endif	
	
#endif
	
#if (VNET_MEDIA5_ENABLE)
	#include "drivers/usart/vNetDriver_usart.cpp"
#endif	

#if (VNET_DEBUG)
	#define VNET_LOG Serial.print	
#endif

// Routing and bridging tables
static U16 route_table[VNET_ROUTING_TABLE] 		 = {0x0000};
static U16 dest_route_table[VNET_ROUTING_TABLE]  = {0x0000};
static U16 multicast_groups[VNET_MULTICAST_SIZE] = {0x0000};

static U8 last_media = 0;
static U32 resettime = 0;

U8 vNet_header[VNET_HEADER_SIZE] = {0x00};						// Header for output frame
oFrame vNet_oFrame;												// Data structure for output frame
oFrame message;													// Temp data structure

// Structure array for frame handling
static vNet vNet_Media[VNET_MEDIA_NUMBER];
static vNet_Data vNet_Media_Data[VNET_MEDIA_NUMBER];

/**************************************************************************/
/*!
    Init the Virtual Network
*/
/**************************************************************************/
void vNet_Init()
{
	U8 i, media = 0;
	
	// Set to zero
	for(media=1;media<=VNET_MEDIA_NUMBER;media++)
	{
			vNet_Media[media-1].src_addr= 0x0000; 
			vNet_Media[media-1].subnetmask  = 0x0000;
			vNet_Media[media-1].mySuperNode =  0x0000;
			
			vNet_Media_Data[media-1].data_available = 0x00;
			vNet_Media_Data[media-1].len  = 0x00;
			vNet_Media_Data[media-1].port = 0x00;
			vNet_Media_Data[media-1].o_src_addr = 0x0000;
			vNet_Media_Data[media-1].f_dest_addr =  0x0000;
			vNet_Media_Data[media-1].data	=  0x0000;		 
	}
	
	// Set to zero
	for(i=0;i<VNET_ROUTING_TABLE;i++)
	{
		route_table[i] = 0x0000;
		dest_route_table[i] = 0x0000;
	}
	
	// Set to zero
	for(i=0;i<VNET_MULTICAST_SIZE;i++)
		multicast_groups[i] = 0x0000;
	
	// Init the active media
	#if (VNET_MEDIA1_ENABLE)
		vNet_Init_M1();
	#endif

	#if (VNET_MEDIA2_ENABLE)
		vNet_Init_M2();	
	#endif
	
	#if (VNET_MEDIA3_ENABLE)
		vNet_Init_M3();		
	#endif

	#if (VNET_MEDIA4_ENABLE)
		vNet_Init_M4();	
	#endif
	
	#if (VNET_MEDIA5_ENABLE)
		vNet_Init_M5();	
	#endif			
}

/**************************************************************************/
/*!
    Send data to other devices over the Virtual Network
*/
/**************************************************************************/
U8 vNet_Send(U16 addr, oFrame *frame, U8 len, U8 port)
{
	U8 media, *frame_pnt;
	U16 routed_addr;
	
	// If the destination is the NULL address, discard it
	if(addr==VNET_ADDR_NULL)
	{
		#if(VNET_DEBUG)
		VNET_LOG("(vNet)<ADDR_NULL>\r\n");
		#endif
		
		// Free the frame and return
		oFrame_Reset();
		return VNET_DATA_FAIL;
	}
	
	// Upper layers cannot use broadcast, broadcast can be used only at vNet level
	if((addr == VNET_ADDR_BRDC) || (addr == VNET_ADDR_nBRDC))
		return	vNet_SendBroadcast(frame, len, port, addr);
	else if((addr > VNET_ADDR_L_MLC) && (addr <= VNET_ADDR_H_MLC))	// If destination is a multicast address
		return vNet_SendMulticast(frame, len, port, addr);
	else
		vNet_OutPath(addr, &routed_addr, &media);			// Look for outpath message

	#if(VNET_DEBUG)
    VNET_LOG("(vNet)<MEDIA><|0x");
	VNET_LOG(media,HEX);
	VNET_LOG(">\r\n");
	#endif
		
	frame_pnt = &vNet_header[0];							// Get header pointer

	// Prepare header
	*frame_pnt++ = len+VNET_HEADER_SIZE;					// Frame Length
	*frame_pnt++ = port;									// Frame Port
	*(U16 *)frame_pnt = addr;								// Final Destination Address
	frame_pnt += sizeof(U16);
	*(U16 *)frame_pnt = vNet_Media[media-1].src_addr;		// Original Source Address
	frame_pnt += sizeof(U16);
	
	// Build the complete frame
	oFrame_Define(&vNet_oFrame);
	oFrame_Set(vNet_header, 0, VNET_HEADER_SIZE, 0, frame);
	
	// Include debug functionalities, if required
	#if(VNET_DEBUG)
	// Print address  
    VNET_LOG("(vNet)<OUT><DADDR><|0x");
	VNET_LOG(routed_addr,HEX);
	VNET_LOG("><|0x");
	VNET_LOG(len+VNET_HEADER_SIZE,HEX);
	VNET_LOG("|0x");
	VNET_LOG(port,HEX);
	VNET_LOG("|0x");
	VNET_LOG(addr,HEX);
	VNET_LOG("|0x");
	VNET_LOG(vNet_Media[media-1].src_addr,HEX);
	
	VNET_LOG(">\r\n");
	#endif
	
	
	// Send the frame
	switch(media)
	{
	#if (VNET_MEDIA1_ENABLE)
		case(1):	// Send out on Media 1
			return vNet_Send_M1(routed_addr, &vNet_oFrame, len + VNET_HEADER_SIZE);
		break;
	#endif
	
	#if (VNET_MEDIA2_ENABLE)	
		case(2):	// Send out on Media 2
			return vNet_Send_M2(routed_addr, &vNet_oFrame, len + VNET_HEADER_SIZE);		
		break;
	#endif
	
	#if (VNET_MEDIA3_ENABLE)	
		case(3):	// Send out on Media 3
			return vNet_Send_M3(routed_addr, &vNet_oFrame, len + VNET_HEADER_SIZE);		
		break;
	#endif
	
	#if (VNET_MEDIA4_ENABLE)	
		case(4):	// Send out on Media 4
			return vNet_Send_M4(routed_addr, &vNet_oFrame, len + VNET_HEADER_SIZE);		
		break;
	#endif
	
	#if (VNET_MEDIA5_ENABLE)		
		case(5):	// Send out on Media 5
			return vNet_Send_M5(routed_addr, &vNet_oFrame, len + VNET_HEADER_SIZE);		
		break;
	#endif
	}
	
	#if(VNET_DEBUG) 
    VNET_LOG("(vNet)<OUT>-FAIL\r\n");
	#endif
	
	return VNET_DATA_FAIL;
}

/**************************************************************************/
/*!
    Send data to other devices over the Virtual Network
*/
/**************************************************************************/
U8 vNet_SendBroadcast(oFrame *frame, U8 len, U8 port, U16 broadcast_addr)
{
	U8 *frame_pnt;
	
	for(U8 media=0;media<VNET_MEDIA_NUMBER;media++)
	{
		if(vnet_media_en[media])
		{
			// oFrames can be used only once, so we use a copy
			oFrame_Copy(&message, frame);
		
			frame_pnt = &vNet_header[0];							// Get header pointer

			// Prepare header
			*frame_pnt++ = len+VNET_HEADER_SIZE;					// Frame Length
			*frame_pnt++ = port;									// Frame Port
			*(U16 *)frame_pnt = broadcast_addr;						// Final Destination Address
			frame_pnt += sizeof(U16);
			*(U16 *)frame_pnt = vNet_Media[media].src_addr;			// Original Source Address
			frame_pnt += sizeof(U16);
			
			// Build the complete frame
			oFrame_Define(&vNet_oFrame);
			oFrame_Set(vNet_header, 0, VNET_HEADER_SIZE, 0, &message);
			
			// Include debug functionalities, if required
			#if(VNET_DEBUG)
			// Print address  
			VNET_LOG("(vNet)<BRD><Media><|0x");
			VNET_LOG(media+1,HEX);
			VNET_LOG("><|0x");
			VNET_LOG(len+VNET_HEADER_SIZE,HEX);
			VNET_LOG("|0x");
			VNET_LOG(port,HEX);
			VNET_LOG("|0x");
			VNET_LOG(broadcast_addr,HEX);
			VNET_LOG("|0x");
			VNET_LOG(vNet_Media[media].src_addr,HEX);
			
			VNET_LOG(">\r\n");
			#endif
			

			// Send the frame
			switch(media+1)
			{
			#if (VNET_MEDIA1_ENABLE)
				case(1):	// Send out on Media 1
					vNet_Send_M1(broadcast_addr, &vNet_oFrame, len + VNET_HEADER_SIZE);
				break;
			#endif
					
			#if (VNET_MEDIA2_ENABLE)	
				case(2):	// Send out on Media 2
					vNet_Send_M2(broadcast_addr, &vNet_oFrame, len + VNET_HEADER_SIZE);		
				break;
			#endif
					
			#if (VNET_MEDIA3_ENABLE)	
				case(3):	// Send out on Media 3
					vNet_Send_M3(broadcast_addr, &vNet_oFrame, len + VNET_HEADER_SIZE);		
				break;
			#endif
					
			#if (VNET_MEDIA4_ENABLE)	
				case(4):	// Send out on Media 4
					vNet_Send_M4(broadcast_addr, &vNet_oFrame, len + VNET_HEADER_SIZE);		
				break;
			#endif
					
			#if (VNET_MEDIA5_ENABLE)		
				case(5):	// Send out on Media 5
					vNet_Send_M5(broadcast_addr, &vNet_oFrame, len + VNET_HEADER_SIZE);		
				break;
			#endif
			}	
		}
	}		
	
	// The frame is no longer needed, we clear it
	oFrame_Define(frame);
	oFrame_Reset();
	
	return VNET_DATA_FAIL;
}

/**************************************************************************/
/*!
    Send data to other devices over the Virtual Network
*/
/**************************************************************************/
U8 vNet_SendMulticast(oFrame *frame, U8 len, U8 port, U16 multicastgroup)
{
	U8 *frame_pnt;
	U16 broadcast_addr = VNET_ADDR_BRDC;
	
	// A valid multicast group is between 0xFF01 and 0xFFFE
	if((multicastgroup <= VNET_ADDR_L_MLC) || (multicastgroup == VNET_ADDR_BRDC)) 
	{
		#if(VNET_DEBUG)
		VNET_LOG("(vNet)<MLT><FAIL>\r\n");
		#endif	
		
		// Free the frame and return
		oFrame_Reset();
		return VNET_DATA_FAIL;
	}
	
	for(U8 media=0;media<VNET_MEDIA_NUMBER;media++)
	if(vnet_media_en[media])
	{
		// oFrames can be used only once, so we use a copy
		oFrame_Copy(&message, frame);
	
		frame_pnt = &vNet_header[0];							// Get header pointer

		// Prepare header
		*frame_pnt++ = len+VNET_HEADER_SIZE;					// Frame Lenght
		*frame_pnt++ = port;									// Frame Port
		*(U16 *)frame_pnt = broadcast_addr;						// Final Destination Address
		frame_pnt += sizeof(U16);
		*(U16 *)frame_pnt = multicastgroup;						// Original Source Address
		frame_pnt += sizeof(U16);
		
		// Build the complete frame
		oFrame_Define(&vNet_oFrame);
		oFrame_Set(vNet_header, 0, VNET_HEADER_SIZE, 0, &message);
		
		// Include debug functionalities, if required
		#if(VNET_DEBUG)
		// Print address  
		VNET_LOG("(vNet)<MLT><Media><|0x");
		VNET_LOG(media+1,HEX);
		VNET_LOG("><|0x");
		VNET_LOG(len+VNET_HEADER_SIZE,HEX);
		VNET_LOG("|0x");
		VNET_LOG(port,HEX);
		VNET_LOG("|0x");
		VNET_LOG(broadcast_addr,HEX);
		VNET_LOG("|0x");
		VNET_LOG(vNet_Media[media].src_addr,HEX);
		
		VNET_LOG(">\r\n");
		#endif
		

		// Send the frame
		switch(media+1)
		{
		#if (VNET_MEDIA1_ENABLE)
			case(1):	// Send out on Media 1
				vNet_Send_M1(broadcast_addr, &vNet_oFrame, len + VNET_HEADER_SIZE);
			break;
		#endif
				
		#if (VNET_MEDIA2_ENABLE)	
			case(2):	// Send out on Media 2
				vNet_Send_M2(broadcast_addr, &vNet_oFrame, len + VNET_HEADER_SIZE);		
			break;
		#endif
				
		#if (VNET_MEDIA3_ENABLE)	
			case(3):	// Send out on Media 3
				vNet_Send_M3(broadcast_addr, &vNet_oFrame, len + VNET_HEADER_SIZE);		
			break;
		#endif
				
		#if (VNET_MEDIA4_ENABLE)	
			case(4):	// Send out on Media 4
				vNet_Send_M4(broadcast_addr, &vNet_oFrame, len + VNET_HEADER_SIZE);		
			break;
		#endif
				
		#if (VNET_MEDIA5_ENABLE)		
			case(5):	// Send out on Media 5
				vNet_Send_M5(broadcast_addr, &vNet_oFrame, len + VNET_HEADER_SIZE);		
			break;
		#endif
		}
		
	}	
	
	// The frame is no longer needed, we clear it
	oFrame_Define(frame);
	oFrame_Reset();
	
	return VNET_DATA_FAIL;
}

/**************************************************************************/
/*!
    Send data to other devices over the Virtual Network
*/
/**************************************************************************/
U8 vNet_SendData(U16 addr, U8 *data, U8 len, U8 port)
{	
	// Define the output frame
	oFrame_Define(&message);
	oFrame_Set(0, data, 0, len, 0);
	
	// Send the output frame
	return vNet_Send(addr, &message, len, port);
	
}

/**************************************************************************/
/*!
    Send data to other devices over the Virtual Network without build the
	vNet header
*/
/**************************************************************************/
U8 vNet_SendRoute(U16 routed_addr, U8 media, U8 *data, U8 len)
{	
	// Include debug functionalities, if required
	#if(VNET_DEBUG)
	// Print address  
    VNET_LOG("(vNet)<OUT><ROUT><|0x");
	VNET_LOG(routed_addr,HEX);
	VNET_LOG("><|0x");
	VNET_LOG(data[0],HEX);
	VNET_LOG("|0x");
	VNET_LOG(data[1],HEX);
	VNET_LOG("|0x");
	VNET_LOG(*(U16*)(data+2),HEX);
	VNET_LOG("|0x");
	VNET_LOG(*(U16*)(data+4),HEX);
		
	for(U8 i=VNET_HEADER_SIZE;i<len;i++)
	{
		VNET_LOG("|0x");
		VNET_LOG(data[i],HEX);
	}
	VNET_LOG(">\r\n");
	#endif
		

	// Build the frame
	oFrame_Define(&vNet_oFrame);
	oFrame_Set(0, data, 0, len, 0);
		
	// Route the frame
	switch(media)
	{
	#if (VNET_MEDIA1_ENABLE)
		case(1):	// Send out on Media 1
			return vNet_Send_M1(routed_addr, &vNet_oFrame, len);
		break;
	#endif	
	
	#if (VNET_MEDIA2_ENABLE)
		case(2):	// Send out on Media 2
			return vNet_Send_M2(routed_addr, &vNet_oFrame, len);				
		break;
	#endif
	
	#if (VNET_MEDIA3_ENABLE)	
		case(3):	// Send out on Media 3
			return vNet_Send_M3(routed_addr, &vNet_oFrame, len);		
		break;
	#endif
	
	#if (VNET_MEDIA4_ENABLE)	
		case(4):	// Send out on Media 4
			return vNet_Send_M4(routed_addr, &vNet_oFrame, len);		
		break;
	#endif
	
	#if (VNET_MEDIA5_ENABLE)	
		case(5):	// Send out on Media 5
			return vNet_Send_M5(routed_addr, &vNet_oFrame, len);		
		break;
	#endif		
	}

}

/**************************************************************************/
/*!
    Look for incoming data, returns the number of media with available data
*/
/**************************************************************************/
U8 vNet_DataAvailable()
{
	U8 i=0;			//Number of messages received

	#if (VNET_MEDIA1_ENABLE)
		if(vNet_DataAvailable_M1())
		{
			// If data available, set the flag and increase the counter
			vNet_Media_Data[0].data_available = 1;
			i++;
		}
	#endif
	
	#if (VNET_MEDIA2_ENABLE)
		if(vNet_DataAvailable_M2())
		{
			vNet_Media_Data[1].data_available = 1;
			i++;
		}	
	#endif
	
	#if (VNET_MEDIA3_ENABLE)
		if(vNet_DataAvailable_M3())
		{
			vNet_Media_Data[2].data_available = 1;
			i++;
		}	
		#endif

	#if (VNET_MEDIA4_ENABLE)
		if(vNet_DataAvailable_M4())
		{
			vNet_Media_Data[3].data_available = 1;
			i++;
		}	
	#endif
	
	#if (VNET_MEDIA5_ENABLE)
		if(vNet_DataAvailable_M5())
		{
			vNet_Media_Data[4].data_available = 1;
			i++;
		}	
	#endif	
	
	// If there are no data available, increase the resettime timer
	if(!i)	resettime++;
	else	resettime=0;

	// If we were here too much times without find available data
	if(resettime>=VNET_RESETTIME)
	{
		resettime=0;
		
		// Reset all the communication interfaces
		vNet_Reset();
	}
		
	return i;
}

/**************************************************************************/
/*!
    Retrieve the vNet port for thenext pending frame
*/
/**************************************************************************/
U8 vNet_GetPort()
{
	#if (VNET_MEDIA1_ENABLE)
		if(vNet_Media_Data[0].data_available == 1)
			return vNet_Media_Data[0].port;
	#endif

	#if (VNET_MEDIA2_ENABLE)
		if(vNet_Media_Data[1].data_available == 1)
			return vNet_Media_Data[1].port;	
	#endif	

	#if (VNET_MEDIA3_ENABLE)
		if(vNet_Media_Data[2].data_available == 1)
			return vNet_Media_Data[2].port;
	#endif

	#if (VNET_MEDIA4_ENABLE)
		if(vNet_Media_Data[3].data_available == 1)
			return vNet_Media_Data[3].port;	
	#endif	

	#if (VNET_MEDIA5_ENABLE)
		if(vNet_Media_Data[4].data_available == 1)
			return vNet_Media_Data[4].port;	
	#endif	
	
	return VNET_DATA_FAIL;	// No data retrieved
}

/**************************************************************************/
/*!
    Retrieve data from the Virtual Network for the selected media, should
	run once for each message in queue from DataAvailable
*/
/**************************************************************************/
U8 vNet_RetrieveData(U8 *data)
{	
	#if (VNET_MEDIA1_ENABLE)
		if(vNet_Media_Data[0].data_available == 1)
		{
			// Retrieve data from buffer
			if(vNet_RetrieveData_M1(data))
			{		
				vNet_Media_Data[0].data = data;		// Assign Data Buffer
				vNet_ParseFrame(1);					// Parse Data Buffer
			
				last_media = 1;
				vNet_Media_Data[0].data_available = 0;
							
				// Route data and remove header, if necessary
				return vNet_RoutingBridging(1);
			}
			// If retreive failed, return error
			vNet_Media_Data[0].data_available = 0;
			return VNET_DATA_FAIL;
		}	
	#endif
	
	#if (VNET_MEDIA2_ENABLE)
		if(vNet_Media_Data[1].data_available == 1)
		{
			// Retrieve data from buffer
			if(vNet_RetrieveData_M2(data))
			{
				vNet_Media_Data[1].data = data;		// Assign Data Buffer
				vNet_ParseFrame(2);					// Parse Data Buffer
	
				last_media = 2;
				vNet_Media_Data[1].data_available = 0;
				
				// Route data and remove header, if necessary
				return vNet_RoutingBridging(2);
			}
			// If retreive failed, return error
			vNet_Media_Data[1].data_available = 0;
			return VNET_DATA_FAIL;
			
		}	

	#endif
	
	#if (VNET_MEDIA3_ENABLE)
		if(vNet_Media_Data[2].data_available == 1)
		{
			// Retrieve data from buffer
			if(vNet_RetrieveData_M3(data))
			{
				vNet_Media_Data[2].data = data;		// Assign Data Buffer
				vNet_ParseFrame(3);					// Parse Data Buffer
			
				last_media = 3;
				vNet_Media_Data[2].data_available = 0;
				
				// Route data and remove header, if necessary
				return vNet_RoutingBridging(3);
			}
			// If retreive failed, return error
			vNet_Media_Data[2].data_available = 0;
			return VNET_DATA_FAIL;			
		}	

	#endif

	#if (VNET_MEDIA4_ENABLE)
		if(vNet_Media_Data[3].data_available == 1)
		{
			// Retrieve data from buffer
			if(vNet_RetrieveData_M4(data))
			{
				vNet_Media_Data[3].data = data;		// Assign Data Buffer
				vNet_ParseFrame(4);					// Parse Data Buffer
			
				last_media = 4;
				vNet_Media_Data[3].data_available = 0;
				
				// Route data and remove header, if necessary
				return vNet_RoutingBridging(4);
			}
			// If retreive failed, return error
			vNet_Media_Data[3].data_available = 0;
			return VNET_DATA_FAIL;				
		}	

	#endif
	
	#if (VNET_MEDIA5_ENABLE)
		if(vNet_Media_Data[4].data_available == 1)
		{
			// Retrieve data from buffer
			if(vNet_RetrieveData_M5(data))
			{
				vNet_Media_Data[4].data = data;		// Assign Data Buffer
				vNet_ParseFrame(5);					// Parse Data Buffer
			
				last_media = 5;
				vNet_Media_Data[4].data_available = 0;
				
				// Route data and remove header, if necessary
				return vNet_RoutingBridging(5);
			}
			// If retreive failed, return error
			vNet_Media_Data[4].data_available = 0;
			return VNET_DATA_FAIL;	
		}	

	#endif	
	
	return VNET_DATA_FAIL;	// No data retrieved
}

/**************************************************************************/
/*!
    Set the vNet address (addr) for the selected media
*/
/**************************************************************************/
void vNet_SetAddress(U16 addr, U8 media)
{
	vNet_Media[media-1].src_addr = addr;
	
	// Include debug functionalities, if required
	#if(VNET_DEBUG)
	// Print address  
    VNET_LOG("(vNet)<SETADDR><|0x");
	VNET_LOG(vNet_Media[media-1].src_addr,HEX);
	VNET_LOG(">\r\n");
	#endif	
	
	// Write address into the driver
	switch(media)
	{
	#if (VNET_MEDIA1_ENABLE)
		case(1):	// Write out on Media 1
			vNet_SetAddress_M1(addr);
		break;
	#endif	
	
	#if (VNET_MEDIA2_ENABLE)
		case(2):	// Write out on Media 2
			vNet_SetAddress_M2(addr);	
		break;
	#endif
	
	#if (VNET_MEDIA3_ENABLE)	
		case(3):	// Write out on Media 3
			vNet_SetAddress_M3(addr);	
		break;
	#endif
	
	#if (VNET_MEDIA4_ENABLE)	
		case(4):	// Write out on Media 4
			vNet_SetAddress_M4(addr);	
		break;
	#endif
	
	#if (VNET_MEDIA5_ENABLE)	
		case(5):	// Write out on Media 5
			vNet_SetAddress_M5(addr);		
		break;
	#endif		
	}
}

/**************************************************************************/
/*!
    Get the vNet address (addr) for the selected media
*/
/**************************************************************************/
U16 vNet_GetAddress(U8 media)
{
	return vNet_Media[media-1].src_addr;
}

/**************************************************************************/
/*!
    Get the last Media used to retrieve data
*/
/**************************************************************************/
U8 vNet_GetLastMedia()
{
	return last_media;
}

/**************************************************************************/
/*!
    Get the last source address (addr) for the selected media
*/
/**************************************************************************/
U16 vNet_GetSourceAddress(U8 media)
{
	return vNet_Media_Data[media-1].src_addr;
}

/**************************************************************************/
/*!
    Get the last original source address (addr) for the selected media
*/
/**************************************************************************/
U16 vNet_GetoSourceAddress(U8 media)
{
	return vNet_Media_Data[media-1].o_src_addr;
}

/**************************************************************************/
/*!
    Get the last final destination address (addr) for the selected media
*/
/**************************************************************************/
U16 vNet_GetfDestinationAddress(U8 media)
{
	return vNet_Media_Data[media-1].f_dest_addr;
}

/**************************************************************************/
/*!
    Get the last port number for the selected media
*/
/**************************************************************************/
U8 vNet_GetPortNumber(U8 media)
{
	return vNet_Media_Data[media-1].port;
}

/**************************************************************************/
/*!
    Set the vNet subnet mask
*/
/**************************************************************************/
void vNet_SetSubnetMask(U16 subnetmask, U8 media)
{
	// If the selected media is not ethernet (ethernet media is equal to 1)
	// assign the subnetmask, otherwise place the subnet mask to 0.
	// In this way we cannot create any subnet on the Ethernet side and all
	// data in Media 1 addresses range are shared over the same Ethernet media.
	if(media != 1)
		vNet_Media[media-1].subnetmask = subnetmask;
	else
		vNet_Media[media-1].subnetmask = 0x0000;	
}

/**************************************************************************/
/*!
    Get the vNet subnet mask
*/
/**************************************************************************/
U16 vNet_GetSubnetMask(U8 media)
{
	return vNet_Media[media-1].subnetmask;
}

/**************************************************************************/
/*!
    Set the vNet associated SuperNode
*/
/**************************************************************************/
void vNet_SetMySuperNode(U16 mySuperNode, U8 media)
{
	vNet_Media[media-1].mySuperNode = mySuperNode;
}

/**************************************************************************/
/*!
    Get the vNet associated SuperNode
*/
/**************************************************************************/
U16 vNet_GetMySuperNode(U8 media)
{
	return vNet_Media[media-1].mySuperNode;
}

/**************************************************************************/
/*!
    Set the entries for the routing tables
*/
/**************************************************************************/
U8 vNet_SetRoutingTable(U16 dest_path, U16 src_path, U8 index)
{
	if(index < VNET_ROUTING_TABLE)
	{
		route_table[index] = src_path;
		dest_route_table[index] = dest_path;
	}	
	else
		return VNET_FAIL;
}
 
/**************************************************************************/
/*!
    Return the code of the media active for the board, this has to be
	used only for devices that has no more than one media active.
*/
/**************************************************************************/
U8 vNet_MyMedia()
{
	U8 i = 0;
	
	// Look for my active media
	while(!vnet_media_en[i] && i < VNET_MEDIA_NUMBER - 1)
		i++;
	
	if(vnet_media_en[i])
		return i+1;
	else
		return VNET_DATA_FAIL;
}
/**************************************************************************/
/*!
    Return the code for the media associated to the address (addr)
*/
/**************************************************************************/
U8 vNet_GetMedia(U16 addr)
{
	U8 i=0;

	// look for address range
	for(i=0;i<VNET_MEDIA_NUMBER;i++)
		if((addr <= vnet_addr_h[i]) && (addr > vnet_addr_l[i]))
			return i+1;
	
	// if address is not in range
	return 0;
}

/**************************************************************************/
/*!
    Return the code of the media active that hasn't yet a valid address
*/
/**************************************************************************/
U8 vNet_MyMediasWithoutAddress(U8* media)
{
	U8 i = 0;
	
	// Returns the fist media that needs an Address
	for(i=0; i<VNET_MEDIA_NUMBER; i++)
	{
		// Return if an active media without an address set if found
		if(vnet_media_en[i] && !(vNet_Media[i].src_addr))
		{
			*media = i+1;
			return VNET_SUCCESS;
		}	
	}
	
	// No media needs an address
	return VNET_FAIL;
}

/**************************************************************************/
/*!
    Evaluate the route or bridge path, give the routed address and the 
	media to use to route or bridge the message
*/
/**************************************************************************/
void vNet_OutPath(U16 addr, U16 *routed_addr, U8 *media)
{
	U16 subn; 			
	U8	route_index = 0;
		
	// Get media used from destination address
	*media = vNet_GetMedia(addr);

	#if(VNET_DEBUG)
    VNET_LOG("(vNet)<OUTPATH><|0x");
	VNET_LOG(*media,HEX);
	VNET_LOG(">\r\n");
	#endif
	
	// Bridge to a node within the same subnet
	if ((vnet_media_en[*media-1]) && ((vNet_Media[*media-1].src_addr & vNet_Media[*media-1].subnetmask) == (addr & vNet_Media[*media-1].subnetmask)))
		*routed_addr = addr;	
	else
	{
		#if (VNET_SUPERNODE)
		// Route to neighbour			
		if(vNet_Media[*media-1].subnetmask)
		{
			subn = addr & vNet_Media[*media-1].subnetmask;		// Final destination subnet
			*routed_addr = subn | 0x0001;						// Route to first address of subnet
		}
		else
			subn = addr & DYNAMICADDR_SUBNETMASK;				// Use a standard subnetmask if the media isn't configured
		
		#if(VNET_DEBUG)
		VNET_LOG("(vNet)<NHBOR><|0x");
		VNET_LOG(*media,HEX);
		VNET_LOG(">\r\n");
		#endif
		
		// Look into the routing table
		while ((route_table[route_index] != subn) && (route_index < VNET_ROUTING_TABLE))	
			route_index++;														   	
			
		// Apply the routing path found in route table, if any
		if (subn && (route_table[route_index] == subn))
		{
			*routed_addr = dest_route_table[route_index];
			*media = vNet_GetMedia(*routed_addr);	
		}	
		#else	
		// Route to my supernode
			
		// Look for my active media
		*media = vNet_MyMedia();
				
		// Associated supernode	address		
		*routed_addr = vNet_Media[*media-1].mySuperNode;
			
		#endif		
	}
}

/**************************************************************************/
/*!
    Check incoming message on selected media, then route or bridge the
	message if necessary
*/
/**************************************************************************/
U8 vNet_RoutingBridging(U8 media)
{
	U16 routed_addr;
	
	// If message is for this node
	if((vNet_Media[media-1].src_addr == vNet_Media_Data[media-1].f_dest_addr))
	{	
		// Remove the header from the message
		memmove(vNet_Media_Data[media-1].data, vNet_Media_Data[media-1].data+VNET_HEADER_SIZE, vNet_Media_Data[media-1].len-VNET_HEADER_SIZE);
		vNet_Media_Data[media-1].len-=VNET_HEADER_SIZE;
		
		return vNet_Media_Data[media-1].len;	// No need to route
	}
	// If is a broadcast that doesn't need rebroadcast
	else if(((vNet_Media_Data[media-1].f_dest_addr) == VNET_ADDR_nBRDC))
	{
		// If is a multicast
		if(((vNet_Media_Data[media-1].o_src_addr) > VNET_ADDR_L_MLC) && ((vNet_Media_Data[media-1].o_src_addr) <= VNET_ADDR_H_MLC))
		{
			U8 i=0;
			// Match the multicast address with the subscribed ones
			for(i=0; i<VNET_MULTICAST_SIZE; i++)
				if(vNet_Media_Data[media-1].o_src_addr == multicast_groups[i])
					break;
				
			// If there is no match discard
			if(i==VNET_MULTICAST_SIZE)	
				return VNET_DATA_FAIL;
		}
		
		// Remove the header from the message
		memmove(vNet_Media_Data[media-1].data, vNet_Media_Data[media-1].data+VNET_HEADER_SIZE, vNet_Media_Data[media-1].len-VNET_HEADER_SIZE);
		vNet_Media_Data[media-1].len-=VNET_HEADER_SIZE;
		
		return vNet_Media_Data[media-1].len;	// No need to route	
	}
	// If is a broadcast message that needs to be spread over the network
	else if ((vNet_Media_Data[media-1].f_dest_addr == VNET_ADDR_BRDC)  || (vNet_Media_Data[media-1].f_dest_addr == VNET_ADDR_wBRDC))
	{
	#if (VNET_SUPERNODE)
		// Generally there is no need to rebroadcast a frame over the same media from where has been received,
		// this isn't true for wireless network, where rebroadcasting over the same media is used to extend
		// the listening range
		U8 skip_mymedia = 1;		// If set, we don't rebroadcast over the same media

		// Modify the destination address as subnet broadcast, this will avoid broadcast loops	
		if(vNet_Media_Data[media-1].f_dest_addr == VNET_ADDR_BRDC)
		{
			*(U16*)(vNet_Media_Data[media-1].data+2) = VNET_ADDR_wBRDC;

			// Rebroadcast over wireless media is allowed
			if((media-1)==VNET_MEDIA2_ID)
				skip_mymedia = 0;		
		}
		#if(VNET_LOOPS)
		else
			*(U16*)(vNet_Media_Data[media-1].data+2) = VNET_ADDR_nBRDC;		// Prevent broadcast loops
		#endif
		
		// If the source address isn't null, rebroadcast the message over the active media
		if(vNet_Media_Data[media-1].o_src_addr)
			for(U8 i=0;i<VNET_MEDIA_NUMBER;i++)
				if(vnet_media_en[i] && (!skip_mymedia || (i != (media-1))))
					vNet_SendRoute(0xFFFF, i+1, vNet_Media_Data[media-1].data, vNet_Media_Data[media-1].len);

		// If is a multicast
		if(((vNet_Media_Data[media-1].o_src_addr) > VNET_ADDR_L_MLC) && ((vNet_Media_Data[media-1].o_src_addr) <= VNET_ADDR_H_MLC))
		{
			U8 i=0;
			// Match the multicast address with the subscribed ones
			for(i=0; i<VNET_MULTICAST_SIZE; i++)
				if(vNet_Media_Data[media-1].o_src_addr == multicast_groups[i])
					break;
				
			// If there is no match discard
			if(i==VNET_MULTICAST_SIZE)	
				return VNET_DATA_FAIL;
		}
					
		// Remove the header from the message
		memmove(vNet_Media_Data[media-1].data, vNet_Media_Data[media-1].data+VNET_HEADER_SIZE, vNet_Media_Data[media-1].len-VNET_HEADER_SIZE);
		vNet_Media_Data[media-1].len-=VNET_HEADER_SIZE;
		return vNet_Media_Data[media-1].len;	// Process the message	
		
	#else	// Only supernodes can spread a broadcast message across the network

		// If is a multicast
		if(((vNet_Media_Data[media-1].o_src_addr) > VNET_ADDR_L_MLC) && ((vNet_Media_Data[media-1].o_src_addr) <= VNET_ADDR_H_MLC))
		{
			U8 i=0;
			// Match the multicast address with the subscribed ones
			for(i=0; i<VNET_MULTICAST_SIZE; i++)
				if(vNet_Media_Data[media-1].o_src_addr == multicast_groups[i])
					break;
				
			// If there is no match discard
			if(i==VNET_MULTICAST_SIZE)	
				return VNET_DATA_FAIL;
		}
	
		// Remove the header from the message
		memmove(vNet_Media_Data[media-1].data, vNet_Media_Data[media-1].data+VNET_HEADER_SIZE, vNet_Media_Data[media-1].len-VNET_HEADER_SIZE);
		vNet_Media_Data[media-1].len-=VNET_HEADER_SIZE;
		return vNet_Media_Data[media-1].len;	// No need to route	
	#endif
	}
	else
	{	
	#if (VNET_SUPERNODE)
		// Find out the routing/bridging path
		U8 out_media;
		vNet_OutPath(vNet_Media_Data[media-1].f_dest_addr, &routed_addr, &out_media);
		
		// Move out the message on the path
		if(vNet_SendRoute(routed_addr, out_media, vNet_Media_Data[media-1].data, vNet_Media_Data[media-1].len))
			return VNET_DATA_ROUTED;

		return VNET_DATA_FAIL;
	#else
		return VNET_DATA_FAIL;
	#endif
	}
}

/**************************************************************************/
/*!
    Parse the frame to get out length, addresses, port 
*/
/**************************************************************************/
void vNet_ParseFrame(U8 media)
{
	U8 *data_pnt;
	data_pnt = vNet_Media_Data[media-1].data;						// Get Data Pointer
	
	vNet_Media_Data[media-1].len = *data_pnt++;						// Retrieve Length
				
	vNet_Media_Data[media-1].port = *data_pnt++;					// Retrieve Port
				
	vNet_Media_Data[media-1].f_dest_addr = (U16)*(U16 *)data_pnt;	// Retrieve Final Destination Address
	data_pnt += sizeof(U16);
				
	vNet_Media_Data[media-1].o_src_addr = (U16)*(U16 *)data_pnt;	// Retrieve Original Source Address
	data_pnt += sizeof(U16);

	switch(media)
	{
	#if (VNET_MEDIA1_ENABLE)
		case(1):
			vNet_Media_Data[media-1].src_addr = vNet_GetSourceAddress_M1(); 
		break;
	#endif
	
	#if (VNET_MEDIA2_ENABLE)
		case(2):
			vNet_Media_Data[media-1].src_addr = vNet_GetSourceAddress_M2(); 
		break;
	#endif
	
	#if (VNET_MEDIA3_ENABLE)
		case(3):
			vNet_Media_Data[media-1].src_addr = vNet_GetSourceAddress_M3(); 
		break;
	#endif
	
	#if (VNET_MEDIA4_ENABLE)
		case(4):
			vNet_Media_Data[media-1].src_addr = vNet_GetSourceAddress_M4(); 
		break;
	#endif		
	
	#if (VNET_MEDIA5_ENABLE)
		case(5):
			vNet_Media_Data[media-1].src_addr = vNet_GetSourceAddress_M5(); 
		break;
	#endif	
		
	}
	
	// Include debug functionalities, if required
	#if(VNET_DEBUG)
	// Print address  
    VNET_LOG("(vNet)<IN><|0x");
	VNET_LOG(vNet_Media_Data[media-1].len,HEX);
	VNET_LOG("|0x");
	VNET_LOG(vNet_Media_Data[media-1].port,HEX);
	VNET_LOG("|0x");
	VNET_LOG((U16)vNet_Media_Data[media-1].f_dest_addr,HEX);
	VNET_LOG("|0x");
	VNET_LOG((U16)vNet_Media_Data[media-1].o_src_addr,HEX);
		
	for(U8 i=VNET_HEADER_SIZE;i<vNet_Media_Data[media-1].len;i++)
	{
		VNET_LOG("|0x");
		VNET_LOG(*(vNet_Media_Data[media-1].data+i),HEX);
	}
	VNET_LOG(">\r\n");
	#endif
		
	#if(VNET_SUPERNODE && VNET_BRDCAST)
	// Sometimes broadcast data can be used to build routing and bridging paths
	if((vNet_Media_Data[media-1].src_addr) && (vNet_Media_Data[media-1].src_addr != vNet_Media_Data[media-1].o_src_addr))
	{
		// Get the media from the original source address
		U8 src_media = vNet_GetMedia(vNet_Media_Data[media-1].o_src_addr);	
		U16 submask = vNet_Media[src_media-1].subnetmask;
	
		// Data from IP sources doesn't need a vNet route
		if((src_media-1)==VNET_MEDIA1_ID)
			return;
	
		// If we have no subnetmask for that media, use a default one
		if(!submask) submask = DYNAMICADDR_SUBNETMASK;
		
		// If there is no direct connection, store the path
		if(!(vnet_media_en[src_media-1]) ||
			(vNet_Media_Data[media-1].src_addr != (vNet_Media_Data[media-1].o_src_addr & submask | 0x0001)))
		{
			// The frame is coming from an unconventional path
			U8 i=0;
				
			// If there is yet an entry there is not more to do
			for(i=0; i<VNET_ROUTING_TABLE; i++)
				if(route_table[i] == (vNet_Media_Data[media-1].o_src_addr & submask))
					return;
					
			// Look and empty entry
			i=0;
			while((route_table[i] != 0x0000) && (i<VNET_ROUTING_TABLE)) i++;
				
			// Update the routing table
			if(i < VNET_ROUTING_TABLE)
			{
				route_table[i] = (vNet_Media_Data[media-1].o_src_addr & submask);
				dest_route_table[i] = vNet_Media_Data[media-1].src_addr;
				
				#if(VNET_DEBUG)
				VNET_LOG("(vNet)<ROUTEUPDATE><|0x");
				VNET_LOG(vNet_Media_Data[media-1].o_src_addr & submask,HEX);
				VNET_LOG("|0x");
				VNET_LOG(vNet_Media_Data[media-1].src_addr,HEX);
				VNET_LOG(">\r\n");
				#endif
			}	
		}
	}	
	#endif	
		
}

/**************************************************************************/
/*!
    Subscribe to a multicast group
*/
/**************************************************************************/
void vNet_SetMulticastGroup(U16 multicastgroup, U8 multicastnumber)
{
	if(multicastnumber < VNET_MULTICAST_SIZE)
		multicast_groups[multicastnumber] = multicastgroup;
	
}

/**************************************************************************/
/*!
    Reset the Virtual Network
*/
/**************************************************************************/
void vNet_Reset()
{
	// Init the active media
	#if (VNET_MEDIA1_ENABLE)
		vNet_Init_M1();
	#endif

	#if (VNET_MEDIA2_ENABLE)
		vNet_Init_M2();	
	#endif
	
	#if (VNET_MEDIA3_ENABLE)
		vNet_Init_M3();		
	#endif

	#if (VNET_MEDIA4_ENABLE)
		vNet_Init_M4();	
	#endif
	
	#if (VNET_MEDIA5_ENABLE)
		vNet_Init_M5();	
	#endif	

	// Set the address for the active media
	for(U8 media=0;media<VNET_MEDIA_NUMBER;media++)
	{
		// Write address into the driver
		switch(media)
		{
		#if (VNET_MEDIA1_ENABLE)
			case(VNET_MEDIA1_ID):	// Write out on Media 1
				vNet_SetAddress_M1(vNet_Media[media].src_addr);
			break;
		#endif	
		
		#if (VNET_MEDIA2_ENABLE)
			case(VNET_MEDIA2_ID):	// Write out on Media 2
				vNet_SetAddress_M2(vNet_Media[media].src_addr);	
			break;
		#endif
		
		#if (VNET_MEDIA3_ENABLE)	
			case(VNET_MEDIA3_ID):	// Write out on Media 3
				vNet_SetAddress_M3(vNet_Media[media].src_addr);	
			break;
		#endif
		
		#if (VNET_MEDIA4_ENABLE)	
			case(VNET_MEDIA4_ID):	// Write out on Media 4
				vNet_SetAddress_M4(vNet_Media[media].src_addr);	
			break;
		#endif
		
		#if (VNET_MEDIA5_ENABLE)	
			case(VNET_MEDIA5_ID):	// Write out on Media 5
				vNet_SetAddress_M5(vNet_Media[media].src_addr);		
			break;
		#endif		
		}
	}		
}