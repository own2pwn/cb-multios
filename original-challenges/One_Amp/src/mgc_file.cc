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

#include <cstring.h>

#include "mgc_file.h"
#include "print.h"
#include "mgc_header_info.h"

cgc_MgcFile::cgc_MgcFile()
{
    frames_ = (cgc_mgc_frame **)NULL;
    num_frames_ = 0;
    size_ = 0;
}

bool cgc_MgcFile::cgc_ReadMgcFile(cgc_FILE *stream)
{
    unsigned char frame_buffer[6400];
    cgc_mgc_frame temp_frame;
    do
    {
        cgc_fread(&temp_frame, sizeof(temp_frame), stream);
        if (!MgcHeaderInfo::cgc_Synced(&temp_frame) || !MgcHeaderInfo::cgc_SongV1(&temp_frame))
        {
            cgc_Clear();
            return false;
        }

        cgc_fread(frame_buffer, MgcHeaderInfo::cgc_CalcFrameSize(&temp_frame), stream);
        cgc_mgc_frame *new_frame = (cgc_mgc_frame *)cgc_malloc(sizeof(cgc_mgc_frame) + MgcHeaderInfo::cgc_CalcFrameSize(&temp_frame));
        cgc_memcpy(new_frame, &temp_frame, sizeof(temp_frame));
        cgc_memcpy(new_frame->data, frame_buffer, MgcHeaderInfo::cgc_CalcFrameSize(&temp_frame));
        cgc_AddFrame(new_frame);
    } while(temp_frame.num_frames_left);

    return true;
}

const cgc_mgc_frame *cgc_MgcFile::cgc_GetFrame(unsigned int idx) const
{
    if (idx < num_frames_)
        return frames_[idx];
    return (cgc_mgc_frame *)NULL;
}

void cgc_MgcFile::cgc_Clear()
{
    if (frames_)
    {
        for (unsigned int i = 0; i < num_frames_; i++)
            cgc_free(frames_[i]);

        delete[] frames_;
        frames_ = (cgc_mgc_frame **)NULL;
    }
    num_frames_ = 0;
    size_ = 0;
}

void cgc_MgcFile::cgc_Remix(unsigned char *mix_buf, unsigned int *idx, const unsigned int buf_size) //idx incrememts by 33
{
    for (unsigned int i = 0; i < num_frames_; i++)
    {
        for (unsigned int j = 0; j < MgcHeaderInfo::cgc_CalcFrameSize(frames_[i]); j++)
        {
            unsigned char *pframe_data = (unsigned char *)(&frames_[i]->data[j]);
            *pframe_data *= mix_buf[*idx];
            *idx = (*idx + 33) % buf_size;
        }
    }
}

void cgc_MgcFile::cgc_PrintFrameData()
{

    cgc_printf("---Raw Frames---" NL);
    for (unsigned int i = 0; i < num_frames_; i++)
    {
        PRINT_ARR_HEX(frames_[i]->data, MgcHeaderInfo::cgc_CalcFrameSize(frames_[i]));
    }
    cgc_printf(NL "----------------" NL);
    cgc_fflush(stdout);

}

unsigned cgc_MgcFile::cgc_TotalFrames()
{
    return num_frames_;
}

bool cgc_MgcFile::cgc_AddFrame(cgc_mgc_frame *mframe)
{
    if (!mframe)
        return false;

    if (!frames_)
    {
        size_ = 16;
        frames_ = new cgc_mgc_frame*[size_];
    }
    if (num_frames_ == size_)
    {
        size_ *= 2;
        cgc_mgc_frame **doubled_list = new cgc_mgc_frame*[size_];
        cgc_memcpy(doubled_list, frames_, sizeof(cgc_mgc_frame *) * num_frames_);
        delete frames_;
        frames_ = doubled_list;
    }
    frames_[num_frames_++] = mframe;
    return true;


}
