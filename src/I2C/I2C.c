/**************************************************************************
	Souliss 

	Modified by Dario Di Maio
	
***************************************************************************/
/*!
    \file 
    \ingroup
*/

/*
  I2C.cpp - I2C library
  Copyright (c) 2011-2012 Wayne Truchsess.  All right reserved.
  Rev 5.0 - January 24th, 2012
          - Removed the use of interrupts completely from the library
            so TWI state changes are now polled. 
          - Added calls to lockup() function in most functions 
            to combat arbitration problems 
          - Fixed scan() procedure which left timeouts enabled 
            and set to 80msec after exiting procedure
          - Changed scan() address range back to 0 - 0x7F
          - Removed all Wire legacy functions from library
          - A big thanks to Richard Baldwin for all the testing
            and feedback with debugging bus lockups!
  Rev 4.0 - January 14th, 2012
          - Updated to make compatible with 8MHz clock frequency
  Rev 3.0 - January 9th, 2012
          - Modified library to be compatible with Arduino 1.0
          - Changed argument type from boolean to uint8_t in pullUp(), 
            setSpeed() and receiveByte() functions for 1.0 compatability
          - Modified return values for timeout feature to report
            back where in the transmission the timeout occured.
          - added function scan() to perform a bus scan to find devices
            attached to the I2C bus.  Similar to work done by Todbot
            and Nick Gammon
  Rev 2.0 - September 19th, 2011
          - Added support for timeout function to prevent 
            and recover from bus lockup (thanks to PaulS
            and CrossRoads on the Arduino forum)
          - Changed return type for stop() from void to
            uint8_t to handle timeOut function 
  Rev 1.0 - August 8th, 2011
  
  This is a modified version of the Arduino Wire/TWI 
  library.  Functions were rewritten to provide more functionality
  and also the use of Repeated Start.  Some I2C devices will not
  function correctly without the use of a Repeated Start.  The 
  initial version of this library only supports the Master.


  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "I2C.h"
#include <Arduino.h>

uint8_t 	bufferIndex = 0;
uint8_t 	returnStatus = 0;
uint8_t 	nack = 0;
uint16_t 	timeOutDelay = 0;

/**************************************************************************
/*!
	Init the TWI/I2C interface for the AVR microcontroller, by default use
	the slowest available frequency clock for standards I2C speed
*/	
/**************************************************************************/
void I2C_begin()
{
	// Set prescaler to maximum to lower the clock
	sbi(TWSR, TWPS0);
	sbi(TWSR, TWPS1);
	
	// Set bit rate for standard TWI speed
	TWBR = ((F_CPU / 100000) - 16) / 2;
  
	// Enable TWI module and acks, no use of interrupts
	TWCR = _BV(TWEN) | _BV(TWEA); 
	
	// Set a timeout of 100 milliseconds
	timeOutDelay = 100;
}

/**************************************************************************
/*!
	Free the TWI hardware
*/	
/**************************************************************************/
void I2C_end()
{
	TWCR = 0;
}

/**************************************************************************
/*!
	Set a timeout in case of missing ack from slave
*/	
/**************************************************************************/
void I2C_timeOut(uint16_t timeOut)
{
	timeOutDelay = timeOut;
}

/**************************************************************************
/*!
	Set internal pullps, use this function only if there are no external
	pullups and check the voltages before.
*/	
/**************************************************************************/
void I2C_pullup(uint8_t activate)
{
	if(activate)
	{
	#	if defined(__AVR_ATmega168__) || defined(__AVR_ATmega8__) || defined(__AVR_ATmega328P__)
			sbi(PORTC, 4);
			sbi(PORTC, 5);
	#	elif defined(__AVR_ATmega128__) || (__AVR_ATmega32U4__)
			sbi(PORTD, 0);
			sbi(PORTD, 1);			
	#	endif
	}
	else
	{
	#	if defined(__AVR_ATmega168__) || defined(__AVR_ATmega8__) || defined(__AVR_ATmega328P__)
			cbi(PORTC, 4);
			cbi(PORTC, 5);
	#	elif defined(__AVR_ATmega128__) || (__AVR_ATmega32U4__)
			cbi(PORTD, 0);
			cbi(PORTD, 1);
	#	endif
	}
}

