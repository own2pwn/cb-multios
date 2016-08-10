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

#include "xpack.h"

#define MAX_BUF_LEN (4 * 1024 * 1024)

cgc_xpk_ctx_t* cgc_xpk_init(size_t size)
{
    cgc_xpk_ctx_t *ctx = (cgc_xpk_ctx_t *) cgc_malloc(sizeof(cgc_xpk_ctx_t));
    if (size == 0)
        size = 1024;
    ctx->buf = cgc_malloc(sizeof(char) * size);
    ctx->idx = 0;
    ctx->len = size;
    return ctx;
}

int cgc__xpk_resize(cgc_xpk_ctx_t *ctx, size_t size)
{
    if (size <= ctx->len)
        return 0;
    if (size <= MAX_BUF_LEN)
    {
        ctx->buf = cgc_realloc(ctx->buf, size);
        ctx->len = size;
        return 0;
    }
    return -1;
}

void cgc_xpk_reset(cgc_xpk_ctx_t *ctx, size_t size)
{
    if (cgc__xpk_resize(ctx, size) == 0)
    {
        cgc_memset(ctx->buf, 0, ctx->len);
        ctx->idx = 0;
    }
}

/* Helper to swap endianness */
cgc_int16_t cgc_le2be16_s(cgc_int16_t n)
{
    return (n << 8) | ((n >> 8) & 0xFF);
}
cgc_uint16_t cgc_le2be16_u(cgc_uint16_t n)
{
    return (n << 8) | (n >> 8);
}
cgc_int32_t cgc_le2be32_s(cgc_int32_t n)
{
    n = ((n << 8) & 0xFF00FF00) | ((n >> 8) & 0x00FF00FF);
    return (n << 16) | ((n >> 16) & 0xFFFF);
}
cgc_uint32_t cgc_le2be32_u(cgc_uint32_t n)
{
    n = ((n << 8) & 0xFF00FF00) | ((n >> 8) & 0x00FF00FF);
    return (n << 16) | (n >> 16);
}
cgc_int64_t cgc_le2be64_s(cgc_int64_t n)
{
    n = ((n << 8) & 0xFF00FF00FF00FF00) | ((n >> 8) & 0x00FF00FF00FF00FF);
    n = ((n << 16) & 0xFFFF0000FFFF0000) | ((n >> 16) & 0x0000FFFF0000FFFF);
    return (n << 32) | ((n >> 32) & 0xFFFFFFFF);
}
cgc_uint64_t cgc_le2be64_u(cgc_uint64_t n)
{
    n = ((n << 8) & 0xFF00FF00FF00FF00) | ((n >> 8) & 0x00FF00FF00FF00FF);
    n = ((n << 16) & 0xFFFF0000FFFF0000) | ((n >> 16) & 0x0000FFFF0000FFFF);
    return (n << 32) | (n >> 32);
}

cgc_xpk_err cgc_xpk_pack_fixint(cgc_xpk_ctx_t *ctx, cgc_uint8_t b)
{
    return cgc_xpk_write(ctx, (const char*) &b, sizeof(cgc_uint8_t));
}

cgc_xpk_err cgc_xpk_write_marker(cgc_xpk_ctx_t *ctx, cgc_xpk_type t)
{
    return cgc_xpk_write(ctx, (const char*) &t, sizeof(cgc_xpk_type));
}

cgc_xpk_err cgc_xpk_read_marker(cgc_xpk_ctx_t *ctx, cgc_xpk_type *t)
{
    cgc_xpk_err err;
    if ((err = cgc_xpk_read(ctx, (char *) t, sizeof(cgc_uint8_t))) != XPK_ERR_NONE) return err;
    return XPK_ERR_NONE;
}

