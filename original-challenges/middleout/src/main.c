/*
 * Copyright (c) 2015 Kaprica Security, Inc.
 *
 * Permission is hereby granted, cgc_free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, cgc_merge, publish, distribute, sublicense, and/or sell
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

#include <libcgc.h>

#include "ctype.h"
#include "stdint.h"
#include "stdlib.h"
#include "string.h"

typedef cgc_int8_t cgc_s8;
typedef cgc_int16_t cgc_s16;
typedef cgc_int32_t cgc_s32;
typedef cgc_int64_t cgc_s64;

typedef cgc_uint8_t cgc_u8;
typedef cgc_uint16_t cgc_u16;
typedef cgc_uint32_t cgc_u32;
typedef cgc_uint64_t cgc_u64;

#define dbg(s, ...) cgc_fdprintf(STDERR, "DEBUG %s:%d:\t" s "\n", __FILE__, __LINE__, ##__VA_ARGS__)
#define err(s, ...) \
({ \
  cgc_fdprintf(STDERR, "DEBUG %s:%d:\t" s "\n", __FILE__, __LINE__, ##__VA_ARGS__); \
  cgc_exit(1); \
})

static size_t end_marker = 0xDEEDACED;

// cgc_calloc or crash
void *cgc_mcalloc(size_t size)
{
  void *x = cgc_calloc(1, size);
  if (!x)
    err("Bad alloc\n");
  return x;
}

typedef struct cgc_any_list_t cgc_any_list_t;
struct cgc_any_list_t {
  cgc_any_list_t* prev;
  cgc_any_list_t* next;
  void* data;
};

void cgc_any_list_add(cgc_any_list_t* new, cgc_any_list_t* prev, cgc_any_list_t* next)
{
  if (next)
    next->prev = new;
  if (prev)
    prev->next = new;
  new->next = next;
  new->prev = prev;
}

#define any_list_add_head(new, head) cgc_any_list_add((new), (head), (head)->next)
#define any_list_add_tail(new, head) cgc_any_list_add((new), ((head)->prev), (head))
#define any_list_first(head) (head)->next

void cgc_any_list_remv(cgc_any_list_t* entry)
{
  if (!entry)
    return;
  entry->next->prev = entry->prev;
  entry->prev->next = entry->next;
  if (entry->data)
    cgc_free(entry->data);
  cgc_free(entry);
}

int cgc_any_list_empty(cgc_any_list_t* head)
{
  return head->next == head;
}

size_t cgc_any_list_length(cgc_any_list_t* head)
{
  size_t n = 0;
  cgc_any_list_t* cur = head->next;
  while (cur->next != head)
    n++;
  return n;
}

cgc_any_list_t* cgc_any_list_alloc(void *data)
{
  cgc_any_list_t* new = cgc_mcalloc(sizeof(cgc_any_list_t));
  new->next = new;
  new->prev = new;
  new->data = data;
  return new;
}

cgc_any_list_t* cgc_any_list_shallow_copy(cgc_any_list_t* list)
{
  if (!list)
    return NULL;
  cgc_any_list_t* copy = cgc_any_list_alloc(NULL);
  for (cgc_any_list_t* cur = list; cur->next != list; cur = cur->next)
    any_list_add_tail(cur, copy);
  return copy;
}

int cgc_send_n_bytes(int fd, size_t n, char *buf)
{
  if (!n || !buf)
    return -1;

  size_t tx = 0, total_sent = 0;

  while (total_sent < n) {
    if (transmit(fd, buf + total_sent, n - total_sent, &tx) != 0) {
      return -1;
    } else if (tx == 0) {
      break;
    } else {
      total_sent += tx;
    }
  }

  return total_sent;
}

int cgc_read_n_bytes(int fd, size_t n, char *buf)
{
  if (!n || !buf)
    return -1;

  size_t rx = 0, total_read = 0;

  while (total_read < n) {
    if (receive(fd, buf + total_read, n - total_read, &rx) != 0) {
      return -1;
    } else if (rx == 0) {
      break;
    } else {
      total_read += rx;
    }
  }

  return total_read;
}

char* cgc_read_until_sequence(int fd, char* sequence, size_t sequence_len)
{
#define STEP 128
  char* out = cgc_mcalloc(sizeof(char) * STEP);
  char* seq_loc = NULL;
  size_t sz = STEP;
  size_t rx = 0;

  while (1) {
    if (rx == sz) {
      sz += STEP;
      out = cgc_realloc(out, sz);
      if (!out)
        err("Bad alloc");
    }

    size_t lrx = cgc_read_n_bytes(fd, 1, out + rx);
    if (lrx <= 0)
      goto error;

    for (size_t k = 0; k < sequence_len; k++)
      if (cgc_memcmp(out + rx - k, sequence, sequence_len) == 0)
        goto done;

    rx += lrx;
  }

done:
  return out;
error:
  cgc_free(out);
  return NULL;
}

#define STEP 128
char* cgc_readline(int fd)
{
  char* out = cgc_mcalloc(sizeof(char) * STEP);
  char* newline_loc = NULL;
  size_t sz = STEP;
  size_t rx = 0;

  while (1) {
    if (rx == sz) {
      sz += STEP;
      out = cgc_realloc(out, sz);
      if (!out)
        err("Bad alloc");
    }

    size_t lrx = cgc_read_n_bytes(fd, 1, out + rx);
    if (lrx <= 0)
      goto error;

    if (*(out + rx) == '\n')
      goto done;

    rx += lrx;
  }

done:
  newline_loc = cgc_memchr(out + rx, '\n', STEP);
  if (!newline_loc)
    goto error;

  *newline_loc = '\0';
  return out;
error:
  cgc_free(out);
  return NULL;
}

typedef struct cgc_writer_t cgc_writer_t;
struct cgc_writer_t
{
  size_t offset;
  size_t cap;
  char* buf;
};

cgc_writer_t* cgc_writer_new(char* buf, size_t cap)
{
  cgc_writer_t* new = cgc_mcalloc(sizeof(cgc_writer_t));
  new->offset = 0;
  new->cap = cap;
  new->buf = buf;
  return new;
}

int cgc_writer_write(cgc_writer_t* writer, void* data, size_t len)
{
  for (size_t i = 0; i < len; i++)
    *(cgc_u8*)(writer->buf + writer->offset + i) = *(cgc_u8*)(data + i);
  writer->offset += len;
  return 0;
}

typedef struct cgc_lz_elem_t cgc_lz_elem_t;
struct cgc_lz_elem_t {
  enum {
    BACKPOINTER = 1,
    LITERAL
  } type;
  size_t back;
  char c;
  size_t len;
};


void cgc_print_lzelem(int fd, cgc_lz_elem_t* lzelem)
{
  switch (lzelem->type) {
  case BACKPOINTER:
    cgc_fdprintf(fd, "(%d,%d)", lzelem->back, lzelem->len);
    break;
  case LITERAL:
    cgc_fdprintf(fd, "%c", lzelem->c);
    break;
  }
}

cgc_u32 cgc_swap_u32(cgc_u32 x)
{
  x = ((x << 8) & 0xFF00FF00) | ((x >> 8) & 0xFF00FF);
  return (x << 16) | (x >> 16);
}

cgc_u32 cgc_pack_lzelem_bp(cgc_lz_elem_t* lzelem)
{
  cgc_u32 out = 0;
  if (lzelem->len >= 1 << 15)
    err("Can't pack");
  out |= 0x80000000;
  out |= (lzelem->len & 0x7FFF) << 16;
  out |= lzelem->back & 0xFFFF;
  out = cgc_swap_u32(out);
  return out;
}

cgc_lz_elem_t* cgc_unpack_lzelem_bp(cgc_u32 in)
{
  in = cgc_swap_u32(in);
  cgc_lz_elem_t* out = cgc_mcalloc(sizeof(cgc_lz_elem_t));
  out->len = (in >> 16) & 0x7FFF;
  out->back = in & 0xFFFF;
  return out;
}


cgc_lz_elem_t* cgc_lz_backpointer_new(size_t back, size_t len)
{
  cgc_lz_elem_t* new = cgc_mcalloc(sizeof(cgc_lz_elem_t));
  new->type = BACKPOINTER;
  new->back = back;
  new->len = len;
  return new;
}

cgc_lz_elem_t* cgc_lz_literal_new(char c)
{
  cgc_lz_elem_t* new = cgc_mcalloc(sizeof(cgc_lz_elem_t));
  new->type = LITERAL;
  new->c = c;
  new->len = 1;
  return new;
}


char* cgc_alnumspc_filter(char* input)
{
  if (!input)
    return NULL;

  size_t input_len = cgc_strlen(input);

  if (!input_len)
    return NULL;

  size_t k = 0;
  char* output = cgc_mcalloc(input_len + 1);

  for (size_t i = 0; i < input_len; i++)
    if (cgc_isalnum(input[i]) || input[i] == ' ')
        output[k++] = input[i];

  return output;
}

typedef struct suffix_t cgc_suffix;
struct suffix_t {
  size_t index;
  char* input;
};

cgc_suffix* cgc_make_suffix(size_t index, char* input)
{
  cgc_suffix* new = cgc_mcalloc(sizeof(cgc_suffix));
  new->index = index;
  new->input = input;
  return new;
}

typedef struct suffix_list_t cgc_suffix_list;
struct suffix_list_t {
  size_t cap;
  size_t size;
  cgc_suffix** suffixes;
};

cgc_suffix_list* cgc_make_suffix_list(size_t cap)
{
  cgc_suffix_list* new = cgc_mcalloc(sizeof(cgc_suffix_list));
  new->cap = cap;
  new->size = 0;
  new->suffixes = cgc_mcalloc(sizeof(cgc_suffix *) * cap);
  return new;
}

int cgc_append_suffix_list(cgc_suffix_list* list, cgc_suffix* cgc_suffix)
{
  if (list->size == list->cap)
    return -1;

  list->suffixes[list->size++] = cgc_suffix;
  return 0;
}

cgc_suffix* cgc_get_suffix(cgc_suffix_list* list, size_t index)
{
  if (index >= list->size)
    err("Bad cgc_suffix list access");
  return list->suffixes[index];
}

int cgc_cmp_suffix(cgc_suffix* l, cgc_suffix* r)
{
  return cgc_strcmp(l->input, r->input);
}

cgc_suffix_list* cgc_merge(cgc_suffix_list* l, cgc_suffix_list* r)
{
  size_t l_len = l->size;
  size_t r_len = r->size;
  size_t l_idx, r_idx, idx;
  cgc_suffix_list* result = cgc_make_suffix_list(l_len + r_len);

  l_idx = r_idx = idx = 0;

  while (l_idx < l_len && r_idx < r_len) {
    if (cgc_cmp_suffix(l->suffixes[l_idx], r->suffixes[r_idx]) <= 0) {
      cgc_append_suffix_list(result, cgc_get_suffix(l, l_idx));
      idx++;
      l_idx++;
    } else {
      cgc_append_suffix_list(result, cgc_get_suffix(r, r_idx));
      idx++;
      r_idx++;
    }
  }

  while (l_idx < l_len) {
      cgc_append_suffix_list(result, cgc_get_suffix(l, l_idx));
      idx++;
      l_idx++;
  }

  while (r_idx < r_len) {
      cgc_append_suffix_list(result, cgc_get_suffix(r, r_idx));
      idx++;
      r_idx++;
  }

  return result;
}

cgc_suffix_list* cgc_merge_sort(cgc_suffix_list* input)
{
  size_t length = input->size;

  if (length <= 1)
    return input;

  cgc_suffix_list* l = cgc_make_suffix_list(length);
  cgc_suffix_list* r = cgc_make_suffix_list(length);

  size_t middle = length / 2;

  for (size_t i = 0; i < middle; i++)
    cgc_append_suffix_list(l, cgc_get_suffix(input, i));

  for (size_t i = middle; i < length; i++)
    cgc_append_suffix_list(r, cgc_get_suffix(input, i));

  cgc_suffix_list* ls = cgc_merge_sort(l);
  cgc_suffix_list* rs = cgc_merge_sort(r);

  if (ls != l)
    cgc_free(l);
  if (rs != r)
    cgc_free(r);

  return cgc_merge(ls, rs);
}

size_t* cgc_build_suffix_array(char* input)
{
  size_t length = cgc_strlen(input);
  size_t* sa = cgc_mcalloc(sizeof(size_t) * length);

  cgc_suffix_list* list = cgc_make_suffix_list(length);
  for (size_t i = 0; i < length; i++)
    cgc_append_suffix_list(list, cgc_make_suffix(i, input + i));

  list = cgc_merge_sort(list);

  for (size_t i = 0; i < length; i++) {
    sa[i] = cgc_get_suffix(list, i)->index;
  }
  return sa;
}

/*
 * match = string we want to find
 * input = string in which we want to find match
 * sa = cgc_suffix array
 */
