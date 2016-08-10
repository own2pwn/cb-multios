/*

Copyright (c) 2015 Cromulence LLC

Authors: Cromulence <cgc@cromulence.com>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

*/
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <ctype.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>

#define FLAG_LEFT_JUSTIFY 	1
#define FLAG_DISPLAY_SIGN 	2
#define FLAG_ZERO_PAD		4 

#define FLAG_HEX_UPPERCASE	8
#define FLAG_FLOAT_EXPONENT	16

#define FLOAT_NON_EXPONENT_MAX		10000000000.0
#define DEFAULT_FLOAT_PRECISION		6

// Wrapper functions for cgc_vprintf and cgc_vsprintf
typedef int (*cgc_tPrintfWrapperFP)( void *ctx, int c, size_t pos );

int cgc_wrapper_output( void *ctx, cgc_tPrintfWrapperFP fpOut, size_t pos, const char *format, cgc_va_list args );

int cgc_WRAPPER_PUTC( void *ctx, int c, size_t pos )
{
        size_t tx_bytes;

        if ( transmit( STDOUT, (const void *)&c, 1, &tx_bytes ) != 0 )
                return (-1);

        return (pos+1);
}

int cgc_WRAPPER_OUTC( void *ctx, int c, size_t pos )
{
	*(((char *)ctx)+pos) = (char)c;

	return (pos+1);	
}

#define BUFFER_PUTC_MAXLEN	256
struct BUFFER_PUTC_DATA
{
	char szBuffer[BUFFER_PUTC_MAXLEN];
	cgc_uint16_t bufferPos;
};

typedef struct BUFFER_PUTC_DATA cgc_tBufferPutcData;

cgc_tBufferPutcData g_putcBuffer;

int cgc_WRAPPER_BUFFER_PUTC( void *ctx, int c, size_t pos )
{
	cgc_tBufferPutcData *pBufferData = (cgc_tBufferPutcData *)ctx;

	while ( pBufferData->bufferPos >= BUFFER_PUTC_MAXLEN )
	{
        	size_t tx_bytes;

        	if ( transmit( STDOUT, (const void *)pBufferData->szBuffer, pBufferData->bufferPos, &tx_bytes ) != 0 )
                	return (-1);

		if ( tx_bytes == 0 )
			return (-1);

		pBufferData->bufferPos -= tx_bytes;
	}		

	pBufferData->szBuffer[pBufferData->bufferPos++] = (char)c;

	return (pos+1);
}

int cgc_putchar( int c )
{
        size_t tx_bytes;

        if ( transmit( STDOUT, (const void *)&c, 1, &tx_bytes ) != 0 )
                return (-1);

        return (c);
}

int cgc_puts( const char *s )
{
	size_t tx_bytes;
	size_t s_len;
	size_t total_sent = 0;

	s_len = cgc_strlen(s);

	while (total_sent != s_len) {
		if ( transmit( STDOUT, s+total_sent, s_len-total_sent, &tx_bytes ) != 0 ) {
			return (-1);
		}
		if (tx_bytes == 0) {
			return (-1);
		}
		total_sent += tx_bytes;
	}

	cgc_putchar( '\n' );

	return (0);
}

int cgc_vprintf_buffered( const char *format, cgc_va_list args )
{
	cgc_tPrintfWrapperFP wrapper_putc_buffered = &cgc_WRAPPER_BUFFER_PUTC;

	cgc_tBufferPutcData g_putcBuffer;
	g_putcBuffer.bufferPos = 0;

	void *ctx = (void *)&g_putcBuffer;
	size_t pos = 0;

	int iReturn = cgc_wrapper_output( ctx, wrapper_putc_buffered, pos, format, args );

	if ( g_putcBuffer.bufferPos > 0 )
	{
        	size_t tx_bytes;

        	if ( transmit( STDOUT, (const void *)g_putcBuffer.szBuffer, g_putcBuffer.bufferPos, &tx_bytes ) != 0 )
                	return (-1);

		g_putcBuffer.bufferPos = 0;
	}

	return iReturn;
}

int cgc_printf( const char *format, ... )
{
	cgc_va_list args;
	va_start(args, format);

#if ENABLE_BUFFERED_PRINTF
	int return_val = cgc_vprintf_buffered( format, args );
#else
	int return_val = cgc_vprintf( format, args );
#endif

	va_end(args);

	return (return_val);	
}

