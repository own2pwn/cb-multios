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

#include <cutil_list.h>
#include <cutil_string.h>

#include "databasefs.h"

cgc_CDBNode::cgc_CDBNode( const CUtil::cgc_String &sName, const CUtil::cgc_String &sOwner, cgc_uint8_t modeBits )
	: m_modeBits( modeBits ), m_sName( sName ), m_sOwner( sOwner )
{

}

cgc_CDBNode::cgc_CDBNode( const char *pszName, const char *pszOwner, cgc_uint8_t modeBits )
	: m_modeBits( modeBits ), m_sName( pszName ), m_sOwner( pszOwner )
{

}

cgc_CDBNode::~cgc_CDBNode( )
{

}

cgc_CDBFile::cgc_CDBFile( const CUtil::cgc_String &sName, const CUtil::cgc_String &sOwner, cgc_uint8_t modeBits )
	: cgc_CDBNode( sName, sOwner, modeBits ), m_fileSize( 0 )
{
	cgc_bzero( m_pChunks, sizeof(cgc_tFileChunk *)*FILE_MAX_CHUNKS );
}

cgc_CDBFile::cgc_CDBFile( const char *pszName, const char *pszOwner, cgc_uint8_t modeBits )
	: cgc_CDBNode( pszName, pszOwner, modeBits ), m_fileSize( 0 )
{
	cgc_bzero( m_pChunks, sizeof(cgc_tFileChunk *)*FILE_MAX_CHUNKS );
}

cgc_CDBFile::~cgc_CDBFile( )
{
	if ( m_fileSize > 0 )
	{
		cgc_uint32_t topChunk = (m_fileSize / FILE_CHUNK_SIZE)+1;
		if ( topChunk > FILE_MAX_CHUNKS )
			topChunk = FILE_MAX_CHUNKS;

		for ( cgc_uint32_t curChunk = 0; curChunk < topChunk; curChunk++ )
		{
			if ( m_pChunks[curChunk] )
			{
				delete m_pChunks[curChunk];
				m_pChunks[curChunk] = NULL;
			}
		}
	}

	m_fileSize = 0;
}

cgc_uint8_t cgc_CDBFile::cgc_WriteData( cgc_int32_t writePos, cgc_uint8_t *pData, cgc_uint32_t writeLength, cgc_uint32_t &writeActual )
{
	if ( !pData )
		return FILE_WRITE_ERROR;

	cgc_uint32_t writeAmount = writeLength;
	if ( writePos + writeLength > MAX_FILE_SIZE )
	{
		return FILE_WRITE_ERROR_MAX_FILESIZE;
	}

	if ( writePos > m_fileSize )
	{
		return FILE_WRITE_PAST_EOF;
	}

	// Begin writing
	cgc_uint32_t fromPos = 0;
	while ( writeAmount > 0 )
	{
		cgc_uint32_t chunkPos = (writePos / FILE_CHUNK_SIZE);
		cgc_uint32_t chunkOffset = (writePos % FILE_CHUNK_SIZE);

		cgc_uint32_t chunkToWrite = writeAmount;
		if ( chunkToWrite > (FILE_CHUNK_SIZE - chunkOffset) )
			chunkToWrite = (FILE_CHUNK_SIZE - chunkOffset);

		if ( !m_pChunks[chunkPos] )
		{
			m_pChunks[chunkPos] = new cgc_tFileChunk;
			cgc_bzero( m_pChunks[chunkPos]->chunkData, FILE_CHUNK_SIZE );
		}

		cgc_memcpy( m_pChunks[chunkPos]->chunkData+chunkOffset, pData+fromPos, chunkToWrite );
		
		writeAmount -= chunkToWrite;
		fromPos += chunkToWrite;
		writePos += chunkToWrite;	 	
	}

	if ( writePos > m_fileSize )
		m_fileSize = writePos;

	writeActual = fromPos;

	return (FILE_WRITE_OK);	
}

