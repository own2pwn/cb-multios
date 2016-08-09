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

#include <cutil_string.h>
#include <cutil_vector.h>

#include "comms.h"
#include "databasefs.h"
#include "networkfs.h"

int cgc_strcmp_unsigned( const unsigned char *s1, const unsigned char *s2 )
{
	if (s1 && !s2) {
                return(1);
        }
        if (!s1 && s2) {
                return(-1);
        }
        if (!s1 && !s2) {
                return(0);
        }

        while (*s1 != '\0' && *s2 != '\0') {
                if (*s1 > *s2) {
                        return(1);
                }
                if (*s1 < *s2) {
                        return(-1);
                }
                s1++;
                s2++;
        }
        if (*s1 != '\0' && *s2 == '\0') {
                return(1);
        }
        if (*s1 == '\0' && *s2 != '\0') {
                return(-1);
        }
        return(0);	
}

cgc_CNetworkFS::cgc_CNetworkFS( cgc_CTimeGen *pTimeGen )
	: m_pFS( NULL ), m_pComms( NULL ), m_pTimeGen( pTimeGen ), m_maxFiles( DEFAULT_MAX_FILES )
{

}

cgc_CNetworkFS::~cgc_CNetworkFS( )
{
	if ( m_pFS )
		delete m_pFS;

	m_pFS = NULL;
}

bool cgc_CNetworkFS::cgc_Init( cgc_CNetworkComm *pComms, cgc_uint32_t maxFiles )
{
	if ( pComms == NULL )
		return (false);

	if ( m_pFS )
	{
		delete m_pFS;
		m_pFS = NULL;	
	}

	// Setup comms and access password
	m_pComms = pComms;

	m_maxFiles = maxFiles;

	m_pFS = new cgc_CDBFS( m_pTimeGen );

	m_pFS->cgc_Init( "rootpasswd", maxFiles );
	m_pFS->cgc_AddFileAsRoot( "passwd", (cgc_uint8_t*)"root:rootpasswd", 15 );

	return (true);
}

bool cgc_CNetworkFS::cgc_Run( void )
{
	bool bDone = false;

	if ( !m_pComms )
	{
		cgc_SetError( "No comms available!" );
		return (false);
	}

	while ( !bDone )
	{
		// Receive Loop
		
		// Read request
		cgc_tRequestHeader oRequestHdr;

		if ( m_pComms->cgc_RecvData( (cgc_uint8_t*)&oRequestHdr, sizeof(oRequestHdr) ) != sizeof(oRequestHdr) )
		{
			// Exit if no data received
			return (true);
		}

		switch( oRequestHdr.requestType )
		{
		case REQUEST_CFS_LOGIN:
			cgc_HandleCFSLogin();
			break;

		case REQUEST_CFS_DIR:
			cgc_HandleCFSDir();
			break;
	
		case REQUEST_CFS_READ:
			cgc_HandleCFSRead();
			break;
	
		case REQUEST_CFS_WRITE:
			cgc_HandleCFSWrite();
			break;

		case REQUEST_CFS_WRITE_APPEND:
			cgc_HandleCFSWriteAppend();
			break;

		case REQUEST_CFS_DEL:
			cgc_HandleCFSDel();
			break;

		case REQUEST_CFS_RENAME:
			cgc_HandleCFSRename();
			break;

		default:
			cgc_SetError( "Unknown request type!" );
			break;
		}

		if ( cgc_HasError() )
			return (false);
	}

	return (false);
}

void cgc_CNetworkFS::cgc_SetError( const char *pszError )
{
	m_sErrorText = pszError;
	m_bError = true;
}

