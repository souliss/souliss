/**************************************************************************
	Souliss Support for DFRobots XBoard Relay
    Copyright (C) 2012  Veseo
	
***************************************************************************/

/**************************************************************************
	Souliss Home Automation
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
	
	Originally developed by Dario Di Maio
	
***************************************************************************/
/*!
    \file 
    \ingroup
*/
#ifndef XBR_H
#define XBR_H

// XBoard Relay Pinouts
#define REL1			8
#define REL2			7	

#define	SetRelay1()			pinMode(8, OUTPUT);
#define	SetRelay2()			pinMode(7, OUTPUT);

#define	CntRelay1(slot)		ssDigOut(8,Souliss_T1n_Coil,slot)
#define	CntRelay2(slot)		ssDigOut(7,Souliss_T1n_Coil,slot)

#endif
