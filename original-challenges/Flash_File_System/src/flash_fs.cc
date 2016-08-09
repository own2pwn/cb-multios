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
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
}

#ifdef NULL
#undef NULL
#define NULL (0)
#endif

// #define FLASH_DEBUG_MODE	(1)

#include "flash_fs.h"
	
cgc_CFlashFS::cgc_CFlashFS( )
	: m_pFlash( NULL ), m_fileTableAddress( 0 )
{

}

cgc_CFlashFS::~cgc_CFlashFS( )
{

}

bool cgc_CFlashFS::cgc_Init( cgc_CNORFlash *pFlash )
{
	bool bFlashErased = false;

	if ( !pFlash )
		return (false);

	if ( pFlash->cgc_GetSectorCount() < 16 )
		return (false);

	m_pFlash = pFlash;

	for ( cgc_uint32_t i = 0; i < MAX_FILE_DESCRIPTORS; i++ )
	{
		m_fileDescriptorTable[i].filePos = 0;
		m_fileDescriptorTable[i].hdrID = OBJ_ID_UNUSED_FD;
	}

	// Reset file descriptor count
	m_fileDescriptorCount = 0;

	// Read in file system header
	cgc_tFlashFSHdr oFSHdr;

	if ( m_pFlash->cgc_ReadData( 0, (cgc_uint8_t*)&oFSHdr, sizeof(oFSHdr) ) != sizeof(oFSHdr) )
		return (false);

	// Check version
	if ( oFSHdr.version == 0xFF )
	{
		// Erased device
		oFSHdr.version = FLASH_FS_VERSION;
		oFSHdr.fileTableBlockIdx = 0;	// Default position
		oFSHdr.fileTableBlockOffset = sizeof(oFSHdr);

		// Write it to flash
		if ( m_pFlash->cgc_WriteData( 0, (cgc_uint8_t*)&oFSHdr, sizeof(oFSHdr) ) != sizeof(oFSHdr) )
		{
			// Write error
			return (false);
		}

		bFlashErased = true;
	}
	
	cgc_memcpy( (void *)&m_fsHdr, (void *)&oFSHdr, sizeof(oFSHdr) );

	// Get the file table address
	m_fileTableAddress = (oFSHdr.fileTableBlockIdx * m_pFlash->cgc_GetBlockSize()) + oFSHdr.fileTableBlockOffset;

	// Sector 1 -- file table
	// Sector 2, 3, and 4 -- object table
	// Sector 5 -> (end-4) -- file data
	// Sector end-4 -> end -- scratch area

	// Find maximum file table size and header table size
	m_fileTableIDMax = ((m_pFlash->cgc_GetBlockSize() * m_pFlash->cgc_GetBlocksPerSector()) - m_fileTableAddress) / sizeof(cgc_tFlashFileTableObj);

	// Find maximum object header ID
	m_objHeaderIDMax = ((m_pFlash->cgc_GetBlockSize() * m_pFlash->cgc_GetBlocksPerSector() * 3) / sizeof(cgc_tFlashFileHdr));

	// Scratch sector setup -- the last 4 sectors are used for scratch
	m_scratchSectorCur = m_pFlash->cgc_GetSectorCount() - 4;

	// Calculate the maximum block ID for data blocks
	m_maxDataBlockID = (m_pFlash->cgc_GetBlocksPerSector() * (m_pFlash->cgc_GetSectorCount() - 4));

	// Clear scratch
	m_pFlash->cgc_SectorErase( m_scratchSectorCur );

	// Load files from file table
	m_fileTableCount = 0;
	m_fileTableIDNext = 0;

	// Remember last object header ID
	m_objHeaderIDNext = 0;

	// Clean file table
	for ( cgc_uint32_t i = 0; i < MAX_FILES; i++ )
	{
		m_fileTable[i].fileTableID = 0xFFFF;
		m_fileTable[i].hdrID = 0xFFFF;
	}

	cgc_uint32_t curAddress = m_fileTableAddress;	
	while ( true )
	{
		// Read first entry in file table
		cgc_tFlashFileTableObj oFileTableObj;

		if ( m_pFlash->cgc_ReadData( curAddress, (cgc_uint8_t*)&oFileTableObj, sizeof(oFileTableObj) ) != sizeof(oFileTableObj) )
		{
			// Read error
			return (false);
		}	

		if ( oFileTableObj.objBits == OBJ_NOT_INIT )
			break;

		if ( oFileTableObj.objBits == OBJ_ERASE )
			;	// Ignore
		else if ( oFileTableObj.objBits == OBJ_INUSE )
		{
			m_fileTable[m_fileTableCount].hdrID = oFileTableObj.objID;
			m_fileTable[m_fileTableCount].fileTableID = m_fileTableIDNext;

			m_fileTableCount++;

			if ( oFileTableObj.objID > m_objHeaderIDNext )
				m_objHeaderIDNext = oFileTableObj.objID;

			if ( m_fileTableCount >= MAX_FILES )
				return (false);
		}
		else
			return (false); // Unknown

		m_fileTableIDNext++;
	}

	if ( bFlashErased )
		m_lastDataBlockID = 0;
	else
	{
		// Find a new data block ID
		cgc_uint16_t dataBlockID = 0;
		do
		{
			// Check for overrun
			if ( dataBlockID >= m_maxDataBlockID )
			{
				// No free data blocks! FLASH FULL
				return (false);
			}		
				
			cgc_uint32_t address = cgc_GetAddressForDataID( dataBlockID );

			// Read data block
			cgc_uint8_t readBits;
		
			if ( m_pFlash->cgc_ReadData( address, (cgc_uint8_t*)&readBits, 1 ) != 1 )
			{
				// Read error
				return (false);
			}

			if ( readBits == OBJ_NOT_INIT )
				break;

			dataBlockID++;
		} while ( true );

		// Remember last data block ID
		m_lastDataBlockID = dataBlockID;
	}

	// Move to the next object
	m_objHeaderIDNext++;
	
	return (true);
}
	
