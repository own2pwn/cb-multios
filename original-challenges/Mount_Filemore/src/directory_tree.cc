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

#include "directory_tree.h"
#include <cstring.h>
#include <cstdio.h>

#include "print.h"

namespace {

void cgc_PrintFile(const cgc_fs_file *file_metadata, unsigned char num_tabs)
{
    for (int i = 0; i < num_tabs; i++)
        cgc_printf("    ");
    cgc_printf("File Name: ");
    PRINT_ARR_CHARS(file_metadata->name, sizeof(file_metadata->name));
}
void cgc_PrintSubdirectory(const cgc_fs_file *file_metadata, unsigned char num_tabs)
{
    for (int i = 0; i < num_tabs; i++)
        cgc_printf("    ");
    cgc_printf("Subdirectory Name: ");
    PRINT_ARR_CHARS(file_metadata->name, sizeof(file_metadata->name));
}
void cgc_PrintDirectory(const cgc_fs_file *file_metadata, char *dirname, int *namelen, bool skip_path_update)
{

    char *pname = &dirname[*namelen];
    for (int i = 0; i < sizeof(file_metadata->name); i++)
    {
        if (file_metadata->name[i] != 0x20 && !skip_path_update)
        {
            *pname++ = file_metadata->name[i];
            ++*namelen;
        }
    }
    if (dirname[*namelen-1] != '/' && (*namelen != 1 || *dirname != '/'))
    {

        *pname = '/';
        ++*namelen;
    }

    cgc_printf("Directory Name: ");
    PRINT_ARR_CHARS(dirname, *namelen);
}

}

cgc_DirectoryTree::cgc_DirectoryTree()
{
    directory_entry_ = (cgc_fs_file *)NULL;
}

cgc_DirectoryTree::cgc_DirectoryTree(cgc_fs_file *directory)
{
    directory_entry_ = directory;
}

void cgc_DirectoryTree::cgc_AddEntry(cgc_fs_file *file_info)
{
    if (!file_info)
        return;

    if (CgFsFile::cgc_IsDirectory(file_info))
        subdirectories_.cgc_Append(cgc_DirectoryTree(file_info));
    else
        file_list_.cgc_Append(file_info);
}


cgc_fs_file *cgc_DirectoryTree::cgc_FindFile(const char *path)
{
    if (!path)
        return (cgc_fs_file *)NULL;
    if (cgc_strcmp("/", path) == 0)
        return directory_entry_;

    cgc_DirectoryTree *dir_tree;
    cgc_fs_file *file_meta;

    if (cgc_FindFileHelper(this, path, &dir_tree, &file_meta))
    {
        if (dir_tree)
            return dir_tree->cgc_directory_entry();
        else if(file_meta)
            return file_meta;
    }

    return (cgc_fs_file *)NULL;
}

cgc_DirectoryTree *cgc_DirectoryTree::cgc_FindDirectory(const char *path)
{
    if (!path)
        return (cgc_DirectoryTree *)NULL;
    if (cgc_strcmp("/", path) == 0)
        return this;

    cgc_DirectoryTree *dir_tree;
    cgc_fs_file *file_meta;

    if (cgc_FindFileHelper(this, path, &dir_tree, &file_meta))
    {
        if (dir_tree)
            return dir_tree;
    }

    return (cgc_DirectoryTree *)NULL;
}

void cgc_DirectoryTree::cgc_ListFiles(const char *path, bool recursive)
{
    cgc_DirectoryTree *dir_tree;
    cgc_fs_file *file_meta;
    if (!path || cgc_strcmp("/", path) == 0)
    {
        char rootdir[] = "";
        cgc_ListFileHelper(this, (cgc_fs_file *)NULL, recursive, rootdir, cgc_strlen(rootdir), false);
        return;
    }
    else
    {
        if (cgc_FindFileHelper(this, path, &dir_tree, &file_meta))
            cgc_ListFileHelper(dir_tree, file_meta, recursive, path, cgc_strlen(path), true);
    }
}

void cgc_DirectoryTree::cgc_ClearTree(bool delete_root)
{
    cgc_fs_file *root_entry;
    if (delete_root)
    {
        delete directory_entry_;
        root_entry = (cgc_fs_file *)NULL;
    }
    else
    {
        root_entry = directory_entry_;
    }

    cgc_ClearTreeHelper(this);
    directory_entry_ = root_entry;
}

