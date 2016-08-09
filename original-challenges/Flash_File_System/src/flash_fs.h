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
#ifndef __FLASH_FS_H__
#define __FLASH_FS_H__

#include "nor_flash.h"

#define RECENT_FILE_TABLE_SIZE		(8)

// Adjust this value to change the maximum number of open file descriptors
#define MAX_FILE_DESCRIPTORS		(16)
#define MAX_FILES			(128)

#define OBJ_ID_UNUSED_FD		(0xFFFF)

#define FD_NOT_FOUND		(-1)

#define OPEN_MODE_READ		(1)
#define OPEN_MODE_WRITE		(2)
#define OPEN_MODE_APPEND	(4)

#define OPEN_FILE_NOT_FOUND	(-1)
#define OPEN_FILE_OUT_OF_SPACE	(-2)
#define OPEN_FILE_MAX_FD	(-3)
#define OPEN_FILE_ALREADY_OPEN	(-4)

#define CLOSE_BAD_FD		(-1)

#define FLASH_FS_VERSION	(0xC1)

#define OBJ_NOT_INIT	(0xFF)
#define OBJ_CLAIM	(0x04)
#define OBJ_ERASE	(0x01)
#define OBJ_INUSE	(0x02)

// A flash file system
class cgc_CFlashFS
{
public:
	cgc_CFlashFS( );
	~cgc_CFlashFS( );

	bool cgc_Init( cgc_CNORFlash *pFlash );

	cgc_int32_t cgc_WriteFile( cgc_int32_t fd, cgc_uint8_t *pData, cgc_uint32_t dataLen );
	cgc_int32_t cgc_ReadFile( cgc_int32_t fd, cgc_uint8_t *pData, cgc_uint32_t dataLen );

	cgc_int32_t cgc_DeleteFile( const char *pszFilename );

	cgc_int32_t cgc_OpenFile( const char *pszFilename, cgc_uint8_t openAttributes );
	cgc_int32_t cgc_CloseFile( cgc_int32_t fd );
	

protected:
	// Header for the entire flash FS
	typedef struct cgc_FLASH_FS_HEADER
	{
		cgc_uint8_t version;
		cgc_uint16_t fileTableBlockIdx;
		cgc_uint16_t fileTableBlockOffset;
	} __attribute__((packed)) cgc_tFlashFSHdr;

	typedef struct cgc_FLASH_FILE_TABLE_OBJ
	{
		cgc_uint8_t objBits;	// Current state (INUSE, ERASE, UNINIT)
		cgc_uint16_t objID;		// ID for header block
	} __attribute__((packed)) cgc_tFlashFileTableObj;

	typedef struct cgc_FLASH_FILE_HEADER
	{
		cgc_uint8_t objBits;	// Current state (INUSE, ERASE, UNINIT)
		cgc_uint16_t dataID;	// Same as the block index -- every file is a block on the device
		cgc_uint32_t objSize;	// The size of the object (file size)
		cgc_uint8_t fileNameLen;
	} __attribute__((packed)) cgc_tFlashFileHdr;

	typedef struct cgc_FLASH_DATA_HEADER
	{
		cgc_uint8_t objBits;	// Current state (INUSE, ERASE, UNINIT)
		cgc_uint16_t prevDataID;	// Prev ID for data object
		cgc_uint16_t nextDataID;	// Next ID for data object
	} __attribute__((packed)) cgc_tFlashDataHdr;

	typedef struct FLASH_SECTOR_HEADER
	{
		cgc_uint16_t sectorMask;	
	} __attribute__((packed)) cgc_tFlashSectorHeader;

	typedef struct cgc_FILE_DESCRIPTOR_TABLE
	{
		cgc_uint16_t hdrID;		// Same as a block index -- every file is a block on the device
		cgc_uint32_t filePos;
	} cgc_tFileDescriptorTable;

	typedef struct cgc_FILE_TABLE_STRUCT
	{
		cgc_uint16_t fileTableID;
		cgc_uint16_t hdrID;
	} cgc_tFileTableData;

protected:
	bool cgc_FindFileDescriptorData( cgc_int16_t fd, cgc_tFileDescriptorTable *pFDData );

	bool cgc_FindFileInFlashTable( const char *pszFilename, cgc_uint16_t *pObjID, cgc_uint32_t *pObjSize );

	cgc_uint32_t cgc_GetFileTableAddress( void );

	bool cgc_CreateNewFile( const char *pszFilename, cgc_uint16_t *pObjID, cgc_uint32_t *pObjSize );

	cgc_uint32_t cgc_GetAddressForHeaderID( cgc_uint16_t objID );
	cgc_uint32_t cgc_GetAddressForFileTableID( cgc_uint16_t objID );
	cgc_uint32_t cgc_GetAddressForDataID( cgc_uint16_t objID );
	cgc_uint32_t cgc_GetAddressForSector( cgc_uint16_t sectorNum );

	cgc_uint16_t cgc_GetNewFileTableID( void );
	cgc_uint16_t cgc_GetNewObjHeaderID( void );

	cgc_uint16_t cgc_GetNextDataBlockID( void );

	bool cgc_CleanFileTable( void );
	bool cgc_CleanObjHeaderTable( void );
	bool cgc_CleanDataBlocks( void );

	cgc_uint16_t cgc_GetNextScratchSector( void );

	bool cgc_CreateNewObject( const char *pszObjectName, cgc_uint16_t fileTableID, cgc_uint16_t hdrID );

	cgc_int32_t cgc_GetNewFileDescriptor( void );

private:
	cgc_uint8_t m_tempBlock[512];

	cgc_CNORFlash *m_pFlash;

	// Max file descriptors available
	cgc_tFileDescriptorTable m_fileDescriptorTable[MAX_FILE_DESCRIPTORS];

	cgc_uint16_t m_fileDescriptorCount;

	cgc_tFlashFSHdr m_fsHdr;
	cgc_uint32_t m_fileTableAddress;

	cgc_tFileTableData m_fileTable[MAX_FILES];
	cgc_uint16_t m_fileTableCount;
	cgc_uint16_t m_fileTableIDNext;

	cgc_uint16_t m_fileTableIDMax;
	cgc_uint16_t m_objHeaderIDMax;

	cgc_uint16_t m_objHeaderIDNext;
	cgc_uint16_t m_scratchSectorCur;	// Current scratch sector

	cgc_uint16_t m_lastDataBlockID;
	cgc_uint16_t m_maxDataBlockID;	// Max data block ID
};

#endif // __FLASH_FS_H__
