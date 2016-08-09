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

#ifndef COMMAND_H
#define COMMAND_H

extern "C" {
  #include <stdlib.h>
  #include <string.h>
};

#include "file_manager.h"

class cgc_Command
{
  public:
    enum CmdType { CT_LIST, CT_CREATE, CT_OPEN, CT_CLOSE,
                   CT_CLOSEALL, CT_READ, CT_MODIFY, CT_DELETE,
                   CT_CREATEDIR, CT_DELETEDIR, CT_CHANGEDIR,
                   CT_QUIT, CT_HELP };
  protected:
    char name[256];
    char alias[256];

  public:
    cgc_Command(const char *name, const char *alias);
    virtual ~cgc_Command() = 0;

    friend bool operator== (cgc_Command &cmd1, cgc_Command &cmd2)
    {
      return (cgc_strcmp(cmd1.name, cmd2.name) == 0);
    }
    friend bool operator!= (cgc_Command &cmd1, cgc_Command &cmd2)
    {
      return !(cmd1 == cmd2);
    }

    virtual const char* cgc_Usage() = 0;
    virtual int cgc_Execute(cgc_FileManager* fm, int argc, char** argv) = 0;
    virtual CmdType cgc_GetType() = 0;

    void cgc_SetName(const char* name)
    {
      /* Assumes the cgc_length of the strings are validated */
      cgc_strcpy(this->name, name);
    }
    void cgc_SetAlias(const char* alias)
    {
      /* Assumes the cgc_length of the strings are validated */
      cgc_strcpy(this->alias, alias);
    }
    const char* cgc_GetName() { return name; }
    const char* cgc_GetAlias() { return alias; }
    
    void cgc_HandleResponse(int res);
};

class cgc_ListCmd: public cgc_Command
{
  public:
    ~cgc_ListCmd() {};
    cgc_ListCmd() : cgc_Command("list", "ls") {}

    const char* cgc_Usage()
    {
      return "list [file ..]\n - Lists files and directories.";
    }

    int cgc_Execute(cgc_FileManager *fm, int argc, char** argv);
    CmdType cgc_GetType() { return CT_LIST; }
};

class cgc_CreateCmd: public cgc_Command
{
  public:
    ~cgc_CreateCmd() {};
    cgc_CreateCmd() : cgc_Command("create", "new") {}

    const char* cgc_Usage()
    {
      return "create <filename>\n - Creates a file.";
    }

    int cgc_Execute(cgc_FileManager *fm, int argc, char** argv);
    CmdType cgc_GetType() { return CT_CREATE; }
};

class cgc_OpenCmd: public cgc_Command
{
  public:
    ~cgc_OpenCmd() {};
    cgc_OpenCmd() : cgc_Command("open", 0) {}

    const char* cgc_Usage()
    {
      return "open <filename>\n - Opens a file.";
    }

    int cgc_Execute(cgc_FileManager *fm, int argc, char** argv);
    CmdType cgc_GetType() { return CT_OPEN; }
};

class cgc_CloseCmd: public cgc_Command
{
  public:
    ~cgc_CloseCmd() {};
    cgc_CloseCmd() : cgc_Command("close", 0) {}

    const char* cgc_Usage()
    {
      return "close <fileno>\n - Closes an open file.";
    }

    int cgc_Execute(cgc_FileManager *fm, int argc, char** argv);
    CmdType cgc_GetType() { return CT_CLOSE; }
};

class cgc_CloseAllCmd: public cgc_Command
{
  public:
    ~cgc_CloseAllCmd() {};
    cgc_CloseAllCmd() : cgc_Command("close_all", 0) {}

    const char* cgc_Usage()
    {
      return "close_all\n - Closes all open files.";
    }

    int cgc_Execute(cgc_FileManager *fm, int argc, char** argv);
    CmdType cgc_GetType() { return CT_CLOSEALL; }
};

class cgc_ReadCmd: public cgc_Command
{
  public:
    ~cgc_ReadCmd() {};
    cgc_ReadCmd() : cgc_Command("read", "cat") {}

    const char* cgc_Usage()
    {
      return "read <fileno> [pos] [len]\n - Reads a file.";
    }

    int cgc_Execute(cgc_FileManager *fm, int argc, char** argv);
    CmdType cgc_GetType() { return CT_READ; }
};

class cgc_ModifyCmd: public cgc_Command
{
  public:
    ~cgc_ModifyCmd() {};
    cgc_ModifyCmd() : cgc_Command("modify", "write") {}

    const char* cgc_Usage()
    {
      return "modify <fileno> [pos]\n - Modifies a file.";
    }

    int cgc_Execute(cgc_FileManager *fm, int argc, char** argv);
    CmdType cgc_GetType() { return CT_MODIFY; }
};

class cgc_DeleteCmd: public cgc_Command
{
  public:
    ~cgc_DeleteCmd() {};
    cgc_DeleteCmd() : cgc_Command("delete", "rm") {}

    const char* cgc_Usage()
    {
      return "delete <filename>\n - Deletes a file.";
    }

    int cgc_Execute(cgc_FileManager *fm, int argc, char** argv);
    CmdType cgc_GetType() { return CT_DELETE; }
};

class cgc_CreateDirCmd: public cgc_Command
{
  public:
    ~cgc_CreateDirCmd() {};
    cgc_CreateDirCmd() : cgc_Command("mkdir", 0) {}

    const char* cgc_Usage()
    {
      return "mkdir <dirname>\n - Creates a directory.";
    }

    int cgc_Execute(cgc_FileManager *fm, int argc, char** argv);
    CmdType cgc_GetType() { return CT_CREATEDIR; }
};

class cgc_DeleteDirCmd: public cgc_Command
{
  public:
    ~cgc_DeleteDirCmd() {};
    cgc_DeleteDirCmd() : cgc_Command("rmdir", 0) {}

    const char* cgc_Usage()
    {
      return "rmdir <dirname>\n - Deletes a directory.";
    }

    int cgc_Execute(cgc_FileManager *fm, int argc, char** argv);
    CmdType cgc_GetType() { return CT_DELETEDIR; }
};

class cgc_ChangeDirCmd: public cgc_Command
{
  public:
    ~cgc_ChangeDirCmd() {};
    cgc_ChangeDirCmd() : cgc_Command("chdir", "cd") {}

    const char* cgc_Usage()
    {
      return "chdir <dirname>\n - Changes the working directory.";
    }

    int cgc_Execute(cgc_FileManager *fm, int argc, char** argv);
    CmdType cgc_GetType() { return CT_CHANGEDIR; }
};

class cgc_QuitCmd: public cgc_Command
{
  public:
    ~cgc_QuitCmd() {};
    cgc_QuitCmd() : cgc_Command("quit", "cgc_exit") {}

    const char* cgc_Usage()
    {
      return "quit\n - Quits the program.";
    }

    int cgc_Execute(cgc_FileManager *fm, int argc, char** argv);
    CmdType cgc_GetType() { return CT_QUIT; }
};

class cgc_HelpCmd: public cgc_Command
{
  public:
    ~cgc_HelpCmd() {};
    cgc_HelpCmd() : cgc_Command("help", "?") {}

    const char* cgc_Usage()
    {
      return "help [command]\n - Displays the help message.";
    }

    int cgc_Execute(cgc_FileManager *fm, int argc, char** argv);
    CmdType cgc_GetType() { return CT_HELP; }
};

#endif
