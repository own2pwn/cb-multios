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
#include "mixer.h"

cgc_Mixer::cgc_Mixer()
    : nextTrackId(0), tracks(cgc_LinkedList<cgc_AudioTrack *>::cgc_deleteDestructor)
{
}

cgc_Mixer::~cgc_Mixer()
{
}

unsigned int cgc_Mixer::cgc_getLength() const
{
    unsigned int length = 0;
    for (auto it = tracks.cgc_begin(); !it.cgc_empty(); it.cgc_next())
    {
        unsigned int trackLength = it.cgc_value()->cgc_getLength();
        if (trackLength > length)
            length = trackLength;
    }
    return length;
}

void cgc_Mixer::cgc_addTrack(cgc_AudioTrack *track)
{
    track->id = nextTrackId++;
    tracks.cgc_append(track);
}

cgc_AudioTrack *cgc_Mixer::cgc_getTrack(unsigned int id) const
{
    for (auto it = tracks.cgc_begin(); !it.cgc_empty(); it.cgc_next())
    {
        cgc_AudioTrack *track = it.cgc_value();
        if (track->id == id)
            return track;
    }

    return NULL;
}

void cgc_Mixer::cgc_removeTrack(unsigned int id)
{
    for (auto it = tracks.cgc_begin(); !it.cgc_empty(); it.cgc_next())
    {
        cgc_AudioTrack *track = it.cgc_value();
        if (track->id == id)
        {
            tracks.cgc_removeAt(it);
            delete track;
            return;
        }
    }
}

bool cgc_Mixer::cgc_splitTrack(unsigned int id)
{
    cgc_AudioTrack *track = cgc_getTrack(id);
    if (track == NULL || !track->cgc_getStereo())
        return false;

    track = track->cgc_toMono();
    cgc_addTrack(track);
    return true;
}

bool cgc_Mixer::cgc_combineTracks(unsigned int leftId, unsigned int rightId)
{
    cgc_AudioTrack *left, *right;
    left = cgc_getTrack(leftId);
    if (left == NULL || left->cgc_getStereo())
        return false;
    for (auto it = tracks.cgc_begin(); !it.cgc_empty(); it.cgc_next())
    {
        right = it.cgc_value();
        if (right->id == rightId)
        {
            if (right->cgc_getStereo())
                return false;
            if (!left->cgc_toStereo(right))
                return false;
            tracks.cgc_removeAt(it);
            return true;
        }
    }
    return false;
}

cgc_AudioTrack *cgc_Mixer::cgc_exportMix()
{
    unsigned int length = cgc_getLength();
    cgc_AudioTrack *output = new cgc_AudioTrack(cgc_AudioStream::cgc_fromSilence(length), cgc_AudioStream::cgc_fromSilence(length));
    for (auto it = tracks.cgc_begin(); !it.cgc_empty(); it.cgc_next())
    {
        cgc_AudioTrack *track = it.cgc_value();
        output->cgc_mix(*track);
    }
    return output;
}

cgc_AudioStream *cgc_Mixer::cgc_generateWhiteNoise(unsigned int length)
{
    rng.cgc_addEntropy((cgc_uint8_t *)0x4347C000, 0x1000);
    cgc_AudioStream *stream = cgc_AudioStream::cgc_fromSilence(length);
    for (unsigned int i = 0; i < length; i++)
    {
        stream->cgc_setSample(i, rng.cgc_randomInt32() >> 1);
    }
    return stream;
}
