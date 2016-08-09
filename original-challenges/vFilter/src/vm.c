/*
 * Copyright (c) 2015 Kaprica Security, Inc.
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
#include <stdio.h>
#include <string.h>
#include "insn.h"

#define STACK_SIZE 1024

typedef struct {
    unsigned char *base;
    unsigned int length;
} cgc_vm_mem_t;

typedef struct {
    union {
        unsigned int number;
        unsigned char *ptr;
    };
    cgc_vm_mem_t *mem;
} cgc_vm_reg_t;

typedef struct {
    cgc_filter_t *filter;
    cgc_vm_mem_t ctx_mem;
    cgc_vm_mem_t stack_mem;
    unsigned int pc;
    cgc_vm_reg_t registers[16];
} cgc_vm_t;

int cgc_syscall_receive(cgc_vm_t *state);
int cgc_syscall_transmit(cgc_vm_t *state);
int cgc_syscall_cgc_random(cgc_vm_t *state);

typedef int (*cgc_syscall_handler_t) (cgc_vm_t *state);

cgc_syscall_handler_t handlers[] = {
    cgc_syscall_receive,
    cgc_syscall_transmit,
    cgc_syscall_cgc_random,
    NULL
};

static unsigned int cgc_swap32(unsigned int x)
{
    return (((x >> 24) & 0xff) << 0) | (((x >> 16) & 0xff) << 8) | (((x >> 8) & 0xff) << 16) | (((x >> 0) & 0xff) << 24);
}

static int cgc_valid_mem(cgc_vm_reg_t *reg, unsigned int offset, unsigned int length)
{
    if (length > INT_MAX)
        return 0;
    return (reg->ptr + offset >= reg->mem->base) && (reg->ptr + offset + length <= reg->mem->base + reg->mem->length);
}

static cgc_vm_mem_t *cgc_new_mem(cgc_vm_mem_t *mem, unsigned char *base, unsigned int length)
{
    if (mem == NULL)
        mem = cgc_malloc(sizeof(cgc_vm_mem_t));
    mem->base = base;
    mem->length = length;
    return mem;
}

unsigned int cgc_filter_execute(cgc_filter_t *filter, unsigned char *ctx, unsigned int ctx_len)
{
    static void* type_table[] = {
        &&do_ldx, &&do_st, &&do_stx, &&do_alu, &&do_jmp
    };
    static void* alu_table[] = {
        &&do_add, &&do_sub, &&do_mul, &&do_div, &&do_mod, &&do_or, &&do_and, &&do_xor,
        &&do_lsh, &&do_rsh, &&do_neg, &&do_mov, &&do_end
    };
    static void* jmp_table[] = {
        &&do_jeq, &&do_jne, &&do_jgt, &&do_jge, &&do_call, &&do_ret
    };
    unsigned int src, retval;
    cgc_vm_mem_t *smem;
    unsigned char *stack = cgc_malloc(STACK_SIZE);
    cgc_vm_reg_t *dst;
    cgc_vm_t *vm;

    vm = cgc_malloc(sizeof(cgc_vm_t));
    vm->filter = filter;
    vm->pc = 0;
    cgc_memset(vm->registers, 0, sizeof(vm->registers));

    /* setup ctx and frame pointers */
    vm->registers[0].ptr = ctx;
    vm->registers[0].mem = cgc_new_mem(&vm->ctx_mem, ctx, ctx_len);
    vm->registers[15].ptr = stack;
    vm->registers[15].mem = cgc_new_mem(&vm->stack_mem, stack, STACK_SIZE);

    cgc_memset(stack, 0, STACK_SIZE);

    while (1)
    {
        cgc_insn_t *insn = &filter->insn[vm->pc];
        vm->pc++;

        goto *type_table[insn->op.alu.type];

do_ldx:
        if (!cgc_valid_mem(&vm->registers[insn->src], insn->offset, 1 << insn->op.mem.size))
            break;
        if (insn->op.mem.size == S_B)
            src = *(unsigned char *)(vm->registers[insn->src].ptr + insn->offset);
        else if (insn->op.mem.size == S_H)
            src = *(unsigned short *)(vm->registers[insn->src].ptr + insn->offset);
        else if (insn->op.mem.size == S_W)
            src = *(unsigned int *)(vm->registers[insn->src].ptr + insn->offset);
        vm->registers[insn->dst].number = src;
#ifdef PATCHED_1
        vm->registers[insn->dst].mem = NULL;
#endif
        if (insn->op.mem.mode == MODE_STACK)
            vm->registers[insn->src].ptr += insn->offset;
        continue;
do_st:
do_stx:
        if (!cgc_valid_mem(&vm->registers[insn->dst], insn->offset, 1 << insn->op.mem.size))
            break;
        if (insn->op.alu.type == TYPE_ST)
            src = insn->extra;
        else
            src = vm->registers[insn->src].number;
        if (insn->op.mem.size == S_B)
            *(unsigned char *)(vm->registers[insn->dst].ptr + insn->offset) = src;
        else if (insn->op.mem.size == S_H)
            *(unsigned short *)(vm->registers[insn->dst].ptr + insn->offset) = src;
        else if (insn->op.mem.size == S_W)
            *(unsigned int *)(vm->registers[insn->dst].ptr + insn->offset) = src;
        if (insn->op.mem.mode == MODE_STACK)
            vm->registers[insn->dst].ptr += insn->offset;
        continue;

do_alu:
        if (insn->op.alu.source == SRC_REG)
        {
            src = vm->registers[insn->src].number;
            smem = vm->registers[insn->src].mem;
        }
        else
        {
            src = insn->extra;
            smem = NULL;
        }
        dst = &vm->registers[insn->dst];
        goto *alu_table[insn->op.alu.code];
do_add:
        dst->number += src;
        dst->mem = (cgc_vm_mem_t *)((cgc_uintptr_t)dst->mem | (cgc_uintptr_t)smem);
        continue;
do_sub:
        dst->number -= src;
        dst->mem = (cgc_vm_mem_t *)((cgc_uintptr_t)dst->mem | (cgc_uintptr_t)smem);
        continue;
do_mul:
        dst->number *= src;
        continue;
do_div:
        if (src == 0)
            break;
        dst->number /= src;
        continue;
do_mod:
        if (src == 0)
            break;
        dst->number %= src;
        continue;
do_or:
        dst->number |= src;
        continue;
do_and:
        dst->number &= src;
        continue;
do_xor:
        dst->number ^= src;
        continue;
do_lsh:
        dst->number <<= src;
        continue;
do_rsh:
        dst->number >>= src;
        continue;
do_neg:
        dst->number = -dst->number;
        continue;
do_mov:
        dst->number = src;
        dst->mem = smem;
        continue;
do_end:
        dst->number = cgc_swap32(dst->number);
        continue;

do_jmp:
        if (insn->op.alu.source == SRC_REG)
            src = vm->registers[insn->src].number;
        else
            src = insn->extra;
        dst = &vm->registers[insn->dst];
        goto *jmp_table[insn->op.alu.code];
do_jeq:
        if (dst->number == src)
            vm->pc += insn->offset;
        continue;
do_jne:
        if (dst->number != src)
            vm->pc += insn->offset;
        continue;
do_jgt:
        if (src > dst->number)
            vm->pc += insn->offset;
        continue;
do_jge:
        if (src >= dst->number)
            vm->pc += insn->offset;
        continue;
do_call:
        if (!handlers[insn->offset](vm))
            break;
        continue;
do_ret:
        retval = vm->registers[0].number;
        goto cleanup;
    }

    retval = 0xFFFFFFFF;

