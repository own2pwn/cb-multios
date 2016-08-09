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
  #include <ctype.h>
  #include "readuntil.h"
};

#include "command.h"
#include "command_manager.h"

cgc_Command::cgc_Command(const char *name, const char *alias)
{
  cgc_memset(this->name, 0, sizeof(this->name));
  cgc_memset(this->alias, 0, sizeof(this->alias));

  cgc_SetName(name);
  if (alias)
    cgc_SetAlias(alias);
}

cgc_Command::~cgc_Command()
{
}

bool cgc_is_number(char* c)
{
  while (*c)
  {
    if (!cgc_isdigit(*c))
      return false;
    c++;
  }
  return true;
}

void cgc_Command::cgc_HandleResponse(int res)
{
  if (res == 0)
    return;
  switch (this->cgc_GetType())
  {
    case CT_LIST:
      /* No error code */
      break;
    case CT_CREATEDIR:
    case CT_CREATE:
      if (res == -1)
        printf("Max number of files reached\n");
      else if (res == -2)
        printf("Internal error\n");
      else if (res == -3)
        printf("cgc_File already exists\n");
      else if (res == -4)
        printf("Name too long\n");
      break;
    case CT_OPEN:
      if (res == -1)
        printf("Max number of opened files reached\n");
      else if (res == -2)
        printf("Internal error\n");
      else if (res == -3)
        printf("Not a regular file\n");
      else if (res == -4)
        printf("Already opened\n");
      else if (res == -5)
        printf("No such file\n");
      break;
    case CT_CLOSE:
      if (res == -1 || res == -2)
        printf("Invalid fileno\n");
      else if (res == -3)
        printf("Not a regular file\n");
      else if (res == 1)
        printf("Already closed\n");
      break;
    case CT_CLOSEALL:
      /* No error code */
      break;
    case CT_READ:
      if (res == -1 || res == -2)
        printf("Invalid fileno\n");
      else if (res == -10)
        printf("Not a regular file\n");
      else if (res == -20)
        printf("Internal error\n");
      else if (res == -30)
        printf("cgc_File not opened\n");
      else if (res == -40)
        printf("Empty file\n");
      else if (res == -50)
        printf("Out of range\n");
      break;
    case CT_MODIFY:
      if (res == -1 || res == -2)
        printf("Invalid fileno\n");
      else if (res == -10)
        printf("Not a regular file\n");
      else if (res == -20 || res == -40)
        printf("Internal error\n");
      else if (res == -30)
        printf("cgc_File not opened\n");
      break;
    case CT_DELETE:
      if (res == -1)
        printf("Internal error\n");
      else if (res == -2)
        printf("Not valid file\n");
      else if (res == -3)
        printf("No such file\n");
      else if (res == -4)
        printf("Not a regular file\n");
      break;
    case CT_DELETEDIR:
      if (res == -1)
        printf("Internal error\n");
      else if (res == -2 || res == -3)
        printf("Not a directory\n");
      else if (res == -4)
        printf("No such directory\n");
      break;
    case CT_CHANGEDIR:
      if (res == -1)
        printf("Internal error\n");
      else if (res == -2)
        printf("Not a directory\n");
      else if (res == -3)
        printf("No such file or directory\n");
      break;
    case CT_QUIT:
      /* No error code */
      break;
    case CT_HELP:
      break;
  }
}

int cgc_ListCmd::cgc_Execute(cgc_FileManager *fm, int argc, char** argv)
{
  /* Current Directory */
  if (argc == 0)
    fm->cgc_PrintFile(0);
  /* Single cgc_File or Directory */
  else if (argc == 1)
  {
    if (!fm->cgc_GetFile(argv[0]))
      printf("%s: No such file or directory\n", argv[0]);
    else
      fm->cgc_PrintFile(argv[0]);
  }
  /* Multiple Files and/or Directories */
  else
  {
    int i;
    for (i = 0; i < argc; ++i)
    {
      if (!fm->cgc_GetFile(argv[i]))
        printf("%s: No such file or directory\n", argv[i]);
      else
      {
        printf("%s:\n", argv[i]);
        fm->cgc_PrintFile(argv[i]);
        if (i < argc - 1)
          printf("\n");
      }
    }
  }
  return 0;
}

int cgc_CreateCmd::cgc_Execute(cgc_FileManager *fm, int argc, char** argv)
{
  if (argc == 0)
  {
    printf("No filename specified\n");
    return -1;
  }
  cgc_HandleResponse(fm->cgc_CreateFile(argv[0]));
  return 0;
}

int cgc_OpenCmd::cgc_Execute(cgc_FileManager *fm, int argc, char** argv)
{
  if (argc == 0)
  {
    printf("No filename specified\n");
    return -1;
  }
  int n = fm->cgc_OpenFile(argv[0]);
  cgc_HandleResponse(n);
  if (n >= 0)
    printf("[%s] opened (%d)\n", argv[0], n);
  return 0;
}

int cgc_CloseCmd::cgc_Execute(cgc_FileManager *fm, int argc, char** argv)
{
  if (argc == 0)
  {
    printf("No fileno specified\n");
    return -1;
  }
  if (!cgc_is_number(argv[0]))
  {
    printf("Invalid fileno\n");
    return -1;
  }
  cgc_HandleResponse(fm->cgc_CloseFile(cgc_strtoul(argv[0], 0, 10)));
  return 0;
}

