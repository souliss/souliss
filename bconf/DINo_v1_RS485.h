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
#ifndef bconf_ssDINov1_H
#define bconf_ssDINov1_H

#define	ETH_INSKETCH
#define VNET_MEDIA_INSKETCH
#define	BOARD_MODEL_INSKETCH
#define	COMMS_MODEL_INSKETCH
#define USARTDRIVER_INSKETCH


// Refer to ethUsrCfg.h, vNetCfg.h and hwBoards.h
#define ETH_W5100  					0
#define ETH_W5200  					0
#define ETH_W5500					0
#define ETH_ENC28J60  				1
#define WIFI_MRF24					0
#define WIFI_LPT200					0
#define	COMMS_MODEL					0x02
#define	BOARD_MODEL					0x03
#define VNET_MEDIA5_ENABLE          1
#define USART_TXENABLE              1
#define USART_TXENPIN               3
#define USARTDRIVER                 Serial
#define USART_LOG                   LOG.print



#endif
