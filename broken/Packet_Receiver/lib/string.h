#ifndef __STRING_H__
#define __STRING_H__

class cgc_String
{
public:
    cgc_String();
    cgc_String( const char *instr );
    cgc_String( const cgc_String &instr );
    ~cgc_String();

    cgc_String& operator+=( const cgc_String &rhs );
    const cgc_String operator+( const cgc_String &rhs ) const;
    const char *cgc_c_str( void ) const;

    bool operator==( const cgc_String &rhs ) const;
    bool operator!=( const cgc_String &rhs ) const;
    void operator=( const cgc_String &rhs );
    void cgc_SetString( const char *pszBuffer );

    cgc_String cgc_upper( void ) const;
    cgc_String cgc_lower( void ) const;

    cgc_String cgc_Trim( size_t cgc_length );
    cgc_String cgc_SubString( size_t startPos, size_t endPos ) const;
    cgc_String cgc_StripSpaces( void ) const;

    size_t cgc_length( void );
    bool cgc_empty( void );

public:
    const static size_t STRING_END_POS = 0xFFFFFFFF;

private:
    size_t m_length;
    char *m_pData;
};

#endif // __STRING_H__
