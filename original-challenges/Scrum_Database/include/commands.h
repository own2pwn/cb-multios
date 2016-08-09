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

#ifndef COMMANDS_H
#define COMMANDS_H

#define TERMINATE 			0xDEAD
#define CREATE_PRODUCT  	0x101
#define CREATE_PBI  		0x102
#define CREATE_SPRINT  		0x103
#define MOVE_PBI_TO_SPRINT	0x201
#define MOVE_SBI_TO_PBI 	0x202
#define LIST_PRODUCTS 		0x301
#define LIST_PBI 			0x302
#define LIST_SPRINTS 		0x303
#define LIST_ALL_PRODUCTS	0x304
#define DELETE_PBI 			0x401
#define DELETE_SPRINT 		0x402
#define DELETE_PRODUCT 		0x403
#define UPDATE_SBI_STATUS	0x501
#define UPDATE_SBI_POINTS   0x502
#define UPDATE_SBI_DESCR 	0x503

#define COMMAND_MAX_LEN 1024

typedef struct {

	unsigned int prefix;
	unsigned short command_length;
	unsigned short command_type;
	unsigned char command[COMMAND_MAX_LEN]; 

} cgc_commandMessageType;

typedef struct {

	unsigned int prefix;
	int command_response;


} cgc_commandResponseType;

typedef struct {

	unsigned short ID;
	char  title;

} cgc_newProductMessageType;

typedef struct {

	unsigned short ID;

} cgc_messageIDType;


typedef struct {

	unsigned short productID;
	unsigned short sprintID;
	char title;

} cgc_newSprintMessageType;


typedef struct {

	unsigned short productID;
	unsigned short sprintID;

} cgc_deleteSprintMessageType;

typedef struct {

	unsigned short productID;
	unsigned short pbItemID;
	unsigned short user_story_points;
	char title;

} cgc_newPBIMessageType;

typedef struct {

	unsigned short productID;
	unsigned short pbItemID;
	unsigned short sprintID;


} cgc_movePBIMessageType;

typedef struct {

	unsigned short productID;
	unsigned short pbItemID;
	unsigned short sprintID;

} cgc_moveToPBIMessageType;

typedef struct {

	unsigned short productID;
	unsigned short pbItemID;

} cgc_deletePBIMessageType;

typedef struct {

	unsigned short productID;
	unsigned short pbItemID;
	unsigned short sprintID;
	unsigned short status;

} cgc_updateSBIMessageType;


typedef struct {

	unsigned short productID;
	unsigned short pbItemID;
	unsigned short sprintID;
	char desc;

} cgc_updateSBIDescMessageType;


int cgc_create_product( cgc_productDefType **database, void *command_data );
int cgc_delete_product( cgc_productDefType **database, cgc_messageIDType *message );
int cgc_list_product( cgc_productDefType *database, cgc_messageIDType *message );
int cgc_create_sprint( cgc_productDefType *database, cgc_newSprintMessageType *message);
int cgc_delete_sprint( cgc_productDefType *database, cgc_deleteSprintMessageType *message);
int cgc_create_pbi( cgc_productDefType *database, cgc_newPBIMessageType *message );
int cgc_move_pbi_to_sprint( cgc_productDefType *database, cgc_movePBIMessageType *message );
int cgc_delete_pbi( cgc_productDefType *database, cgc_deletePBIMessageType *message);
int cgc_update_sbi_status( cgc_productDefType *database, cgc_updateSBIMessageType *message );
int cgc_update_sbi_points( cgc_productDefType *database, cgc_updateSBIMessageType *message );
int cgc_move_sbi_to_pbl( cgc_productDefType *database, cgc_moveToPBIMessageType *message );
int cgc_update_sbi_description( cgc_productDefType *database, cgc_updateSBIDescMessageType *message );
int cgc_list_all_products( cgc_productDefType *database );

int cgc_send_response( int response_code );

#endif

