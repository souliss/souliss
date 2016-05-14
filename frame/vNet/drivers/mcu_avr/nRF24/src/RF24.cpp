/*
 Copyright (C) 2011 J. Coliz <maniacbug@ymail.com>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 2 as published by the Free Software Foundation.
 */

#include "nRF24L01.h"
#include "RF24.h"

static const uint8_t child_pipe[] =
{
  RX_ADDR_P0, RX_ADDR_P1, RX_ADDR_P2, RX_ADDR_P3, RX_ADDR_P4, RX_ADDR_P5
};
static const uint8_t child_payload_size[] =
{
  RX_PW_P0, RX_PW_P1, RX_PW_P2, RX_PW_P3, RX_PW_P4, RX_PW_P5
};
static const uint8_t child_pipe_enable[] =
{
  ERX_P0, ERX_P1, ERX_P2, ERX_P3, ERX_P4, ERX_P5
};

/****************************************************************************/

void RF24::csn(int mode)
{
  // Minimum ideal SPI bus speed is 2x data rate
  // If we assume 2Mbs data rate and 16Mhz clock, a
  // divider of 4 is the minimum we want.
  // CLK:BUS 8Mhz:2Mhz, 16Mhz:4Mhz, or 20Mhz:5Mhz

/* NOTE : Ensure that SPI settings are feasible for all slaves */

#ifndef	nRF24SPI_INSKETCH
  nRF24SPI.setBitOrder(MSBFIRST);
  nRF24SPI.setDataMode(SPI_MODE0);
  nRF24SPI.setClockDivider(SPI_CLOCK_DIV4);
#endif

  digitalWrite(csn_pin,mode);
}

/****************************************************************************/

void RF24::ce(int level)
{
  digitalWrite(ce_pin,level);
}

/****************************************************************************/

uint8_t RF24::read_register(uint8_t reg, uint8_t* buf, uint8_t len)
{
  uint8_t status;

  csn(LOW);
  status = nRF24SPI.transfer( R_REGISTER | ( REGISTER_MASK & reg ) );
  while ( len-- )
    *buf++ = nRF24SPI.transfer(0xff);

  csn(HIGH);

  return status;
}

/****************************************************************************/

uint8_t RF24::read_register(uint8_t reg)
{
  csn(LOW);
  nRF24SPI.transfer( R_REGISTER | ( REGISTER_MASK & reg ) );
  uint8_t result = nRF24SPI.transfer(0xff);

  csn(HIGH);
  return result;
}

/****************************************************************************/

uint8_t RF24::write_register(uint8_t reg, const uint8_t* buf, uint8_t len)
{
  uint8_t status;

  csn(LOW);
  status = nRF24SPI.transfer( W_REGISTER | ( REGISTER_MASK & reg ) );
  while ( len-- )
    nRF24SPI.transfer(*buf++);

  csn(HIGH);

  return status;
}

/****************************************************************************/

uint8_t RF24::write_register(uint8_t reg, uint8_t value)
{
  uint8_t status;

  csn(LOW);
  status = nRF24SPI.transfer( W_REGISTER | ( REGISTER_MASK & reg ) );
  nRF24SPI.transfer(value);
  csn(HIGH);

  return status;
}

/****************************************************************************/

