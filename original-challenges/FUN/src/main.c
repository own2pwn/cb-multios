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
#include <stdlib.h>
#include <ctype.h>
#include <stddef.h>
#include <stdint.h>

#include "text.h"
#include "words_r.h"

#define INTPTR_MAX INT32_MAX
#define UINTPTR_MAX UINT32_MAX

#define CALL(x, ...) ((cgc_funptr_t)((cgc_uintptr_t)funcs[(x)] - secret_key))(__VA_ARGS__)
#define STATIC_SECRET_KEY 0x12345678
cgc_uintptr_t secret_key;
typedef cgc_uintptr_t (*cgc_funptr_t) (cgc_uintptr_t, ...);
cgc_funptr_t funcs[];
enum {
    PROTECT,
    CALLOC,
    FREE,
    STRDUP,
    STRCPY,
    MEMCPY,
    STRNCMP,
    STRCMP,
    BYTECMP,
    TO_LOWER,
    SEARCH_WORDS,
    SORT_WORDS,
    ADD_WORD,
    VERIFY_WORD,
    ANALYZE_TEXT,
    GENERATE_TEXT,
    RANDOM_DICT_WORD,
    RANDOM_WORD,
    RANDINT,
    INIT,
    STRNCPY,
    STRLEN,
    IS_ALPHA,
    PRINTF,
    READ,
    READ_ALL,
    READ_UINT,
    READ_STRING,
    WRITE_UINT,
    WRITE_ERROR,
    WRITE_STRING,
    HASH_STRING,
    HTBL_INIT,
    HTBL_ENLARGE,
    HTBL_GET,
    HTBL_SET,
    HTBL_FREE,
    HTBL_FIRST,
    HTBL_NEXT,
    FUNCS_END
};

enum {
    CMD_QUIT = 0,
    CMD_REPROTECT = 0x35897932,
    CMD_ADD_WORD = 0x38462643,
    CMD_SORT_WORDS = 0x38327950,
    CMD_GENERATE_TEXT = 0x2884,
    CMD_TRAINING = 0x1971,
    CMD_SEARCH_WORD = 0x69399375,
    CMD_SEARCH_PARTIAL = CMD_SEARCH_WORD + 1
    // = 0x10582097
    // = 0x49445923
};

enum {
    ERROR_OK = 0,
    ERROR_INVAL = 1,
    ERROR_NOMEM = 2,
    ERROR_NOTFOUND = 3
};

typedef struct htbl_node {
    struct htbl_node *next;
    struct htbl_node *prev;
    char *key;
    cgc_uintptr_t value;
} cgc_htbl_node_t;

typedef struct {
    cgc_size_t i;
    cgc_htbl_node_t *cur;
} cgc_htbl_iter_t;

typedef struct {
    unsigned int count;
    unsigned int size;
    cgc_htbl_node_t **nodes;
} cgc_htbl_t;

static void cgc_my_to_lower(char *s)
{
    cgc_size_t i;
    for (i = 0; s[i] != 0; i++)
        s[i] |= 0x20;
}

static void cgc_my_strcpy(char *dst, const char *src)
{
    cgc_size_t length = CALL(STRLEN, src);
    CALL(MEMCPY, dst, src, length + 1);
}

static void cgc_my_memcpy(char *dst, char *src, cgc_size_t length)
{
    cgc_size_t i;
    for (i = 0; i != length; i++)
        dst[i] = src[i];
}

static cgc_uintptr_t cgc_my_hash_string(const char *s, cgc_uintptr_t tbl_size)
{
    cgc_size_t i;
    cgc_uintptr_t result = 0xDEADBEEF;
    for (i = 0; s[i] != 0; i++)
        result = (result * 13) ^ s[i];

    return result % tbl_size;
}

