/*

Author: Bryce Kerley <bk@cromulence.co>

Copyright (c) 2014-2015 Cromulence LLC

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

#include <libcgc.h>
#include "churn.h"

cgc_churn_item gyre[CHURN_GYRE_SIZE];
size_t gyre_pos = 0;

cgc_uint64  cgc_init_one(cgc_uint64,  cgc_uint64);
cgc_float64 cgc_init_two(cgc_float64, cgc_float64);
cgc_uint64  cgc_init_thr(cgc_uint64,  cgc_uint64);
cgc_float64 cgc_init_fou(cgc_float64, cgc_float64);

void cgc_churn_initialize(cgc_churn_item* start) {
  // "zedo cat"
  cgc_churn_item cursor = {0x7a65646f20636174};
  cgc_uint64 shifter = 5;

  for (cgc_uint32 i = 0; i < (CHURN_GYRE_SIZE / CHURN_INITIALIZER_COUNT); i++) {
    cgc_uint32 s = i * 4;
    cgc_churn_item ct;

    gyre[s + 0].i = cursor.i = cgc_init_one(cursor.i ^ s, start[0].i);
    ct = cursor;
    ct.i ^= s;
    gyre[s + 1].f = cursor.f = cgc_init_two(ct.f, start[1].f);
    gyre[s + 2].i = cursor.i = cgc_init_thr(cursor.i ^ s, start[2].i);
    ct = cursor;
    ct.i ^= s;
    gyre[s + 3].f = cursor.f = cgc_init_fou(ct.f, start[3].f);
  }

  gyre_pos = 0;
}



cgc_uint64 cgc_init_one(cgc_uint64 i, cgc_uint64 j) {
  i = i ^ (j >> 21);
  i = i ^ ((j << 37) & 0x6469652068617264); // die hard
  i = i ^ ((j >> 9) & 0x6372616e6b74776f); // cranktwo
  i = i ^ (j << 50);
  return i;
}

cgc_float64 cgc_init_two(cgc_float64 i, cgc_float64 j) {
  i = fabs(sin(i)) + fabs(cos(j));
  i *= 3 + cos(i);
  i += cos(j) + cos(i);
  i *= tan(j);
  return i;
}

cgc_uint64 cgc_init_thr(cgc_uint64 i, cgc_uint64 j) {
  i = i ^ (j << 16);
  i = i ^ ((j >> 44) & 0x6578706e64626c73); // expndbls
  i = i ^ ((j << 36) & 0x636f6d6d616e646f); // commando
  i = i ^ (j >> 10);
  return i;
}

cgc_float64 cgc_init_fou(cgc_float64 i, cgc_float64 j) {
  cgc_float64 k = fabs(cos(j) / sin(i));
  j -= sin(k);
  i += log2(fabs(k));
  k += i + j;
  i -= sin(fabs(k));
  return i;
}

cgc_uint8 cgc_churn_rand_uint8() {
  cgc_uint64 candidate = cgc_churn_rand_uint64();
  cgc_uint64 selector = 8 * (candidate & 7);
  return (cgc_uint8) ((candidate >> selector) & 0xff);
}

cgc_uint32 cgc_churn_rand_uint32() {
  cgc_uint64 candidate = cgc_churn_rand_uint64();
  if ((candidate & 1) == 1) {
    return (cgc_uint32) ((candidate >> 31) & 0x00000000ffffffff);
  } else {
    return (cgc_uint32) candidate & 0x00000000ffffffff;
  }
};

cgc_uint64 cgc_churn_rand_uint64() {
  return cgc_churn_rand_item().i;
}

cgc_float64 cgc_churn_rand_float64() {
  return cgc_churn_rand_item().f;
}

cgc_churn_item cgc_churn_rand_item() {
  cgc_churn_item candidate = gyre[gyre_pos];
  cgc_churn_item replacement;
  replacement.i = cgc_init_one(candidate.i, gyre_pos);
  replacement.f = cgc_init_fou(candidate.f, replacement.f);
  gyre[gyre_pos] = replacement;
  gyre_pos = (gyre_pos + 1) % CHURN_GYRE_SIZE;

  return candidate;
}
