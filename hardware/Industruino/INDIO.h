/**************************************************************************
	Souliss Support for Industruino PLC
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
#ifndef INDUSTRUINO_INDIO_H
#define INDUSTRUINO_INDIO_H

// Pin mapping

#define DI1		1
#define DI2		2
#define DI3		3
#define DI4		4
#define DI5		5
#define DI6		6
#define DI7		7
#define DI8		8

#define DO1		1
#define DO2		2
#define DO3		3
#define DO4		4
#define DO5		5
#define DO6		6
#define DO7		7
#define DO8		8

#define AI1		A1
#define AI2		A2
#define AI3		A3
#define AI4		A4

#define AO1		A1
#define AO2		A2

// Redefine the LOCAL_IO to use the INDIO
#define	LOCAL_IO
#define	dRead	Indio.digitalRead
#define	dWrite	Indio.digitalWrite
#define	aRead	Indio.analogRead
#define	aWrite	Indio.analogWrite
									
#endif
