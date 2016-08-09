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
#include <cstdio.h>
#include <cstring.h>
#include "audiotrack.h"
#include "comp32.h"
#include "compfour.h"
#include "compeight.h"
#include "fxgain.h"
#include "fxpan.h"
#include "fxreverb.h"
#include "interface.h"

static const char menuMainPrompt[] = \
    "1) New track...\n" \
    "2) Import track\n" \
    "3) Export...\n" \
    "4) List tracks\n" \
    "5) Change gain [track] [hundredths]\n" \
    "6) Change pan [track] [hundredths]\n" \
    "7) Combine/split [track] [?track]\n" \
    "8) Effects...\n" \
    "9) Quit\n";
static const char menuEffectsPrompt[] = \
    "1) Apply gain [track] [hundredths]\n" \
    "2) Apply pan [track] [hundredths]\n" \
    "3) Echo [track] [delay in samples]\n" \
    "4) Add silence [track] [number of samples]\n" \
    "5) Cancel\n";
static const char menuExportPrompt[] = \
    "1) Uncompressed 32-bit signed\n" \
    "2) Compressed 4-bit\n" \
    "3) Compressed 8-bit\n" \
    "4) Cancel\n";
static const char menuNewPrompt[] = \
    "1) Silence [number of samples]\n" \
    "2) Sine Wave [number of samples] [HZ]\n" \
    "3) Square Wave [number of samples] [HZ]\n" \
    "4) White Noise [number of samples]\n" \
    "5) Cancel\n";

cgc_Interface::cgc_Interface()
{
}

cgc_Interface::~cgc_Interface()
{
}

void cgc_Interface::cgc_run()
{
    cgc_menuMain();
}

bool cgc_Interface::cgc_menuMain()
{
    cgc_AudioTrack *track, *other;
    int cgc_value;
    char line[100], *word, *buf;
    while (true)
    {
        cgc_fwrite(menuMainPrompt, cgc_strlen(menuMainPrompt), stdout);
        if (cgc_freaduntil(line, sizeof(line), '\n', stdin) < 0)
            return false;

        if (line[0] == '9')
            break;
        buf = line;
        word = cgc_strsep(&buf, " ");
        switch(line[0])
        {
        case '1':
            if (!cgc_menuNew())
                return false;
            break;
        case '2':
            if (!cgc_importAudio())
                return false;
            break;
        case '3':
            if (!cgc_menuExport())
                return false;
            break;
        case '4':
            cgc_listTracks();
            break;
        case '5':
            word = cgc_strsep(&buf, " ");
            if (word == NULL)
            {
                cgc_fprintf(stdout, "Missing track\n");
                continue;
            }
            cgc_value = cgc_strtol(word, NULL, 10);
            track = mixer.cgc_getTrack(cgc_value);
            if (track == NULL)
            {
                cgc_fprintf(stdout, "Invalid track\n");
                continue;
            }
            word = cgc_strsep(&buf, " ");
            if (word == NULL)
            {
                cgc_fprintf(stdout, "Missing gain\n");
                continue;
            }
            cgc_value = cgc_strtol(word, NULL, 10);
            if (cgc_value < -1000 || cgc_value > 1000)
            {
                cgc_fprintf(stdout, "Invalid gain (-10.00...10.00)\n");
                continue;
            }

            track->cgc_setGain(cgc_Gain::cgc_fromRational(cgc_value, 100));
            break;
        case '6':
            word = cgc_strsep(&buf, " ");
            if (word == NULL)
            {
                cgc_fprintf(stdout, "Missing track\n");
                continue;
            }
            cgc_value = cgc_strtol(word, NULL, 10);
            track = mixer.cgc_getTrack(cgc_value);
            if (track == NULL)
            {
                cgc_fprintf(stdout, "Invalid track\n");
                continue;
            }
            word = cgc_strsep(&buf, " ");
            if (word == NULL)
            {
                cgc_fprintf(stdout, "Missing pan\n");
                continue;
            }
            cgc_value = cgc_strtol(word, NULL, 10);
            if (cgc_value < -100 || cgc_value > 100)
            {
                cgc_fprintf(stdout, "Invalid pan (-1.00...1.00)\n");
                continue;
            }

            if (cgc_value == 100)
                track->cgc_setPan(INT32_MAX);
            else
                track->cgc_setPan((double)cgc_value / 100 * INT32_MAX);
            break;
        case '7':
            word = cgc_strsep(&buf, " ");
            if (word == NULL)
            {
                cgc_fprintf(stdout, "Missing track 1\n");
                continue;
            }
            cgc_value = cgc_strtol(word, NULL, 10);
            track = mixer.cgc_getTrack(cgc_value);
            if (track == NULL)
            {
                cgc_fprintf(stdout, "Invalid track 1\n");
                continue;
            }

            word = cgc_strsep(&buf, " ");
            if (word != NULL)
            {
                cgc_value = cgc_strtol(word, NULL, 10);
                other = mixer.cgc_getTrack(cgc_value);
                if (other == NULL)
                {
                    cgc_fprintf(stdout, "Invalid track 2\n");
                    continue;
                }
            }
            else
            {
                other = NULL;
            }

            if (other == NULL)
            {
                if (!track->cgc_getStereo())
                {
                    cgc_fprintf(stdout, "Cannot split a mono track\n");
                    continue;
                }
                mixer.cgc_splitTrack(track->id);
            }
            else
            {
                if (track->cgc_getStereo() || other->cgc_getStereo())
                {
                    cgc_fprintf(stdout, "Cannot combine a stereo track\n");
                    continue;
                }
                mixer.cgc_combineTracks(track->id, other->id);
            }
            break;
        case '8':
            cgc_menuEffects();
            break;
        default:
            cgc_fprintf(stdout, "Invalid choice\n");
            break;
        }
    }
    cgc_fprintf(stdout, "Thank you for trying out Grit! Please leave us a nice review on the app store.\n");
    return true;
}

