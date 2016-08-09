/*

Author: James Connor (jymbo@cromulence.co)

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

#include <libcgc.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdint.h>
#include <mymath.h>
#include "service.h"
#include "llist.h"

cgc_psListNode cgc_get_last_element_s(cgc_psList thisList){
	cgc_psListNode thisNode = NULL; 
	cgc_psListNode tempNode = NULL;
	thisNode = thisList->listRoot;
	while (thisNode != NULL){
		tempNode = thisNode;
		thisNode = thisNode->next;
	}
	return tempNode;
}
//returns NULL if no elements
cgc_pdListNode cgc_get_last_element_d(cgc_pdList thisList){
	cgc_pdListNode thisNode = NULL; 
	cgc_pdListNode tempNode = NULL;
	thisNode = thisList->listRoot;
	while (thisNode != NULL){
		tempNode = thisNode;
		thisNode = thisNode->next;
	}
	return tempNode;
}

cgc_psList cgc_create_single_list(){
	cgc_psList thisList = NULL;
	if (  allocate( sizeof(cgc_sList), 0, (void**)&thisList ) !=0  ){
		cgc_puts("\n**Allocate returned an error.\n");
		_terminate(1);	
	}
	cgc_bzero(thisList, sizeof(cgc_sList));
	thisList->listRoot = NULL;
	thisList->count = 0;
	return thisList;
}


//create double linked list 
cgc_pdList cgc_create_double_list(){
	cgc_pdList thisList = NULL;
	if (  allocate( sizeof(cgc_dList), 0, (void**)&thisList ) !=0  ){
			cgc_puts("\n**Allocate returned an error.\n");
			_terminate(1);
	}
	cgc_bzero(thisList, sizeof(cgc_dList));
	thisList->listRoot = NULL;
	thisList->count = 0;
	return thisList;
}




//if prev is null, insert as first node. if prev is last append
cgc_pdListNode cgc_insert_double_list_node(cgc_pdList thisList, cgc_pdListNode prevNode){
	cgc_pdListNode newNode = NULL;


	//allocate a node
	if (  allocate( sizeof(cgc_dListNode), 0, (void**)&newNode ) != 0  ){
		cgc_puts("\n**Allocate returned an error.\n");
		_terminate(1);
		}
	cgc_bzero(newNode, sizeof(cgc_dListNode));
	//cases are 
	//1) newlist insert at listroot, new->next is null, new->prev is null, listroot is new
	//2) existing insert at listroot, new->next is listroot, new->prev is null, listroot is new, new->next->prev is new
	//3) existing insert after prevNode where prevNode is not last node, new->next is prevNode->next, new-prev is prevNode, new->prev->next is new, new->next->prev is new
	//4) existing insert after prevNode where prevNode is last node, new->prev is prevNode, new->next is NULL, new->prev->next is new 
	//case 1 & 2
	if (prevNode == NULL){
		newNode->next = thisList->listRoot;
		thisList->listRoot = newNode;
	//case 3 & 4
	} else {  
		newNode->next = prevNode->next;
	}
	//prevNode is either NULL or prevNode
	newNode->prev = prevNode;
//if there is a next node, set next->prev to new
	if (newNode->next != NULL){
		newNode->next->prev = newNode;
	}
//if there is a prev node, set prev->next to new
	if (newNode->prev != NULL){
		newNode->prev->next = newNode;
	}
	thisList->count++;
	return newNode;
}

//returns pointer to prevNode, or null if first node is deleted
cgc_pdListNode cgc_delete_double_list_node(cgc_pdList thisList, cgc_pdListNode deletedNode){
	//empty list
	cgc_pdListNode retNode = NULL;	
	if ( (deletedNode == NULL) || (thisList->count == 0) ){
		cgc_puts("**Cannot delete and empty list.");
		return NULL;
	}

	//cases are:
	//1) delete first element in 1 element list, resulting in empty list
	//		listRoot = NULL, deallocate node
	//2) delete first element in >1 element list.
	//		listRoot = deletedNode->next, deletedNode->next->prev = NULL, deallocate node 
	//3) delete nth element in list where n+1 exists
	// 		 deletedNode->prev->next = deletedNode->next, deletedNode->next->prev = deletedNode->prev, deallocate node
	//4) delete nth element in list where n is last element
	//		deletedNode->prev->next = NULL, deallocate node
	//case 1 & 2
	//deletedNode->prev == NULL
	if (deletedNode == thisList->listRoot){
		thisList->listRoot = deletedNode->next;
	//case 3 & 4
	}else {
		//not first, there must be a prev, so point it to next, which may be NULL
		deletedNode->prev->next = deletedNode->next;
	}
	// if there is a next, point it to prev, which may be NULL
	if (deletedNode->next != NULL){
		deletedNode->next->prev = deletedNode->prev;
	}

	retNode = deletedNode->prev;
	deallocate(deletedNode, sizeof(cgc_dListNode));
	thisList->count--;
	return retNode;
}


// if prevNode is NULL insert at begining, else insert after prevNode
// returns pointer to inserted node
cgc_psListNode cgc_insert_single_list_node(cgc_psList thisList, cgc_psListNode prevNode){
	cgc_psListNode newNode = NULL;
	cgc_psListNode tempNode = NULL;
	if (  allocate( sizeof(cgc_sListNode), 0, (void**)&newNode ) != 0  ){
		cgc_puts("\n**Allocate returned an error.\n");
		_terminate(1);
		}
	cgc_bzero(newNode, sizeof(cgc_sListNode));
	//cases are 
	//1) newlist insert at listroot, new->next is null, listroot is new
	//2) existing insert at listroot, new->next is listroot, listroot is new
	//3) existing insert after prevNode where prevNode is not last node, new->next is prevNode->next, prevNode->next is new
	//4) existing insert after prevNode where prevNode is last node, new->next is NULL, prev->next is new 
	//case 1 & 2	

	if (prevNode == NULL){
		newNode->next = thisList->listRoot;
		thisList->listRoot = newNode;

	//case 3 & 4
	}else{
		newNode->next = prevNode->next;
		prevNode->next = newNode;
	}
	thisList->count++;
	return newNode;
}	
