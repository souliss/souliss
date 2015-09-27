/**************************************************************************
	Souliss Support for Iono Relay Board by Sfera Labs
    Copyright (C) 2015  Veseo
	
***************************************************************************/

/**************************************************************************
	Souliss Home Automation
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
#ifndef IONO_H
#define IONO_H

#define DO1  A4
#define DO2  A5
#define DO3  5
#define DO4  6
#define DO5  7
#define DO6  8
            
#define DI1  A0
#define AV1  A0
#define AI1  A0
            
#define DI2  A1
#define AV2  A1
#define AI2  A1
            
#define DI3  A2
#define AV3  A2
#define AI3  A2
            
#define DI4  A3
#define AV4  A3
#define AI4  A3
            
#define DI5  2
#define DI6  3

#define AO1  9


// Set the Reset and PWDN pins in DINo version 2
#define	InitIono()			pinMode(DO1, OUTPUT);	\
							pinMode(DO2, OUTPUT);	\
							pinMode(DO3, OUTPUT);	\
							pinMode(DO4, OUTPUT);	\
							pinMode(DO5, OUTPUT);	\
							pinMode(DO6, OUTPUT);	\
							pinMode(DI1, INPUT);	\
							pinMode(DI2, INPUT);	\
							pinMode(DI3, INPUT);	\
							pinMode(DI4, INPUT);	\
							pinMode(DI5, INPUT);	\
							pinMode(DI6, INPUT);	\
							pinMode(AO1, OUTPUT)
										
#endif
