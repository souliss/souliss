/**************************************************************************
	Souliss 
    Copyright (C) 2014  Veseo

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

// Include configuration files
#include "conf/frame/MaCacoCfg.h"
#include "conf/uIP/uIPopt.h"
#include "conf/chibi/chibiUsrCfg.h"
#include "conf/nRF24/nRF24UsrCfg.h"
#include "conf/RFM69/RFM69UsrCfg.h"
#include "conf/usart/usartUsrCfg.h"												
#include "conf/eth/ethUsrCfg.h"
#include "conf/hardware/hwBoards.h"
#include "conf/frame/SoulissCfg.h"
#include "conf/frame/vNetCfg.h"
#include "conf/cores/cores.h"
#include "user/user_config.h"

// Define to zero if not used
#ifndef	VNET_MEDIA1_ENABLE
#define	VNET_MEDIA1_ENABLE	0
#endif

#ifndef	VNET_MEDIA2_ENABLE
#define	VNET_MEDIA2_ENABLE	0
#endif

#ifndef	VNET_MEDIA3_ENABLE
#define	VNET_MEDIA3_ENABLE	0
#endif

#ifndef	VNET_MEDIA4_ENABLE
#define	VNET_MEDIA4_ENABLE	0
#endif

#ifndef	VNET_MEDIA5_ENABLE
#define	VNET_MEDIA5_ENABLE	0
#endif

const U16 vnet_media_en[VNET_MEDIA_NUMBER] = {VNET_MEDIA1_ENABLE,  // Media 1
												VNET_MEDIA2_ENABLE,  // Media 2
												VNET_MEDIA3_ENABLE,  // Media 3
												VNET_MEDIA4_ENABLE,  // Media 4
												VNET_MEDIA5_ENABLE   // Media 5	
												};
												
#endif