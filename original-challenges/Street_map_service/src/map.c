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

#include "map.h"


/*
graph traversal service that emulates a street cgc_map.  
provides routing from point to point 
cgc_map can be created/edited by adding streets, intersections, 
and addresses
*/


cgc_pmap cgc_init_map(char mapName[32]){
	cgc_pmap thisMap;
	if (  allocate( sizeof(cgc_map), 0, (void**)&thisMap ) != 0  ){
	cgc_puts("\n**Allocate returned an error.\n");
	_terminate(1);
	}
	cgc_strcpy(thisMap->name, mapName);
	thisMap->name[31] = 0;
	thisMap->roadList = cgc_create_double_list();
	return thisMap;
}


cgc_proad cgc_add_road(cgc_pmap thisMap, char roadName[32], double roadLength){
	cgc_proad thisRoad = NULL;
	cgc_pdListNode thisNode = NULL;
	cgc_pdListNode lastRoadNode = cgc_get_last_element_d(thisMap->roadList);
	thisNode = cgc_insert_double_list_node(thisMap->roadList, lastRoadNode);
	thisRoad = (cgc_proad)thisNode->data;
	cgc_strcpy(thisRoad->name, roadName);
	thisRoad->name[cgc_strlen(roadName)] = 0;
	thisRoad->length = roadLength;
	thisRoad->addressList = cgc_create_double_list();
	thisRoad->intersectionList = cgc_create_double_list();
	thisRoad->self = thisNode;
	return thisRoad;

}

void cgc_print_roads(cgc_pmap thisMap){
	cgc_proad thisRoad;
	//check for empty cgc_map
	if (thisMap->roadList->listRoot != NULL){
		thisRoad = (cgc_proad)thisMap->roadList->listRoot->data;
	}else{
		thisRoad = NULL;
	}
	cgc_printf("The cgc_map @s contains @d roads.\n", thisMap->name, thisMap->roadList->count);
	int i = 1;
	while (thisRoad != NULL){
		cgc_printf("(@d) @s is @f units long\n", i, thisRoad->name, thisRoad->length);
		i++;
		if (thisRoad->self->next != NULL){
			thisRoad = (cgc_proad)thisRoad->self->next->data;
		}else{
			thisRoad = NULL;
		}
	}

	//print all cgc_road names and length for cgc_map
	return;
}
//returns null if not found

//insert cgc_address in list after last cgc_address in list with lower or equal number, if equal numbers new is inserted after last. 
cgc_paddress cgc_add_address ( int number, char resident[32], cgc_proad thisRoad, cgc_pintersection prevIntersection,
 						double distance_to_prev, cgc_pintersection nextIntersection, double distance_to_next ){
	cgc_address tempAddress;
	cgc_paddress thisAddress = &tempAddress;
	cgc_pdListNode thisNode = NULL;
	cgc_pdListNode prevNode = NULL;
	cgc_paddress nextAddress = NULL;
	cgc_paddress prevAddress = NULL;
	cgc_bzero(thisAddress, sizeof(cgc_address));
	//if empty addressList or first entry is higher number, insert at begining(prevAddress = NULL)
	if (thisRoad->addressList->listRoot != NULL){
		nextAddress = (cgc_paddress)thisRoad->addressList->listRoot->data;
	}
	//find where to insert new cgc_address in list
	while(nextAddress != NULL){
		//if next cgc_address number is higher, break, prevaddress will be set to NULL or prev.
		if (nextAddress->number > number){
			 nextAddress = NULL;
		//else set prevaddress to nextaddress and if there is a next loop, else break	 
		}else{
			prevAddress = nextAddress;
			if (nextAddress->self->next != NULL){
				nextAddress = (cgc_paddress)nextAddress->self->next->data;
			} else{nextAddress = NULL;}
		}
	}
	// at this point prevaddress is either NULL (insert at begining), or an cgc_address pointer
	if (prevAddress != NULL){
		prevNode = prevAddress->self;
	}

	thisNode = cgc_insert_double_list_node(thisRoad->addressList, prevNode);
	if (thisNode == NULL){
		cgc_puts("**cgc_insert_double_list_node failed");
		return NULL;
	}
	thisAddress->number = number;
	cgc_strcpy(thisAddress->resident, resident);
	thisAddress->resident[31] = 0;
	thisAddress->thisRoad = thisRoad;
	thisAddress->prevIntersection = prevIntersection;
	thisAddress->distance_to_prev = distance_to_prev;
	thisAddress->nextIntersection = nextIntersection;
	thisAddress->distance_to_next = distance_to_next;
	thisAddress->self = thisNode;
	cgc_memcpy( thisNode->data, thisAddress, sizeof(cgc_address) );
	return (cgc_paddress)thisNode->data;
}

