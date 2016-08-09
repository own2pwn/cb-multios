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
#ifndef __DBFS_H__
#define __DBFS_H__

#include <cutil_list.h>
#include <cutil_string.h>

#include "timegen.h"

#define SEEK_SET		(1)
#define SEEK_CUR		(0)
#define SEEK_END		(2)

#define FS_NODE_FILE		(1)
#define FS_NODE_DIRECTORY	(2)

#define FS_MODE_OWNER_READ	(1)
#define FS_MODE_OWNER_WRITE	(2)
#define FS_MODE_OTHER_READ	(4)
#define FS_MODE_OTHER_WRITE	(8)

// Open modes
#define FILE_OPEN_READ		(1)
#define FILE_OPEN_WRITE		(2)

class cgc_CDBNode 
{
public:
	cgc_CDBNode( const CUtil::cgc_String &sName, const CUtil::cgc_String &sOwner, cgc_uint8_t modeBits );
	cgc_CDBNode( const char *pszName, const char *pszOwner, cgc_uint8_t modeBits );
	virtual ~cgc_CDBNode( );
	
	const CUtil::cgc_String &cgc_GetName( void ) const { return m_sName; };
	const CUtil::cgc_String &cgc_GetOwner( void ) const { return m_sOwner; };
	cgc_uint8_t cgc_GetModeBits( void ) const { return m_modeBits; };

	virtual cgc_uint8_t cgc_GetType( void ) const = 0;

	void cgc_SetName( const char *pszName ) { m_sName = pszName; };
	void cgc_SetOwner( const char *pszOwner ) { m_sOwner = pszOwner; };

	cgc_uint32_t cgc_GetAccessTime( void ) const { return m_accessTime; };
	void cgc_SetAccessTime( cgc_uint32_t accessTime ) { m_accessTime = accessTime; };

protected:
	CUtil::cgc_String m_sName;
	CUtil::cgc_String m_sOwner;
	cgc_uint8_t m_modeBits;
	cgc_uint32_t m_accessTime;

public:	
	CUtil::DLL_LINK( cgc_CDBNode ) m_dbLink;	
};

// 1 MB max file size
#define FILE_CHUNK_SIZE		(4096)
#define FILE_MAX_CHUNKS		(256)
#define MAX_FILE_SIZE		(FILE_CHUNK_SIZE * FILE_MAX_CHUNKS)

#define FILE_WRITE_OK			(0)
#define FILE_WRITE_ERROR_MAX_FILESIZE	(1)
#define FILE_WRITE_PAST_EOF		(2)
#define FILE_WRITE_ERROR		(3)

#define FILE_READ_OK			(0)
#define FILE_READ_EOF			(1)
#define FILE_READ_ERROR			(2)

class cgc_CDBFile : public cgc_CDBNode
{
public:
	cgc_CDBFile( const CUtil::cgc_String &sName, const CUtil::cgc_String &sOwner, cgc_uint8_t modeBits );
	cgc_CDBFile( const char *pszName, const char *pszOwner, cgc_uint8_t modeBits );
	~cgc_CDBFile( );

	cgc_uint8_t cgc_GetType( void ) const { return (FS_NODE_FILE); };

	cgc_int32_t cgc_GetFileSize( void ) const { return m_fileSize; };

	cgc_uint8_t cgc_WriteData( cgc_int32_t writePos, cgc_uint8_t *pData, cgc_uint32_t writeLength, cgc_uint32_t &writeActual );
	cgc_uint8_t cgc_ReadData( cgc_int32_t readPos, cgc_uint8_t *pData, cgc_uint32_t readLength, cgc_uint32_t &readActual );

protected:
	typedef struct cgc_FILE_CHUNK
	{
		cgc_uint8_t chunkData[FILE_CHUNK_SIZE];
	} cgc_tFileChunk;

	cgc_int32_t m_fileSize;

	cgc_tFileChunk *m_pChunks[FILE_MAX_CHUNKS];
};

class cgc_CDBFSUser
{
public:
	cgc_CDBFSUser( const CUtil::cgc_String &sUsername, const CUtil::cgc_String &sPassword );
	cgc_CDBFSUser( const char *pszUsername, const char *pszPassword );
	~cgc_CDBFSUser( );

	const CUtil::cgc_String &cgc_GetUsername( void ) const { return m_sUsername; };
	const CUtil::cgc_String &cgc_GetPassword( void ) const { return m_sPassword; };

private:
	CUtil::cgc_String m_sUsername;
	CUtil::cgc_String m_sPassword;

public:
	CUtil::DLL_LINK( cgc_CDBFSUser ) m_fsUserLink;
};

class cgc_CDBFSOpenFile
{
public:
	cgc_CDBFSOpenFile( cgc_CDBFile *pFile, cgc_uint8_t openMode );
	~cgc_CDBFSOpenFile( );

	cgc_CDBFile *cgc_GetFile( void ) { return m_pFile; };
	
