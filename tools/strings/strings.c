/**************************************************************************
	String Tools 	

	Modified by Juan Pinto
***************************************************************************/

// Check if Values is between 0-255
boolean checkRange(String Value)
{
	if (Value.toInt() < 0 || Value.toInt() > 255)
		return false;
	else
		return true;
}

// Convert a single hex digit character to its integer value (from https://code.google.com/p/avr-netino/)
unsigned char h2int(char c)
{
	if (c >= '0' && c <='9'){
		return((unsigned char)c - '0');
	}
	if (c >= 'a' && c <='f'){
		return((unsigned char)c - 'a' + 10);
	}
	if (c >= 'A' && c <='F'){
		return((unsigned char)c - 'A' + 10);
	}
	return(0);
}

// Parse the + and % characters into an URL (based on https://code.google.com/p/avr-netino/)
String urldecode(String input) 
{
	char c;
	String ret = "";
	
	for(byte t=0;t<input.length();t++)
	{
		c = input[t];
		if (c == '+') c = ' ';
		if (c == '%') {
			t++;
			c = input[t];
			t++;
			c = (h2int(c) << 4) | h2int(input[t]);
		}
		
		ret.concat(c);
	}
	return ret;

}



