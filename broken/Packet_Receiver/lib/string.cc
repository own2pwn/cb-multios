
extern "C"
{
#include <libcgc.h>
#include <stdlib.h>
}

#include "string.h"

#ifdef NULL
#undef NULL
#define NULL (0)
#endif

cgc_String::cgc_String()
{
    m_length = 0;
    m_pData = NULL;
}

cgc_String::cgc_String( const char *instr )
{
    size_t alloc_length = cgc_strlen( instr ) + 1;
    m_pData = new char[alloc_length];

    cgc_memcpy( (void *)m_pData, (void *)instr, alloc_length-1 );
    m_pData[alloc_length-1] = '\0';

    m_length = alloc_length-1;
}

cgc_String::cgc_String( const cgc_String &instr )
{
    size_t alloc_length = instr.m_length+1;
    m_pData = new char[ alloc_length ];

    cgc_memcpy( m_pData, instr.m_pData, instr.m_length );

    m_pData[alloc_length-1] = '\0';
    m_length = instr.m_length;
}

cgc_String::~cgc_String()
{
    if ( m_pData )
        delete [] m_pData;
}

cgc_String& cgc_String::operator+=( const cgc_String &rhs )
{
    size_t alloc_length = m_length + rhs.m_length + 1;
    char *pNewData = new char[alloc_length];

    cgc_memcpy( pNewData, m_pData, m_length );
    cgc_memcpy( pNewData+m_length, rhs.m_pData, rhs.m_length );

    if ( m_pData )
        delete [] m_pData;

    m_pData = pNewData;
    m_length = alloc_length-1;

    return *this;
}

const cgc_String cgc_String::operator+( const cgc_String &rhs ) const
{
    return cgc_String(*this) += rhs;
}

const char *cgc_String::cgc_c_str( void ) const
{
    return (m_pData);
}

size_t cgc_String::cgc_length( void )
{
    return (m_length);
}

bool cgc_String::cgc_empty( void )
{
    return (m_length == 0);
}

cgc_String cgc_String::cgc_upper( void ) const
{
    cgc_String upperStr = *this;

    for ( size_t i = 0; i < upperStr.m_length; i++ )
        upperStr.m_pData[i] = cgc_toupper( upperStr.m_pData[i] );

    return (upperStr);
}

cgc_String cgc_String::cgc_lower( void ) const
{
    cgc_String lowerStr = *this;

    for ( size_t i = 0; i < lowerStr.m_length; i++ )
        lowerStr.m_pData[i] = cgc_tolower( lowerStr.m_pData[i] );

    return (lowerStr);
}

cgc_String cgc_String::cgc_StripSpaces( void ) const
{
    // Very inefficient method of stripping spaces
    size_t i;
    for ( i = 0; i < m_length; i++ )
    {
        if ( m_pData[i] != ' ' )
            break;
    }

    return (cgc_SubString( i, cgc_String::STRING_END_POS ));
}

cgc_String cgc_String::cgc_Trim( size_t cgc_length )
{
    *this = cgc_SubString( 0, cgc_length );

    return (*this);
}

cgc_String cgc_String::cgc_SubString( size_t startPos, size_t endPos ) const
{
    if ( startPos >= m_length )
        return cgc_String("");

    if ( endPos > m_length )
        endPos = m_length;

    if ( startPos >= endPos )
        return cgc_String("");

    size_t new_length = endPos - startPos;
    char *pszNewStr = new char[new_length+1];

    // Copy in string
    size_t copyPos = 0;
    for ( size_t curPos = startPos; curPos < endPos; curPos++ )
        pszNewStr[copyPos++] = m_pData[curPos];

    pszNewStr[copyPos] = '\0';

    return cgc_String(pszNewStr);
}

bool cgc_String::operator==( const cgc_String &rhs ) const
{
    if ( m_length != rhs.m_length )
        return (false);

    for ( size_t i = 0; i < m_length; i++ )
        if ( m_pData[i] != rhs.m_pData[i] )
            return (false);

    return (true);
}

bool cgc_String::operator!=( const cgc_String &rhs ) const
{
    return !(*this == rhs);
}

void cgc_String::operator=( const cgc_String &rhs )
{
    if ( m_pData )
        delete [] m_pData;

    size_t alloc_length = rhs.m_length+1;
    m_pData = new char[ alloc_length ];

    cgc_memcpy( m_pData, rhs.m_pData, rhs.m_length );

    m_pData[alloc_length-1] = '\0';
    m_length = rhs.m_length;
}

void cgc_String::cgc_SetString( const char *pszBuffer )
{
    if ( m_pData )
        delete [] m_pData;

    size_t alloc_length = cgc_strlen( pszBuffer ) + 1;
    m_pData = new char[alloc_length];

    cgc_memcpy( (void *)m_pData, (void *)pszBuffer, alloc_length-1 );
    m_pData[alloc_length-1] = '\0';

    m_length = alloc_length-1;
}
