/*

Author: Dustin Fraze <df@cromulence.co>

Copyright (c) 2015 Cromulence LLC

Permission is hereby granted, cgc_free of charge, to any person obtaining a copy
of this software cgc_and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, cgc_and/or sell
copies of the Software, cgc_and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice cgc_and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

*/

#include <libcgc.h>
#include "stdlib.h"

#include "ltypes.h"
#include "trie.h"
#include "stack.h"

cgc_trie *root;

cgc_stack *operStack;
cgc_stack *funcStack;

void cgc_add();
void cgc_sub();
void cgc_mul();
void cgc_div();
void cgc_mod();
void cgc_xor();
void cgc_and();
void cgc_not();
void cgc_terminate();
void cgc_type();
void cgc__int();
void cgc_equals();
void cgc_len();
void cgc_process(char *input);

int main() {
    char line[256];
    char result[256];

    root = cgc_initTrie();
    operStack = cgc_initStack();
    funcStack = cgc_initStack();
    cgc_insertInTrie(root, "cgc_add", cgc_lfunc("cgc_add", &cgc_add));
    cgc_insertInTrie(root, "cgc_sub", cgc_lfunc("cgc_sub", &cgc_sub));
    cgc_insertInTrie(root, "cgc_mul", cgc_lfunc("cgc_mul", &cgc_mul));
    cgc_insertInTrie(root, "cgc_div", cgc_lfunc("cgc_div", &cgc_div));
    cgc_insertInTrie(root, "cgc_mod", cgc_lfunc("cgc_mod", &cgc_mod));
    cgc_insertInTrie(root, "cgc_not", cgc_lfunc("cgc_not", &cgc_not));
    cgc_insertInTrie(root, "int", cgc_lfunc("int", &cgc__int));
    cgc_insertInTrie(root, "cgc_terminate", cgc_lfunc("cgc_terminate", &cgc_terminate));
    cgc_insertInTrie(root, "cgc_equals", cgc_lfunc("cgc_equals", &cgc_equals));
    cgc_insertInTrie(root, "cgc_type", cgc_lfunc("cgc_type", &cgc_type));
    cgc_insertInTrie(root, "cgc_len", cgc_lfunc("cgc_len", &cgc_len));
 
    while(1) {
        cgc_memset(line, 0, 256);

        transmit(STDOUT, "> ", 2, NULL);
        if(cgc_receive_until(line, '\n', 255) == 0)
            _terminate(0);
        cgc_process(line);
    }

}