cgc_int32_t cgc_CFlashFS::cgc_WriteFile( cgc_int32_t fd, cgc_uint8_t *pData, cgc_uint32_t dataLen )
{
	// Find file descriptor
	cgc_tFileDescriptorTable oFDData;

	if ( !cgc_FindFileDescriptorData( fd, &oFDData ) )
	{
		// FD not found
		return (FD_NOT_FOUND);
	}

	// Write to file
	cgc_uint16_t hdrID = oFDData.hdrID;

	cgc_tFlashFileHdr oHdrData;

	cgc_uint32_t hdrAddress = cgc_GetAddressForHeaderID( hdrID );

	if ( m_pFlash->cgc_ReadData( hdrAddress, (cgc_uint8_t*)&oHdrData, sizeof(oHdrData) ) != sizeof(oHdrData) )
	{
		// Read failure
		return (-1);
	}

	// Find proper position to write to
	cgc_uint16_t dataID = oHdrData.dataID;

	cgc_uint32_t filePos = oFDData.filePos;

	if ( filePos > oHdrData.objSize )
	{
		// Error
#if FLASH_DEBUG_MODE
		cgc_printf( "FS ERR: File position past file size\n" );
#endif
		filePos = oHdrData.objSize;
	}

	// Remember file size
	cgc_uint32_t fileSize = oHdrData.objSize;

	// Find block to write to and offset of block to write to (starting)
	cgc_uint32_t movePos = 0;
	cgc_uint32_t baseBlockSize = (m_pFlash->cgc_GetBlockSize() - sizeof(cgc_tFlashDataHdr));

	cgc_uint16_t prevBlockID = 0xFFFF;
	cgc_uint16_t startBlockID = dataID;
	cgc_uint16_t curBlockID = dataID;
	cgc_uint16_t curBlockOffset = 0;

	while ( movePos < filePos )
	{
		cgc_uint32_t moveRemaining = (filePos - movePos);

		cgc_uint32_t maxMove = 0;
		curBlockOffset = (sizeof(cgc_tFlashDataHdr));
		if ( movePos == 0 )
		{
			curBlockOffset += oHdrData.fileNameLen;
			maxMove = (baseBlockSize - oHdrData.fileNameLen);
		}
		else
		{
			//curBlockOffset = (sizeof(cgc_tFlashDataHdr));
			maxMove = baseBlockSize;
		}

		if ( maxMove > moveRemaining )
		{
			// Record starting offset
			curBlockOffset += (moveRemaining);
			break;
		} 
	
		// Move to next block
		cgc_tFlashDataHdr oDataHdr;

		cgc_uint32_t dataAddress = cgc_GetAddressForDataID( curBlockID ); 
		if ( m_pFlash->cgc_ReadData( dataAddress, (cgc_uint8_t*)&oDataHdr, sizeof(oDataHdr) ) != sizeof(oDataHdr) )
		{
			// Errror
#if FLASH_DEBUG_MODE
			cgc_printf( "FS ERR: Read error\n" );
#endif
			return -1;
		}

		if ( oDataHdr.objBits != OBJ_INUSE )
		{
#if FLASH_DEBUG_MODE
			cgc_printf( "FS ERR: Data block should be in use\n" );
#endif
			return -1;
		}

		prevBlockID = curBlockID;
		curBlockID = oDataHdr.nextDataID;	
		curBlockOffset = (sizeof(cgc_tFlashDataHdr));

		movePos += maxMove;
	}

	// Did not move -- so set block offset
	if ( filePos == 0 )
		curBlockOffset = (sizeof(cgc_tFlashDataHdr) + oHdrData.fileNameLen);

	// Begin writing to file
	cgc_uint32_t dataToWrite = dataLen;	
	cgc_uint32_t dataPos = 0;
	
	while ( dataToWrite > 0 )
	{	
		cgc_uint32_t dataWritten = 0;
		cgc_uint32_t dataToWriteInBlock = dataToWrite;
		cgc_uint16_t nextBlockID = 0xFFFF;

		if ( curBlockOffset+dataToWriteInBlock > m_pFlash->cgc_GetBlockSize() )
			dataToWriteInBlock = (m_pFlash->cgc_GetBlockSize()-curBlockOffset);

		// Do we need to overwrite this data block???
		if ( filePos < fileSize )
		{
			// Use a slow block erase for overwrites to preserve linked list indexes
			cgc_uint32_t blockSize = m_pFlash->cgc_GetBlockSize();

			cgc_uint32_t blockAddress = cgc_GetAddressForDataID( curBlockID );

			cgc_tFlashDataHdr oPrevDataHdr;
			cgc_tFlashDataHdr oNewDataHdr;

			if ( m_pFlash->cgc_ReadData( blockAddress, (cgc_uint8_t*)&oPrevDataHdr, sizeof(oPrevDataHdr) ) != sizeof(oPrevDataHdr) )
			{
				// Read error
				return (-1);
			}

			// Remember next block ID
			nextBlockID = oPrevDataHdr.nextDataID;

			oNewDataHdr.objBits = OBJ_INUSE;
			oNewDataHdr.prevDataID = oPrevDataHdr.prevDataID;
			oNewDataHdr.nextDataID = oPrevDataHdr.nextDataID;

			// Read in temp data (up to where we start overwriting it)
			if ( m_pFlash->cgc_ReadData( (blockAddress+sizeof(oPrevDataHdr)), m_tempBlock, curBlockOffset-sizeof(oPrevDataHdr)) != (curBlockOffset-sizeof(oPrevDataHdr)) )
			{
				// Read error
				return (-1);
			}

			// Add in new data to write
			cgc_memcpy( m_tempBlock+(curBlockOffset-sizeof(oPrevDataHdr)), pData+dataPos, dataToWriteInBlock );

			// Is there any data remaining in the block that wasn't written
			if ( (curBlockOffset+dataToWriteInBlock) < blockSize )
			{
				if ( m_pFlash->cgc_ReadData( blockAddress+curBlockOffset+dataToWriteInBlock, m_tempBlock+(curBlockOffset-sizeof(oPrevDataHdr))+dataToWriteInBlock, blockSize-(curBlockOffset+dataToWriteInBlock) ) != (blockSize - (curBlockOffset+dataToWriteInBlock)) )
				{
					// Read error
					return (-1);
				}
			}

			cgc_uint32_t blockToEraseID = (m_pFlash->cgc_GetBlocksPerSector() * 4) + curBlockID;
			m_pFlash->cgc_BlockErase( blockToEraseID ); 
			
			// Write new block
			if ( m_pFlash->cgc_WriteData( blockAddress, (cgc_uint8_t*)&oNewDataHdr, sizeof(oNewDataHdr) ) != sizeof(oNewDataHdr) )
			{
				// Write error
				return (-1);
			}

			if ( m_pFlash->cgc_WriteData( (blockAddress+sizeof(oNewDataHdr)), m_tempBlock, blockSize-sizeof(oNewDataHdr) ) != (blockSize-sizeof(oNewDataHdr)) )
			{
				// Write error
				return (-1);
			}
		}
		else
		{
			// Currently appending new data -- allocate block as necessary
			if ( curBlockID == 0xFFFF )
			{
				// Allocate
				curBlockID = cgc_GetNextDataBlockID();	

				cgc_tFlashDataHdr oNewBlockHdr;
				oNewBlockHdr.objBits = OBJ_INUSE;
				oNewBlockHdr.nextDataID = 0xFFFF;
				oNewBlockHdr.prevDataID = prevBlockID;

				// Remember next ID
				nextBlockID = 0xFFFF;

				cgc_uint32_t newBlockAddress = cgc_GetAddressForDataID( curBlockID );

				if ( m_pFlash->cgc_WriteData( newBlockAddress, (cgc_uint8_t*)&oNewBlockHdr, sizeof(oNewBlockHdr) ) != sizeof(oNewBlockHdr) )
				{
					// Read error
					return (-1);
				}

				// Write old
				cgc_tFlashDataHdr oPrevBlockHdr;
				
				cgc_uint32_t prevBlockAddress = cgc_GetAddressForDataID( prevBlockID );
				
				if ( m_pFlash->cgc_ReadData( prevBlockAddress, (cgc_uint8_t*)&oPrevBlockHdr, sizeof(oPrevBlockHdr) ) != sizeof(oPrevBlockHdr) )
				{
					// Read error
					return (-1);
				}

				if ( oPrevBlockHdr.nextDataID != 0xFFFF )
				{
#if FLASH_DEBUG_MODE
					cgc_printf( "FS ERR: Previous data block ID should be 0xFFFF\n" );
#endif
					return (-1);
				}

				oPrevBlockHdr.nextDataID = curBlockID;
	
				if ( m_pFlash->cgc_WriteData( prevBlockAddress, (cgc_uint8_t*)&oPrevBlockHdr, sizeof(oPrevBlockHdr) ) != sizeof(oPrevBlockHdr) )
				{
					// Write error
					return (-1);
				}		
			}
			else
			{
				cgc_uint32_t curBlockAddress = cgc_GetAddressForDataID( curBlockID );

				cgc_tFlashDataHdr oCurBlockHdr;
				if ( m_pFlash->cgc_ReadData( curBlockAddress, (cgc_uint8_t*)&oCurBlockHdr, sizeof(oCurBlockHdr) ) != sizeof(oCurBlockHdr) )
				{
					// Read error
					return (-1);
				}

				// Remember next block ID
				nextBlockID = oCurBlockHdr.nextDataID;
			}
		
			// Write data
			cgc_uint32_t curBlockAddress = cgc_GetAddressForDataID( curBlockID );

			if ( m_pFlash->cgc_WriteData( curBlockAddress+curBlockOffset, pData+dataPos, dataToWriteInBlock ) != dataToWriteInBlock )
			{
				// Write error
				return (-1);
			}			
		}

		prevBlockID = curBlockID;

		curBlockID = nextBlockID;

		curBlockOffset = sizeof(cgc_tFlashDataHdr);

		// Update file position
		filePos += dataToWriteInBlock;

		// Update data position
		dataPos += dataToWriteInBlock;

		// Reduce amount of data left to write
		dataToWrite -= dataToWriteInBlock;	
	}

	// Check if file size changed
	if ( filePos > fileSize )
	{
		// Update file header
		cgc_tFlashFileHdr oNewFileHdr;

		// Create new header -- updating file size
		oNewFileHdr.objBits = OBJ_INUSE;
		oNewFileHdr.dataID = oHdrData.dataID;
		oNewFileHdr.objSize = filePos;
		oNewFileHdr.fileNameLen = oHdrData.fileNameLen;
	
		// Write old header -- as erased
		oHdrData.objBits = OBJ_ERASE;
		
		if ( m_pFlash->cgc_WriteData( hdrAddress, (cgc_uint8_t*)&oHdrData, sizeof(oHdrData) ) != sizeof(oHdrData) )
		{
			// Write failure
			return (-1);
		}

		// Allocate new header
		cgc_uint16_t newHdrID = cgc_GetNewObjHeaderID();

		// Update file table
		cgc_uint32_t i;
		for ( i = 0; i < MAX_FILES; i++ )
		{
			if ( m_fileTable[i].hdrID == hdrID )
			{
				// Update file table entry to point to new header
				cgc_uint16_t newFileTableID = cgc_GetNewFileTableID();

				cgc_uint32_t oldFileTableAddress = cgc_GetAddressForFileTableID( m_fileTable[i].fileTableID );
				
				cgc_uint8_t tempByte = OBJ_ERASE;	
				if ( m_pFlash->cgc_WriteData( oldFileTableAddress, &tempByte, sizeof(tempByte) ) != sizeof(tempByte) )
				{
					// Write error
					return (-1);
				}

				m_fileTable[i].hdrID = newHdrID; 
				m_fileTable[i].fileTableID = newFileTableID;
				break;
			}
		}

		if ( i == MAX_FILES )
		{
#if FLASH_DEBUG_MODE
			cgc_printf( "FS ERR: File table entry not found when attempting update on file table\n" );
#endif
			return (-1);
		}

		// Now update the FD table (multiple FDs might be pointing to this file)
		for ( i = 0; i < MAX_FILE_DESCRIPTORS; i++ )
		{
			if ( m_fileDescriptorTable[i].hdrID == hdrID )
				m_fileDescriptorTable[i].hdrID = newHdrID;
		}

		// Lastly write the new file table block
		cgc_uint32_t newHdrAddress = cgc_GetAddressForHeaderID( newHdrID );

		if ( m_pFlash->cgc_WriteData( newHdrAddress, (cgc_uint8_t*)&oNewFileHdr, sizeof(oNewFileHdr) ) != sizeof(oNewFileHdr) )
		{
			// Write error
			return (-1);
		}
	}

	// Update file descriptor's file position
	m_fileDescriptorTable[fd].filePos = filePos;

	// Return amount written
	return (dataPos);
}

