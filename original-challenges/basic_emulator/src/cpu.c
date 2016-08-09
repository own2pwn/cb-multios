/*
 * Author: Andrew Wesie <andrew.wesie@kapricasecurity.com>
 *
 * Copyright (c) 2014 Kaprica Security, Inc.
 *
 * Permission is hereby granted, cgc_free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "gb.h"
#include "util.h"

static cgc_uint8_t *cgc_r_8b(cgc_gb_t *gb, int r);
static cgc_uint16_t *cgc_r_16b(cgc_gb_t *gb, int r);
static void cgc_flags_bitwise(cgc_gb_t *gb, cgc_uint16_t v);
static cgc_uint8_t cgc_do_add_8b(cgc_gb_t *gb, cgc_uint8_t lhs, cgc_uint8_t rhs, cgc_uint8_t carry);
static cgc_uint16_t cgc_do_add_16b(cgc_gb_t *gb, cgc_uint16_t lhs, cgc_uint16_t rhs, cgc_uint8_t carry);
static cgc_uint8_t cgc_do_sub_8b(cgc_gb_t *gb, cgc_uint8_t lhs, cgc_uint8_t rhs, cgc_uint8_t carry);
static void cgc_do_call(cgc_gb_t *gb, cgc_uint16_t addr);
static void cgc_do_ret(cgc_gb_t *gb);
static int cgc_step_prefix(cgc_gb_t *gb);
static int cgc_step_misc(cgc_gb_t *gb);
static int cgc_step(cgc_gb_t *gb);
static void cgc_do_interrupt(cgc_gb_t *gb, int n);
static void cgc_check_interrupts(cgc_gb_t *gb);

static cgc_uint8_t *cgc_r_8b(cgc_gb_t *gb, int r)
{
    if (r == 0x00)
        return &gb->R_B;
    if (r == 0x01)
        return &gb->R_C;
    if (r == 0x02)
        return &gb->R_D;
    if (r == 0x03)
        return &gb->R_E;
    if (r == 0x04)
        return &gb->R_H;
    if (r == 0x05)
        return &gb->R_L;
    if (r == 0x06)
        return &gb->mem[gb->R_HL];
    if (r == 0x07)
        return &gb->R_A;
    return NULL;
}

static cgc_uint16_t *cgc_r_16b(cgc_gb_t *gb, int r)
{
    if (r == 0x00)
        return &gb->R_BC;
    if (r == 0x01)
        return &gb->R_DE;
    if (r == 0x02)
        return &gb->R_HL;
    if (r == 0x03)
        return &gb->R_SP;
    return NULL;
}

static void cgc_flags_bitwise(cgc_gb_t *gb, cgc_uint16_t v)
{
    gb->R_F = v == 0 ? F_Z : 0;
}

static cgc_uint8_t cgc_do_add_8b(cgc_gb_t *gb, cgc_uint8_t lhs, cgc_uint8_t rhs, cgc_uint8_t carry)
{
    cgc_uint16_t result = lhs + rhs + carry;
    gb->R_F = 0;
    if ((cgc_uint8_t)result == 0)
        gb->R_F |= F_Z;
    if (result > 0xFF)
        gb->R_F |= F_C;
    if (((lhs & 0xf) + (rhs & 0xf)) & 0x10)
        gb->R_F |= F_H;
    if ((((lhs + rhs) & 0xf) + (carry & 0xf)) & 0x10)
        gb->R_F |= F_H;
    return result;
}

static cgc_uint16_t cgc_do_add_16b(cgc_gb_t *gb, cgc_uint16_t lhs, cgc_uint16_t rhs, cgc_uint8_t carry)
{
    cgc_uint32_t result = lhs + rhs + carry;
    gb->R_F &= ~(F_N | F_C | F_H);
    if (result > 0xFFFF)
        gb->R_F |= F_C;
    if (((lhs & 0xFFF) + (rhs & 0xFFF)) & 0x1000)
        gb->R_F |= F_H;
    return result;
}

static cgc_uint8_t cgc_do_sub_8b(cgc_gb_t *gb, cgc_uint8_t lhs, cgc_uint8_t rhs, cgc_uint8_t carry)
{
    cgc_uint16_t result = lhs - rhs - carry;
    gb->R_F = F_N;
    if ((cgc_uint8_t)result == 0)
        gb->R_F |= F_Z;
    if (result > 0xFF)
        gb->R_F |= F_C;
    if (((lhs & 0xf) - (rhs & 0xf)) & 0x10)
        gb->R_F |= F_H;
    if ((((lhs - rhs) & 0xf) - (carry & 0xf)) & 0x10)
        gb->R_F |= F_H;
    return result;
}

static void cgc_do_call(cgc_gb_t *gb, cgc_uint16_t addr)
{
    gb->R_SP -= 2;
    *(cgc_uint16_t *)&gb->mem[gb->R_SP] = gb->R_PC;
    gb->R_PC = addr;
}

static void cgc_do_ret(cgc_gb_t *gb)
{
    gb->R_PC = *(cgc_uint16_t *)&gb->mem[gb->R_SP];
    gb->R_SP += 2;
}

static int cgc_step_prefix(cgc_gb_t *gb)
{
    cgc_uint8_t *pc = &gb->mem[gb->R_PC];
    cgc_uint8_t opc = pc[1];
    cgc_uint8_t rn = opc & 7;
    cgc_uint8_t *r = cgc_r_8b(gb, rn);

    int c;
    int b = (opc & 0x38) >> 3;
    cgc_uint8_t v = *r;
    if ((opc & 0xF8) == 0x00) /* RLC */
    {
        c = (v & 0x80) >> 7;
        *r = (v << 1) | c;
        cgc_flags_bitwise(gb, *r);
        if (c)
            gb->R_F |= F_C;
    }
    else if ((opc & 0xF8) == 0x08) /* RRC */
    {
        c = (v & 1);
        *r = (v >> 1) | (c << 7);
        cgc_flags_bitwise(gb, *r);
        if (c)
            gb->R_F |= F_C;
    }
    else if ((opc & 0xF8) == 0x10) /* RL */
    {
        c = (v & 0x80) >> 7;
        *r = (v << 1) | (!!(gb->R_F & F_C));
        cgc_flags_bitwise(gb, *r);
        if (c)
            gb->R_F |= F_C;
    }
    else if ((opc & 0xF8) == 0x18) /* RR */
    {
        c = (v & 1);
        *r = (v >> 1) | ((!!(gb->R_F & F_C)) << 7);
        cgc_flags_bitwise(gb, *r);
        if (c)
            gb->R_F |= F_C;
    }
    else if ((opc & 0xF8) == 0x20) /* SLA */
    {
        c = (v & 0x80) >> 7;
        *r = (v << 1);
        cgc_flags_bitwise(gb, *r);
        if (c)
            gb->R_F |= F_C;
    }
    else if ((opc & 0xF8) == 0x28) /* SRA */
    {
        c = (v & 1);
        *r = (v >> 1) | (v & 0x80);
        cgc_flags_bitwise(gb, *r);
        if (c)
            gb->R_F |= F_C;
    }
    else if ((opc & 0xF8) == 0x30) /* SWAP */
    {
        *r = (v << 4) | (v >> 4);
        cgc_flags_bitwise(gb, *r);
    }
    else if ((opc & 0xF8) == 0x38) /* SRL */
    {
        c = (v & 1);
        *r = (v >> 1);
        cgc_flags_bitwise(gb, *r);
        if (c)
            gb->R_F |= F_C;
    }
    else if ((opc & 0xC0) == 0x40) /* BIT */
    {
        gb->R_F &= ~(F_N | F_Z);
        gb->R_F |= F_H;
        if ((v & (1 << b)) == 0)
            gb->R_F |= F_Z;
    }
    else if ((opc & 0xC0) == 0x80) /* RES */
    {
        *r = v & ~(1 << b);
    }
    else if ((opc & 0xC0) == 0xC0) /* SET */
    {
        *r = v | (1 << b);
    }
    else
    {
        ERR("Invalid prefix opcode");
        return 0;
    }

    gb->R_PC += 2;

    if (rn == 6)
        return 16;
    else
        return 8;
}

