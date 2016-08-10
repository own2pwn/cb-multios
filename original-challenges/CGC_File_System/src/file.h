/*
 Copyright (c) 2014 Cromulence LLC
 
 Permission is hereby granted, cgc_free of charge, to any person obtaining a copy
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

#ifndef __FILE_H__
#define __FILE_H__

#include <libcgc.h>
#include <stdlib.h>
#include <malloc.h>

#define FILE 0
#define DIR 1

typedef struct cgc_file {
	char name[256];
	unsigned int type;
	unsigned int length;
	char *data;
} cgc_file, *cgc_pfile;

/// Allocates the cgc_file structure and zeros the buffer
cgc_pfile cgc_init_file( void );

/// Sets the files name to that specified by name
int cgc_set_name( cgc_pfile pf, char *name);

/// Sets the type of cgc_file as indicated by type only FILE or DIR are valid
int cgc_set_type( cgc_pfile pf, int type);

/// Allocates a new copy of the data buffer and inserts it into the structure
int cgc_set_data( cgc_pfile pf, int length, char *data );

/// Frees the cgc_file structure and associated data
void cgc_free_file( cgc_pfile pf );

/// Adds a cgc_file to the root node
int cgc_add_file( cgc_pfile nf );

/// Retrieves the cgc_file specified by name
cgc_pfile cgc_get_file( char *name );

/// Reallocates the cgc_pfile list to allow an additional cgc_file
int cgc_fixup_dir_length( cgc_pfile d );

/// Utility function to search for the next '/' in a string
int cgc_find_next_slash( char *str, int start, int max );

/// Retrieves the cgc_pfile beneath a given directory if it exists
cgc_pfile cgc_retrieve_sub( cgc_pfile pf, char *name );

/// Frees the cgc_file structure associated with name
int cgc_delete_file( char *name );

/// Returns 0 if cgc_file is not a sub 1 if it is
int cgc_does_sub_file_exist( cgc_pfile pf, char *name);

/// Bubble sort the files of a directory
int cgc_bubble_sort( cgc_pfile parent );

#endif