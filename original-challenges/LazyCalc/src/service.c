/*
 * Copyright (c) 2016 Kaprica Security, Inc.
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
#include <stdio.h>
#include <string.h>

#include "calc.h"

#define CMD_ADD 0x903C5CE4UL
#define CMD_SUB 0xC66AC07FUL
#define CMD_MUL 0x3FD26000UL
#define CMD_DIV 0xC5554A87UL
#define CMD_MOD 0x5EAC467DUL
#define CMD_EXP 0xAE22983DUL
#define CMD_IMP 0xB635A8ADUL
#define CMD_CMP 0x285C3CB4UL
#define CMD_QUT 0x7DA7BAE3UL

int g_rslr = 0;
cgc_op_t *g_calcs = NULL;
cgc_size_t g_sz_calcs = 0;
cgc_size_t g_num_calcs = 0;

void cgc_handle_calc(unsigned int cmd)
{
  cgc_op_type type;
  int arg1, arg2;
  switch (cmd)
  {
    case CMD_ADD:
      type = OP_ADD; break;
    case CMD_SUB:
      type = OP_SUB; break;
    case CMD_MUL:
      type = OP_MUL; break;
    case CMD_DIV:
      type = OP_DIV; break;
    case CMD_MOD:
      type = OP_MOD; break;
  }
  if (cgc_fread(&arg1, sizeof(int), stdin) != sizeof(int))
    cgc_exit(0);
  if (cgc_fread(&arg2, sizeof(int), stdin) != sizeof(int))
    cgc_exit(0);
  if (arg1 == 0 || arg2 == 0)
    return;
  g_calcs[g_num_calcs].arg1 = arg1;
  g_calcs[g_num_calcs].arg2 = arg2;
  g_calcs[g_num_calcs].type = type;
  g_num_calcs++;
}

void cgc_handle_exp(int *results)
{
  unsigned int i;
  for (i = 0; i < g_num_calcs; ++i)
  {
    cgc_op_t op;
    op.type = OP_CMP;
    op.arg1 = (unsigned int) &g_calcs[i];
    cgc_calc_compute(&op);
    g_calcs[i].result += g_rslr;
    results[i] = g_calcs[i].result;
  }
}

void cgc_handle_imp(cgc_op_t **calcs)
{
  cgc_op_t op;
  unsigned int i;
  if (cgc_fread(&g_sz_calcs, sizeof(cgc_size_t), stdin) != sizeof(cgc_size_t))
    cgc_exit(0);
  if (g_sz_calcs > 30 || g_sz_calcs < 3)
    return;
  if (g_calcs)
    cgc_free(g_calcs);
  g_calcs = (cgc_op_t *) cgc_malloc(g_sz_calcs * sizeof(cgc_op_t));
  *calcs = g_calcs;
  g_num_calcs = 0;
  for (i = 0; i < g_sz_calcs; ++i)
  {
    if (cgc_fread(&op, sizeof(cgc_op_t), stdin) != sizeof(cgc_op_t))
      cgc_exit(0);
    if (op.arg1 == 0 || op.arg2 == 0)
      cgc_exit(0);
    cgc_memcpy(&g_calcs[g_num_calcs], &op, sizeof(cgc_op_t));
    g_num_calcs++;
  }
}

void cgc_print_cmp(cgc_op_t *cop)
{
  char *o_s = NULL;
  cgc_op_t *op = (cgc_op_t *) cop->arg1;
  switch (op->type)
  {
    case OP_ADD:
      o_s = "+"; break;
    case OP_SUB:
      o_s = "-"; break;
    case OP_MUL:
      o_s = "*"; break;
    case OP_DIV:
      o_s = "/"; break;
    case OP_MOD:
      o_s = "%"; break;
    default:
      o_s = "?"; break;
  }
  cgc_printf("A: %d, B: %d\n", op->arg1, op->arg2);
  cgc_printf("Result of A %s B: %d\n", o_s, op->result);
}

void cgc_handle_cmp()
{
  unsigned int i;
  cgc_printf("\n");
  for (i = 0; i < g_num_calcs; ++i)
  {
    cgc_op_t op;
    op.type = OP_CMP;
    op.arg1 = (int) &g_calcs[i];
    cgc_calc_compute(&op);
    g_calcs[i].result += g_rslr;
    cgc_printf("Slot #%02d\n", i + 1);
    cgc_print_cmp(&op);
  }
}


int __attribute__((fastcall)) main(int secret_page_i, char *unused[]) {
    void *secret_page = (void *)secret_page_i;

    (void) secret_page;

    cgc_op_t *calcs = NULL;
    unsigned int cmd, i, sum = 0;
    int results[30] = { 0 };
    for (i = 0; i < 4096 / 4; ++i)
      sum += *(unsigned int *)&secret_page[i*4];
    sum %= 128;
    g_rslr = sum;
    if (((char *)secret_page)[0] & 1)
      g_rslr = -g_rslr;
    cgc_fwrite(&g_rslr, 2, stdout);

    if (cgc_fread(&g_sz_calcs, sizeof(cgc_size_t), stdin) != sizeof(cgc_size_t))
      return 0;
#ifdef PATCHED_1
    if (g_sz_calcs > 30 || g_sz_calcs < 3)
#else
    if (g_sz_calcs < 3)
#endif
    {
      cgc_fwrite("\xFF\xFF\xFF\xF0", 4, stdout);
      return 0;
    }

    calcs = (cgc_op_t *) cgc_malloc(g_sz_calcs * sizeof(cgc_op_t));
    g_calcs = calcs;

    while (1)
    {
      if (cgc_fread(&cmd, sizeof(unsigned int), stdin) != sizeof(unsigned int))
        return 0;
      switch (cmd)
      {
        case CMD_ADD:
        case CMD_SUB:
        case CMD_MUL:
        case CMD_DIV:
        case CMD_MOD:
          if (g_num_calcs == g_sz_calcs)
            cgc_fwrite("\xFF\xFF\xFF\xAB", 4, stdout);
          else
            cgc_handle_calc(cmd);
          break;
        case CMD_EXP:
          cgc_memset(results, 0, sizeof(results));
          cgc_handle_exp(results);
          for (i = 0; i < g_num_calcs; ++i)
            cgc_fwrite(&results[i], sizeof(int), stdout);
          break;
        case CMD_IMP:
          cgc_handle_imp(&calcs); break;
        case CMD_CMP:
          cgc_handle_cmp(); break;
        case CMD_QUT:
          cgc_fwrite("\x00\x00\x00\x00", 4, stdout);
          cgc_free(calcs);
          return 0;
        default:
          cgc_fwrite("\xFF\xFF\xFF\xFF", 4, stdout);
          break;
      }
    }
    cgc_exit(0);
}