cgc_int32_t cgc_CFlashFS::cgc_ReadFile( cgc_int32_t fd, cgc_uint8_t *pData, cgc_uint32_t dataLen )
{
	// Find file descriptor
	cgc_tFileDescriptorTable oFDData;

	if ( !cgc_FindFileDescriptorData( fd, &oFDData ) )
	{
		// FD not found
		return (FD_NOT_FOUND);
	}

	// Write to file
	cgc_uint16_t hdrID = oFDData.hdrID;

	cgc_tFlashFileHdr oHdrData;

	cgc_uint32_t hdrAddress = cgc_GetAddressForHeaderID( hdrID );

	if ( m_pFlash->cgc_ReadData( hdrAddress, (cgc_uint8_t*)&oHdrData, sizeof(oHdrData) ) != sizeof(oHdrData) )
	{
		// Read failure
		return (-1);
	}

	// Find proper position to write to
	cgc_uint16_t dataID = oHdrData.dataID;

	cgc_uint32_t filePos = oFDData.filePos;

	if ( filePos > oHdrData.objSize )
	{
		// Error
#if FLASH_DEBUG_MODE
		cgc_printf( "FS ERR: File position past file size\n" );
#endif
		filePos = oHdrData.objSize;

		return (-1);
	}

	// Only read the amount remaining
	if ( filePos+dataLen > oHdrData.objSize )
		dataLen = oHdrData.objSize - filePos;

	// IF we don't have anything to read -- just return 0
	if ( dataLen == 0 )
		return (0);

	// Remember file size
	cgc_uint32_t fileSize = oHdrData.objSize;

	// Find block to write to and offset of block to write to (starting)
	cgc_uint32_t movePos = 0;
	cgc_uint32_t baseBlockSize = (m_pFlash->cgc_GetBlockSize() - sizeof(cgc_tFlashDataHdr));

	cgc_uint16_t prevBlockID = 0xFFFF;
	cgc_uint16_t startBlockID = dataID;
	cgc_uint16_t curBlockID = dataID;
	cgc_uint16_t curBlockOffset = 0;

	while ( movePos < filePos )
	{
		cgc_uint32_t moveRemaining = (filePos - movePos);

		cgc_uint32_t maxMove = 0;
		curBlockOffset = sizeof(cgc_tFlashDataHdr);
		if ( movePos == 0 )
		{
			curBlockOffset += oHdrData.fileNameLen;
			maxMove = (baseBlockSize - oHdrData.fileNameLen);
		}
		else
		{
			maxMove = baseBlockSize;
		}

		if ( maxMove > moveRemaining )
		{
			// Record starting offset
			curBlockOffset += (moveRemaining);
			break;
		} 
	
		// Move to next block
		cgc_tFlashDataHdr oDataHdr;

		cgc_uint32_t dataAddress = cgc_GetAddressForDataID( curBlockID ); 
		if ( m_pFlash->cgc_ReadData( dataAddress, (cgc_uint8_t*)&oDataHdr, sizeof(oDataHdr) ) != sizeof(oDataHdr) )
		{
			// Errror
#if FLASH_DEBUG_MODE
			cgc_printf( "FS ERR: Read error\n" );
#endif
			return -1;
		}

		if ( oDataHdr.objBits != OBJ_INUSE )
		{
#if FLASH_DEBUG_MODE
			cgc_printf( "FS ERR: Data block should be in use\n" );
#endif
			return -1;
		}

		curBlockOffset = (sizeof(cgc_tFlashDataHdr));
		
		prevBlockID = curBlockID;
		curBlockID = oDataHdr.nextDataID;	

		movePos += maxMove;
	}

	// Did not move -- so set block offset
	if ( filePos == 0 )
		curBlockOffset = (sizeof(cgc_tFlashDataHdr) + oHdrData.fileNameLen);

	// Begin reading from file
	cgc_uint32_t dataToRead = dataLen;	
	cgc_uint32_t dataPos = 0;
	
	while ( dataToRead > 0 )
	{	
		cgc_uint32_t dataToReadInBlock = dataToRead;

		if ( curBlockOffset+dataToReadInBlock > m_pFlash->cgc_GetBlockSize() )
			dataToReadInBlock = (m_pFlash->cgc_GetBlockSize()-curBlockOffset);

		if ( curBlockID == 0xFFFF )
		{
#if FLASH_DEBUG_MODE
			cgc_printf( "FS ERR: Read past inuse blocks\n" );
#endif
			return -1;
		}

		cgc_uint32_t blockAddress = cgc_GetAddressForDataID( curBlockID );

		cgc_tFlashDataHdr oDataHdr;

		// Read block
		if ( m_pFlash->cgc_ReadData( blockAddress, (cgc_uint8_t*)&oDataHdr, sizeof(oDataHdr) ) != sizeof(oDataHdr) )
		{
			// Read error
			return (-1);
		}

		// Validate
		if ( oDataHdr.objBits != OBJ_INUSE )
		{
#if FLASH_DEBUG_MODE
			cgc_printf( "FS ERR: Read block should be in use\n" );
#endif
			return -1;
		}

		// Read
		if ( m_pFlash->cgc_ReadData( blockAddress+curBlockOffset, pData+dataPos, dataToReadInBlock ) != dataToReadInBlock )
		{
			// Read errror
			return (-1);
		}
		
		curBlockID = oDataHdr.nextDataID;
	
		curBlockOffset = sizeof(cgc_tFlashDataHdr);
	
		dataToRead -= dataToReadInBlock;
		dataPos += dataToReadInBlock;

		// Update file position
		filePos += dataToReadInBlock;
	}

	// Read doesn't change any data in flash
	m_fileDescriptorTable[fd].filePos = filePos;

	return (dataPos);
}