uint8_t RF24::write_payload(const void* buf, uint8_t len)
{
    uint8_t status;
    oFrame* frame=0;

    // Check if the pointer refers to an oFrame or to a standard buffer

    if(len)				// Standard buffer
    {
        const uint8_t* current = reinterpret_cast<const uint8_t*>(buf);

        uint8_t data_len = min(len,payload_size);
        uint8_t blank_len = dynamic_payloads_enabled ? 0 : payload_size - data_len;

        csn(LOW);
        status = nRF24SPI.transfer( W_TX_PAYLOAD );
        while ( data_len-- )
            nRF24SPI.transfer(*current++);
        while ( blank_len-- )
            nRF24SPI.transfer(0);
        csn(HIGH);
    }
    else if(len==0)		// oFrame
    {

    /* NOTE: Ensure that proper casting is performed */

        frame = (oFrame*)buf;				// Pointer to output frame
        oFrame_Define(frame);				// Set the frame
        len = oFrame_GetLenght();			// Get the total length

        if(len > NRF24_MAX_PAYLOAD)			// The nRF24L01 support small payloads, we simply cut here
            len = NRF24_MAX_PAYLOAD;		// and fill with zero once received

        uint8_t data_len = min(len,payload_size);
        uint8_t blank_len = dynamic_payloads_enabled ? 0 : payload_size - data_len;

        csn(LOW);
        status = nRF24SPI.transfer( W_TX_PAYLOAD );
        while ( data_len-- )
            if(oFrame_Available())
                nRF24SPI.transfer(oFrame_GetByte());
            else
                return 0;

        while ( blank_len-- )
            nRF24SPI.transfer(0);

        oFrame_Reset();
        csn(HIGH);
    }


  return status;
}

/****************************************************************************/

uint8_t RF24::read_payload(void* buf, uint8_t len)
{
  uint8_t status;
  uint8_t* current = reinterpret_cast<uint8_t*>(buf);

  uint8_t data_len = min(len,payload_size);
  uint8_t blank_len = dynamic_payloads_enabled ? 0 : payload_size - data_len;

  csn(LOW);
  status = nRF24SPI.transfer( R_RX_PAYLOAD );
  while ( data_len-- )
    *current++ = nRF24SPI.transfer(0xff);
  while ( blank_len-- )
    nRF24SPI.transfer(0xff);
  csn(HIGH);

  return status;
}

/****************************************************************************/

uint8_t RF24::flush_rx(void)
{
  uint8_t status;

  csn(LOW);
  status = nRF24SPI.transfer( FLUSH_RX );
  csn(HIGH);

  return status;
}

/****************************************************************************/

uint8_t RF24::flush_tx(void)
{
  uint8_t status;

  csn(LOW);
  status = nRF24SPI.transfer( FLUSH_TX );
  csn(HIGH);

  return status;
}

/****************************************************************************/

uint8_t RF24::get_status(void)
{
  uint8_t status;

  csn(LOW);
  status = nRF24SPI.transfer( NOP );
  csn(HIGH);

  return status;
}

/****************************************************************************/

RF24::RF24(uint8_t _cepin, uint8_t _cspin):
  ce_pin(_cepin), csn_pin(_cspin), wide_band(true), p_variant(false),
  payload_size(32), ack_payload_available(false), dynamic_payloads_enabled(false)//,
  //pipe0_reading_address(0)
{
    pipe0_reading_address[0]=0;
}

/****************************************************************************/

void RF24::setChannel(uint8_t channel)
{
  // TODO: This method could take advantage of the 'wide_band' calculation
  // done in setChannel() to require certain channel spacing.

  const uint8_t max_channel = 125;
  write_register(RF_CH,min(channel,max_channel));
}

/****************************************************************************/

uint8_t RF24::getChannel()
{

  return read_register(RF_CH);
}

/****************************************************************************/

void RF24::setPayloadSize(uint8_t size)
{
  const uint8_t max_payload_size = 32;
  payload_size = min(size,max_payload_size);
}

/****************************************************************************/

uint8_t RF24::getPayloadSize(void)
{
  return payload_size;
}

/****************************************************************************/