	cgc_int32_t cgc_GetFilePosition( void ) const { return m_curFilePos; };
	bool cgc_SetFilePosition( cgc_int32_t filePosition )
	{
		if ( !m_pFile )
			return (false);

		if ( filePosition > m_pFile->cgc_GetFileSize() )
			filePosition = m_pFile->cgc_GetFileSize();

		m_curFilePos = filePosition;

		return (true);
	}

	bool cgc_SetFilePositionEnd( void )
	{
		if ( !m_pFile )
			return (false);

		m_curFilePos = m_pFile->cgc_GetFileSize();

		return (true);	
	}

	cgc_uint8_t cgc_GetMode( void ) const { return m_openMode; };

private:
	cgc_uint8_t m_openMode;
	cgc_int32_t m_curFilePos;
	cgc_CDBFile *m_pFile;

public:
	CUtil::DLL_LINK( cgc_CDBFSOpenFile ) m_openFileLink;
};

// Database File System 
class cgc_CDBFS
{
public:
	cgc_CDBFS( cgc_CTimeGen *pTimeGen );
	~cgc_CDBFS( );

	bool cgc_Init( CUtil::cgc_String sRootPassword, cgc_uint32_t maxFiles );

	bool cgc_AddFileAsRoot( CUtil::cgc_String sFilename, cgc_uint8_t *pFileData, cgc_uint32_t dataLen );

	cgc_CDBFSUser *cgc_FindUser( CUtil::cgc_String sUsername );

	bool cgc_SetUser( CUtil::cgc_String sUsername, CUtil::cgc_String sPassword );

	bool cgc_AddUser( CUtil::cgc_String sUsername, CUtil::cgc_String sPassword );

	bool cgc_DeleteUser( CUtil::cgc_String sUsername );

	// Accessor
	bool cgc_HasLastError( void ) const { return m_bLastError; };
	const CUtil::cgc_String &cgc_GetLastError( void ) const { return m_sLastError; };
	
	// FS functions
	bool cgc_RenameFile( CUtil::cgc_String sOldFilename, CUtil::cgc_String sNewFilename );
	bool cgc_DeleteFile( CUtil::cgc_String sFilename );

	cgc_CDBFSOpenFile *cgc_OpenFile( CUtil::cgc_String sFilename, const char *pszMode );
	bool cgc_CloseFile( cgc_CDBFSOpenFile *pFile );

	cgc_CDBFile *cgc_ListFileFirst( void ) { return (cgc_CDBFile *)m_nodeList.cgc_GetFirst(); };
	cgc_CDBFile *cgc_ListFileNext( cgc_CDBFile *pCur ) { return (cgc_CDBFile *)m_nodeList.cgc_GetNext( pCur ); };

	cgc_uint32_t cgc_FileRead( cgc_uint8_t *pBuf, cgc_uint32_t size, cgc_uint32_t nitems, cgc_CDBFSOpenFile *pFP );
	cgc_uint32_t cgc_FileWrite( cgc_uint8_t *pBuf, cgc_uint32_t size, cgc_uint32_t nitems, cgc_CDBFSOpenFile *pFP );
	cgc_uint32_t cgc_FileSeek( cgc_CDBFSOpenFile *pFP, cgc_int32_t offset, cgc_int8_t origin );

private:
	bool cgc_IsUserRoot( void )
	{
		if ( m_pCurUser && m_pCurUser->cgc_GetUsername() == "root" )
			return (true);
		else
			return (false);
	}

	void cgc_SetError( const char *pszError ) { m_bLastError = true; m_sLastError = pszError; };
	void cgc_ClearError( void ) { m_bLastError = false; m_sLastError = ""; };

	cgc_CDBFile *cgc_FindFileByName( const CUtil::cgc_String &sFilename );

	bool cgc_IsFileOpen( cgc_CDBFile *pFile );

	bool cgc_IsMaxFilesReached( void ) 
	{
		return (m_curFiles >= m_maxFiles);
	}

	cgc_CDBFile *cgc_CreateFile( const CUtil::cgc_String &sFilename, const CUtil::cgc_String &sOwner, cgc_uint8_t modeBits )
	{
		cgc_CDBFile *pFile = new cgc_CDBFile( sFilename, sOwner, modeBits );

		// If access time is available set it	
		if ( m_pTimeGen )
			pFile->cgc_SetAccessTime( m_pTimeGen->cgc_GetUnixTimestamp() );
	
		m_nodeList.cgc_AddLast( pFile );

		return (pFile);
	}

private:
	cgc_uint32_t m_maxFiles;
	cgc_uint32_t m_curFiles;

	bool m_bLastError;
	CUtil::cgc_String m_sLastError;

	cgc_CTimeGen *m_pTimeGen;

	cgc_CDBFSUser *m_pCurUser;

	// Starting nodes (root of file system tree)
	CUtil::DLL_LIST( cgc_CDBNode, m_dbLink )	m_nodeList;	

	// List of users that can access file system
	CUtil::DLL_LIST( cgc_CDBFSUser, m_fsUserLink ) m_userList;

	// List of open files for file system
	CUtil::DLL_LIST( cgc_CDBFSOpenFile, m_openFileLink ) m_openFileList;
};

#endif // __DBFS_H__
