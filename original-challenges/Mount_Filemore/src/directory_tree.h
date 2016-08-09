/*
 * Copyright (c) 2016 Kaprica Security, Inc.
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

#ifndef DIRECTORY_TREE_H_
#define DIRECTORY_TREE_H_

#include "fs_file.h"

class cgc_DirectoryTree
{
  public:
    cgc_DirectoryTree();
    cgc_DirectoryTree(cgc_fs_file *directory);
    void cgc_AddEntry(cgc_fs_file *file_info);
    cgc_fs_file *cgc_FindFile(const char *path);
    cgc_DirectoryTree *cgc_FindDirectory(const char *path);
    void cgc_ListFiles(const char *path, bool recursive);
    void cgc_ClearTree(bool delete_root);

    cgc_fs_file *cgc_directory_entry();
    cgc_Array<cgc_DirectoryTree> cgc_subdirectories();
    cgc_Array<cgc_fs_file *> cgc_file_list();

  private:
    cgc_fs_file *directory_entry_;
    cgc_Array<cgc_DirectoryTree> subdirectories_;
    cgc_Array<cgc_fs_file *> file_list_;

    bool cgc_FindFileHelper(cgc_DirectoryTree *dirnode, const char *path, cgc_DirectoryTree **found_dir, cgc_fs_file **found_file);
    void cgc_ListFileHelper(cgc_DirectoryTree *dirnode, const cgc_fs_file *file_info, bool recursive, const char *dirname, int namelen, bool skip_path_update);
    void cgc_ClearTreeHelper(cgc_DirectoryTree *dirnode);
};
#endif