cgc_int32_t cgc_CFlashFS::cgc_DeleteFile( const char *pszFilename )
{
	cgc_uint16_t hdrID;
	cgc_uint32_t objSize;

	// Not in recent file list -- try looking in file table
	if ( !cgc_FindFileInFlashTable( pszFilename, &hdrID, &objSize ) )
	{
		return (-1);
	}

	cgc_tFlashFileHdr oHdrData;

	cgc_uint32_t hdrAddress = cgc_GetAddressForHeaderID( hdrID );

	if ( m_pFlash->cgc_ReadData( hdrAddress, (cgc_uint8_t*)&oHdrData, sizeof(oHdrData) ) != sizeof(oHdrData) )
	{
		// Read failure
		return (-1);
	}

	cgc_uint16_t curBlockID = oHdrData.dataID;
	cgc_uint16_t nextBlockID = 0xFFFF;

	do
	{
		// Move to next block
		cgc_tFlashDataHdr oDataHdr;

		cgc_uint32_t dataAddress = cgc_GetAddressForDataID( curBlockID ); 
		if ( m_pFlash->cgc_ReadData( dataAddress, (cgc_uint8_t*)&oDataHdr, sizeof(oDataHdr) ) != sizeof(oDataHdr) )
		{
			// Errror
#if FLASH_DEBUG_MODE
			cgc_printf( "FS ERR: Delete error\n" );
#endif
			return -1;
		}

		if ( oDataHdr.objBits != OBJ_INUSE )
		{
#if FLASH_DEBUG_MODE
			cgc_printf( "FS ERR: Data block should be in use, while deleting file\n" );
#endif
			return -1;
		}

		// Mark it erased
		oDataHdr.objBits = OBJ_ERASE;

		// Write
		if ( m_pFlash->cgc_WriteData( dataAddress, (cgc_uint8_t*)&oDataHdr, sizeof(oDataHdr) ) != sizeof(oDataHdr) )
		{
			// Write error
			return (-1);
		}

		curBlockID = oDataHdr.nextDataID;	

		// Check for last block
		if ( curBlockID == 0xFFFF )
			break;

	} while ( true );

	// Delete file header
	oHdrData.objBits = OBJ_ERASE;

	// Write header
	if ( m_pFlash->cgc_WriteData( hdrAddress, (cgc_uint8_t*)&oHdrData, sizeof(oHdrData) ) != sizeof(oHdrData) )
	{
		// Write error
		return (-1);
	}

	// Delete file table entry
	cgc_uint16_t fileTableID = 0xFFFF;
	for ( cgc_uint32_t i = 0; i < MAX_FILES; i++ )
	{
		if ( m_fileTable[i].hdrID == hdrID )
		{
			m_fileTable[i].hdrID = 0xFFFF;
			fileTableID = m_fileTable[i].fileTableID;

			m_fileTable[i].fileTableID = 0xFFFF;

			m_fileTableCount--;
			break;
		}
	}

	if ( fileTableID == 0xFFFF )
	{
#if FLASH_DEBUG_MODE
			cgc_printf( "FS ERR: File table entry not found, while deleting file\n" );
#endif
			return -1;
	}

	// Write file table entry
	cgc_uint32_t fileTableAddress = cgc_GetAddressForFileTableID( fileTableID );

	cgc_uint8_t tempByte = OBJ_ERASE;
	if ( m_pFlash->cgc_WriteData( fileTableAddress, &tempByte, sizeof(tempByte) ) != sizeof(tempByte) )
	{
		// Write error
		return (-1);
	}

	// Lastly remove all file descriptors
	for ( cgc_uint32_t i = 0; i < MAX_FILE_DESCRIPTORS; i++ )
	{
		if ( m_fileDescriptorTable[i].hdrID == hdrID )
		{
			m_fileDescriptorTable[i].hdrID = OBJ_ID_UNUSED_FD;
			m_fileDescriptorTable[i].filePos = 0;

			m_fileDescriptorCount--;
		}
	}

	return (0);
}

