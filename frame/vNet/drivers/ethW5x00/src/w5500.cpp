/*
 * Copyright (c) 2010 by WIZnet <support@wiznet.co.kr>
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of either the GNU General Public License version 2
 * or the GNU Lesser General Public License version 2.1, both as
 * published by the Free Software Foundation.
 */

#include <stdio.h>
#include <string.h>

#include "w5500.h"

// W5500 controller instance
W5500Class W5x00;

void W5500Class::init(void)
{
    delay(300);

    initSS();
    SPI.begin();
    for (int i=0; i<MAX_SOCK_NUM; i++) {
        uint8_t cntl_byte = (0x0C + (i<<5));
        write( 0x1E, cntl_byte, 2); //0x1E - Sn_RXBUF_SIZE
        write( 0x1F, cntl_byte, 2); //0x1F - Sn_TXBUF_SIZE
    }
}

uint16_t W5500Class::getTXFreeSize(SOCKET s)
{
    uint16_t val=0, val1=0;
    do {
        val1 = readSnTX_FSR(s);
        if (val1 != 0)
            val = readSnTX_FSR(s);
    } 
    while (val != val1);
    return val;
}

uint16_t W5500Class::getRXReceivedSize(SOCKET s)
{
    uint16_t val=0,val1=0;
    do {
        val1 = readSnRX_RSR(s);
        if (val1 != 0)
            val = readSnRX_RSR(s);
    } 
    while (val != val1);
    return val;
}

void W5500Class::send_data_processing(SOCKET s, const uint8_t *data, uint16_t len)
{
  // This is same as having no offset in a call to send_data_processing_offset
  send_data_processing_offset(s, 0, data, len);

}

void W5500Class::send_data_processing_offset(SOCKET s, uint16_t data_offset, const uint8_t *data, uint16_t len)
{
    uint16_t ptr = readSnTX_WR(s);
    uint8_t cntl_byte = (0x14+(s<<5));
    ptr += data_offset;
	oFrame* frame=0;
	
	// If len is equal to zero the buf pointer to a structure
	if(len==0)
	{	
		frame = (oFrame*)data;				// Pointer to output frame
		oFrame_Define(frame);				// Set the frame
		len = oFrame_GetLenght();			// Get the total lenght
	}
	
	// If there is no frame use data as pointer, otherwise get data from the oFrame
	if(frame==0)
	{	
		write(ptr, cntl_byte, data, len);
		ptr += len;
		writeSnTX_WR(s, ptr);
	}
	else
	{
		// Write down to the W5500
		for(U8 i=0;i<len;i++)
			if(oFrame_Available())
				write(ptr++, cntl_byte, oFrame_GetByte());
			else
				break;

		writeSnTX_WR(s, ptr);		
	}
}

void W5500Class::recv_data_processing(SOCKET s, uint8_t *data, uint16_t len, uint8_t peek)
{
    uint16_t ptr;
    ptr = readSnRX_RD(s);

    read_data(s, ptr, data, len);
    if (!peek)
    {
        ptr += len;
        writeSnRX_RD(s, ptr);
    }
}

void W5500Class::read_data(SOCKET s, volatile uint16_t src, volatile uint8_t *dst, uint16_t len)
{
    uint8_t cntl_byte = (0x18+(s<<5));
    read(src , cntl_byte, (uint8_t *)dst, len);
}

uint8_t W5500Class::write(uint16_t _addr, uint8_t _cb, uint8_t _data)
{
    setSS();  
    SPI.transfer(_addr >> 8);
    SPI.transfer(_addr & 0xFF);
    SPI.transfer(_cb);
    SPI.transfer(_data);
    resetSS();
	
    return 1;
}

uint16_t W5500Class::write(uint16_t _addr, uint8_t _cb, const uint8_t *_buf, uint16_t _len)
{
    setSS();
    SPI.transfer(_addr >> 8);
    SPI.transfer(_addr & 0xFF);
    SPI.transfer(_cb);
    for (uint16_t i=0; i<_len; i++){
        SPI.transfer(_buf[i]);
    }
    resetSS();
    return _len;
}

uint8_t W5500Class::read(uint16_t _addr, uint8_t _cb)
{
    setSS();
    SPI.transfer(_addr >> 8);
    SPI.transfer(_addr & 0xFF);
    SPI.transfer(_cb);
    uint8_t _data = SPI.transfer(0);
    resetSS();
    return _data;
}

uint16_t W5500Class::read(uint16_t _addr, uint8_t _cb, uint8_t *_buf, uint16_t _len)
{ 
    setSS();
    SPI.transfer(_addr >> 8);
    SPI.transfer(_addr & 0xFF);
    SPI.transfer(_cb);
    for (uint16_t i=0; i<_len; i++){
        _buf[i] = SPI.transfer(0);
    }
    resetSS();
    return _len;
}

void W5500Class::execCmdSn(SOCKET s, SockCMD _cmd) {
    // Send command to socket
    writeSnCR(s, _cmd);
    // Wait for command to complete
    while (readSnCR(s))
    ;
}