bool RF24::begin(void)
{
  uint8_t setup=0;

  // Initialize pins
  pinMode(ce_pin,OUTPUT);
  pinMode(csn_pin,OUTPUT);

  // Initialize SPI bus
  nRF24SPI.begin();

  ce(LOW);
  csn(HIGH);

  // Must allow the radio time to settle else configuration bits will not necessarily stick.
  // This is actually only required following power up but some settling time also appears to
  // be required after resets too. For full coverage, we'll always assume the worst.
  // Enabling 16b CRC is by far the most obvious case if the wrong timing is used - or skipped.
  // Technically we require 4.5ms + 14us as a worst case. We'll just call it 5ms for good measure.
  // WARNING: Delay is based on P-variant whereby non-P *may* require different timing.
  delay( 6 ) ;

  powerDown(); // Needed for for correct initialization

  // Reset CONFIG and enable 16-bit CRC. <<<+
  write_register( CONFIG, 0b00001100 ) ;


  // Set 1500uS (minimum for 32B payload in ESB@250KBPS) timeouts, to make testing a little easier
  // WARNING: If this is ever lowered, either 250KBS mode with AA is broken or maximum packet
  // sizes must never be used. See documentation for a more complete explanation.
  write_register(SETUP_RETR,(B0100 << ARD) | (B1111 << ARC));

  setRetries(5,15);

  // Restore our default PA level
  setPALevel( RF24_PA_MAX ) ;

  // Determine if this is a p or non-p RF24 module and then
  // reset our data rate back to default value. This works
  // because a non-P variant won't allow the data rate to
  // be set to 250Kbps.
  if( setDataRate( RF24_250KBPS ) )
  {
    p_variant = true ;
  }

  setup = read_register(RF_SETUP);

  // Then set the data rate to the slowest (and most reliable) speed supported by all
  // hardware.
  setDataRate( RF24_1MBPS );


  // Initialize CRC and request 2-byte (16bit) CRC <<<-
  //setCRCLength( RF24_CRC_16 ) ;

  // Disable dynamic payloads, to match dynamic_payloads_enabled setting <<<+
    toggle_features();
    write_register(FEATURE,0 );
    write_register(DYNPD,0);


  // Reset current status
  // Notice reset and flush is the last thing we do
  write_register(STATUS,_BV(RX_DR) | _BV(TX_DS) | _BV(MAX_RT) );

  // Set up default configuration.  Callers can always change it later.
  // This channel should be universally safe and not bleed over into adjacent
  // spectrum.
  setChannel(NRF24_CHANNEL);

  // Flush buffers
  flush_rx();
  flush_tx();

  powerUp(); //Power up by default when begin() is called

  // Enable PTX, do not write CE high so radio will remain in standby I mode ( 130us max to transition to RX or TX instead of 1500us from powerUp )
  // PTX should use only 22uA of power
  write_register(CONFIG, ( read_register(CONFIG) ) & ~_BV(PRIM_RX) );

  // if setup is 0 or ff then there was no response from module
  return ( setup != 0 && setup != 0xff );

}

/****************************************************************************/

void RF24::startListening(void)
{

    powerUp();

    write_register(CONFIG, read_register(CONFIG) | _BV(PRIM_RX));
    write_register(STATUS, _BV(RX_DR) | _BV(TX_DS) | _BV(MAX_RT) );
    ce(HIGH);
    // Restore the pipe0 adddress, if exists
    if (pipe0_reading_address[0] > 0){
        write_register(RX_ADDR_P0, pipe0_reading_address, 5);
    }else{
        closeReadingPipe(0);
    }

    // Flush buffers
    //flush_rx();
    if(read_register(FEATURE) & _BV(EN_ACK_PAY)){
        flush_tx();
    }

    // Go!
    //delayMicroseconds(100);
}

/****************************************************************************/

void RF24::stopListening(void)
{
    ce(LOW);

    delayMicroseconds(txRxDelay);

    if(read_register(FEATURE) & _BV(EN_ACK_PAY)){
        delayMicroseconds(txRxDelay); //200
        flush_tx();
    }
    //flush_rx();
    write_register(CONFIG, ( read_register(CONFIG) ) & ~_BV(PRIM_RX) );
    write_register(EN_RXADDR,read_register(EN_RXADDR) | _BV(pgm_read_byte(&child_pipe_enable[0]))); // Enable RX on pipe0

    //delayMicroseconds(100);

}