cgc_ssize_t cgc_search(char* match, size_t match_len, size_t* sa, char* input, size_t input_len, cgc_ssize_t max_index)
{
  if (!match || !match_len || !sa || !input || !input_len)
    return -1;

  size_t l = 0, r = input_len - 1, mid;

  while (l <= r) {
    mid = l + ((r - l) / 2);

    int cmp = cgc_strncmp(match, input + sa[mid], match_len);

    if (cmp == 0) {
      if (max_index >= 0 && sa[mid] + match_len >= max_index)
        return -1;
      return sa[mid];
    } else if (cmp < 0)
      r = mid - 1;
    else
      l = mid + 1;
  }

  return -1;
}

static inline size_t cgc_clamp_sub(size_t x, size_t y, size_t cgc_min)
{
  if (x - y > x)
    return cgc_min;
  else
    return x - y;
}

static inline size_t cgc_min(a, b) {
  return a > b ? b : a;
}

size_t cgc_prefix_len(const char* s1, const char* s2, const char* end)
{
  size_t max = 0, n = 0;
  if (s1 < s2) {
    max = s2 - s1;
    if (end - s2 < max)
      max = end - s2;
  } else {
    max = s1 - s2;
    if (end - s1 < max)
      max = end - s1;
  }

#define BLOCK_SIZE (8)

  size_t rounded = max & ~(BLOCK_SIZE - 1);

  while (n < rounded &&
      *s1++ == *s2++ &&
      *s1++ == *s2++ &&
      *s1++ == *s2++ &&
      *s1++ == *s2++ &&
      *s1++ == *s2++ &&
      *s1++ == *s2++ &&
      *s1++ == *s2++ &&
      *s1++ == *s2++
  )
    n += BLOCK_SIZE;

  while (n < max && *s1 && *s2 && *s1 == *s2)
    n++, s1++, s2++;

  return n;
}

