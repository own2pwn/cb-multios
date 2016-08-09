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
#include "compeight.h"

unsigned int cgc_CompressionEight::cgc_getId() const
{
    return COMPRESSION_8;
}

unsigned int cgc_CompressionEight::cgc_getMaxBufferSize(const cgc_AudioTrack &track) const
{
    return (track.cgc_getStereo() ? 2 : 1) * track.cgc_getLength() * sizeof(cgc_int8_t);
}

static cgc_uint8_t cgc_encodeSample(cgc_int32_t sample)
{
    cgc_uint8_t value;
    sample >>= 18;
    if (sample >= 0)
    {
        value = 0x80;
    }
    else
    {
        value = 0x00;
        sample = -sample - 1;
    }
    if (sample <= 30)
        value |= 0x70 + (30 - sample) / 2;
    else if (sample <= 94)
        value |= 0x60 + (94 - sample) / 4;
    else if (sample <= 222)
        value |= 0x50 + (222 - sample) / 8;
    else if (sample <= 478)
        value |= 0x40 + (478 - sample) / 16;
    else if (sample <= 990)
        value |= 0x30 + (990 - sample) / 32;
    else if (sample <= 2014)
        value |= 0x20 + (2014 - sample) / 64;
    else if (sample <= 4062)
        value |= 0x10 + (4062 - sample) / 128;
    else if (sample <= 8190)
        value |= 0x00 + (8190 - sample) / 258;

    return value;
}

static cgc_int32_t cgc_decodeSample(cgc_uint8_t value)
{
    cgc_int32_t sample;
    int interval = value & 0xf;

    if ((value & 0x70) == 0x00)
        sample = 8190 - 258 * interval;
    else if ((value & 0x70) == 0x10)
        sample = 4062 - 128 * interval;
    else if ((value & 0x70) == 0x20)
        sample = 2014 - 64 * interval;
    else if ((value & 0x70) == 0x30)
        sample = 990 - 32 * interval;
    else if ((value & 0x70) == 0x40)
        sample = 478 - 16 * interval;
    else if ((value & 0x70) == 0x50)
        sample = 222 - 8 * interval;
    else if ((value & 0x70) == 0x60)
        sample = 94 - 4 * interval;
    else if ((value & 0x70) == 0x70)
        sample = 30 - 2 * interval;

    if (value & 0x80)
        return sample << 18;
    else
        return (-sample - 1) << 18;
}

void cgc_CompressionEight::cgc_compress(const cgc_AudioTrack &track, cgc_uint8_t *dest) const
{
    for (unsigned int i = 0; i < track.cgc_getLength(); i++)
    {
        cgc_int32_t sample;
        sample = track.cgc_getChannel(0)->cgc_getSample(i);
        *dest++ = cgc_encodeSample(sample);
        if (track.cgc_getStereo())
        {
            sample = track.cgc_getChannel(1)->cgc_getSample(i);
            *dest++ = cgc_encodeSample(sample);
        }
    }
}

void cgc_CompressionEight::cgc_decompress(cgc_AudioTrack &track, cgc_uint8_t *src) const
{
    for (unsigned int i = 0; i < track.cgc_getLength(); i++)
    {
        track.cgc_getChannel(0)->cgc_setSample(i, cgc_decodeSample(*src++));
        if (track.cgc_getStereo())
        {
            track.cgc_getChannel(1)->cgc_setSample(i, cgc_decodeSample(*src++));
        }
    }
}