cgc_uint8_t cgc_CDBFile::cgc_ReadData( cgc_int32_t readPos, cgc_uint8_t *pData, cgc_uint32_t readLength, cgc_uint32_t &readActual )
{
	if ( readPos > m_fileSize )
		return (FILE_READ_ERROR);

	if ( readLength == 0 )
		return (FILE_READ_OK);

	if ( !pData )
		return (FILE_READ_EOF);

	cgc_uint32_t readAmount = readLength;
	if ( readPos+readAmount > m_fileSize )
		readAmount = (m_fileSize - readPos);

	if ( readAmount > readLength )
		readAmount = readLength;

	cgc_uint32_t fromPos = 0;
	while ( readAmount > 0 )
	{
		cgc_uint32_t chunkPos = (readPos / FILE_CHUNK_SIZE);
		cgc_uint32_t chunkOffset = (readPos % FILE_CHUNK_SIZE);

		cgc_uint32_t chunkToRead = readAmount;
		if ( chunkToRead > (FILE_CHUNK_SIZE - chunkOffset) )
			chunkToRead = (FILE_CHUNK_SIZE - chunkOffset);

		if ( !m_pChunks[chunkPos] )
		{
			// Critical error
			return (FILE_READ_ERROR);
		}

		cgc_memcpy( pData+fromPos, m_pChunks[chunkPos]->chunkData+chunkOffset, chunkToRead );

		readAmount -= chunkToRead;
		fromPos += chunkToRead;
		readPos += chunkToRead;	 	
	}
	
	readActual = fromPos;

	return (FILE_READ_OK);	
}

cgc_CDBFSUser::cgc_CDBFSUser( const CUtil::cgc_String &sUsername, const CUtil::cgc_String &sPassword )
	: m_sUsername( sUsername ), m_sPassword( sPassword )
{

}

cgc_CDBFSUser::cgc_CDBFSUser( const char *pszUsername, const char *pszPassword )
	: m_sUsername( pszUsername ), m_sPassword( pszPassword )
{

}

cgc_CDBFSUser::~cgc_CDBFSUser( )
{

}

cgc_CDBFSOpenFile::cgc_CDBFSOpenFile( cgc_CDBFile *pFile, cgc_uint8_t openMode )
	: m_pFile( pFile ), m_curFilePos( 0 ), m_openMode( openMode )
{

}

cgc_CDBFSOpenFile::~cgc_CDBFSOpenFile( )
{

}

cgc_CDBFS::cgc_CDBFS( cgc_CTimeGen *pTimeGen )
	: m_bLastError( false ), m_pTimeGen( pTimeGen ), m_sLastError( "" ), m_pCurUser( NULL ), m_curFiles( 0 ), m_maxFiles( 8 )
{

}

cgc_CDBFS::~cgc_CDBFS( )
{

}

bool cgc_CDBFS::cgc_Init( CUtil::cgc_String sRootPassword, cgc_uint32_t maxFiles )
{
	cgc_ClearError();

	m_maxFiles = maxFiles;
	m_curFiles = 0;

	m_nodeList.cgc_DeleteAll();
	m_userList.cgc_DeleteAll();
	m_openFileList.cgc_DeleteAll();

	m_userList.cgc_AddFirst( new cgc_CDBFSUser( "root", sRootPassword ) );

	m_pCurUser = NULL;

	return (true);
}

bool cgc_CDBFS::cgc_AddFileAsRoot( CUtil::cgc_String sFilename, cgc_uint8_t *pData, cgc_uint32_t dataLen )
{
	if ( cgc_FindFileByName( sFilename ) )
	{
		cgc_SetError( "File already exists" );

		// Failed to add
		return (false);
	}

	if ( cgc_IsMaxFilesReached() )
	{
		cgc_SetError( "Max files created" );
		return (false);
	}

	// Create new file
	cgc_CDBFile *pFile = cgc_CreateFile( sFilename, "root", FS_MODE_OWNER_READ|FS_MODE_OWNER_WRITE );
	m_curFiles++;	

	cgc_uint32_t fileWriteActual;
	cgc_uint32_t fileWriteAmount = dataLen;

	cgc_uint8_t writeStatus;
	if ( (writeStatus = pFile->cgc_WriteData( 0, pData, fileWriteAmount, fileWriteActual)) != FILE_WRITE_OK )
	{
		cgc_SetError( "Write error" );
		return (false);
	}

	// Write success
	cgc_ClearError();

	return (true);	
}

cgc_CDBFSUser *cgc_CDBFS::cgc_FindUser( CUtil::cgc_String sUsername )
{
	cgc_CDBFSUser *pCur;
	
	for ( pCur = m_userList.cgc_GetFirst(); pCur; pCur = m_userList.cgc_GetNext( pCur ) )
	{
		if ( pCur->cgc_GetUsername() == sUsername )
			return (pCur);
	}

	return (NULL);
}

bool cgc_CDBFS::cgc_SetUser( CUtil::cgc_String sUsername, CUtil::cgc_String sPassword )
{
	cgc_CDBFSUser *pUser;

	if ( !(pUser = cgc_FindUser( sUsername )) )
	{
		cgc_SetError( "Invalid username" );

		return (false);
	}

	if ( pUser->cgc_GetPassword() != sPassword )
	{
		cgc_SetError( "Invalid password" );

		return (false);
	}

	cgc_ClearError();
	
	m_pCurUser = pUser;

	return (true);
}

