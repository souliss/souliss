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
/**************************************************************************/
#ifndef GENCONF_H
#define GENCONF_H

/**

	DO NOT MODIFY THIS FILE
	
	Configuration parameters shall be set in conf/QuickCfg.h, if you will to do a 
	full configuration manually use the dedicated files located in the same folder
	
**/

// Get Configuration
//
//	This file is referred as in the whole framework as the configuration one,
//	it contains the configuration parameters from all the configuration files
//	located in the /conf folder.
//
//	The configuration process is available as Quick and Detailed configuration,
//	quick configuration allows a simplified single point of configuration and 
//	doesn't require specific knowledge of Souliss framework and its component.
//	Quick configuration is available for supported hardware solution, and cover
//	most of available configuration cases.
//
//	Detailed configuration is enabled when the quick configuration is disabled
//	and require a knowledge of the Souliss framework and its components, lets
//	define the part of code that shall be compiled freedomly.
//	Configuration constrains are based on drivers features and some configuration
//	may not be allowed or supported.
//
//	Is not requested to edit this file in order to get a working configuration,
//	all configuration files are located in /conf folder.
//

#include "conf/QuickCfg.h"

#if(QC_ENABLE)	//	If Quick Configuration is enabled
	
	// Define the drivers to be loaded based on the board type
