/**************************************************************************
	ASCII Tools 	
***************************************************************************/

void ASCII_float2str(float val, byte precision, char *buf, unsigned bufLen = 0xffff);
unsigned ASCII_long2str(unsigned long val, char *buf, unsigned bufLen = 0xffff, byte width = 0);
void ASCII_num2str(uint8_t *data, uint8_t base, uint8_t *len);

/**************************************************************************
/*!
	Format a floating point value with number of decimal places.
	
	The 'precision' parameter is a number from 0 to 6 indicating the desired 
	decimal places. The 'buf' parameter points to a buffer to receive the 
	formatted string.  This must be 	sufficiently large to contain the 
	resulting string.  The buffer's length may be 	optionally specified.  
	If it is given, the maximum length of the generated string will be one 
	less than the specified value.
	
	example: fmtDouble(3.1415, 2, buf); // produces 3.14 (two decimal places)
*/	
/**************************************************************************/
void ASCII_float2str(float val, byte precision, char *buf, unsigned bufLen)
{
  if (!buf || !bufLen)
    return;

  // limit the precision to the maximum allowed value
  const byte maxPrecision = 6;
  if (precision > maxPrecision)
    precision = maxPrecision;

  if (--bufLen > 0)
  {
    // check for a negative value
    if (val < 0.0)
    {
      val = -val;
      *buf = '-';
      bufLen--;
    }

    // compute the rounding factor and fractional multiplier
    double roundingFactor = 0.5;
    unsigned long mult = 1;
    for (byte i = 0; i < precision; i++)
    {
      roundingFactor /= 10.0;
      mult *= 10;
    }

    if (bufLen > 0)
    {
      // apply the rounding factor
      val += roundingFactor;

      // add the integral portion to the buffer
      unsigned len = ASCII_long2str((unsigned long)val, buf, bufLen);
      buf += len;
      bufLen -= len;
    }

    // handle the fractional portion
    if ((precision > 0) && (bufLen > 0))
    {
      *buf++ = '.';
      if (--bufLen > 0)
        buf += ASCII_long2str((unsigned long)((val - (unsigned long)val) * mult), buf, bufLen, precision);
    }
  }

  // null-terminate the string
  *buf = '\0';

}

/**************************************************************************
/*!
	Produce a formatted string in a buffer corresponding to the value provided.
    If the 'width' parameter is non-zero, the value will be padded with leading
    zeroes to achieve the specified width.  The number of characters added to
    the buffer (not including the null termination) is returned.
*/	
/**************************************************************************/
unsigned ASCII_long2str(unsigned long val, char *buf, unsigned bufLen, byte width)
{
  if (!buf || !bufLen)
    return(0);

  // produce the digit string (backwards in the digit buffer)
  char dbuf[10];
  unsigned idx = 0;
  while (idx < sizeof(dbuf))
  {
    dbuf[idx++] = (val % 10) + '0';
    if ((val /= 10) == 0)
      break;
  }

  // copy the optional leading zeroes and digits to the target buffer
  unsigned len = 0;
  byte padding = (width > idx) ? width - idx : 0;
  char c = '0';
  while ((--bufLen > 0) && (idx || padding))
  {
    if (padding)
      padding--;
    else
      c = dbuf[--idx];
    *buf++ = c;
    len++;
  }

  // add the null termination
  *buf = '\0';
  return(len);
}

/**************************************************************************/
/*!
	Convert number in string
*/
/**************************************************************************/
uint16_t ASCII_str2num(uint8_t *data, uint8_t *plen) 
{
	uint16_t c = 0, n = 0;	
	
	while((data[c] != '.') && (data[c] != ',') && (data[c] != '\0') && (data[c] != '#') && (data[c] != '$')) {	
		n = ((n * 10) + (data[c] - '0'));
		c++;
	}
	
	c++;
	*plen+=c;
	
	return n;
}
/**************************************************************************/
/*!
	Convert number in string
*/
/**************************************************************************/
void ASCII_num2str(uint8_t *data, uint8_t base, uint8_t *len) 
{
	// Convert a number into a string
	unsigned long i = 0, n, nn;
	
	// Save the incoming byte
	n = *(unsigned long*)data;

	if (n == 0) 		// Print 0
	{
		data[0] = '0';
		*len += 1;
	} 
	else				// Print other chars
	{
		// Save the value to convert
		nn = n;
		
		while (nn > 0) 
		{
			i++;
			nn /= base;
		}

		*len += i;		// Store the size
		
		for (; i > 0; i--)
		{
			data[i-1] = n % base;
			n /= base;
			
			data[i-1] = (data[i-1] < 10 ? '0' + data[i-1] : 'A' + data[i-1] - 10);
		}
	}
}

/**************************************************************************/
/*!
	Compare a string with a sample one
*/
/**************************************************************************/
uint8_t compare_string(char *string, char *sample, uint8_t len)
{
	uint8_t i=0;
	
	while((i < len) && (*string==*sample))
	{ 
		string++;
		sample++;
		
		i++;
	}
	
	if( i == len )
		return 1;
	else
		return 0;
}

/**************************************************************************/
/*!
	Returns the number of times that a character is into a string 
*/
/**************************************************************************/
uint8_t nof_string(char *string, char sample, uint8_t len)
{
	uint8_t i=0, n=0;
	
	// Verify the number of matching chars
	while(i < len)
	{
		if(*(string+i)==sample)	n++;
		
		i++;
	}
	
	return n;
}