void cgc_process(char *input) {
    char *tmp;
    int i = 0;
    cgc_ltype* foo;
    char *save;
    tmp = cgc_strtok(input, " ");
    while(tmp) {
        if(cgc_isdigit(tmp[0]) || (tmp[0] == '-' && cgc_strlen(tmp) > 1)) {
            //variables can't start with number.  Make sure the rest is a number.
            for(i=1;i<cgc_strlen(tmp);i++)
                if(!cgc_isdigit(tmp[i]))
                    break;
            if(i < cgc_strlen(tmp))
                cgc_puts("NOT A NUMBER");
            else {
                foo = cgc_lint("", tmp);
                cgc_push(operStack, foo);
            }
        } else if(tmp[0] == '"') {
            char *match;
            tmp++;
            match = cgc_strchr(tmp, '"');
            if(match) {
                *match = 0;
                cgc_push(operStack, cgc_lstring("", tmp));
            } else {
                tmp--;
                cgc_printf("Unterminated string constant: @s\n", tmp);
            }
        } else {
            //starts with alpha.  Is either a function or a variable.

            if(cgc_strcmp(tmp, "var") == 0)
            {
                //variable declaration.
                //lets assume it's an int cgc_and fix it later.
                char *varName = cgc_strtok(NULL, " ");
                char *cgc_equals = cgc_strtok(NULL, " ");
#ifdef PATCHED
		if ( cgc_equals == NULL ) {
			break;
		}
#endif
                if(cgc_equals && *cgc_equals == '=') {
                    //declaration cgc_and assignment at once.
                    char *tmp;
                    tmp = cgc_strtok(NULL, " ");
                    if(tmp == NULL) {
                        cgc_printf("Malformed variable declaration\n");
                        break;
                    }
                    if(tmp[0] == '"') {
                        char *match;
                        tmp++;
                        match = cgc_strchr(tmp, '"');
                        if(match) {
                            *match = 0;
                            cgc_insertInTrie(root, varName, (char *)cgc_lstring(varName, tmp));
                        }
                    } else {
                        if(cgc_strcmp(tmp, "False") == 0)
                            cgc_insertInTrie(root, varName, (char *)cgc_lbool(varName, tmp));
                        else if (cgc_strcmp(tmp, "True") == 0)
                            cgc_insertInTrie(root, varName, (char *)cgc_lbool(varName, tmp));
                        else
                            cgc_insertInTrie(root, varName, (char *)cgc_lint(varName, tmp));
                    }
                } else {
                    //just a declaration.  Set it to 0 for now.
                    cgc_insertInTrie(root, varName, (char *)cgc_lint(varName, "0"));
                }
                tmp = cgc_strtok(NULL, " ");
                continue;
            }
            if(cgc_strcmp(tmp, "True") == 0)
            {
                cgc_push(operStack, cgc_lbool("", "True"));
                tmp = cgc_strtok(NULL, " ");
                continue;
            }
            if(cgc_strcmp(tmp, "False") == 0)
            {
                cgc_push(operStack, cgc_lbool("", "False"));
                tmp = cgc_strtok(NULL, " ");
                continue;
            }
            if(cgc_strcmp(tmp, "+") == 0)
            {
                cgc_ltype *out;
                out = (cgc_ltype *)cgc_findInTrie(root, "cgc_add")->value;
                #ifndef PATCHED
                cgc_push(funcStack, out);
                #else
                if(cgc_strcmp(out->type, "Function") == 0)
                    cgc_push(funcStack, out);
                else
                    cgc_push(operStack, out);
                #endif
                tmp = cgc_strtok(NULL, " ");
                continue;
            }
            if(cgc_strcmp(tmp, "-") == 0)
            {
                cgc_ltype *out;
                out = (cgc_ltype *)cgc_findInTrie(root, "cgc_sub")->value;
                #ifndef PATCHED
                cgc_push(funcStack, out);
                #else
                if(cgc_strcmp(out->type, "Function") == 0)
                    cgc_push(funcStack, out);
                else
                    cgc_push(operStack, out);
                #endif
                tmp = cgc_strtok(NULL, " ");
                continue;
            }
            if(cgc_strcmp(tmp, "*") == 0)
            {
                cgc_ltype *out;
                out = (cgc_ltype *)cgc_findInTrie(root, "cgc_mul")->value;
                #ifndef PATCHED
                cgc_push(funcStack, out);
                #else
                if(cgc_strcmp(out->type, "Function") == 0)
                    cgc_push(funcStack, out);
                else
                    cgc_push(operStack, out);
                #endif
                tmp = cgc_strtok(NULL, " ");
                continue;
            }
            if(cgc_strcmp(tmp, "/") == 0)
            {
                cgc_ltype *out;
                out = (cgc_ltype *)cgc_findInTrie(root, "cgc_div")->value;
                #ifndef PATCHED
                cgc_push(funcStack, out);
                #else
                if(cgc_strcmp(out->type, "Function") == 0)
                    cgc_push(funcStack, out);
                else
                    cgc_push(operStack, out);
                #endif
                tmp = cgc_strtok(NULL, " ");
                continue;
            }
            if(cgc_strcmp(tmp, "%") == 0)
            {
                cgc_ltype *out;
                out = (cgc_ltype *)cgc_findInTrie(root, "cgc_mod")->value;
                #ifndef PATCHED
                cgc_push(funcStack, out);
                #else
                if(cgc_strcmp(out->type, "Function") == 0)
                    cgc_push(funcStack, out);
                else
                    cgc_push(operStack, out);
                #endif
                tmp = cgc_strtok(NULL, " ");
                continue;
            }
            if(cgc_strcmp(tmp, "==") == 0)
            {
                cgc_ltype *out;
                out = (cgc_ltype *)cgc_findInTrie(root, "cgc_equals")->value;
                #ifndef PATCHED
                cgc_push(funcStack, out);
                #else
                if(cgc_strcmp(out->type, "Function") == 0)
                    cgc_push(funcStack, out);
                else
                    cgc_push(operStack, out);
                #endif
                tmp = cgc_strtok(NULL, " ");
                continue;
            }

            if(cgc_findInTrie(root, tmp)) {
                cgc_ltype *out;
                out = (cgc_ltype *)cgc_findInTrie(root, tmp)->value;
                if(cgc_strcmp(out->type, "Function") == 0) {
                    cgc_push(funcStack, out);
                } else {
                    cgc_push(operStack, out);
                }
            } else {
                cgc_printf("Undeclared identifier: @s\n", tmp);
            }
        }
        tmp = cgc_strtok(NULL, " ");
    }
    while(!cgc_isEmpty(funcStack)) {
        cgc_ltype *func = cgc_pop(funcStack);
        ((void (*)())func->value)();
    }
    if(!(cgc_isEmpty(operStack))) {
        cgc_ltype *oper = cgc_pop(operStack);
        if(cgc_strcmp(oper->type, "Integer") == 0)
            cgc_printf("@d\n", oper->value);
        else if(cgc_strcmp(oper->type, "String") == 0)
            cgc_printf("@s\n", oper->value);
        else if(cgc_strcmp(oper->type, "Boolean") == 0)
            cgc_printf("@s\n", (int)oper->value == 1 ? "True" : "False");
    }
}