static int cgc_step_misc(cgc_gb_t *gb)
{
    cgc_uint8_t *pc = &gb->mem[gb->R_PC];
    cgc_uint8_t opc = pc[0];

    int c;
    cgc_uint8_t t8;
    cgc_uint16_t t16;
    switch (opc)
    {
    case 0x00:
        /* NOP */
        gb->R_PC++;
        return 4;
    case 0x07:
        /* RLCA */
        c = gb->R_A >> 7;
        gb->R_A = (gb->R_A << 1) | c;
        gb->R_F = 0;
        if (c)
            gb->R_F |= F_C;
        else
            gb->R_F &= ~F_C;
        gb->R_PC++;
        return 4;
    case 0x08:
        *(cgc_uint16_t *)&gb->mem[*(cgc_uint16_t *)&pc[1]] = gb->R_SP;
        gb->R_PC +=3;
        return 20;
    case 0x0F:
        /* RRCA */
        c = gb->R_A & 1;
        gb->R_A = (c << 7) | (gb->R_A >> 1);
        gb->R_F = 0;
        if (c)
            gb->R_F |= F_C;
        else
            gb->R_F &= ~F_C;
        gb->R_PC++;
        return 4;
    case 0x10:
        /* STOP 0 */
        gb->R_PC++;
        return 0;
    case 0x17:
        /* RLA */
        c = gb->R_A >> 7;
        gb->R_A = (gb->R_A << 1) | (!!(gb->R_F & F_C));
        gb->R_F = 0;
        if (c)
            gb->R_F |= F_C;
        else
            gb->R_F &= ~F_C;
        gb->R_PC++;
        return 4;
    case 0x18:
        /* JR r8 */
        gb->R_PC += 2;
        gb->R_PC += (cgc_int8_t)pc[1];
        return 12;
    case 0x1F:
        /* RRA */
        c = gb->R_A & 1;
        gb->R_A = (!!(gb->R_F & F_C) << 7) | (gb->R_A >> 1);
        gb->R_F = 0;
        if (c)
            gb->R_F |= F_C;
        else
            gb->R_F &= ~F_C;
        gb->R_PC++;
        return 4;
    case 0x20:
        /* JR NZ, r8 */
        gb->R_PC += 2;
        if (gb->R_F & F_Z)
        {
            return 8;
        }
        else
        {
            gb->R_PC += (cgc_int8_t)pc[1];
            return 12;
        }
    case 0x27:
        /* DAA */
        t8 = 0;
        cgc_uint8_t oldA = gb->R_A;
        cgc_uint8_t oldF = gb->R_F;
        if (gb->R_F & F_N)
        {
            if (gb->R_F & F_H)
                gb->R_A -= 0x06;
            if (gb->R_F & F_C)
                gb->R_A -= 0x60;
        }
        else
        {
            if (gb->R_A > 0x99 || (gb->R_F & F_C))
            {
                t8 |= 0x60;
                gb->R_F |= F_C;
            }
            if ((gb->R_A & 0xF) > 0x9 || (gb->R_F & F_H))
            {
                t8 |= 0x06;
            }
            gb->R_A += t8;
        }
        gb->R_F &= ~(F_Z | F_H);
        if (gb->R_A == 0)
            gb->R_F |= F_Z;
        gb->R_PC++;
        return 4;
    case 0x28:
        /* JR Z, r8 */
        gb->R_PC += 2;
        if (gb->R_F & F_Z)
        {
            gb->R_PC += (cgc_int8_t)pc[1];
            return 12;
        }
        else
        {
            return 8;
        }
    case 0x2F:
        /* CPL A */
        gb->R_A = ~gb->R_A;
        gb->R_F |= F_N | F_H;
        gb->R_PC++;
        return 4;
    case 0x30:
        /* JR NC, r8 */
        gb->R_PC += 2;
        if (gb->R_F & F_C)
        {
            return 8;
        }
        else
        {
            gb->R_PC += (cgc_int8_t)pc[1];
            return 12;
        }
    case 0x37:
        /* SCF */
        gb->R_F &= ~(F_N | F_H);
        gb->R_F |= F_C;
        gb->R_PC++;
        return 4;
    case 0x38:
        /* JR C, r8 */
        gb->R_PC += 2;
        if (gb->R_F & F_C)
        {
            gb->R_PC += (cgc_int8_t)pc[1];
            return 12;
        }
        else
        {
            return 8;
        }
    case 0x3F:
        /* CCF */
        gb->R_F &= ~(F_N | F_H);
        if (gb->R_F & F_C)
            gb->R_F &= ~F_C;
        else
            gb->R_F |= F_C;
        gb->R_PC++;
        return 4;
    case 0x76:
        /* HALT */
        gb->halted = 1;
        gb->R_PC++;
        return 4;
    case 0xC0:
        /* RET NZ */
        if (gb->R_F & F_Z)
        {
            gb->R_PC++;
            return 8;
        }
        else
        {
            cgc_do_ret(gb);
            return 16;
        }
    case 0xC2:
        /* JP NZ, a16 */
        if (gb->R_F & F_Z)
        {
            gb->R_PC += 3;
            return 12;
        }
        else
        {
            gb->R_PC = *(cgc_uint16_t *)&pc[1];
            return 16;
        }
    case 0xC3:
        /* JP a16 */
        gb->R_PC = *(cgc_uint16_t *)&pc[1];
        return 16;
    case 0xC4:
        /* CALL NZ, a16 */
        gb->R_PC += 3;
        if (gb->R_F & F_Z)
        {
            return 12;
        }
        else
        {
            cgc_do_call(gb, *(cgc_uint16_t *)&pc[1]);
            return 24;
        }
    case 0xC6:
        gb->R_A = cgc_do_add_8b(gb, gb->R_A, pc[1], 0);
        gb->R_PC += 2;
        return 8;
    case 0xC8:
        /* RET Z */
        if (gb->R_F & F_Z)
        {
            cgc_do_ret(gb);
            return 16;
        }
        else
        {
            gb->R_PC++;
            return 8;
        }
    case 0xC9:
        /* RET */
        cgc_do_ret(gb);
        return 16;
    case 0xCA:
        /* JP Z, a16 */
        if (gb->R_F & F_Z)
        {
            gb->R_PC = *(cgc_uint16_t *)&pc[1];
            return 16;
        }
        else
        {
            gb->R_PC += 3;
            return 12;
        }
    case 0xCB:
        return cgc_step_prefix(gb);
    case 0xCC:
        /* CALL Z, a16 */
        gb->R_PC += 3;
        if (gb->R_F & F_Z)
        {
            cgc_do_call(gb, *(cgc_uint16_t *)&pc[1]);
            return 24;
        }
        else
            return 12;
    case 0xCD:
        /* CALL a16 */
        gb->R_PC += 3;
        cgc_do_call(gb, *(cgc_uint16_t *)&pc[1]);
        return 24;
    case 0xCE:
        gb->R_A = cgc_do_add_8b(gb, gb->R_A, pc[1], !!(gb->R_F & F_C));
        gb->R_PC += 2;
        return 8;
    case 0xD0:
        /* RET NC */
        if (gb->R_F & F_C)
        {
            gb->R_PC++;
            return 8;
        }
        else
        {
            cgc_do_ret(gb);
            return 16;
        }
    case 0xD2:
        /* JP NC, a16 */
        if (gb->R_F & F_C)
        {
            gb->R_PC += 3;
            return 12;
        }
        else
        {
            gb->R_PC = *(cgc_uint16_t *)&pc[1];
            return 16;
        }
    case 0xD4:
        /* CALL NC, a16 */
        gb->R_PC += 3;
        if (gb->R_F & F_C)
        {
            return 12;
        }
        else
        {
            cgc_do_call(gb, *(cgc_uint16_t *)&pc[1]);
            return 24;
        }
    case 0xD6:
        gb->R_A = cgc_do_sub_8b(gb, gb->R_A, pc[1], 0);
        gb->R_PC += 2;
        return 8;
    case 0xD8:
        /* RET C */
        if (gb->R_F & F_C)
        {
            cgc_do_ret(gb);
            return 16;
        }
        else
        {
            gb->R_PC++;
            return 8;
        }
    case 0xD9:
        /* RETI */
        cgc_do_ret(gb);
        gb->reg.IME = IME_IE;
        return 16;
    case 0xDA:
        /* JP C, a16 */
        if (gb->R_F & F_C)
        {
            gb->R_PC = *(cgc_uint16_t *)&pc[1];
            return 16;
        }
        else
        {
            gb->R_PC += 3;
            return 12;
        }
    case 0xDC:
        /* CALL C, a16 */
        gb->R_PC += 3;
        if (gb->R_F & F_C)
        {
            cgc_do_call(gb, *(cgc_uint16_t *)&pc[1]);
            return 24;
        }
        else
            return 12;
    case 0xDE:
        gb->R_A = cgc_do_sub_8b(gb, gb->R_A, pc[1], !!(gb->R_F & F_C));
        gb->R_PC += 2;
        return 8;
    case 0xE0:
        gb->mem[0xFF00 | pc[1]] = gb->R_A;
        gb->R_PC += 2;
        return 12;
    case 0xE2:
        gb->mem[0xFF00 | gb->R_C] = gb->R_A;
        gb->R_PC++;
        return 8;
    case 0xE6:
        gb->R_A &= pc[1];
        cgc_flags_bitwise(gb, gb->R_A);
        gb->R_F |= F_H;
        gb->R_PC += 2;
        return 8;
    case 0xE8:
        gb->R_SP = cgc_do_add_16b(gb, gb->R_SP, (cgc_int8_t)pc[1], 0);
        gb->R_F &= ~F_Z;
        gb->R_PC += 2;
        return 16;
    case 0xE9:
        /* JP HL */
        gb->R_PC = gb->R_HL;
        return 4;
    case 0xEA:
        gb->mem[*(cgc_uint16_t*)&pc[1]] = gb->R_A;
        gb->R_PC += 3;
        return 16;
    case 0xEE:
        gb->R_A ^= pc[1];
        cgc_flags_bitwise(gb, gb->R_A);
        gb->R_PC += 2;
        return 8;
    case 0xF0:
        gb->R_A = gb->mem[0xFF00 | pc[1]];
        gb->R_PC += 2;
        return 12;
    case 0xF2:
        gb->R_A = gb->mem[0xFF00 | gb->R_C];
        gb->R_PC++;
        return 8;
    case 0xF3:
        gb->reg.IME |= IME_WAIT | IME_DI;
        gb->R_PC++;
        return 4;
    case 0xF6:
        gb->R_A |= pc[1];
        cgc_flags_bitwise(gb, gb->R_A);
        gb->R_PC += 2;
        return 8;
    case 0xF8:
        gb->R_HL = cgc_do_add_16b(gb, gb->R_SP, (cgc_int8_t)pc[1], 0);
        gb->R_F &= ~F_Z;
        gb->R_PC += 2;
        return 12;
    case 0xF9:
        gb->R_SP = gb->R_HL;
        gb->R_PC++;
        return 8;
    case 0xFA:
        gb->R_A = gb->mem[*(cgc_uint16_t*)&pc[1]];
        gb->R_PC += 3;
        return 16;
    case 0xFB:
        gb->reg.IME |= IME_WAIT | IME_EI;
        gb->R_PC++;
        return 4;
    case 0xFE:
        /* compare gb->R_A and pc[1] */
        cgc_do_sub_8b(gb, gb->R_A, pc[1], 0);
        gb->R_PC += 2;
        return 8;
    default:
        return 0;
    }
}

