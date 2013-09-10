/**************************************************************************
	Souliss Home Automation
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
#ifndef SoulissDHT_H
#define SoulissDHT_H

#include "DHT.h"

#define DHT11 11
#define DHT22 22
#define DHT21 21

#define concat(a, b)				a##b
#define	ssDHT11_Init(pin, id)		ssDHT concat(ssDHT, id)(pin, DHT11)	// ssDHT11_Init(2, 1); became ssDHT ssDHT1(2, DHT11);
#define	ssDHT21_Init(pin, id)		ssDHT concat(ssDHT, id)(pin, DHT21)	
#define	ssDHT22_Init(pin, id)		ssDHT concat(ssDHT, id)(pin, DHT22)	

#define ssDHT_Begin(id)				concat(ssDHT, id).begin()
#define ssDHT_readTemperature(id)	concat(ssDHT, id).readTemperature()
#define ssDHT_readHumidity(id)		concat(ssDHT, id).readHumidity()

#endif
