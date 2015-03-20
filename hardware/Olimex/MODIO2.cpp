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

#include "MODIO2.h"
#include "src/I2C/I2C.c"

// Define the I2C data buffer
uint8_t	mIO_cmd[7] = {OLIMEX, 0, ADDRESS, 0, 0, 0};

/**************************************************************************
/*!
	Init the MOD-IO2 driver for model 02	(2 relays, 7 GPIOs)	
*/	
/**************************************************************************/
void MODIO_Init()
{
	// Init the I2C
	I2C_begin();	
	
	mIO_cmd[ADDR0] = OLIMEX;				// I2C Slave Address (aka ADDR0)
	mIO_cmd[ADDR1] = MODIO2;	            // I2C Register		 (aka ADDR1)
	mIO_cmd[ADDR2] = ADDRESS;	            // I2C Register		 (aka ADDR2)
}

/**************************************************************************
/*!
	Set GPIO pin modes for MOD-IO2
	
		Pins associated to a 0 are set as outputs rather 1 set as input.
	
		Is not allowed to read the register value, so configuration have to 
		be done for all pins in one shot, use GPIO0...GPIO7 defines to get 
		the register value.
		
		Example, set GPIO0/1 as inputs and GPIO6/7 as outputs
			gpio_register = (~(GPIO7 | GPIO6) & (GPIO1 | GPIO0))
*/	
/**************************************************************************/
void mIO_PinMode(uint8_t gpio_register)			
{
	// Addresses was set previously, only require functional code and
	// command	
	mIO_cmd[FUNCODE] = SET_TRIS;				// Set TRIS register in MOD-IO2	
	mIO_cmd[CMD]	 = gpio_register;			// Register value
	
	// Write data on I2C bus
	I2C_write(mIO_cmd, 5);
	
}

/**************************************************************************
/*!
	Set GPIO pullups for MOD-IO2
	
		Pins associated to a 0 has no pullps rather 1 set has it.
	
		Is not allowed to read the register value, so configuration have to 
		be done for all pins in one shot, use GPIO0...GPIO7 defines to get 
		the register value.
		
		Example, set pullup for GPIO0/1 and remove for all others
			gpio_register = (~(GPIO7 | GPIO6 | GPIO5 | GPIO4 
									 | GPIO3 | GPIO2) & (GPIO1 | GPIO0))
*/	
/**************************************************************************/
void mIO_Pullup(uint8_t gpio_register)				
{
	// Addresses was set previously, only require functional code and
	// command	
	mIO_cmd[FUNCODE] = SET_PU;				// Set pullup register
	mIO_cmd[CMD]	 = gpio_register;		// Register value

	// Write data on I2C bus	
	I2C_write(mIO_cmd, 5);	
	
}

/**************************************************************************
/*!
	Set relay state for MOD-IO2
	
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
	I2C_write(mIO_cmd, 5);				
}									

/**************************************************************************
/*!
	Set digital output for MOD-IO2
	
		Set or reset digital output state. Is not allowed to read the register
		value, so configuration have to be done for all pins in one shot, use 
		GPIO0...7 defines to get the register value.
		
		Example, set digital outputs GPIO0 and GPIO1
			gpio_register = (GPIO1 | GPIO0)
*/	
/**************************************************************************/	
void mIO_digitalWrite(uint8_t gpio_register)		
{
	// Addresses was set previously, only require functional code and
	// command	
	mIO_cmd[FUNCODE] = SET_LAT;				// Set pullup register
	mIO_cmd[CMD]	 = gpio_register;	    // Register value

	// Write data on I2C bus		
	I2C_write(mIO_cmd, 5);				
}

/**************************************************************************
/*!
	Read actual value for analog input
*/	
/**************************************************************************/										
uint16_t mIO_analogRead(uint8_t pin)		
{
	// Addresses was set previously, only require functional code
	mIO_cmd[FUNCODE] = (GET_AN0+pin);	
	I2C_write(mIO_cmd, 4);				// Write I2C read request on the bus
	
	//Prepare the read on the I2C bus
	mIO_cmd[CMD]	 = mIO_cmd[ADDR0];	
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
	mIO_cmd[FUNCODE] = GET_PORT;		
	I2C_write(mIO_cmd, 4);				// Write I2C read request on the bus
	
	//Prepare the read on the I2C bus
	mIO_cmd[CMD]	 = mIO_cmd[ADDR0];	
	I2C_read(&mIO_cmd[CMD], 1);		

	// Return actual pin value
	return (mIO_cmd[CMD] & (pin));	 	
}