cgc_xpk_err cgc_xpk_read_obj(cgc_xpk_ctx_t *ctx, cgc_xpk_obj_t *o)
{
    cgc_xpk_err err;
    cgc_uint8_t marker = XPK_NONE;

    if ((err = cgc_xpk_read_marker(ctx, (cgc_xpk_type *) &marker)) != XPK_ERR_NONE) return err;
    o->type = marker;
    switch (marker)
    {
        case XPK_FALSE:
            o->data.b = 0;
            break;
        case XPK_TRUE:
            o->data.b = 1;
            break;
        case XPK_BYTES:
        case XPK_STRING:
            o->data.len = 0;
            if ((err = cgc_xpk_read(ctx, (char *) &o->data.len, sizeof(cgc_uint16_t))) != XPK_ERR_NONE) return err;
            o->data.len = cgc_le2be16_u(o->data.len);
            break;
        case XPK_ARRAY:
        case XPK_MAP:
            o->data.len = 0;
            if ((err = cgc_xpk_read(ctx, (char *) &o->data.len, sizeof(cgc_uint8_t))) != XPK_ERR_NONE) return err;
            break;
        case XPK_INT8:
            if ((err = cgc_xpk_read(ctx, (char *) &o->data.i8, sizeof(cgc_int8_t))) != XPK_ERR_NONE) return err;
            break;
        case XPK_INT16:
            if ((err = cgc_xpk_read(ctx, (char *) &o->data.i16, sizeof(cgc_int16_t))) != XPK_ERR_NONE) return err;
            o->data.i16 = cgc_le2be16_s(o->data.i16);
            break;
        case XPK_INT32:
            if ((err = cgc_xpk_read(ctx, (char *) &o->data.i32, sizeof(cgc_int32_t))) != XPK_ERR_NONE) return err;
            o->data.i32 = cgc_le2be32_s(o->data.i32);
            break;
        case XPK_INT64:
            if ((err = cgc_xpk_read(ctx, (char *) &o->data.i64, sizeof(cgc_int64_t))) != XPK_ERR_NONE) return err;
            o->data.i64 = cgc_le2be64_s(o->data.i64);
            break;
        case XPK_UINT8:
            if ((err = cgc_xpk_read(ctx, (char *) &o->data.u8, sizeof(cgc_uint8_t))) != XPK_ERR_NONE) return err;
            break;
        case XPK_UINT16:
            if ((err = cgc_xpk_read(ctx, (char *) &o->data.u16, sizeof(cgc_uint16_t))) != XPK_ERR_NONE) return err;
            o->data.u16 = cgc_le2be16_u(o->data.u16);
            break;
        case XPK_UINT32:
            if ((err = cgc_xpk_read(ctx, (char *) &o->data.u32, sizeof(cgc_uint32_t))) != XPK_ERR_NONE) return err;
            o->data.u32 = cgc_le2be32_u(o->data.u32);
            break;
        case XPK_UINT64:
            if ((err = cgc_xpk_read(ctx, (char *) &o->data.u64, sizeof(cgc_uint64_t))) != XPK_ERR_NONE) return err;
            o->data.u64 = cgc_le2be64_u(o->data.u64);
            break;
        case XPK_NIL:
            break;
        default:
            if (marker < (1 << 7)) { o->type = XPK_POSITIVE_FIXINT; o->data.u8 = marker; }
            else if (marker < 0xA0) { o->type = XPK_FIXSTRING; o->data.len = marker & 0x1F; }
            else if (marker >= 0xE0) { o->type = XPK_NEGATIVE_FIXINT; o->data.i8 = marker; }
            else { o->type = XPK_NONE; o->data.u64 = 0; }
    }

    return XPK_ERR_NONE;
}

cgc_xpk_err cgc_xpk_next_type(cgc_xpk_ctx_t *ctx, cgc_xpk_type *t)
{
    cgc_xpk_err err;
    cgc_uint8_t marker;
    if ((err = cgc_xpk_peek(ctx, (char *) &marker, sizeof(cgc_uint8_t))) != XPK_ERR_NONE) return err;
    switch (marker)
    {
        case XPK_FALSE:
        case XPK_TRUE:
        case XPK_BYTES:
        case XPK_STRING:
        case XPK_ARRAY:
        case XPK_MAP:
        case XPK_INT8:
        case XPK_INT16:
        case XPK_INT32:
        case XPK_INT64:
        case XPK_UINT8:
        case XPK_UINT16:
        case XPK_UINT32:
        case XPK_UINT64:
        case XPK_NIL:
            *t = marker;
            break;
        default:
            if (marker < (1 << 7))
                *t = XPK_POSITIVE_FIXINT;
            else if (marker < 0xA0)
                *t = XPK_FIXSTRING;
            else if (marker >= 0xE0)
                *t = XPK_NEGATIVE_FIXINT;
            else
                return XPK_ERR_INVALID_TYPE;
    }
    return XPK_ERR_NONE;
}