bool cgc_CDBFS::cgc_AddUser( CUtil::cgc_String sUsername, CUtil::cgc_String sPassword )
{
	if ( m_pCurUser == NULL )
	{
		cgc_SetError( "Must be root" );

		return (false);
	}

	if ( m_pCurUser->cgc_GetUsername() != "root" )
	{
		cgc_SetError( "Must be root" );

		return (false);
	}

	if ( cgc_FindUser( sUsername ) )
	{
		cgc_SetError( "User already exists" );

		return (false);
	}

	cgc_ClearError();
	
	m_userList.cgc_AddLast( new cgc_CDBFSUser( sUsername, sPassword ) );

	return (true);
}

bool cgc_CDBFS::cgc_DeleteUser( CUtil::cgc_String sUsername )
{
	if ( m_pCurUser == NULL )
	{
		cgc_SetError( "Must be root" );

		return (false);
	}

	if ( m_pCurUser->cgc_GetUsername() != "root" )
	{
		cgc_SetError( "Must be root" );

		return (false);
	}

	cgc_CDBFSUser *pUser;
	if ( !(pUser = cgc_FindUser( sUsername )) )
	{
		cgc_SetError( "User not found" );

		return (false);
	}

	if ( pUser->cgc_GetUsername() == "root" )
	{
		cgc_SetError( "Can't delete root user" );
		
		return (false);
	}

	cgc_ClearError();

	delete pUser;

	return (true);
}

bool cgc_CDBFS::cgc_RenameFile( CUtil::cgc_String sOldFilename, CUtil::cgc_String sNewFilename )
{
	// Quick check if they are logged in
	if ( m_pCurUser == NULL )
	{
		cgc_SetError( "Permission denied" );
		return (false);
	}
	
	// Rename file anywhere on the file system to a new location (can be a move as well)
	cgc_CDBFile *pFile = cgc_FindFileByName( sOldFilename );

	if ( !pFile )
	{
		cgc_SetError( "Source file not found" );
		return (false);
	}

	// Check permissions
	if ( !cgc_IsUserRoot() && !(pFile->cgc_GetOwner() == m_pCurUser->cgc_GetUsername()) )
	{
		cgc_SetError( "Permission denied" );
		return (false);
	}

	// Check destination
	if ( cgc_FindFileByName( sNewFilename ) )
	{
		cgc_SetError( "File already exists" );
		return (false);
	}

	// Remove
	pFile->m_dbLink.cgc_Unlink();

	// Change name
	pFile->cgc_SetName( sNewFilename.cgc_c_str() );

	// Re-add
	m_nodeList.cgc_AddLast( pFile );

	// Clear file system errors	
	cgc_ClearError();

	return (true);
}

bool cgc_CDBFS::cgc_DeleteFile( CUtil::cgc_String sFilename )
{
	// Quick check if they are logged in
	if ( m_pCurUser == NULL )
	{
		cgc_SetError( "Must login first" );
		return (false);
	}
	
	// Rename file anywhere on the file system to a new location (can be a move as well)
	cgc_CDBFile *pFile = cgc_FindFileByName( sFilename );

	if ( !pFile )
	{
		cgc_SetError( "Invalid filename" );
		return (false);
	}

	// Check permissions
	if ( !cgc_IsUserRoot() && !(pFile->cgc_GetOwner() == m_pCurUser->cgc_GetUsername()) )
	{
		cgc_SetError( "Permission denied" );
		return (false);
	}

	if ( cgc_IsFileOpen( pFile ) )
	{
		// Close all open handles
		if ( pFile )
		{
			cgc_CDBFSOpenFile *pNext;
			for ( cgc_CDBFSOpenFile *pCurOpenFile = m_openFileList.cgc_GetFirst(); pCurOpenFile; pCurOpenFile = pNext )
			{
				pNext = m_openFileList.cgc_GetNext( pCurOpenFile );
				if ( pCurOpenFile->cgc_GetFile() == pFile )
				{
					delete pCurOpenFile;
				}
			}
		}
	}

	// Delete file
	delete pFile;

	// Free up file
	m_curFiles--;

	// Clear file system errors
	cgc_ClearError();

	return (true);
}

