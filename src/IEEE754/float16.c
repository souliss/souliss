/**************************************************************************
	Souliss
    Copyright (C) 2011  Veseo

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
	
	Modified by Dario Di Maio
	
***************************************************************************/

/**************************************************************************
 *
 * Function:    halfprecision
 * Filename:    halfprecision.c
 * Programmer:  James Tursa
 * Version:     1.0
 * Date:        March 3, 2009
 * Copyright:   (c) 2009 by James Tursa, All Rights Reserved
 *
 *  This code uses the BSD License:
 *
 *  Redistribution and use in source and binary forms, with or without 
 *  modification, are permitted provided that the following conditions are 
 *  met:
 *
 *     * Redistributions of source code must retain the above copyright 
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright 
 *       notice, this list of conditions and the following disclaimer in 
 *       the documentation and/or other materials provided with the distribution
 *      
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE 
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 *  POSSIBILITY OF SUCH DAMAGE.
 *
 * halfprecision converts the input argument to/from a half precision floating
 * point bit pattern corresponding to IEEE 754r. The bit pattern is stored in a
 * uint16 class variable. Please note that halfprecision is *not* a class. That
 * is, you cannot do any arithmetic with the half precision bit patterns.
 * halfprecision is simply a function that converts the IEEE 754r half precision
 * bit pattern to/from other numeric MATLAB variables. You can, however, take
 * the half precision bit patterns, convert them to single or double, do the
 * operation, and then convert the result back manually.
 *
 * Note that the special cases of -Inf, +Inf, and NaN are handled correctly.
 * Also, note that the -1e30 and 1e30 values overflow the half precision format
 * and are converted into half precision -Inf and +Inf values, and stay that
 * way when they are converted back into doubles.
 *
 * For the denormalized cases, note that 2^(-24) is the smallest number that can
 * be represented in half precision exactly. 2^(-25) will convert to 2^(-24)
 * because of the rounding algorithm used, and 2^(-26) is too small and underflows
 * to zero.
 *
 ***************************************************************************/

/*!
    \file 
    \ingroup

*/ 

/**************************************************************************
/*!
	Convert a single precision floating point into an half precision one
*/	
/**************************************************************************/ 
void float16(uint16_t *output, float *input)
{
    uint16_t *hp = (uint16_t*) output; 	
    uint32_t *xp = (uint32_t*) input; 
    uint16_t	hs, he, hm;
    uint32_t	x, xs, xe, xm;
    int hes;
    

	x = *(uint32_t*) input;
	if( (x & 0x7FFFFFFF) == 0 ) 										// Signed zero   
		*hp = (uint16_t) (x >> 16);  									// Return the signed zero
	else 
	{ 
		// Not zero
		xs = x & 0x80000000;  											// Pick off sign bit
		xe = x & 0x7F800000;  											// Pick off exponent bits
		xm = x & 0x007FFFFF;  											// Pick off mantissa bits
            
		if( xe == 0 ) 													// Denormal will underflow, return a signed zero
			*hp = (uint16_t) (xs >> 16);
		else if( xe == 0x7F800000) 
		{  
			// Inf or NaN (all the exponent bits are set)
			if( xm == 0 ) 
				*hp = (uint16_t) ((xs >> 16) | 0x7C00); 				// Mantissa is zero, so convert as signed Inf
			else 
				*hp = (uint16_t) 0xFE00; 								// NaN, only 1st mantissa bit set
		}	
		else 
		{ 
			// Normalized number
			hs = (uint16_t) (xs >> 16); 							// Sign bit
			hes = ((int)(xe >> 23)) - 127 + 15; 					// Exponent unbias the single, then bias the halfp
			if( hes >= 0x1F ) 										// Overflow
				*hp = (uint16_t) ((xs >> 16) | 0x7C00); 			// Signed Inf
			else if( hes <= 0 ) 								  	// Underflow
			{
				if( (14 - hes) > 24 )								// Mantissa shifted all the way off & no rounding possibility
					hm = (uint16_t) 0;  							// Set mantissa to zero
				else
				{
					xm |= 0x00800000;  								// Add the hidden leading bit
					hm = (uint16_t) (xm >> (14 - hes)); 			// Mantissa
					if( (xm >> (13 - hes)) & 0x00000001 ) 			// Check for rounding
						hm += (uint16_t) 1; 						// Round, might overflow into exp bit, but this is OK
				}
				
				*hp = (hs | hm); 									// Combine sign bit and mantissa bits, biased exponent is zero
			} 
			else 
			{
				he = (uint16_t) (hes << 10); 						// Exponent
				hm = (uint16_t) (xm >> 13); 						// Mantissa
				if( xm & 0x00001000 ) 								// Check for rounding
					*hp = (hs | he | hm) + (uint16_t) 1; 			// Round, might overflow to inf, this is OK
				else
					*hp = (hs | he | hm);  							// No rounding
			}
		}
	}
}


/**************************************************************************
/*!
	Convert an half precision floating point into an single precision one
*/	
/**************************************************************************/ 
void float32(uint16_t *input, float *output)
{
    uint16_t *hp = (uint16_t *) input;
    uint32_t *xp = (uint32_t *) output; 
    uint16_t h, hs, he, hm;
    uint32_t xs, xe, xm;
    int32_t xes;
    int e;
    
	h = *hp;
	if( (h & 0x7FFF) == 0 )   											// Signed zero
		*xp = ((uint32_t) h) << 16;  									// Return the signed zero
	else 																// Not zero
	{ 
		hs = h & 0x8000;  												// Pick off sign bit
		he = h & 0x7C00;  												// Pick off exponent bits
		hm = h & 0x03FF;  												// Pick off mantissa bits
		if( he == 0 ) 													// Denormal will convert to normalized
		{  
			e = -1; 													// The following loop figures out how much extra to adjust the exponent
			do 
			{
				e++;
				hm <<= 1;
			} 
			while( (hm & 0x0400) == 0 ); 								// Shift until leading bit overflows into exponent bit
			
			xs = ((uint32_t) hs) << 16; 								// Sign bit
			xes = ((uint32_t) (he >> 10)) - 15 + 127 - e; 				// Exponent unbias the halfp, then bias the single
			xe = (uint32_t) (xes << 23); 								// Exponent
			xm = ((uint32_t) (hm & 0x03FF)) << 13; 						// Mantissa
			*xp++ = (xs | xe | xm); 									// Combine sign bit, exponent bits, and mantissa bits
		}
		else if( he == 0x7C00 ) 										// Inf or NaN (all the exponent bits are set)
		{  
			if( hm == 0 )  												
				*xp++ = (((uint32_t) hs) << 16) | ((uint32_t) 0x7F800000); //Mantissa is zero convert as signed Inf
			else 
				*xp++ = (uint32_t) 0xFFC00000; 							// NaN, only 1st mantissa bit set
		} 
		else 															// Normalized number
		{ 
			xs = ((uint32_t) hs) << 16; 								// Sign bit
			xes = ((int32_t) (he >> 10)) - 15 + 127; 					// Exponent unbias the halfp, then bias the single
			xe = (uint32_t) (xes << 23); 								// Exponent
			xm = ((uint32_t) hm) << 13; 								// Mantissa
			*xp++ = (xs | xe | xm); 									// Combine sign bit, exponent bits, and mantissa bits
		}
	}
}