cgc_xpk_err cgc_xpk_write(cgc_xpk_ctx_t *ctx, const char *in, size_t len)
{
    if (ctx->idx + len <= ctx->len || cgc__xpk_resize(ctx, ctx->len * 2) == 0)
    {
        cgc_memcpy(&ctx->buf[ctx->idx], in, len);
        ctx->idx += len;
        return XPK_ERR_NONE;
    }
    return XPK_ERR_INTERNAL;
}

cgc_xpk_err cgc_xpk_read(cgc_xpk_ctx_t *ctx, char *out, size_t len)
{
    if (ctx->idx + len <= ctx->len)
    {
        cgc_memcpy(out, &ctx->buf[ctx->idx], len);
        ctx->idx += len;
        return XPK_ERR_NONE;
    }
    return XPK_ERR_INTERNAL;
}

cgc_xpk_err cgc_xpk_peek(cgc_xpk_ctx_t *ctx, char *out, size_t len)
{
    if (ctx->idx + len > ctx->len)
        return XPK_ERR_INTERNAL;
    cgc_memcpy(out, &ctx->buf[ctx->idx], len);
    return XPK_ERR_NONE;
}

cgc_xpk_err cgc_xpk_pack_int(cgc_xpk_ctx_t *ctx, cgc_int64_t si)
{
    cgc_xpk_err err = XPK_ERR_NONE;
    if (si >= 0)
        return cgc_xpk_pack_uint(ctx, si);
    if (si >= -(1 << 5))
        return cgc_xpk_pack_fixint(ctx, si);
    else if (si >= -(1 << 7))
    {
        if ((err = cgc_xpk_write_marker(ctx, XPK_INT8)) != XPK_ERR_NONE) return err;
        if ((err = cgc_xpk_write(ctx, (const char *) &si, sizeof(cgc_int8_t))) != XPK_ERR_NONE) return err;
    }
    else if (si >= -(1 << 15))
    {
        if ((err = cgc_xpk_write_marker(ctx, XPK_INT16)) != XPK_ERR_NONE) return err;
        si = cgc_le2be16_s(si);
        if ((err = cgc_xpk_write(ctx, (const char *) &si, sizeof(cgc_int16_t))) != XPK_ERR_NONE) return err;
    }
    else if (si >= -(1 << 31))
    {
        if ((err = cgc_xpk_write_marker(ctx, XPK_INT32)) != XPK_ERR_NONE) return err;
        si = cgc_le2be32_s(si);
        if ((err = cgc_xpk_write(ctx, (const char *) &si, sizeof(cgc_int32_t))) != XPK_ERR_NONE) return err;
    }
    else
    {
        if ((err = cgc_xpk_write_marker(ctx, XPK_INT64)) != XPK_ERR_NONE) return err;
        si = cgc_le2be64_s(si);
        if ((err = cgc_xpk_write(ctx, (const char *) &si, sizeof(cgc_int64_t))) != XPK_ERR_NONE) return err;
    }
    return err;
}

cgc_xpk_err cgc_xpk_pack_uint(cgc_xpk_ctx_t *ctx, cgc_uint64_t ui)
{
    cgc_xpk_err err = XPK_ERR_NONE;
    if (ui < (1 << 7))
        return cgc_xpk_pack_fixint(ctx, ui);
    else if (ui < (1 << 8))
    {
        if ((err = cgc_xpk_write_marker(ctx, XPK_UINT8)) != XPK_ERR_NONE) return err;
        if ((err = cgc_xpk_write(ctx, (const char *) &ui, sizeof(cgc_uint8_t))) != XPK_ERR_NONE) return err;
    }
    else if (ui < (1 << 16))
    {
        if ((err = cgc_xpk_write_marker(ctx, XPK_UINT16)) != XPK_ERR_NONE) return err;
        ui = cgc_le2be16_u(ui);
        if ((err = cgc_xpk_write(ctx, (const char *) &ui, sizeof(cgc_uint16_t))) != XPK_ERR_NONE) return err;
    }
    else if (ui <= 0xFFFFFFFF)
    {
        if ((err = cgc_xpk_write_marker(ctx, XPK_UINT32)) != XPK_ERR_NONE) return err;
        ui = cgc_le2be32_u(ui);
        if ((err = cgc_xpk_write(ctx, (const char *) &ui, sizeof(cgc_uint32_t))) != XPK_ERR_NONE) return err;
    }
    else
    {
        if ((err = cgc_xpk_write_marker(ctx, XPK_UINT64)) != XPK_ERR_NONE) return err;
        ui = cgc_le2be64_u(ui);
        if ((err = cgc_xpk_write(ctx, (const char *) &ui, sizeof(cgc_uint64_t))) != XPK_ERR_NONE) return err;
    }
    return err;
}

