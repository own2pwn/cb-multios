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
#include "blogpost.h"
#include "asciiart.h"
#include "picture.h"

cgc_BlogPost *cgc_BlogPost::cgc_create_blog_post(cgc_User *user, char *_title)
{
    if (!user || !_title || !*_title)
        return NULL;

    return new cgc_BlogPost(user, _title);
}

cgc_BlogPost::cgc_BlogPost(cgc_User *user, char *_title)
{
    owner = user;
    cgc_strcpy(title, _title);
    cgc_memset(post_order, 0, sizeof(post_order));
    total_views = 0;
}

cgc_BlogPost::~cgc_BlogPost()
{

}

char *cgc_BlogPost::cgc_get_title()
{
    return title;
}

bool cgc_BlogPost::cgc_add_text_block(char *text)
{
    cgc_size_t len = cgc_strlen(post_order);
    if (!text || !*text) {
        printf("Can't cgc_add empty text\n");
        return false;
    }

    if (len >= sizeof(post_order)) {
        printf("Post is too big\n");
        return false;
    }

    text_blocks.cgc_add(text);
    post_order[len++] = 't';
    return true;
}

bool cgc_BlogPost::cgc_add_file(cgc_File *file)
{
    cgc_size_t len;
    if (!file) {
        printf("Bad file\n");
        return false;
    }

    len = cgc_strlen(post_order);
    if (len >= sizeof(post_order)) {
        printf("Post is too big\n");
        return false;
    }

    files.cgc_add(file);
    post_order[len] = 'f';
    post_order[++len] = '\0';
    return true;
}

void cgc_BlogPost::cgc_like(cgc_User *user)
{
    for (int i=0; i < likes.cgc_length(); i++) {
        if (likes[i] == user) {
            printf("You already liked this\n");
            return;
        }
    }

    likes.cgc_add(user);
    owner->cgc_add_like();
}

void cgc_BlogPost::cgc_print_post()
{
    cgc_size_t text_idx = 0, file_idx = 0;
    cgc_File *file;

    printf("---%s---\n", title);
    for(cgc_size_t i = 0; i < cgc_strlen(post_order); i++) {
        switch(post_order[i]) {
        case 't':
            printf("%s\n", text_blocks[text_idx++]); break;
        case 'f':
            file = files[file_idx++];
            switch(file->cgc_tell_filetype()) {
            case ASCIIART:
                ((cgc_AsciiArt *)(file))->cgc_print_asciiart(); break;
            case PICTURE:
                ((cgc_Picture *)(file))->cgc_print_picture(); break;
            default:
                printf("Unsupported file type\n");
            }
            break;
        default:
            printf("Bad post\n");
            return;
        }
    }
}
