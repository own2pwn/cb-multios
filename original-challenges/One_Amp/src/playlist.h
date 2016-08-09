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

#ifndef PLAYLIST_H_
#define PLAYLIST_H_

#define MAX_PLAYLIST_LENGTH 16000
#include "tag_and_file.h"

class cgc_Playlist
{
  public:
    cgc_Playlist();
    bool cgc_AddSong(const cgc_tag_and_file *song);
    bool cgc_RemoveSong(const unsigned int song_id, cgc_tag_and_file *removed_song);
    cgc_tag_and_file *cgc_GetSong(const unsigned int song_id);
    void cgc_SortById();
    void cgc_SortByTitle();
    void cgc_SortByArtistAndAlbum();
    void cgc_SortByArtistAndTitle();
    void cgc_SortByAlbum();
    void cgc_SortByAlbumAndTitle();
    void cgc_ListAllSongs();
    void cgc_ClearPlaylist(bool delete_playlist);

    unsigned int cgc_length();
  private:
    cgc_tag_and_file *playlist_;
    unsigned int length_;
    unsigned int list_size_;

    void cgc_Sort(cgc_tag_and_file *playlist, unsigned int cgc_length, int (*compare_fn)(cgc_tag_and_file *playlist1, cgc_tag_and_file *playlist2));
    void cgc_SortHelper(cgc_tag_and_file *playlist, cgc_tag_and_file *duped_list, unsigned int start_idx, unsigned int end_idx,
                    int (*compare_fn)(cgc_tag_and_file *playlist1, cgc_tag_and_file *playlist2));
};

#endif

