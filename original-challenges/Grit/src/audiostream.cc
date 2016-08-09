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
#include <cmath.h>
#include <cstring.h>
#include "audiostream.h"

cgc_AudioStream::cgc_AudioStream(unsigned int length_) : length(length_), size(length_)
{
    samples = new cgc_int32_t[size];
}

cgc_AudioStream::~cgc_AudioStream()
{
    delete samples;
}

cgc_AudioStream *cgc_AudioStream::cgc_fromSilence(unsigned int samples)
{
    cgc_AudioStream *stream = new cgc_AudioStream(samples);
    for (unsigned int i = 0; i < stream->length; i++)
        stream->samples[i] = 0;
    return stream;
}

cgc_AudioStream *cgc_AudioStream::cgc_fromSineWave(unsigned int samples, unsigned int hz)
{
    cgc_AudioStream *stream = new cgc_AudioStream(samples);
    for (unsigned int i = 0; i < stream->length; i++)
    {
        double tmp = INT32_MAX/2 * sin(PI * 2 * hz / SAMPLE_RATE * i);

        stream->samples[i] = tmp;
    }
    return stream;
}

cgc_AudioStream *cgc_AudioStream::cgc_fromSquareWave(unsigned int samples, unsigned int hz)
{
    cgc_AudioStream *stream = new cgc_AudioStream(samples);
    double rate = 2.0 * SAMPLE_RATE / hz;
    for (unsigned int i = 0; i < stream->length; i++)
    {
        int tmp = i / rate;

        stream->samples[i] = tmp % 2 ? INT32_MAX/2 : -(INT32_MAX/2);
    }
    return stream;
}

void cgc_AudioStream::cgc_enlarge(unsigned int length)
{
    if (length <= this->length)
        return;

    if (size < length)
    {
        cgc_int32_t *newsamples;
        while (size < length)
        {
            if (size < 32*1024)
                size *= 2;
            else
                size += 32*1024;
        }

        newsamples = new cgc_int32_t[size];
        cgc_memcpy(newsamples, samples, this->length * sizeof(cgc_int32_t));
        delete[] samples;
        samples = newsamples;
    }

    cgc_memset(&samples[this->length], 0, sizeof(cgc_int32_t) * (length - this->length));
    this->length = length;
}

void cgc_AudioStream::cgc_setLength(unsigned int length)
{
    if (this->length <= length)
    {
        cgc_enlarge(length);
    }
    else
    {
        this->length = length;
    }
}

void cgc_AudioStream::cgc_addSilence(unsigned int length)
{
    cgc_enlarge(this->length + length);
}

void cgc_AudioStream::cgc_mix(const cgc_AudioStream &src, cgc_Gain gain)
{
    if (cgc_getLength() < src.cgc_getLength())
        return;

    for (unsigned int i = 0; i < src.cgc_getLength(); i++)
    {
        long long x = samples[i];
        x += gain.cgc_adjustSample(src.samples[i]);
        if (x >= INT32_MAX)
            samples[i] = INT32_MAX;
        else if (x <= INT32_MIN)
            samples[i] = INT32_MIN;
        else
            samples[i] = x;
    }
}
