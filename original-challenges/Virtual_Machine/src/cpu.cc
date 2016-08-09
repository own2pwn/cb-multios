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
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <prng.h>
}

#define DEBUG_INSTRUCTION_EXECUTION	(1)

#include <cutil_string.h>

#include "cpu.h"
#include "mmu.h"
#include "vm.h"

#ifdef NULL
#undef NULL
#define NULL (0)
#endif

#define GET_BIT( val, x ) ((val >> x) & 0x1)
#define GET_BITS( val, x, y ) ((val >> x) & ((1<<((y-x)+1))-1))

cgc_CCPU::cgc_CCPU( void *pMagicPage )
	: m_pMagicPage( pMagicPage )
{

}

cgc_CCPU::~cgc_CCPU( )
{

}

bool cgc_CCPU::cgc_Init( cgc_CMMU *pMMU, cgc_CVM *pVM, cgc_CDMA *pDMA, cgc_uint16_t entryAddress )
{
	if ( !pMMU )
		return (false);

	if ( !pVM )
		return (false);

	if ( !pDMA )
		return (false);

	m_pDMA = pDMA;
	m_pMMU = pMMU;
	m_pVM = pVM;

	// Initialize registers!
	for ( cgc_uint8_t i = 0; i < MAX_CPU_REGISTERS; i++ )
	{
		m_regs[i] = 0;
	}

	m_regs[REG_PC] = entryAddress;
	m_regs[REG_SP] = 0xFFFC;

	m_instrCount = 0;
	m_bException = false;

	m_sExceptionText = "";

	return (true);
}

bool cgc_CCPU::cgc_Run( void )
{
	if ( !m_pMMU || !m_pVM )
		return (false);

	while ( cgc_RunSingleInstruction() )
		;

	if ( m_bException )
		return (false);

	return (true);
}

CUtil::cgc_String cgc_CCPU::cgc_DumpRegisters( void )
{
	char szTemp[1024];

	cgc_sprintf( szTemp, "Instructions: $d\nR0  = $08X R1  = $08X R2  = $08X R3  = $08X\nR4  = $08X R5  = $08X R6  = $08X R7  = $08X\nR8  = $08X R9  = $08X R10 = $08X R11 = $08X\nR12 = $08X R13 = $08X SP  = $08X PC  = $08X", m_instrCount, m_regs[0], m_regs[1], m_regs[2], m_regs[3], m_regs[4], m_regs[5], m_regs[6], m_regs[7], m_regs[8], m_regs[9], m_regs[10], m_regs[11], m_regs[12], m_regs[13], m_regs[14], m_regs[15] );

	return CUtil::cgc_String(szTemp);	
}