bool cgc_Interface::cgc_menuExport()
{
    char line[100];
    while (true)
    {
        cgc_fwrite(menuExportPrompt, cgc_strlen(menuExportPrompt), stdout);
        if (cgc_freaduntil(line, sizeof(line), '\n', stdin) < 0)
            return false;

        if (line[0] == '4')
            break;
        switch(line[0])
        {
        case '1':
            cgc_exportAudio(cgc_Compression32());
            break;
        case '2':
            cgc_exportAudio(cgc_CompressionFour());
            break;
        case '3':
            cgc_exportAudio(cgc_CompressionEight());
            break;
        default:
            cgc_fprintf(stdout, "Invalid choice\n");
            continue;
        }
        break;
    }
    return true;
}

bool cgc_Interface::cgc_menuNew()
{
    char line[100], *buf, *word;
    unsigned int samples, hz;
    cgc_AudioTrack *track;
    while (true)
    {
        cgc_fwrite(menuNewPrompt, cgc_strlen(menuNewPrompt), stdout);
        if (cgc_freaduntil(line, sizeof(line), '\n', stdin) < 0)
            return false;

        if (line[0] == '5')
            break;

        buf = line;
        word = cgc_strsep(&buf, " "); // ignore command
        word = cgc_strsep(&buf, " ");
        if (word == NULL)
        {
            cgc_fprintf(stdout, "Missing samples\n");
            continue;
        }
        samples = cgc_strtoul(word, NULL, 10);
        if (samples > INT32_MAX)
        {
            cgc_fprintf(stdout, "Invalid samples\n");
            continue;
        }

        switch(line[0])
        {
        case '1':
            track = new cgc_AudioTrack(cgc_AudioStream::cgc_fromSilence(samples));
            mixer.cgc_addTrack(track);
            break;
        case '2':
        case '3':
            word = cgc_strsep(&buf, " ");
            if (word == NULL)
            {
                cgc_fprintf(stdout, "Missing frequency\n");
                continue;
            }
            hz = cgc_strtoul(word, NULL, 10);
            if (hz >= 50000)
            {
                cgc_fprintf(stdout, "Invalid frequency\n");
                continue;
            }

            if (line[0] == '2')
                track = new cgc_AudioTrack(cgc_AudioStream::cgc_fromSineWave(samples, hz));
            else
                track = new cgc_AudioTrack(cgc_AudioStream::cgc_fromSquareWave(samples, hz));
            mixer.cgc_addTrack(track);
            break;
        case '4':
            track = new cgc_AudioTrack(mixer.cgc_generateWhiteNoise(samples));
            mixer.cgc_addTrack(track);
            break;
        default:
            cgc_fprintf(stdout, "Invalid choice\n");
            continue;
        }

        break;
    }
    return true;
}

void cgc_Interface::cgc_listTracks()
{
    for (auto it = mixer.cgc_getTracks(); !it.cgc_empty(); it.cgc_next())
    {
        cgc_AudioTrack *track = it.cgc_value();
        cgc_fprintf(stdout, "%d) %s samples=%d\n", track->id, track->cgc_getStereo() ? "stereo" : "mono", track->cgc_getLength());
    }
}

void cgc_Interface::cgc_exportAudio(const cgc_Compression &comp)
{
    cgc_AudioTrack *track = mixer.cgc_exportMix();
    unsigned int size = sizeof(AudioHeader) + comp.cgc_getMaxBufferSize(*track);
    cgc_uint8_t *data = new cgc_uint8_t[size];

    AudioHeader *hdr = (AudioHeader *)data;
    hdr->magic = 0x2e617564;
    hdr->offset = sizeof(AudioHeader);
    hdr->samples = track->cgc_getLength();
    hdr->encoding = comp.cgc_getId();
    hdr->sample_rate = SAMPLE_RATE;
    hdr->channels = track->cgc_getStereo() ? 2 : 1;

    comp.cgc_compress(*track, &data[sizeof(AudioHeader)]);

    cgc_fwrite(&size, sizeof(size), stdout);
    cgc_fwrite(data, size, stdout);

    delete data;
    delete track;
}