static cgc_intptr_t cgc_my_htbl_init(cgc_htbl_t *tbl, cgc_uintptr_t initial_size)
{
    tbl->count = 0;
    tbl->size = initial_size;
    tbl->nodes = CALL(CALLOC, tbl->size, sizeof(cgc_htbl_node_t *));

    if (tbl->nodes == NULL)
        return 0;

    return 1;
}

static void cgc_my_htbl_enlarge(cgc_htbl_t *tbl)
{
    cgc_size_t i, new_size = (tbl->size + 1) * 2;
    cgc_htbl_node_t **new_tbl;

    new_tbl = CALL(CALLOC, new_size, sizeof(cgc_htbl_node_t *));
    if (new_tbl == NULL)
        return;

    for (i = 0; i < tbl->size; i++)
    {
        cgc_htbl_node_t *node, *next_node;
        for (node = tbl->nodes[i]; node != NULL; node = next_node)
        {
            cgc_uintptr_t hash = CALL(HASH_STRING, node->key, new_size);
            next_node = node->next;

            node->next = new_tbl[hash];
            node->prev = NULL;
            if (node->next)
                node->next->prev = node;
            new_tbl[hash] = node;
        }
    }

    CALL(FREE, tbl->nodes);
    tbl->nodes = new_tbl;
    tbl->size = new_size;
}

static void cgc_my_htbl_first(cgc_htbl_t *tbl, cgc_htbl_iter_t *iter)
{
    cgc_size_t i;
    for (i = 0; i < tbl->size; i++)
        if (tbl->nodes[i] != NULL)
            break;

    iter->i = i;
    iter->cur = tbl->nodes[i];
}

static void cgc_my_htbl_next(cgc_htbl_t *tbl, cgc_htbl_iter_t *iter)
{
    if (iter->cur == NULL)
        return;

    if (iter->cur->next != NULL)
    {
        iter->cur = iter->cur->next;
    }
    else
    {
        cgc_size_t i;
        for (i = iter->i + 1; i < tbl->size; i++)
            if (tbl->nodes[i] != NULL)
                break;
        iter->i = i;

        if (iter->i == tbl->size)
            iter->cur = NULL;
        else
            iter->cur = tbl->nodes[i];
    }
}

static inline cgc_htbl_node_t *cgc_find_node(cgc_htbl_t *tbl, const char *key)
{
    cgc_uintptr_t hash = CALL(HASH_STRING, key, tbl->size);
    cgc_htbl_node_t *node;

    for (node = tbl->nodes[hash]; node != NULL; node = node->next)
        if (CALL(STRCMP, key, node->key) == 0)
            break;

    return node;
}

static cgc_uintptr_t cgc_my_htbl_get(cgc_htbl_t *tbl, const char * key)
{
    cgc_htbl_node_t *node = cgc_find_node(tbl, key);

    if (node == NULL)
        return NULL;
    else
        return node->value;
}

static cgc_uintptr_t cgc_my_htbl_set(cgc_htbl_t *tbl, const char *key, cgc_uintptr_t value)
{
    cgc_htbl_node_t *node = cgc_find_node(tbl, key);
    if (node == NULL)
    {
        cgc_uintptr_t hash;
        if (tbl->count * 2 >= tbl->size)
            CALL(HTBL_ENLARGE, tbl);

        if (tbl->nodes == NULL)
            return NULL;

        node = cgc_malloc(sizeof(cgc_htbl_node_t));
        if (node == NULL)
            return NULL;

        node->key = CALL(STRDUP, key);
        if (node->key == NULL)
        {
            cgc_free(node);
            return NULL;
        }
        node->value = value;

        hash = CALL(HASH_STRING, key, tbl->size);
        node->next = tbl->nodes[hash];
        node->prev = NULL;
        if (node->next)
            node->next->prev = node;
        tbl->nodes[hash] = node;
        tbl->count++;
        return NULL;
    }
    else
    {
        cgc_uintptr_t retval = node->value;
        node->value = value;
        return retval;
    }
}