cleanup:
    cgc_free(stack);
    cgc_free(vm);
    return retval;
}

int cgc_syscall_receive(cgc_vm_t *vm)
{
    int ret;
    unsigned int length = vm->registers[1].number;
    if (!cgc_valid_mem(&vm->registers[0], 0, length))
        return 0;
    cgc_fflush(stdout);
    ret = cgc_fread(vm->registers[0].ptr, length, stdin);
    vm->registers[0].number = ret;
#ifdef PATCHED_1
    vm->registers[0].mem = NULL;
#endif
    return ret >= 0;
}

int cgc_syscall_transmit(cgc_vm_t *vm)
{
    int ret;
    unsigned int length = vm->registers[1].number;
    if (!cgc_valid_mem(&vm->registers[0], 0, length))
        return 0;
    ret = cgc_fwrite(vm->registers[0].ptr, length, stdout);
    vm->registers[0].number = ret;
#ifdef PATCHED_1
    vm->registers[0].mem = NULL;
#endif
    return ret >= 0;
}

int cgc_syscall_cgc_random(cgc_vm_t *vm)
{
    size_t bytes;
    unsigned int length = vm->registers[1].number;
    if (!cgc_valid_mem(&vm->registers[0], 0, length))
        return 0;
    cgc_random(vm->registers[0].ptr, length, &bytes);
    return 1;
}

