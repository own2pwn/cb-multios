
/*

Author: Steve Wood <swood@cromulence.com>

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

#ifndef SERVICE_H
#define SERVICE_H

#define INITIALIZATION_DATA 0x4347C000
#define INIT_DATA_LEN 4096


typedef struct backlogItem {

	unsigned short ID;
	unsigned short story_points;
	unsigned short status;

	char *description;

	struct backlogItem *next;

} cgc_backlogItemType;


typedef struct sprintEntry {

	unsigned short ID;
	char *title;
	unsigned short duration;
	unsigned short total_points;
	unsigned short velocity_required;

	cgc_backlogItemType *sprintBacklogList;

	struct sprintEntry *next;

} cgc_sprintEntryType;


typedef struct productDef {

	unsigned short ID;
	char *title;
	cgc_backlogItemType *productBacklog;
	cgc_sprintEntryType *sprintList;

	struct productDef *next;

} cgc_productDefType;


int cgc_init_database( cgc_productDefType **database );
int cgc_destroy_database ( cgc_productDefType **database );
unsigned int cgc_calc_version();
int cgc_receive_commands( cgc_productDefType **database );
int cgc_receive_bytes (unsigned char *buffer, size_t count);

#endif