static void cgc_my_htbl_free(cgc_htbl_t *tbl, cgc_uintptr_t free_value)
{
    cgc_size_t i;

    for (i = 0; i < tbl->size; i++)
    {
        cgc_htbl_node_t *node, *next_node;
        for (node = tbl->nodes[i]; node != NULL; node = next_node)
        {
            next_node = node->next;
            CALL(free_value, node->value);
            CALL(FREE, node);
        }
    }

    CALL(FREE, tbl->nodes);
    tbl->nodes = NULL;
    tbl->size = 0;
    tbl->count = 0;
}

static char *cgc_my_strdup(const char *s)
{
    cgc_size_t length = CALL(STRLEN, s);
    char *str = CALL(CALLOC, length + 1, 1);
    if (str == NULL)
        return NULL;
    CALL(STRCPY, str, s);
    return str;
}

static cgc_intptr_t cgc_my_strlen(const char *s)
{
    cgc_size_t i;
    for (i = 0; s[i] != 0; i++) ;
    return i;
}

static void cgc_my_strncpy(char * dest, const char * src, cgc_intptr_t len)
{
    cgc_size_t i;
    for (i = 0; i < len; i++)
    {
        dest[i] = src[i];
        if (src[i] == 0)
            break;
    }
}

static cgc_intptr_t cgc_my_strncmp(char * a, char * b, cgc_intptr_t len)
{
    cgc_size_t i;
    for (i = 0; i < len && a[i] != 0 && b[i] != 0; i++)
    {
        int result = CALL(BYTECMP, a[i], b[i]);
        if (result != 0)
            return result;
    }

    if (i == len)
        return 0;

    return CALL(BYTECMP, a[i], b[i]);
}

static cgc_intptr_t cgc_my_strcmp(char * a, char * b)
{
    cgc_size_t i;
    for (i = 0; a[i] != 0 && b[i] != 0; i++)
    {
        int result = CALL(BYTECMP, a[i], b[i]);
        if (result != 0)
            return result;
    }

    return CALL(BYTECMP, a[i], b[i]);
}

static cgc_intptr_t cgc_my_bytecmp(cgc_intptr_t a, cgc_intptr_t b)
{
    return a - b;
}

static cgc_intptr_t cgc_my_search_words(const char *needle, int partial)
{
    int len = CALL(STRLEN, needle);
    cgc_size_t i;
    cgc_uintptr_t f = partial ? STRNCMP : STRCMP;
    for (i = 0; words[i] != NULL; i++)
        if (CALL(f, needle, words[i], len) == 0)
            return i;

    return -1;
}

static void cgc_my_sort_words(cgc_uintptr_t comparator, cgc_intptr_t reverse)
{
    int done;
    cgc_size_t i;

    do {
        done = 1;
        for (i = 1; words[i] != NULL; i++)
        {
            int result = CALL(comparator, words[i-1], words[i]);
            if ((!reverse && result > 0) || (reverse && result < 0))
            {
                const char *tmp = words[i];
                words[i] = words[i-1];
                words[i-1] = tmp;
                done = 0;
            }
        }
    } while (!done);
}

static cgc_intptr_t cgc_my_verify_word(const char *word)
{
    int len = CALL(STRLEN, word), i;
    
    // check that it contains a non-trivial root word
    for (i = 0; i < len - 3; i++)
    {
        char root[4];
        CALL(STRNCPY, root, word, 3);
        root[3] = 0;

        if ((cgc_intptr_t)CALL(SEARCH_WORDS, root, 1) >= 0)
            break;
    }
    if (i == len - 3)
        return 0;

    // check that word doesn't already exist
    if ((cgc_intptr_t)CALL(SEARCH_WORDS, word, 0) >= 0)
        return 0;

    return 1;
}

