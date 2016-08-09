/*
 * Copyright (C) Narf Industries <info@narfindustries.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
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
#include "service.h"
#include "actions.h"
#include "libc.h"

#define GREET "Hello and welcome to the RLEStream(TM) beta!\n\n"\
    "Use any of the following commands to interact:\n"\
    "cgc_list - List currently uploaded videos\n"\
    "cgc_play <name> <key> - Play a cgc_video\n"\
    "cgc_add <name> - Upload a cgc_video (will prompt for more data)\n"\
    "cgc_remove <name> - Delete a cgc_video\n\n"\
    "Thank you for choosing us for your archaic streaming needs!\n\n"

#define WAT "wat"

typedef struct cgc_action {
    char *name;
    void (*cgc_action)(char *req);
} cgc_action;

int main(void) {
    int i;
    char cgc_recv[1024] = {0};
    vhead = NULL;

    cgc_action alist[5] = {
                            {.name = "cgc_list", .cgc_action = cgc_list},
                            {.name = "cgc_play ", .cgc_action = cgc_play},
                            {.name = "cgc_add ", .cgc_action = cgc_add},
                            {.name = "cgc_remove ", .cgc_action = cgc_remove},
                            {.name = "cgc_quit", .cgc_action = cgc_quit}
    };
    
    SSENDL(sizeof(GREET)-1, GREET);

    while (1) {
        cgc_promptc(cgc_recv, sizeof(cgc_recv), "8=D ");

        for (i = 0; i < sizeof(alist)/sizeof(cgc_action); i++) {
            if (cgc_startswith(cgc_recv, alist[i].name)) {
                alist[i].cgc_action(cgc_recv+cgc_strlen(alist[i].name));
                break;
            }
        }

        if (i == sizeof(alist)/sizeof(cgc_action)) {
            SSENDL(sizeof(WAT)-1, WAT);
        }

    }
    return 0;
}