bool cgc_CCPU::cgc_RunSingleInstruction( void )
{
	cgc_uint16_t instr = cgc_FetchNextInstruction( );

	if ( m_bException )
		return (false);

	if ( m_instrCount++ > MAX_INSTRUCTION_COUNT )
	{
		m_sExceptionText = "Max instructions reached";
		m_bException = true;
		return (false);
	}
#if DEBUG_INSTRUCTION_EXECUTION
	{
		cgc_printf( "DEBUG:$s\n=====\n", cgc_DumpRegisters().cgc_c_str() );
	}
#endif

	// Service DMA requests (1 at a time) per instruction -- run at the same clock as the instruction dispatch
	m_pDMA->cgc_ServiceDMA( m_pMMU );

	switch( GET_BITS( instr, 13, 15 ) )
	{
	case 0:
		if ( GET_BIT( instr, 12 ) == 0 )
		{
			// Load immediate value HI
			cgc_LoadImmediateHI( GET_BITS( instr, 8, 11 ), GET_BITS( instr, 0, 7 ) );
		}
		else
		{
			// Load immediate value LO
			cgc_LoadImmediateLO( GET_BITS( instr, 8, 11 ), GET_BITS( instr, 0, 7 ) );
		}	
		break;

	case 1:
		if ( GET_BIT( instr, 12 ) == 0 )
		{
			cgc_AddReg( GET_BITS( instr, 8, 11 ), GET_BITS( instr, 4, 7 ), GET_BITS( instr, 0, 3 ) );
		}
		else
		{
			cgc_SubReg( GET_BITS( instr, 8, 11 ), GET_BITS( instr, 4, 7 ), GET_BITS( instr, 0, 3 ) );
		}
		break;

	case 2:
		if ( GET_BIT( instr, 12 ) == 0 )
		{
			cgc_MulReg( GET_BITS( instr, 8, 11 ), GET_BITS( instr, 4, 7 ), GET_BITS( instr, 0, 3 ) );
		}
		else
		{
			cgc_DivReg( GET_BITS( instr, 8, 11 ), GET_BITS( instr, 4, 7 ), GET_BITS( instr, 0, 3 ) );
		}
		break;

	case 3:
		switch( GET_BITS( instr, 8, 11 ) )
		{
		case 0:
			cgc_MovReg( GET_BITS( instr, 4, 7 ), GET_BITS( instr, 0, 3 ) );
			break;

		case 1:
			cgc_SwapReg( GET_BITS( instr, 4, 7 ), GET_BITS( instr, 0, 3 ) );
			break;

		case 2:
			cgc_JumpRegZero( GET_BITS( instr, 4, 7 ), GET_BITS( instr, 0, 3 ) );
			break;

		case 3:
			cgc_JumpRegNotZero( GET_BITS( instr, 4, 7 ), GET_BITS( instr, 0, 3 ) );
			break;

		case 4:
			cgc_DMAInit( GET_BITS( instr, 0, 7 ) );
			break;

		case 5:
			cgc_DMARead( GET_BITS( instr, 4, 7 ), GET_BITS( instr, 0, 3 ) ); 
			break;

		case 6:
			cgc_DMAWrite( GET_BITS( instr, 4, 7 ), GET_BITS( instr, 0, 3 ) );
			break;

		case 7:
			cgc_JumpOffset( 0, GET_BITS( instr, 0, 7 ) );
			break;

		case 8:
			cgc_JumpOffset( 1, GET_BITS( instr, 0, 7 ) );
			break;

		case 9:
			cgc_JumpReg( GET_BITS( instr, 0, 3 ) );
			break;

		case 10:	// GetInstrCount
			cgc_GetInstructionCount( GET_BITS( instr, 0, 3 ), GET_BITS( instr, 4, 7 ) );
			break;

		case 11:	// cgc_GetRand
			cgc_GetRand( GET_BITS( instr, 0, 3 ) );
			break;

		case 12:
			cgc_GetSeedMaterial( GET_BITS( instr, 0, 3 ) );
			break;

		case 13:
			cgc_LoadRegister( GET_BITS( instr, 4, 7 ), GET_BITS( instr, 0, 3 ) );
			break;

		case 14:
			cgc_StoreRegister( GET_BITS( instr, 4, 7 ), GET_BITS( instr, 0, 3 ) );
			break;

		default:
			m_sExceptionText = "Invalid opcode";
			m_bException = true;
			break;
		}
		break;

	case 4:
		switch ( GET_BITS( instr, 11, 12 ) )
		{
		case 0:
			cgc_AddImm( GET_BITS( instr, 7, 10 ), GET_BITS( instr, 0, 6 ) );
			break;

		case 1:
			cgc_SubImm( GET_BITS( instr, 7, 10 ), GET_BITS( instr, 0, 6 ) ); 
			break;

		case 2:
			cgc_MulImm( GET_BITS( instr, 7, 10 ), GET_BITS( instr, 0, 6 ) );
			break;

		case 3:
			cgc_DivImm( GET_BITS( instr, 7, 10 ), GET_BITS( instr, 0, 6 ) );
			break;
		}
		break;

	case 5:
		if ( GET_BIT( instr, 12 ) == 0 )
		{
			cgc_JumpLTReg( GET_BITS( instr, 8, 11 ), GET_BITS( instr, 4, 7 ), GET_BITS( instr, 0, 3 ) );
		}
		else
		{
			cgc_JumpGTReg( GET_BITS( instr, 8, 11 ), GET_BITS( instr, 4, 7 ), GET_BITS( instr, 0, 3 ) );
		}
		break;

	case 6:
		cgc_JumpOffsetZero( GET_BITS( instr, 9, 12 ), GET_BIT( instr, 8 ), GET_BITS( instr, 0, 7 ) );
		break;	

	case 7:
		cgc_JumpOffsetNotZero( GET_BITS( instr, 9, 12 ), GET_BIT( instr, 8 ), GET_BITS( instr, 0, 7 ) );
		break;
	}

	if ( m_bException )
		return (false);

	return (true);
}

