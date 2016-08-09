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

#ifndef CgFs_H_
#define CgFs_H_

#include <cstdio.h>
#include "bhdr.h"
#include "fs_info_sector.h"
#include "cluster.h"
#include "directory_tree.h"

class cgc_CgFsImg
{
  public:
    cgc_CgFsImg();
    ~cgc_CgFsImg();

    unsigned int cgc_cgfst_id();
    unsigned int cgc_end_of_cluster_marker();

    bool cgc_IsMounted();
    bool cgc_Mount(cgc_FILE *img);
    bool cgc_Unmount();

    cgc_Array<cgc_cluster_data> cgc_GetFileClusters(const char *path);
    cgc_Array<cgc_cluster_data> cgc_GetFileClusters(unsigned int start_cluster);
    void cgc_ListFiles(const char *path, bool recursive);

    cgc_fs_file *cgc_FindFile(const char *filepath);
    bool cgc_AddFile(const char *path, const char *filename, char *data, unsigned int file_size);
    bool cgc_AddDirectory(const char *path, const char *dirname);
    bool cgc_DeleteFile(const char *path);
    bool cgc_DeleteDirectory(const char *path);

    bool cgc_PreviewFile(const char *path, char **pdata, unsigned int *num_bytes);
    bool cgc_ReadFromFile(const char *path, unsigned int offset, unsigned int num_bytes_to_read, char **pdata, unsigned int *num_bytes_read);
    bool cgc_WriteToFile(const char *path, unsigned int offset, unsigned int num_bytes_to_write, char *data, unsigned int *num_bytes_written);
    bool cgc_UpdateFileSize(const char *path, unsigned int new_size);

    void cgc_DebugMetadata();
  private:
    bool is_mounted_;
    cgc_bhdr hdr_;
    cgc_fs_info_sector info_sector_;

    unsigned int *cluster_map_;
    unsigned int num_clusters_;

    unsigned char *cluster_region_;
    unsigned int cluster_size_;
    unsigned char *raw_data_;
    unsigned int raw_data_size_;
    cgc_DirectoryTree root_directory_;

    unsigned int cgc_FindUnusedCluster();
    void cgc_ClearCluster(unsigned int cluster_idx);
    unsigned int cgc_AddCluster(unsigned int start_cluster);
    bool cgc_PopCluster(unsigned int start_cluster, unsigned int *popped_cluster);
    bool cgc_ClearAllClusters(unsigned int start_cluster);

    bool cgc_FileExists(const char *dirpath, const char *filename);
    void cgc_DeleteDirectoryHelper(cgc_DirectoryTree *dirnode, bool is_root);
    bool cgc_FileReadWrite(const char *path, unsigned int offset, unsigned int num_bytes_to_rw, char *data, unsigned int *num_bytes, unsigned int operation);

    void cgc_AddMetadataEntry(cgc_fs_file *new_entry, unsigned int directory_cluster_idx);
    void cgc_ParseDirectoryTree(cgc_DirectoryTree *directory);
    void cgc_ParseDirectoryTree(cgc_DirectoryTree *directory, unsigned int directory_cluster_idx);
    void cgc_RebuildTree();
};

#endif