cgc_xpk_err cgc_xpk_pack_none(cgc_xpk_ctx_t *ctx)
{
    return cgc_xpk_write_marker(ctx, XPK_NONE);
}

cgc_xpk_err cgc_xpk_pack_nil(cgc_xpk_ctx_t *ctx)
{
    return cgc_xpk_write_marker(ctx, XPK_NIL);
}

cgc_xpk_err cgc_xpk_pack_bool(cgc_xpk_ctx_t *ctx, cgc_uint8_t b)
{
    return b ? cgc_xpk_write_marker(ctx, XPK_TRUE) : cgc_xpk_write_marker(ctx, XPK_FALSE);
}

cgc_xpk_err cgc_xpk_pack_str(cgc_xpk_ctx_t *ctx, const char *str)
{
    cgc_xpk_err err = XPK_ERR_NONE;

    size_t len_b, len = cgc_strlen(str);
    if (len <= 0x1F)
    {
        cgc_uint8_t val = XPK_FIXSTRING | len;
        if ((err = cgc_xpk_write(ctx, (const char *) &val, sizeof(cgc_uint8_t))) != XPK_ERR_NONE) return err;
        if (len > 0 && (err = cgc_xpk_write(ctx, str, len)) != XPK_ERR_NONE) return err;
    }
    else if (len < (1 << 16))
    {
        if ((err = cgc_xpk_write_marker(ctx, XPK_STRING)) != XPK_ERR_NONE) return err;
        len_b = cgc_le2be16_u(len);
        if ((err = cgc_xpk_write(ctx, (const char *) &len_b, sizeof(cgc_uint16_t))) != XPK_ERR_NONE) return err;
        if (len > 0 && (err = cgc_xpk_write(ctx, str, len)) != XPK_ERR_NONE) return err;
    }
    else
        err = XPK_ERR_DATA_LONG;

    return err;
}

cgc_xpk_err cgc_xpk_pack_bytes(cgc_xpk_ctx_t *ctx, const char *bytes, size_t len)
{
    cgc_xpk_err err = XPK_ERR_NONE;

    size_t len_b;
    if (len < (1 << 16))
    {
        if ((err = cgc_xpk_write_marker(ctx, XPK_BYTES)) != XPK_ERR_NONE) return err;
        len_b = cgc_le2be16_u(len);
        if ((err = cgc_xpk_write(ctx, (const char *) &len_b, sizeof(cgc_uint16_t))) != XPK_ERR_NONE) return err;
        if (len > 0 && (err = cgc_xpk_write(ctx, bytes, len)) != XPK_ERR_NONE) return err;
    }
    else
        err = XPK_ERR_DATA_LONG;

    return err;
}

cgc_xpk_err cgc_xpk_pack_array(cgc_xpk_ctx_t *ctx, size_t len)
{
    cgc_xpk_err err = XPK_ERR_NONE;

    if (len < (1 << 8))
    {
        if ((err = cgc_xpk_write_marker(ctx, XPK_ARRAY)) != XPK_ERR_NONE) return err;
        if ((err = cgc_xpk_write(ctx, (const char *) &len, sizeof(cgc_uint8_t))) != XPK_ERR_NONE) return err;
    }
    else
        err = XPK_ERR_ARRAY_LONG;

    return err;
}

cgc_xpk_err cgc_xpk_pack_map(cgc_xpk_ctx_t *ctx, size_t len)
{
    cgc_xpk_err err = XPK_ERR_NONE;

    if (len < (1 << 8))
    {
        if ((err = cgc_xpk_write_marker(ctx, XPK_MAP)) != XPK_ERR_NONE) return err;
        if ((err = cgc_xpk_write(ctx, (const char *) &len, sizeof(cgc_uint8_t))) != XPK_ERR_NONE) return err;
    }
    else
        err = XPK_ERR_MAP_LONG;

    return err;
}


