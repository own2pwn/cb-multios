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

#include "file.h"

extern cgc_pfile root;

int cgc_bubble_sort( cgc_pfile parent )
{
    int outer_index = 0;
    int total_count = 0;
    cgc_pfile *list = NULL;
    cgc_pfile nl = NULL;
    int result = 0;
    
    if (parent == NULL) {
        return 0;
    }
    
    if ( parent->data == NULL ) {
        return 0;
    }
    
    /// Do an initial count on the list since it is possible
    /// that some have been freed. If this is the case then fix
    /// up the pointers and modify the length
    list = (cgc_pfile*)parent->data;
    
    while ( outer_index < parent->length ) {
        if ( list[outer_index] != NULL ) {
            total_count++;
        }
        
        outer_index++;
    }
    
    if (total_count < parent->length ) {
        outer_index = 0;
        total_count = 0;
        
        while ( outer_index < parent->length ) {
            if ( list[outer_index] != NULL ) {
                list[total_count] = list[outer_index];
                total_count++;
            }
            
            outer_index++;
        }
        
        outer_index = total_count;
        
        /// Clear out the remainders
        while ( total_count < parent->length) {
            list[total_count] = NULL;
            total_count++;
        }
        
#ifdef PATCHED
        /// Update the correct length
        parent->length = outer_index;
#endif
        
    }
    
    outer_index = 0;
    
    while ( outer_index < parent->length) {
        total_count = outer_index+1;
        
        while ( total_count < parent->length ) {
            result = cgc_strcmp( list[outer_index]->name, list[total_count]->name);
            
            if ( result > 0 ) {
                nl = list[total_count];
                list[total_count] = list[outer_index];
                list[outer_index] = nl;
            }
            
            total_count++;
        }
        outer_index++;
    }
    
    return 1;
}

int cgc_remove_sub_file( cgc_pfile parent, char *name )
{
    cgc_pfile *list = NULL;
    int index = 0;
    cgc_pfile t = NULL;
    
    if ( parent == NULL || name == NULL ) {
        return 0;
    }
    
    if ( parent->data == NULL ) {
        return 0;
    }
    
    list = (cgc_pfile*)parent->data;
    
    while ( index < parent->length ) {
        t = list[index];
        
        if ( t == NULL ) {
            index++;
            continue;
        }
        
        if ( cgc_strcmp( t->name, name ) == 0 ) {
            
            if ( t->type == DIR ) {
                cgc_printf("[ERROR] Cannot delete a directory\n");
                return 0;
            }
            
            cgc_free_file( t );
            list[index] = NULL;
            return 1;
        }
        
        index ++;
    }
    
    return 0;
}

int cgc_delete_file( char *name )
{
    cgc_pfile base = root;
    char nm[256];
    int start = 0;
    int end = 0;
    int max = 0;
    
    if ( name == NULL ) {
        return 0;
    }
    
    /// Handle base case
    if ( name[0] != '/' ) {
        if ( cgc_does_sub_file_exist( base, name ) == 0 ) {
            cgc_printf("[ERROR] Could not locate $s\n", name);
            return 0;
        }
        
        if ( cgc_remove_sub_file( base, name ) != 0 ) {
            cgc_printf("[INFO] $s removed\n", name );
            return 1;
        } else {
            return 0;
        }
    }
    
    start = 1;
    max = cgc_strlen(name);
   
    if ( max > 256 ) {
        cgc_printf("[ERROR] Name too long\n");
        return 0;
    }
 
    while ( end != -1 ) {
        end = cgc_find_next_slash( name, start, max );
        
        cgc_memset(nm, 0, 256);
        
        if ( end == -1 ) {

	    if ( (max - start) > 256 ) {
               cgc_printf("[ERROR] Size calculation failed\n");
               return 0;
	    }

            cgc_memcpy( nm, name+start, max-start);
            
            if ( cgc_does_sub_file_exist( base, nm ) == 0 ) {
                cgc_printf("[ERROR] Could not locate $s\n", name );
                return 0;
            }
            
            if ( cgc_remove_sub_file( base, nm ) != 0 ) {
                cgc_printf("[INFO] $s removed\n", nm );
                return 1;
            } else {
                return 0;
            }
        }
       
        if ( (end-start) > 256 ) {
            cgc_printf("[ERROR] Size calculation failed\n");
            return 0;
        }
 
        cgc_memcpy( nm, name+start, end-start);
        base = cgc_retrieve_sub( base, nm );
        
        if ( base == NULL ) {
            cgc_printf("[ERROR] Failed to locate directory $s\n", nm);
            return 0;
        }

	if ( base->type != DIR ) {
	    return 0;
        }
        
        start = end + 1;
    
    }
    
    return 0;
}

