/*

Author: Joe Rogers <joe@cromulence.com>

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
#include "stdlib.h"
#include "stdint.h"

/*
The filesystem is shown graphically below.  The cgc_superblock
contiains an array of pointers to the cgc_inode pages.  Each cgc_inode 
page contains 16 inodes.  Each cgc_inode can describe a file
or cgc_directory (based on the type var).  Files are made up of 
multiple 512 byte blocks.  A single cgc_inode can describe up to
8 of those blocks for a total of 4096 bytes.  If more space
is needed for a given file, the indirect pointer can point
to another cgc_inode which provides another 8 blocks and, if 
necessary, another indirect pointer.  Directories use a single
cgc_block in the cgc_inode.  That cgc_block contains an array of
up to 128 inodes describing files or subdirectories.  So, a given
cgc_directory is limited to 128 files or subdirectories.  
------------------
| cgc_superblock     |
------------------
| magic          | 
| blocksize      |
| fs_size        |    -------------- 
| inode_page[64] | -> | inode_page |
------------------    --------------    ------------------
                      | cgc_inode[0]   | -> | cgc_inode          |
                      | ...        |    ------------------
                      | cgc_inode[15]  |    | fname          |
                      --------------    | type           |
                                        | fsize          |
                                        | num_blocks     |
                                        | indirect_block |    -------------
                                        | blocks[8]      | -> | cgc_block     |
                                        ------------------    -------------
                                                              | data[512] |
                                                              -------------
Directory as defined above.
--------------
| cgc_directory  |
--------------
| cgc_inode[0]   |
| ...        |
| cgc_inode[127] |
--------------


An array of 256 elements called the free_list contains pointers
to all of the allocated data pages.  Each page contains 
8 blocks (4096 page size / 8 blocks = 512 byte cgc_block size).
As a cgc_block is used in an cgc_inode, the free_mask is marked with a
'1' in the corresponding bit position for the cgc_block in the page.
So, for example, cgc_block[0] being used would result in a mask
value of 0x80.
-------------
| free_list |
-------------
| free_mask |    -------------
| page      | -> | page      |
-------------    -------------
                 | cgc_block[0]  |
                 | ...       |
                 | cgc_block[15] |    
                 -------------

*/
#define PAGE_SIZE 4096
#define MAX_CMD 256

// cgc_block
#define DATA_BLOCK_SIZE 512
typedef unsigned char cgc_block[DATA_BLOCK_SIZE];

// cgc_inode
#define INODE_FREE 0
#define INODE_FILE 1
#define INODE_DIRECTORY 2
#define INODE_DATA_BLOCKS 8
#define MAX_FILE_NAME_LEN 208
// sizeof(cgc_inode) = 256 for convenience of 4096 byte page sizes
typedef struct _inode {
	char fname[MAX_FILE_NAME_LEN];
	cgc_uint32_t type;
	cgc_uint32_t fsize;
	cgc_uint32_t num_blocks;
	struct _inode *indirect_inode;
	cgc_block *blocks[INODE_DATA_BLOCKS];
} cgc_inode;

// cgc_superblock
#define CRFS 0x43524653
#define MAX_INODE_PAGES 64
#define INODES_PER_PAGE 16
typedef struct _superblock {
	cgc_uint32_t magic;
	cgc_uint32_t blocksize;
	cgc_uint32_t fs_size;
	cgc_inode *inode_pages[MAX_INODE_PAGES];
} cgc_superblock;

// global pointer to our filesystem
cgc_superblock fs;

// cgc_free blocks
#define MAX_FREE_PAGES 256
#define MAX_FS_SIZE PAGE_SIZE*MAX_FREE_PAGES
#define BLOCKS_PER_PAGE 8
typedef struct _free_t {
	cgc_uint8_t in_use;
	cgc_block *page;
} cgc_free_t;
cgc_free_t free_list[MAX_FREE_PAGES];

// cgc_directory
#define MAX_DIR_INODES 128
typedef struct _directory {
	cgc_inode *inodes[MAX_DIR_INODES];
} cgc_directory;

// cgc_FILE struct
#define READ 0
#define WRITE 1
#define APPEND 2
typedef struct _FILE {
	cgc_inode *i;
	cgc_uint32_t pos;
	cgc_uint32_t mode;
	cgc_inode *curr_pos_inode;
	cgc_uint32_t index;
	cgc_uint32_t b_index;
} cgc_FILE;

int cgc_InitFS(cgc_uint32_t);
int cgc_DestroyFS();
cgc_inode *cgc_CreateFile(char *, char *);
cgc_inode *cgc_FindInode(char *);
int cgc_CheckFileExists(char *, cgc_inode **);
int cgc_SplitPath(char *, char *, char *);
int cgc_ReadFile(char *);
int cgc_FreeDataBlock(cgc_block *);
int cgc_mkdir(char *);
int cgc_rmdir(char *);
int cgc_unlink(char *);
cgc_FILE *cgc_fopen(char *, const char *);
size_t cgc_fread(void *, size_t, size_t, cgc_FILE *);
size_t cgc_fwrite(const void *, size_t, size_t, cgc_FILE *);
int cgc_fclose(cgc_FILE *);
int cgc_StatusFS();
int cgc_ls(char *);
