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
#ifndef USART_CFG_H
#define USART_CFG_H

/**************************************************************************/
/*!
	Select USART Baud Rate for wire communication, lower baud rate help over
	longer distances rather higher ones help in lower collision rate.

	Never select more than one driver per time
	        
		Value       Media
        0x0         Disable (Default)
        0x1         Enable
	
		USART_BAUD9k6		-  USART at 9600   bps
		USART_BAUD19k2		-  USART at 19200  bps
		USART_BAUD115k2		-  USART at 115200 bps
*/
/**************************************************************************/
#define USART_BAUD9k6			1
#define USART_BAUD19k2			0
#define USART_BAUD115k2			0

/**************************************************************************/
/*!
	Select the transmission enable pin of the transceiver (if any)
	        
		Value       Media
        0x0         Disable (Default)
        0x1         Enable
	
		USART_TXENABLE		-  If enabled, the TXENPIN is used before transmitting
		USART_TXENPIN		-  The pin connected to driver
*/
/**************************************************************************/
#define USART_TXENABLE			0
#define USART_TXENPIN			3

/**************************************************************************/
/*!
	If enabled print the header and payload of incoming, outgoing and routed
	frames.
	
        Value       Status
        0x0         Disable (Default)
        0x1         Enable
*/
/**************************************************************************/
#define USART_DEBUG  			0

#endif
							  