cgc_pfile cgc_retrieve_sub( cgc_pfile pf, char *name )
{
    cgc_pfile *list = NULL;
    int index = 0;
    cgc_pfile t = NULL;
    
    if ( pf == NULL || name == NULL ) {
        return NULL;
    }
    
    if ( pf->type != DIR ) {
        return NULL;
    }

    list = (cgc_pfile*)pf->data;
    
    while ( index < pf->length ) {
        t = list[index];
        
        if ( t == NULL ) {
            index++;
            continue;
        }
        
        if ( cgc_strcmp( t->name, name ) == 0 ) {
            return t;
        }
        
        index++;
    }
    
    return NULL;
}

int cgc_find_next_slash( char *str, int start, int max )
{
    int index = -1;
    
    if ( str == NULL ) {
        return -1;
    }
    
    while ( start < max ) {
        if ( str[start] == '/' ) {
            index = start;
            break;
        } else {
            start++;
        }
    }
    
    return index;
}

int cgc_fixup_dir_length( cgc_pfile d )
{
    char *nd = NULL;
    
    if ( d == NULL ) {
        return 0;
    }
    
    /// Base case
    if ( d->length == 0 ) {
        d->length = 1;
        
        /// 8 is used because it is rounded up anyway
        d->data = cgc_malloc( 8 );
        
        if ( d->data == NULL ) {
            cgc_printf("[ERROR] Failed to fixup dir data\n");
            d->length = 0;
            return 0;
        }
        
        cgc_memset(d->data, 0, 8);
    } else {
        /// Increment it by 1
        d->length += 1;
        
        nd = cgc_malloc( d->length * sizeof(cgc_pfile));
        
        if ( nd == NULL ) {
            d->length--;
            return 0;
        }
        
        cgc_memset(nd, 0, d->length * sizeof(cgc_pfile));
        cgc_memcpy(nd, d->data, (d->length-1)*sizeof(cgc_pfile));
        cgc_free(d->data);
        d->data = nd;
    }
    
    return 1;
}

cgc_pfile cgc_get_file( char *name )
{
    cgc_pfile cbase = root;
    cgc_pfile rv = NULL;
    int start = 0;
    int end = 0;
    int max = 0;
    
    char sdir[256];
    
    if ( name == NULL ) {
        return NULL;
    }
    
    /// Handle the base case
    if ( name[0] != '/') {
        rv = cgc_retrieve_sub( cbase, name );
        
        return rv;
    }
    
    start = 1;
    max = cgc_strlen(name);
    
    while ( end != -1 ) {
        end = cgc_find_next_slash( name, start, max );
        
        if ( end == -1 ) {
            cgc_memset( sdir, 0, 256 );
            cgc_memcpy( sdir, name+start, max-start);
            
            rv = cgc_retrieve_sub( cbase, sdir );
            
            return rv;
        }
        
        cgc_memset( sdir, 0, 256 );
        cgc_memcpy( sdir, name+start, end-start);
        
        cbase = cgc_retrieve_sub( cbase, sdir );
        
        start = end + 1;
        if ( cbase ) {
            if (cbase->type != DIR ) {
                cgc_printf("[ERROR] $s is not a directory\n", sdir );
                return NULL;
            }
        } else {
            return cbase;
        }
    }
    
    return NULL;
}

int cgc_does_sub_file_exist( cgc_pfile pf, char *name)
{
    cgc_pfile *list = NULL;
    int index = 0;
    cgc_pfile t = NULL;
    
    list = (cgc_pfile*)pf->data;
    
    while ( index < pf->length ) {
        t = list[index];
        
        if ( t == NULL ) {
            index++;
            continue;
        }
        
        if ( cgc_strcmp( t->name, name ) == 0 ) {
            return 1;
        }
        
        index++;
    }
    
    return 0;
}