static int cgc_step(cgc_gb_t *gb)
{
    cgc_uint8_t *pc = &gb->mem[gb->R_PC];
    cgc_uint8_t opc = pc[0];
    //ERR("pc %04X opc %02X", gb->R_PC, opc);

    int cycles = cgc_step_misc(gb);
    if (cycles != 0)
    {
        return cycles;
    }
    else if ((opc & 0xC7) == 0x02)
    {
        cgc_uint16_t *r;
        switch (opc & 0x30)
        {
        case 0x00:
            r = &gb->R_BC;
            break;
        case 0x10:
            r = &gb->R_DE;
            break;
        case 0x20:
        case 0x30:
            r = &gb->R_HL;
            break;
        }
        
        if (opc & 0x08) /* LD A, (rr) */
            gb->R_A = gb->mem[*r];
        else /* LD (rr), A */
            gb->mem[*r] = gb->R_A;

        switch (opc & 0x30)
        {
        case 0x20:
            gb->R_HL++;
            break;
        case 0x30:
            gb->R_HL--;
            break;
        }
        gb->R_PC++;
        return 8;
    }
    else if ((opc & 0xC7) == 0x04) /* INC r */
    {
        cgc_uint8_t rn = (opc & 0x38) >> 3;
        cgc_uint8_t *r = cgc_r_8b(gb, rn);
        cgc_uint8_t old = *r;
        (*r)++;
        gb->R_F &= ~(F_Z | F_N | F_H);
        if (*r == 0)
            gb->R_F |= F_Z;
        if ((old & 0xF0) != (*r & 0xF0))
            gb->R_F |= F_H;
        gb->R_PC++;
        if (rn == 6)
            return 12;
        else
            return 4;
    }
    else if ((opc & 0xC7) == 0x05) /* DEC r */
    {
        cgc_uint8_t rn = (opc & 0x38) >> 3;
        cgc_uint8_t *r = cgc_r_8b(gb, rn);
        cgc_uint8_t old = *r;
        (*r)--;
        gb->R_F &= ~(F_Z | F_N | F_H);
        gb->R_F |= F_N;
        if (*r == 0)
            gb->R_F |= F_Z;
        if ((old & 0xF0) != (*r & 0xF0))
            gb->R_F |= F_H;
        gb->R_PC++;
        if (rn == 6)
            return 12;
        else
            return 4;
    }
    else if ((opc & 0xC7) == 0x06) /* LD r, n */
    {
        cgc_uint8_t rn = (opc & 0x38) >> 3;
        cgc_uint8_t *r = cgc_r_8b(gb, rn);
        *r = pc[1];
        gb->R_PC += 2;
        if (rn == 6)
            return 12;
        else
            return 8;
    }
    else if ((opc & 0xCF) == 0x01) /* LD rr, nn nn */
    {
        cgc_uint8_t rn = (opc & 0x30) >> 4;
        cgc_uint16_t *r = cgc_r_16b(gb, rn);
        *r = *(cgc_uint16_t *)&pc[1];
        gb->R_PC += 3;
        return 12;
    }
    else if ((opc & 0xCF) == 0x03) /* INC rr */
    {
        cgc_uint8_t rn = (opc & 0x30) >> 4;
        cgc_uint16_t *r = cgc_r_16b(gb, rn);
        (*r)++;
        gb->R_PC++;
        return 8;
    }
    else if ((opc & 0xCF) == 0x09) /* ADD HL, rr */
    {
        cgc_uint8_t rn = (opc & 0x30) >> 4;
        cgc_uint16_t *r = cgc_r_16b(gb, rn);
        gb->R_HL = cgc_do_add_16b(gb, gb->R_HL, *r, 0);
        gb->R_PC++;
        return 8;
    }
    else if ((opc & 0xCF) == 0x0B) /* DEC rr */
    {
        cgc_uint8_t rn = (opc & 0x30) >> 4;
        cgc_uint16_t *r = cgc_r_16b(gb, rn);
        (*r)--;
        gb->R_PC++;
        return 8;
    }
    else if ((opc & 0xC0) == 0x40) /* LD r1, r2 */
    {
        cgc_uint8_t rn1 = opc & 0x07;
        cgc_uint8_t rn2 = (opc & 0x38) >> 3;
        cgc_uint8_t *r1 = cgc_r_8b(gb, rn1);
        cgc_uint8_t *r2 = cgc_r_8b(gb, rn2);
        *r2 = *r1;
        gb->R_PC++;
        if (rn1 == 6 || rn2 == 6)
            return 8;
        else
            return 4;
    }
    else if ((opc & 0xF8) == 0x80) /* ADD A, r */
    {
        cgc_uint8_t rn = opc & 7;
        cgc_uint8_t *r = cgc_r_8b(gb, rn);
        gb->R_A = cgc_do_add_8b(gb, gb->R_A, *r, 0);
        gb->R_PC++;
        if (rn == 6)
            return 8;
        else
            return 4;
    }
    else if ((opc & 0xF8) == 0x88) /* ADC A, r */
    {
        cgc_uint8_t rn = opc & 7;
        cgc_uint8_t *r = cgc_r_8b(gb, rn);
        gb->R_A = cgc_do_add_8b(gb, gb->R_A, *r, !!(gb->R_F & F_C));
        gb->R_PC++;
        if (rn == 6)
            return 8;
        else
            return 4;
    }
    else if ((opc & 0xF8) == 0x90) /* SUB A, r */
    {
        cgc_uint8_t rn = opc & 7;
        cgc_uint8_t *r = cgc_r_8b(gb, rn);
        cgc_uint8_t old = gb->R_A;
        gb->R_A = cgc_do_sub_8b(gb, gb->R_A, *r, 0);
        gb->R_PC++;
        if (rn == 6)
            return 8;
        else
            return 4;
    }
    else if ((opc & 0xF8) == 0x98) /* SBC A, r */
    {
        cgc_uint8_t rn = opc & 7;
        cgc_uint8_t *r = cgc_r_8b(gb, rn);
        gb->R_A = cgc_do_sub_8b(gb, gb->R_A, *r, !!(gb->R_F & F_C));
        gb->R_PC++;
        if (rn == 6)
            return 8;
        else
            return 4;
    }
    else if ((opc & 0xF8) == 0xA0) /* AND A, r */
    {
        cgc_uint8_t rn = opc & 7;
        cgc_uint8_t *r = cgc_r_8b(gb, rn);
        gb->R_A &= *r;
        cgc_flags_bitwise(gb, gb->R_A);
        gb->R_F |= F_H;
        gb->R_PC++;
        if (rn == 6)
            return 8;
        else
            return 4;
    }
    else if ((opc & 0xF8) == 0xA8) /* XOR A, r */
    {
        cgc_uint8_t rn = opc & 7;
        cgc_uint8_t *r = cgc_r_8b(gb, rn);
        gb->R_A ^= *r;
        cgc_flags_bitwise(gb, gb->R_A);
        gb->R_PC++;
        if (rn == 6)
            return 8;
        else
            return 4;
    }
    else if ((opc & 0xF8) == 0xB0) /* OR A, r */
    {
        cgc_uint8_t rn = opc & 7;
        cgc_uint8_t *r = cgc_r_8b(gb, rn);
        gb->R_A |= *r;
        cgc_flags_bitwise(gb, gb->R_A);
        gb->R_PC++;
        if (rn == 6)
            return 8;
        else
            return 4;
    }
    else if ((opc & 0xF8) == 0xB8) /* CP A, r */
    {
        cgc_uint8_t rn = opc & 7;
        cgc_uint8_t *r = cgc_r_8b(gb, rn);
        cgc_do_sub_8b(gb, gb->R_A, *r, 0);
        gb->R_PC++;
        if (rn == 6)
            return 8;
        else
            return 4;
    }
    else if ((opc & 0xC7) == 0xC7) /* RST n */
    {
        cgc_uint8_t n = opc & 0x38;
        gb->R_PC++;
        cgc_do_call(gb, n);
        return 16;
    }
    else if ((opc & 0xCF) == 0xC1) /* POP r */
    {
        cgc_uint8_t rn = (opc & 0x30) >> 4;
        cgc_uint16_t *r;
        if (rn == 3)
            r = &gb->R_AF;
        else
            r = cgc_r_16b(gb, rn);
        *r = *(cgc_uint16_t *)&gb->mem[gb->R_SP];
        if (rn == 3)
            gb->R_F &= 0xF0;
        gb->R_SP += 2;
        gb->R_PC++;
        return 16;
    }
    else if ((opc & 0xCF) == 0xC5) /* PUSH r */
    {
        cgc_uint8_t rn = (opc & 0x30) >> 4;
        cgc_uint16_t *r;
        if (rn == 3)
            r = &gb->R_AF;
        else
            r = cgc_r_16b(gb, rn);
        gb->R_SP -= 2;
        *(cgc_uint16_t *)&gb->mem[gb->R_SP] = *r;
        gb->R_PC++;
        return 16;
    }
    else
    {
        ERR("Invalid opcode %02X", opc);
        return 0;
    }
}

