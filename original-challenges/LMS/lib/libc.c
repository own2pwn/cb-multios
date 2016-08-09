/*
 * Copyright (C) Narf Industries <info@narfindustries.com>
 *
 * Permission is hereby granted, cgc_free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include <libcgc.h>
#include "libc.h"

bool heapinit_done = false;
cgc_heap_chunk_t freedc = {0};
cgc_heap_chunk_t allocatedc = {0};
cgc_heap_chunk_t *freed = NULL;
cgc_heap_chunk_t *allocated = NULL;

uint8_t *lastpage = NULL;
uint32_t curleft = 0;

uint32_t __cookie = 0;

void cgc_promptc(char *buf, uint16_t  size, char *prompt) {

    SSEND(cgc_strlen(prompt), prompt);

    SRECV((uint32_t)size, buf);
}

int cgc_sendall(int fd, const char *buf, size_t size) {
    size_t sent = 0;
    size_t total = 0;

    if (!buf)
        return -1;

    if (!size)
        return 0;

    while (size) {
        if (transmit(fd, buf, size, &sent)) {
            return -1;
        }

        buf += sent;
        total += sent;
        size -= sent;
    }

    return total;
}

int cgc_sendline(int fd, const char *buf, size_t size) {
    int ret;
    ret = cgc_sendall(fd, buf, size);
    if (ret < 0) {
        return ret;
    } else {
        if (transmit(fd, "\n", 1, &size))
            return -1;
        else
            return ++ret;
    }
}

int cgc_recv(int fd, char *buf, size_t size) {
    size_t bytes_read = 0;
    size_t total_read = 0;

    if (!size)
        return 0;

    if (!buf)
        return -1;

    while (size) {
        if (receive(fd, buf++, 1, &bytes_read))
            return -1;
        
        total_read++;
        size--;
    }

    return total_read;

}
int cgc_recvline(int fd, char *buf, size_t size) {
    size_t bytes_read = 0;
    size_t total_read = 0;

    if (!size)
        return 0;

    if (!buf)
        return -1;

    while (size) {
        if (receive(fd, buf++, 1, &bytes_read))
            return -1;
        
        total_read++;
        size--;
        
        if (*(buf-1) == '\n')
            break;
    }

    if (*(buf-1) != '\n')
        return -2;
    else
        *(buf-1) = '\0';

    return total_read;
}

//non-standard convention, returns num bytes copied instead of s1
size_t cgc_strcpy(char *s1, char *s2) {
    char *tmp = s1;
    while (*s2) {
        *tmp = *s2;
        tmp++;
        s2++;
    }
    *(tmp++)='\0';
    return tmp-s1-1;
}

//non-standard convention, returns num bytes copied instead of s1
size_t cgc_strncpy(char *s1, char *s2, size_t n) {
    char *tmp = s1;
    while ((tmp-s1 < n) && *s2) {
        *tmp = *s2;
        tmp++;
        s2++;
    }
    *(tmp++)='\0';
    return tmp-s1-1;
}

char * cgc_strcat(char *s1, char *s2) {
    char *tmp = s1;
    while (*tmp) tmp++;
    cgc_strcpy(tmp,s2);
    return s1;
}

size_t cgc_strlen(char *s) {
    char *tmp = s;
    while (*tmp) tmp++;
    return (size_t)(tmp-s);
}

int cgc_streq(char *s1, char *s2) {
    while (*s1 && *s2){
        if (*s1 != *s2)
            return 0;
	    s1++;
	    s2++;
    }
    return (*s1 == '\0') && (*s2 == '\0');
}

int cgc_startswith(char *s1, char *s2) {
    while (*s1 && *s2) {
        if (*s1 != *s2)
            return 0;
	    s1++;
	    s2++;
    }
    return *s2 == '\0';
}

// takes a uint32 and converts it to a string saved in str_buf
// str_buf must be large enough to fit the number(s) and '\0'
// returns 0 on success, -1 if error due to buf_size
int cgc_uint2str(char* str_buf, int buf_size, uint32_t i) {

    int idx = 0;
    uint32_t tmp;
    int digit;

    // at least fits 1 digit and '\0'
    if (buf_size < 2) {
        return -1;
    }

    tmp = i;

    // increment index in str_buf to where rightmost digit goes
    do {
        idx++;
        tmp = tmp/10;
    } while (tmp > 0);

    // see if this number will fit in the buffer
    if (idx >= buf_size)
        return -1;

    //testing
    // str_buf[0] = '0' - i;
    // str_buf[1] = '\0';

    // cgc_insert '\0'
    str_buf[idx--] = '\0';

    // move left through string, writing digits along the way
    do {
        digit = i % 10;
        str_buf[idx--] = '0' + digit;
        i /= 10;
    } while (i > 0);

    return 0;
}

// takes an int32 and converts it to a string saved in str_buf
// str_buf must be large enough to fit the sign, number(s), and '\0'
// returns 0 on success, -1 if error due to buf_size
int cgc_int2str(char* str_buf, int buf_size, int i) {

    int idx = 0;
    int tmp;
    int digit;

    // at least fits 1 digit and '\0'
    if (buf_size < 2) {
        return -1;
    }

    if (i < 0) {
        if (buf_size < 3)
            return -1;
        str_buf[idx++] = '-';
    } else {
        i *= -1; // make i negative
    }

    // i is always 0 or negative at this point.
    tmp = i;

    // increment index in str_buf to where rightmost digit goes
    do {
        idx++;
        tmp = tmp/10;
    } while (tmp < 0);

    // see if this number will fit in the buffer
    if (idx >= buf_size)
        return -1;

    //testing
    // str_buf[0] = '0' - i;
    // str_buf[1] = '\0';

    // cgc_insert '\0'
    str_buf[idx--] = '\0';

    // move left through string, writing digits along the way
    do {
        digit = -1 * (i % 10);
        str_buf[idx--] = '0' + digit;
        i /= 10;
    } while (i < 0);

    return 0;
}

// takes a string and converts it to an uint32
// MAX uint32 is +/- 2^31-1 (2,147,483,647) which is 10 digits
// returns 0 if str_buf is "0" or has no digits.
uint32_t cgc_str2uint(const char* str_buf) {
    int result = 0;
    int max_chars = 10; // max number of chars read from str_buf
    int i = 0;

    if (str_buf == NULL)
        return result;

    for (; i < max_chars; i++) {
        if ( str_buf[i] >= '0' && str_buf[i] <= '9') {
            result *= 10;
            result += str_buf[i] - '0';

        } else {
            break;
        }
    }

    return result;
}

void * cgc_memset(void *dst, char c, size_t n) {
    size_t i;
    for (i=0; i<n; i++) {
        *((uint8_t*)dst+i) = c;
    }
    return dst;
}

void * cgc_memcpy(void *dst, void *src, size_t n) {
    size_t i;
    for (i=0; i<n; i++) {
        *((uint8_t*)dst+i) = *((uint8_t*)src+i);
    }
    return dst;
}

char * cgc_b2hex(uint8_t b, char *h) {
    if (b>>4 < 10)
        h[0] = (b>>4)+0x30;
    else
        h[0] = (b>>4)+0x41-10;

    if ((b&0xf) < 10)
        h[1] = (b&0xf)+0x30;
    else
        h[1] = (b&0xf)+0x41-10;
    h[2] = '\0';
    return h;
}

char * cgc_strchr(char *str, char c) {
    char *tmp = str;
    while (*tmp) {
        if (*tmp == c)
            return tmp;
        tmp++;
    }
    return 0;
}

//modulus
int cgc___umoddi3(int a, int b) {
    return a-(a/b*b);
}

void cgc_sleep(int s) {
    struct cgc_timeval tv;
    tv.tv_sec = s;
    tv.tv_usec = 0;
    cgc_fdwait(0, NULL, NULL, &tv, NULL);
}

int cgc_memcmp(void *a, void *b, size_t n) {
    size_t i;
    for (i=0; i < n; i++)
        if ( *(uint8_t*)(a+i) != *(uint8_t*)(b+i))
            return -1;
    return 0;
}

static void cgc_heapinit() {
    allocated = &allocatedc;
    freed = &freedc;
    allocated->next = allocated;
    allocated->prev = allocated;
    freed->next = freed;
    freed->prev = freed;
    heapinit_done = true;
}

static void cgc_insert(cgc_heap_chunk_t *head, cgc_heap_chunk_t *node) {
    node->next = head;
    node->prev = head->prev;
    node->prev->next = node;
    head->prev = node;
}

static void cgc_remove(cgc_heap_chunk_t *node) {
    node->prev->next = node->next;
    node->next->prev = node->prev;
    node->next = NULL;
    node->prev = NULL;
}

void cgc_checkheap() {
    /*
     * Verify that there is no overlap between freed and allocated lists.
     */
    cgc_heap_chunk_t *fchunk = freed;
    cgc_heap_chunk_t *achunk;
    while (fchunk->next != freed) {
        achunk = allocated;
        while (achunk->next != allocated) {
            if ((uint32_t)fchunk < (uint32_t)(((uint8_t*)achunk)+achunk->size) &&
                (uint32_t)achunk < (uint32_t)(((uint8_t*)fchunk)+fchunk->size)) {
                LOG("corrupt");
                _terminate(283);
            }
            achunk = achunk->next;
        }
        fchunk = fchunk->next;
    }
}