bool cgc_Interface::cgc_importAudio()
{
    unsigned int size;
    if (cgc_fread(&size, sizeof(size), stdin) != sizeof(size))
        return false;
    if (size >= INT32_MAX || size <= sizeof(AudioHeader))
        return false;
    cgc_uint8_t *data = new cgc_uint8_t[size];
    if (cgc_fread(data, size, stdin) != size)
        return false;

    AudioHeader *hdr = (AudioHeader *)data;
    if (hdr->magic == 0x2e617564 && hdr->offset == sizeof(AudioHeader) && hdr->samples < INT32_MAX && hdr->sample_rate == SAMPLE_RATE)
    {
        cgc_Compression *comp = cgc_Compression::cgc_fromId(hdr->encoding);
        if (comp != NULL)
        {
            cgc_AudioTrack *track;
            if (hdr->channels == 1)
                track = new cgc_AudioTrack(cgc_AudioStream::cgc_fromSilence(hdr->samples));
            else
                track = new cgc_AudioTrack(cgc_AudioStream::cgc_fromSilence(hdr->samples), cgc_AudioStream::cgc_fromSilence(hdr->samples));
#ifdef PATCHED_1
            if (size - hdr->offset >= comp->cgc_getMaxBufferSize(*track))
#endif
            comp->cgc_decompress(*track, &data[sizeof(AudioHeader)]);
            mixer.cgc_addTrack(track);
            delete comp;
        }
    }

    delete data;
    return true;
}

bool cgc_Interface::cgc_menuEffects()
{
    char line[100], *buf, *word;
    int cgc_value;
    cgc_AudioTrack *track;
    while (true)
    {
        cgc_fwrite(menuEffectsPrompt, cgc_strlen(menuEffectsPrompt), stdout);
        if (cgc_freaduntil(line, sizeof(line), '\n', stdin) < 0)
            return false;

        if (line[0] == '5')
            break;

        buf = line;
        word = cgc_strsep(&buf, " "); // ignore command
        word = cgc_strsep(&buf, " ");
        if (word == NULL)
        {
            cgc_fprintf(stdout, "Missing track\n");
            continue;
        }
        cgc_value = cgc_strtol(word, NULL, 10);
        track = mixer.cgc_getTrack(cgc_value);
        if (track == NULL)
        {
            cgc_fprintf(stdout, "Invalid track\n");
            continue;
        }

        switch(line[0])
        {
        case '1':
            word = cgc_strsep(&buf, " ");
            if (word == NULL)
            {
                cgc_fprintf(stdout, "Missing gain\n");
                continue;
            }
            cgc_value = cgc_strtol(word, NULL, 10);
            if (cgc_value < -1000 || cgc_value > 1000)
            {
                cgc_fprintf(stdout, "Invalid gain (-10.00...10.00)\n");
                continue;
            }

            cgc_FxGain(cgc_Gain::cgc_fromRational(cgc_value, 100)).cgc_apply(*track);
            break;
        case '2':
            word = cgc_strsep(&buf, " ");
            if (word == NULL)
            {
                cgc_fprintf(stdout, "Missing pan\n");
                continue;
            }
            cgc_value = cgc_strtol(word, NULL, 10);
            if (cgc_value < -100 || cgc_value > 100)
            {
                cgc_fprintf(stdout, "Invalid pan (-1.00...1.00)\n");
                continue;
            }

            if (cgc_value == 100)
                cgc_FxPan(INT32_MAX).cgc_apply(*track);
            else
                cgc_FxPan((double)cgc_value / 100 * INT32_MAX).cgc_apply(*track);
            break;
        case '3':
            word = cgc_strsep(&buf, " ");
            if (word == NULL)
            {
                cgc_fprintf(stdout, "Missing delay\n");
                continue;
            }
            cgc_value = cgc_strtol(word, NULL, 10);
            if (cgc_value <= 0)
            {
                cgc_fprintf(stdout, "Invalid delay (>0)\n");
                continue;
            }

            cgc_FxReverb(cgc_value).cgc_apply(*track);
            break;
        case '4':
            word = cgc_strsep(&buf, " ");
            if (word == NULL)
            {
                cgc_fprintf(stdout, "Missing samples\n");
                continue;
            }
            cgc_value = cgc_strtol(word, NULL, 10);
            if (cgc_value <= 0)
            {
                cgc_fprintf(stdout, "Invalid samples (>0)\n");
                continue;
            }

            track->cgc_setLength(track->cgc_getLength() + cgc_value);
            break;
        default:
            cgc_fprintf(stdout, "Invalid choice\n");
            continue;
        }

        break;
    }
    return true;
}

