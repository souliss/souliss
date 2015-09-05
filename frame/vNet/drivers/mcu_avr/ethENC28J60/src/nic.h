#ifndef __NIC_H__
#define __NIC_H__

/**************************************************************************
	Souliss - vNet Virtualized Network
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
	
	Developed by Dario Di Maio
	
***************************************************************************/
/*!
    \file 
    \ingroup

*/
/**************************************************************************/

/*****************************************************************************
*  Module Name:       NIC Driver Interface for uIP-AVR Port
*  
*  Created By:        Louis Beaudoin (www.embedded-creations.com)
*
*  Original Release:  November 16, 2003 
*
*  Module Description:  
*  Provides three functions to interface with a NIC driver
*  These functions can be called directly from the main uIP control loop
*  to send packets from uip_buf and uip_appbuf, and store incoming packets to
*  uip_buf
*
*
*****************************************************************************/

#include "ENC28J60.h"

#define	nic_init(macaddr)	enc28j60Init(macaddr)
void nic_send(void);
unsigned char nic_poll(void);

#endif
