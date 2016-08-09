#ifndef __SERVICE_H__
#define __SERVICE_H__

#include <libcgc.h>
#include <stdlib.h>

#define SET 0
#define VALUE 1

#define UNION 1
#define INTERSECT 2
#define SETDIFF 3
#define POWERSET 4
#define CARTESIAN 5

typedef enum cgc_setState
{
	start = 0,
	open_set,
	close_set,
	open_double,
	close_double,
	new_var,
	set_var,
	set_end,
	error

} cgc_setState;

/**
 * Structure to hold the initial command line parsing
 * 	Something like  var = |"2","3"| will be split
 *	up into "var", "=", and '|"2","3"|'
 **/
typedef struct cgc_splitCommand
{
	char *left;
	char mid;
	char *right;
} cgc_splitCommand, *cgc_psplitCommand;

/**
 * Structure to store an individual element of a set
 *	The primary purpose of this structure is to
 *	indicate whether or not the element is a set
 *	as well.
 **/
typedef struct cgc_setElement
{
	/// Value of the element. This can be an actual
	///	value or name of a set
	char *value;

	/// Type. Either 0 for set or 1 for value
	int type;
} cgc_setElement, *cgc_psetElement;

/**
 * This set will be a doubly linked list to each declared
 *	set variable.
 **/
typedef struct cgc_setArray
{
	/// Pointer to the next set in the list
	struct cgc_setArray *next;

	/// Pointer to the previous set in the list
	struct cgc_setArray *prev;

	/// Set variable name
	char varName[0x10];

	/// Number of elements within the set
	int varCount;

	/// Pointer to an array of cgc_setElement pointers. The size
	///	of this array is indicated by varCount with a max of 15
	cgc_psetElement sElems[15];

} cgc_setArray, *cgc_psetArray;


/*********************************************************
 * Function prototypes
 *********************************************************/

cgc_psetArray cgc_symmetric_difference( cgc_psetArray setone, cgc_psetArray settwo );
cgc_psetArray cgc_set_difference( cgc_psetArray setone, cgc_psetArray settwo );
cgc_psetArray cgc_unionset( cgc_psetArray setone, cgc_psetArray settwo );
cgc_psetArray cgc_retrieve_set( char *setname );

#endif