void cgc_CNetworkFS::cgc_HandleCFSLogin( void )
{
	if ( !m_pFS )
	{
		cgc_SetError( "No file system" );
		return;
	}

	typedef struct cgc_REQUEST_CFS_LOGIN_STRUCT
	{
		cgc_uint8_t usernameLen;
		cgc_uint8_t passwordLen;
	} __attribute__((packed)) cgc_tRequestCFSLogin;

	cgc_tRequestCFSLogin oLoginHdr;

	if ( m_pComms->cgc_RecvData( (cgc_uint8_t*)&oLoginHdr, sizeof(oLoginHdr) ) != sizeof(oLoginHdr) )
	{
		cgc_SetError( "cgc_Login read error" );
		return;
	}

	// Read in username and password
	char *pszUsername = new char[oLoginHdr.usernameLen+1];
	char *pszPassword = new char[oLoginHdr.passwordLen+1];

	if ( m_pComms->cgc_RecvData( (cgc_uint8_t*)pszUsername, oLoginHdr.usernameLen ) != oLoginHdr.usernameLen )
	{
		delete pszUsername;
		delete pszPassword;

		cgc_SetError( "cgc_Login read error" );
		return;
	}

	pszUsername[oLoginHdr.usernameLen] = '\0';

	if ( m_pComms->cgc_RecvData( (cgc_uint8_t*)pszPassword, oLoginHdr.passwordLen ) != oLoginHdr.passwordLen )
	{
		delete pszUsername;
		delete pszPassword;

		cgc_SetError( "cgc_Login read error" );
		return;
	}

	pszPassword[oLoginHdr.passwordLen] = '\0';

	CUtil::cgc_String sUsername = pszUsername;
	CUtil::cgc_String sPassword = pszPassword;

	delete pszUsername;
	delete pszPassword;
	
	// Validate login!
	bool bLoginValid = m_pFS->cgc_SetUser( sUsername, sPassword );

	if ( !bLoginValid )
	{
		// cgc_Login invalid!!
		cgc_SendResponse( REQUEST_CFS_LOGIN, RESPONSE_LOGIN_FAILED, NULL, 0 );
		
		return;
	}

	// cgc_Login succeeded
	cgc_SendResponse( REQUEST_CFS_LOGIN, RESPONSE_SUCCESS, NULL, 0 );
}

void cgc_CNetworkFS::cgc_HandleCFSDir( void )
{
	// Directory listing	
	if ( !m_pFS )
	{
		cgc_SetError( "No file system" );
		return;
	}

	cgc_uint32_t dataSize = 512;

	CUtil::DLL_LIST( cgc_CTempFileList, m_oLink ) oSortedList;

	cgc_CDBFile *pFile;
	for ( pFile = m_pFS->cgc_ListFileFirst( ); pFile; pFile = m_pFS->cgc_ListFileNext( pFile ) )
	{
		if ( pFile->cgc_GetName().cgc_GetLength() > 32 )
			dataSize += pFile->cgc_GetName().cgc_GetLength();
		else
			dataSize += 32;

		if ( pFile->cgc_GetOwner().cgc_GetLength() > 32 )
			dataSize += pFile->cgc_GetOwner().cgc_GetLength();	
		else
			dataSize += 32;

		// Create sorted list
		if ( oSortedList.cgc_IsEmpty() )
			oSortedList.cgc_AddLast( new cgc_CTempFileList( pFile ) );
		else
		{
			// Find the right location
			cgc_CTempFileList *pPrevFile = NULL;
			cgc_CTempFileList *pCurFile = NULL;
			for ( pCurFile = oSortedList.cgc_GetFirst(); pCurFile; pCurFile = oSortedList.cgc_GetNext( pCurFile ) )
			{
				if ( cgc_strcmp_unsigned( (unsigned char *)pFile->cgc_GetName().cgc_c_str(), (unsigned char *)pCurFile->m_pFile->cgc_GetName().cgc_c_str() ) < 0 )
					break;

				pPrevFile = pCurFile;
			}
	
			if ( pPrevFile == NULL )
				oSortedList.cgc_AddFirst( new cgc_CTempFileList( pFile ) );
			else if ( pPrevFile == pCurFile )
				oSortedList.cgc_AddLast( new cgc_CTempFileList( pFile ) );
			else
				oSortedList.cgc_AddAfter( pPrevFile, new cgc_CTempFileList( pFile ) );
		}	

		dataSize += 8;	// Size
		dataSize += 4;  // Mode
		dataSize += 10;  // Timestamp
		dataSize += 5;  // Spaces and newline
	}

	char *pszFileList = new char[dataSize];
	cgc_uint32_t pos = 0;

	pos += cgc_sprintf( pszFileList, "$-32s $-32s $-8s $-4s $-10s\n", "Filename", "Owner", "Size", "Mode", "Timestamp" );

	cgc_uint32_t sendSize = 0;	
	
	cgc_CTempFileList *pFileList;	
	for ( pFileList = oSortedList.cgc_GetFirst(); pFileList; pFileList = oSortedList.cgc_GetNext( pFileList ) )	
	{
		pFile = pFileList->m_pFile;

		char modeStr[6] = "----";

		cgc_uint8_t modeBits = pFile->cgc_GetModeBits();

		if ( modeBits & FS_MODE_OWNER_READ )
			modeStr[0] = 'r';

		if ( modeBits & FS_MODE_OWNER_WRITE )
			modeStr[1] = 'w';
	
		if ( modeBits & FS_MODE_OTHER_READ )
			modeStr[2] = 'r';
		
		if ( modeBits & FS_MODE_OTHER_WRITE )
			modeStr[3] = 'w';

		pos += cgc_sprintf( pszFileList+pos, "$-32s $-32s $-8d $-4s $-10d\n", pFile->cgc_GetName().cgc_c_str(), pFile->cgc_GetOwner().cgc_c_str(), pFile->cgc_GetFileSize(), modeStr, pFile->cgc_GetAccessTime() );
	}

	// Delete all the temporary storage holders
	oSortedList.cgc_DeleteAll();

	// Send pos worth of data for response
	cgc_SendResponse( REQUEST_CFS_DIR, RESPONSE_SUCCESS, (cgc_uint8_t*)pszFileList, pos );

	delete pszFileList;
}