static cgc_intptr_t cgc_my_add_word(const char *word)
{
    cgc_size_t i;
    if (CALL(VERIFY_WORD, word) == 0)
        return 0;

    for (i = 0; words[i] != NULL; i++) ;

#ifdef PATCHED
    if (i >= sizeof(words) / sizeof(words[0]))
        return 0;
#endif

    words[i] = CALL(STRDUP, word);
    words[i+1] = NULL;
    return 1;
}

static const char *cgc_my_random_dict_word()
{
    cgc_size_t i, r;
    for (i = 0; words[i] != NULL; i++) ;
    r = CALL(RANDINT, 0, i);
    return words[r];
}

cgc_uint32_t cgc_tornado();
static cgc_uintptr_t cgc_my_randint(cgc_uintptr_t min, cgc_uintptr_t max)
{
    cgc_size_t bytes;
    cgc_uintptr_t rand, range = max - min;

    int fd;
try_again:
#if 0
    if (cgc_random(&rand, sizeof(rand), &bytes) != 0 || bytes != sizeof(rand))
        return min;
#endif
    rand = cgc_tornado();
    
    cgc_uintptr_t q = UINTPTR_MAX - (UINTPTR_MAX % range);
    if (rand >= q)
        goto try_again;

    return min + (rand % range);
}

static const char *cgc_my_random_word(cgc_htbl_t *tbl, cgc_uintptr_t cur_length)
{
    cgc_size_t count = 0;
    cgc_uintptr_t rand;
    cgc_htbl_iter_t iter;

    for (CALL(HTBL_FIRST, tbl, &iter); iter.cur != NULL; CALL(HTBL_NEXT, tbl, &iter))
        count += iter.cur->value;

    if (count == 0)
        return NULL;

    rand = CALL(RANDINT, 0, count);
    for (CALL(HTBL_FIRST, tbl, &iter); iter.cur != NULL; CALL(HTBL_NEXT, tbl, &iter))
    {
        if (rand < iter.cur->value)
            break;
        rand -= iter.cur->value;
    }

    // end the sentence if possible and cur_length is large
    if (CALL(HTBL_GET, tbl, "") != 0)
    {
        rand = CALL(RANDINT, 0, 10);
        if (cur_length >= 30)
        {
            if (rand >= 3)
                return "";
        }
        else if (cur_length >= 20)
        {
            if (rand >= 5)
                return "";
        }
        else
        {
            if (rand >= 9)
                return "";
        }
    }

    return iter.cur->key;
}

static char *cgc_my_generate_text(cgc_htbl_t *tbl, int training)
{
    const char *prev = "", *word, *outword;
    char tmp[128];
    cgc_size_t i = 0;
    cgc_uintptr_t len;

    do {
        word = CALL(RANDOM_WORD, CALL(HTBL_GET, tbl, prev), i);
        if (word == NULL)
            return NULL;

        if (!training || CALL(RANDINT, 0, 10) != 0)
            outword = word;
        else
            outword = CALL(RANDOM_DICT_WORD, 0);

        len = CALL(STRLEN, outword);
        if (i + len + 5 >= sizeof(tmp))
            break;

        if (i == 0)
        {
            CALL(MEMCPY, &tmp[i], outword, len);
            tmp[i] &= ~0x20; // make upper case
        }
        else
        {
            if (CALL(STRCMP, outword, "s") == 0)
                tmp[i++] = '\'';
            else if (CALL(STRCMP, outword, "") != 0)
                tmp[i++] = ' ';
            CALL(MEMCPY, &tmp[i], outword, len);
        }

        prev = word;
        i += len;
    } while (CALL(STRCMP, prev, "") != 0);

    if (i + len + 5 >= sizeof(tmp))
    {
        tmp[i++] = '.';
        tmp[i++] = '.';
        tmp[i++] = '.';
    }
    else
    {
        tmp[i++] = '.';
    }
    tmp[i++] = 0;
    return CALL(STRDUP, tmp);
}

