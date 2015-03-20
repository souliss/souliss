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
#ifndef bconf_ssnRF24L01_H
#define bconf_ssnRF24L01_H

#define NRF24_INSKETCH
#define VNET_MEDIA_INSKETCH
#define	COMMS_MODEL_INSKETCH
#define	VNET_RESETTIME_INSKETCH

// Refer to chibiUsrCfg.h and vNetCfg.h
#define NRF24						1
#define VNET_MEDIA2_ENABLE  		1
#define	COMMS_MODEL					6

// Some nRF24L01 doesn't work reliably over time, so we lower the maximum time that a 
// node can be inactive before force a reset of the radio
#define	VNET_RESETTIME				0x42F6

#endif