/****************************************************************************/

void RF24::powerDown(void)
{
    ce(LOW); // Guarantee CE is low on powerDown
    write_register(CONFIG,read_register(CONFIG) & ~_BV(PWR_UP));
}

/****************************************************************************/

void RF24::powerUp(void)
{
    int8_t cfg = read_register(CONFIG);

    // if not powered up then power up and wait for the radio to initialize
    if (!(cfg & _BV(PWR_UP))){
        write_register(CONFIG,read_register(CONFIG) | _BV(PWR_UP));

        // For nRF24L01+ to go from power down mode to TX or RX mode it must first pass through stand-by mode.
        // There must be a delay of Tpd2stby (see Table 16.) after the nRF24L01+ leaves power down mode before
        // the CEis set high. - Tpd2stby can be up to 5ms per the 1.0 datasheet
        delay(6);
    }
}

/******************************************************************/

bool RF24::write( const void* buf, uint8_t len )
{
  bool result = false;

  // Begin the write
  startWrite(buf,len);

  // ------------
  // At this point we could return from a non-blocking write, and then call
  // the rest after an interrupt

  // Instead, we are going to block here until we get TX_DS (transmission completed and ack'd)
  // or MAX_RT (maximum retries, transmission failed).  Also, we'll timeout in case the radio
  // is flaky and we get neither.

  // IN the end, the send should be blocking.  It comes back in 60ms worst case, or much faster
  // if I tighted up the retry logic.  (Default settings will be 1500us.
  // Monitor the send
  uint8_t observe_tx;
  uint8_t status;
  uint32_t sent_at = millis();
  const uint32_t timeout = 500; //ms to wait for timeout
  do
  {
    status = read_register(OBSERVE_TX,&observe_tx,1);
  }
  while( ! ( status & ( _BV(TX_DS) | _BV(MAX_RT) ) ) && ( millis() - sent_at < timeout ) );

  // The part above is what you could recreate with your own interrupt handler,
  // and then call this when you got an interrupt
  // ------------

  // Call this when you get an interrupt
  // The status tells us three things
  // * The send was successful (TX_DS)
  // * The send failed, too many retries (MAX_RT)
  // * There is an ack packet waiting (RX_DR)
  bool tx_ok, tx_fail;
  whatHappened(tx_ok,tx_fail,ack_payload_available);

  //printf("%u%u%u\r\n",tx_ok,tx_fail,ack_payload_available);

  result = tx_ok;

  // Handle the ack packet
  if ( ack_payload_available )
    ack_payload_length = getDynamicPayloadSize();

  // Yay, we are done.

 /***
    Comment out, we don't really need to power down the radio after sending data

  // Power down
  powerDown();

***/

  // Flush buffers (Is this a relic of past experimentation, and not needed anymore??)
  flush_tx();

  return result;
}
/****************************************************************************/

void RF24::startWrite( const void* buf, uint8_t len )
{
  // Send the payload
  write_payload( buf, len );

  // Allons!
  ce(HIGH);
  delayMicroseconds(50);
  ce(LOW);
}

/****************************************************************************/

uint8_t RF24::getDynamicPayloadSize(void)
{
  uint8_t result = 0;

  csn(LOW);
  nRF24SPI.transfer( R_RX_PL_WID );
  result = nRF24SPI.transfer(0xff);
  csn(HIGH);

  if(result > 32)
  {
      flush_rx();
      delay(2);
      return 0;
  }

  return result;
}

/****************************************************************************/

bool RF24::available(void)
{
  return available(NULL);
}

/****************************************************************************/

bool RF24::available(uint8_t* pipe_num)
{
    if (!( read_register(FIFO_STATUS) & _BV(RX_EMPTY) )){

        // If the caller wants the pipe number, include that
        if ( pipe_num ){
            uint8_t status = get_status();
            *pipe_num = ( status >> RX_P_NO ) & 0b111;
        }
        return 1;
    }


    return 0;


}