cgc_CDBFSOpenFile *cgc_CDBFS::cgc_OpenFile( CUtil::cgc_String sFilename, const char *pszMode )
{
	// Check mode
	if ( pszMode == NULL )
	{
		cgc_SetError( "Invalid filename or mode" );
		return (NULL);
	}

	if ( cgc_strlen(pszMode) != 1 )
	{
		cgc_SetError( "Invalid mode" );
		return (NULL);
	}

	if ( pszMode[0] != 'r' && pszMode[0] != 'w' && pszMode[0] != 'a' )
	{
		cgc_SetError( "Invalid mode" );
		return (NULL);
	}

	if ( !m_pCurUser )
	{
		cgc_SetError( "Must login first" );
		return (NULL);
	}

	// Find file
	cgc_CDBFile *pFile = cgc_FindFileByName( sFilename );

	if ( (!pFile && pszMode[0] == 'r') || (!pFile && pszMode[0] == 'a') )
	{
		// Open for reading and file does not exist
		cgc_SetError( "Unable to locate file" );
		return (NULL);
	}

	// If we are creating a file (write mode)
	if ( !pFile )
	{
		// Creating file
		if ( !cgc_IsUserRoot() )
		{
			cgc_SetError( "Permission denied" );
			return (NULL);
		}

		if ( cgc_IsMaxFilesReached() )
		{
			cgc_SetError( "Max files created" );
			return (NULL);
		}

		// Create new file
		pFile = cgc_CreateFile( sFilename, m_pCurUser->cgc_GetUsername(), FS_MODE_OWNER_READ|FS_MODE_OWNER_WRITE );
		m_curFiles++;	
	}	

	// Create open file data structure
	cgc_uint8_t openMode = 0;
	if ( pszMode[0] == 'r' )
		openMode = FILE_OPEN_READ;
	else
		openMode = FILE_OPEN_WRITE;

	cgc_CDBFSOpenFile *pOpenFileData = new cgc_CDBFSOpenFile( pFile, openMode );

	m_openFileList.cgc_AddLast( pOpenFileData ); 

	// If opened in append mode!
	if ( pszMode[0] == 'a' )
		pOpenFileData->cgc_SetFilePositionEnd();

	cgc_ClearError();
	return (pOpenFileData);
}

bool cgc_CDBFS::cgc_CloseFile( cgc_CDBFSOpenFile *pFile )
{
	if ( !pFile )
	{
		cgc_SetError( "Invalid file pointer" );
		return (false);
	}

	// Check for file pointer
	bool bFound = false;	
	for ( cgc_CDBFSOpenFile *pOpenFileCur = m_openFileList.cgc_GetFirst(); pOpenFileCur; pOpenFileCur = m_openFileList.cgc_GetNext( pOpenFileCur ) )
	{
		if ( pOpenFileCur == pFile )
		{
			bFound = true;
			break;
		}
	}

	if ( !bFound )
	{
		cgc_SetError( "Invalid file pointer" );
		return (false);	
	}

	delete pFile;

	// Clear error
	cgc_ClearError();
	return (true);
}

bool cgc_CDBFS::cgc_IsFileOpen( cgc_CDBFile *pFile )
{
	if ( !pFile )
		return (false);

	for ( cgc_CDBFSOpenFile *pOpenFileCur = m_openFileList.cgc_GetFirst(); pOpenFileCur; pOpenFileCur = m_openFileList.cgc_GetNext( pOpenFileCur ) )
	{
		if ( pOpenFileCur->cgc_GetFile() == pFile )
			return (true);
	}

	return (false);
}

cgc_CDBFile *cgc_CDBFS::cgc_FindFileByName( const CUtil::cgc_String &sFilename )
{
	// Scan for file name
	for ( cgc_CDBNode *pNode = m_nodeList.cgc_GetFirst(); pNode; pNode = m_nodeList.cgc_GetNext( pNode ) )
	{
		if ( pNode->cgc_GetType() == FS_NODE_FILE )
		{
			if ( pNode->cgc_GetName() == sFilename )
				return (cgc_CDBFile *)pNode;
		}
	}

	// File not found
	return NULL;
}

cgc_uint32_t cgc_CDBFS::cgc_FileRead( cgc_uint8_t *pBuf, cgc_uint32_t size, cgc_uint32_t nitems, cgc_CDBFSOpenFile *pFP )
{
	if ( !pBuf )
	{
		cgc_SetError( "Invalid buffer" );
		return (0);
	}

	if ( !pFP )
	{
		cgc_SetError( "Invalid file pointer" );
		return (0);
	}

	// Check for file pointer
	bool bFound = false;	
	for ( cgc_CDBFSOpenFile *pOpenFileCur = m_openFileList.cgc_GetFirst(); pOpenFileCur; pOpenFileCur = m_openFileList.cgc_GetNext( pOpenFileCur ) )
	{
		if ( pOpenFileCur == pFP )
		{
			bFound = true;
			break;
		}
	}
	
	if ( !bFound )
	{
		cgc_SetError( "Invalid file pointer" );
		return (0);
	}
	

	cgc_int32_t filePos = pFP->cgc_GetFilePosition();
	cgc_CDBFile *pDBFile = pFP->cgc_GetFile();

	if ( !pDBFile )
	{
		cgc_SetError( "Invalid file pointer" );
		return (0);
	}	

	cgc_uint32_t fileReadAmount = (size*nitems);
	cgc_uint32_t fileReadActual = 0;

	cgc_uint8_t readStatus;

	if ( (readStatus = pDBFile->cgc_ReadData( filePos, pBuf, fileReadAmount, fileReadActual )) != FILE_READ_OK )
	{
		cgc_SetError( "Read error" );
		return (fileReadActual);
	}

	// Clear error
	cgc_ClearError();

	return (fileReadActual);
}

