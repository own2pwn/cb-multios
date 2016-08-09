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

extern "C" {
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>

#include "readuntil.h"
};

#include "command.h"
#include "command_manager.h"
#include "file_manager.h"
#include "llist.h"

cgc_CommandManager *cmdMan;
cgc_FileManager *fileMan;

void cgc_call_inits(void);

void cgc_init_commands()
{
  /* Create commands */
  cmdMan = new cgc_CommandManager(fileMan);
  cgc_ListCmd *list = new cgc_ListCmd();
  cgc_CreateCmd *create = new cgc_CreateCmd();
  cgc_OpenCmd *open = new cgc_OpenCmd();
  cgc_CloseCmd *close = new cgc_CloseCmd();
  cgc_CloseAllCmd *closeAll = new cgc_CloseAllCmd();
  cgc_ReadCmd *read = new cgc_ReadCmd();
  cgc_ModifyCmd *modify = new cgc_ModifyCmd();
  cgc_DeleteCmd *del = new cgc_DeleteCmd();
  cgc_CreateDirCmd *createDir = new cgc_CreateDirCmd();
  cgc_DeleteDirCmd *deleteDir = new cgc_DeleteDirCmd();
  cgc_ChangeDirCmd *changeDir = new cgc_ChangeDirCmd();
  cgc_QuitCmd *quit = new cgc_QuitCmd();
  cgc_HelpCmd *help = new cgc_HelpCmd();

  /* Install commands */
  cmdMan->cgc_InstallCommand(list);
  cmdMan->cgc_InstallCommand(create);
  cmdMan->cgc_InstallCommand(open);
  cmdMan->cgc_InstallCommand(close);
  cmdMan->cgc_InstallCommand(closeAll);
  cmdMan->cgc_InstallCommand(read);
  cmdMan->cgc_InstallCommand(modify);
  cmdMan->cgc_InstallCommand(del);
  cmdMan->cgc_InstallCommand(createDir);
  cmdMan->cgc_InstallCommand(deleteDir);
  cmdMan->cgc_InstallCommand(changeDir);
  cmdMan->cgc_InstallCommand(quit);
  cmdMan->cgc_InstallCommand(help);
}

char** cgc_parse_args(char *s, int *argc)
{
  char **argv = 0, *p;
  int i = 0;
  *argc = 0;

  if (!s && *s)
    return 0;
  p = s;
  while (*p)
  {
    while (cgc_isspace(*p)) p++;
    if (!*p) break;
    while (*p && !cgc_isspace(*p)) p++;
    (*argc)++;
  }
  if (*argc > 0)
  {
    p = s;
    argv = (char **) cgc_calloc((*argc + 1), sizeof(char *));
    while (cgc_isspace(*p)) p++;
    while (*p)
    {
      argv[i] = p;
      while (*p && !cgc_isspace(*p)) p++;
      if (!*p) break;
      *p = '\0';
      p++;
      while (*p && cgc_isspace(*p)) p++;
      i++;
    }
  }
  return argv;
}

int main()
{
  int argc;
  char buf[2048], **argv = 0;
  fileMan = new cgc_FileManager();

  /* init */
  
  cgc_init_commands();

  while (1)
  {
    printf("$ ");
    if (cgc_read_until(STDIN, buf, sizeof(buf), '\n') < 0)
      return -1;
    argv = cgc_parse_args(buf, &argc);
    if (argc)
    {
      if (cmdMan->cgc_ExecuteCommand(argv[0], --argc, &argv[1]) < 0)
        printf("Unknown command [%s]\n", argv[0]);
      cgc_free(argv);
    }
  }
  return 0;
}
