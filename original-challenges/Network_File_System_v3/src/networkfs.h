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
#ifndef __NETWORK_FS_H__
#define __NETWORK_FS_H__

#include <cutil_list.h>

#include "databasefs.h"
#include "timegen.h"

class cgc_CNetworkComm;
class cgc_CDBFS;

class cgc_CNetworkFS
{
public:
	static const cgc_uint32_t DEFAULT_MAX_FILES = 10;

public:
	cgc_CNetworkFS( cgc_CTimeGen *pTimeGen );
	~cgc_CNetworkFS( );

	bool cgc_Init( cgc_CNetworkComm *pComms, cgc_uint32_t maxFiles );

	bool cgc_Run( void );

	void cgc_SetError( const char *pszError );

	bool cgc_HasError( void ) const { return m_bError; };
	const CUtil::cgc_String &cgc_GetError( void ) const { return m_sErrorText; };

protected:
	void cgc_HandleCFSLogin( void );
	void cgc_HandleCFSDir( void );
	void cgc_HandleCFSRead( void );
	void cgc_HandleCFSWrite( void );
	void cgc_HandleCFSWriteAppend( void );
	void cgc_HandleCFSDel( void );
	void cgc_HandleCFSRename( void );

	
protected:
	typedef enum
	{
		REQUEST_CFS_LOGIN	= 0,
		REQUEST_CFS_DIR		= 1,
		REQUEST_CFS_READ	= 2,
		REQUEST_CFS_WRITE	= 3,
		REQUEST_CFS_WRITE_APPEND = 4,
		REQUEST_CFS_DEL		= 5,
		REQUEST_CFS_RENAME	= 6
	} cgc_eRequestType;

	typedef enum
	{
		RESPONSE_SUCCESS	= 0,
		RESPONSE_LOGIN_FAILED 	= 1,
		RESPONSE_SYSTEM_FAILURE	= 2,
		RESPONSE_INVALID_FILE	= 3,
		RESPONSE_DELETE_FAILED	= 4,
		RESPONSE_RENAME_FAILED 	= 5,
		RESPONSE_TOO_MANY_OPEN_FILES = 6	
	} cgc_eResponseType;

	typedef struct cgc_REQUEST_HEADER
	{
		cgc_uint8_t requestType;
	} __attribute__((packed)) cgc_tRequestHeader;

	typedef struct cgc_RESPONSE_HEADER
	{
		cgc_uint8_t responseType;
		cgc_uint8_t responseCode;
		cgc_uint16_t responseLength;
	} __attribute__((packed)) cgc_tResponseHeader;

	void cgc_SendResponse( cgc_eRequestType, cgc_eResponseType, cgc_uint8_t *pData, cgc_uint16_t dataLen );

private:
	class cgc_CTempFileList
	{
	public:
		cgc_CTempFileList( cgc_CDBFile *pFile )
			: m_pFile( pFile )	
		{

		}

		cgc_CDBFile *m_pFile;
		CUtil::DLL_LINK( cgc_CTempFileList ) m_oLink;
	};

private:
	cgc_CTimeGen *m_pTimeGen;
	cgc_CDBFS *m_pFS;
	cgc_CNetworkComm *m_pComms;

	cgc_uint32_t m_maxFiles;

	bool m_bError;
	CUtil::cgc_String m_sErrorText;
};

#endif // __NETWORK_FS_H__