void cgc_CCPU::cgc_LoadImmediateHI( cgc_uint8_t reg, cgc_uint8_t immediate )
{
	m_regs[reg] = (m_regs[reg] & 0xFF) | ((cgc_uint16_t)immediate << 8);
}

void cgc_CCPU::cgc_LoadImmediateLO( cgc_uint8_t reg, cgc_uint8_t immediate )
{
	m_regs[reg] = (m_regs[reg] & 0xFF00) | immediate;
}

void cgc_CCPU::cgc_AddReg( cgc_uint8_t regD, cgc_uint8_t regL, cgc_uint8_t regR )
{
	m_regs[regD] = m_regs[regL] + m_regs[regR];
}

void cgc_CCPU::cgc_SubReg( cgc_uint8_t regD, cgc_uint8_t regL, cgc_uint8_t regR )
{
	m_regs[regD] = m_regs[regL] - m_regs[regR];
}

void cgc_CCPU::cgc_MulReg( cgc_uint8_t regD, cgc_uint8_t regL, cgc_uint8_t regR )
{
	m_regs[regD] = m_regs[regL] * m_regs[regR];
}

void cgc_CCPU::cgc_DivReg( cgc_uint8_t regD, cgc_uint8_t regL, cgc_uint8_t regR )
{
	if ( m_regs[regR] == 0 )
	{
		m_sExceptionText = "Divide by zero";
		m_bException = true;
		return;
	}

	m_regs[regD] = m_regs[regL] / m_regs[regR];
}

void cgc_CCPU::cgc_AddImm( cgc_uint8_t regD, cgc_uint8_t immediate )
{
	m_regs[regD] += immediate;
}

void cgc_CCPU::cgc_SubImm( cgc_uint8_t regD, cgc_uint8_t immediate )
{
	m_regs[regD] -= immediate;
}

void cgc_CCPU::cgc_MulImm( cgc_uint8_t regD, cgc_uint8_t immediate )
{
	m_regs[regD] *= immediate;
}

void cgc_CCPU::cgc_DivImm( cgc_uint8_t regD, cgc_uint8_t immediate )
{
	if ( immediate == 0 )
	{
		m_sExceptionText = "Divide by zero";
		m_bException = true;
		return;
	}

	m_regs[regD] /= immediate;
}

void cgc_CCPU::cgc_MovReg( cgc_uint8_t regL, cgc_uint8_t regR )
{
	m_regs[regL] =  m_regs[regR];
}

void cgc_CCPU::cgc_SwapReg( cgc_uint8_t regL, cgc_uint8_t regR )
{
	cgc_uint16_t temp = m_regs[regL];
	m_regs[regL] = m_regs[regR];
	m_regs[regR] = temp;
}

void cgc_CCPU::cgc_JumpOffsetZero( cgc_uint8_t reg, cgc_uint8_t sign, cgc_uint8_t imm )
{
	if ( m_regs[reg] == 0 )
	{
		if ( sign == 0 )
			m_regs[REG_PC] += ((cgc_uint16_t)imm) << 1;
		else
			m_regs[REG_PC] -= ((cgc_uint16_t)imm) << 1;
	}

}

void cgc_CCPU::cgc_JumpOffsetNotZero( cgc_uint8_t reg, cgc_uint8_t sign, cgc_uint8_t imm )
{
	if ( m_regs[reg] != 0 )
	{
		if ( sign == 0 )
			m_regs[REG_PC] += ((cgc_uint16_t)imm) << 1;
		else
			m_regs[REG_PC] -= ((cgc_uint16_t)imm) << 1;
	}
}

void cgc_CCPU::cgc_JumpRegZero( cgc_uint8_t reg, cgc_uint8_t offsetReg )
{
	if ( m_regs[reg] == 0 )
		m_regs[REG_PC] += m_regs[offsetReg];
}

void cgc_CCPU::cgc_JumpRegNotZero( cgc_uint8_t reg, cgc_uint8_t offsetReg )
{
	if ( m_regs[reg] != 0 )
		m_regs[REG_PC] += m_regs[offsetReg];
}

