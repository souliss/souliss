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
#ifndef CONTROLLINOMAXI
#define CONTROLLINOMAXI

#define D0 		2
#define D1 		3
#define D2 		4
#define D3 		5
#define D4 		6
#define D5 		7
#define D6 		8
#define D7 		9
#define D8 		10
#define D9 		11
#define D10 	12
#define D11 	13

#define A0 		54
#define A1 		55
#define A2 		56
#define A3 		57
#define A4 		58
#define A5 		59
#define A6 		60
#define A7 		61
#define A8 		62
#define A9 		63
#define IN0 	18
#define IN1 	19

#define MINUS 	14
#define PLUS 	15

#define R0 		22
#define R1 		23
#define R2 		24
#define R3 		25
#define R4 		26
#define R5 		27
#define R6 		28
#define R7 		29
#define R8 		30
#define R9 		31

// Set the Reset and PWDN pins in DINo version 2
#define	InitControllino()	pinMode(2, OUTPUT);					\
							pinMode(3, OUTPUT);					\
							pinMode(4, OUTPUT);					\
							pinMode(5, OUTPUT);					\
							pinMode(6, OUTPUT);					\
							pinMode(7, OUTPUT);					\
							pinMode(8, OUTPUT);					\
							pinMode(9, OUTPUT);					\
							pinMode(10, OUTPUT);				\
							pinMode(11, OUTPUT);				\
							pinMode(12, OUTPUT);				\
                            pinMode(13, OUTPUT);				\
							pinMode(18, INPUT);					\
                            pinMode(19, INPUT);					\
							pinMode(22, OUTPUT);				\
							pinMode(23, OUTPUT);				\
							pinMode(24, OUTPUT);				\
							pinMode(25, OUTPUT);				\
							pinMode(26, OUTPUT);				\
							pinMode(27, OUTPUT);				\
							pinMode(28, OUTPUT);				\
							pinMode(29, OUTPUT);				\
							pinMode(30, OUTPUT);				\
							pinMode(31, OUTPUT);				\
							delay(1000)	
							
#endif