#define WINDOW_SIZE (32 * 1024)
size_t cgc_compress(char *input, size_t input_len, char *output, size_t output_len, size_t* sa)
{
  cgc_lz_elem_t* lzelem;
  cgc_any_list_t* lzelem_list = cgc_any_list_alloc(NULL);

  cgc_writer_t *writer = cgc_writer_new(output, output_len);

  lzelem = cgc_lz_literal_new(*input);
  cgc_any_list_t* new_elem = cgc_any_list_alloc(lzelem);
  any_list_add_tail(new_elem, lzelem_list);

  for (size_t i = 1; i < input_len;) {
    size_t max_match = 1;
    size_t best_len = 0;
    cgc_ssize_t best_ret = -1;
    while (max_match <= i) {
      cgc_ssize_t ret = cgc_search(input + i, max_match, sa, input, input_len, i);
      if (ret <= 0)
        break;

      size_t match_len = cgc_prefix_len(input + ret, input + i, input + input_len);

      if (match_len > best_len) {
        best_len = match_len;
        best_ret = ret;
      }

      max_match++;
    }

    if (best_ret < 0 || best_len < 8)
      lzelem = cgc_lz_literal_new(input[i]);
    else
      lzelem = cgc_lz_backpointer_new(i - best_ret, best_len);

    new_elem = cgc_any_list_alloc(lzelem);
    any_list_add_tail(new_elem, lzelem_list);
    i += lzelem->len;
  }

  cgc_any_list_t* cur;
  for (cur = any_list_first(lzelem_list); cur != lzelem_list; cur = cur->next) {
    lzelem = cur->data;
    if (lzelem->type == LITERAL) {
      if (writer->offset + sizeof(char) > writer->cap)
        err("Doesn't cgc_compress enough");
      cgc_writer_write(writer, (cgc_u8 *)&lzelem->c, 1);
    } else {
      cgc_u32 packed = cgc_pack_lzelem_bp(lzelem);
      if (writer->offset + sizeof(packed) > writer->cap)
        err("Doesn't cgc_compress enough");
      cgc_writer_write(writer, &packed, sizeof(packed));
    }
    cgc_print_lzelem(STDIN, lzelem);
  }

  cgc_writer_write(writer, (cgc_u8*)&end_marker, sizeof(end_marker));

  size_t ret =  writer->offset;
  cgc_free(writer);
  for (cur = any_list_first(lzelem_list); cur != lzelem_list;) {
    cgc_any_list_t* tmp = cur->next;;
    if (cur->data)
      cgc_free(cur->data);
    cgc_free(cur);
    cur = tmp;
  }

  return ret;
}

