/**************************************************************************
	Souliss Support for Authometion IoTuino
    Copyright (C) 2014  Veseo
	
***************************************************************************/

/**************************************************************************
	Souliss Home Automation
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
#ifndef IOTUINOUSB_H
#define IOTUINOUSB_H

// Init the Radio and the WiFi module, get the IP address from the module
// and setup Souliss and vNet					
#define	InitIoTuino()		Iotuino.initialize();							\
							Iotuino.setRadioTransmission(18);				\
							lpt200_init();									\
							uint8_t t_ipaddr[4];							\
							getip(t_ipaddr);								\
							Souliss_SetIPAddress(t_ipaddr, 0, 0);			\
							Souliss_SetLocalAddress(memory_map, t_ipaddr[3])
							
					
// Init the Radio			
#define	InitIoTuino_RadioOnly(chipselect)		Iotuino.initialize(chipselect);							\
												Iotuino.setRadioTransmission(18);					
					
#endif
