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

#ifndef map_h
#define map_h



#include "llist.h"

typedef struct intersection_s {
	char FLAG[32];
	cgc_pdListNode self;
	struct road_s *other_road;
	struct intersection_s *prevIntersection;
	double distance_to_prev;
	struct intersection_s *nextIntersection;
	double distance_to_next;
} cgc_intersection, *cgc_pintersection;

typedef struct road_s {
	char FLAG[16];
	cgc_pdListNode self;
	char name[32];
	cgc_pdList addressList;
	cgc_pdList intersectionList; 
	double length;
}cgc_road, *cgc_proad;

typedef struct map_s {
	char name[32];
	cgc_pdList roadList;
}cgc_map, *cgc_pmap;


typedef struct address_s {
	char FLAG[8];
	cgc_pdListNode self;
	int number;
	char resident[32];
	cgc_proad thisRoad;
	cgc_pintersection prevIntersection;
	double distance_to_prev;
	cgc_pintersection nextIntersection;
	double distance_to_next;
}cgc_address, *cgc_paddress;

typedef struct routeList_s {
	cgc_proad thisRoad;//pointer to the newly discovered unique cgc_road
	struct routeList_s *parent;//parent is the cgc_road that was being explored when this is pushed on the list.
	char filler[120];//data[4088],  501 fits 8(503+8)=4088..9 to crash,247 fits 16(247+8)=4080..18 to crash ,119 fits 32(119+8)=4064..121 to crash 
} cgc_routeList, *cgc_prouteList;

cgc_pmap cgc_init_map(char mapName[32]);
cgc_proad cgc_add_road(cgc_pmap thisMap, char roadName[32], double roadLength);
void cgc_print_roads(cgc_pmap thisMap);
cgc_paddress cgc_add_address ( int number, char resident[32], cgc_proad thisRoad, cgc_pintersection prevIntersection,
 						double distance_to_prev, cgc_pintersection nextIntersection, double distance_to_next );
void cgc_print_addresses(cgc_proad thisRoad);
cgc_pintersection cgc_add_intersection ( cgc_proad thisRoad, cgc_proad other_road, cgc_pintersection prevIntersection, double distance_to_prev, double distance_to_next );
void cgc_print_intersections(cgc_proad thisRoad);
cgc_pintersection cgc_delete_intersection(cgc_proad thisRoad, cgc_pintersection deletedIntersection);
//new
cgc_psList cgc_init_turnList();
cgc_psList cgc_get_route(cgc_pmap thisMap, cgc_psList turnList, cgc_proad targetRoad, cgc_proad startRoad);


#endif 