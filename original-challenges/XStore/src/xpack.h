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

#ifndef XPACK_H
#define XPACK_H

#include <stdlib.h>
#include <stdint.h>
#include <string.h>

typedef enum {
    XPK_POSITIVE_FIXINT = 0x00,
    XPK_NEGATIVE_FIXINT = 0xE0,
    XPK_FIXSTRING       = 0x80,
    XPK_NONE            = 0xA0,
    XPK_FALSE           = 0xA1,
    XPK_TRUE            = 0xA2,
    XPK_BYTES           = 0xA3,
    XPK_ARRAY           = 0xA4,
    XPK_MAP             = 0xA5,
    XPK_NIL             = 0xA6,
    XPK_INT8            = 0xB0,
    XPK_INT16           = 0xB1,
    XPK_INT32           = 0xB2,
    XPK_INT64           = 0xB3,
    XPK_UINT8           = 0xC0,
    XPK_UINT16          = 0xC1,
    XPK_UINT32          = 0xC2,
    XPK_UINT64          = 0xC3,
    XPK_STRING          = 0xD0
} __attribute__((packed)) cgc_xpk_type;

typedef enum {
    XPK_ERR_NONE            = 0x00,
    XPK_ERR_DATA_LONG       = 0x01,
    XPK_ERR_ARRAY_LONG      = 0x02,
    XPK_ERR_MAP_LONG        = 0x03,
    XPK_ERR_INVALID_TYPE    = 0x04,
    XPK_ERR_INTERNAL        = 0x05,
} __attribute__((packed)) cgc_xpk_err;

typedef union {
    cgc_uint8_t b;
    cgc_int8_t i8;
    cgc_int16_t i16;
    cgc_int32_t i32;
    cgc_int64_t i64;
    cgc_uint8_t u8;
    cgc_uint16_t u16;
    cgc_uint32_t u32;
    cgc_uint64_t u64;
    cgc_size_t len;
} cgc_xpk_data_t;

typedef struct {
    cgc_xpk_type type;
    cgc_xpk_data_t data;
} cgc_xpk_obj_t;

typedef struct {
    char *buf;
    cgc_size_t idx;
    cgc_size_t len;
} cgc_xpk_ctx_t;

/* Utility */
cgc_xpk_ctx_t* cgc_xpk_init(cgc_size_t size);
void cgc_xpk_reset(cgc_xpk_ctx_t *ctx, cgc_size_t size);
cgc_xpk_err cgc_xpk_write(cgc_xpk_ctx_t *ctx, const char *in, cgc_size_t len);
cgc_xpk_err cgc_xpk_read(cgc_xpk_ctx_t *ctx, char *out, cgc_size_t len);
cgc_xpk_err cgc_xpk_peek(cgc_xpk_ctx_t *ctx, char *out, cgc_size_t len);
cgc_xpk_err cgc_xpk_read_obj(cgc_xpk_ctx_t *ctx, cgc_xpk_obj_t *o);
cgc_xpk_err cgc_xpk_next_type(cgc_xpk_ctx_t *ctx, cgc_xpk_type *t);

/* Pack */
cgc_xpk_err cgc_xpk_pack_int(cgc_xpk_ctx_t *ctx, cgc_int64_t si);
cgc_xpk_err cgc_xpk_pack_uint(cgc_xpk_ctx_t *ctx, cgc_uint64_t ui);
cgc_xpk_err cgc_xpk_pack_none(cgc_xpk_ctx_t *ctx);
cgc_xpk_err cgc_xpk_pack_nil(cgc_xpk_ctx_t *ctx);
cgc_xpk_err cgc_xpk_pack_bool(cgc_xpk_ctx_t *ctx, cgc_uint8_t b);
cgc_xpk_err cgc_xpk_pack_str(cgc_xpk_ctx_t *ctx, const char *str);
cgc_xpk_err cgc_xpk_pack_bytes(cgc_xpk_ctx_t *ctx, const char *bytes, cgc_size_t len);
cgc_xpk_err cgc_xpk_pack_array(cgc_xpk_ctx_t *ctx, cgc_size_t len);
cgc_xpk_err cgc_xpk_pack_map(cgc_xpk_ctx_t *ctx, cgc_size_t len);

/* Unpack */
cgc_xpk_err cgc_xpk_unpack_int(cgc_xpk_ctx_t *ctx, cgc_uint64_t *i, cgc_uint8_t *sign);
cgc_xpk_err cgc_xpk_unpack_nil(cgc_xpk_ctx_t *ctx);
cgc_xpk_err cgc_xpk_unpack_bool(cgc_xpk_ctx_t *ctx, cgc_uint8_t *b);
cgc_xpk_err cgc_xpk_unpack_str(cgc_xpk_ctx_t *ctx, cgc_size_t *len);
cgc_xpk_err cgc_xpk_unpack_bytes(cgc_xpk_ctx_t *ctx, cgc_size_t *len);
cgc_xpk_err cgc_xpk_unpack_array(cgc_xpk_ctx_t *ctx, cgc_size_t *len);
cgc_xpk_err cgc_xpk_unpack_map(cgc_xpk_ctx_t *ctx, cgc_size_t *len);

#endif