static void cgc_my_analyze_text(cgc_htbl_t *tbl, const char *text)
{
    char word[64], prev[64];
    cgc_uintptr_t next_tbl;
    const char *s;

    prev[0] = 0;
    for (s = text; *s != 0; )
    {
        const char *start, *end;
        int has_period = 0;

        while (*s != 0 && CALL(IS_ALPHA, *s) == 0)
            s++;
        if (*s == 0) break;

        start = s;
        while (*s != 0 && CALL(IS_ALPHA, *s) != 0)
            s++;
        end = s;

        if (end-start >= sizeof(word))
            continue;

        while (*s != 0 && CALL(IS_ALPHA, *s) == 0)
        {
            if (*s++ == '.')
            {
                has_period = 1;
                break;
            }
        }

        CALL(MEMCPY, word, start, end-start);
        word[end-start] = 0;
        CALL(TO_LOWER, word);

add_next_word:
        next_tbl = CALL(HTBL_GET, tbl, prev);
        if (next_tbl == 0)
        {
            next_tbl = CALL(CALLOC, 1, sizeof(cgc_htbl_t));
            if (next_tbl == 0)
                continue;
            CALL(HTBL_INIT, next_tbl, 8);
            CALL(HTBL_SET, tbl, prev, next_tbl);
        }
        CALL(HTBL_SET, next_tbl, word, CALL(HTBL_GET, next_tbl, word) + 1);

        CALL(STRCPY, prev, word);
        if (has_period)
        {
            has_period = 0;
            word[0] = 0;
            goto add_next_word;
        }
    }

    if (word[0] != 0)
    {
        word[0] = 0;
        next_tbl = CALL(HTBL_GET, tbl, prev);
        if (next_tbl == 0)
        {
            next_tbl = CALL(CALLOC, 1, sizeof(cgc_htbl_t));
            if (next_tbl == 0)
                return;
            CALL(HTBL_INIT, next_tbl, 8);
            CALL(HTBL_SET, tbl, prev, next_tbl);
        }
        CALL(HTBL_SET, next_tbl, word, CALL(HTBL_GET, next_tbl, word) + 1);
    }
}

static void cgc_my_write_error(cgc_uintptr_t value)
{
    CALL(WRITE_UINT, value);
}

static void cgc_my_write_uint(cgc_uintptr_t value)
{
    cgc_uint8_t tmp[sizeof(value) + 1];
    int to_send;
    cgc_size_t bytes;

    if (value < 0x80)
    {
        tmp[0] = value;
        to_send = 1;
    }
    else if (value < 0x7F00)
    {
        tmp[0] = 0x80 | (value >> 8);
        tmp[1] = value;
        to_send = 2;
    }
    else
    {
        tmp[0] = 0xFF;
        *(cgc_uintptr_t *)&tmp[1] = value;
        to_send = 1 + sizeof(cgc_uintptr_t);
    }

    transmit(STDOUT, tmp, to_send, &bytes);
}

static void cgc_my_write_string(const char *s)
{
    cgc_uintptr_t len = CALL(STRLEN, s);
    cgc_size_t bytes;
    CALL(WRITE_UINT, len);
    transmit(STDOUT, s, len, &bytes);
}

static cgc_uintptr_t cgc_my_read_all(char *buf, cgc_uintptr_t len)
{
    cgc_size_t bytes;
    while (len > 0)
    {
        if (receive(STDIN, buf, len, &bytes) != 0 || bytes == 0)
            return 0;
        buf += bytes;
        len -= bytes;
    }
    return 1;
}

static cgc_intptr_t cgc_my_read_uint(cgc_uintptr_t *pvalue)
{
    cgc_uintptr_t value;
    cgc_uint8_t byte;
    if (!CALL(READ_ALL, &byte, 1))
        return 0;

    if (byte < 0x80)
    {
        value = byte;
    }
    else if (byte < 0xFF)
    {
        cgc_uint8_t byte2;
        if (!CALL(READ_ALL, &byte2, 1))
            return 0;
        value = ((byte & 0x7F) << 8) | byte2;
    }
    else
    {
        if (!CALL(READ_ALL, &value, sizeof(value)))
            return 0;
    }

    *pvalue = value;
    return 1;
}