int cgc_vprintf( const char *format, cgc_va_list args )
{
	cgc_tPrintfWrapperFP wrapper_putc = &cgc_WRAPPER_PUTC;
	void *ctx = NULL;
	size_t pos = 0;

	return cgc_wrapper_output( ctx, wrapper_putc, pos, format, args );	
}

int cgc_sprintf( char *buf, const char *format, ... )
{
	cgc_va_list args;
	va_start(args, format);

	int return_val = cgc_vsprintf( buf, format, args );

	va_end(args);

	return (return_val);	
}

int cgc_vsprintf( char *buf, const char *format, cgc_va_list args )
{
	cgc_tPrintfWrapperFP wrapper_outc = &cgc_WRAPPER_OUTC;
	void *ctx = buf;
	size_t pos = 0;

	int iReturnValue = cgc_wrapper_output( ctx, wrapper_outc, pos, format, args );

	(*wrapper_outc)( ctx, '\0', iReturnValue );

	return iReturnValue;
}

int cgc_DebugPrint( const char *format, ... )
{
  cgc_va_list args;
  va_start(args, format);

  char buf[2000];
  int return_val = cgc_vsprintf( buf, format, args );

  va_end(args);

  transmit(STDERR, buf, cgc_strlen(buf), NULL);

  return (return_val);  
}

// NOTE This is reversed -- it will be printed in reverse by the cgc_printf helper!
size_t cgc_printf_int_to_string( cgc_uint32_t val, cgc_uint32_t base, char *str, cgc_int32_t flags )
{
	size_t pos = 0;
	cgc_int32_t n;

	if ( val == 0 )
	{
		str[0] = '0';
		return 1;
	}

	while ( val > 0 )
	{
		n = val % base;
		val = val / base;

		if ( base == 16 )
		{
			if ( n < 10 )
				str[pos++] = '0' + n;
			else
			{
				if ( flags & FLAG_HEX_UPPERCASE )
					str[pos++] = 'A' + (n-10);
				else
					str[pos++] = 'a' + (n-10);
			}
		}
		else
			str[pos++] = '0' + n;	
	}

	return (pos);
}
					
size_t cgc_printf_helper_int( void *ctx, cgc_tPrintfWrapperFP fpOut, size_t pos, cgc_int32_t val, cgc_uint32_t base, cgc_int32_t width, cgc_int32_t precision, cgc_int32_t flags )
{
	size_t max_printlen = 0;
	size_t pad_length = 0;
	cgc_int8_t is_negative = 0;
	size_t character_count = 0;
	char temp_str[32];

	if ( base == 10 && val < 0 )
	{
		is_negative = 1;
		val = -val;

		max_printlen++;
	}

	character_count = cgc_printf_int_to_string( (cgc_uint32_t)val, base, temp_str, flags );
	max_printlen += character_count;

	if ( width > 0 )
	{
		if ( max_printlen < width )
			pad_length = width-max_printlen;
	}

	// Precision will override width	
	if ( precision > 0 )
	{
		flags |= FLAG_ZERO_PAD;
		if ( character_count < precision )
			pad_length = precision-character_count;
	}

	if ( !(flags & FLAG_LEFT_JUSTIFY) )
	{
		if ( is_negative && pad_length > 0 && (flags & FLAG_ZERO_PAD) )
		{
			pos = (*fpOut)( ctx, '-', pos );
			is_negative = 0;
		}

		while ( pad_length-- > 0 )
		{
			if ( (flags & FLAG_ZERO_PAD) )
				pos = (*fpOut)( ctx, '0', pos );
			else	
				pos = (*fpOut)( ctx, ' ', pos );	
		}
	}

	if ( is_negative )
	{
		pos = (*fpOut)( ctx, '-', pos );
		is_negative = 0;
	}

	size_t i = character_count;
	while ( i > 0 )
	{
		pos = (*fpOut)( ctx, temp_str[i-1], pos );
		i--;
	}
	
	if ( (flags & FLAG_LEFT_JUSTIFY) )
	{
		while ( pad_length-- > 0 )
			pos = (*fpOut)( ctx, ' ', pos );
	}

	return pos;	
}