cgc_xpk_err cgc_xpk_unpack_int(cgc_xpk_ctx_t *ctx, cgc_uint64_t *i, cgc_uint8_t *sign)
{
    cgc_xpk_err err;
    cgc_xpk_obj_t o;

    if ((err = cgc_xpk_read_obj(ctx, &o)) != XPK_ERR_NONE) return err;

    switch (o.type)
    {
        case XPK_NEGATIVE_FIXINT:
        case XPK_INT8:
            *i = o.data.i8;
            *sign = 1;
            break;
        case XPK_INT16:
            *i = o.data.i16;
            *sign = 1;
            break;
        case XPK_INT32:
            *i = o.data.i32;
            *sign = 1;
            break;
        case XPK_INT64:
            *i = o.data.i64;
            *sign = 1;
            break;
        case XPK_POSITIVE_FIXINT:
        case XPK_UINT8:
            *i = o.data.u8;
            *sign = 0;
            break;
        case XPK_UINT16:
            *i = o.data.u16;
            *sign = 0;
            break;
        case XPK_UINT32:
            *i = o.data.u32;
            *sign = 0;
            break;
        case XPK_UINT64:
            *i = o.data.u64;
            *sign = 0;
            break;
        default:
            return XPK_ERR_INVALID_TYPE;
    }

    return XPK_ERR_NONE;
}

cgc_xpk_err cgc_xpk_unpack_nil(cgc_xpk_ctx_t *ctx)
{
    cgc_xpk_err err;
    cgc_xpk_obj_t o;

    if ((err = cgc_xpk_read_obj(ctx, &o)) != XPK_ERR_NONE) return err;
    if (o.type != XPK_NIL)
        return XPK_ERR_INVALID_TYPE;
    return XPK_ERR_NONE;
}

cgc_xpk_err cgc_xpk_unpack_bool(cgc_xpk_ctx_t *ctx, cgc_uint8_t *b)
{
    cgc_xpk_err err;
    cgc_xpk_obj_t o;

    if ((err = cgc_xpk_read_obj(ctx, &o)) != XPK_ERR_NONE) return err;
    if (o.type != XPK_TRUE && o.type != XPK_FALSE)
        return XPK_ERR_INVALID_TYPE;
    *b = o.data.b;
    return XPK_ERR_NONE;
}

cgc_xpk_err cgc_xpk_unpack_str(cgc_xpk_ctx_t *ctx, size_t *len)
{
    cgc_xpk_err err;
    cgc_xpk_obj_t o;

    if ((err = cgc_xpk_read_obj(ctx, &o)) != XPK_ERR_NONE) return err;
    if (o.type != XPK_FIXSTRING && o.type != XPK_STRING)
        return XPK_ERR_INVALID_TYPE;
    *len = o.data.len;
    return XPK_ERR_NONE;
}

cgc_xpk_err cgc_xpk_unpack_bytes(cgc_xpk_ctx_t *ctx, size_t *len)
{
    cgc_xpk_err err;
    cgc_xpk_obj_t o;

    if ((err = cgc_xpk_read_obj(ctx, &o)) != XPK_ERR_NONE) return err;
    if (o.type != XPK_BYTES)
        return XPK_ERR_INVALID_TYPE;
    *len = o.data.len;
    return XPK_ERR_NONE;
}

cgc_xpk_err cgc_xpk_unpack_array(cgc_xpk_ctx_t *ctx, size_t *len)
{
    cgc_xpk_err err;
    cgc_xpk_obj_t o;

    if ((err = cgc_xpk_read_obj(ctx, &o)) != XPK_ERR_NONE) return err;
    if (o.type != XPK_ARRAY)
        return XPK_ERR_INVALID_TYPE;
    *len = o.data.len;
    return XPK_ERR_NONE;
}

cgc_xpk_err cgc_xpk_unpack_map(cgc_xpk_ctx_t *ctx, size_t *len)
{
    cgc_xpk_err err;
    cgc_xpk_obj_t o;

    if ((err = cgc_xpk_read_obj(ctx, &o)) != XPK_ERR_NONE) return err;
    if (o.type != XPK_MAP)
        return XPK_ERR_INVALID_TYPE;
    *len = o.data.len;
    return XPK_ERR_NONE;
}