/*

Copyright (c) 2016 Cromulence LLC

Authors: Dan Czarnecki <cgc@cromulence.com>

Permission is hereby granted, cgc_free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

*/

#include "compress.h"

// 8-bit color
//
//    1  1  1  1  1  1  1  1
//    R  R  R  G  G  B  B  B
//

enum {
  R,
  G,
  B
};

int red_blue[8]  = { 0x00, 0x20, 0x40, 0x60, 0xA0, 0xC0, 0xE0, 0xFF };
int green[4]     = { 0x00, 0x60, 0xB0, 0xFF };

cgc_uint8_t cgc_ClosestMatch(cgc_uint8_t type, cgc_int16_t val) {
  cgc_uint8_t close_diff = 0xff;
  cgc_uint8_t temp_diff = 0;
  cgc_uint8_t max_idx = ((type == R || type == B) ? 8 : 4);
  cgc_uint8_t close_idx = max_idx;

  int i;

  for(i = 0; i < max_idx; i++) {
    if (type == R || type == B)
      temp_diff = ABS((int)(val - red_blue[i]));
    else
      temp_diff = ABS((int)(val - green[i]));

    if (temp_diff < close_diff) {
      close_diff = temp_diff;
      close_idx = i;
    }
  }

  return close_idx;

}

void cgc_Compress(cgc_t3DCPixel **px_list, cgc_uint8_t *compressed_data, cgc_uint16_t *data_len) {
  if (px_list == NULL) {
    return;
  }

  cgc_uint8_t color = 0;
  cgc_uint8_t coord = 0;

  cgc_t3DCPixel *px = NULL;
  cgc_uint16_t count = MAX_PIXELS * 7; // (x, y, z, color)
  cgc_uint16_t i = 0;
  int p_idx = 0;

  while (i < count) {
    color = 0;
    px = px_list[p_idx++];
    // red
    color += (cgc_ClosestMatch(R, px->r) << 5);

    // green
    color += (cgc_ClosestMatch(G, px->g) << 3);

    // blue
    color += (cgc_ClosestMatch(B, px->b) << 0);

    cgc_memcpy(compressed_data+i, &px->x, sizeof(cgc_int16_t));
    i += sizeof(cgc_int16_t);

    cgc_memcpy(compressed_data+i, &px->y, sizeof(cgc_int16_t));
    i += sizeof(cgc_int16_t);

    cgc_memcpy(compressed_data+i, &px->z, sizeof(cgc_int16_t));
    i += sizeof(cgc_int16_t);

    cgc_memcpy(compressed_data+i, &color, 1);
    i++;
  }

  cgc_memcpy(data_len, &i, 2);

  return;
}


void cgc_Decompress(cgc_uint8_t *in_data, cgc_uint8_t *out_data, cgc_uint16_t *data_len) {
  cgc_uint32_t in_offset = 0;
  cgc_uint32_t out_offset = 0;

  int p_idx = 0;
  cgc_uint8_t color;
  cgc_uint8_t r,g,b;

  while (in_offset < *data_len) {
    cgc_memcpy(out_data+out_offset, in_data+in_offset, sizeof(cgc_int16_t));
    out_offset += sizeof(cgc_int16_t);
    in_offset += sizeof(cgc_int16_t);

    cgc_memcpy(out_data+out_offset, in_data+in_offset, sizeof(cgc_int16_t));
    out_offset += sizeof(cgc_int16_t);
    in_offset += sizeof(cgc_int16_t);

    cgc_memcpy(out_data+out_offset, in_data+in_offset, sizeof(cgc_int16_t));
    out_offset += sizeof(cgc_int16_t);
    in_offset += sizeof(cgc_int16_t);

    color = in_data[in_offset++];

    // red
    r = ((0xe0) & color) >> 5;

    // green
    g = ((0x18) & color) >> 3;

    // blue
    b = ((0x7) & color) >> 0;

    out_data[out_offset++] = red_blue[r];
    out_data[out_offset++] = green[g];
    out_data[out_offset++] = red_blue[b];

    out_data[out_offset++] = 0xff;

  }

  cgc_memcpy(data_len, &out_offset, 2);

  return;
}

void cgc_WriteOut(cgc_t3DCPixel **px_list, cgc_uint8_t *data, cgc_uint16_t data_len) {
  // load into our pixel data

  if (px_list == NULL) {
    return;
  }

  cgc_uint16_t offset = 0;
  cgc_uint16_t i = 0;
  cgc_t3DCPixel *tmp;

  while (offset < data_len) {
    tmp = px_list[i];

    offset += 6;

    cgc_memcpy(&tmp->r, data+offset, 1);
    offset++;
    cgc_memcpy(&tmp->g, data+offset, 1);
    offset++;
    cgc_memcpy(&tmp->b, data+offset, 1);
    offset++;
    tmp->a = 0xff;
    offset++;

    i++;

  }

}
