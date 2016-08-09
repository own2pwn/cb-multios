
/*

Author: Steve Wood <swood@cromulence.com>

Copyright (c) 2016 Cromulence LLC

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

#define INITIALIZATION_DATA 0x4347C000
#define INIT_DATA_LEN 4096

#define MAX_CONNECTIONS 10

typedef struct connectionList {

	char destCode[4];
	unsigned int cost;
	unsigned int time;

	struct connectionList *next;

} cgc_connectionListType;


typedef struct airportInfo {

	char code[4];
	unsigned short delayFactor;

	cgc_connectionListType *connections;

	struct airportInfo *next;

} cgc_airportInfoType;


int cgc_loadDB( cgc_airportInfoType **airports );
int cgc_showAirports( cgc_airportInfoType *airports, char *command );
int cgc_addAirport( cgc_airportInfoType **airports, char *command );
int cgc_deleteAirport( cgc_airportInfoType **airports, char *command );
int cgc_findRoutes( cgc_airportInfoType *airports, char *command );
int cgc_check4Code( cgc_airportInfoType *airports, char apCode[4]);
int cgc_check4ConnectionCode( cgc_connectionListType *connections, char apCode[4] );
int cgc_execute_cmd(cgc_airportInfoType **airports, char *buffer);
unsigned int cgc_check_db();

#endif


