/*
 * Copyright (C) Narf Industries <info@narfindustries.com>
 *
 * Permission is hereby granted, cgc_free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef PEOPLE_H
#define PEOPLE_H 1

#include <libcgc.h>


typedef struct person cgc_Person;
typedef struct relation cgc_Relation;
typedef struct search cgc_Search;

typedef enum {  RT_NONE = 0, 
				RT_SELF = 1, 
				RT_MOTHER = 2,
				RT_FATHER = 4,
				RT_ADOPTING_PARENT = 8,
				RT_ADOPTED_CHILD = 16,
				RT_BIO_CHILD = 32,
				RT_PARTNER = 64,
				RT_FORMER = 128,
			} cgc_R_TYPES;

#define PERSON_UNKNOWN  0x00000000
#define RELATED 		0x10000001
#define NOT_RELATED 	0x14000041

struct relation {
	cgc_uint32_t person_id;		// 0x00000000 (Unknown)
	cgc_Person *person;			// NULL when id is Unknown
};

struct search {
	cgc_Person   *p;		// The person being processed
	cgc_uint32_t  visited;	// bit map of relations in this person which have been processed (bitmap of cgc_R_TYPES)
};


typedef void __attribute__((regparm(2))) (*cgc_shift_relation_fn)(cgc_Person *p, cgc_uint32_t id);

struct person {
	cgc_uint32_t id;
	struct {
		cgc_uint16_t birth_year;
		cgc_uint16_t death_year; 	// 0 means still alive
	} lifecycle;
	struct {
		cgc_uint8_t fname[10];
		cgc_uint8_t mname[10];
		cgc_uint8_t lname[10];
	} name;
	struct {
		cgc_Relation adopted[10];
		cgc_Relation biological[10];
	} children;
	struct {
		cgc_Relation biological_mother;
		cgc_Relation biological_father;
		cgc_Relation adopting[2];
	} parents;
	struct {
		cgc_Relation current;		// we'll ignore polygomy
		cgc_Relation former[2];
	} partners;
	cgc_shift_relation_fn shift;
	cgc_Person *next;
};

// qty of input data required to create a person (id, birth_year, fname, mname, lname)
#define PERSON_IN_SZ (sizeof(cgc_uint32_t) + sizeof(cgc_uint16_t) + 3*10*sizeof(cgc_uint8_t))

cgc_Person *cgc_new_person(char *input);
void cgc_add_person_to_list(cgc_Person **p_list, cgc_Person *p);
cgc_Person *cgc_get_person_by_id(cgc_Person *p_list, cgc_uint32_t id);
cgc_uint32_t cgc_count_people(cgc_Person *p_list);


#endif