cgc_int32_t cgc_CFlashFS::cgc_OpenFile( const char *pszFilename, cgc_uint8_t openAttributes )
{
	bool bFileFound = false;

	cgc_uint16_t objID;
	cgc_uint32_t objSize;

	if ( m_fileDescriptorCount >= MAX_FILE_DESCRIPTORS )
	{
		// Max FDs
		return (OPEN_FILE_MAX_FD);
	}

	// Not in recent file list -- try looking in file table
	if ( !cgc_FindFileInFlashTable( pszFilename, &objID, &objSize ) )
	{
		// No file at all
		if ( openAttributes != OPEN_MODE_WRITE )
		{
			// Fail -- not opened writeable
			return (OPEN_FILE_NOT_FOUND);
		}

		// No file present -- create it
		if ( !cgc_CreateNewFile( pszFilename, &objID, &objSize ) )
		{
			// Fail -- out of free blocks
			return (OPEN_FILE_OUT_OF_SPACE);
		}
	}
	
	cgc_int32_t newFD = cgc_GetNewFileDescriptor();

	if ( newFD == -1 )
	{
		// File already opened
		return (OPEN_FILE_ALREADY_OPEN);
	}

	// File found or file created for write mode
	m_fileDescriptorTable[newFD].hdrID = objID;

	if ( openAttributes == OPEN_MODE_APPEND )
		m_fileDescriptorTable[newFD].filePos = objSize;
	else
		m_fileDescriptorTable[newFD].filePos = 0;

	return (newFD);
}