/**************************************************************************
/*!
	Write a stream of data on the I2C bus as write request, first byte of 
	the stream shall be the address.
	
	The return code list is the following:
		1 - Waiting for successful completion of a Start bit
		2 - Waiting for ACK/NACK while addressing slave in transmit mode (MT)
		3 - Waiting for ACK/NACK while sending data to the slave
		4 - Waiting for successful completion of a Repeated Start
		5 - Waiting for ACK/NACK while addressing slave in receiver mode (MR)
		6 - Waiting for ACK/NACK while receiving data from the slave
		7 - Waiting for successful completion of the Stop bit

	All possible return values:
		0           Function executed with no errors
		1 - 7       Timeout occurred, see above list
		8 - 0xFF    See datasheet for exact meaning	
		
*/	
/**************************************************************************/
uint8_t I2C_write(uint8_t *data, uint8_t numberBytes)
{
	bufferIndex = 0;
	returnStatus = 0;
	
	// Send a start bit, return if failed
	returnStatus = start();
	if(returnStatus)
		return(returnStatus);
	
	// Send the first byte, this is the address+W
	returnStatus = sendAddress(SLA_W(data[bufferIndex]));
	
	// If failed, return the error code
	if(returnStatus)
	{
		if(returnStatus == 1)
			return(2);
			
		return(returnStatus);
	}
	
	// Send the data stream
	for (bufferIndex = 1; bufferIndex < numberBytes; bufferIndex++)
	{
		returnStatus = sendByte(data[bufferIndex]);	
		// If failed, return the error code
		if(returnStatus)
		{
			if(returnStatus == 1)
				return(3);
			return(returnStatus);
		}
	}
	
	// Send the stop bit
	returnStatus = stop();
	// If failed, return the error code
	if(returnStatus)
	{
		if(returnStatus == 1)
			return(7);
		return(returnStatus);
	}
	
	// Transmission completed, return status
	return(returnStatus);
}
/**************************************************************************
/*!
	Read data from the I2C bus, first byte in the data buffer shall be the
	address to read from, this will be overwritten once get data.		
*/	
/**************************************************************************/
uint8_t I2C_read(uint8_t *data, uint8_t numberBytes)
{
	bufferIndex = 0;
	returnStatus = 0;
	nack = numberBytes - 1;

	// Send start bit
	returnStatus = start();
	if(returnStatus){return(returnStatus);}
	
	// Write a read request on the I2C bus
	returnStatus = sendAddress(SLA_R(data[bufferIndex]));
	// If failed, return
	if(returnStatus)
	{
		if(returnStatus == 1)
			return(5);
			
		return(returnStatus);
	}
	
	// Read data from the bus
	for(bufferIndex = 0; bufferIndex < numberBytes; bufferIndex++)
	{
		// Send ACK for all bytes out of last
		if( bufferIndex == nack )
		{	
			// Receive data
			returnStatus = receiveByte(0);
			
			// If failed
			if(returnStatus == 1)
				return(6);				
			if(returnStatus != MR_DATA_NACK)
				return(returnStatus);
		}
		else
		{
			// Receive data
			returnStatus = receiveByte(1);
			
			// If failed
			if(returnStatus == 1)
				return(6);
			if(returnStatus != MR_DATA_ACK)
				return(returnStatus);
		}
		
		// Save data into the buffer
		data[bufferIndex] = TWDR;

	}
	
	// Data transfer completed, send a stop bit
	returnStatus = stop();
	
	// Return final state
	if(returnStatus)
	{
		// If failed
		if(returnStatus == 1)
			return(7);
			
		// If successfull	
		return(returnStatus);
	}
	return(returnStatus);
}


uint8_t start()
{
	unsigned long startingTime = millis();
	TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN);
	while (!(TWCR & (1<<TWINT)))
	{
		if(!timeOutDelay){continue;}
		if((millis() - startingTime) >= timeOutDelay)
		{
			lockUp();
			return(1);
		}   
	}
	if ((TWI_STATUS == START) || (TWI_STATUS == REPEATED_START))
	{
		return(0);
	}
	if (TWI_STATUS == LOST_ARBTRTN)
	{
		uint8_t bufferedStatus = TWI_STATUS;
		lockUp();
		return(bufferedStatus);
	}
	return(TWI_STATUS);
}

uint8_t sendAddress(uint8_t i2cAddress)
{
	TWDR = i2cAddress;
	unsigned long startingTime = millis();
	TWCR = (1<<TWINT) | (1<<TWEN);
	while (!(TWCR & (1<<TWINT)))
	{
		if(!timeOutDelay){continue;}
		if((millis() - startingTime) >= timeOutDelay)
		{
			lockUp();
			return(1);
		}  
	}
	if ((TWI_STATUS == MT_SLA_ACK) || (TWI_STATUS == MR_SLA_ACK))
	{
		return(0);
	}
	uint8_t bufferedStatus = TWI_STATUS;
	if ((TWI_STATUS == MT_SLA_NACK) || (TWI_STATUS == MR_SLA_NACK))
	{
		stop();
		return(bufferedStatus);
	}
	else
	{
		lockUp();
		return(bufferedStatus);
	} 
}

uint8_t sendByte(uint8_t i2cData)
{
	TWDR = i2cData;
	unsigned long startingTime = millis();
	TWCR = (1<<TWINT) | (1<<TWEN);
	while (!(TWCR & (1<<TWINT)))
	{ 
		if(!timeOutDelay){continue;}
		if((millis() - startingTime) >= timeOutDelay)
		{
			lockUp();
			return(1);
		}
	} 
	if (TWI_STATUS == MT_DATA_ACK)
	{
		return(0);
	}
	uint8_t bufferedStatus = TWI_STATUS;
	if (TWI_STATUS == MT_DATA_NACK)
	{
		stop();
		return(bufferedStatus);
	}
	else
	{  
		lockUp();
	return(bufferedStatus);
	} 
}

uint8_t receiveByte(uint8_t ack)
{
	unsigned long startingTime = millis();
	if(ack)
	{
		TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWEA);
	}
	else
	{
		TWCR = (1<<TWINT) | (1<<TWEN);
	}
	while (!(TWCR & (1<<TWINT)))
	{
		if(!timeOutDelay){continue;}
		if((millis() - startingTime) >= timeOutDelay)
		{
			lockUp();
			return(1);
		}
	}
	if (TWI_STATUS == LOST_ARBTRTN)
	{
		uint8_t bufferedStatus = TWI_STATUS;
		lockUp();
		return(bufferedStatus);
	}
	return(TWI_STATUS); 
}

uint8_t stop()
{
	unsigned long startingTime = millis();
	TWCR = (1<<TWINT)|(1<<TWEN)| (1<<TWSTO);
	while ((TWCR & (1<<TWSTO)))
	{
		if(!timeOutDelay){continue;}
		if((millis() - startingTime) >= timeOutDelay)
		{
			lockUp();
			return(1);
		} 
	}
	return(0);
}

void lockUp()
{
	TWCR = 0; //releases SDA and SCL lines to high impedance
	TWCR = _BV(TWEN) | _BV(TWEA); //reinitialize TWI 
}

