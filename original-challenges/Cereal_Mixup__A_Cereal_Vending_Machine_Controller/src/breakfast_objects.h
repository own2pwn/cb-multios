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

#ifndef BREAKFAST_OBJECTS_H
#define BREAKFAST_OBJECTS_H 1



// TODO: enable "other" types after the basic stuff is up and working.

typedef enum SerialVersionUID {LIQUID, CEREAL, TOPPINGS, GENERIC_STRING,
			PRINTER_STRING, COMMAND_RUNNER, SVU_COUNT} cgc_svu_t;

typedef enum LiquidsSubTypeID {DAIRY_MILK, DAIRY_CREAM,	WATER, SOY_MILK,
			ALMOND_MILK, L_STI_COUNT} cgc_l_sti_t;

typedef enum CerealsSubTypeID {SUGAR_LOOPS, MAIZE_FLAKES, MARSHMALLOW_FIGURINES,
			CHOCOLATE_RICE_PELLETS, OHHS_OF_OATS, CRUNCHY_PUFFS, FRUITZ_N_NUTZ,
			C_STI_COUNT} cgc_c_sti_t;

typedef enum ToppingsSubTypeID {CHOCOLATE_DROPS, BLUEBERRIES, BERRY_MEDLEY,
			SUGAR_CUBE,	STRAWBERRIES, T_STI_COUNT} cgc_t_sti_t;

extern const char *typeName[];

// Object types for the application

typedef struct liquids cgc_Liquids;
struct liquids {
	cgc_svu_t serialVersionUID;
	unsigned char typeName[8];
	cgc_l_sti_t subTypeID;
	int (*set_data)(cgc_Liquids *, cgc_svu_t, cgc_l_sti_t);
	void (*destroy)(cgc_Liquids *);
};

typedef struct cereals cgc_Cereals;
struct cereals {
	cgc_svu_t serialVersionUID;
	unsigned char typeName[8];
	cgc_c_sti_t subTypeID;
	int (*set_data)(cgc_Cereals *, cgc_svu_t, cgc_c_sti_t);
	void (*destroy)(cgc_Cereals *);
};

typedef struct toppings cgc_Toppings;
struct toppings {
	cgc_svu_t serialVersionUID;
	unsigned char typeName[9];
	cgc_t_sti_t subTypeID;
	int (*set_data)(cgc_Toppings *, cgc_svu_t, cgc_t_sti_t);
	void (*destroy)(cgc_Toppings *);
};

// Object types that come with the SDK (whether you ask for them or not)
typedef struct genericString cgc_GenericString;
struct genericString {
	cgc_svu_t serialVersionUID;
	unsigned char typeName[15];
	char *str;
	int (*set_data)(cgc_GenericString *, cgc_svu_t, char *);
	void (*destroy)(cgc_GenericString *);
};

typedef struct printerString cgc_PrinterString;
struct printerString {
	cgc_svu_t serialVersionUID;
	unsigned char typeName[15];
	char *str;
	int (*set_data)(cgc_PrinterString *, cgc_svu_t, char *);
	void (*destroy)(cgc_PrinterString *);
};

typedef __attribute__((regparm(2))) void (*cgc_run_fn)(int, int, int);

typedef struct commandRunner cgc_CommandRunner;
struct commandRunner {
	cgc_svu_t serialVersionUID;
	unsigned char typeName[15];
	cgc_run_fn fn;
	unsigned short argc;
	int argv[3];
	int (*set_data)(cgc_CommandRunner *, cgc_svu_t, unsigned short, char *);
	void (*destroy)(cgc_CommandRunner *);
};

/**
 * Construct an empty cgc_Liquids object
 *
 * @return VA of object or NULL on error
 */
cgc_Liquids *cgc_constructor_liquids(void);
/**
 * Construct an empty cgc_Cereals object
 *
 * @return VA of object or NULL on error
 */
cgc_Cereals *cgc_constructor_cereals(void);
/**
 * Construct an empty cgc_Toppings object
 *
 * @return VA of object or NULL on error
 */
cgc_Toppings *cgc_constructor_toppings(void);

/**
 * Deserialize a cgc_Liquids object from a given buffer
 *
 * @param in 	Pointer to input buffer
 * @return VA of deserialized object or NULL on error
 */
cgc_Liquids *cgc_deserialize_liquids(char **in);
/**
 * Deserialize a cgc_Cereals object from a given buffer
 *
 * @param in 	Pointer to input buffer
 * @return VA of deserialized object or NULL on error
 */
cgc_Cereals *cgc_deserialize_cereals(char **in);
/**
 * Deserialize a cgc_Toppings object from a given buffer
 *
 * @param in 	Pointer to input buffer
 * @return VA of deserialized object or NULL on error
 */
cgc_Toppings *cgc_deserialize_toppings(char **in);

/**
 * Check input buffer for serialized forms of inherited types
 *
 * @param serialVersionUID 	UID of serialized type to deserialized
 * @param cur 				Pointer to input buffer
 * @return SUCCESS if inherited type was found/deserialized, else -1
 */
cgc_ssize_t cgc_check_inherited_types(cgc_svu_t serialVersionUID, char **cur);

#endif

