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
#ifndef VNET_H
#define VNET_H

#include "tools/oFrame.h"
#include "GetConfig.h"		// need : vNet_Config.h, ethUsrCfg.h

#if (VNET_MEDIA1_ENABLE)
	// Driver for Wiznet W5100
	#if (ETH_W5100 || ETH_W5200 )
		#include "drivers/ethW5x00/vNetDriver_eth.h"	
	#endif
	
	// Driver for Microchip ENC28J60
	#if (ETH_ENC28J60)
		#include "drivers/ethENC28J60/vNetDriver_eth.h"	
	#endif
	
	// Driver for Microchip MRF2WB0MA
	#if (WIFI_MRF24)
		#include "drivers/wifiMRF24/vNetDriver_eth.h"	
	#endif
	
#endif

#if (VNET_MEDIA2_ENABLE)
	#include "drivers/chibi/vNetDriver_chibi.h"	
#endif
	
#if (VNET_MEDIA3_ENABLE)
	// Driver for Wiznet W5100
	#if (ETH_W5100 || ETH_W5200 )
		#include "drivers/ethW5x00/vNetDriver_raw.h"	
	#endif
	
	// Driver for Microchip EN28J60
	#if (ETH_ENC28J60)
		#include "drivers/ethENC28J60/vNetDriver_raw.h"	
	#endif	
#endif
	
#if (VNET_MEDIA5_ENABLE)
	#include "drivers/usart/vNetDriver_usart.h"	
#endif	

typedef struct
{
    U16 src_addr;		// Source Address
	U16 subnetmask;		// Subnet Mask
	U16 mySuperNode;	// Associated Supernode
} vNet;

typedef struct
{
	U8 data_available;		// Data Available
    U8 len;					// Message Lenght
	U8 port;				// Message Port
	U16 src_addr;			// Source Address
	U16 o_src_addr;			// Original Source Address
    U16 f_dest_addr;		// Final Destination Address
	U8 *data;				// Message Data
} vNet_Data;

void vNet_Init();											
U8 vNet_Send(U16 addr, oFrame *frame, U8 len, U8 port);	
U8 vNet_SendBroadcast(oFrame *frame, U8 len, U8 port);
U8 vNet_SendData(U16 addr, U8 *data, U8 len, U8 port);				
U8 vNet_DataAvailable();											
U8 vNet_RetrieveData(U8 *data);
void vNet_SetAddress(U16 addr, U8 media);
U16 vNet_GetAddress(U8 media);
U8 vNet_GetLastMedia();
U16 vNet_GetSourceAddress(U8 media);
U16 vNet_GetoSourceAddress(U8 media);
U16 vNet_GetfDestinationAddress(U8 media);
U8 vNet_GetPort();
U8 vNet_GetPortNumber(U8 media);
void vNet_SetSubnetMask(U16 subnetmask, U8 media);
U16 vNet_GetSubnetMask(U8 media);
void vNet_SetMySuperNode(U16 mySuperNode, U8 media);
U16 vNet_GetMySuperNode(U8 media);
U8 vNet_SetRoutingTable(U16 dest_path, U16 src_path, U8 index);
U8 vNet_SetBridgingTable(U8 media_in, U8 media_out);
U8 vNet_MyMedia();
U8 vNet_GetMedia(U16 addr);

#endif