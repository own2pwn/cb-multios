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
#include <cstdint.h>
#include "comp32.h"

unsigned int cgc_Compression32::cgc_getId() const
{
    return COMPRESSION_32;
}

unsigned int cgc_Compression32::cgc_getMaxBufferSize(const cgc_AudioTrack &track) const
{
    return (track.cgc_getStereo() ? 2 : 1) * track.cgc_getLength() * sizeof(cgc_int32_t);
}

void cgc_Compression32::cgc_compress(const cgc_AudioTrack &track, cgc_uint8_t *dest) const
{
    cgc_int32_t *dest32 = (cgc_int32_t *)dest;
    for (unsigned int i = 0; i < track.cgc_getLength(); i++)
    {
        // interleave channels
        *dest32++ = track.cgc_getChannel(0)->cgc_getSample(i);
        if (track.cgc_getStereo())
            *dest32++ = track.cgc_getChannel(1)->cgc_getSample(i);
    }
}

void cgc_Compression32::cgc_decompress(cgc_AudioTrack &track, cgc_uint8_t *src) const
{
    cgc_int32_t *src32 = (cgc_int32_t *)src;
    for (unsigned int i = 0; i < track.cgc_getLength(); i++)
    {
        // un-interleave channels
        track.cgc_getChannel(0)->cgc_setSample(i, *src32++);
        if (track.cgc_getStereo())
            track.cgc_getChannel(1)->cgc_setSample(i, *src32++);
    }
}
