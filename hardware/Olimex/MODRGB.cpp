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

#include "MODRGB.h"
#include "src/I2C/I2C.c"

// Define the I2C data buffer
uint8_t	mRGB_cmd[7] = {OLIMEX, 0, ADDRESS, 0, 0, 0};

/**************************************************************************
/*!
	Init the MOD-RGB driver 
*/	
/**************************************************************************/
void mRGB_Init()
{
	// Init the I2C
	I2C_begin();	
		
	mRGB_cmd[ADDR0] = OLIMEX;				// I2C Slave Address (aka ADDR0)	
	mRGB_cmd[ADDR1] = MODRGB;				// I2C Register		 (aka ADDR1)
	mRGB_cmd[ADDR2] = ADDRESS;				// I2C Register		 (aka ADDR2)
}	

/**************************************************************************
/*!
	Reset the RGB Controller
*/	
/**************************************************************************/
void mRGB_Reset()			
{
	// Addresses was set previously, only require functional code
	mRGB_cmd[FUNCODE] = STOP_PWM;				// Stop PWM
	
	// Write data on I2C bus
	I2C_write(mRGB_cmd, 4);
	
	// Addresses was set previously, only require functional code
	mRGB_cmd[FUNCODE] = START_PWM;				// Start PWM
	
	// Write data on I2C bus
	I2C_write(mRGB_cmd, 4);
	
}

/**************************************************************************
/*!
	Set the color in RGB, needs a pointer to a 3 byte array with the
	color code
*/	
/**************************************************************************/									
void mRGB_SetColor(uint8_t *color)
{
	// Addresses was set previously, only require functional code and
	// data
	mRGB_cmd[FUNCODE] = SET_RGB;			// Set pullup register
	
	// Load the requested color in RGB format into the command buffer
	for(U8 i=0;i<3;i++)
		mRGB_cmd[DATA1+i]	  = *(color+i);

	// Write data on I2C bus	
	I2C_write(mRGB_cmd, 7);				
}									