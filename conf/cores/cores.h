/**************************************************************************
	Souliss
    Copyright (C) 2015  Veseo

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
#ifndef CORES_H
#define CORES_H

// Actually there is a tight dependency with core releases especially for
// products that gets frequent updates, in some cases we can use the below
// option to runs on special cores.

// By default there is always a single core release supported for each 
// microcontroller class, but some exception can help to handle the time between
// a core release and the relevant distribution of the updates.


/**************************************************************************/
/*!
	ESP8266 Core Selection
	
	Below you can select the core that you have installed in your IDE, 
	having a core different from the latest supported one can result in 
	compile error or unpredicted behavior.

		Value       
        0x0         Disable
        0x1         Enable
*/
/**************************************************************************/
#ifndef ESP8266_CORE_INSKETCH
#	define ESP8266_G39819F0			1
#	define ESP8266_GEF26cCF			0
#endif

#endif