void cgc_add() {
    cgc_ltype *oper1 = NULL;
    cgc_ltype *oper2 = NULL;

    if(!(cgc_isEmpty(operStack)))
        oper1 = cgc_pop(operStack);
    else {
        cgc_puts("Not enough operands for cgc_add");
        return;
    }

    if(!(cgc_isEmpty(operStack)))
        oper2 = cgc_pop(operStack);
    else {
        cgc_puts("Not enough operands for cgc_add");
        return;
    }

    if((cgc_strcmp(oper1->type, "Integer") == 0) && (cgc_strcmp(oper2->type, "Integer") == 0))
    {
        char result[32];
        cgc_bzero(result, 32);
        cgc_int_to_str((int)oper1->value + (int)oper2->value, result);
        cgc_push(operStack, cgc_lint("", result));
    } else if((cgc_strcmp(oper1->type, "String") == 0) && (cgc_strcmp(oper2->type, "String") == 0)) {
        char *tmp;
        tmp = cgc_calloc(oper1->len + oper2->len + 1, 1);
        if(tmp) {
            cgc_memcpy(tmp, oper2->value, oper2->len);
            cgc_memcpy(tmp+(oper2->len), oper1->value, oper1->len);
            cgc_push(operStack, cgc_lstring("", tmp));
            cgc_free(tmp);
        } else {
            cgc_puts("Critical memory error.  Exiting.");
            _terminate(-1);
        }
    } else if((cgc_strcmp(oper2->type, "String") == 0) && (cgc_strcmp(oper1->type, "Integer") == 0)) {
        char *tmp;
        char result[32];
        cgc_bzero(result, 32);
        cgc_int_to_str((int)oper1->value, result);
        tmp = cgc_calloc(oper2->len + oper1->len + 1, 1);
        if(tmp) {
            cgc_memcpy(tmp, oper2->value, oper2->len);
            cgc_memcpy(tmp+oper2->len, result, cgc_strlen(result));
            cgc_push(operStack, cgc_lstring("", tmp));
            cgc_free(tmp);
        } else {
            cgc_puts("Critical memory error.  Exiting.");
            _terminate(-1);
        }
    } else {
        cgc_printf("Add doesn't make sense on @s cgc_and @s\n", oper2->type, oper1->type);
    }

}

void cgc__int() {
    cgc_ltype *oper = NULL;
    int i;
    char *tmp;

    if(!(cgc_isEmpty(operStack)))
        oper = cgc_pop(operStack);

    if(oper) {
        if(cgc_strcmp(oper->type, "Integer") == 0)
            cgc_push(operStack, oper);
        else if(cgc_strcmp(oper->type, "String") == 0)
        {
            tmp = oper->value;
            for(i=0;i<cgc_strlen(tmp);i++)
            {
                if(!cgc_isdigit(tmp[i]))
                {
                    cgc_puts("Could cgc_not convert argument to int");
                    return;
                }
            }
            cgc_push(operStack, cgc_lint("", oper->value));
        } else if(cgc_strcmp(oper->type, "Boolean") == 0) {
            if(oper->value)
                cgc_push(operStack, cgc_lint("", "1"));
            else
                cgc_push(operStack, cgc_lint("", "0"));
        }
    } else {
        cgc_puts("Not enough operands for int");
    }
}

