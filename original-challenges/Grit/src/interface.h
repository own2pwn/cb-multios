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
#include "comp.h"
#include "mixer.h"

struct AudioHeader
{
    cgc_uint32_t magic;
    cgc_uint32_t offset;
    cgc_uint32_t samples;
    cgc_uint32_t encoding;
    cgc_uint32_t sample_rate;
    cgc_uint32_t channels;
};

class cgc_Interface
{
public:
    cgc_Interface();
    ~cgc_Interface();
    void cgc_run();
private:
    void cgc_exportAudio(const cgc_Compression &comp);
    bool cgc_importAudio();
    void cgc_listTracks();
    bool cgc_menuMain();
    bool cgc_menuEffects();
    bool cgc_menuExport();
    bool cgc_menuNew();
    cgc_Mixer mixer;
};
