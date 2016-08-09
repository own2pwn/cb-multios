/*

Author: Joe Rogers <joe@cromulence.com>

Copyright (c) 2014 Cromulence LLC

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
#include <libcgc.h>
#include "stdlib.h"
#include "fs.h"
#include "cmd.h"

char *ARGV[MAX_ARGS];
cgc_uint32_t ARGC;
extern cgc_ShellCmds cmds[];
char CWD[MAX_CMD];

int cgc_FreeArgs() {
	int i = 0;

	while (ARGV[i]) {
		deallocate(ARGV[i], cgc_strlen(ARGV[i]));
		ARGV[i] = NULL;
		i++;
	}

	return(0);
}

int cgc_ParseArgs(char *cmd) {
	char *tok;
	char *t;
	int new_len;
	int open_quote = 0;

	// start with a clean slate
	cgc_bzero(ARGV, MAX_ARGS*sizeof(char*));
	ARGC = 0;

	// look for spaces
	if ((tok = cgc_strtok(cmd, " ")) == NULL) {
		ARGC = 0;
		return(0);
	}
	ARGV[ARGC++] = cgc_strdup(tok);

	while ((tok = cgc_strtok(NULL, " ")) != NULL && ARGC < MAX_ARGS-1) {
		// handle closing quote made up of multiple " "-separated tokens
		if (open_quote && tok[cgc_strlen(tok)-1] == '"') {
			new_len = cgc_strlen(ARGV[ARGC]) + cgc_strlen(tok) + 1;
			if (allocate(new_len, 0, (void *)&t)) {
				cgc_FreeArgs();
				ARGC = 0;
				return(0);
			}
			cgc_strcpy(t, ARGV[ARGC]+1);
			cgc_strcat(t, " ");
			tok[cgc_strlen(tok)-1] = '\0';
			cgc_strcat(t, tok);
			deallocate(ARGV[ARGC], cgc_strlen(ARGV[ARGC]));
			ARGV[ARGC++] = t;
			open_quote = 0;

		// handle middle tokens for quoted string
		} else if (open_quote) {
			new_len = cgc_strlen(ARGV[ARGC]) + cgc_strlen(tok) + 2;
			if (allocate(new_len, 0, (void *)&t)) {
				cgc_FreeArgs();
				ARGC = 0;
				return(0);
			}
			cgc_strcpy(t, ARGV[ARGC]);
			cgc_strcat(t, " ");
			cgc_strcat(t, tok);
			deallocate(ARGV[ARGC], cgc_strlen(ARGV[ARGC]));
			ARGV[ARGC] = t;

		// handle token delimited by quotes
		} else if (tok[0] == '"' & tok[cgc_strlen(tok)-1] == '"') {
			tok[cgc_strlen(tok)-1] = '\0';
			ARGV[ARGC++] = cgc_strdup(tok+1);

		// handle starting quote 
		} else if (tok[0] == '"') {
			open_quote = 1;
			ARGV[ARGC] = cgc_strdup(tok);

		// not a quoted token
		} else {
			ARGV[ARGC++] = cgc_strdup(tok);
		}
	}

	// found a starting, but no ending quote, abort processing
	if (open_quote) {
		cgc_puts("missing quote");
		cgc_FreeArgs();
		ARGC = 0;
		return(0);
	}

	// too many args, abort processing
	if (ARGC == MAX_ARGS-1) {
		cgc_puts("too many args");
		cgc_FreeArgs();
		ARGC = 0;
		return(0);
	}

	return(ARGC);

}

int main(void) {
	char cmd[MAX_CMD];
	cgc_ShellCmds *c;

	cgc_bzero(CWD, MAX_CMD);
	cgc_strcpy(CWD, "/");

	if (cgc_InitFS(512*1024)) {
		cgc_puts("Failed to initialize the RAM file system\n");
		return(-1);
	}

	cgc_puts("Welcome to the cgcfs shell!");
	cgc_puts("Type help for a list of available commands.");
	cgc_printf("% ");
	cgc_bzero(cmd, MAX_CMD);
	while (cgc_readUntil(cmd, MAX_CMD-1, '\n') != -1) {
		cgc_ParseArgs(cmd);

		// parse failure
		if (ARGC == 0) {
			cgc_FreeArgs();
			cgc_bzero(cmd, MAX_CMD);
			cgc_printf("% ");
			continue;
		}

		// find the command being requested
		if (ARGV[0] != NULL) {
			c = cmds;
			while (c->command != NULL) {
				if (!cgc_strcmp(c->command, ARGV[0])) {
					// run the command
					c->handler();

					break;
				}
				c++;
			}
		}
		if (c == NULL) {
			cgc_puts("Invalid command");
		}
		if (c->command == NULL) {
			cgc_puts("Invalid command");
		}

		cgc_FreeArgs();
		cgc_bzero(cmd, MAX_CMD);
		cgc_printf("% ");
	}

	if (cgc_DestroyFS()) {
		cgc_puts("Failed to destroy the RAM file system\n");
		return(-1); 
	}

	return(0);

}