static cgc_intptr_t cgc_my_read(char *buf, cgc_intptr_t max)
{
    cgc_uintptr_t rlen;
    
    if (!CALL(READ_UINT, &rlen))
        return -1;

    if (rlen > max)
        return -1;

    if (!CALL(READ_ALL, buf, rlen))
        return -1;

    return rlen;
}

static cgc_intptr_t cgc_my_read_string(char *buf, cgc_intptr_t max)
{
    cgc_intptr_t actual;
    if (max == 0)
        return 0;
    actual = CALL(READ, buf, max-1);
    if (actual < 0)
        return actual;
    buf[actual] = 0;
    return actual;
}

static void cgc_my_protect(cgc_uintptr_t new_key)
{
    cgc_size_t i;
    for (i = 0; i < FUNCS_END; i++)
        *(cgc_uintptr_t *)(&funcs[i]) += new_key - secret_key;
    secret_key = new_key;
}

static void cgc_my_init()
{
    secret_key = STATIC_SECRET_KEY;
}

int main()
{
    cgc_uintptr_t cmd, value;
    char buf[32], *tmp;
    cgc_htbl_t tbl;

    CALL(INIT, 0);
    CALL(PROTECT, 0xDEADBEEF);

    CALL(HTBL_INIT, &tbl, 128);
    CALL(ANALYZE_TEXT, &tbl, text);

    while (1)
    {
        if (!CALL(READ_UINT, &cmd))
            break;

        if (cmd == CMD_QUIT)
        {
            CALL(WRITE_ERROR, ERROR_OK);
            break;
        }

        switch (cmd)
        {
        case CMD_REPROTECT:
            if (CALL(READ_UINT, &value))
            {
                CALL(PROTECT, value);
                CALL(WRITE_ERROR, ERROR_OK);
            }
            else
            {
                CALL(WRITE_ERROR, ERROR_INVAL);
            }
            break;
        case CMD_ADD_WORD:
            if ((cgc_intptr_t)CALL(READ_STRING, buf, sizeof(buf)) >= 0)
            {
                if (CALL(ADD_WORD, buf))
                    CALL(WRITE_ERROR, ERROR_OK);
                else
                    CALL(WRITE_ERROR, ERROR_INVAL);
            }
            else
            {
                CALL(WRITE_ERROR, ERROR_INVAL);
            }
            break;
        case CMD_SORT_WORDS:
            if (CALL(READ_UINT, &value))
            {
                CALL(SORT_WORDS, STRCMP, value);
                CALL(WRITE_ERROR, ERROR_OK);
            }
            else
            {
                CALL(WRITE_ERROR, ERROR_INVAL);
            }
            break;
        case CMD_SEARCH_PARTIAL:
        case CMD_SEARCH_WORD:
            if ((cgc_intptr_t)CALL(READ_STRING, buf, sizeof(buf)) >= 0)
            {
                cgc_intptr_t idx = CALL(SEARCH_WORDS, buf, cmd == CMD_SEARCH_PARTIAL);
                if (idx < 0)
                {
                    CALL(WRITE_ERROR, ERROR_NOTFOUND);
                }
                else
                {
                    CALL(WRITE_ERROR, ERROR_OK);
                    CALL(WRITE_UINT, (cgc_uintptr_t)idx);
                }
            }
            else
            {
                CALL(WRITE_ERROR, ERROR_INVAL);
            }
            break;
        case CMD_GENERATE_TEXT:
            tmp = CALL(GENERATE_TEXT, &tbl, 0);
            if (tmp != NULL)
            {
                CALL(WRITE_ERROR, ERROR_OK);
                CALL(WRITE_STRING, tmp);
                CALL(FREE, tmp);
            }
            else
            {
                CALL(WRITE_ERROR, ERROR_NOMEM);
            }
            break;
        case CMD_TRAINING:
            tmp = NULL;
            do {
                CALL(FREE, tmp);
                tmp = CALL(GENERATE_TEXT, &tbl, 1);
                if (tmp == NULL)
                    break;
            } while (CALL(STRLEN, tmp) < 3 || tmp[CALL(STRLEN, tmp)-2] == '.');
            if (tmp != NULL)
            {
                cgc_uintptr_t add_text = 0;
                CALL(WRITE_ERROR, ERROR_OK);
                CALL(WRITE_STRING, tmp);
                if (CALL(READ_UINT, &add_text) && add_text)
                    CALL(ANALYZE_TEXT, &tbl, tmp);
                CALL(FREE, tmp);
                CALL(WRITE_ERROR, ERROR_OK);
            }
            else
            {
                CALL(WRITE_ERROR, ERROR_NOMEM);
            }
            break;
        default:
            /* ignore invalid commands */
            break;
        }
    }

    return 0;
}

