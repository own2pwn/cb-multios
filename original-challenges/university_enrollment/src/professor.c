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
#include "professor.h"
#include "ptrlist.h"

#include <string.h>

cgc_ptrlist_t *g_all_profs = NULL;

void cgc_add_professor(cgc_professor_t *prof)
{
    if (!prof)
        return;

    if (!g_all_profs)
        g_all_profs = cgc_create_ptrlist();

    cgc_add_item(g_all_profs, prof);
}

cgc_professor_t *cgc_create_professor(char *name)
{
    if (!name)
        return NULL;

    cgc_professor_t *prof = cgc_malloc(sizeof(cgc_professor_t));
    if (!prof)
        return NULL;

    prof->name = cgc_strdup(name);
    prof->courses_taught = cgc_create_ptrlist();
    cgc_add_professor(prof);

    return prof;
}

void cgc_add_course_to_professor(cgc_professor_t *prof, cgc_course_t *course)
{
    if (!prof || !course)
        return;

    cgc_add_item(prof->courses_taught, course);
}

cgc_professor_t *cgc_get_professor(char *name)
{
    if (!name || !g_all_profs)
        return NULL;

    int i = 0;
    cgc_professor_t *iter;
    for (i = 0; i < g_all_profs->length; i++) {
        iter = get_item(cgc_professor_t, g_all_profs, i);
        if (cgc_strcasecmp(iter->name, name) == 0)
            return iter;
    }

    return NULL;
}

cgc_course_t *cgc_get_professor_class(cgc_professor_t *prof, cgc_size_t idx)
{
    if (!prof)
        return NULL;

    return get_item(cgc_course_t, prof->courses_taught, idx);
}

void cgc_list_professors()
{
    if (!g_all_profs)
        return;

    int i = 0;
    cgc_professor_t *iter;
    printf("--All Professors--\n");
    for (i = 0; i < g_all_profs->length; i++) {
        iter = get_item(cgc_professor_t, g_all_profs, i);
        printf("%s\n", iter->name);
    }
}

void cgc_list_classes_taught(cgc_professor_t *prof)
{
    if (!prof)
        return;

    int i = 0;
    cgc_course_t *iter;
    printf("--%s's Classes--\n", prof->name);
    cgc_print_course_banner();
    for (i = 0; i < prof->courses_taught->length; i++) {
        printf("#%d|", i+1);
        iter = get_item(cgc_course_t, prof->courses_taught, i);
        iter->print_course(iter); //brief?
    }
}

