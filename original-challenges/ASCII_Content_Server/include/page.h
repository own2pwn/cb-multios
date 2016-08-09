/*

Author: Debbie Nuttall <debbie@cromulence.co>

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
#ifndef PAGE_H
#define PAGE_H

typedef struct sPageVar{
  char name[64];
  char *value;
  struct sPageVar *next;
} cgc_PageVar;

int cgc_AddPageVar(cgc_PageVar *varlist, char *cmd);
cgc_PageVar *cgc_GetPageVar(cgc_PageVar *varlist, char *name, char *end);
void cgc_DestroyVarList(cgc_PageVar *varlist);
int cgc_InteractWithPage(char *page, int page_size, char *override_data);
int cgc_ServePage(char *page, int page_size);
int cgc_ServePageWithOverride(char *page, int page_size, cgc_PageVar *override_list);

#endif // PAGE_H