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

#include "command_manager.h"

cgc_CommandManager::cgc_CommandManager(cgc_FileManager *fm)
{
  this->fm = fm;
  commands = new cgc_List<cgc_Command *>(0);
}

cgc_CommandManager::~cgc_CommandManager()
{
}

cgc_List<cgc_Command *>* cgc_CommandManager::cgc_GetCommands()
{
  return commands;
}

void cgc_CommandManager::cgc_InstallCommand(cgc_Command *cmd)
{
  commands->cgc_add(cmd);
}

void cgc_CommandManager::cgc_UninstallCommand(const char* name)
{
  int i;
  for (i = 0; i < commands->cgc_length(); ++i)
  {
    cgc_Command *cmd = commands->cgc_get(i);
    if (cgc_strcmp(cmd->cgc_GetName(), name) == 0)
    {
      commands->cgc_remove(cmd);
      delete cmd;
      return;
    }
  }
}

int cgc_CommandManager::cgc_ExecuteCommand(const char* name, int argc, char** argv)
{
  int i;
  for (i = 0; i < commands->cgc_length(); ++i)
  {
    cgc_Command *cmd = commands->cgc_get(i);
    if (cgc_strcmp(cmd->cgc_GetName(), name) == 0 || cgc_strcmp(cmd->cgc_GetAlias(), name) == 0)
    {
      if (cmd->cgc_GetType() == cgc_Command::CT_HELP)
        cmd->cgc_Execute((cgc_FileManager *) this, argc, argv); 	// HACKZ
      else
        cmd->cgc_Execute(fm, argc, argv);
      return 0;
    }
  }
  return -1;
}