size_t cgc_printf_float_to_string( double val, cgc_uint8_t fraction_precision_digit_count, char *str, cgc_int32_t flags )
{
	size_t pos = 0;
	cgc_int32_t n;
	
	double display_precision = pow( 10.0, -fraction_precision_digit_count );
	
	if ( val == 0.0 )
	{
		str[pos++] = '0';
		str[pos++] = '.';

		for ( cgc_uint8_t i = 0; i < fraction_precision_digit_count; i++ )
			str[pos++] = '0';
		
		if ( flags & FLAG_FLOAT_EXPONENT )
		{
			str[pos++] = 'e';
			str[pos++] = '+';
			str[pos++] = '0';
			str[pos++] = '0';
			str[pos++] = '0';
		}

		return pos;
	}
	else if ( isnan( val ) )
	{
		str[pos++] = 'N';
		str[pos++] = 'a';
		str[pos++] = 'N';
		return pos;
	}
	else if ( isinf( val ) )
	{
		str[pos++] = 'I';
		str[pos++] = 'N';
		str[pos++] = 'F';
		return pos;
	}
	
	// Impose a maximal amount before switching to exponent mode
	if ( val >= FLOAT_NON_EXPONENT_MAX )
		flags |= FLAG_FLOAT_EXPONENT;
	
	if ( val < 0.0 )
		val = val - (display_precision * 0.5);
	else
		val = val + (display_precision * 0.5);

	// Calculate magnitude!
	cgc_int16_t magnitude = log10( val );

	// Calculate cgc_round position
	if ( flags & FLAG_FLOAT_EXPONENT )
	{
		double new_round_precision;
	
	 	cgc_int16_t round_position = magnitude - fraction_precision_digit_count;

		if ( val < 1.0 )
			new_round_precision = pow( 10, round_position-1 );
		else
			new_round_precision = pow( 10, round_position );

		//if ( new_round_precision < display_precision )
		display_precision = new_round_precision;
	}	

	// HANDLE negative
	if ( val < 0.0 )
	{
		val = -val;
		str[pos] = '-';
		pos++;
	}


	// Will be set to magnitude on first digit...	
	cgc_int16_t exponent_value = 0;

	cgc_uint16_t fraction_count = 0;
	cgc_int8_t is_fraction_digits = 0;

	if ( val < display_precision )
	{
		str[pos++] = '0';
		str[pos++] = '.';
		
		is_fraction_digits = 1;
	} 
	else if ( magnitude < 0 && val > display_precision && !(flags & FLAG_FLOAT_EXPONENT) )
	{
		str[pos++] = '0';
		str[pos++] = '.';

		cgc_int16_t temp_zero_count = magnitude;
		while ( ++temp_zero_count < 0 )
		{
			str[pos++] = '0';
			fraction_count++;
		}
		is_fraction_digits = 1;
	}

	while ( magnitude >= 0 || val > display_precision )
	{
		double divider = pow( 10.0, magnitude );

		if ( divider > 0.0 && !isinf(divider) )
		{
			cgc_uint8_t digit = (cgc_uint8_t)cgc_floor( val / divider );
			val -= ((double)digit * divider);

			if ( flags & FLAG_FLOAT_EXPONENT && is_fraction_digits == 0 )
			{
				// First digit... print it followed by a decimal...
				if ( exponent_value == 0 )
					exponent_value = magnitude;
				
				if ( digit == 0 )
					exponent_value--;
				else
				{
					str[pos++] = '0' + digit;
					str[pos++] = '.';

					// Remember we are in fraction mode now (exponent mode) -- to terminate at display precision
					is_fraction_digits = 1;
				}
			}
			else
			{
				if ( magnitude < 0.0 && is_fraction_digits == 0 )
				{
					str[pos++] = '0';
					str[pos++] = '.';

					is_fraction_digits = 1;
				}

				str[pos++] = '0' + digit;

				if ( is_fraction_digits )
					fraction_count++;	
			}
		}

		if ( magnitude == 0.0 && val > 0.0 && !(flags & FLAG_FLOAT_EXPONENT) )
		{
			str[pos++] = '.';

			is_fraction_digits = 1;
		}

		if ( fraction_count >= fraction_precision_digit_count )
			break;

		magnitude--;
	}

	while ( is_fraction_digits && fraction_count < fraction_precision_digit_count )
	{
		// ADD 0's
		str[pos++] = '0';
		fraction_count++;
	}

	if ( (flags & FLAG_FLOAT_EXPONENT) )
	{
		// ADD exponent
		str[pos++] = 'e';

		if ( exponent_value < 0 )
		{
			exponent_value = -exponent_value;
			str[pos++] = '-';
		}
		else
			str[pos++] = '+';

		if ( exponent_value == 0 )
		{
			for ( cgc_uint8_t i = 0; i < 3; i++ )
				str[pos++] = '0';
		}
		else
		{
			cgc_uint8_t exponent_digit_count = 0;
			cgc_uint16_t exponent_magnitude = log10( exponent_value );
		
			for ( cgc_uint8_t i = exponent_magnitude; i < 2; i++ )
			{
				str[pos++] = '0';
				exponent_digit_count++;
			}

			while ( exponent_digit_count++ < 3 )
			{
				cgc_uint16_t exponent_divider = pow( 10, exponent_magnitude );

				cgc_uint8_t exponent_digit = (exponent_value / exponent_divider);
				str[pos++] = '0' + exponent_digit;

				exponent_value -= (exponent_digit * exponent_divider) ;
				exponent_magnitude--;
			}	
		}
	}	

	return (pos);	
}