void cgc_cpu_interrupt(cgc_gb_t *gb, int n)
{
    gb->mem[IO_IF] |= (1 << n);
}

static void cgc_do_interrupt(cgc_gb_t *gb, int n)
{
    // reset bit in IF
    gb->mem[IO_IF] &= ~(1 << n);
    // disable interrupts
    gb->reg.IME = 0;
    // call exception vector
    cgc_do_call(gb, 0x040 + n * 8);
}

static void cgc_check_interrupts(cgc_gb_t *gb)
{
    cgc_uint8_t masked = gb->mem[IO_IE] & gb->mem[IO_IF];
    // process HALT even if IME disabled
    if (masked)
        gb->halted = 0;
    if ((gb->reg.IME & IME_IE) == 0)
        return;
    if (masked & (1 << 0))
        cgc_do_interrupt(gb, 0);
    else if (masked & (1 << 1))
        cgc_do_interrupt(gb, 1);
    else if (masked & (1 << 2))
        cgc_do_interrupt(gb, 2);
    else if (masked & (1 << 3))
        cgc_do_interrupt(gb, 3);
    else if (masked & (1 << 4))
        cgc_do_interrupt(gb, 4);
}

int cgc_cpu_tick(cgc_gb_t *gb)
{
    if (gb->reg.IME & IME_WAIT)
    {
        gb->reg.IME &= ~IME_WAIT;
    }
    else
    {
        if (gb->reg.IME & IME_DI)
            gb->reg.IME &= ~IME_IE;
        if (gb->reg.IME & IME_EI)
            gb->reg.IME |= IME_IE;
        gb->reg.IME &= ~(IME_DI | IME_EI);
    }

    cgc_check_interrupts(gb);

    if (gb->ticks_skip > 1)
    {
        gb->ticks_skip--;
    }
    else if (!gb->halted)
    {
        int cycles = cgc_step(gb);
        if (cycles == 0)
            return 0;
        gb->ticks_skip = cycles / 4;
    }
    return 1;
}