#	if(QC_BOARDTYPE == 0x00)
#		define	BOARD_MODEL				0x00
#		define VNET_MEDIA1_ENABLE  		0
#		define VNET_MEDIA2_ENABLE  		0
#		define VNET_MEDIA3_ENABLE  		0
#		define VNET_MEDIA4_ENABLE  		0
#		define VNET_MEDIA5_ENABLE  		0
#	elif(QC_BOARDTYPE == 0x01)
#		define BOARD_MODEL				0x01
#		define CHIBI_AT86RF230			1
#		define VNET_MEDIA1_ENABLE  		0
#		define VNET_MEDIA2_ENABLE  		1
#		define VNET_MEDIA3_ENABLE  		0
#		define VNET_MEDIA4_ENABLE  		0
#		define VNET_MEDIA5_ENABLE  		0
#	elif(QC_BOARDTYPE == 0x02)
#		define	ETH_W5100				1	
#		define	COMMS_MODEL				0x01
#		define	BOARD_MODEL				0x01
#		define	CHIBI_AT86RF230			1
#		define 	VNET_SUPERNODE  		1
#		define VNET_MEDIA1_ENABLE  		1
#		define VNET_MEDIA2_ENABLE  		1
#		define VNET_MEDIA3_ENABLE  		0
#		define VNET_MEDIA4_ENABLE  		0
#		define VNET_MEDIA5_ENABLE  		0
#	elif(QC_BOARDTYPE == 0x03)
#		define	ETH_W5100				1	
#		define	COMMS_MODEL				0x01
#		define	BOARD_MODEL				0x02
#		define VNET_MEDIA1_ENABLE  		1
#		define VNET_MEDIA2_ENABLE  		0
#		define VNET_MEDIA3_ENABLE  		0
#		define VNET_MEDIA4_ENABLE  		0
#		define VNET_MEDIA5_ENABLE  		0
#	elif(QC_BOARDTYPE == 0x04)
#		define	ETH_W5100				1	
#		define	COMMS_MODEL				0x01
#		define	BOARD_MODEL				0x02
#		define VNET_MEDIA1_ENABLE  		1
#		define VNET_MEDIA2_ENABLE  		0
#		define VNET_MEDIA3_ENABLE  		0
#		define VNET_MEDIA4_ENABLE  		0
#		define VNET_MEDIA5_ENABLE  		0
#	elif(QC_BOARDTYPE == 0x05)
#		define	ETH_ENC28J60			1	
#		define	COMMS_MODEL				0x02
#		define	BOARD_MODEL				0x02
#		define VNET_MEDIA1_ENABLE 		1
#		define VNET_MEDIA2_ENABLE  		0
#		define VNET_MEDIA3_ENABLE  		0
#		define VNET_MEDIA4_ENABLE  		0
#		define VNET_MEDIA5_ENABLE  		0
#	elif(QC_BOARDTYPE == 0x06)
#		define	ETH_ENC28J60			1
#		define	COMMS_MODEL				0x02
#		define	BOARD_MODEL				0x03
#		define VNET_MEDIA1_ENABLE  		1
#		define VNET_MEDIA2_ENABLE  		0
#		define VNET_MEDIA3_ENABLE  		0
#		define VNET_MEDIA4_ENABLE  		0
#		define VNET_MEDIA5_ENABLE  		0
#	elif(QC_BOARDTYPE == 0x07)
#		define	ETH_ENC28J60			1	
#		define	COMMS_MODEL				0x04
#		define	BOARD_MODEL				0x04
#		define VNET_MEDIA1_ENABLE  		1
#		define VNET_MEDIA2_ENABLE  		0
#		define VNET_MEDIA3_ENABLE  		0
#		define VNET_MEDIA4_ENABLE  		0
#		define VNET_MEDIA5_ENABLE  		0
#	elif(QC_BOARDTYPE == 0x08)
#		define	ETH_ENC28J60			1	
#		define	COMMS_MODEL				0x04
#		define	BOARD_MODEL				0x05
#		define VNET_MEDIA1_ENABLE  		1
#		define VNET_MEDIA2_ENABLE  		0
#		define VNET_MEDIA3_ENABLE  		0
#		define VNET_MEDIA4_ENABLE  		0
#		define VNET_MEDIA5_ENABLE  		0
#	elif(QC_BOARDTYPE == 0x09)
#		define	ETH_ENC28J60			1	
#		define	COMMS_MODEL				0x04
#		define	BOARD_MODEL				0x06
#		define VNET_MEDIA1_ENABLE  		1
#		define VNET_MEDIA2_ENABLE  		0
#		define VNET_MEDIA3_ENABLE  		0
#		define VNET_MEDIA4_ENABLE  		0
#		define VNET_MEDIA5_ENABLE  		0
#	elif(QC_BOARDTYPE == 0x0A)
#		define	WIFI_MRF24				1	
#		define	COMMS_MODEL				0x05
#		define	BOARD_MODEL				0x04
#		define VNET_MEDIA1_ENABLE  		1
#		define VNET_MEDIA2_ENABLE  		0
#		define VNET_MEDIA3_ENABLE  		0
#		define VNET_MEDIA4_ENABLE  		0
#		define VNET_MEDIA5_ENABLE  		0
#	elif(QC_BOARDTYPE == 0x0B)
#		define	WIFI_MRF24				1
#		define	COMMS_MODEL				0x05
#		define	BOARD_MODEL				0x05
#		define VNET_MEDIA1_ENABLE  		1
#		define VNET_MEDIA2_ENABLE  		0
#		define VNET_MEDIA3_ENABLE  		0
#		define VNET_MEDIA4_ENABLE  		0
#		define VNET_MEDIA5_ENABLE  		0
#	elif(QC_BOARDTYPE == 0x0C)
#		define	WIFI_MRF24				1	
#		define	COMMS_MODEL				0x05																
#		define	BOARD_MODEL				0x06
#		define VNET_MEDIA1_ENABLE  		1
#		define VNET_MEDIA2_ENABLE  		0
#		define VNET_MEDIA3_ENABLE  		0
#		define VNET_MEDIA4_ENABLE  		0
#		define VNET_MEDIA5_ENABLE  		0
#	elif(QC_BOARDTYPE == 0x0D)
#		define	ETH_ENC28J60			1	
#		define	COMMS_MODEL				0x04
#		define	BOARD_MODEL				0x04
#		define	IOBOARD_MODEL			0x01
#		define VNET_MEDIA1_ENABLE  		1
#		define VNET_MEDIA2_ENABLE  		0
#		define VNET_MEDIA3_ENABLE  		0
#		define VNET_MEDIA4_ENABLE  		0
#		define VNET_MEDIA5_ENABLE  		0
#	elif(QC_BOARDTYPE == 0x0E)
#		define	ETH_ENC28J60			1	
#		define	COMMS_MODEL				0x04
#		define	BOARD_MODEL				0x05
#		define	IOBOARD_MODEL			0x01
#		define VNET_MEDIA1_ENABLE  		1
#		define VNET_MEDIA2_ENABLE  		0
#		define VNET_MEDIA3_ENABLE  		0
#		define VNET_MEDIA4_ENABLE  		0
#		define VNET_MEDIA5_ENABLE  		0
#	elif(QC_BOARDTYPE == 0x0F)
#		define	ETH_ENC28J60			1	
#		define	COMMS_MODEL				0x04
#		define	BOARD_MODEL				0x06
#		define	IOBOARD_MODEL			0x01
#		define VNET_MEDIA1_ENABLE  		1
#		define VNET_MEDIA2_ENABLE  		0
#		define VNET_MEDIA3_ENABLE  		0
#		define VNET_MEDIA4_ENABLE  		0
#		define VNET_MEDIA5_ENABLE  		0
#	elif(QC_BOARDTYPE == 0x10)
#		define	WIFI_MRF24				1	
#		define	COMMS_MODEL				0x05
#		define	BOARD_MODEL				0x04
#		define	IOBOARD_MODEL			0x01
#		define VNET_MEDIA1_ENABLE  		1
#		define VNET_MEDIA2_ENABLE  		0
#		define VNET_MEDIA3_ENABLE  		0
#		define VNET_MEDIA4_ENABLE  		0
#		define VNET_MEDIA5_ENABLE  		0
#	elif(QC_BOARDTYPE == 0x11)
#		define	WIFI_MRF24				1
#		define	COMMS_MODEL				0x05
#		define	BOARD_MODEL				0x05
#		define	IOBOARD_MODEL			0x01
#		define VNET_MEDIA1_ENABLE  		1
#		define VNET_MEDIA2_ENABLE  		0
#		define VNET_MEDIA3_ENABLE  		0
#		define VNET_MEDIA4_ENABLE  		0
#		define VNET_MEDIA5_ENABLE  		0
#	elif(QC_BOARDTYPE == 0x12)
#		define	WIFI_MRF24				1	
#		define	COMMS_MODEL				0x05																
#		define	BOARD_MODEL				0x06
#		define	IOBOARD_MODEL			0x01
#		define VNET_MEDIA1_ENABLE  		1
#		define VNET_MEDIA2_ENABLE  		0
#		define VNET_MEDIA3_ENABLE  		0
#		define VNET_MEDIA4_ENABLE  		0
#		define VNET_MEDIA5_ENABLE  		0
#	elif(QC_BOARDTYPE == 0x13)
#		define	ETH_ENC28J60			1	
#		define	COMMS_MODEL				0x04
#		define	BOARD_MODEL				0x04
#		define	IOBOARD_MODEL			0x02
#		define VNET_MEDIA1_ENABLE  		1
#		define VNET_MEDIA2_ENABLE  		0
#		define VNET_MEDIA3_ENABLE  		0
#		define VNET_MEDIA4_ENABLE  		0
#		define VNET_MEDIA5_ENABLE  		0
#	elif(QC_BOARDTYPE == 0x14)
#		define	ETH_ENC28J60			1	
#		define	COMMS_MODEL				0x04
#		define	BOARD_MODEL				0x05
#		define	IOBOARD_MODEL			0x02
#		define VNET_MEDIA1_ENABLE  		1
#		define VNET_MEDIA2_ENABLE  		0
#		define VNET_MEDIA3_ENABLE  		0
#		define VNET_MEDIA4_ENABLE  		0
#		define VNET_MEDIA5_ENABLE  		0
#	elif(QC_BOARDTYPE == 0x15)
#		define	ETH_ENC28J60			1	
#		define	COMMS_MODEL				0x04
#		define	BOARD_MODEL				0x06
#		define	IOBOARD_MODEL			0x02
#		define VNET_MEDIA1_ENABLE  		1
#		define VNET_MEDIA2_ENABLE  		0
#		define VNET_MEDIA3_ENABLE  		0
#		define VNET_MEDIA4_ENABLE  		0
#		define VNET_MEDIA5_ENABLE  		0
#	elif(QC_BOARDTYPE == 0x16)
#		define	WIFI_MRF24				1	
#		define	COMMS_MODEL				0x05
#		define	BOARD_MODEL				0x04
#		define	IOBOARD_MODEL			0x02
#		define VNET_MEDIA1_ENABLE  		1
#		define VNET_MEDIA2_ENABLE  		0
#		define VNET_MEDIA3_ENABLE  		0
#		define VNET_MEDIA4_ENABLE  		0
#		define VNET_MEDIA5_ENABLE  		0
#	elif(QC_BOARDTYPE == 0x17)
#		define	WIFI_MRF24				1
#		define	COMMS_MODEL				0x05
#		define	BOARD_MODEL				0x05
#		define	IOBOARD_MODEL			0x02
#		define VNET_MEDIA1_ENABLE  		1
#		define VNET_MEDIA2_ENABLE  		0
#		define VNET_MEDIA3_ENABLE  		0
#		define VNET_MEDIA4_ENABLE  		0
#		define VNET_MEDIA5_ENABLE  		0
#	elif(QC_BOARDTYPE == 0x18)
#		define	WIFI_MRF24				1	
#		define	COMMS_MODEL				0x05																
#		define	BOARD_MODEL				0x06
#		define	IOBOARD_MODEL			0x02
#		define VNET_MEDIA1_ENABLE  		1
#		define VNET_MEDIA2_ENABLE  		0
#		define VNET_MEDIA3_ENABLE  		0
#		define VNET_MEDIA4_ENABLE  		0
#		define VNET_MEDIA5_ENABLE  		0
#	elif(QC_BOARDTYPE == 0x19)
#		define	ETH_ENC28J60			1	
#		define	COMMS_MODEL				0x04
#		define	BOARD_MODEL				0x04
#		define	IOBOARD_MODEL			0x03
#		define VNET_MEDIA1_ENABLE  		1
#		define VNET_MEDIA2_ENABLE  		0
#		define VNET_MEDIA3_ENABLE  		0
#		define VNET_MEDIA4_ENABLE  		0
#		define VNET_MEDIA5_ENABLE  		0
#	elif(QC_BOARDTYPE == 0x1A)
#		define	ETH_ENC28J60			1	
#		define	COMMS_MODEL				0x04
#		define	BOARD_MODEL				0x05
#		define	IOBOARD_MODEL			0x03
#		define VNET_MEDIA1_ENABLE  		1
#		define VNET_MEDIA2_ENABLE  		0
#		define VNET_MEDIA3_ENABLE  		0
#		define VNET_MEDIA4_ENABLE  		0
#		define VNET_MEDIA5_ENABLE  		0
#	elif(QC_BOARDTYPE == 0x1B)
#		define	ETH_ENC28J60			1	
#		define	COMMS_MODEL				0x04
#		define	BOARD_MODEL				0x06
#		define	IOBOARD_MODEL			0x03
#		define VNET_MEDIA1_ENABLE  		1
#		define VNET_MEDIA2_ENABLE  		0
#		define VNET_MEDIA3_ENABLE  		0
#		define VNET_MEDIA4_ENABLE  		0
#		define VNET_MEDIA5_ENABLE  		0
#	elif(QC_BOARDTYPE == 0x1C)
#		define	WIFI_MRF24				1	
#		define	COMMS_MODEL				0x05
#		define	BOARD_MODEL				0x04
#		define	IOBOARD_MODEL			0x03
#		define VNET_MEDIA1_ENABLE  		1
#		define VNET_MEDIA2_ENABLE  		0
#		define VNET_MEDIA3_ENABLE  		0
#		define VNET_MEDIA4_ENABLE  		0
#		define VNET_MEDIA5_ENABLE  		0
#	elif(QC_BOARDTYPE == 0x1D)
#		define	WIFI_MRF24				1
#		define	COMMS_MODEL				0x05
#		define	BOARD_MODEL				0x05
#		define	IOBOARD_MODEL			0x03
#		define VNET_MEDIA1_ENABLE  		1
#		define VNET_MEDIA2_ENABLE  		0
#		define VNET_MEDIA3_ENABLE  		0
#		define VNET_MEDIA4_ENABLE  		0
#		define VNET_MEDIA5_ENABLE  		0
#	elif(QC_BOARDTYPE == 0x1E)
#		define	WIFI_MRF24				1	
#		define	COMMS_MODEL				0x05																
#		define	BOARD_MODEL				0x06
#		define	IOBOARD_MODEL			0x03
#		define VNET_MEDIA1_ENABLE  		1
#		define VNET_MEDIA2_ENABLE  		0
#		define VNET_MEDIA3_ENABLE  		0
#		define VNET_MEDIA4_ENABLE  		0
#		define VNET_MEDIA5_ENABLE  		0
#	elif(QC_BOARDTYPE == 0x20)
#		define	ETH_W5200				1	
#		define	COMMS_MODEL				0x01
#		define	BOARD_MODEL				0x08
#		define VNET_MEDIA1_ENABLE  		1
#		define VNET_MEDIA2_ENABLE  		0
#		define VNET_MEDIA3_ENABLE  		0
#		define VNET_MEDIA4_ENABLE  		0
#		define VNET_MEDIA5_ENABLE  		0
#	elif(QC_BOARDTYPE == 0x21)
#		define	ETH_W5200				1	
#		define	COMMS_MODEL				0x01
#		define	BOARD_MODEL				0x08
#		define 	VNET_SUPERNODE  		1
#		define VNET_MEDIA1_ENABLE  		1
#		define VNET_MEDIA2_ENABLE  		0
#		define VNET_MEDIA3_ENABLE  		0
#		define VNET_MEDIA4_ENABLE  		0
#		define VNET_MEDIA5_ENABLE  		1
#	elif(QC_BOARDTYPE == 0x22)
#		define	ETH_W5200				1	
#		define	COMMS_MODEL				0x01
#		define	BOARD_MODEL				0x08
#		define VNET_MEDIA1_ENABLE  		0
#		define VNET_MEDIA2_ENABLE  		0
#		define VNET_MEDIA3_ENABLE  		0
#		define VNET_MEDIA4_ENABLE  		0
#		define VNET_MEDIA5_ENABLE  		1
#	elif(QC_BOARDTYPE == 0x30)
#		define	ETH_W5100				1	
#		define	COMMS_MODEL				0x01
#		define	BOARD_MODEL				0x07
#		define VNET_MEDIA1_ENABLE  		1
#		define VNET_MEDIA2_ENABLE  		0
#		define VNET_MEDIA3_ENABLE  		0
#		define VNET_MEDIA4_ENABLE  		0
#		define VNET_MEDIA5_ENABLE  		0
#	elif(QC_BOARDTYPE == 0x31)
#		define	ETH_W5100				1	
#		define	COMMS_MODEL				0x01
#		define	BOARD_MODEL				0x02
#		define VNET_MEDIA1_ENABLE  		1
#		define VNET_MEDIA2_ENABLE  		0
#		define VNET_MEDIA3_ENABLE  		0
#		define VNET_MEDIA4_ENABLE  		0
#		define VNET_MEDIA5_ENABLE  		0
#	elif(QC_BOARDTYPE == 0x40)
#		define	ETH_ENC28J60			1	
#		define	COMMS_MODEL				0x02
#		define	BOARD_MODEL				0x01
#		define	CHIBI_AT86RF230			1
#		define 	VNET_SUPERNODE  		1
#		define VNET_MEDIA1_ENABLE  		1
#		define VNET_MEDIA2_ENABLE  		1
#		define VNET_MEDIA3_ENABLE  		0
#		define VNET_MEDIA4_ENABLE  		0
#		define VNET_MEDIA5_ENABLE  		0
#	elif(QC_BOARDTYPE == 0x41)
#		define	ETH_W5100				1	
#		define	COMMS_MODEL				0x01
#		define	BOARD_MODEL				0x02
#		define 	VNET_SUPERNODE  		1
#		define VNET_MEDIA1_ENABLE  		1
#		define VNET_MEDIA2_ENABLE  		0
#		define VNET_MEDIA3_ENABLE  		0
#		define VNET_MEDIA4_ENABLE  		0
#		define VNET_MEDIA5_ENABLE  		1
#	elif(QC_BOARDTYPE == 0x42)
#		define	ETH_ENC28J60			1	
#		define	COMMS_MODEL				0x01
#		define	BOARD_MODEL				0x02
#		define 	VNET_SUPERNODE  		1
#		define VNET_MEDIA1_ENABLE  		1
#		define VNET_MEDIA2_ENABLE  		0
#		define VNET_MEDIA3_ENABLE  		0
#		define VNET_MEDIA4_ENABLE  		0
#		define VNET_MEDIA5_ENABLE  		1
#	elif(QC_BOARDTYPE == 0x43)	
#		define	COMMS_MODEL				0x01
#		define	BOARD_MODEL				0x02
#		define VNET_MEDIA1_ENABLE  		0
#		define VNET_MEDIA2_ENABLE  		0
#		define VNET_MEDIA3_ENABLE  		0
#		define VNET_MEDIA4_ENABLE  		0
#		define VNET_MEDIA5_ENABLE  		1
#	elif(QC_BOARDTYPE == 0x44)
#		define	ETH_W5100				1
#		define	NRF24					1	
#		define	COMMS_MODEL				0x01
#		define	BOARD_MODEL				0x02
#		define 	VNET_SUPERNODE  		1
#		define VNET_MEDIA1_ENABLE  		1
#		define VNET_MEDIA2_ENABLE  		1
#		define VNET_MEDIA3_ENABLE  		0
#		define VNET_MEDIA4_ENABLE  		0
#		define VNET_MEDIA5_ENABLE  		0
#	elif(QC_BOARDTYPE == 0x45)
#		define	ETH_ENC28J60			1	
#		define	NRF24					1	
#		define	COMMS_MODEL				0x01
#		define	BOARD_MODEL				0x02
#		define 	VNET_SUPERNODE  		1
#		define VNET_MEDIA1_ENABLE  		1
#		define VNET_MEDIA2_ENABLE  		1
#		define VNET_MEDIA3_ENABLE  		0
#		define VNET_MEDIA4_ENABLE  		0
#		define VNET_MEDIA5_ENABLE  		0
#	elif(QC_BOARDTYPE == 0x46)	
#		define	COMMS_MODEL				0x06
#		define	BOARD_MODEL				0x02
#		define	NRF24					1	
#		define VNET_MEDIA1_ENABLE  		0
#		define VNET_MEDIA2_ENABLE  		1
#		define VNET_MEDIA3_ENABLE  		0
#		define VNET_MEDIA4_ENABLE  		0
#		define VNET_MEDIA5_ENABLE  		0
#	elif(QC_BOARDTYPE == 0x47)	
#		define	COMMS_MODEL				0x06
#		define	BOARD_MODEL				0x02
#		define	NRF24					1	
#		define 	VNET_SUPERNODE  		1
#		define VNET_MEDIA1_ENABLE  		0
#		define VNET_MEDIA2_ENABLE  		1
#		define VNET_MEDIA3_ENABLE  		0
#		define VNET_MEDIA4_ENABLE  		0
#		define VNET_MEDIA5_ENABLE  		0
#	elif(QC_BOARDTYPE == 0x48)
#		define	ETH_W5200				1	
#		define	COMMS_MODEL				0x01
#		define	BOARD_MODEL				0x02
#		define VNET_MEDIA1_ENABLE  		1
#		define VNET_MEDIA2_ENABLE  		0
#		define VNET_MEDIA3_ENABLE  		0
#		define VNET_MEDIA4_ENABLE  		0
#		define VNET_MEDIA5_ENABLE  		0
#	elif(QC_BOARDTYPE == 0x49)
#		define	ETH_W5500				1	
#		define	COMMS_MODEL				0x01
#		define	BOARD_MODEL				0x02
#		define VNET_MEDIA1_ENABLE  		1
#		define VNET_MEDIA2_ENABLE  		0
#		define VNET_MEDIA3_ENABLE  		0
#		define VNET_MEDIA4_ENABLE  		0
#		define VNET_MEDIA5_ENABLE  		0
#	elif(QC_BOARDTYPE == 0x50)
#		define	WIFI_LPT200				1	
#		define	COMMS_MODEL				0x09
#		define	BOARD_MODEL				0x07
#		define	IOBOARD_MODEL			0x04
#		define VNET_MEDIA1_ENABLE  		1
#		define VNET_MEDIA2_ENABLE  		0
#		define VNET_MEDIA3_ENABLE  		0
#		define VNET_MEDIA4_ENABLE  		0
#		define VNET_MEDIA5_ENABLE  		0
#	elif(QC_BOARDTYPE == 0x51)
#		define	ETH_W5100				1
#		define	COMMS_MODEL				0x01
#		define	BOARD_MODEL				0x07
#		define	IOBOARD_MODEL			0x04
#		define VNET_MEDIA1_ENABLE  		1
#		define VNET_MEDIA2_ENABLE  		0
#		define VNET_MEDIA3_ENABLE  		0
#		define VNET_MEDIA4_ENABLE  		0
#		define VNET_MEDIA5_ENABLE  		0
#	elif(QC_BOARDTYPE == 0x60)
#		define	ETH_W5100				1
#		define	HOPERF_RFM69			1	
#		define	COMMS_MODEL				0x08
#		define	BOARD_MODEL				0x0A
#		define 	VNET_SUPERNODE  		1
#		define VNET_MEDIA1_ENABLE  		1
#		define VNET_MEDIA2_ENABLE  		1
#		define VNET_MEDIA3_ENABLE  		0
#		define VNET_MEDIA4_ENABLE  		0
#		define VNET_MEDIA5_ENABLE  		0
#	elif(QC_BOARDTYPE == 0x61)
#		define	ETH_ENC28J60			1	
#		define	HOPERF_RFM69			1	
#		define	COMMS_MODEL				0x08
#		define	BOARD_MODEL				0x0A
#		define 	VNET_SUPERNODE  		1
#		define VNET_MEDIA1_ENABLE  		1
#		define VNET_MEDIA2_ENABLE  		1
#		define VNET_MEDIA3_ENABLE  		0
#		define VNET_MEDIA4_ENABLE  		0
#		define VNET_MEDIA5_ENABLE  		0
#	elif(QC_BOARDTYPE == 0x62)	
#		define	COMMS_MODEL				0x08
#		define	BOARD_MODEL				0x0A
#		define	HOPERF_RFM69			1	
#		define VNET_MEDIA1_ENABLE  		0
#		define VNET_MEDIA2_ENABLE  		1
#		define VNET_MEDIA3_ENABLE  		0
#		define VNET_MEDIA4_ENABLE  		0
#		define VNET_MEDIA5_ENABLE  		0
#	elif(QC_BOARDTYPE == 0x63)	
#		define	COMMS_MODEL				0x08
#		define	BOARD_MODEL				0x0A
#		define	HOPERF_RFM69			1	
#		define 	VNET_SUPERNODE  		1
#		define VNET_MEDIA1_ENABLE  		0
#		define VNET_MEDIA2_ENABLE  		1
#		define VNET_MEDIA3_ENABLE  		0
#		define VNET_MEDIA4_ENABLE  		0
#		define VNET_MEDIA5_ENABLE  		0
#	endif

