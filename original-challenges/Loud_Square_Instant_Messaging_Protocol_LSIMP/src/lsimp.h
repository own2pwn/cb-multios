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

#ifndef LSIMP_H
#define LSIMP_H

#include <libcgc.h>
#include <string.h>

#define MAX_KEY_LEN   128
#define MAX_DATA_LEN  512
#define MAX_TEXT_LEN  1024
#define MAX_SEQ_NUM   500

enum lsimp_msg_type_t {
  LMT_HELO = 0x4f4c4548,
  LMT_KEYX = 0x5859454b,
  LMT_DATA = 0x41544144,
  LMT_TEXT = 0x54584554
};

typedef struct lsimp_helo {
  char version;
  char secure_mode;
  int ttl;
} cgc_lsimp_helo_t;

typedef struct lsimp_keyx {
  char *key;
  unsigned int key_len;
  char option;
} cgc_lsimp_keyx_t;

typedef struct lsimp_data {
  int seq;
  char *data;
  unsigned int data_len;
} cgc_lsimp_data_t;

typedef struct lsimp_text {
  char *msg;
  unsigned int msg_len;
} cgc_lsimp_text_t;

typedef struct lsimp_msg {
  struct lsimp_msg *next;
  enum lsimp_msg_type_t type;
  union {
    cgc_lsimp_helo_t helo;
    cgc_lsimp_keyx_t keyx;
    cgc_lsimp_data_t data;
    cgc_lsimp_text_t text;
  };
} cgc_lsimp_msg_t;

cgc_lsimp_msg_t* cgc_parse_msg(char *buf, unsigned int buf_len);
int cgc_decode_data(cgc_lsimp_keyx_t *keyx, cgc_lsimp_data_t* data);

#endif
