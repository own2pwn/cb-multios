/*
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

#include <libcgc.h>

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "vm.h"

int cgc_read_n_bytes(int fd, cgc_size_t n, cgc_uint8_t *buf)
{
  if (!n || !buf)
    return -1;

  cgc_size_t rx = 0, total_read = 0;

  while (total_read < n) {
    if (receive(fd, buf + total_read, n - total_read, &rx) != 0) {
      return -1;
    } else if (rx == 0) {
      break;
    } else {
      total_read += rx;
    }
  }

  return total_read;
}

int cgc_read_header(int fd)
{
#define HEADER_LENGTH 4
  cgc_uint8_t header[HEADER_LENGTH] = { 0, 'M', 'V', 'i' };
  cgc_uint8_t buf[HEADER_LENGTH] = { 0, 0, 0, 0 };

  if (cgc_read_n_bytes(fd, HEADER_LENGTH, buf) != HEADER_LENGTH)
    return -1;

  return !cgc_memcmp(header, buf, HEADER_LENGTH);
}

cgc_uint32_t cgc_read_flags(int fd, int *err)
{
  cgc_uint32_t flags = 0;
  if (cgc_read_n_bytes(fd, sizeof(flags), (cgc_uint8_t *)&flags) != sizeof(flags))
    *err = 1;

  return flags;
}

cgc_state *cgc_init_state(int fd, cgc_uint32_t flags)
{
  int read_regs = flags & REG_FLAG;
  cgc_size_t read_mem_sz = flags & MEM_FLAG;

  cgc_state *new = cgc_calloc(1, sizeof(cgc_state));
  if (!new)
    goto err;

  if (read_regs)
    if (cgc_read_n_bytes(fd, sizeof(new->registers), (cgc_uint8_t *)new->registers) != sizeof(new->registers))
      goto err;

  if (read_mem_sz)
    if (cgc_read_n_bytes(fd, read_mem_sz, new->memory) != read_mem_sz)
      goto err;

  return new;

err:
  if (cgc_free) cgc_free(new);
  return NULL;
}

int cgc_dump_regs(int fd, cgc_state *machine)
{
  return transmit(fd, machine->registers, sizeof(machine->registers), NULL) == 0;
}

int cgc_process_load(cgc_state *machine, cgc_uint8_t dst, cgc_uint8_t src, cgc_uint16_t val)
{
  if (src)
    val += machine->registers[src];
  val &= 0xFFFF;
  machine->registers[dst] = machine->memory[val];
  return 0;
}

int cgc_process_str(cgc_state *machine, cgc_uint8_t dst, cgc_uint8_t src, cgc_uint16_t val)
{
  if (dst)
    val += machine->registers[dst];
  val &= 0xFFFF;
  machine->memory[val] = src == 0 ? 0 : machine->registers[src];
  return 0;
}

int cgc_frob(int fd, void *mem_start, cgc_size_t len, cgc_size_t *un2)
{
  for (cgc_size_t i = 0; i < len; i++)
    ((cgc_uint8_t *)mem_start)[i] ^= 0x42;
  return 0;
}

int cgc_process_sys(cgc_state *machine)
{
  struct {
    int fd;
    int (*fp)(int, void *, cgc_size_t, cgc_size_t *);
    char pad[0x100];
  } sv;

  if (machine->registers[1] == 0) {
    sv.fd = STDOUT;
    sv.fp = (int (*)(int, void *, cgc_size_t, cgc_size_t *))transmit;
  } else if (machine->registers[1] == 1) {
    sv.fd = STDIN;
    sv.fp = receive;
  }

  cgc_size_t start, len;
  if ((sv.fp == transmit && machine->registers[1] == 0) ||
      (sv.fp == receive && machine->registers[1] == 1)) {
    start = machine->registers[2] & 0xFFFF;
    len = machine->registers[3] & 0xFFFF;
    if (start + len > MEMORY_SIZE || len == 0)
      return 0;
  } else if (sv.fp == cgc_frob) {
    start = 0;
#ifdef PATCHED
    len = machine->registers[8] & 0xFFFF;
#else
    len = machine->registers[8];
#endif
  } else {
    return 0;
  }

  sv.fp(sv.fd, &(machine->memory[start]), len, NULL);
  return 0;
}

int cgc_process_add(cgc_state *machine, cgc_uint8_t dst, cgc_uint8_t src, cgc_int16_t val)
{
  if (src)
    machine->registers[dst] += machine->registers[src] + val;
  else
    machine->registers[dst] += val;
  return 0;
}

int cgc_process_sub(cgc_state *machine, cgc_uint8_t dst, cgc_uint8_t src, cgc_int16_t val)
{
  if (src)
    machine->registers[dst] -= machine->registers[src] + val;
  else
    machine->registers[dst] -= val;
  return 0;
}

int cgc_process_mul(cgc_state *machine, cgc_uint8_t dst, cgc_uint8_t src, cgc_int16_t val)
{
  if (src)
    machine->registers[dst] *= machine->registers[src] + val;
  else
    machine->registers[dst] *= val;
  return 0;
}

int cgc_process_div(cgc_state *machine, cgc_uint8_t dst, cgc_uint8_t src, cgc_int16_t val)
{
  if (src) {
    if (machine->registers[src] + val == 0)
      return -1;
    else
      machine->registers[dst] /= machine->registers[src] + val;
  } else {
    if (val == 0)
      return -1;
    else
      machine->registers[dst] /= val;
  }
  return 0;
}

int cgc_process_or(cgc_state *machine, cgc_uint8_t dst, cgc_uint8_t src, cgc_int16_t val)
{
  if (src)
    machine->registers[dst] |= machine->registers[src] | val;
  else
    machine->registers[dst] |= val;
  return 0;
}

int cgc_process_and(cgc_state *machine, cgc_uint8_t dst, cgc_uint8_t src, cgc_int16_t val)
{
  if (src)
    machine->registers[dst] &= machine->registers[src] | val;
  else
    machine->registers[dst] &= val;
  return 0;
}

int cgc_process_xor(cgc_state *machine, cgc_uint8_t dst, cgc_uint8_t src, cgc_int16_t val)
{
  struct {
    cgc_uint32_t old_val;
    cgc_uint32_t new_val;
    char pad[0x100];
  } sv;

  sv.old_val = machine->registers[dst];
  sv.new_val = sv.old_val ^ (src ? machine->registers[src] | val : val);
  machine->registers[dst] = sv.new_val;
  return 0;
}

int cgc_process_slt(cgc_state *machine, cgc_uint8_t dst, cgc_uint8_t src2, cgc_uint16_t src1)
{
  machine->registers[dst] = (src1 == 0 ? 0 : machine->registers[src1 & 0xF]) >
    (src2 == 0 ? 0 : machine->registers[src2]);
  return 0;
}

int cgc_process_slte(cgc_state *machine, cgc_uint8_t dst, cgc_uint8_t src2, cgc_uint16_t src1)
{
  machine->registers[dst] = (src1 == 0 ? 0 : machine->registers[src1 & 0xF]) >=
    (src2 == 0 ? 0 : machine->registers[src2]);
  return 0;
}

int cgc_handle_inst(cgc_state *machine, cgc_inst *cur)
{
  switch (cur->op) {
    case LOAD:
      return cgc_process_load(machine, DST(cur), SRC(cur), cur->val);
    case STR:
      return cgc_process_str(machine, DST(cur), SRC(cur), cur->val);
    case SYS:
      return cgc_process_sys(machine);
    case ADD:
      return cgc_process_add(machine, DST(cur), SRC(cur), cur->val);
    case SUB:
      return cgc_process_sub(machine, DST(cur), SRC(cur), cur->val);
    case MUL:
      return cgc_process_mul(machine, DST(cur), SRC(cur), cur->val);
    case DIV:
      return cgc_process_div(machine, DST(cur), SRC(cur), cur->val);
    case OR:
      return cgc_process_or(machine, DST(cur), SRC(cur), cur->val);
    case AND:
      return cgc_process_and(machine, DST(cur), SRC(cur), cur->val);
    case XOR:
      return cgc_process_xor(machine, DST(cur), SRC(cur), cur->val);
    case SLT:
      return cgc_process_slt(machine, DST(cur), SRC(cur), cur->val);
    case SLTE:
      return cgc_process_slte(machine, DST(cur), SRC(cur), cur->val);
    default:
      return -1;
  }
}

int cgc_read_inst(int fd, cgc_state *machine, cgc_inst *cur)
{
  int ret = -1;
  if (!cgc_read_n_bytes(fd, sizeof(cgc_inst), (cgc_uint8_t *)cur) == sizeof(cgc_inst))
    return -1;

  ret = cgc_handle_inst(machine, cur);
  return ret;
}

int main(void)
{
  void *x = cgc_frob;
  transmit(STDOUT, &x, sizeof(void *), NULL);
  if (!cgc_read_header(STDIN))
    return -1;

  int err = 0;
  int flags = cgc_read_flags(STDIN, &err);
  if (err)
    return -1;

  cgc_inst cur;
  cgc_state *machine = cgc_init_state(STDIN, flags);
  int ret;

  while (1) {
    if (cgc_read_inst(STDIN, machine, &cur) < 0)
      break;

    if (!cgc_dump_regs(STDOUT, machine))
      break;
  }

  transmit(STDOUT, "DONE", 5, NULL);
}