#	if  (QC_GATEWAYTYPE == 0x00)
#		define	MaCaco_USERMODE			0
#		define MaCaco_PERSISTANCE		0
#		define MaCaco_SUBSCRIBERS		0
#	elif(QC_GATEWAYTYPE == 0x01)
#		define	MaCaco_USERMODE			1
#		define MaCaco_SUBSCRIBERS		1
#		define MaCaco_PERSISTANCE		0
#		define 	VNET_SUPERNODE  		1
#	elif(QC_GATEWAYTYPE == 0x02)
#		define	MaCaco_USERMODE			1
#		define MaCaco_SUBSCRIBERS		1
#		define MaCaco_PERSISTANCE		1
#		define 	VNET_SUPERNODE  		1
#	elif(QC_GATEWAYTYPE == 0x03)
#		define	MaCaco_USERMODE			1
#		define MaCaco_SUBSCRIBERS		1
#		define 	ARDUINO_ETHLIB			1
#		define	ARDUINO_DHCP			1
#		define	ARDUINO_DNS				1
#		define MaCaco_PERSISTANCE		0
#		define 	VNET_SUPERNODE  		1
#	elif(QC_GATEWAYTYPE == 0x04)
#		define	MaCaco_USERMODE			1
#		define MaCaco_SUBSCRIBERS		1
#		define 	ARDUINO_ETHLIB			1
#		define	ARDUINO_DHCP			1
#		define	ARDUINO_DNS				1
#		define MaCaco_PERSISTANCE		1
#		define 	VNET_SUPERNODE  		1
#	elif(QC_GATEWAYTYPE == 0x05)
#		define	MaCaco_USERMODE			1
#		define MaCaco_SUBSCRIBERS		1
#		define MaCaco_LASTIN			1
#		define 	VNET_SUPERNODE  		1
#	elif(QC_GATEWAYTYPE == 0x06)
#		define	MaCaco_USERMODE			1
#		define MaCaco_SUBSCRIBERS		1
#		define 	ARDUINO_ETHLIB			1
#		define	ARDUINO_DHCP			1
#		define	ARDUINO_DNS				1
#		define MaCaco_LASTIN			1
#		define 	VNET_SUPERNODE  		1
#	endif

