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
#ifndef HWSENSORS_H
#define HWSENSORS_H

/**************************************************************************/
/*!
	Select the sensor used to include automatically proper drivers, multiple
	selection is allowed.
	        
		Value       Media
        0x00        No selection (Default)
		0x01		Selected
 
	List of available sensors and relevant description,
		
		Name		Description
		DHT			Temperature and humidity DHT11, DHT21, DHT22
 
*/
/**************************************************************************/
#if(!(QC_ENABLE))					// Define manually only in Detailed Configuration Mode
#	define	DHT_SENSOR				0x00
#endif

#endif