cgc_fs_file *cgc_DirectoryTree::cgc_directory_entry()
{
    return directory_entry_;
}

cgc_Array<cgc_DirectoryTree> cgc_DirectoryTree::cgc_subdirectories()
{
    return subdirectories_;
}

cgc_Array<cgc_fs_file *> cgc_DirectoryTree::cgc_file_list()
{
    return file_list_;
}

bool cgc_DirectoryTree::cgc_FindFileHelper(cgc_DirectoryTree *dirnode, const char *path, cgc_DirectoryTree **found_dir, cgc_fs_file **found_file)
{
    *found_dir = (cgc_DirectoryTree *)NULL;
    *found_file = (cgc_fs_file *)NULL;

    bool end_of_path = false;
    char *dir_path = (char *)path;
    char *filename;
    unsigned short name_len = 0;
    if (*dir_path == '/')
        ++dir_path;
    filename = dir_path;

    for( ; *dir_path && *dir_path != '/'; dir_path++)
        ++name_len;
    if (!*dir_path)
        end_of_path = true;

    if (name_len > sizeof(((cgc_fs_file *)0)->name))
        return (cgc_fs_file *)NULL;

    for (int i = 0; i < dirnode->subdirectories_.cgc_length(); i++)
    {
        if (CgFsFile::cgc_NameEquals(dirnode->subdirectories_[i].directory_entry_, filename, name_len))
        {

            if (filename[name_len] == '\0' || (filename[name_len] == '/' && filename[name_len + 1] == '\0'))
            {
                *found_dir = &dirnode->subdirectories_[i];
                return true;
            }
            else
            {
                return cgc_FindFileHelper(&dirnode->subdirectories_[i], filename + name_len, found_dir, found_file);
            }
        }
    }

    if (end_of_path)
    {
        for (int i = 0; i < dirnode->file_list_.cgc_length(); i++)
        {
            if (CgFsFile::cgc_NameEquals(dirnode->file_list_[i], filename, name_len))
            {
                *found_file = dirnode->file_list_[i];
                return true;
            }
        }
    }

    return false;
}

void cgc_DirectoryTree::cgc_ListFileHelper(cgc_DirectoryTree *dirnode, const cgc_fs_file *file_info, bool recursive, const char *dirname, int namelen,
                                    bool skip_path_update)
{
    char *cur_dir = (char *)NULL;
    int cur_namelen = 0;
    if (dirnode)
    {
        cur_dir = new char[namelen + sizeof(((cgc_fs_file *)0)->name) + 1];
        cur_namelen = namelen;
        cgc_memcpy(cur_dir, dirname, namelen);

        cgc_PrintDirectory(dirnode->directory_entry_, cur_dir, &cur_namelen, skip_path_update);
        if (!dirnode->subdirectories_.cgc_length() && !dirnode->file_list_.cgc_length())
        {
            cgc_printf("    --Empty Directory--" NL NL);
            return;
        }

        for (int i = 0; i < dirnode->subdirectories_.cgc_length(); i++)
            cgc_PrintSubdirectory(dirnode->subdirectories_[i].cgc_directory_entry(), 1);
        for (int i = 0; i < dirnode->file_list_.cgc_length(); i++)
            cgc_PrintFile(dirnode->file_list_[i], 1);
        cgc_printf(NL);

        if (recursive)
        {
            for (int i = 0; i < dirnode->subdirectories_.cgc_length(); i++)
                cgc_ListFileHelper(&dirnode->subdirectories_[i], (cgc_fs_file *)NULL, recursive, cur_dir, cur_namelen, false);
        }
    }
    else if (file_info)
    {
        cgc_PrintFile(file_info, 0);
    }

    if (cur_dir)
        delete[] cur_dir;

}

void cgc_DirectoryTree::cgc_ClearTreeHelper(cgc_DirectoryTree *dirnode)
{
    if (!dirnode)
        return;

    for (int i = 0; i < dirnode->subdirectories_.cgc_length(); i++)
        cgc_ClearTreeHelper(&dirnode->subdirectories_[i]);

    directory_entry_ = NULL;
    dirnode->subdirectories_.cgc_Clear();
    dirnode->file_list_.cgc_Clear();
}

