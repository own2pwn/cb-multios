/*

Author: James Connor <jymbo@cromulence.com>

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
#ifndef PAGESIZE
#define PAGESIZE 4096
#endif


#ifndef llist_h
#define llist_h


typedef struct dListNode_s {
	struct dListNode_s *prev;
	struct dListNode_s *next;
	void *this;
	char data[128];
}cgc_dListNode, *cgc_pdListNode;

typedef struct sListNode_s {
	char data[4080];
	struct sListNode_s *next;
	unsigned int count;
}cgc_sListNode, *cgc_psListNode;

typedef struct dList_s {
	cgc_pdListNode listRoot;
	int count;
}cgc_dList, *cgc_pdList;

typedef struct sList_s {
	cgc_psListNode listRoot;
	int count;
	int countRemaining;
}cgc_sList, *cgc_psList;


cgc_psListNode cgc_get_last_element_s(cgc_psList thisList);
cgc_pdListNode cgc_get_last_element_d(cgc_pdList thisList);
cgc_psList cgc_create_single_list();
cgc_pdList cgc_create_double_list();
cgc_pdListNode cgc_insert_double_list_node(cgc_pdList thisList, cgc_pdListNode prevNode);
cgc_pdListNode cgc_delete_double_list_node(cgc_pdList thisList, cgc_pdListNode deletedNode);
cgc_psListNode cgc_insert_single_list_node(cgc_psList thisList, cgc_psListNode prevNode);
#endif