/****************************************************************************/

bool RF24::read( void* buf, uint8_t len )
{
  // Fetch the payload
  read_payload( buf, len );

  //Clear the two possible interrupt flags with one command
  write_register(STATUS,_BV(RX_DR) | _BV(MAX_RT) | _BV(TX_DS) );

  // was this the last of the data available?
  return read_register(FIFO_STATUS) & _BV(RX_EMPTY);
}

/****************************************************************************/

void RF24::whatHappened(bool& tx_ok,bool& tx_fail,bool& rx_ready)
{
  // Read the status & reset the status in one easy call
  // Or is that such a good idea?
  uint8_t status = write_register(STATUS,_BV(RX_DR) | _BV(TX_DS) | _BV(MAX_RT) );

  // Report to the user what happened
  tx_ok = status & _BV(TX_DS);
  tx_fail = status & _BV(MAX_RT);
  rx_ready = status & _BV(RX_DR);
}

/****************************************************************************/

void RF24::openWritingPipe(uint64_t value)
{
  // Note that AVR 8-bit uC's store this LSB first, and the NRF24L01(+)
  // expects it LSB first too, so we're good.

  write_register(RX_ADDR_P0, reinterpret_cast<uint8_t*>(&value), 5);
  write_register(TX_ADDR, reinterpret_cast<uint8_t*>(&value), 5);

  const uint8_t max_payload_size = 32;
  write_register(RX_PW_P0,min(payload_size,max_payload_size));
}

/****************************************************************************/

void RF24::openReadingPipe(uint8_t child, uint64_t address)
{
  // If this is pipe 0, cache the address.  This is needed because
  // openWritingPipe() will overwrite the pipe 0 address, so
  // startListening() will have to restore it.
  if (child == 0)
    memcpy(pipe0_reading_address,&address,5);

  if (child <= 6)
  {
    // For pipes 2-5, only write the LSB
    if ( child < 2 )
      write_register(child_pipe[child], reinterpret_cast<const uint8_t*>(&address), 5);
    else
      write_register(child_pipe[child], reinterpret_cast<const uint8_t*>(&address), 1);

    write_register(child_payload_size[child],payload_size);

    // Note it would be more efficient to set all of the bits for all open
    // pipes at once.  However, I thought it would make the calling code
    // more simple to do it this way.
    write_register(EN_RXADDR,read_register(EN_RXADDR) | _BV(child_pipe_enable[child]));
  }
}

/****************************************************************************/

void RF24::closeReadingPipe( uint8_t pipe )
{
  write_register(EN_RXADDR,read_register(EN_RXADDR) & ~_BV(pgm_read_byte(&child_pipe_enable[pipe])));
}

/****************************************************************************/

void RF24::toggle_features(void)
{
  csn(LOW);
  nRF24SPI.transfer( ACTIVATE );
  nRF24SPI.transfer( 0x73 );
  csn(HIGH);
}

/****************************************************************************/

void RF24::enableDynamicPayloads(void)
{
  // Enable dynamic payload throughout the system
  write_register(FEATURE,read_register(FEATURE) | _BV(EN_DPL) );

  // If it didn't work, the features are not enabled
  if ( ! read_register(FEATURE) )
  {
    // So enable them and try again
    toggle_features();
    write_register(FEATURE,read_register(FEATURE) | _BV(EN_DPL) );
  }

  // Enable dynamic payload on all pipes
  //
  // Not sure the use case of only having dynamic payload on certain
  // pipes, so the library does not support it.
  write_register(DYNPD,read_register(DYNPD) | _BV(DPL_P5) | _BV(DPL_P4) | _BV(DPL_P3) | _BV(DPL_P2) | _BV(DPL_P1) | _BV(DPL_P0));

  dynamic_payloads_enabled = true;
}

/****************************************************************************/