#	if  (QC_INTERFACE == 0x00)
#	elif(QC_INTERFACE == 0x01)
#		define	HTTPSERVER				1
#	elif(QC_INTERFACE == 0x02)
#		define	XMLSERVER				1
#	elif(QC_INTERFACE == 0x03)
#		define	XMLSERVER				2
#	elif(QC_INTERFACE == 0x04)
#		define	MODBUS					1
#		define 	MODBUS_TCP  			1
#		define 	MODBUS_RTU  			0
#	elif(QC_INTERFACE == 0x05)
#		define	MODBUS					1
#		define 	MODBUS_TCP  			0
#		define 	MODBUS_RTU  			1
#	endif

// In case of DYNAMIC ADDRESSING if the IP BROADCAST option is set, data between nodes will
// flow over Media3 as IP broadcast. Only the Gateway node will be able to process unicast vNet
// frames.
// This option remove the need of a bind between vNet and IP addresses, this is useful when a 
// static IP address could not be applied and there is not enough RAM to run a DHCP to get an IP
// address.
#	if  (MaCaco_USERMODE && VNET_MEDIA1_ENABLE && DYNAMICADDRESSING && IPBROADCAST)
#		define VNET_MEDIA3_ENABLE  		1
#	elif(VNET_MEDIA1_ENABLE && DYNAMICADDRESSING && IPBROADCAST)
#		define VNET_MEDIA1_ENABLE  		0
#		define VNET_MEDIA3_ENABLE  		1
#	else
#		define VNET_MEDIA3_ENABLE  		0
#	endif