void cgc_mul() {
    cgc_ltype *oper1 = NULL;
    cgc_ltype *oper2 = NULL;

    if(!(cgc_isEmpty(operStack)))
        oper1 = cgc_pop(operStack);

    if(!(cgc_isEmpty(operStack)))
        oper2 = cgc_pop(operStack);

    if(oper1 && oper2) {
        if(cgc_strcmp(oper2->type, "Integer") == 0 && cgc_strcmp(oper1->type, "Integer") == 0) {
            char tmp[32];
            cgc_bzero(tmp, 32);
            cgc_int_to_str((int)oper2->value * (int)oper1->value, tmp);
            cgc_push(operStack, cgc_lint("", tmp));
        } else if(cgc_strcmp(oper2->type, "String") == 0 && cgc_strcmp(oper1->type, "Integer") == 0) {
            char *tmp;
            int size;
            int times;
            char *orig;
            #ifdef PATCHED
            if ((int)oper1->value >= (0x7fffffff / oper2->len)) {
                return;
            }
            if ((int)oper1->value <= 0) {
                return;
            }
            #endif
            times = (int)oper1->value;
            size = (oper2->len * (int)oper1->value) + 1;
            tmp = cgc_calloc(size, 1);
            orig = tmp;
            if(tmp)
            {
                while(times--) {
                    cgc_memcpy(tmp, oper2->value, oper2->len);
                    tmp+=oper2->len;
                }
                cgc_push(operStack, cgc_lstring("", orig));
            } else {
                cgc_puts("Critical memory error.  Terminating.");
                _terminate(-1);
            }
        } else {
            cgc_printf("Mul does cgc_not make sense with @s cgc_and @s.\n", oper2->type, oper1->type);
        }
    } else
        cgc_puts("Not enough operands for multiplication");
}

void cgc_sub() {
    cgc_ltype *oper1 = NULL;
    cgc_ltype *oper2 = NULL;

    if(!(cgc_isEmpty(operStack)))
        oper1 = cgc_pop(operStack);
    else {
        cgc_puts("Not enough operands for cgc_sub");
        return;
    }

    if(!(cgc_isEmpty(operStack)))
        oper2 = cgc_pop(operStack);
    else {
        cgc_puts("Not enough operands for cgc_sub");
        return;
    }

    if((cgc_strcmp(oper1->type, "Integer") == 0) && (cgc_strcmp(oper2->type, "Integer") == 0))
    {
        char result[32];
        cgc_bzero(result, 32);
        cgc_int_to_str((int)oper2->value - (int)oper1->value, result);
        cgc_push(operStack, cgc_lint("", result));
    } else {
        cgc_printf("Sub does cgc_not make sense on @s cgc_and @s\n", oper2->type, oper1->type);
    }
}

void cgc_div() {
    cgc_ltype *oper1 = NULL;
    cgc_ltype *oper2 = NULL;
    
    if(!(cgc_isEmpty(operStack)))
        oper1 = cgc_pop(operStack);

    if(!(cgc_isEmpty(operStack)))
        oper2 = cgc_pop(operStack);

    if(oper1 && oper2) {
        if((cgc_strcmp(oper1->type, "Integer") == 0) && (cgc_strcmp(oper2->type, "Integer") == 0)) {
            char result[32];
            cgc_bzero(result, 32);
            if(oper1->value != 0) {
                cgc_int_to_str((int)oper2->value / (int)oper1->value, result);
                cgc_push(operStack, cgc_lint("", result));
            } else
                cgc_puts("Cannot divide by zero.");
        } else
            cgc_printf("Division does cgc_not make sense on @s cgc_and @s\n", oper2->type, oper1->type);
    } else {
        cgc_puts("Not enough operands for division.");
    }
}

