/*******************************************************************
    Copyright (C) 2009 FreakLabs
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions
    are met:

    1. Redistributions of source code must retain the above copyright
       notice, this list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright
       notice, this list of conditions and the following disclaimer in the
       documentation and/or other materials provided with the distribution.
    3. Neither the name of the the copyright holder nor the names of its contributors
       may be used to endorse or promote products derived from this software
       without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS'' AND
    ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
    ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
    FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
    DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
    OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
    HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
    LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
    OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
    SUCH DAMAGE.

    Originally written by Christopher Wang aka Akiba.
    Please post support questions to the FreakLabs forum.

*******************************************************************/
/*!
    \file 
    \ingroup


*/
/**************************************************************************/
#include "chb.h"
#include "chb_spi.h"

/**************************************************************************/
/*!
    Initialize the SPI
*/
/**************************************************************************/
void chb_spi_init()
{
    // configure the SPI slave_select, spi clk, and mosi pins as output. the miso pin
    // is cleared since its an input.
    CHB_SPI_DDIR |=  _BV(CHB_SCK) | _BV(CHB_MOSI);
    CHB_SPI_CS_DDIR |= _BV(CHB_SPI_CS_PIN);

    CHB_SPI_PORT |= _BV(CHB_SCK);
    CHB_SPI_CS_PORT |= _BV(CHB_SPI_CS_PIN);

    // enable pullups on MISO to make the rise times faster
    CHB_SPI_PORT |= _BV(CHB_MISO);

    // *IMPORTANT* enable the pullup on the original SPI CS pin. Otherwise, if it suddenly goes low,
    // the SPI will default to slave mode.
    CHB_SPI_PORT |= _BV(CHB_SPI_SELN);

    // set to master mode
    // set the clock freq
    SPCR |= _BV(MSTR) | _BV(SPR0);
    SPSR |= _BV(SPI2X);

    // enable the SPI master
    SPCR |= (1 << SPE);

    // set the slave select to idle
    CHB_SPI_DISABLE();
}

/**************************************************************************/
/*!
    This function both reads and writes data. For write operations, include data
    to be written as argument. For read ops, use dummy data as arg. Returned
    data is read byte val.
*/
/**************************************************************************/
U8 chb_xfer_byte(U8 data)
{
    SPDR = data;
    while (!(SPSR & (1<<SPIF)));
    return SPDR;
}