#	include "conf/frame/MaCacoCfg.h"
#	include "conf/uIP/uIPopt.h"
#	include "conf/chibi/chibiUsrCfg.h"
#	include "conf/nRF24/nRF24UsrCfg.h"
#	include "conf/RFM69/RFM69UsrCfg.h"
#	include "conf/usart/usartUsrCfg.h"												
#	include "conf/eth/ethUsrCfg.h"
#	include "conf/hardware/hwBoards.h"
#	include "conf/frame/SoulissCfg.h"
#	include "conf/frame/vNetCfg.h"

	const U16 vnet_media_en[VNET_MEDIA_NUMBER] = {VNET_MEDIA1_ENABLE,  // Media 1
												VNET_MEDIA2_ENABLE,  // Media 2
												VNET_MEDIA3_ENABLE,  // Media 3
												VNET_MEDIA4_ENABLE,  // Media 4
												VNET_MEDIA5_ENABLE   // Media 5	
												};

												
#else			// If Detailed Configuration is enabled
	
#	include "conf/frame/MaCacoCfg.h"
#	include "conf/uIP/uIPopt.h"
#	include "conf/chibi/chibiUsrCfg.h"
#	include "conf/nRF24/nRF24UsrCfg.h"
#	include "conf/RFM69/RFM69UsrCfg.h"
#	include "conf/usart/usartUsrCfg.h"												
#	include "conf/eth/ethUsrCfg.h"
#	include "conf/hardware/hwBoards.h"
#	include "conf/frame/SoulissCfg.h"
#	include "conf/frame/vNetCfg.h"	
	
#endif

#endif