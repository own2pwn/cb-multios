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

#include <cstdio.h>
#include <cstdint.h>
#include <cstring.h>

#include "vector.h"
#include "nnet.h"

#define MAX_NUM_BED   20.0
#define MAX_NUM_BATH  25.0
#define MAX_SQ_FT     20000.0
#define MAX_NUM_CRIME 500.0
#define MAX_PRICE     50000.0

unsigned int g_state;

void cgc_init_g_state(char *secret)
{
  g_state = *(unsigned int *)secret * 1103515247 + 13579;
}

unsigned int cgc_g_prng()
{
  g_state = g_state * 1103515247 + 13579;
  return (g_state / 65536) % 32768;
}

int cgc_start_training(cgc_NeuralNet &nnet)
{
  cgc_uint16_t numBeds, numBaths, sqFt, numCrimes, price;
  unsigned int i;
  size_t numSamples;
  cgc_vector<double> input, target;
  if (cgc_fread(&numSamples, sizeof(size_t), stdin) != sizeof(size_t))
    goto fail;
  if (numSamples < 500 || numSamples > 1000)
    goto fail;
  for (i = 0; i < numSamples; ++i)
  {
    if (cgc_fread(&numBeds, sizeof(cgc_uint16_t), stdin) != sizeof(cgc_uint16_t) || numBeds > MAX_NUM_BED)
      goto fail;
    if (cgc_fread(&numBaths, sizeof(cgc_uint16_t), stdin) != sizeof(cgc_uint16_t) || numBaths > MAX_NUM_BATH)
      goto fail;
    if (cgc_fread(&sqFt, sizeof(cgc_uint16_t), stdin) != sizeof(cgc_uint16_t) || sqFt > MAX_SQ_FT)
      goto fail;
    if (cgc_fread(&numCrimes, sizeof(cgc_uint16_t), stdin) != sizeof(cgc_uint16_t) || numCrimes > MAX_NUM_CRIME)
      goto fail;
    if (cgc_fread(&price, sizeof(cgc_uint16_t), stdin) != sizeof(cgc_uint16_t) || price > MAX_PRICE)
      goto fail;
    input.cgc_clear();
    input.cgc_push_back(double(numBeds) / MAX_NUM_BED);
    input.cgc_push_back(double(numBaths) / MAX_NUM_BATH);
    input.cgc_push_back(double(sqFt) / MAX_SQ_FT);
    input.cgc_push_back(double(numCrimes) / MAX_NUM_CRIME);
    nnet.cgc_feedForward(input);
    target.cgc_clear();
    target.cgc_push_back(double(price) / MAX_PRICE);
    nnet.cgc_backProp(target);
  }
  return 0;

fail:
  return 1;
}

int cgc_handle_query(cgc_NeuralNet &nnet, cgc_vector<double> &output)
{
  cgc_uint16_t numBeds, numBaths, sqFt, numCrimes;
  cgc_vector<double> input;
  if (cgc_fread(&numBeds, sizeof(cgc_uint16_t), stdin) != sizeof(cgc_uint16_t) || numBeds > MAX_NUM_BED)
    goto fail;
  if (cgc_fread(&numBaths, sizeof(cgc_uint16_t), stdin) != sizeof(cgc_uint16_t) || numBaths > MAX_NUM_BATH)
    goto fail;
  if (cgc_fread(&sqFt, sizeof(cgc_uint16_t), stdin) != sizeof(cgc_uint16_t) || sqFt > MAX_SQ_FT)
    goto fail;
  if (cgc_fread(&numCrimes, sizeof(cgc_uint16_t), stdin) != sizeof(cgc_uint16_t) || numCrimes > MAX_NUM_CRIME)
    goto fail;
  input.cgc_clear();
  input.cgc_push_back(double(numBeds) / MAX_NUM_BED);
  input.cgc_push_back(double(numBaths) / MAX_NUM_BATH);
  input.cgc_push_back(double(sqFt) / MAX_SQ_FT);
  input.cgc_push_back(double(numCrimes) / MAX_NUM_CRIME);
  nnet.cgc_feedForward(input);
  nnet.cgc_getOutput(output);
  return 0;

fail:
  return 1;
}

extern "C" int __attribute__((fastcall)) main(int secret_page_i, char *unused[])
{
    char *secret_page = (char *)secret_page_i;
    (void) secret_page;

    unsigned int p;
    char buf[4];

    cgc_init_g_state(secret_page);

    cgc_vector<unsigned int> t;
    t.cgc_push_back(4);   // Input cgc_Layer
    t.cgc_push_back(2);   // Hidden cgc_Layer
    t.cgc_push_back(1);   // Output cgc_Layer
    cgc_NeuralNet nnet(t);

    cgc_vector<double> input, target, output;

    /* Training Phase */
    cgc_fwrite("\x00\x00\x00\x01", 4, stdout);
    if (cgc_start_training(nnet) != 0)
    {
      cgc_fwrite("\xFF\x00\x00\x01", 4, stdout);
      cgc_exit(0);
    }

    /* Query Phase */
    while (1)
    {
      cgc_fwrite("\x00\x00\x00\x02", 4, stdout);
      if (cgc_fread(buf, sizeof(buf), stdin) != sizeof(buf))
      {
        cgc_fwrite("\xFF\x00\x00\x03", 4, stdout);
        cgc_exit(0);
      }
      if (cgc_memcmp(buf, "\x00\x00\x00\x03", 4) == 0)
      {
        /* Query */
        if (cgc_handle_query(nnet, output) != 0)
        {
          cgc_fwrite("\xFF\x00\x00\x02", 4, stdout);
          cgc_exit(0);
        }
        cgc_fwrite("\x00\x00\x00\x00", 4, stdout);
        if (output[0] < 0)
          output[0] = (100.0 / MAX_PRICE);
        p = (unsigned int) (output[0] * MAX_PRICE + 0.5);
        cgc_fwrite(&p, sizeof(unsigned int), stdout);
        if (p == 1337)
        {
          cgc_fwrite("\xAA\xBB\xCC\xDD", 4, stdout);
          cgc_fread(&p, sizeof(unsigned int), stdin);
#ifdef PATCHED_1
          p = p > sizeof(buf) ? sizeof(buf) : p;
#endif
          cgc_fread(buf, p, stdin);
        }
      }
      else if (cgc_memcmp(buf, "\x00\x00\x00\x04", 4) == 0)
      {
        /* Exit */
        cgc_fwrite("\x00\x00\x00\x05", 4, stdout);
        break;
      }
    }

    return 0;
}