cgc_int32_t cgc_CFlashFS::cgc_CloseFile( cgc_int32_t fd )
{
	// Bug here
#if PATCHED_1
	if ( fd < 0 )
		return (CLOSE_BAD_FD);
#endif

	if ( fd >= MAX_FILE_DESCRIPTORS )
		return (CLOSE_BAD_FD);

	if ( m_fileDescriptorTable[fd].hdrID == OBJ_ID_UNUSED_FD )
		return (CLOSE_BAD_FD);

	m_fileDescriptorTable[fd].hdrID = OBJ_ID_UNUSED_FD;
	m_fileDescriptorTable[fd].filePos = 0;

	m_fileDescriptorCount--;

	return (fd);
}

bool cgc_CFlashFS::cgc_FindFileDescriptorData( cgc_int16_t fd, cgc_tFileDescriptorTable *pFDData )
{
	if ( fd < 0 || fd >= MAX_FILE_DESCRIPTORS )
		return (false);

	if ( m_fileDescriptorTable[fd].hdrID == OBJ_ID_UNUSED_FD )
		return (false);

	pFDData->hdrID = m_fileDescriptorTable[fd].hdrID;
	pFDData->filePos = m_fileDescriptorTable[fd].filePos;

	return (true);
}

bool cgc_CFlashFS::cgc_FindFileInFlashTable( const char *pszFilename, cgc_uint16_t *pObjID, cgc_uint32_t *pObjSize )
{
	char szFilenameTemp[256];
	
	cgc_uint8_t testFileLen = cgc_strlen( pszFilename );

	// Read first file in file table
	// Loop through file descriptor table
	cgc_uint32_t fileTableHitCount = 0;
	for ( cgc_uint32_t i = 0; i < MAX_FILES; i++ )	
	{
		if ( fileTableHitCount >= m_fileTableCount )
			break; // Exit early if we need to
		
		if ( m_fileTable[i].hdrID == 0xFFFF )
			continue;

		fileTableHitCount++;		

		cgc_uint32_t fileHeaderAddr = cgc_GetAddressForHeaderID( m_fileTable[i].hdrID );

		cgc_tFlashFileHdr oFileHdr;
		if ( m_pFlash->cgc_ReadData( fileHeaderAddr, (cgc_uint8_t*)&oFileHdr, sizeof(oFileHdr) ) != sizeof(oFileHdr) )
			return (false);

		if ( testFileLen != oFileHdr.fileNameLen )
			continue;
 
		// Read file name
		cgc_uint32_t fileNameAddress = cgc_GetAddressForDataID( oFileHdr.dataID ) + sizeof(cgc_tFlashDataHdr);

		if ( m_pFlash->cgc_ReadData( fileNameAddress, (cgc_uint8_t*)szFilenameTemp, oFileHdr.fileNameLen ) != oFileHdr.fileNameLen )
			return (false);

		szFilenameTemp[oFileHdr.fileNameLen] = '\0';

		if ( cgc_strcmp( pszFilename, szFilenameTemp ) == 0 )
		{
			(*pObjID) = m_fileTable[i].hdrID;
			(*pObjSize) = oFileHdr.objSize;

			return (true);
		}

	}

	return (false);	
}

cgc_uint32_t cgc_CFlashFS::cgc_GetFileTableAddress( void )
{
	return (m_fileTableAddress);
}

bool cgc_CFlashFS::cgc_CreateNewFile( const char *pszFilename, cgc_uint16_t *pObjID, cgc_uint32_t *pObjSize )
{
	if ( m_fileTableCount >= MAX_FILES )
		return (false);

	cgc_uint16_t objHeaderID = cgc_GetNewObjHeaderID();

	if ( objHeaderID == 0xFFFF )
	{
		// Clean object header table
		cgc_CleanObjHeaderTable();
	
		objHeaderID = cgc_GetNewObjHeaderID();
	}

	if ( objHeaderID == 0xFFFF )
		return (false);	// Critical error

	// Get a new file table header
	cgc_uint16_t fileTableID = cgc_GetNewFileTableID( );

	if ( fileTableID == 0xFFFF )
	{
		// Clean file table
		cgc_CleanFileTable();
	
		fileTableID = cgc_GetNewFileTableID( );
	}	

	if ( fileTableID == 0xFFFF )
		return (false);	// Critical error -- out of file table indexes

	if ( !cgc_CreateNewObject( pszFilename, fileTableID, objHeaderID ) )
		return (false); // Critical error -- out of space?

	(*pObjID) = objHeaderID;
	(*pObjSize) = 0;			

	return (true);
}

cgc_uint32_t cgc_CFlashFS::cgc_GetAddressForHeaderID( cgc_uint16_t objID )
{
	return ((m_pFlash->cgc_GetBlockSize() * m_pFlash->cgc_GetBlocksPerSector()) + (sizeof(cgc_tFlashFileHdr) * objID));	
}

cgc_uint32_t cgc_CFlashFS::cgc_GetAddressForFileTableID( cgc_uint16_t objID )
{
	return (m_fileTableAddress + (sizeof(cgc_tFlashFileTableObj) * objID));
}

cgc_uint32_t cgc_CFlashFS::cgc_GetAddressForDataID( cgc_uint16_t objID )
{
	return ((cgc_GetAddressForSector( 4 )) + (m_pFlash->cgc_GetBlockSize() * objID));
}

cgc_uint32_t cgc_CFlashFS::cgc_GetAddressForSector( cgc_uint16_t sectorNum )
{
	return (m_pFlash->cgc_GetBlockSize() * m_pFlash->cgc_GetBlocksPerSector() * sectorNum);
}

cgc_uint16_t cgc_CFlashFS::cgc_GetNewFileTableID( void )
{
	if ( m_fileTableCount >= MAX_FILES )
		return (0xFFFF);
	
	cgc_uint16_t newFileTableID = m_fileTableIDNext;
	do
	{
		if ( newFileTableID >= MAX_FILES )
			newFileTableID = 0;

		if ( m_fileTable[newFileTableID].hdrID == 0xFFFF )
			break;

		newFileTableID++;
	} while ( true );

	if ( m_fileTableIDNext < MAX_FILES )
		m_fileTableIDNext++;

	return (newFileTableID);
}

