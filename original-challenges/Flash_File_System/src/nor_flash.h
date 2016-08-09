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
#ifndef __NOR_FLASH_H__
#define __NOR_FLASH_H__

#define MAX_BLOCK_SIZE	(512)
#define MAX_BLOCKS_PER_SECTOR	(16)
#define MAX_SECTOR_COUNT	(256)

#define FLASH_SUCCESS	(0)
#define FLASH_INVALID_BLOCK	(-1)
#define FLASH_INVALID_SECTOR	(-2)
#define FLASH_NO_MEMORY		(-3)
#define FLASH_INVALID_ADDRESS	(-4)

// Class to simulate the behavior of NOR Flash
class cgc_CNORFlash
{
public:
	cgc_CNORFlash( );
	~cgc_CNORFlash( );

	void cgc_Init( cgc_uint32_t blockSize, cgc_uint32_t blocksPerSector, cgc_uint32_t sectorCount );

	cgc_int32_t cgc_WriteData( cgc_uint32_t address, cgc_uint8_t *pData, cgc_uint32_t dataLen );
	cgc_int32_t cgc_ReadData( cgc_uint32_t address, cgc_uint8_t *pData, cgc_uint32_t dataLen );	

	cgc_int32_t cgc_DeviceErase( void );
	cgc_int32_t cgc_SectorErase( cgc_uint32_t sectorNum );
	cgc_int32_t cgc_BlockErase( cgc_uint32_t blockNum );

	cgc_uint32_t cgc_GetBlockSize( void ) const { return m_blockSize; };
	cgc_uint32_t cgc_GetBlocksPerSector( void ) const { return m_blocksPerSector; };
	cgc_uint32_t cgc_GetSectorCount( void ) const { return m_sectorCount; };

private:
	cgc_uint8_t *m_pFlashMemory;
	cgc_uint32_t m_blockSize;
	cgc_uint32_t m_blocksPerSector;
	cgc_uint32_t m_sectorCount;
};

#endif // __NOR_FLASH_H__