void cgc_print_addresses(cgc_proad thisRoad){
	cgc_paddress thisAddress;
	//check for empty cgc_map
	if (thisRoad->addressList->listRoot != NULL){
		thisAddress = (cgc_paddress)thisRoad->addressList->listRoot->data;
	}else{
		thisAddress = NULL;
	}
	cgc_printf("The cgc_road @s contains @d addresses.\n", thisRoad->name, thisRoad->addressList->count);
	int i = 1;
	while (thisAddress != NULL){
		cgc_printf("(@d)\t@s\n\t@d @s\n", i, thisAddress->resident, thisAddress->number, thisRoad->name);
		i++;
		if (thisAddress->self->next != NULL){
			thisAddress = (cgc_paddress)thisAddress->self->next->data;
		}else{
			thisAddress = NULL;
		}
	}

	//print all cgc_road names and length for cgc_map
	return;
}

//on insert, have update prev and next for prevIntersection and nexIntersection, or you have a problem. data inside data is ugly
//if prevInterseciton is NULL, insert at begining, else insert at prevIntersection.
cgc_pintersection cgc_add_intersection ( cgc_proad thisRoad, cgc_proad other_road, cgc_pintersection prevIntersection, double distance_to_prev, double distance_to_next ){
	cgc_intersection tempIntersection;
	cgc_pintersection newIntersection = NULL;
	cgc_pdListNode newNode;
	cgc_pdListNode prevIntersectionNode = NULL;
	cgc_pintersection nextIntersection = NULL;
	if (prevIntersection != NULL){
		prevIntersectionNode = prevIntersection->self;
	}

	newNode = cgc_insert_double_list_node(thisRoad->intersectionList, prevIntersectionNode);
	if (newNode == NULL){
		cgc_puts("Failed to insert node");
		return NULL;
	} 
	newIntersection = (cgc_pintersection)newNode->data;

	if (prevIntersection != NULL){
		prevIntersection->nextIntersection = (cgc_pintersection)newNode->data;
	}
	if (newNode->next != NULL){
		nextIntersection = (cgc_pintersection)newNode->next->data;
		nextIntersection->prevIntersection = newIntersection;
	}

	newIntersection->prevIntersection = prevIntersection;
	newIntersection->nextIntersection = nextIntersection;
	newIntersection->self = newNode;
	newIntersection->other_road = other_road;
	newIntersection->distance_to_prev = distance_to_prev;
	newIntersection->distance_to_next = distance_to_next;
	return newIntersection;
}

void cgc_print_intersections(cgc_proad thisRoad){
	cgc_pintersection thisIntersection = NULL;
	if (thisRoad->intersectionList->listRoot != NULL){
		thisIntersection = (cgc_pintersection)thisRoad->intersectionList->listRoot->data;
	}
	cgc_printf("The cgc_road @s contains @d intersections.\n", thisRoad->name, thisRoad->intersectionList->count);
	int i = 1;
	while (thisIntersection != NULL){
		cgc_printf("(@d)\t@s intersects @s\n", i, thisRoad->name, thisIntersection->other_road->name );
		i++;
		if (thisIntersection->prevIntersection != NULL){
			cgc_printf("\t\t@f units after the cgc_intersection of @s\n",thisIntersection->distance_to_prev, 
					thisIntersection->prevIntersection->other_road->name);
		}
		if (thisIntersection->nextIntersection != NULL){
			cgc_printf("\t\t@f units before the cgc_intersection of @s\n", thisIntersection->distance_to_next, 
					thisIntersection->nextIntersection->other_road->name);
		}
		if (thisIntersection->self->next != NULL){
			thisIntersection = (cgc_pintersection)thisIntersection->self->next->data;
		}else {thisIntersection = NULL;}
	}
	return;
}

