/*

Author: Dustin Fraze (df@cromulence.co)

Copyright (c) 2014 Cromulence LLC

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

#ifndef _FMT2_H
#define _FMT2_H

#include "instructions.h"
#include "cpu.h"

#pragma pack(1)

enum fmt2Enum {
	JMP,
	JAL,
	LDI,
	LMBD,
	SCAN,
	HALT,
	RESERVED_1,
	RESERVED_2,
	RESERVED_3,
	RESERVED_4,
	RESERVED_5,
	RESERVED_6,
	RESERVED_7,
	RESERVED_8,
	RESERVED_9,
	SLP
};

typedef struct _fmt2InstructionHeader {
	unsigned char opFmt:3;
	unsigned char subOp:4;
	unsigned char other:1;
} cgc_fmt2InstructionHeader;

typedef struct _fmt2BranchInstructionReg {
	cgc_regTarget rs2;
	unsigned char reserved;
} cgc_fmt2BranchInstructionReg;

typedef struct _fmt2BranchInstruction {
	cgc_fmt2InstructionHeader header;
	union { 
		cgc_fmt2BranchInstructionReg reg;
		unsigned short imm;
	} arg2;
	cgc_regTarget rd;
} cgc_fmt2BranchInstruction;

typedef struct _fmt2LdiInstruction {
	cgc_fmt2InstructionHeader header;
	unsigned short imm:16;
	cgc_regTarget rd;
} cgc_fmt2LdiInstruction;

typedef struct _fmt2LmbdInstruction {
	cgc_fmt2InstructionHeader header;
	union {
		cgc_regTarget rs2;
		unsigned char imm;
	} arg2;
	cgc_regTarget rs1;
	cgc_regTarget rd;
} cgc_fmt2LmbdInstruction;

typedef struct _fmt2ScanInstruction {
	cgc_fmt2InstructionHeader header;
	union {
		cgc_regTarget rs2;
		unsigned char imm;
	} arg2;
	cgc_regTarget rs1;
	cgc_regTarget rd;
} cgc_fmt2ScanInstruction;

typedef struct _fmt2HaltInstruction {
	cgc_fmt2InstructionHeader header;
	unsigned char res1;
	unsigned char res2;
	unsigned char res3;
} cgc_fmt2HaltInstruction;

typedef struct _fmt2SlpInstruction {
	cgc_fmt2InstructionHeader header;
	unsigned char WakeOnStatus:1;
	unsigned char res1:7;
	unsigned char res2;
	unsigned char res3;
} cgc_fmt2SlpInstruction;

void cgc_doBranch(cgc_pruCPU *cpu, cgc_fmt2BranchInstruction inst);
void cgc_doLdi(cgc_pruCPU *cpu, cgc_fmt2LdiInstruction inst);
void cgc_doLmbd(cgc_pruCPU *cpu, cgc_fmt2LmbdInstruction inst);
void cgc_doScan(cgc_pruCPU *cpu, cgc_fmt2ScanInstruction inst);

#endif