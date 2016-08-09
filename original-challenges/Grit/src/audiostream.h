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
#pragma once
#include <cstdint.h>
#include "gain.h"

#define SAMPLE_RATE 8000

class cgc_AudioStream
{
public:
    ~cgc_AudioStream();
    static cgc_AudioStream *cgc_fromSilence(unsigned int samples);
    static cgc_AudioStream *cgc_fromSineWave(unsigned int samples, unsigned int hz);
    static cgc_AudioStream *cgc_fromSquareWave(unsigned int samples, unsigned int hz);

    inline unsigned int cgc_getLength() const
    {
        return length;
    }
    inline cgc_int32_t cgc_getSample(unsigned int i) const
    {
        if (i < length)
            return samples[i];
        return 0;
    }

    void cgc_setLength(unsigned int length);
    inline void cgc_setSample(unsigned int i, cgc_int32_t sample)
    {
        if (i >= length)
            cgc_setLength(i+1);
        samples[i] = sample;
    }
    void cgc_addSilence(unsigned int length);
    void cgc_mix(const cgc_AudioStream &src, cgc_Gain gain);
private:
    cgc_AudioStream(unsigned int length);
    void cgc_enlarge(unsigned int length);

    unsigned int length;
    unsigned int size;
    cgc_int32_t *samples;
};