//returns cgc_pintersection which contains reference to searchpIntersection
//finding next for findall is (cgc_pintersection)pintersectionReturned->self->next->data
cgc_pintersection cgc_find_pintersection(cgc_proad thisRoad, cgc_pintersection searchpIntersection, cgc_pintersection startIntersection){
	cgc_pintersection tempIntersection = NULL;
	if (thisRoad->intersectionList->count == 0){
		cgc_puts("cgc_intersection list is empty");
		return (cgc_pintersection)NULL;
	}
	if (startIntersection == NULL){
		tempIntersection = (cgc_pintersection)thisRoad->intersectionList->listRoot->data;
	}else{
		tempIntersection = startIntersection;
	}
	while (tempIntersection != NULL){
		if ( (tempIntersection->prevIntersection == searchpIntersection) || 
			 (tempIntersection->nextIntersection == searchpIntersection) ){
			return tempIntersection;
		}
		if (tempIntersection->self->next != NULL){
			tempIntersection = (cgc_pintersection)tempIntersection->self->next->data;
		}else{
			tempIntersection = NULL;
		}
	}
	return NULL;
}

cgc_paddress cgc_find_pintersection_addresses(cgc_proad thisRoad, cgc_pintersection searchpIntersection, cgc_paddress startAddress){
	cgc_paddress tempAddress = NULL;
	if (startAddress == NULL){
		if(thisRoad->addressList->count == 0){
			cgc_puts("addresss list is empty");
			return (cgc_paddress)NULL;
		}
		tempAddress = (cgc_paddress) thisRoad->addressList->listRoot->data;
	}else{
		tempAddress = startAddress;
	}
	while (tempAddress != NULL){
		if ( (tempAddress->prevIntersection == searchpIntersection) ||
			(tempAddress->nextIntersection == searchpIntersection) ){
			return tempAddress;
		}
		if (tempAddress->self->next != NULL){
			tempAddress = (cgc_paddress)tempAddress->self->next->data;
		}else{
			tempAddress = NULL;
		}
	}

	return NULL;
}
//deletes cgc_intersection and set any reference to cgc_pintersection to prev or next or null
cgc_pintersection cgc_delete_intersection(cgc_proad thisRoad, cgc_pintersection deletedIntersection){
	cgc_pintersection tempIntersection =  NULL;
	cgc_pintersection thisIntersection = NULL;
	cgc_pdListNode thisNode = NULL;
	//check for pointers to the deleted cgc_intersection 
	cgc_pintersection refIntersection = cgc_find_pintersection(thisRoad, deletedIntersection, NULL);
	while (refIntersection != NULL){
		// if prevIntersection is the match, replace with deleted->prev or NULL
		if (refIntersection->prevIntersection == deletedIntersection){
			if (deletedIntersection->self->prev != NULL){
				refIntersection->prevIntersection = (cgc_pintersection)deletedIntersection->self->prev->data;
			}else{ 
				refIntersection->prevIntersection = NULL;
			}
		// if nextIntersection is the match, replace with deleted->next or NULL
		}else {
			if (deletedIntersection->self->next != NULL){
				refIntersection->nextIntersection = (cgc_pintersection)deletedIntersection->self->next->data;
			}else{ 
				refIntersection->nextIntersection = NULL;
			}
		}
		refIntersection = cgc_find_pintersection(thisRoad, deletedIntersection, refIntersection);
	}
	//check for pointers to deleted cgc_intersection in addresses
	cgc_paddress refAddress = cgc_find_pintersection_addresses(thisRoad, deletedIntersection, NULL);
	while (refAddress != NULL){
		//if prevIntersection is the match, replace with deleted->next or NULL
		if (refAddress->prevIntersection == deletedIntersection){
			if (deletedIntersection->self->prev != NULL){
				refAddress->prevIntersection = (cgc_pintersection)deletedIntersection->self->prev->data;
			}else{
				refAddress->prevIntersection = NULL;
			}
	// if nextIntersection is the match, replace with deleted->next or NULL			
		}else {
			if (deletedIntersection->self->next != NULL){
				refAddress->nextIntersection = (cgc_pintersection)deletedIntersection->self->next->data;
			}else{
				refAddress->nextIntersection = NULL;
			}
		}
		refAddress = cgc_find_pintersection_addresses(thisRoad,deletedIntersection, refAddress);
	}
	thisNode = cgc_delete_double_list_node(thisRoad->intersectionList, deletedIntersection->self);
	if (thisNode == NULL){
		return NULL;
	}
	return (cgc_pintersection)thisNode->data;
}


