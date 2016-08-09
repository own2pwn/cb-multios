
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

#ifndef FILE_S_H
#define FILE_S_H

#define MAX_FILENAME_LEN 40
#define MAX_OPEN_FILES 5

#define ROOT_ID 1

#define MASTER_BLOCK_NUMBER 0
#define FREE_BLOCK_LIST 1
#define INITIAL_ROOT_DIR_BLOCK 2


typedef int cgc_securityIdType;
typedef unsigned char cgc_otherPermsType;

enum fileTypes { 	DIRECTORY = 0x1,
					REGULAR = 0x2,
					RW_MEMORY = 0x3,
					RO_MEMORY = 0x4
};

enum errorCodes { 

					NO_ERROR = 0x0,
					ERROR_NOT_FOUND = -1,
					ERROR_BAD_TYPE = -2,
					ERROR_BAD_GEOMETRY = -3,
					ERROR_BAD_SIZE = -4,
					ERROR_NO_BLOCKS = -5, 
					ERROR_BAD_VALUE = -6,
					ERROR_EOF = -7,
					ERROR_READ_ERROR = -8,
					ERROR_INIT = -9,
					ERROR_MAX_EXCEEDED = -10,
					ERROR_BAD_HANDLE = -11,
					ERROR_FILE_EXISTS = -12, 
					ERROR_FILE_OPEN = -13,
					ERROR_NO_PERMISSION = -14

};

typedef struct { 

	unsigned int inUse;
	unsigned int totalSize;
	unsigned int blockSize;
	unsigned int totalBlocks;
	unsigned int rootDirBlock;
	unsigned int freeListBlock;
	unsigned int dirEntriesPerBlock;
	unsigned int blockEntriesPerCatalog;
	unsigned int checkValue;

}  cgc_MasterBlockType;

typedef struct {

	unsigned int totalSize;
	unsigned int blockSize;
	unsigned int totalBlocks;
	unsigned int freeBlocks;
	unsigned int maxEntriesPerDirectory;
	unsigned int maxBlocksPerFile;

} cgc_fileSystemInfoType;

typedef struct {

	cgc_MasterBlockType mblock0;
	cgc_MasterBlockType mblock1;
	cgc_MasterBlockType mblock3;

} cgc_mbStructType;


typedef struct  { 

	unsigned int RESERVED;
	cgc_securityIdType securityID;
	cgc_otherPermsType othersPermissions;
	unsigned int fileSize;
	enum fileTypes fileType;
	unsigned int firstCatalogBlock;
	char name[MAX_FILENAME_LEN];

} cgc_directoryEntryType;

typedef struct  { 

	unsigned short maxEntries;
	unsigned short numEntries;
	unsigned int nextBlock;
	cgc_directoryEntryType entry[];

} cgc_rootDirType;


typedef struct {

	enum fileTypes type;
	unsigned int size;

} cgc_fileInfoType;

typedef struct {

	cgc_directoryEntryType *currentFile;
	unsigned int dirEntryNum;
	enum fileTypes;
	char filespec[MAX_FILENAME_LEN];
	
} cgc_findFileHandleType;

typedef char cgc_fileHandleType;

typedef struct {

	unsigned int inUse;
	unsigned int dirEntryNum;
	unsigned int readBlockNum;
	unsigned int readPos;
	unsigned int writeBlockNum;
	unsigned int writePos;
	unsigned int fileSize;
	unsigned int firstCatalogBlock;
	cgc_securityIdType securityID;
	cgc_otherPermsType othersPermissions;
	enum fileTypes fileType;

} cgc_fileCursorType;



int cgc_initFileSystem( unsigned int sectorSize, unsigned int blockSize, unsigned int totalSize );
int cgc_createFile( char *name, enum fileTypes type, cgc_securityIdType);
int cgc_deleteFile( cgc_fileHandleType fh, cgc_securityIdType );
int cgc_truncateFile ( cgc_fileHandleType fh, cgc_securityIdType );
int cgc_findFiles( char *filespec, cgc_findFileHandleType **currentFile );
int cgc_findNextFile( cgc_findFileHandleType *currentFile);
int cgc_statusFile( char *name, cgc_fileInfoType *info );
cgc_fileHandleType cgc_openFile( char *name, cgc_securityIdType );
int cgc_writeFile( cgc_fileHandleType fh, char *buffer, unsigned int size, cgc_securityIdType );
int cgc_readFile(cgc_fileHandleType fh, char *buffer, unsigned int size, int relPosition, unsigned int *numRead, cgc_securityIdType);
int cgc_readFileUntil( cgc_fileHandleType fh, char *buffer, unsigned int size, char delim, unsigned int *numRead, cgc_securityIdType securityID );
int cgc_closeFile( cgc_fileHandleType fh );
int cgc_flushFile( cgc_fileHandleType fh );
int cgc_fileReadPosition( cgc_fileHandleType fh, unsigned int offset );
int cgc_fileReadPosRelative( cgc_fileHandleType fh, int offset );
int cgc_fileWritePosition( cgc_fileHandleType fh, unsigned int offset );
int cgc_fileWritePosRelative( cgc_fileHandleType fh, int offset );
int cgc_getFileSystemInfo(cgc_fileSystemInfoType *fsInfo);
int cgc_createDirectory( char *name, cgc_securityIdType );
int cgc_setPerms(cgc_fileHandleType fh, cgc_otherPermsType othersPermissions, cgc_securityIdType securityID);
int cgc_makeMemoryFile(char *name, unsigned int address, unsigned int length, char accessType, cgc_securityIdType  );


#endif