size_t cgc_decompress(char* input, char *output, size_t output_size)
{
  size_t in_idx = 0;
  cgc_writer_t *writer = cgc_writer_new(output, output_size);

  while (1) {
    if (cgc_memcmp(&end_marker, input + in_idx, sizeof(end_marker)) == 0)
      break;

    if ((*(input + in_idx)) & 0x80) {
      cgc_lz_elem_t* elem = cgc_unpack_lzelem_bp(*(cgc_u32*)(input + in_idx));
      if (elem->back > writer->offset)
        err("Bad back pointer %d", elem->back);

#ifdef PATCHED
      if (writer->offset + elem->len > writer->cap)
        err("Expands too much");
#endif
      cgc_writer_write(writer, writer->buf + writer->offset - elem->back, elem->len);

      in_idx += sizeof(cgc_u32);
    } else {
#ifdef PATCHED
      if (writer->offset + 1 > writer->cap)
        err("Expands too much");
#endif
      cgc_writer_write(writer, input + in_idx, 1);
      in_idx++;
    }
  }

  size_t ret =  writer->offset;
  cgc_free(writer);
  return ret;
}


extern char* padding;
int main(void)
{
#define COMPRESSED_SZ (32 * 1024 + 4)
  char compressed[COMPRESSED_SZ];
#define DECOMPRESSED_SZ (64 * 1024)
  char decompressed[DECOMPRESSED_SZ];

  while (1) {
    char* action = cgc_readline(STDIN);
    if (!action)
      return -1;

    if (cgc_strcmp(action, "cgc_compress") == 0) {
      char* input = cgc_readline(STDIN);
      char* filtered_input = cgc_alnumspc_filter(input);
      cgc_free(input);
      cgc_free(action);
      if (!filtered_input)
        return -1;

      if (cgc_strlen(filtered_input) > DECOMPRESSED_SZ)
        return -1;

      size_t* sa = cgc_build_suffix_array(filtered_input);
      if (!sa)
        return -1;

      size_t compressed_sz = cgc_compress(filtered_input, cgc_strlen(filtered_input), compressed, COMPRESSED_SZ, sa);
      cgc_send_n_bytes(STDOUT, compressed_sz, compressed);

      cgc_free(filtered_input);
      cgc_free(sa);

    } else if (cgc_strcmp(action, "cgc_decompress") == 0) {
      char* input = cgc_read_until_sequence(STDIN, (char*)&end_marker, 4);
      if (!input)
        return -1;
      cgc_memset(decompressed, 0, DECOMPRESSED_SZ);
      size_t decompressed_sz = cgc_decompress(input, decompressed, DECOMPRESSED_SZ);
      cgc_send_n_bytes(STDOUT, decompressed_sz, decompressed);

      cgc_free(action);
      cgc_free(input);

    } else if (cgc_strcmp(action, "quit") == 0) {
      break;
    }
  }
  return 0;
}
