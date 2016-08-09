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
#ifndef __CPU_H__
#define __CPU_H__

#include <cutil_string.h>

class cgc_CDMA;
class cgc_CMMU;
class cgc_CVM;

#define MAX_INSTRUCTION_COUNT	(400)

#define MAX_CPU_REGISTERS	16
#define REG_PC			15
#define REG_SP			14

class cgc_CCPU
{
public:
	cgc_CCPU( void *pMagicPage );
	~cgc_CCPU( );

	bool cgc_Init( cgc_CMMU *pMMU, cgc_CVM *pVM, cgc_CDMA *pDMA, cgc_uint16_t entryAddress );
	bool cgc_Run( void );

	bool cgc_HasException( void ) { return m_bException; };
	CUtil::cgc_String cgc_GetExceptionText( void ) { return m_sExceptionText; };

	CUtil::cgc_String cgc_DumpRegisters( void );

protected:
	bool cgc_RunSingleInstruction( void );

	cgc_uint16_t cgc_FetchNextInstruction( void );
#if 0	
	cgc_uint16_t cgc_Read16( cgc_uint16_t address );
	void cgc_Write16( cgc_uint16_t address, cgc_uint16_t value );
#endif

	void cgc_LoadImmediateHI( cgc_uint8_t reg, cgc_uint8_t immediate );
	void cgc_LoadImmediateLO( cgc_uint8_t reg, cgc_uint8_t immediate );

	void cgc_AddReg( cgc_uint8_t regD, cgc_uint8_t regL, cgc_uint8_t regR );
	void cgc_SubReg( cgc_uint8_t regD, cgc_uint8_t regL, cgc_uint8_t regR );
	void cgc_MulReg( cgc_uint8_t regD, cgc_uint8_t regL, cgc_uint8_t regR );
	void cgc_DivReg( cgc_uint8_t regD, cgc_uint8_t regL, cgc_uint8_t regR );

	void cgc_AddImm( cgc_uint8_t reg, cgc_uint8_t immediate );
	void cgc_SubImm( cgc_uint8_t reg, cgc_uint8_t immediate );
	void cgc_MulImm( cgc_uint8_t reg, cgc_uint8_t immediate );
	void cgc_DivImm( cgc_uint8_t reg, cgc_uint8_t immediate );

	void cgc_MovReg( cgc_uint8_t regL, cgc_uint8_t regR );
	void cgc_SwapReg( cgc_uint8_t regL, cgc_uint8_t regR );

	void cgc_JumpOffsetZero( cgc_uint8_t reg, cgc_uint8_t sign, cgc_uint8_t imm );
	void cgc_JumpOffsetNotZero( cgc_uint8_t reg, cgc_uint8_t sign, cgc_uint8_t imm );

	void cgc_JumpLTReg( cgc_uint8_t regL, cgc_uint8_t regR, cgc_uint8_t offsetReg );
	void cgc_JumpGTReg( cgc_uint8_t regL, cgc_uint8_t regR, cgc_uint8_t offsetReg );

	void cgc_JumpReg( cgc_uint8_t offsetReg );
	void cgc_JumpOffset( cgc_uint8_t sign, cgc_uint16_t offset );

	void cgc_JumpRegZero( cgc_uint8_t reg, cgc_uint8_t offsetReg );
	void cgc_JumpRegNotZero( cgc_uint8_t reg, cgc_uint8_t offsetReg );

	void cgc_DMAInit( cgc_uint8_t deviceID );
	void cgc_DMARead( cgc_uint8_t regFrom, cgc_uint8_t regLen );
	void cgc_DMAWrite( cgc_uint8_t regFrom, cgc_uint8_t regLen );

	void cgc_GetInstructionCount( cgc_uint8_t regL, cgc_uint8_t regR );

	void cgc_GetRand( cgc_uint8_t reg );
	void cgc_GetSeedMaterial( cgc_uint8_t reg );

	void cgc_LoadRegister( cgc_uint8_t regD, cgc_uint8_t regAddr );
	void cgc_StoreRegister( cgc_uint8_t regD, cgc_uint8_t regAddr );

private:
	cgc_uint16_t cgc_GetMagicPageSeed( void );

private:
	cgc_CDMA *m_pDMA;
	cgc_CMMU *m_pMMU;
	cgc_CVM *m_pVM;

	cgc_uint16_t m_regs[MAX_CPU_REGISTERS];

	cgc_uint32_t m_instrCount;	// Counts number of instructions executed
	bool m_bException;

	CUtil::cgc_String m_sExceptionText;	// Exception text

	void *m_pMagicPage;

	cgc_uint8_t m_dmaDeviceID;
};

#endif // _CPU_H__