cgc_uint16_t cgc_CFlashFS::cgc_GetNewObjHeaderID( void )
{
	if ( m_objHeaderIDNext >= m_objHeaderIDMax )	
		return (0xFFFF);

	cgc_uint16_t newObjHeaderID = m_objHeaderIDNext++;

	return (newObjHeaderID);
}

bool cgc_CFlashFS::cgc_CleanFileTable( void )
{
	// Easy cleanup -- just rewrite the file table after erasing sector 1
	m_pFlash->cgc_SectorErase( 0 );
	
	// Rewrite
	cgc_tFlashFSHdr oFSHdr;
	
	oFSHdr.version = FLASH_FS_VERSION;
	oFSHdr.fileTableBlockIdx = 0;	// Default position
	oFSHdr.fileTableBlockOffset = sizeof(oFSHdr);

	// Write it to flash
	if ( m_pFlash->cgc_WriteData( 0, (cgc_uint8_t*)&oFSHdr, sizeof(oFSHdr) ) != sizeof(oFSHdr) )
	{
		// Write error
		return (false);
	}

	cgc_uint32_t curAddress = cgc_GetFileTableAddress();
	cgc_uint16_t tableIDCur = 0;
	for ( cgc_uint32_t i = 0; i < m_fileTableCount; i++ )
	{
		do
		{
			if ( tableIDCur >= MAX_FILES )
			{
				// Error
				return (false);
			}

			if ( m_fileTable[tableIDCur].hdrID != 0xFFFF )
			{
				m_fileTable[i].hdrID = m_fileTable[tableIDCur].hdrID;
				m_fileTable[i].fileTableID = i;
				break;
			}
			
			tableIDCur++;

		} while ( true );

		cgc_tFlashFileTableObj oFileTableObj;

		oFileTableObj.objBits = OBJ_INUSE;
		oFileTableObj.objID = m_fileTable[tableIDCur].hdrID;

		if ( m_pFlash->cgc_WriteData( curAddress, (cgc_uint8_t*)&oFileTableObj, sizeof(oFileTableObj) ) != sizeof(oFileTableObj) )
		{
			// Write error
			return (false);
		}

		curAddress += sizeof(oFileTableObj);
	}

	// Reset file table
	m_fileTableIDNext = m_fileTableCount;

	for ( tableIDCur = m_fileTableIDNext; tableIDCur < MAX_FILES; tableIDCur++ )
	{
		m_fileTable[tableIDCur].hdrID = 0xFFFF;
		m_fileTable[tableIDCur].fileTableID = 0xFFFF;
	}

	return (true);
}

bool cgc_CFlashFS::cgc_CleanObjHeaderTable( void )
{
	// Object header table is in sectors 1-3, there can be up to MAX_FILES object headers
	// Copy into the scratch sector	

	bool bDone = false;
	cgc_uint32_t fileTablePos = 0;

	cgc_uint32_t scratchStartSector = 0;
	cgc_uint32_t scratchEndSector = 0;
	cgc_uint32_t fileHeaderCount = 0;
	do
	{
		cgc_uint32_t scratchCurAddress = cgc_GetAddressForSector( m_scratchSectorCur );
		cgc_uint32_t scratchEndAddress = cgc_GetAddressForSector( m_scratchSectorCur+1 );

		cgc_uint32_t i = 0;

		if ( scratchStartSector == 0 )
			scratchStartSector = m_scratchSectorCur;

		scratchEndSector = m_scratchSectorCur+1;

		// Go through the file table and move it to the scratch sector
		for ( i = fileTablePos; fileHeaderCount < m_fileTableCount; i++ )
		{
			if ( m_fileTable[i].hdrID == 0xFFFF )
				continue;

			if ( i >= MAX_FILES )
			{
#if FLASH_DEBUG_MODE
				cgc_printf( "FS ERR: Cleaning object header table -- wrap-around (should not happen)\n" );
#endif
				return (false);
			}

			// Copy data
			cgc_tFlashFileHdr oTemp;

			if ( scratchCurAddress+sizeof(oTemp) > scratchEndAddress )
				break;

			cgc_uint32_t fileTableHeaderAddr = cgc_GetAddressForFileTableID( m_fileTable[i].hdrID );

			if ( m_pFlash->cgc_ReadData( fileTableHeaderAddr, (cgc_uint8_t*)&oTemp, sizeof(oTemp) ) != sizeof(oTemp) )
			{
				// Read error
				return (false);
			}

			// Write to scratch
			if ( m_pFlash->cgc_WriteData( scratchCurAddress, (cgc_uint8_t*)&oTemp, sizeof(oTemp) ) != sizeof(oTemp) )
			{
				// Write error
				return (false);
			}

			scratchCurAddress += sizeof(oTemp);
	
			fileHeaderCount++;
		}
		
		if ( fileHeaderCount == m_fileTableCount )
			bDone = true;

		fileTablePos = i;

		m_scratchSectorCur = cgc_GetNextScratchSector();
	}	
	while ( !bDone );

	// Erase file header sectors
	m_pFlash->cgc_SectorErase( 1 );
	m_pFlash->cgc_SectorErase( 2 );
	m_pFlash->cgc_SectorErase( 3 );

	// Copy back
	cgc_uint32_t headerCurAddress = cgc_GetAddressForHeaderID( 0 );

	cgc_uint32_t copyPerSector = ((m_pFlash->cgc_GetBlockSize() * m_pFlash->cgc_GetBlocksPerSector()) / sizeof(cgc_tFlashFileHdr)) * sizeof(cgc_tFlashFileHdr);

	fileTablePos = 0;
	fileHeaderCount = 0;
	
	do
	{
		// Copy as much as possible
		cgc_uint32_t copyFromPos = cgc_GetAddressForSector( scratchStartSector );
		cgc_uint32_t copyFromEndPos = copyFromPos + copyPerSector;

		scratchStartSector++;

		// Copy
		cgc_uint32_t i = 0;
		for ( i = fileTablePos; fileHeaderCount < m_fileTableCount; i++ )
		{
			if ( m_fileTable[i].hdrID == 0xFFFF )
				continue;

			if ( i >= MAX_FILES )
			{
#if FLASH_DEBUG_MODE
				cgc_printf( "FS ERR: Cleaning object header table -- wrap-around (should not happen)\n" );
#endif
				return (false);
			}

			cgc_tFlashFileHdr oTemp;

			if ( copyFromPos+sizeof(oTemp) > copyFromEndPos )
				break;

			// From scratch to new position in header table
			if ( m_pFlash->cgc_ReadData( copyFromPos, (cgc_uint8_t*)&oTemp, sizeof(oTemp) ) != sizeof(oTemp) )
			{
				// Read error
				return (false);
			}

			// Write to scratch
			if ( m_pFlash->cgc_WriteData( headerCurAddress, (cgc_uint8_t*)&oTemp, sizeof(oTemp) ) != sizeof(oTemp) )
			{
				// Write error
				return (false);
			}

			// Reset file table
			m_fileTable[i].hdrID = fileHeaderCount;

			fileHeaderCount++;

			copyFromPos += sizeof(oTemp);
			headerCurAddress += sizeof(oTemp);
		}

		fileTablePos = i;
	
		if ( fileHeaderCount == m_fileTableCount )
			break;	

		// Wrap around scratch sector
		if ( scratchStartSector >= m_pFlash->cgc_GetSectorCount() )
			scratchStartSector = m_pFlash->cgc_GetSectorCount() - 4;

	} while ( true );

	// Always clean file table after rewriting object header table
	cgc_CleanFileTable();

	return (true);
}

