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

#ifndef SADFACE_H
#define SADFACE_H

#include "dict.h"

typedef enum {
  SAD_VAR_STR,
  SAD_VAR_INT,
  SAD_VAR_FLOAT,
  SAD_VAR_BOOL
} cgc_sad_var_type_t;

typedef struct {
  cgc_sad_var_type_t type;
  union {
    char *s;
    int i;
    float f;
    unsigned char b;
  } value;
} cgc_sad_var_t;

typedef struct {
  const char *open_face;
  const char *close_face;
} cgc_faces_t;

typedef struct {
  cgc_faces_t faces;
  char *content;
  size_t content_len;
  size_t idx;
  cgc_dict_t **vars;
} cgc_sadface_ctx_t;

typedef enum {
  SAD_NODE_ROOT,
  SAD_NODE_NORMAL,
  SAD_NODE_VAR,
  SAD_NODE_SECTION
} cgc_sad_node_type_t;

typedef struct sad_node {
  cgc_sad_node_type_t type;
  struct sad_node *next;
  size_t idx;
  size_t len;
  union {
    cgc_sad_var_t *var;
    struct sad_node *child;
  };
} cgc_sad_node_t;

char* cgc_sadface_var2str(cgc_sad_var_t *var);
int cgc_sadface_init(cgc_sadface_ctx_t **ctx, cgc_faces_t *faces, char *content, cgc_dict_t **vars);
int cgc_sadface_render(cgc_sadface_ctx_t *ctx, char *out, size_t *out_len);
void cgc_sadface_destroy(cgc_sadface_ctx_t *ctx);

#endif
