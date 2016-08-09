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
#ifndef __CLF_H__
#define __CLF_H__

class cgc_CCLF
{
public:
	friend class cgc_CVM;

public:
	cgc_CCLF();
	~cgc_CCLF();

	bool cgc_LoadFile( cgc_uint8_t *pFileData, cgc_uint32_t fileSize );

public:
	static const cgc_uint32_t MAX_LOADER_FILE_SIZE = 64000;

	static const cgc_uint32_t CLF_HEADER_MAGIC = 0xc3585c51;	// push ecx, pop esp, pop eax, ret

	static const cgc_uint32_t CLF_MAX_SECTION_COUNT = 10;

private:
	enum 
	{
		SECTION_TYPE_EXECUTE = 0,
		SECTION_TYPE_DATA = 1,
		SECTION_TYPE_COMMENT = 2
	} eSectionType;

	// Section file header
	typedef struct SECTION_FILE_HEADER
	{
		cgc_uint8_t sectionType;
		cgc_uint16_t sectionSize;
		cgc_uint16_t sectionAddress;
		cgc_uint32_t fileOffset;
	} __attribute__((packed))  cgc_tSectionFileHeader;

	// Section headers define loadable sections, such as execute, data, and comment
	typedef struct cgc_SECTION_DATA
	{
		cgc_uint8_t sectionType;
		cgc_uint16_t sectionSize;
		cgc_uint16_t sectionAddress;
		cgc_uint8_t *pSectionData;	
	} cgc_tSectionData;

	cgc_tSectionData *m_pSections;
	cgc_uint8_t m_sectionCount;
	cgc_uint16_t m_entryAddress;
};

#endif // __CLF_H__