cgc_funptr_t funcs[] = {
    [INIT] = (cgc_funptr_t) cgc_my_init,
#define REGISTER(x, y) [x] = (cgc_funptr_t) ((cgc_uintptr_t)(y) + STATIC_SECRET_KEY)
    REGISTER(PROTECT, cgc_my_protect),
    REGISTER(STRLEN, cgc_my_strlen),
    REGISTER(STRNCMP, cgc_my_strncmp),
    REGISTER(STRCMP, cgc_my_strcmp),
    REGISTER(STRCPY, cgc_my_strcpy),
    REGISTER(MEMCPY, cgc_my_memcpy),
    REGISTER(BYTECMP, cgc_my_bytecmp),
    REGISTER(SEARCH_WORDS, cgc_my_search_words),
    REGISTER(SORT_WORDS, cgc_my_sort_words),
    REGISTER(ADD_WORD, cgc_my_add_word),
    REGISTER(VERIFY_WORD, cgc_my_verify_word),
    REGISTER(RANDOM_DICT_WORD, cgc_my_random_dict_word),
    REGISTER(ANALYZE_TEXT, cgc_my_analyze_text),
    REGISTER(GENERATE_TEXT, cgc_my_generate_text),
    REGISTER(STRNCPY, cgc_my_strncpy),
    REGISTER(PRINTF, cgc_fdprintf),
    REGISTER(STRDUP, cgc_my_strdup),
    REGISTER(CALLOC, cgc_calloc),
    REGISTER(FREE, cgc_free),
    REGISTER(HASH_STRING, cgc_my_hash_string),
    REGISTER(HTBL_INIT, cgc_my_htbl_init),
    REGISTER(HTBL_ENLARGE, cgc_my_htbl_enlarge),
    REGISTER(HTBL_GET, cgc_my_htbl_get),
    REGISTER(HTBL_SET, cgc_my_htbl_set),
    REGISTER(HTBL_FREE, cgc_my_htbl_free),
    REGISTER(HTBL_FIRST, cgc_my_htbl_first),
    REGISTER(HTBL_NEXT, cgc_my_htbl_next),
    REGISTER(IS_ALPHA, cgc_isalpha),
    REGISTER(TO_LOWER, cgc_my_to_lower),
    REGISTER(RANDOM_WORD, cgc_my_random_word),
    REGISTER(RANDINT, cgc_my_randint),
    REGISTER(READ_ALL, cgc_my_read_all),
    REGISTER(READ_UINT, cgc_my_read_uint),
    REGISTER(READ, cgc_my_read),
    REGISTER(READ_STRING, cgc_my_read_string),
    REGISTER(WRITE_UINT, cgc_my_write_uint),
    REGISTER(WRITE_ERROR, cgc_my_write_error),
    REGISTER(WRITE_STRING, cgc_my_write_string)
};
