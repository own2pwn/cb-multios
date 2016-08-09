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

#include <libcgc.h>
#include <stdint.h>
#include "libc.h"
#include "char.h"
#include "strlen.h"
#include "memcpy.h"


#include "breakfast_objects.h"

const char *typeName[] = {"cgc_Liquids", "cgc_Cereals", "cgc_Toppings", "cgc_GenericString",
							"cgc_PrinterString", "cgc_CommandRunner"};

// Serialized content, SVU as UINT32, STI as UINT32, and name as series of chars.

/**
 * Set values in a cgc_Liquids object
 *
 * @param g 				cgc_Liquids object (this)
 * @param serialVersionUID 	UID of this serialized type
 * @param subTypeID			subTypeID number
 * @return 0 on SUCCESS
 */
int cgc_set_data_liquids(cgc_Liquids *l, cgc_svu_t serialVersionUID, cgc_l_sti_t subTypeID) {
	l->serialVersionUID = serialVersionUID;
	cgc_memcpy(l->typeName, typeName[LIQUID], cgc_strlen(typeName[LIQUID]));
	l->subTypeID = subTypeID;
	return 0;
}

/**
 * Destroy a cgc_Liquids object
 *
 * @param l 	cgc_Liquids object (this)
 */
void cgc_destructor_liquids(cgc_Liquids *l) {
	cgc_free(l);
}

cgc_Liquids *cgc_constructor_liquids(void) {

	cgc_Liquids *l = cgc_malloc(sizeof(cgc_Liquids));
	MALLOC_OK(l);
	l->set_data = &cgc_set_data_liquids;
	l->destroy = &cgc_destructor_liquids;
	return l;
}

/**
 * Set values in a cgc_Cereals object
 *
 * @param g 				cgc_Cereals object (this)
 * @param serialVersionUID 	UID of this serialized type
 * @param subTypeID			subTypeID number
 * @return 0 on SUCCESS
 */
int cgc_set_data_cereals(cgc_Cereals *c, cgc_svu_t serialVersionUID, cgc_c_sti_t subTypeID) {
	c->serialVersionUID = serialVersionUID;
	cgc_memcpy(c->typeName, typeName[CEREAL], cgc_strlen(typeName[CEREAL]));
	c->subTypeID = subTypeID;
	return 0;
}

/**
 * Destroy a cgc_Cereals object
 *
 * @param c 	cgc_Cereals object (this)
 */
void cgc_destructor_cereals(cgc_Cereals *c) {
	cgc_free(c);
}

cgc_Cereals *cgc_constructor_cereals(void) {

	cgc_Cereals *c = cgc_malloc(sizeof(cgc_Cereals));
	MALLOC_OK(c);
	c->set_data = &cgc_set_data_cereals;
	c->destroy = &cgc_destructor_cereals;
	return c;
}

/**
 * Set values in a cgc_Toppings object
 *
 * @param g 				cgc_Toppings object (this)
 * @param serialVersionUID 	UID of this serialized type
 * @param subTypeID			subTypeID number
 * @return 0 on SUCCESS
 */
int cgc_set_data_toppings(cgc_Toppings *t, cgc_svu_t serialVersionUID, cgc_t_sti_t subTypeID) {
	t->serialVersionUID = serialVersionUID;
	cgc_memcpy(t->typeName, typeName[TOPPINGS], cgc_strlen(typeName[TOPPINGS]));
	t->subTypeID = subTypeID;
	return 0;
}

/**
 * Destroy a cgc_Toppings object
 *
 * @param t 	cgc_Toppings object (this)
 */
void cgc_destructor_toppings(cgc_Toppings *t) {
	cgc_free(t);
}

cgc_Toppings *cgc_constructor_toppings(void) {

	cgc_Toppings *t = cgc_malloc(sizeof(cgc_Toppings));
	MALLOC_OK(t);
	t->set_data = &cgc_set_data_toppings;
	t->destroy = &cgc_destructor_toppings;
	return t;
}