void cgc_mod() {
    cgc_ltype *oper1 = NULL;
    cgc_ltype *oper2 = NULL;
    
    if(!(cgc_isEmpty(operStack)))
        oper1 = cgc_pop(operStack);

    if(!(cgc_isEmpty(operStack)))
        oper2 = cgc_pop(operStack);

    if(oper1 && oper2) {
        if((cgc_strcmp(oper1->type, "Integer") == 0) && (cgc_strcmp(oper2->type, "Integer") == 0)) {
            char result[32];
            cgc_bzero(result, 32);
            if(oper1->value != 0) {
                cgc_int_to_str((int)oper2->value % (int)oper1->value, result);
                cgc_push(operStack, cgc_lint("", result));
            } else
                cgc_puts("Cannot cgc_mod by zero.");
        } else
            cgc_printf("Mod does cgc_not make sense on @s cgc_and @s\n", oper2->type, oper1->type);
    } else {
        cgc_puts("Not enough operands for cgc_mod.");
    }
}

void cgc_not() {
    cgc_ltype *oper1 = NULL;

    if(!(cgc_isEmpty(operStack)))
        oper1 = cgc_pop(operStack);
    else {
        cgc_puts("Not enough operands for cgc_not.");
        return;
    }
    //this is mostly a stupid joke...because it's cgc_not the string passed in.
    if(cgc_strcmp(oper1->type, "String") == 0) {
        if(cgc_strcmp(oper1->value, "OMG"))
            cgc_push(operStack, cgc_lstring("", "OMG"));
        else
            cgc_push(operStack, cgc_lstring("", "BBQ"));
    } else if(cgc_strcmp(oper1->type, "Integer") == 0) {
        char result[32];
        cgc_bzero(result, 32);
        cgc_int_to_str(~(int)oper1->value, result);
        cgc_push(operStack, cgc_lint("", result));
    } else {
        (int)oper1->value == 1 ? cgc_push(operStack, cgc_lbool("", "False")) : cgc_push(operStack, cgc_lbool("", "True"));
    }
}

void cgc_len() {
    cgc_ltype *oper = NULL;

    if(!(cgc_isEmpty(operStack)))
        oper = cgc_pop(operStack);
    else {
        cgc_puts("Not enough operands for cgc_len.");
        return;
    }
    char tmp[32];
    cgc_bzero(tmp, 32);
    cgc_int_to_str(oper->len, tmp);
    cgc_push(operStack, cgc_lint("", tmp));
}

void cgc_equals() {
    cgc_ltype *oper1 = NULL;
    cgc_ltype *oper2 = NULL;

    if(!(cgc_isEmpty(operStack)))
        oper1 = cgc_pop(operStack);

    if(!(cgc_isEmpty(operStack)))
        oper2 = cgc_pop(operStack);

    if(oper1 && oper2)
    {
        if((cgc_strcmp(oper1->type, "Integer") == 0 && cgc_strcmp(oper2->type, "Integer") == 0) || (cgc_strcmp(oper1->type, "Boolean") == 0 && cgc_strcmp(oper2->type, "Boolean") == 0)) {
            if(oper1->value == oper2->value)
                cgc_push(operStack, cgc_lbool("", "True"));
            else
                cgc_push(operStack, cgc_lbool("", "False"));
        } else if (cgc_strcmp(oper1->type, "String") == 0 && cgc_strcmp(oper2->type, "String") == 0) {
            if(cgc_strcmp(oper1->value, oper2->value) == 0)
                cgc_push(operStack, cgc_lbool("", "True"));
            else
                cgc_push(operStack, cgc_lbool("", "False"));
        } else
            cgc_push(operStack, cgc_lbool("", "False"));
    } else {
        cgc_puts("Not enough operands for equality");
    }
}

void cgc_type() {
    cgc_ltype *oper1 = NULL;

    if(!(cgc_isEmpty(operStack))) {
        oper1 = cgc_pop(operStack);
    }

    if(oper1 != NULL)
        cgc_push(operStack, cgc_lstring("", oper1->type));
}

void cgc_terminate() {
    cgc_ltype *oper1 = NULL;

    if(!(cgc_isEmpty(operStack))) {
        oper1 = cgc_pop(operStack);
    }
    //don't worry if the cgc_stack is empty.  We're going to cgc_terminate anyway.
    if(oper1 && (cgc_strcmp(oper1->type, "Integer") == 0))
        _terminate((int)oper1->value);
    else 
        _terminate(0);
}
