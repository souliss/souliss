/**************************************************************************
	Souliss 
    Copyright (C) 2016  Veseo

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
	
	Originally developed by Plamen Kovandjiev and Dimitar Antonov
	
***************************************************************************/
/*!
    \file 
    \ingroup
*/

/* ----------------------------------------------------------------------- */
/* Expander MCP23S17 methods. */
/* ----------------------------------------------------------------------- */

/**
 * @brief Read an expander MCP23S17 a register.
 *
 * @param address A register address.
 *
 * @return The data from the register.
 */
uint8_t DINoWroom_ExpanderReadRegister(uint8_t address)
{
	_expTxData[0] = READ_CMD;
	_expTxData[1] = address;

	digitalWrite(CS, LOW);
	SPI.transferBytes(_expTxData, _expRxData, 3);
	digitalWrite(CS, HIGH);

	return _expRxData[2];
}

/**
 * @brief Write data in expander MCP23S17 register.
 *
 * @param address A register address.
 * @param data A byte for write.
 *
 * @return void.
 */
void DINoWroom_ExpanderWriteRegister(uint8_t address, uint8_t data)
{
	_expTxData[0] = WRITE_CMD;
	_expTxData[1] = address;
	_expTxData[2] = data;

	digitalWrite(CS, LOW);
	SPI.transferBytes(_expTxData, _expRxData, 3);
	digitalWrite(CS, HIGH);
}

/**
 * @brief Set a expander MCP23S17 the pin direction.
 *
 * @param pinNumber Pin number for set.
 * @param mode direction mode. 0 - INPUT, 1 - OUTPUT.
 *
 * @return void
 */
void DINoWroom_ExpanderSetDirection(uint8_t pinNumber, uint8_t mode)
{
	uint8_t registerData = DINoWroom_ExpanderReadRegister(IODIR);

	if (INPUT == mode)
	{
		registerData |= (1 << pinNumber);
	}
	else
	{
		registerData &= ~(1 << pinNumber);
	}

	DINoWroom_ExpanderWriteRegister(IODIR, registerData);
}

/**
 * @brief Set expander MCP23S17 pin direction.
 *
 * @param address Register address.
 * @param data Byte to write.
 *
 * @return void.
 */
void DINoWroom_ExpanderInitGPIO()
{
	// Relays.
	for (uint8_t i = 0; i < RELAY_COUNT; i++)
	{
		DINoWroom_ExpanderSetDirection(RELAY_PINS[i], OUTPUT);
	}

	// Opto inputs.
	for (uint8_t i = 0; i < OPTOIN_COUNT; i++)
	{
		DINoWroom_ExpanderSetDirection(OPTOIN_PINS[i], INPUT);
	}
}

/**
 * @brief Set expander MCP23S17 pin state.
 *
 * @param pinNumber The number of pin to be set.
 * @param state The pin state, true - 1, false - 0.
 *
 * @return void
 */
void DINoWroom_ExpanderSetPin(uint8_t pinNumber, bool state)
{
	uint8_t registerData = DINoWroom_ExpanderReadRegister(OLAT);

	if (state)
	{
		registerData |= (1 << pinNumber);
	}
	else
	{
		registerData &= ~(1 << pinNumber);
	}

	DINoWroom_ExpanderWriteRegister(OLAT, registerData);
}

/**
 * @brief Get expander MCP23S17 pin state.
 *
 * @param pinNumber The number of pin to be get.
 *
 * @return State true - 1, false - 0.
 */
bool DINoWroom_ExpanderGetPin(uint8_t pinNumber)
{
	uint8_t registerData = DINoWroom_ExpanderReadRegister(GPIO);

	return registerData & (1 << pinNumber);
}