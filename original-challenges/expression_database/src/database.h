/*

Author: Jason Williams <jdw@cromulence.com>

Copyright (c) 2014 Cromulence LLC

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
#ifndef __DATABASE_H__
#define __DATABASE_H__

#include <stdint.h>
#include "date.h"

#define MAX_STRING_LENGTH       (128)
#define MAX_DDAP_RECORDS        (64)

#define BAD_RECORD_ERROR        (0xFFFFFFFF)

typedef struct __DDAP_RECORD_ENTRY
{
    cgc_uint32_t index;
    char szFirstName[MAX_STRING_LENGTH+1];
    char szLastName[MAX_STRING_LENGTH+1];
    char szUserName[MAX_STRING_LENGTH+1];
    cgc_tDateTime birthDate;
} cgc_tDDAPRecord;

#define GET_DB_YEAR( year ) ( year + 1900 )

void cgc_init_database( void );

cgc_uint32_t cgc_db_add_record( char *pszUserName, char *pszFirstName, char *pszLastName, cgc_tDateTime birthDate );
cgc_uint32_t cgc_db_update_record( cgc_uint32_t index, char *pszUserName, char *pszFirstName, char *pszLastName, cgc_tDateTime birthDate );
cgc_uint32_t cgc_db_remove_record( cgc_uint32_t index );
cgc_uint32_t cgc_db_get_record_count( void );

cgc_tDDAPRecord *cgc_db_search_first_name( char *pszFirstName );
cgc_tDDAPRecord *cgc_db_search_last_name( char *pszLastName );
cgc_tDDAPRecord *cgc_db_search_index( cgc_uint32_t index );
cgc_tDDAPRecord *cgc_db_search_user_name( char *pszUserName );
cgc_tDDAPRecord *cgc_db_search_birth_date( cgc_tDateTime date  );

void cgc_print_record_helper( cgc_tDDAPRecord * );

#endif // __DATABASE_H__
