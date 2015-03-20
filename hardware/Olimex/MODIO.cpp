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

#include "MODIO.h"
#include "src/I2C/I2C.c"

// Define the I2C data buffer
uint8_t	mIO_cmd[7] = {OLIMEX, 0, ADDRESS, 0, 0, 0};

/**************************************************************************
/*!
	Init the MOD-IO driver for model 01	(4 relays, 4 opto-insulated inputs)
*/	
/**************************************************************************/
void MODIO_Init()
{
	// Init the I2C
	I2C_begin();	
		
	mIO_cmd[ADDR0] = NONE;				// Not used
	mIO_cmd[ADDR1] = NONE;				// Not used
	mIO_cmd[ADDR2] = MODIO1;			// I2C Slave Address
}	

/**************************************************************************
/*!
	Set relay state for MOD-IO
	
		Set or reset relay state. Is not allowed to read the register value, 
		so configuration have to be done for all pins in one shot, use 
		RELAY1...4 defines to get the register value.
		
		Example, set relays RELAY1 and RELAY2 and reset all the others
			relay_reg = (~(RELAY4 | RELAY3) & (RELAY1 | RELAY2))
*/	
/**************************************************************************/									
void mIO_Relay(uint8_t relay_reg)
{
	// Addresses was set previously, only require functional code and
	// command	
	mIO_cmd[FUNCODE] = SET_REL;				// Set pullup register
	mIO_cmd[CMD]	 = relay_reg;			// Register value

	// Write data on I2C bus	
	I2C_write(&mIO_cmd[ADDR2], 3);				
}									

/**************************************************************************
/*!
	Read actual value for analog input
*/	
/**************************************************************************/										
uint16_t mIO_analogRead(uint8_t pin)		
{
	// Addresses was set previously, only require functional code
	mIO_cmd[FUNCODE] = (GET_AN1+pin-1);	
	I2C_write(&mIO_cmd[ADDR2], 2);				// Write I2C read request on the bus
	
	//Prepare the read on the I2C bus
	mIO_cmd[CMD]	 = mIO_cmd[ADDR2];	
	I2C_read(&mIO_cmd[CMD], 2);	

	// Return actual pin value
	return (*(uint16_t*)(&mIO_cmd[CMD])); 	
}

/**************************************************************************
/*!
	Read actual value for digital input
*/	
/**************************************************************************/
uint8_t mIO_digitalRead(uint8_t pin)	
{
	// Addresses was set previously, only require functional code
	mIO_cmd[FUNCODE] = GET_IN;		
	I2C_write(&mIO_cmd[ADDR2], 2);				// Write I2C read request on the bus
	
	//Prepare the read on the I2C bus
	mIO_cmd[CMD]	 = mIO_cmd[ADDR2];	
	I2C_read(&mIO_cmd[CMD], 1);		
	
	// Return actual pin value
	return (mIO_cmd[CMD] & (pin));	 	
}