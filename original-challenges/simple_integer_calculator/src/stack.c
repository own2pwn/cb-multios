/*

Author: Dustin Fraze <df@cromulence.co>

Copyright (c) 2015 Cromulence LLC

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

#include "stack.h"
#include <libcgc.h>
#include <stdlib.h>

void *cgc_pop(cgc_stack *myStack) {
	void *ret;
	ret = (void *)myStack->items[myStack->top];
	myStack->top--;
	return ret;
}

void cgc_push(cgc_stack *myStack, void *value) {
	#ifdef PATCHED
	if(myStack->top < 31) {
	#endif
		myStack->top++;
		myStack->items[myStack->top] = value;
	#ifdef PATCHED
	}
	#endif
}

int cgc_isEmpty(cgc_stack *myStack) {
	return myStack->top == 0;
}


cgc_stack *cgc_initStack() {
	cgc_stack *ret;

	ret = cgc_calloc(sizeof(cgc_stack), 1);
	if(ret != NULL) {

	} else {
		cgc_puts("Critical memory error.  Corwardly exiting.");
		_terminate(-1);
	}

	return ret;

}