void RF24::enableAckPayload(void)
{
  //
  // enable ack payload and dynamic payload features
  //

  write_register(FEATURE,read_register(FEATURE) | _BV(EN_ACK_PAY) | _BV(EN_DPL) );

  // If it didn't work, the features are not enabled
  if ( ! read_register(FEATURE) )
  {
    // So enable them and try again
    toggle_features();
    write_register(FEATURE,read_register(FEATURE) | _BV(EN_ACK_PAY) | _BV(EN_DPL) );
  }

  //
  // Enable dynamic payload on pipes 0 & 1
  //

  write_register(DYNPD,read_register(DYNPD) | _BV(DPL_P1) | _BV(DPL_P0));
}

/****************************************************************************/

void RF24::writeAckPayload(uint8_t pipe, const void* buf, uint8_t len)
{
  const uint8_t* current = reinterpret_cast<const uint8_t*>(buf);

  csn(LOW);
  nRF24SPI.transfer( W_ACK_PAYLOAD | ( pipe & B111 ) );
  const uint8_t max_payload_size = 32;
  uint8_t data_len = min(len,max_payload_size);
  while ( data_len-- )
    nRF24SPI.transfer(*current++);

  csn(HIGH);
}

/****************************************************************************/

bool RF24::isAckPayloadAvailable(void)
{
  return ! (read_register(FIFO_STATUS) & _BV(RX_EMPTY));
}

/****************************************************************************/

bool RF24::isPVariant(void)
{
  return p_variant ;
}

/****************************************************************************/

void RF24::setAutoAck(bool enable)
{
  if ( enable )
    write_register(EN_AA, B111111);
  else
    write_register(EN_AA, 0);
}

/****************************************************************************/

void RF24::setAutoAck( uint8_t pipe, bool enable )
{
  if ( pipe <= 6 )
  {
    uint8_t en_aa = read_register( EN_AA ) ;
    if( enable )
    {
      en_aa |= _BV(pipe) ;
    }
    else
    {
      en_aa &= ~_BV(pipe) ;
    }
    write_register( EN_AA, en_aa ) ;
  }
}

/****************************************************************************/

bool RF24::testCarrier(void)
{
  return ( read_register(CD) & 1 );
}

/****************************************************************************/

bool RF24::testRPD(void)
{
  return ( read_register(RPD) & 1 ) ;
}

/****************************************************************************/

void RF24::setPALevel(rf24_pa_dbm_e level)
{
  uint8_t setup = read_register(RF_SETUP) ;
  setup &= ~(_BV(RF_PWR_LOW) | _BV(RF_PWR_HIGH)) ;

  // switch uses RAM (evil!)
  if ( level == RF24_PA_MAX )
  {
    setup |= (_BV(RF_PWR_LOW) | _BV(RF_PWR_HIGH)) ;
  }
  else if ( level == RF24_PA_HIGH )
  {
    setup |= _BV(RF_PWR_HIGH) ;
  }
  else if ( level == RF24_PA_LOW )
  {
    setup |= _BV(RF_PWR_LOW);
  }
  else if ( level == RF24_PA_MIN )
  {
    // nothing
  }
  else if ( level == RF24_PA_ERROR )
  {
    // On error, go to maximum PA
    setup |= (_BV(RF_PWR_LOW) | _BV(RF_PWR_HIGH)) ;
  }

  write_register( RF_SETUP, setup ) ;
}

/****************************************************************************/

rf24_pa_dbm_e RF24::getPALevel(void)
{
  rf24_pa_dbm_e result = RF24_PA_ERROR ;
  uint8_t power = read_register(RF_SETUP) & (_BV(RF_PWR_LOW) | _BV(RF_PWR_HIGH)) ;

  // switch uses RAM (evil!)
  if ( power == (_BV(RF_PWR_LOW) | _BV(RF_PWR_HIGH)) )
  {
    result = RF24_PA_MAX ;
  }
  else if ( power == _BV(RF_PWR_HIGH) )
  {
    result = RF24_PA_HIGH ;
  }
  else if ( power == _BV(RF_PWR_LOW) )
  {
    result = RF24_PA_LOW ;
  }
  else
  {
    result = RF24_PA_MIN ;
  }

  return result ;
}

