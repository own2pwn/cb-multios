/*

Author: Jason Williams <jdw@cromulence.com>

Copyright (c) 2015 Cromulence LLC

Permission is hereby granted, free of charge, to any person obtaining a copy
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
extern "C"
{
#include <libcgc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <prng.h>
#include <fs.h>
}

#include "nor_flash.h"
#include "flash_fs.h"
#include "comms.h"

#define PAGE_SIZE	(4096)

void cgc_TestFS( cgc_CNORFlash *pNORFlash )
{
	cgc_CFlashFS oFS;

	oFS.cgc_Init( pNORFlash );

	cgc_int32_t test1FD = oFS.cgc_OpenFile( "test.txt", OPEN_MODE_WRITE );
	cgc_int32_t test2FD = oFS.cgc_OpenFile( "test2.txt", OPEN_MODE_WRITE );
	cgc_int32_t test3FD = oFS.cgc_OpenFile( "test3.txt", OPEN_MODE_WRITE );

	cgc_printf( "Test1 FD is: $d\n", test1FD );
	cgc_printf( "Test2 FD is: $d\n", test2FD );
	cgc_printf( "Test3 FD is: $d\n", test3FD );

	cgc_int32_t retValue = oFS.cgc_WriteFile( test1FD, (cgc_uint8_t*)"blah", 4 );

	cgc_printf( "Ret value is: $d\n", retValue );

	cgc_uint8_t testData[2048];
	cgc_uint8_t testData2[2048];
	
	retValue = oFS.cgc_WriteFile( test1FD, testData, 1792 );

	cgc_printf( "Ret value is: $d\n", retValue );

	retValue = oFS.cgc_CloseFile( test1FD );

	cgc_printf( "Close ret: $d\n", retValue );

	test1FD = oFS.cgc_OpenFile( "test.txt", OPEN_MODE_READ );

	cgc_printf( "Open read FD is: $d\n", test1FD );

	cgc_uint8_t readBlahBuffer[6];
	retValue = oFS.cgc_ReadFile( test1FD, readBlahBuffer, 4 );

	cgc_printf( "Read return is: $d\n", retValue );
	readBlahBuffer[5] = '\0';
	cgc_printf( "Read blah buffer is: $s\n", readBlahBuffer );

	retValue = oFS.cgc_DeleteFile( "test.txt" );

	cgc_printf( "Delete test.txt ret: $d\n", retValue );

	retValue = oFS.cgc_ReadFile( test1FD, readBlahBuffer, 4 );
		
	cgc_printf( "Read from FD=1 is: $d\n", retValue );
}
	
typedef struct cgc_TEST_COMMAND_RESPONSE
{
	cgc_int32_t retValue;
	cgc_uint32_t responseLen;
} cgc_tTestCommandResponse;

bool cgc_SendCommandResponse( cgc_tTestCommandResponse *pResponse, cgc_uint8_t *pData, cgc_uint32_t dataLen )
{
	pResponse->responseLen = dataLen;

	if ( cgc_SendData( STDOUT, (cgc_uint8_t*)pResponse, sizeof(cgc_tTestCommandResponse) ) != sizeof(cgc_tTestCommandResponse) )
	{
		// Comm error
		return (false);
	}

	if ( dataLen > 0 )
	{
		if ( cgc_SendData( STDOUT, pData, dataLen ) != dataLen )
		{
			// Commm error
			return (false);
		}
	}

	return (true);
}

#define MAX_TEMP_TEST_LEN	4096

bool cgc_OpenFileCommand( cgc_CFlashFS *pFS )
{
	typedef struct cgc_OPEN_FILE_COMMAND
	{
		cgc_uint8_t fileNameLen;
		cgc_uint8_t openMode;
	} cgc_tOpenFileCommand;

	cgc_uint8_t openFileName[257];
	cgc_tTestCommandResponse oResponse;
	cgc_tOpenFileCommand oCommandData;

	if ( cgc_RecvData( STDIN, (cgc_uint8_t*)&oCommandData, sizeof(oCommandData) ) != sizeof(oCommandData) )
	{
		// Bad command
		return true;
	}

	if ( cgc_RecvData( STDIN, openFileName, oCommandData.fileNameLen ) != oCommandData.fileNameLen )
	{
		// Bad command
		return true;
	}

	openFileName[oCommandData.fileNameLen] = '\0';

	cgc_int32_t retValue = pFS->cgc_OpenFile( (const char *)openFileName, oCommandData.openMode );

	// Set return value
	oResponse.retValue = retValue;

	// Send response!
	cgc_SendCommandResponse( &oResponse, (cgc_uint8_t*)NULL, 0 );

	return false;
}

bool cgc_CloseFileCommand( cgc_CFlashFS *pFS )
{
	typedef struct cgc_CLOSE_FILE_COMMAND
	{
		cgc_int32_t fd;
	} cgc_tCloseFileCommand;

	cgc_tTestCommandResponse oResponse;
	cgc_tCloseFileCommand oCommandData;

	if ( cgc_RecvData( STDIN, (cgc_uint8_t*)&oCommandData, sizeof(oCommandData) ) != sizeof(oCommandData) )
	{
		// Bad command
		return true;
	}

	cgc_int32_t retValue = pFS->cgc_CloseFile( oCommandData.fd );

	// Set return value
	oResponse.retValue = retValue;

	// Send response!
	cgc_SendCommandResponse( &oResponse, (cgc_uint8_t*)NULL, 0 );

	return false;
}

bool cgc_ReadFileCommand( cgc_CFlashFS *pFS, cgc_uint8_t *tempData )
{
	typedef struct cgc_READ_FILE_COMMAND
	{
		cgc_int32_t fd;
		cgc_uint32_t dataLen;
	} cgc_tReadFileCommand;

	cgc_tTestCommandResponse oResponse;
	cgc_tReadFileCommand oCommandData;

	if ( cgc_RecvData( STDIN, (cgc_uint8_t*)&oCommandData, sizeof(oCommandData) ) != sizeof(oCommandData) )
	{
		// Bad command
		return true;
	}

	if ( oCommandData.dataLen > MAX_TEMP_TEST_LEN )
		oCommandData.dataLen = MAX_TEMP_TEST_LEN;

	cgc_int32_t retValue = pFS->cgc_ReadFile( oCommandData.fd, tempData, oCommandData.dataLen );

	// Set return value
	oResponse.retValue = retValue;

	// Send response!
	if ( oResponse.retValue <= 0 )
		cgc_SendCommandResponse( &oResponse, (cgc_uint8_t*)NULL, 0 );
	else
		cgc_SendCommandResponse( &oResponse, tempData, oCommandData.dataLen );

	return false;
}

bool cgc_WriteFileCommand( cgc_CFlashFS *pFS, cgc_uint8_t *tempData )
{
	typedef struct cgc_WRITE_FILE_COMMAND
	{
		cgc_int32_t fd;
		cgc_uint32_t dataLen;
	} cgc_tWriteFileCommand;

	cgc_tTestCommandResponse oResponse;
	cgc_tWriteFileCommand oCommandData;

	if ( cgc_RecvData( STDIN, (cgc_uint8_t*)&oCommandData, sizeof(oCommandData) ) != sizeof(oCommandData) )
	{
		// Bad command
		return true;
	}

	if ( oCommandData.dataLen > MAX_TEMP_TEST_LEN )
		oCommandData.dataLen = MAX_TEMP_TEST_LEN;

	if ( cgc_RecvData( STDIN, tempData, oCommandData.dataLen ) != oCommandData.dataLen )
	{
		// Bad command
		return true;
	}

	cgc_int32_t retValue = pFS->cgc_WriteFile( oCommandData.fd, tempData, oCommandData.dataLen );

	// Set return value
	oResponse.retValue = retValue;

	// Send response!
	cgc_SendCommandResponse( &oResponse, (cgc_uint8_t*)NULL, 0 );

	return false;
}	

bool cgc_DeleteFileCommand( cgc_CFlashFS *pFS )
{
	typedef struct cgc_DELETE_FILE_COMMAND
	{
		cgc_uint8_t fileNameLen;
	} cgc_tDeleteFileCommand;

	cgc_uint8_t deleteFileName[257];
	cgc_tTestCommandResponse oResponse;
	cgc_tDeleteFileCommand oCommandData;

	if ( cgc_RecvData( STDIN, (cgc_uint8_t*)&oCommandData, sizeof(oCommandData) ) != sizeof(oCommandData) )
	{
		// Bad command
		return true;
	}

	if ( cgc_RecvData( STDIN, deleteFileName, oCommandData.fileNameLen ) != oCommandData.fileNameLen )
	{
		// Bad command
		return true;
	}

	deleteFileName[oCommandData.fileNameLen] = '\0';

	cgc_int32_t retValue = pFS->cgc_DeleteFile( (const char *)deleteFileName );

	// Set return value
	oResponse.retValue = retValue;

	// Send response!
	cgc_SendCommandResponse( &oResponse, (cgc_uint8_t*)NULL, 0 );

	return false;
}	

void cgc_SetupDevice( int secret_page_i, cgc_CFlashFS *pFS, cgc_CNORFlash *pNORFlash )
{
	cgc_uint8_t *pMagicPage = (cgc_uint8_t*)secret_page_i;

	cgc_CNORFlash oFlash;
	cgc_CFlashFS oFS;

	// Use magic page data to init device
	cgc_uint32_t blockSize, blocksPerSector, sectorCount;
	
	if ( pMagicPage[0] < 0x80 )
		blockSize = 256;
	else
		blockSize = 512;

	blocksPerSector = ((pMagicPage[1] % 3)+2) * 4; // 8, 12, 16

	sectorCount = ((pMagicPage[2] % 32)+128) * 4; // 128 -> 252 sectors
 
	pNORFlash->cgc_Init( blockSize, blocksPerSector, sectorCount );

	pFS->cgc_Init( pNORFlash );

	cgc_printf( "Starting test for device ($d, $d, $d):\n", blockSize, blocksPerSector, sectorCount );
}

void cgc_RunTestSystem( cgc_CFlashFS *pFS )
{
	cgc_uint8_t tempData[MAX_TEMP_TEST_LEN];

	bool bDone = false;
	do
	{
		cgc_uint8_t commandNum;
	
		if ( cgc_RecvData( STDIN, &commandNum, sizeof(commandNum) ) != sizeof(commandNum) )
			break;

		switch ( commandNum )
		{
		case 0:	// cgc_OpenFile
			bDone = cgc_OpenFileCommand( pFS );
			break;

		case 1:	// cgc_CloseFile
			bDone = cgc_CloseFileCommand( pFS );
			break;

		case 2:	// cgc_WriteFile
			bDone = cgc_WriteFileCommand( pFS, tempData );
			break;

		case 3:	// cgc_ReadFile
			bDone = cgc_ReadFileCommand( pFS, tempData );
			break;

		case 4:	// cgc_DeleteFile
			bDone = cgc_DeleteFileCommand( pFS );
			break;

		case 5:
			cgc_printf("Stop simulation\n" );
			bDone = true;
			break;

		default:
			cgc_printf( "Invalid command\n" );
			bDone = true;
			// Invalid command
			break;
		}
	} while ( !bDone );
}

int __attribute__((fastcall)) main(int secret_page_i, char *unused[]) 
{
	cgc_CNORFlash oFlash;
	cgc_CFlashFS oFS;

	cgc_SetupDevice( secret_page_i, &oFS, &oFlash );
	cgc_RunTestSystem( &oFS );
}

#if 0
	cgc_CNORFlash oFlash;

	oFlash.cgc_Init( 512, 16, 128 );

	cgc_printf( "$08X\n", secret_page_i );

	cgc_TestFS( &oFlash );

	return (0);
}
#endif