size_t cgc_printf_helper_float( void *ctx, cgc_tPrintfWrapperFP fpOut, size_t pos, double val, cgc_int32_t width, cgc_int32_t precision, cgc_int32_t flags )
{
	size_t max_printlen = 0;
	size_t pad_length = 0;
	cgc_int8_t is_negative = 0;
	size_t character_count = 0;
	char temp_str[32];

	if ( val < 0.0 )
	{
		val = -val;
		is_negative = 1;

		max_printlen++;
	}

	if ( precision == 0 )
		character_count = cgc_printf_float_to_string( val, DEFAULT_FLOAT_PRECISION, temp_str, flags );
	else
		character_count = cgc_printf_float_to_string( val, precision, temp_str, flags );
	
	max_printlen += character_count;

	if ( width > 0 )
	{
		if ( max_printlen < width )
			pad_length = width-max_printlen;
	}

	if ( !(flags & FLAG_LEFT_JUSTIFY) )
	{
		if ( is_negative && pad_length > 0 && (flags & FLAG_ZERO_PAD) )
		{
			pos = (*fpOut)( ctx, '-', pos );
			is_negative = 0;
		}

		while ( pad_length-- > 0 )
		{
			if ( (flags & FLAG_ZERO_PAD) )
				pos = (*fpOut)( ctx, '0', pos );
			else	
				pos = (*fpOut)( ctx, ' ', pos );	
		}
	}

	if ( is_negative )
	{
		pos = (*fpOut)( ctx, '-', pos );
		is_negative = 0;
	}

	for ( size_t i = 0; i < character_count; i++ )	
		pos = (*fpOut)( ctx, temp_str[i], pos );

	if ( (flags & FLAG_LEFT_JUSTIFY) )
	{
		while ( pad_length-- > 0 )
			pos = (*fpOut)( ctx, ' ', pos );
	}

	return (pos);
}
				
size_t cgc_printf_helper_string( void *ctx, cgc_tPrintfWrapperFP fpOut, size_t pos, const char *outStr, cgc_int32_t width, cgc_int32_t precision, cgc_int32_t flags )
{
	if ( precision == 0 && width == 0 )
	{
		// Fast print
		while ( *outStr != '\0' )
		{
			pos = (*fpOut)( ctx, *outStr, pos );
			outStr++;
		}

		return (pos);
	}

	size_t max_printlen = cgc_strlen( outStr );
	size_t pad_length = 0;

	if ( precision > 0 )
	{
		if ( max_printlen > precision )
			max_printlen = precision;
	}

	if ( width > 0 )
	{
		if ( max_printlen < width )
			pad_length = width-max_printlen;
	}

	if ( !(flags & FLAG_LEFT_JUSTIFY) )
	{
		while ( pad_length-- > 0 )
		{
			if ( (flags & FLAG_ZERO_PAD) )
				pos = (*fpOut)( ctx, '0', pos );
			else	
				pos = (*fpOut)( ctx, ' ', pos );	
		}
	}

	// Output string up to maxlength
	while ( max_printlen-- > 0 )
		pos = (*fpOut)( ctx, *outStr++, pos );

	if ( (flags & FLAG_LEFT_JUSTIFY) )
	{
		while ( pad_length-- > 0 )
			pos = (*fpOut)( ctx, ' ', pos );
	}

	return pos;	
}

