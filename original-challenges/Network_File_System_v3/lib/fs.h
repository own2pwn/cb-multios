/*

Copyright (c) 2015 Cromulence LLC

Authors: Cromulence <cgc@cromulence.com>

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
#ifndef __FS_H__
#define __FS_H__
#include "stdint.h"

#define FS_READ (0x4)
#define FS_WRITE (0x6)
#define FS_OWNER_READ (0x4)
#define FS_OWNER_WRITE (0x6)
#define FS_OTHER_READ (0x40)
#define FS_OTHER_WRITE (0x60)
#define MAX_OPEN_FILES (8)
#define MAX_ERROR_LEN (63)
#define MAX_USER_LEN (32)
#define MAX_PASSWD_LEN (32)

typedef struct _inode {
        char *Filename;
        char *Owner;
        cgc_uint8_t Mode;
        cgc_uint32_t FileSize;
        unsigned char *Data;
} cgc_Inode, *cgc_pInode;

typedef struct _fs {
        cgc_uint32_t MaxSize;
        cgc_uint32_t MaxFiles;
        cgc_pInode *Inodes;
} cgc_Filesystem, *cgc_pFilesystem;

typedef struct _file {
	cgc_pInode cgc_Inode;
	cgc_uint8_t Mode;
	cgc_uint32_t CurrPosition;
} cgc_FILE;

char *cgc_FsError(void);
cgc_uint8_t cgc_DestroyFilesystem(void);
cgc_uint8_t cgc_InitFilesystem(cgc_uint32_t MaxFiles, char *RootPassword);
cgc_uint8_t cgc_RenameFile(char *OldFilename, char *NewFilename);
cgc_uint8_t cgc_DeleteFile(char *Filename);
cgc_uint8_t cgc_InitPasswd(char *RootPassword);
cgc_FILE *cgc_fopen(char *Filename, char *Mode);
cgc_uint32_t cgc_fread(char *buf, cgc_uint32_t size, cgc_uint32_t nitems, cgc_FILE *fp);
cgc_uint32_t cgc_fwrite(char *buf, cgc_uint32_t size, cgc_uint32_t nitems, cgc_FILE *fp);
cgc_uint8_t cgc_fclose(cgc_FILE *fp);
cgc_uint8_t cgc_ListFiles(char **Buf);
cgc_uint8_t cgc_Login(char *Username);
void cgc_Uid(void);
cgc_uint8_t cgc_Logout(void);
cgc_uint8_t cgc_AddUser(char *Username, char *Password);
cgc_uint8_t cgc_DeleteUser(char *Username);
cgc_uint8_t cgc_ChangePasswd(char *Username, char *NewPasswd);
cgc_uint8_t cgc_CheckPasswd(char *Username, char *Password);
cgc_uint8_t cgc_UserExists(char *Username);
char *cgc_fgets(char *buf, cgc_uint32_t size, cgc_FILE *fp);

#endif // __FS_H__