/**
 * Set values in a cgc_GenericString object
 *
 * @param g 				cgc_GenericString object (this)
 * @param serialVersionUID 	UID of this serialized type
 * @param str 				String to load into object
 * @return 0 on SUCCESS
 */
int cgc_set_data_generic_string(cgc_GenericString *g, cgc_svu_t serialVersionUID, char *str) {
	unsigned int s_len = cgc_strlen(str);
	g->serialVersionUID = serialVersionUID;
	cgc_memcpy(g->typeName, typeName[GENERIC_STRING], cgc_strlen(typeName[GENERIC_STRING]));
	// no protection against very very long strings
	g->str = cgc_malloc(s_len);
	MALLOC_OK(g->str);
	// no requirements on str content (except NULL is the terminator)
	cgc_memcpy(g->str, str, s_len);
	return 0;
}

/**
 * Destroy a cgc_GenericString object
 *
 * @param g 	cgc_GenericString object (this)
 */
void cgc_destructor_generic_string(cgc_GenericString *g) {
	if (NULL != g->str)
		cgc_free(g->str);
	cgc_free(g);
}

/**
 * Construct an empty cgc_GenericString object
 *
 * @return VA of object
 */
cgc_GenericString *cgc_constructor_generic_string(void) {
	cgc_GenericString *g = cgc_calloc(sizeof(cgc_GenericString));
	MALLOC_OK(g);
	g->set_data = &cgc_set_data_generic_string;
	g->destroy = &cgc_destructor_generic_string;
	return g;
}

/**
 * Set values in a cgc_PrinterString object (printable ASCII chars)
 *
 * @param p 				cgc_PrinterString object (this)
 * @param serialVersionUID 	UID of this serialized type
 * @param str 				String to load into object
 * @return 0 on SUCCESS, else -1 on error
 */
int cgc_set_data_printer_string(cgc_PrinterString *p, cgc_svu_t serialVersionUID, char *str) {
	unsigned int s_len = cgc_strlen(str);
	p->serialVersionUID = serialVersionUID;
	cgc_memcpy(p->typeName, typeName[PRINTER_STRING], cgc_strlen(typeName[PRINTER_STRING]));

	for (unsigned int i = 0; i < s_len; i++) {
		if (FALSE == cgc_is_printable((unsigned char)str[i])) {
			return -1;
		}
	}

	p->str = cgc_malloc(s_len);
	MALLOC_OK(p->str);
	cgc_memcpy(p->str, str, s_len);

	return 0;
}

/**
 * Destroy a cgc_PrinterString object
 *
 * @param p 	cgc_PrinterString object (this)
 */
void cgc_destructor_printer_string(cgc_PrinterString *p) {
	if (NULL != p->str)
		cgc_free(p->str);
	cgc_free(p);
}

/**
 * Construct an empty cgc_PrinterString object
 *
 * @return VA of object
 */
cgc_PrinterString *cgc_constructor_printer_string(void) {
	cgc_PrinterString *p = cgc_calloc(sizeof(cgc_PrinterString));
	MALLOC_OK(p);
	p->set_data = &cgc_set_data_printer_string;
	p->destroy = &cgc_destructor_printer_string;
	return p;
}

/**
 * Fucntion to execute a given command with the given args.
 *
 * @param fn 		Function to be executed
 * @param arg1 		First arg to fn
 * @param arg2 		Second arg to fn
 * @param arg3 		Third arg to fn
 * @return 0
 */
int cgc_os_exec(cgc_run_fn fn, int arg1, int arg2, int arg3) {
	fn(arg1, arg2, arg3);
	return 0;
}

/**
 * Set values in a cgc_CommandRunner object
 *
 * @param c 				cgc_CommandRunner object (this)
 * @param serialVersionUID 	UID of this serialized type
 * @param argc 				Number of args in argv + 1 for program
 * @param argv 				Buffer with arguments
 * @return 0 on SUCCESS, else -1 on error
 */
