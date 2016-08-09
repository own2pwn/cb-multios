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
#include "audiotrack.h"

cgc_AudioTrack::cgc_AudioTrack()
{
    pan = 0;
    mute = false;
}

cgc_AudioTrack::cgc_AudioTrack(cgc_AudioStream *left) : cgc_AudioTrack()
{
    stereo = false;
    channels[0] = left;
}

cgc_AudioTrack::cgc_AudioTrack(cgc_AudioStream *left, cgc_AudioStream *right) : cgc_AudioTrack()
{
    stereo = true;
    channels[0] = left;
    channels[1] = right;

    // make sure both channels are the same length
    if (left->cgc_getLength() < right->cgc_getLength())
        left->cgc_setLength(right->cgc_getLength());
    else if (right->cgc_getLength() < left->cgc_getLength())
        right->cgc_setLength(left->cgc_getLength());
}

cgc_AudioTrack::~cgc_AudioTrack()
{
    if (channels[0])
        delete channels[0];
    if (stereo)
        if (channels[1])
            delete channels[1];
}

void cgc_AudioTrack::cgc_setLength(unsigned int length)
{
    channels[0]->cgc_setLength(length);
    if (stereo)
        channels[1]->cgc_setLength(length);
}

void cgc_AudioTrack::cgc_mix(const cgc_AudioTrack &src)
{
    if (cgc_getLength() < src.cgc_getLength())
        cgc_setLength(src.cgc_getLength());

    if (!stereo)
    {
        // XXX we don't currently support down-mixing stereo to mono
        if (src.stereo)
            return;

        // ignore pan since we are mixing mono to mono
        channels[0]->cgc_mix(*src.channels[0], src.gain);
    }
    else if (!src.stereo)
    {
        // cgc_mix src into both channels using pan
        channels[0]->cgc_mix(*src.channels[0], src.gain * cgc_Gain::cgc_fromPanLeft(src.pan));
        channels[1]->cgc_mix(*src.channels[0], src.gain * cgc_Gain::cgc_fromPanRight(src.pan));
    }
    else
    {
        // cgc_mix channels independently
        channels[0]->cgc_mix(*src.channels[0], src.gain * cgc_Gain::cgc_fromPanLeft(src.pan));
        channels[1]->cgc_mix(*src.channels[1], src.gain * cgc_Gain::cgc_fromPanRight(src.pan));
    }
}

bool cgc_AudioTrack::cgc_toStereo(cgc_AudioTrack *other)
{
    if (stereo || other->stereo)
        return false;

    stereo = true;
    channels[1] = other->channels[0];
    other->channels[0] = NULL;
    delete other;

    // make sure both channels are the same length
    if (channels[0]->cgc_getLength() < channels[1]->cgc_getLength())
        channels[0]->cgc_setLength(channels[1]->cgc_getLength());
    else if (channels[1]->cgc_getLength() < channels[0]->cgc_getLength())
        channels[1]->cgc_setLength(channels[0]->cgc_getLength());
    return true;
}