void cgc_CNetworkFS::cgc_HandleCFSRead( void )
{
	if ( !m_pFS )
	{
		cgc_SetError( "No file system" );
		return;
	}

	typedef struct cgc_REQUEST_CFS_READ_STRUCT
	{
		cgc_uint32_t fileOffset;
		cgc_uint16_t readLength;
		cgc_uint8_t fileNameLength;
	} __attribute__((packed)) cgc_tRequestCFSRead;

	cgc_tRequestCFSRead oReadHdr;

	if ( m_pComms->cgc_RecvData( (cgc_uint8_t*)&oReadHdr, sizeof(oReadHdr) ) != sizeof(oReadHdr) )
	{
		cgc_SetError( "Read read error" );
		return;
	}

	char *pszFilename = new char[oReadHdr.fileNameLength+1];

	// Read in filename
	if ( m_pComms->cgc_RecvData( (cgc_uint8_t*)pszFilename, oReadHdr.fileNameLength ) != oReadHdr.fileNameLength )
	{
		delete pszFilename;

		cgc_SetError( "Read read error" );
		return;
	}

	pszFilename[oReadHdr.fileNameLength] = '\0';

	CUtil::cgc_String sFilename = pszFilename;
	delete pszFilename;

	// Open file
	cgc_CDBFSOpenFile *pFP = m_pFS->cgc_OpenFile( sFilename, "r" );

	if ( !pFP )
	{
		cgc_SendResponse( REQUEST_CFS_READ, RESPONSE_INVALID_FILE, NULL, 0 );

		return;
	}

	// Seek
	if ( m_pFS->cgc_FileSeek( pFP, oReadHdr.fileOffset, SEEK_SET ) != 0 )
	{
		cgc_SendResponse( REQUEST_CFS_READ, RESPONSE_SYSTEM_FAILURE, NULL, 0 );

		return;
	}

	// Read
	// Allocate send buffer
	cgc_uint8_t *pReadData = new cgc_uint8_t[ oReadHdr.readLength ];

	if ( m_pFS->cgc_FileRead( pReadData, oReadHdr.readLength, 1, pFP ) == 0 )
	{
		// Read failure
		delete pReadData;

		cgc_SendResponse( REQUEST_CFS_READ, RESPONSE_SYSTEM_FAILURE, NULL, 0 );

		return;
	}

	// Close file
	m_pFS->cgc_CloseFile( pFP );
	pFP = NULL;

	cgc_SendResponse( REQUEST_CFS_READ, RESPONSE_SUCCESS, pReadData, oReadHdr.readLength );

	return;
}