int cgc_set_data_command_runner(cgc_CommandRunner *c, cgc_svu_t serialVersionUID, unsigned short argc, char *argv) {
	c->serialVersionUID = serialVersionUID;
	cgc_memcpy(c->typeName, typeName[COMMAND_RUNNER], cgc_strlen(typeName[COMMAND_RUNNER]));

	int args[3] = {0};

	// hacky argv tokenizer
	char *c_p_argv;

	c->argc = argc;
	if ((argc < 2) || (4 < argc)) return -1; // need fn name and 1 arg (can have up to 3 args)

	c_p_argv = argv;
	c->fn = *(void **)c_p_argv; // first 4 bytes of argv is fn addr (fn ptr)

	unsigned int idx = 0;
	while (idx < (argc - 1)) { // argv -> int space int [space int]... NULL
		c_p_argv = c_p_argv + sizeof(int) + sizeof(char);
		c->argv[idx++] = *(int *)c_p_argv;
	}

	// run command with given args
	cgc_os_exec(c->fn, c->argv[0], c->argv[1], c->argv[2]);

	return 0;
}

/**
 * Destroy a cgc_CommandRunner object
 *
 * @param c 	cgc_CommandRunner object (this)
 */
void cgc_destructor_command_runner(cgc_CommandRunner *c) {
	cgc_free(c);
}

/**
 * Construct an empty cgc_CommandRunner object
 *
 * @return VA of object
 */
cgc_CommandRunner *cgc_constructor_command_runner(void) {
	cgc_CommandRunner *c = cgc_calloc(sizeof(cgc_CommandRunner));
	MALLOC_OK(c);
	c->set_data = &cgc_set_data_command_runner;
	c->destroy = &cgc_destructor_command_runner;
	return c;
}

cgc_Liquids *cgc_deserialize_liquids(char **in) {
	cgc_svu_t *p_svu;
	cgc_l_sti_t *p_l_sti;
	int ret;

	p_svu = (cgc_svu_t *)*in;
	*in += sizeof(cgc_svu_t);
	*in += cgc_strlen(typeName[LIQUID]);
	p_l_sti = (cgc_l_sti_t *)*in;
	*in += sizeof(cgc_l_sti_t);

	cgc_Liquids *p_liquids = cgc_constructor_liquids();
	ret = p_liquids->set_data(p_liquids, *p_svu, *p_l_sti);
	if (0 > ret) {
		p_liquids->destroy(p_liquids);
		p_liquids = NULL;
	}
	return p_liquids;
}

cgc_Cereals *cgc_deserialize_cereals(char **in) {
	cgc_svu_t *p_svu;
	cgc_c_sti_t *p_c_sti;
	int ret;

	p_svu = (cgc_svu_t *)*in;
	*in += sizeof(cgc_svu_t);
	*in += cgc_strlen(typeName[CEREAL]);
	p_c_sti = (cgc_c_sti_t *)*in;
	*in += sizeof(cgc_c_sti_t);

	cgc_Cereals *p_cereals = cgc_constructor_cereals();
	ret = p_cereals->set_data(p_cereals, *p_svu, *p_c_sti);
	if (0 > ret) {
		p_cereals->destroy(p_cereals);
		p_cereals = NULL;
	}
	return p_cereals;
}

cgc_Toppings *cgc_deserialize_toppings(char **in) {
	cgc_svu_t *p_svu;
	cgc_t_sti_t *p_t_sti;
	int ret;

	p_svu = (cgc_svu_t *)*in;
	*in += sizeof(cgc_svu_t);
	*in += cgc_strlen(typeName[TOPPINGS]);
	p_t_sti = (cgc_t_sti_t *)*in;
	*in += sizeof(cgc_t_sti_t);

	cgc_Toppings *p_toppings = cgc_constructor_toppings();
	ret = p_toppings->set_data(p_toppings, *p_svu, *p_t_sti);
	if (0 > ret) {
		p_toppings->destroy(p_toppings);
		p_toppings = NULL;
	}
	return p_toppings;
}