void *cgc_malloc(size_t size) {
    /*
     * A very stupid cgc_malloc implementation, meant to be simple.
     * Keeps a list of allocated and freed chunks
     * Alloc walks list of freed chunks to see if any are large enough
     * If not, it allocates space large enough to store
     * Oh, and we never actually cgc_free pages. It's quality software.
     *
     */
    if (!heapinit_done) 
        cgc_heapinit();

    if (size == 0)
        return NULL;

    cgc_heap_chunk_t *chunk = freed;
    //need space for inline metadata
    size += sizeof(cgc_heap_chunk_t);

    //walk freed list to see if we can find match
    while (chunk->size < size && chunk->next != freed) {
        chunk = chunk->next;
    }

    if (chunk->size >= size) {
        //found a match, cgc_remove from freed list, add to allocated list, and return
        //SSSENDL("found cgc_free chunk");
        cgc_remove(chunk);
        cgc_insert(allocated,chunk);
        return ((uint8_t*)chunk)+sizeof(cgc_heap_chunk_t);
    }

    //see if cgc_free space in last allocated page is enough
    if (size <= curleft) {
        //SSSENDL("had enough left in current page");
        chunk = (cgc_heap_chunk_t*)lastpage;
        chunk->size = size;
        lastpage += size;
        curleft -= size;
        cgc_insert(allocated,chunk);
        return ((uint8_t*)chunk)+sizeof(cgc_heap_chunk_t);
    }

    //need to allocate new page

    //SSSENDL("allocating new page");
    //first add the remaining page to our freed list as a lazy hack
    //if there's not enough left, we just let it leak
    if (curleft > sizeof(cgc_heap_chunk_t)) {
        //SSSENDL("adding remainder to cgc_free list");
        chunk = (cgc_heap_chunk_t*)lastpage;
        chunk->size = curleft;
        cgc_insert(freed,chunk);
    }

    if (allocate(size,0,(void**)&chunk) != 0)
        return NULL;

    chunk->size = size;
    cgc_insert(allocated,chunk);

    lastpage = ((uint8_t*)chunk)+size;
    //this is bad.
    if ((size & 0xfff) != 0)
        curleft = PAGE_SIZE-(size&(PAGE_SIZE-1));
    else
        curleft = 0;
    return ((uint8_t*)chunk)+sizeof(cgc_heap_chunk_t);
}

void *cgc_calloc(size_t size) {
    void *ptr;

    if (!(ptr = cgc_malloc(size)))
        return NULL;

    cgc_memset(ptr,'\0',size);
    return ptr;
}

void cgc_free(void *p) {
    /*
     * A very stupid cgc_free for a very stupid cgc_malloc
     * Simply moves pointer from allocated to cgc_free list
     * With no checking of anything, obviously
     *
     */
    if (!p)
        return;

    cgc_heap_chunk_t *chunk = (cgc_heap_chunk_t*)((uint8_t*)p - sizeof(cgc_heap_chunk_t));

    //fix allocated list
    cgc_remove(chunk);

    //add chunk to the freed list
    cgc_insert(freed,chunk);
    return;
}

void cgc___stack_cookie_init() {
    RAND(&__cookie, sizeof(__cookie), NULL);
}

void cgc___stack_cookie_fail() {
    SSENDL(sizeof(COOKIEFAIL)-1,COOKIEFAIL);
    _terminate(66);
}