int cgc_CloseAllCmd::cgc_Execute(cgc_FileManager *fm, int argc, char** argv)
{
  fm->cgc_CloseAll();
  return 0;
}

int cgc_ReadCmd::cgc_Execute(cgc_FileManager *fm, int argc, char** argv)
{
  unsigned int fileno;
  cgc_size_t pos = 0, len = 0;
  if (argc == 0)
  {
    printf("No fileno specified\n");
    return -1;
  }
  if (!cgc_is_number(argv[0]))
  {
    printf("Invalid fileno\n");
    return -1;
  }
  fileno = cgc_strtoul(argv[0], 0, 10);
  if (argc > 1)
  {
    if (!cgc_is_number(argv[1]))
    {
      printf("Invalid pos\n");
      return -1;
    }
    pos = cgc_strtoul(argv[1], 0, 10);
  }
  if (argc > 2)
  {
    if (!cgc_is_number(argv[2]))
    {
      printf("Invalid len\n");
      return -1;
    }
    len = cgc_strtoul(argv[2], 0, 10);
  }

  char *buf;
  int n = fm->cgc_ReadFile(fileno, pos, len, &buf);
  cgc_HandleResponse(n);
  if (n >= 0)
  {
    transmit(STDOUT, buf, n, &len);
    cgc_free(buf);
  }
  return 0;
}

int cgc_ModifyCmd::cgc_Execute(cgc_FileManager *fm, int argc, char** argv)
{
  int n, len;
  unsigned int fileno;
  char buf[512];
  cgc_size_t pos = 0, numBytes = 0;
  if (argc == 0)
  {
    printf("No fileno specified\n");
    return -1;
  }
  if (!cgc_is_number(argv[0]))
  {
    printf("Invalid fileno\n");
    return -1;
  }
  fileno = cgc_strtoul(argv[0], 0, 10);
  if (argc > 1)
  {
    if (!cgc_is_number(argv[1]))
    {
      printf("Invalid pos\n");
      return -1;
    }
    pos = cgc_strtoul(argv[1], 0, 10);
  }
  /* Test if writable */
  n = fm->cgc_ModifyFile(fileno, pos, 0, 0);
  cgc_HandleResponse(n);
  if (n)
    return -1;
  printf("<< Edit Mode - end with EOF >>\n");
  printf("pos: %d\n", pos);
  printf("==============================\n");
  while (1)
  {
    if ((len = cgc_read_until(STDIN, buf, sizeof(buf), '\n')) < 0)
      return -1;
    if (cgc_strcmp(buf, "EOF") == 0)
      break;
    buf[len - 1] = '\n';
    n = fm->cgc_ModifyFile(fileno, pos, buf, len);
    cgc_HandleResponse(n);
    if (n != len)
      return -1;
    pos += n;
    numBytes += n;
  }
  printf("==============================\n");
  printf("Wrote %d byte(s)\n", numBytes);
  return 0;
}

int cgc_DeleteCmd::cgc_Execute(cgc_FileManager *fm, int argc, char** argv)
{
  if (argc == 0)
  {
    printf("No file specified\n");
    return -1;
  }
  cgc_HandleResponse(fm->cgc_DeleteFile(argv[0]));
  return 0;
}

int cgc_CreateDirCmd::cgc_Execute(cgc_FileManager *fm, int argc, char** argv)
{
  if (argc == 0)
  {
    printf("No dirname specified\n");
    return -1;
  }
  cgc_HandleResponse(fm->cgc_CreateDirectory(argv[0]));
  return 0;
}

int cgc_DeleteDirCmd::cgc_Execute(cgc_FileManager *fm, int argc, char** argv)
{
  if (argc == 0)
  {
    printf("No dirname specified\n");
    return -1;
  }
  cgc_HandleResponse(fm->cgc_DeleteDirectory(argv[0]));
  return 0;
}

int cgc_ChangeDirCmd::cgc_Execute(cgc_FileManager *fm, int argc, char** argv)
{
  if (argc == 0)
    fm->cgc_ChangeDirectory(0);
  else
    cgc_HandleResponse(fm->cgc_ChangeDirectory(argv[0]));
  return 0;
}

int cgc_QuitCmd::cgc_Execute(cgc_FileManager *fm, int argc, char** argv)
{
  cgc_exit(0);
  return 0;
}

int cgc_HelpCmd::cgc_Execute(cgc_FileManager *fm, int argc, char** argv)
{
  int i, j;
  cgc_CommandManager *cm = (cgc_CommandManager *) fm; // HACKZ
  cgc_List<cgc_Command *> *commands = cm->cgc_GetCommands();
  for (i = 0; i < commands->cgc_length(); ++i)
  {
    cgc_Command *cmd = commands->cgc_get(i);
    if (argc == 0)
      printf("%s\n", cmd->cgc_Usage());
    else
    {
      for (j = 0; j < argc; ++j)
      {
        if (cgc_strcmp(argv[j], cmd->cgc_GetName()) == 0 ||
            cgc_strcmp(argv[j], cmd->cgc_GetAlias()) == 0)
        {
          printf("%s\n", cmd->cgc_Usage());
          break;
        }
      }
    }
  }
  return 0;
}
