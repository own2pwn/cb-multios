/*

Author: James Connor (jymbo@cromulence.co)

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

#ifndef service_h
#define service_h
#define GROUP 		0x47
#define USER 		0x55
#define DIRECTORY 	0x44
#define LINK 		0x4c
#define FILE 		0x46
#define READ 		0x01
#define WRITE 		0x02
#define EXECUTE 	0x04
#define MAXPATH		4000
#define MAXCOMMAND 	256
#define MAXUPLOAD 	1000000 //1M

//dllist of users
typedef struct User_s {
	struct User_s *prev;
	struct User_s *next;
	char *name;
} cgc_sUser, *cgc_pUser;

//dllist of users in a group
typedef struct GroupUserList_s {
	struct GroupUserList_s *prev;
	struct GroupUserList_s *next;
	cgc_pUser user;
} cgc_sGroupUserList, *cgc_pGroupUserList;

//dllist of groups
typedef struct Group_s {
	unsigned int userCount;
	struct Group_s *prev;
	struct Group_s *next;
	cgc_pGroupUserList userList; 
	char *name;
} cgc_sGroup, *cgc_pGroup; 

//dllist of file chunks
typedef struct FileChunk_s {
	char *chunk;
	unsigned int chunkSize;
	struct FileChunk_s *prev;
	struct FileChunk_s *next;
} cgc_sFileChunk, *cgc_pFileChunk;

//list Head and count for file chunks
typedef struct File_s {
	unsigned int chunkCount;
	cgc_pFileChunk head;
	cgc_pFileChunk tail;
} cgc_sFile, *cgc_pFile;

//dllist of pointers to allowed users and perms
typedef struct Perms_s {
	struct Perms_s *prev;
	struct Perms_s *next;
	cgc_pUser user;
	cgc_pGroup group;
} cgc_sPerms, *cgc_pPerms;

//ddlist of files, links, and directories (another dllist)
typedef struct Node_s {
	//interesting bug would allow changing type without changing pointer
	struct Node_s *next;
	struct Node_s *prev;
	//type is directory, link (pointer to existing node), or file
	//if type is directory, element is node
	char type;
	unsigned int linkCount;
	unsigned int date;
	cgc_pPerms perms;
	char *name;
	cgc_pFile file;
	//parent is pointer to ../
	struct Node_s *parent;
	struct Node_s *directoryHeadNode;
	struct Node_s *directoryTailNode;
} cgc_sNode, *cgc_pNode;

//directory is a linked list of nodes, nodes are pointers to files or directories
//interesting bug would result from mv a directory to itself or its subdirs

//program data / all peristent context

typedef struct DataStruct_s {
	cgc_pNode root;
	cgc_pNode workingDir;
	cgc_pUser currentUser;
	cgc_pGroup currentGroup;
	cgc_pUser user;
	cgc_pGroup group;
	unsigned int date;
} cgc_sDataStruct, *cgc_pDataStruct;

void cgc_die(char *);
void *cgc_mallocOrDie(int, char *);
cgc_pDataStruct cgc_init_data();
cgc_pPerms cgc_find_perm_by_name(char *name, cgc_pNode node, cgc_pDataStruct workingData);
cgc_pPerms cgc_add_perm(cgc_pUser, cgc_pGroup, cgc_pNode);
cgc_pPerms cgc_delete_perms(cgc_pNode, cgc_pPerms);
void cgc_validate_current_perms(cgc_pNode node, cgc_pDataStruct workingData);
cgc_pNode cgc__add_node( char, unsigned int, char *, cgc_pNode, cgc_pUser );
int cgc_get_file_size(cgc_pFile file);
void cgc_delete_file_bytes(cgc_pFile file, unsigned int newSize);
cgc_pFileChunk cgc_add_file_chunk( char *, cgc_pFile, unsigned int );
cgc_pNode cgc_add_file( cgc_pNode, unsigned int, unsigned int, char *, char *, cgc_pUser );
cgc_pFileChunk cgc_delete_chunk(cgc_pFile file, cgc_pFileChunk chunk);
void cgc_delete_file(cgc_pFile file);
cgc_pNode cgc_delete_node(cgc_pNode node, cgc_pDataStruct workingData);
cgc_pNode cgc_add_directory( unsigned int, char *, cgc_pNode, cgc_pUser );
cgc_pUser cgc__move_user(cgc_pUser before, cgc_pUser moved, cgc_pDataStruct workingData);
cgc_pUser cgc_move_user(cgc_pUser, cgc_pUser, cgc_pDataStruct );
cgc_pUser cgc__add_user( char *, cgc_pUser );
cgc_pUser cgc_add_user( char *, cgc_pDataStruct);
cgc_pGroupUserList cgc_is_user_in_group( cgc_pUser user, cgc_pGroup group );
int cgc_is_user_valid(cgc_pUser user, cgc_pDataStruct workingData);
int cgc_is_group_valid(cgc_pGroup group, cgc_pDataStruct workingData);
cgc_pGroupUserList cgc_remove_user_from_group(cgc_pUser user, cgc_pGroup group);
cgc_pUser cgc_remove_user(cgc_pUser, cgc_pDataStruct );
cgc_pGroup cgc_remove_group(cgc_pGroup group, cgc_pDataStruct workingData);
cgc_pUser cgc_find_user_by_name( char *, cgc_pDataStruct );
cgc_pUser cgc_find_user_by_number( int , cgc_pDataStruct );
cgc_pGroup cgc__add_group( char *, cgc_pGroup );
cgc_pGroup cgc_add_group( char *, cgc_pDataStruct);
cgc_pGroupUserList cgc_add_user_to_group( cgc_pUser, cgc_pGroup );
cgc_pGroup cgc_find_group_by_number( int number, cgc_pDataStruct workingData);
cgc_pGroup cgc_find_group_by_name( char *, cgc_pDataStruct );
char *cgc_recursive_path(cgc_pNode, cgc_pNode);
cgc_pNode cgc_find_node_by_name(char *name, cgc_pNode directory);
cgc_pNode cgc_find_directory_by_name(char *name, cgc_pNode directory);
cgc_pFile cgc_find_file_by_name(char *name, cgc_pNode directory);
cgc_pNode cgc_find_file_node_by_name(char *name, cgc_pNode directory);
void cgc_str_of_path(char *, cgc_pDataStruct, cgc_pNode);
void cgc_destroy_data( cgc_pDataStruct );



#endif