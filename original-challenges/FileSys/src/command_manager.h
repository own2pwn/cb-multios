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

#ifndef COMMAND_MANAGER_H
#define COMMAND_MANAGER_H

extern "C" {
  #include <stdlib.h>
  #include <string.h>
};

#include "command.h"
#include "file_manager.h"
#include "llist.h"

class cgc_CommandManager
{
  protected:
    cgc_List<cgc_Command *> *commands;
    cgc_FileManager *fm;

  public:
    cgc_CommandManager(cgc_FileManager *fm);
    ~cgc_CommandManager();

    cgc_List<cgc_Command *>* cgc_GetCommands();
    void cgc_InstallCommand(cgc_Command *cmd);
    void cgc_UninstallCommand(const char* name);
    int cgc_ExecuteCommand(const char* name, int argc, char** argv);
};

#endif
