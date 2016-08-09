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
#include <cstdlib.h>
#include "audiostream.h"
#include "gain.h"

class cgc_AudioTrack
{
public:
    cgc_AudioTrack(cgc_AudioStream *);
    cgc_AudioTrack(cgc_AudioStream *, cgc_AudioStream *);
    ~cgc_AudioTrack();

    inline unsigned int cgc_getLength() const
    {
        return channels[0]->cgc_getLength();
    }
    inline bool cgc_getStereo() const
    {
        return stereo;
    }
    inline cgc_AudioStream *cgc_getChannel(unsigned int i) const
    {
        if (i == 0)
            return channels[0];

        if (stereo)
            return channels[1];
        else
            return NULL;
    }

    void cgc_setLength(unsigned int length);
    inline void cgc_setGain(cgc_Gain level)
    {
        gain = level;
    }
    inline void cgc_setPan(cgc_int32_t cgc_value)
    {
        pan = cgc_value;
    }
    inline void cgc_setMute(bool cgc_value)
    {
        mute = cgc_value;
    }
    inline cgc_AudioTrack *cgc_toMono()
    {
        if (!stereo)
            return NULL;

        stereo = false;
        return new cgc_AudioTrack(channels[1]);
    }
    bool cgc_toStereo(cgc_AudioTrack *other);
    void cgc_mix(const cgc_AudioTrack &src);
public:
    unsigned int id;
private:
    cgc_AudioTrack();

    bool stereo;
    cgc_AudioStream *channels[2];
    cgc_Gain gain;
    cgc_int32_t pan;
    bool mute;
};