int cgc_is_in_turn_list(cgc_psList turnList, cgc_proad thisRoad){
	unsigned int i = 0;
	cgc_prouteList thisRouteList = NULL;
	if (turnList->listRoot == NULL){
		return 0;
	}
	thisRouteList = (cgc_prouteList)turnList->listRoot->data;
	while(thisRouteList->thisRoad != NULL){
		if (thisRouteList->thisRoad == thisRoad){
			return 1;
		}
		thisRouteList = (cgc_prouteList)(  (unsigned)turnList->listRoot->data + (unsigned int)( sizeof(cgc_routeList) * (i) )  );
		i++;
	}
	return 0;
}

cgc_psList cgc_init_turnList(){
	cgc_psList thisList = NULL;
	thisList = cgc_create_single_list();
	cgc_insert_single_list_node(thisList, NULL);
	return thisList;
}

cgc_prouteList cgc_push_to_turn_list(cgc_pmap thisMap, cgc_psList turnList, cgc_proad thisRoad, cgc_prouteList parent){
	cgc_prouteList thisRouteList = NULL;


#ifdef PATCHED
	//32
	if (  (turnList->listRoot->count) > ( 3800/(sizeof(cgc_routeList)) )  ){
		return NULL;
	}
#endif

#ifndef PATCHED
	//CWE-467 use of sizeof() on pointer type
	//1020
	if (  (turnList->listRoot->count) > ( 3800/(sizeof(cgc_prouteList)) )  ){
		return NULL;
	}
#endif	
	thisRouteList = (cgc_prouteList)(  (unsigned)turnList->listRoot->data + (unsigned int)( sizeof(cgc_routeList) * (unsigned int)(turnList->listRoot->count) )  );
	cgc_bzero(thisRouteList, sizeof(cgc_routeList));
	thisRouteList->thisRoad = thisRoad;
	thisRouteList->parent = parent;
	turnList->listRoot->count++;
	return thisRouteList;
}

cgc_psList cgc_get_route(cgc_pmap thisMap, cgc_psList turnList, cgc_proad targetRoad, cgc_proad startRoad){

	unsigned int curRouteListCount = 0;
//	turnList->count--;
//	turnList->count = 0;
	turnList->listRoot->count = 0;
	cgc_bzero(turnList->listRoot->data, 4080);
//
//	cgc_bzero(turnList->listRoot->count, sizeof(int));
//	
	cgc_prouteList curRouteList = cgc_push_to_turn_list(thisMap, turnList, startRoad, NULL);
	cgc_proad curRoad = NULL; 
	cgc_proad intersectionOtherRoad = NULL;
	cgc_pintersection thisIntersection = NULL;
	while (curRouteList->thisRoad != NULL){
		curRoad = curRouteList->thisRoad;
		if (curRoad->intersectionList->listRoot != NULL){
			thisIntersection = (cgc_pintersection)curRoad->intersectionList->listRoot->data;
		}else{thisIntersection = NULL;}
		while (thisIntersection != NULL){
			intersectionOtherRoad = thisIntersection->other_road;
			if ( !(cgc_is_in_turn_list(turnList, intersectionOtherRoad)) ){
				cgc_prouteList result = cgc_push_to_turn_list(thisMap, turnList, intersectionOtherRoad, curRouteList);
				if (result == NULL){
					return NULL; 
				}
				if (intersectionOtherRoad == targetRoad){
					return turnList;
				}
			}
			//move to next cgc_intersection if there is another
			if (thisIntersection->self->next != NULL){
				thisIntersection = (cgc_pintersection)thisIntersection->self->next->data;
			}else {thisIntersection = NULL;}
			//end while(curIntersection != NULL)
			}
		curRouteListCount++;
		curRouteList = (cgc_prouteList) (  (unsigned int)curRouteList  +  (unsigned int)(sizeof(cgc_routeList))  );
	}
	return NULL;
}


