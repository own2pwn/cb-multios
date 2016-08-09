/*
 * Copyright (c) 2016 Kaprica Security, Inc.
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

#include "compare.h"

int cgc_CompareIds(cgc_tag_and_file *song1, cgc_tag_and_file *song2)
{
    if (song1->tag.id > song2->tag.id)
        return 1;
    else if (song1->tag.id == song2->tag.id)
        return 0;
    else
        return -1;
}

int cgc_CompareTitles(cgc_tag_and_file *song1, cgc_tag_and_file *song2)
{
    return cgc_memcmp(song1->tag.title, song2->tag.title, sizeof(song1->tag.title));
}

int cgc_CompareArtists(cgc_tag_and_file *song1, cgc_tag_and_file *song2)
{
    return cgc_memcmp(song1->tag.artist, song2->tag.artist, sizeof(song1->tag.artist));
}

int cgc_CompareAlbums(cgc_tag_and_file *song1, cgc_tag_and_file *song2)
{
    return cgc_memcmp(song1->tag.album, song2->tag.album, sizeof(song1->tag.album));
}

int cgc_CompareAlbumTracks(cgc_tag_and_file *song1, cgc_tag_and_file *song2)
{
    if (song1->tag.track_number > song2->tag.track_number)
        return 1;
    else if (song1->tag.track_number == song2->tag.track_number)
        return 0;
    else
        return -1;
}