cgc_uint32_t cgc_CDBFS::cgc_FileWrite( cgc_uint8_t *pBuf, cgc_uint32_t size, cgc_uint32_t nitems, cgc_CDBFSOpenFile *pFP )
{
	if ( !pBuf )
	{
		cgc_SetError( "Invalid buffer" );
		return (0);
	}

	if ( !pFP )
	{
		cgc_SetError( "Invalid file pointer" );
		return (0);
	}

	// Check for file pointer
	bool bFound = false;	
	for ( cgc_CDBFSOpenFile *pOpenFileCur = m_openFileList.cgc_GetFirst(); pOpenFileCur; pOpenFileCur = m_openFileList.cgc_GetNext( pOpenFileCur ) )
	{
		if ( pOpenFileCur == pFP )
		{
			bFound = true;
			break;
		}
	}
	
	if ( !bFound )
	{
		cgc_SetError( "Invalid file pointer" );
		return (0);
	}
	
	// Check to make sure file is opened writeable
	if ( pFP->cgc_GetMode() != FILE_OPEN_WRITE )
	{
		cgc_SetError( "File opened read only" );
		return (0);
	}

	cgc_int32_t filePos = pFP->cgc_GetFilePosition();
	cgc_CDBFile *pDBFile = pFP->cgc_GetFile();

	if ( !pDBFile )
	{
		cgc_SetError( "Invalid file pointer" );
		return (0);
	}

	cgc_uint32_t fileWriteAmount = (size*nitems);
	cgc_uint32_t fileWriteActual = 0;

	cgc_uint8_t writeStatus;
	if ( (writeStatus = pDBFile->cgc_WriteData( filePos, pBuf, fileWriteAmount, fileWriteActual)) != FILE_WRITE_OK )
	{
		cgc_SetError( "Write error" );
		return (fileWriteActual);
	}

	// Clear error
	cgc_ClearError();

	return (fileWriteActual);
}

cgc_uint32_t cgc_CDBFS::cgc_FileSeek( cgc_CDBFSOpenFile *pFP, cgc_int32_t offset, cgc_int8_t origin )
{
	if ( !pFP )
	{
		cgc_SetError( "Invalid file pointer" );
		return (-1);
	}

	// Check for file pointer
	bool bFound = false;	
	for ( cgc_CDBFSOpenFile *pOpenFileCur = m_openFileList.cgc_GetFirst(); pOpenFileCur; pOpenFileCur = m_openFileList.cgc_GetNext( pOpenFileCur ) )
	{
		if ( pOpenFileCur == pFP )
		{
			bFound = true;
			break;
		}
	}
	
	if ( !bFound )
	{
		cgc_SetError( "Invalid file pointer" );
		return (-1);
	}
	
	cgc_int32_t filePos = pFP->cgc_GetFilePosition();
	cgc_CDBFile *pFile = pFP->cgc_GetFile();

	if ( !pFile )
	{
		cgc_SetError( "Invalid file pointer" );
		return (-1);
	}

	cgc_int32_t newPos = 0;
	if ( origin == SEEK_SET )
		newPos = offset;
	else if ( origin == SEEK_CUR )
		newPos = ((cgc_int32_t)filePos) + offset;
	else if ( origin == SEEK_END )
		newPos = ((cgc_int32_t)pFile->cgc_GetFileSize()) + offset;

#ifdef PATCHED_1
	if ( newPos < 0 || newPos > pFile->cgc_GetFileSize() )
#else
	if ( newPos > pFile->cgc_GetFileSize() )
#endif
	{
		cgc_SetError( "Out of range" );
		return (-1);
	} 

	if ( !pFP->cgc_SetFilePosition( newPos ) )
	{
		cgc_SetError( "Out of range" );
		return (-1);
	}

	// Clear errors
	cgc_ClearError();

	return (0);
}