/****************************************************************************/

bool RF24::setDataRate(rf24_datarate_e speed)
{
  bool result = false;
  uint8_t setup = read_register(RF_SETUP) ;

  // HIGH and LOW '00' is 1Mbs - our default
  wide_band = false ;
  setup &= ~(_BV(RF_DR_LOW) | _BV(RF_DR_HIGH)) ;
  txRxDelay=85;

  if( speed == RF24_250KBPS )
  {
    // Must set the RF_DR_LOW to 1; RF_DR_HIGH (used to be RF_DR) is already 0
    // Making it '10'.
    wide_band = false ;
    setup |= _BV( RF_DR_LOW ) ;
    txRxDelay=155;
  }
  else
  {
    // Set 2Mbs, RF_DR (RF_DR_HIGH) is set 1
    // Making it '01'
    if ( speed == RF24_2MBPS )
    {
      wide_band = true ;
      setup |= _BV(RF_DR_HIGH);
      txRxDelay=65;
    }
    else
    {
      // 1Mbs
      wide_band = false ;
    }
  }
  write_register(RF_SETUP,setup);

  // Verify our result
  if ( read_register(RF_SETUP) == setup )
  {
    result = true;
  }
  else
  {
    wide_band = false;
  }

  return result;
}

/****************************************************************************/

rf24_datarate_e RF24::getDataRate( void )
{
  rf24_datarate_e result ;
  uint8_t dr = read_register(RF_SETUP) & (_BV(RF_DR_LOW) | _BV(RF_DR_HIGH));

  // switch uses RAM (evil!)
  // Order matters in our case below
  if ( dr == _BV(RF_DR_LOW) )
  {
    // '10' = 250KBPS
    result = RF24_250KBPS ;
  }
  else if ( dr == _BV(RF_DR_HIGH) )
  {
    // '01' = 2MBPS
    result = RF24_2MBPS ;
  }
  else
  {
    // '00' = 1MBPS
    result = RF24_1MBPS ;
  }
  return result ;
}

/****************************************************************************/

void RF24::setCRCLength(rf24_crclength_e length)
{
  uint8_t config = read_register(CONFIG) & ~( _BV(CRCO) | _BV(EN_CRC)) ;

  // switch uses RAM (evil!)
  if ( length == RF24_CRC_DISABLED )
  {
    // Do nothing, we turned it off above.
  }
  else if ( length == RF24_CRC_8 )
  {
    config |= _BV(EN_CRC);
  }
  else
  {
    config |= _BV(EN_CRC);
    config |= _BV( CRCO );
  }
  write_register( CONFIG, config ) ;
}

/****************************************************************************/

rf24_crclength_e RF24::getCRCLength(void)
{
  rf24_crclength_e result = RF24_CRC_DISABLED;
  uint8_t config = read_register(CONFIG) & ( _BV(CRCO) | _BV(EN_CRC)) ;

  if ( config & _BV(EN_CRC ) )
  {
    if ( config & _BV(CRCO) )
      result = RF24_CRC_16;
    else
      result = RF24_CRC_8;
  }

  return result;
}

/****************************************************************************/

void RF24::disableCRC( void )
{
  uint8_t disable = read_register(CONFIG) & ~_BV(EN_CRC) ;
  write_register( CONFIG, disable ) ;
}

/****************************************************************************/
void RF24::setRetries(uint8_t delay, uint8_t count)
{
 write_register(SETUP_RETR,(delay&0xf)<<ARD | (count&0xf)<<ARC);
}

// vim:ai:cin:sts=2 sw=2 ft=cpp