int cgc_add_file_to_dir( cgc_pfile dir, cgc_pfile cgc_file )
{
    if ( dir == NULL || cgc_file == NULL ) {
        return 0;
    }
    
    if ( cgc_fixup_dir_length( dir ) == 0 ) {
        return 0;
    }
    
    ((cgc_pfile*)dir->data)[ dir->length-1] = cgc_file;
    
    return 1;
}

int cgc_add_file( cgc_pfile nf )
{
    int start = 0;
    int end = 0;
    char base[256];
    char *nd = NULL;
    int max = 0;
    cgc_pfile cbase_dir = root;
    cgc_pfile temp = NULL;
    
    if ( nf == NULL ) {
        return 0;
    }

    // If it does not start with a slash then it is just added to root
    // It is possible to have '/' in the name but it won't affect it
    if ( nf->name[0] != '/' ) {
        if ( cgc_add_file_to_dir( cbase_dir, nf ) == 0 ) {
            cgc_printf("[ERROR] Failed to add cgc_file to root\n");
            return 0;
        }
        
        return 1;
    }
    
    start = 1;
    max = cgc_strlen(nf->name);
    
    /// Handle the case where the name is just '/'
    if ( max == 1 ) {
        cgc_printf("[ERROR] You cannot add '/'\n");
        return 0;
    }
    
    while ( end != -1 ) {
        end = cgc_find_next_slash( nf->name, start, max );
        
        /// If this is the end then copy out the name and add it in
        if ( end == -1 ) {
            cgc_memset(base, 0, 256);
            cgc_memcpy( base, nf->name+start, max-start );
            cgc_memset( nf->name, 0, 256);
            cgc_memcpy( nf->name, base, max - start );
            
            if ( cgc_does_sub_file_exist( cbase_dir, nf->name) == 1) {
                cgc_printf("[ERROR] File already exists\n");
                return 0;
            }
            
            if ( cgc_add_file_to_dir( cbase_dir, nf) == 0 ) {
                cgc_printf("[ERROR] Failed to add cgc_file to $s\n", cbase_dir->name);
                return 0;
            }
            
            return 1;
        } else {
            cgc_memset( base, 0, 256);
            
            /// Copy the dir name and determine if it is valid
            cgc_memcpy( base, nf->name+start, end-start);
            
            temp = cgc_retrieve_sub( cbase_dir, base );
            
            if ( temp == NULL ) {
                cgc_printf("[ERROR] Directory $s does not exist.\n", base);
                return 0;
            } else if ( temp->type != DIR ) {
                cgc_printf("[ERROR] $s is not a directory\n", base );
                return 0;
            } else {
                cbase_dir = temp;
            }
            
            start = end + 1;
        }
    }
    return 1;
}


void cgc_free_file( cgc_pfile f )
{
    if ( f == NULL ) {
        return;
    }
    
    if ( f->data != NULL ) {
        cgc_free(f->data);
    }
    
    cgc_free(f);
    
    return;
}

cgc_pfile cgc_init_file( void )
{
	cgc_pfile new_file = NULL;

	new_file = cgc_malloc( sizeof(cgc_file) );

	if ( new_file == NULL ) {
		return new_file;
	}

	cgc_memset( new_file, 0, sizeof(cgc_file) );

	return new_file;
}

int cgc_set_name( cgc_pfile pf, char *name)
{
    int length = 0;
    
    if (pf == NULL ) {
        return 0;
    }
    
    if ( name == NULL ) {
        return 0;
    }
    
    length = cgc_strlen( name );
    
    if ( length > 255 ) {
        return 0;
    }
    
    cgc_memcpy( pf->name, name, length );
    
    return length;
}

int cgc_set_type( cgc_pfile pf, int type )
{
    if ( pf == NULL ) {
        return 0;
    }
    
    if ( type != FILE && type != DIR ) {
        return 0;
    }
    
    pf->type = type;
    
    return 1;
}

int cgc_set_data( cgc_pfile pf, int length, char *data )
{
    char *tn = NULL;
    
    if ( pf == NULL ) {
        return 0;
    }
    
    if ( data == NULL ) {
        return 0;
    }
    
    if ( length > 0x400 ) {
        return 0;
    }
    
    tn = cgc_malloc( length + 1 );
    
    if ( tn == NULL ) {
        return 0;
    }
    
    cgc_memcpy( tn, data, length + 1 );
    
    pf->length = length;
    pf->data = tn;
    
    return 1;
}