void cgc_CNetworkFS::cgc_HandleCFSWrite( void )
{
	if ( !m_pFS )
	{
		cgc_SetError( "No file system" );
		return;
	}

	typedef struct cgc_REQUEST_CFS_WRITE_STRUCT
	{
		cgc_uint16_t writeLength;
		cgc_uint8_t fileNameLength;
	} __attribute__((packed)) cgc_tRequestCFSWrite;

	cgc_tRequestCFSWrite oWriteHdr;

	if ( m_pComms->cgc_RecvData( (cgc_uint8_t*)&oWriteHdr, sizeof(oWriteHdr) ) != sizeof(oWriteHdr) )
	{
		cgc_SetError( "Write read error" );
		return;
	}

	char *pszFilename = new char[oWriteHdr.fileNameLength+1];
	cgc_uint8_t *pData = new cgc_uint8_t[oWriteHdr.writeLength];

	// Read in filename
	if ( m_pComms->cgc_RecvData( (cgc_uint8_t*)pszFilename, oWriteHdr.fileNameLength ) != oWriteHdr.fileNameLength )
	{
		delete pszFilename;
		delete pData;

		cgc_SetError( "Write read error" );
		return;
	}

	pszFilename[oWriteHdr.fileNameLength] = '\0';

	if ( m_pComms->cgc_RecvData( pData, oWriteHdr.writeLength ) != oWriteHdr.writeLength )
	{
		delete pszFilename;
		delete pData;

		cgc_SetError( "Write read error" );
		return;
	}

	CUtil::cgc_String sFilename = pszFilename;
	delete pszFilename;

	// Open file
	cgc_CDBFSOpenFile *pFP = m_pFS->cgc_OpenFile( sFilename, "w" );

	if ( !pFP )
	{
		delete pData;

		cgc_SendResponse( REQUEST_CFS_WRITE, RESPONSE_INVALID_FILE, NULL, 0 );

		return;
	}

	// Seek
	if ( m_pFS->cgc_FileSeek( pFP, 0, SEEK_SET ) != 0 )
	{
		delete pData;

		cgc_SendResponse( REQUEST_CFS_WRITE, RESPONSE_SYSTEM_FAILURE, NULL, 0 );

		return;
	}

	// Write
	// Send buffer
	if ( m_pFS->cgc_FileWrite( pData, oWriteHdr.writeLength, 1, pFP ) == 0 )
	{
		// Write failure
		delete pData;

		cgc_SendResponse( REQUEST_CFS_WRITE, RESPONSE_SYSTEM_FAILURE, NULL, 0 );

		return;
	}

	// Free buffer
	delete pData;

	// Close file
	m_pFS->cgc_CloseFile( pFP );
	pFP = NULL;

	cgc_SendResponse( REQUEST_CFS_WRITE, RESPONSE_SUCCESS, NULL, 0 );

	return;
}

void cgc_CNetworkFS::cgc_HandleCFSWriteAppend( void )
{
	if ( !m_pFS )
	{
		cgc_SetError( "No file system" );
		return;
	}

	typedef struct cgc_REQUEST_CFS_WRITE_APPEND_STRUCT
	{
		cgc_uint16_t dataLen;
		cgc_uint8_t fileNameLength;
	} __attribute__((packed)) cgc_tRequestCFSWriteAppend;

	cgc_tRequestCFSWriteAppend oWriteAppendHdr;

	if ( m_pComms->cgc_RecvData( (cgc_uint8_t*)&oWriteAppendHdr, sizeof(oWriteAppendHdr) ) != sizeof(oWriteAppendHdr) )
	{
		cgc_SetError( "Write append read error" );
		return;
	}

	cgc_uint8_t *pData = new cgc_uint8_t[oWriteAppendHdr.dataLen];
	char *pszFilename = new char[oWriteAppendHdr.fileNameLength+1];

	// Read in filename
	if ( m_pComms->cgc_RecvData( (cgc_uint8_t*)pszFilename, oWriteAppendHdr.fileNameLength ) != oWriteAppendHdr.fileNameLength )
	{
		delete pData;
		delete pszFilename;

		cgc_SetError( "Write append read error" );
		return;
	}

	pszFilename[oWriteAppendHdr.fileNameLength] = '\0';

	// Read data
	if ( m_pComms->cgc_RecvData( pData, oWriteAppendHdr.dataLen ) != oWriteAppendHdr.dataLen )
	{
		delete pData;
		delete pszFilename;

		cgc_SetError( "Write append read error" );
		return;
	}

	CUtil::cgc_String sFilename = pszFilename;
	delete pszFilename;

	// Open file (in append mode)
	cgc_CDBFSOpenFile *pFP = m_pFS->cgc_OpenFile( sFilename, "a" );

	if ( !pFP )
	{
		delete pData;

		cgc_SendResponse( REQUEST_CFS_WRITE_APPEND, RESPONSE_INVALID_FILE, NULL, 0 );

		return;
	}

	// Write
	if ( m_pFS->cgc_FileWrite( pData, oWriteAppendHdr.dataLen, 1, pFP ) == 0 )
	{
		// Write failure
		delete pData;

		cgc_SendResponse( REQUEST_CFS_WRITE_APPEND, RESPONSE_SYSTEM_FAILURE, NULL, 0 );

		return;
	}
	
	// Free buffer
	delete pData;

	// Close file
	m_pFS->cgc_CloseFile( pFP );
	pFP = NULL;

	cgc_SendResponse( REQUEST_CFS_WRITE_APPEND, RESPONSE_SUCCESS, NULL, 0 );
}

