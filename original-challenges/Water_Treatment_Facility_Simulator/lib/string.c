#include <string.h>
#include <stdint.h>

size_t cgc_strlen( const char *str )
{
	size_t len = 0;
	while ( *str++ != '\0' )
		len++;

	return len;
}

void cgc_bzero(void *s, size_t n) {
        while (n) {
                ((char *)s)[--n] = '\0';
        }
        ((char *)s)[n] = '\0';
}

void *cgc_memset( void *ptr, int value, size_t num )
{
	void *ptr_temp = ptr;
	cgc_uint8_t set_value_byte = (cgc_uint8_t)value;
	cgc_uint32_t set_value_dword = (set_value_byte << 24) | (set_value_byte << 16) | (set_value_byte << 8) | set_value_byte;

	while ( num >= 4 )
	{
		*((cgc_uint32_t*)ptr++) = set_value_dword;	
		num-=4;	
	}

	while ( num > 0 )
	{
		*((cgc_uint8_t*)ptr++) = set_value_byte;	
		num--;
	}

	return (ptr_temp);
}