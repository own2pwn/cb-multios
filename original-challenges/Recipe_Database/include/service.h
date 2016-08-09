/*

Author: Steve Wood <swood@cromulence.co>

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

#ifndef SERVICE_H
#define SERVICE_H

typedef struct Ingredient {

	char measurement[20];
	char item[100];
	struct Ingredient *next;

} cgc_Ingredient_Type;


typedef struct Recipe {

	char Title[200];
	int Tagged;
	cgc_Ingredient_Type *Ingredient_List;
	char **Instructions;
	struct Recipe *next;

} cgc_Recipe_Type;


typedef struct List {

	char *item;
	struct List *next;

} cgc_Shopping_List_Type;


void cgc_print_main_menu(void);
int cgc_new_recipe(cgc_Recipe_Type **);
int cgc_get_ingredients(cgc_Recipe_Type *);
void cgc_get_instructions(cgc_Recipe_Type *);
void cgc_print_all_tagged(cgc_Recipe_Type *);
void cgc_find_recipe(cgc_Recipe_Type *);
void cgc_print_recipe(cgc_Recipe_Type *);
void cgc_print_shopping_list(cgc_Recipe_Type *);
cgc_Shopping_List_Type *cgc_build_shopping_list(cgc_Recipe_Type *);
int cgc_match_str(char *search, char *);
int cgc_validate_measurement(char *);
void cgc_sort_shopping_list(cgc_Shopping_List_Type *);
int cgc_compare_strings(char *, char *);
int cgc_split_ingredient(char *, char *, int, char *, int);

//int parse_command(char *buffer, char separator, char ***args);
//void find_matches(bst_node_type *head, int arg_count, char **args);
#endif