bool cgc_CFlashFS::cgc_CleanDataBlocks( void )
{
	// Clean up some sectors to use -- normally this would be done
	// progressively over the life of the device (for speed reasons)

	// Find first emtpy 
	cgc_uint32_t curDataID = 0;
	for ( curDataID = 0; curDataID < m_maxDataBlockID; curDataID++ )
	{
		cgc_uint32_t curBlockAddress = cgc_GetAddressForDataID( curDataID );
	}

	cgc_CleanObjHeaderTable();

	// Out of memory	
	return (false);
}

cgc_uint16_t cgc_CFlashFS::cgc_GetNextScratchSector( void )
{
	m_scratchSectorCur++;

	if ( m_scratchSectorCur >= m_pFlash->cgc_GetSectorCount() )
		m_scratchSectorCur = m_pFlash->cgc_GetSectorCount() - 4;

	m_pFlash->cgc_SectorErase( m_scratchSectorCur );

	return (m_scratchSectorCur);
}

bool cgc_CFlashFS::cgc_CreateNewObject( const char *pszObjectName, cgc_uint16_t fileTableID, cgc_uint16_t hdrID )
{
	cgc_uint8_t objNameLen = cgc_strlen(pszObjectName);

	// Find new spot for object
	for ( cgc_uint32_t i = 0; i < MAX_FILES; i++ )
	{
		if ( m_fileTable[i].hdrID == 0xFFFF )
		{
			m_fileTable[i].hdrID = hdrID;
			m_fileTable[i].fileTableID = fileTableID;
	
			m_fileTableCount++;
			break;
		}
	}

	// Get a data block
	cgc_uint16_t dataBlockID = cgc_GetNextDataBlockID();

	// Claim block -- but not used yet
	cgc_tFlashDataHdr oDataHdr;

	oDataHdr.objBits = OBJ_INUSE;
	oDataHdr.prevDataID = 0x0;	// First in block list
	oDataHdr.nextDataID = 0xFFFF;	// Not init'ed yet

	cgc_uint32_t dataAddress = cgc_GetAddressForDataID( dataBlockID );

	if ( m_pFlash->cgc_WriteData( dataAddress, (cgc_uint8_t*)&oDataHdr, sizeof(oDataHdr) ) != sizeof(oDataHdr) )
	{
		// Fail to write data
		return (false);
	}

	// Write name
	if ( m_pFlash->cgc_WriteData( (dataAddress+sizeof(oDataHdr)), (cgc_uint8_t*)pszObjectName, objNameLen ) != objNameLen )
	{
		// Fail to write data
		return (false);
	}
	
	// Write header data
	cgc_tFlashFileHdr oFileHdr;

	oFileHdr.objBits = OBJ_INUSE;
	oFileHdr.dataID = dataBlockID;
	oFileHdr.objSize = 0;
	oFileHdr.fileNameLen = objNameLen;

	// Write header
	cgc_uint32_t hdrAddress = cgc_GetAddressForHeaderID( hdrID );

	if ( m_pFlash->cgc_WriteData( hdrAddress, (cgc_uint8_t*)&oFileHdr, sizeof(oFileHdr) ) != sizeof(oFileHdr) )
	{
		// Fail to write header
		return (false);
	}
	
	return (true);
}

cgc_int32_t cgc_CFlashFS::cgc_GetNewFileDescriptor( void )
{
	if ( m_fileDescriptorCount >= MAX_FILE_DESCRIPTORS )
		return (-1);

	for ( cgc_uint32_t i = 0; i < MAX_FILE_DESCRIPTORS; i++ )
	{
		if ( m_fileDescriptorTable[i].hdrID == OBJ_ID_UNUSED_FD )
		{
			m_fileDescriptorCount++;
			return (i);
		}
	}

	return (-1);
}

cgc_uint16_t cgc_CFlashFS::cgc_GetNextDataBlockID( void )
{
	// Get a new data block
	cgc_uint16_t nextDataBlockID = m_lastDataBlockID++;

	if ( nextDataBlockID >= m_maxDataBlockID )
	{
		// Clean
		cgc_CleanDataBlocks( );
	}
	
	nextDataBlockID = m_lastDataBlockID++;

	return (nextDataBlockID);
}