int cgc_wrapper_output( void *ctx, cgc_tPrintfWrapperFP fpOut, size_t pos, const char *format, cgc_va_list args )
{

	cgc_int32_t flags = 0;
	cgc_int32_t width = 0;
	cgc_int32_t pad_length = 0;
	cgc_int32_t precision = 0;

	while ( *format != '\0' )
	{
		char curChar = *format;
		format++;

		if ( curChar == '$' )
		{
			flags = width = pad_length = precision = 0;

			if ( *format == '\0' )
				break;

			if ( *format == '$' )
			{
				// Emit %
				pos = (*fpOut)( ctx, '$', pos );
				continue;
			}

			if ( *format == '-' )
			{
				flags |= FLAG_LEFT_JUSTIFY;

				format++;

				if ( *format == '\0' )
					break;
			}

			// Check width
			if ( cgc_isdigit( *format ) )
			{
				if ( *format == '0' )
					flags |= FLAG_ZERO_PAD;

				const char *startpos = format;
				while ( cgc_isdigit( *format ) )
					format++;

				width = cgc_atoi( startpos );

				if ( *format == '\0' )
					break;
			}

			// Check precision
			if ( *format == '.' )
			{
				format++;
				
				if ( *format == '\0' )
					break;

				const char *startpos = format;
				while ( cgc_isdigit( *format ) )
					format++;

				precision = cgc_atoi( startpos );

				if ( *format == '\0' )
					break;
			}

			switch ( *format )
			{
			case 's':
				{
					// String
					const char *print_str = va_arg( args, char * );
					pos = cgc_printf_helper_string( ctx, fpOut, pos, print_str, width, precision, flags );
				}
				break;

			case 'd':
				{
					// Print integer
					cgc_int32_t print_int = va_arg( args, cgc_int32_t );
					pos = cgc_printf_helper_int( ctx, fpOut, pos, print_int, 10, width, precision, flags );	
				}
				break;

			case 'x':
				{
					// Print hex (lower case)
					cgc_int32_t print_int = va_arg( args, cgc_int32_t );
					pos = cgc_printf_helper_int( ctx, fpOut, pos, print_int, 16, width, precision, flags );
				}
				break;

			case 'X':
				{
					// Print hex (upper case)
					flags |= FLAG_HEX_UPPERCASE;

					cgc_int32_t print_int = va_arg( args, cgc_int32_t );
					pos = cgc_printf_helper_int( ctx, fpOut, pos, print_int, 16, width, precision, flags );
				}
				break;

			case 'f':
				{
					// Print float
					double print_float = va_arg( args, double );
					pos = cgc_printf_helper_float( ctx, fpOut, pos, print_float, width, precision, flags );	
				}
				break;

			case 'e':
				{
					// Print float -- force exponent mode
					flags |= FLAG_FLOAT_EXPONENT;

					double print_float = va_arg( args, double );
					pos = cgc_printf_helper_float( ctx, fpOut, pos, print_float, width, precision, flags );
				}
				break;

			case 'n':
				{
					cgc_int32_t *signed_int_p = va_arg( args, cgc_int32_t* );

					(*signed_int_p) = pos;
				}
				break;

			case 'c':
				{
					// Print single char
					char char_arg = (char )va_arg( args, int );	
				
					char temp_str[2];
					temp_str[0] = char_arg;
					temp_str[1] = '\0';

					pos = cgc_printf_helper_string( ctx, fpOut, pos, temp_str, width, 0, flags );
				}
				break;

			default:
				// Unsupported
				break;
			}

			format++;	
		}
		else
			pos = (*fpOut)( ctx, curChar, pos );
	}

	return (pos);
}