/**
 * Deserialize a cgc_GenericString object from a given buffer
 *
 * @param in 	Pointer to input buffer
 * @return VA of deserialized object or NULL on error
 */
cgc_GenericString *cgc_deserialize_generic_string(char **in) {
	cgc_svu_t *p_svu;
	char *str;
	int ret;

	p_svu = (cgc_svu_t *)*in;
	*in += sizeof(cgc_svu_t);
	*in += cgc_strlen(typeName[GENERIC_STRING]);
	str = *in;
	*in += cgc_strlen(str) + 1; // include count for NULL term

	cgc_GenericString *p_generic_string = cgc_constructor_generic_string();
	ret = p_generic_string->set_data(p_generic_string, *p_svu, str);
	if (0 > ret) {
		p_generic_string->destroy(p_generic_string);
		p_generic_string = NULL;
	}
	return p_generic_string;
}

/**
 * Deserialize a cgc_PrinterString object from a given buffer
 *
 * @param in 	Pointer to input buffer
 * @return VA of deserialized object or NULL on error
 */
cgc_PrinterString *cgc_deserialize_printer_string(char **in) {
	cgc_svu_t *p_svu;
	char *str;
	int ret;

	p_svu = (cgc_svu_t *)*in;
	*in += sizeof(cgc_svu_t);
	*in += cgc_strlen(typeName[PRINTER_STRING]);
	str = *in;
	*in += cgc_strlen(str) + 1; // include count for NULL term

	cgc_PrinterString *p_printer_string = cgc_constructor_printer_string();
	ret = p_printer_string->set_data(p_printer_string, *p_svu, str);
	if (0 > ret) {
		p_printer_string->destroy(p_printer_string);
		p_printer_string = NULL;
	}
	return p_printer_string;
}

/**
 * Deserialize a cgc_CommandRunner object from a given buffer
 *
 * @param in 	Pointer to input buffer
 * @return VA of deserialized object or NULL on error
 */
cgc_CommandRunner *cgc_deserialize_command_runner(char **in) {
	cgc_svu_t *p_svu;
	char *argv;
	unsigned short argc; // number of 4 byte values in argv
	int ret;

	p_svu = (cgc_svu_t *)*in;
	*in += sizeof(cgc_svu_t);
	*in += cgc_strlen(typeName[COMMAND_RUNNER]);
	argc = *(unsigned short *)*in;
	if ((argc < 2) || (4 < argc)) return NULL; // need fn name and 1 arg (can have up to 3 args)

	*in += sizeof(unsigned short);
	argv = *in;
	*in += (argc + 1) * (sizeof(unsigned int) + sizeof(char)); // int space int [space int]... NULL

	cgc_CommandRunner *p_command_runner = cgc_constructor_command_runner();
	ret = p_command_runner->set_data(p_command_runner, *p_svu, argc, argv);
	if (0 > ret) {
		p_command_runner->destroy(p_command_runner);
		p_command_runner = NULL;
	}
	return p_command_runner;
}

cgc_ssize_t cgc_check_inherited_types(cgc_svu_t serialVersionUID, char **cur) {

	// inherited types
	cgc_GenericString *p_generic_string;
	cgc_PrinterString *p_printer_string;
	cgc_CommandRunner *p_command_runner;

	switch(serialVersionUID) {
		case GENERIC_STRING:
			p_generic_string = cgc_deserialize_generic_string(cur);
			if (NULL == p_generic_string) return -1;
			p_generic_string->destroy(p_generic_string);
			break;
		case PRINTER_STRING:
			p_printer_string = cgc_deserialize_printer_string(cur);
			if (NULL == p_printer_string) return -1;
			p_printer_string->destroy(p_printer_string);
			break;
		case COMMAND_RUNNER:
			p_command_runner = cgc_deserialize_command_runner(cur);
			if (NULL == p_command_runner) return -1;
			p_command_runner->destroy(p_command_runner);
			break;
		default:
			return -1;
	}	
	return 0;
}