void cgc_CNetworkFS::cgc_HandleCFSDel( void )
{
	if ( !m_pFS )
	{
		cgc_SetError( "No file system" );
		return;
	}

	typedef struct cgc_REQUEST_CFS_DELETE_STRUCT
	{
		cgc_uint8_t fileNameLength;
	} __attribute__((packed)) cgc_tRequestCFSDelete;

	cgc_tRequestCFSDelete oDeleteHdr;

	if ( m_pComms->cgc_RecvData( (cgc_uint8_t*)&oDeleteHdr, sizeof(oDeleteHdr) ) != sizeof(oDeleteHdr) )
	{
		cgc_SetError( "Delete read error" );
		return;
	}

	char *pszFilename = new char[oDeleteHdr.fileNameLength+1];

	// Read in filename
	if ( m_pComms->cgc_RecvData( (cgc_uint8_t*)pszFilename, oDeleteHdr.fileNameLength ) != oDeleteHdr.fileNameLength )
	{
		delete pszFilename;

		cgc_SetError( "Delete read error" );
		return;
	}

	pszFilename[oDeleteHdr.fileNameLength] = '\0';

	CUtil::cgc_String sFilename = pszFilename;
	delete pszFilename;

	if ( !m_pFS->cgc_DeleteFile( sFilename ) )
	{
		cgc_SendResponse( REQUEST_CFS_DEL, RESPONSE_DELETE_FAILED, NULL, 0 );

		return;
	}
	
	cgc_SendResponse( REQUEST_CFS_DEL, RESPONSE_SUCCESS, NULL, 0 );
}

void cgc_CNetworkFS::cgc_HandleCFSRename( void )
{
	if ( !m_pFS )
	{
		cgc_SetError( "No file system" );
		return;
	}

	typedef struct cgc_REQUEST_CFS_RENAME_STRUCT
	{
		cgc_uint8_t origFileNameLength;
		cgc_uint8_t newFileNameLength;
	} __attribute__((packed)) cgc_tRequestCFSRename;

	cgc_tRequestCFSRename oRenameHdr;

	if ( m_pComms->cgc_RecvData( (cgc_uint8_t*)&oRenameHdr, sizeof(oRenameHdr) ) != sizeof(oRenameHdr) )
	{
		cgc_SetError( "Rename read error" );
		return;
	}

	char *pszOrigFilename = new char[oRenameHdr.origFileNameLength+1];
	char *pszNewFilename = new char [oRenameHdr.newFileNameLength+1];

	// Read in filename
	if ( m_pComms->cgc_RecvData( (cgc_uint8_t*)pszOrigFilename, oRenameHdr.origFileNameLength ) != oRenameHdr.origFileNameLength )
	{
		delete pszOrigFilename;
		delete pszNewFilename;

		cgc_SetError( "Rename read error" );
		return;
	}

	pszOrigFilename[oRenameHdr.origFileNameLength] = '\0';

	if ( m_pComms->cgc_RecvData( (cgc_uint8_t*)pszNewFilename, oRenameHdr.newFileNameLength ) != oRenameHdr.newFileNameLength )
	{
		delete pszOrigFilename;
		delete pszNewFilename;

		cgc_SetError( "Rename read error" );
		return;
	}

	pszNewFilename[oRenameHdr.newFileNameLength] = '\0';

	CUtil::cgc_String sOrigFilename = pszOrigFilename;
	CUtil::cgc_String sNewFilename = pszNewFilename;

	delete pszOrigFilename;
	delete pszNewFilename;

	if ( !m_pFS->cgc_RenameFile( sOrigFilename, sNewFilename ) )
	{
		cgc_SendResponse( REQUEST_CFS_RENAME, RESPONSE_RENAME_FAILED, NULL, 0 );

		return;
	}
	
	cgc_SendResponse( REQUEST_CFS_RENAME, RESPONSE_SUCCESS, NULL, 0 );
}

void cgc_CNetworkFS::cgc_SendResponse( cgc_eRequestType requestType, cgc_eResponseType responseType, cgc_uint8_t *pResponseData, cgc_uint16_t responseLength )
{
	cgc_tResponseHeader oResponseHdr;

	oResponseHdr.responseType = requestType;
	oResponseHdr.responseCode = responseType;
	oResponseHdr.responseLength = responseLength;

	// Create and send a resopnse -- include header and response data
	m_pComms->cgc_SendData( (cgc_uint8_t*)&oResponseHdr, sizeof(oResponseHdr) );

	if ( responseLength == 0 )
		return;

	m_pComms->cgc_SendData( (cgc_uint8_t*)pResponseData, responseLength );
}