void cgc_CCPU::cgc_JumpLTReg( cgc_uint8_t regL, cgc_uint8_t regR, cgc_uint8_t offsetReg )
{
	if ( m_regs[regL] < m_regs[regR] )
		m_regs[REG_PC] += m_regs[offsetReg];
}

void cgc_CCPU::cgc_JumpGTReg( cgc_uint8_t regL, cgc_uint8_t regR, cgc_uint8_t offsetReg )
{
	if ( m_regs[regL] > m_regs[regR] )
		m_regs[REG_PC] += m_regs[offsetReg];
}

void cgc_CCPU::cgc_JumpReg( cgc_uint8_t offsetReg )
{
	m_regs[REG_PC] = m_regs[offsetReg];
}

void cgc_CCPU::cgc_JumpOffset( cgc_uint8_t sign, cgc_uint16_t offset )
{
	if ( sign == 0 )
		m_regs[REG_PC] += offset;
	else
		m_regs[REG_PC] -= offset;
}

void cgc_CCPU::cgc_DMAInit( cgc_uint8_t deviceID )
{
	m_dmaDeviceID = deviceID;
}

void cgc_CCPU::cgc_DMAWrite( cgc_uint8_t regFrom, cgc_uint8_t regLen )
{
	m_pDMA->cgc_InitWriteWorker( m_dmaDeviceID, m_regs[regFrom], m_regs[regLen] );
}

void cgc_CCPU::cgc_DMARead( cgc_uint8_t regFrom, cgc_uint8_t regLen )
{
	m_pDMA->cgc_InitReadWorker( m_dmaDeviceID, m_regs[regFrom], m_regs[regLen] );
}

void cgc_CCPU::cgc_GetInstructionCount( cgc_uint8_t reg1, cgc_uint8_t reg2 )
{
	m_regs[reg1] = (m_instrCount >> 16) & 0xFFFF;
	m_regs[reg2] = (m_instrCount & 0xFFFF);
}

void cgc_CCPU::cgc_GetRand( cgc_uint8_t reg )
{
	m_regs[reg] = cgc_prng()&0xFFFF;
}

cgc_uint16_t cgc_CCPU::cgc_GetMagicPageSeed( void )
{
	cgc_uint16_t offset = (m_instrCount % 256) << 2;

	return *((cgc_uint16_t*)((cgc_uint8_t*)m_pMagicPage + offset));
}

void cgc_CCPU::cgc_GetSeedMaterial( cgc_uint8_t reg )
{
	m_regs[reg] = (cgc_prng()&0xFFFF ^ cgc_GetMagicPageSeed( ) );
}

void cgc_CCPU::cgc_LoadRegister( cgc_uint8_t regD, cgc_uint8_t regAddr )
{
	cgc_uint16_t loadValue;

	if ( !m_pMMU->cgc_Read16( m_regs[regAddr], loadValue ) )
	{
		m_sExceptionText = "MMU cgc_Read Exception";
		m_bException = true;
		return;
	}

	m_regs[regD] = loadValue;
}

void cgc_CCPU::cgc_StoreRegister( cgc_uint8_t regD, cgc_uint8_t regAddr )
{
	if ( !m_pMMU->cgc_Write16( m_regs[regAddr], m_regs[regD] ) )
	{
		m_sExceptionText = "MMU cgc_Write Exception";
		m_bException = true;
		return;
	}
}

cgc_uint16_t cgc_CCPU::cgc_FetchNextInstruction( void )
{
	cgc_uint16_t instr;

	if ( !m_pMMU->cgc_Fetch16( m_regs[REG_PC], instr ) )
	{
		m_sExceptionText = "MMU Fetch Exception";
		m_bException = true;
		m_regs[REG_PC] += 2;
		return (0);
	}

	m_regs[REG_PC] += 2;

	return (instr);
}

#if 0 
cgc_uint16_t cgc_CCPU::cgc_Read16( cgc_uint16_t address )
{
	cgc_uint16_t value;

	if ( !m_pMMU->cgc_Read16( address, value ) )
	{
		m_bException = true;
		return (0);
	}

	return (value);
}

void cgc_CCPU::cgc_Write16( cgc_uint16_t address, cgc_uint16_t value )
{
	if ( !m_pMMU->cgc_Write16( address, value ) )
	{
		m_bException = true;
		return;
	}
}
#endif
