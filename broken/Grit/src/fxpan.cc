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
#include "fxpan.h"
#include "gain.h"

cgc_FxPan::cgc_FxPan(cgc_int32_t pan_) : pan(pan_)
{
}

void cgc_FxPan::cgc_apply(cgc_AudioTrack &track) const
{
    if (!track.cgc_getStereo())
        return;

    cgc_apply(*track.cgc_getChannel(0), cgc_Gain::cgc_fromPanLeft(pan));
    cgc_apply(*track.cgc_getChannel(1), cgc_Gain::cgc_fromPanRight(pan));
}

void cgc_FxPan::cgc_apply(cgc_AudioStream &stream, cgc_Gain gain) const
{
    for (unsigned int i = 0; i < stream.cgc_getLength(); i++)
    {
        stream.cgc_setSample(i, gain.cgc_adjustSample(stream.cgc_getSample(i)));
    }
}
