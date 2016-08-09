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

#include "cgfs.h"
#include <cstdlib.h>
#include <cstring.h>

#include "print.h"

#define FREE_CLUSTER 0x00000000
#define END_OF_FILE 0x0FFFFFF8

#define MAX_IMG_SIZE (1024 * 1024 * 128)
#define MAX_FILE_SIZE (1024 * 1024)

#define OP_READ_FILE 1000
#define OP_WRITE_FILE 2000

cgc_CgFsImg::cgc_CgFsImg()
{
    is_mounted_ = false;
    cluster_map_ = NULL;
    num_clusters_ = 0;
    cluster_region_ = NULL;
    raw_data_size_ = 0;
    cluster_size_ = 0;
}
cgc_CgFsImg::~cgc_CgFsImg()
{
    cgc_Unmount();
}

bool cgc_CgFsImg::cgc_IsMounted()
{
    return is_mounted_;
}

bool cgc_CgFsImg::cgc_Mount(cgc_FILE *fs)
{
    if (!fs || is_mounted_)
        return false;

    if (cgc_fread((char *)(&hdr_), sizeof(hdr_), fs) != sizeof(hdr_))
        return false;
    if (cgc_fread((char *)(&info_sector_), sizeof(info_sector_), fs) != sizeof(info_sector_))
        return false;
    if (hdr_.bytes_per_sector != 512 || hdr_.sectors_per_cluster != 1)
        return false;

    int total_sectors = hdr_.total_sectors_small ? hdr_.total_sectors_small : hdr_.total_sectors_large;
    raw_data_size_ = (total_sectors * hdr_.bytes_per_sector) - sizeof(cgc_bhdr) - sizeof(cgc_fs_info_sector);
    if (raw_data_size_ > MAX_IMG_SIZE)
        return false;

    num_clusters_ = (hdr_.sectors_per_cgfst * hdr_.bytes_per_sector) / sizeof(unsigned int);

    raw_data_ = new unsigned char[raw_data_size_];
    cluster_map_ = (unsigned int *)(raw_data_ + hdr_.num_reserved_sectors * hdr_.bytes_per_sector - sizeof(cgc_bhdr) - sizeof(cgc_fs_info_sector));
    cluster_region_ = (unsigned char *)(&cluster_map_[0]) + ((hdr_.sectors_per_cgfst * hdr_.num_cgfst) * hdr_.bytes_per_sector);
    cluster_size_ = hdr_.bytes_per_sector * hdr_.sectors_per_cluster;

    if (cgc_fread(raw_data_, raw_data_size_, fs) != raw_data_size_)
        return false;
    is_mounted_ = true;

    char root_name[] = { '/', 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20 };
    cgc_fs_file *root_entry = new cgc_fs_file;
    cgc_memset(root_entry->name, 0, sizeof(((cgc_fs_file *)0)->name));
    cgc_memcpy(root_entry->name, root_name, sizeof(root_name));
    root_entry->attrib = 0x10;
    root_entry->starting_cluster = hdr_.root_cluster_idx;
    root_entry->size = 0;
    root_directory_ = cgc_DirectoryTree(root_entry);

    cgc_ParseDirectoryTree(&root_directory_, hdr_.root_cluster_idx);


    return true;
}

bool cgc_CgFsImg::cgc_Unmount()
{
    cgc_memset(&hdr_, 0, sizeof(cgc_bhdr));
    cgc_memset(&info_sector_, 0, sizeof(cgc_fs_info_sector));
    if (raw_data_)
        delete[] raw_data_;
    raw_data_ = NULL;
    raw_data_size_ = 0;
    cluster_region_ = NULL;
    cluster_size_ = 0;
    cluster_map_ = NULL;
    num_clusters_ = 0;
    root_directory_.cgc_ClearTree(true);
    if (!is_mounted_)
        return false;

    is_mounted_ = false;
    return true;
}

cgc_Array<cgc_cluster_data> cgc_CgFsImg::cgc_GetFileClusters(const char *path)
{
    cgc_Array<cgc_cluster_data> file_clusters;
    cgc_fs_file *pfile = root_directory_.cgc_FindFile(path);
    if (!pfile)
        return file_clusters;
    else
        return cgc_GetFileClusters(pfile->starting_cluster);
}

cgc_Array<cgc_cluster_data> cgc_CgFsImg::cgc_GetFileClusters(unsigned int start_cluster)
{
    cgc_Array<cgc_cluster_data> file_clusters;
    if (!is_mounted_)
        return file_clusters;

    unsigned int cluster_idx = start_cluster;
    while (cluster_idx > 1 && cluster_idx < END_OF_FILE)
    {
        cgc_cluster_data file_cluster;
        file_cluster.data = &cluster_region_[(cluster_idx - 2) * cluster_size_];
        file_cluster.cluster_size = cluster_size_;
        file_cluster.cluster_idx = cluster_idx;
        file_clusters.cgc_Append(file_cluster);
        cluster_idx = cluster_map_[cluster_idx];
    }

    return file_clusters;
}

void cgc_CgFsImg::cgc_ListFiles(const char *path, bool recursive)
{
    root_directory_.cgc_ListFiles(path, recursive);
}

cgc_fs_file *cgc_CgFsImg::cgc_FindFile(const char *filepath)
{
    return root_directory_.cgc_FindFile(filepath);
}

bool cgc_CgFsImg::cgc_AddFile(const char *dirpath, const char *filename, char *data, unsigned int file_size)
{
    if(!dirpath || !filename || cgc_strlen(filename) > sizeof(((cgc_fs_file *)0)->name)
            || !file_size || file_size > MAX_FILE_SIZE || cgc_FileExists(dirpath, filename))
        return false;

    cgc_fs_file *parent_dir = root_directory_.cgc_FindFile(dirpath);
    if (!parent_dir || !CgFsFile::cgc_IsDirectory(parent_dir))
        return false;

    unsigned int start_cluster = cgc_FindUnusedCluster();
    if (!start_cluster)
        return false;

    cgc_fs_file new_file;
    new_file.starting_cluster = start_cluster;
    cgc_memset(new_file.name, 0x20, sizeof(new_file.name));
    cgc_memcpy(new_file.name, filename, cgc_strlen(filename));
    new_file.attrib = 0;
    cgc_memset(new_file.reserved, 0, sizeof(new_file.reserved));
    new_file.size = file_size;

    cgc_AddMetadataEntry(&new_file, parent_dir->starting_cluster);
    cluster_map_[start_cluster] = END_OF_FILE;
    unsigned int allocated_size = file_size > cluster_size_ ? cluster_size_ : file_size;
    if (data)
        cgc_memcpy(&cluster_region_[(start_cluster - 2) * cluster_size_], data, allocated_size);
    else
        cgc_memset(&cluster_region_[(start_cluster - 2) * cluster_size_], 0, allocated_size);

    while (allocated_size < file_size)
    {
        unsigned int new_cluster_idx = cgc_AddCluster(start_cluster);
        unsigned int write_size = file_size - allocated_size > cluster_size_ ? cluster_size_ : file_size - allocated_size;
        if(data)
            cgc_memcpy(&cluster_region_[(new_cluster_idx - 2) * cluster_size_], &data[allocated_size], write_size);
        else
            cgc_memset(&cluster_region_[(new_cluster_idx - 2) * cluster_size_], 0, write_size);
        allocated_size += write_size;
    }

    cgc_RebuildTree();
    return true;
}

bool cgc_CgFsImg::cgc_AddDirectory(const char *dirpath, const char *dirname)
{
    if(!dirpath || !dirname || cgc_strlen(dirname) > sizeof(((cgc_fs_file *)0)->name) || cgc_FileExists(dirpath, dirname))
        return false;

    cgc_fs_file *parent_dir = root_directory_.cgc_FindFile(dirpath);
    if (!parent_dir || !CgFsFile::cgc_IsDirectory(parent_dir))
        return false;

    unsigned int start_cluster = cgc_FindUnusedCluster();
    cgc_ClearCluster(start_cluster);
    if (!start_cluster)
        return false;

    cgc_fs_file new_file;
    new_file.starting_cluster = start_cluster;
    cgc_memset(new_file.name, 0x20, sizeof(new_file.name));
    cgc_memcpy(new_file.name, dirname, cgc_strlen(dirname));
    new_file.attrib = 0x10;
    cgc_memset(new_file.reserved, 0, sizeof(new_file.reserved));
    new_file.size = 0;

    cgc_AddMetadataEntry(&new_file, parent_dir->starting_cluster);
    cluster_map_[start_cluster] = END_OF_FILE;

    cgc_RebuildTree();
    return true;
}

bool cgc_CgFsImg::cgc_DeleteFile(const char *path)
{
    cgc_fs_file *pfile = root_directory_.cgc_FindFile(path);
    if (!pfile || !CgFsFile::cgc_IsFile(pfile))
        return false;

    cgc_ClearAllClusters(pfile->starting_cluster);
    CgFsFile::cgc_DeleteMetadataEntry(pfile);
    cgc_RebuildTree();

    return true;
}

bool cgc_CgFsImg::cgc_DeleteDirectory(const char *path)
{
    cgc_DirectoryTree *pdir = root_directory_.cgc_FindDirectory(path);
    if (!pdir)
        return false;

    cgc_DeleteDirectoryHelper(pdir, pdir == &root_directory_);
    cgc_RebuildTree();
    return true;
}

bool cgc_CgFsImg::cgc_PreviewFile(const char *path, char **pdata, unsigned int *num_bytes)
{
    *num_bytes = 0;
    if (!path || !pdata)
        return false;
    cgc_fs_file *found_file = root_directory_.cgc_FindFile(path);
    if (!found_file || !CgFsFile::cgc_IsFile(found_file))
        return false;

    unsigned int cluster_idx = found_file->starting_cluster;
    if (cluster_idx <= 1 || cluster_idx > END_OF_FILE)
        return false;

    cgc_cluster_data file_cluster;
    file_cluster.data = &cluster_region_[(cluster_idx - 2) * cluster_size_];
    file_cluster.cluster_size = cluster_size_;
    file_cluster.cluster_idx = cluster_idx;

    *num_bytes = cluster_size_ < found_file->size  || CgFsFile::cgc_IsDirectory(found_file) ? cluster_size_ : found_file->size;
    *pdata = new char[*num_bytes];
    char *data = *pdata;
    cgc_memcpy(data, file_cluster.data, *num_bytes);
    return true;
}

bool cgc_CgFsImg::cgc_ReadFromFile(const char *path, unsigned int offset, unsigned int num_bytes_to_read, char **pdata, unsigned int *num_bytes)
{
    *num_bytes = 0;
    if (!path || !pdata)
        return false;
    cgc_fs_file *found_file = root_directory_.cgc_FindFile(path);
    if (!found_file || !CgFsFile::cgc_IsFile(found_file))
        return false;
    if (offset > found_file->size)
        return false;

    num_bytes_to_read = num_bytes_to_read ? num_bytes_to_read : found_file->size;
    num_bytes_to_read = offset + num_bytes_to_read > found_file->size ? found_file->size - offset : num_bytes_to_read;
    *pdata = new char[num_bytes_to_read];
    char *data = *pdata;
    cgc_Array<cgc_cluster_data> file_clusters = cgc_GetFileClusters(found_file->starting_cluster);
    for (int i = 0; i < file_clusters.cgc_length() && *num_bytes < num_bytes_to_read; i++)
    {
        if (offset < i * cluster_size_ || offset >= (i+1) * cluster_size_)
            continue;

        int relative_offset = offset % cluster_size_;
        int num_bytes_left = num_bytes_to_read - *num_bytes;
        int read_size = num_bytes_left <= cluster_size_ ? num_bytes_left : cluster_size_;
        read_size = read_size + relative_offset > cluster_size_ ? read_size - (read_size + relative_offset - cluster_size_) : read_size;
        cgc_memcpy(&data[*num_bytes], &file_clusters[i].data[relative_offset], read_size);

        offset = offset - (relative_offset) + cluster_size_;
        *num_bytes += read_size;
    }

    return true;
}

bool cgc_CgFsImg::cgc_WriteToFile(const char *path, unsigned int offset, unsigned int num_bytes_to_write, char *data, unsigned int *num_bytes)
{
    *num_bytes = 0;
    if (!path || !num_bytes_to_write || !data)
        return false;
    cgc_fs_file *found_file = root_directory_.cgc_FindFile(path);
    if (!found_file || !CgFsFile::cgc_IsFile(found_file))
        return false;
    if (offset > found_file->size)
        return false;

    num_bytes_to_write = offset + num_bytes_to_write > found_file->size ? found_file->size - offset : num_bytes_to_write;
    cgc_Array<cgc_cluster_data> file_clusters = cgc_GetFileClusters(found_file->starting_cluster);
    for (int i = 0; i < file_clusters.cgc_length() && *num_bytes < num_bytes_to_write; i++)
    {
        if (offset < i * cluster_size_ || offset >= (i+1) * cluster_size_)
            continue;

        int relative_offset = offset % cluster_size_;
        int num_bytes_left = num_bytes_to_write - *num_bytes;
        int write_size = num_bytes_left <= cluster_size_ ? num_bytes_left : cluster_size_;
        write_size = write_size + relative_offset > cluster_size_ ? write_size - (write_size + relative_offset - cluster_size_) : write_size;

#ifndef PATCHED_1
        if (num_bytes_left < cluster_size_)
            cgc_memcpy(&file_clusters[i].data[relative_offset], &data[*num_bytes], num_bytes_left);
        else
            cgc_memcpy(&file_clusters[i].data[relative_offset], &data[*num_bytes], write_size);
#else
        cgc_memcpy(&file_clusters[i].data[relative_offset], &data[*num_bytes], write_size);
#endif

        offset = offset - (relative_offset) + cluster_size_;
        *num_bytes += write_size;
    }
    return true;
}

bool cgc_CgFsImg::cgc_UpdateFileSize(const char *path, unsigned int new_size)
{
    if (!path || !new_size)
        return false;
    cgc_fs_file *found_file = root_directory_.cgc_FindFile(path);
    if (!found_file || !CgFsFile::cgc_IsFile(found_file))
        return false;

    unsigned int old_size = found_file->size;
    unsigned int num_old_clusters = (old_size-1) / cluster_size_ + 1;
    unsigned int num_new_clusters = (new_size-1) / cluster_size_ + 1;
    if (old_size == new_size)
        return true;
    else if (old_size < new_size)
    {
        while (num_old_clusters < num_new_clusters)
        {
            if (cgc_AddCluster(found_file->starting_cluster))
            {
                ++num_old_clusters;
            }
            else
            {
                found_file->size = num_old_clusters * cluster_size_;
                return false;
            }
        }
        found_file->size = new_size;
        return true;
    }
    else
    {
        while (num_old_clusters > num_new_clusters)
        {
            unsigned int popped_cluster = 0;
            if (cgc_PopCluster(found_file->starting_cluster, &popped_cluster))
            {
                --num_old_clusters;
            }
            else
            {
                found_file->size = num_old_clusters * cluster_size_;
                return false;
            }
        }
        found_file->size = new_size;
        return true;
    }
}

void cgc_CgFsImg::cgc_DebugMetadata()
{
    if (is_mounted_)
    {
        cgc_debug_bhdr(hdr_);
        cgc_printf(NL NL);
        cgc_debug_fs_info(info_sector_);
        cgc_printf(NL NL);
        cgc_printf("****Cluster Information****" NL);
        cgc_printf("Num Clusters = " ESC "d" NL, num_clusters_);
        cgc_printf("First 16 bytes of the cluster map:" NL);
        PRINT_ARR_BYTES(cluster_map_, 128);
        cgc_printf("First 64 bytes of the cluster region:" NL);
        PRINT_ARR_BYTES(cluster_region_, 64);
        cgc_printf("********END METADATA INFO********" NL);
        cgc_fflush(stdout);
    }
}

/*********************/
/* Private Functions */
/*********************/
unsigned int cgc_CgFsImg::cgc_FindUnusedCluster()
{
    for (int i = 3; i < num_clusters_; i++)
    {
        if(cluster_map_[i] == FREE_CLUSTER)
            return i;
    }
    return 0;
}

void cgc_CgFsImg::cgc_ClearCluster(unsigned int cluster_idx)
{
    if (cluster_idx > 1 && cluster_idx < num_clusters_)
        cgc_memset(&cluster_region_[(cluster_idx - 2) * cluster_size_], 0, cluster_size_);
}

unsigned int cgc_CgFsImg::cgc_AddCluster(unsigned int start_cluster)
{
    unsigned int free_cluster_idx = cgc_FindUnusedCluster();
    if (!free_cluster_idx || start_cluster < 2 || start_cluster >= END_OF_FILE)
        return 0;

    unsigned int cluster_idx = start_cluster;
    unsigned int last_cluster_idx = start_cluster;
    while (cluster_idx > 1 && cluster_idx < END_OF_FILE)
    {
        last_cluster_idx = cluster_idx;
        cluster_idx = cluster_map_[cluster_idx];
    }

    cluster_map_[last_cluster_idx] = free_cluster_idx;
    cluster_map_[free_cluster_idx] = END_OF_FILE;
    return free_cluster_idx;
}

bool cgc_CgFsImg::cgc_PopCluster(unsigned int start_cluster, unsigned int *popped_cluster)
{
    if (start_cluster < 2 || start_cluster >= END_OF_FILE)
        return false;

    unsigned int cluster_idx = start_cluster;
    unsigned int last_cluster_idx = start_cluster;
    unsigned int prev_cluster_idx = start_cluster;
    while (cluster_idx > 1 && cluster_idx < END_OF_FILE)
    {
        prev_cluster_idx = last_cluster_idx;
        last_cluster_idx = cluster_idx;
        cluster_idx = cluster_map_[cluster_idx];
    }

    cluster_map_[prev_cluster_idx] = END_OF_FILE;
    cluster_map_[last_cluster_idx] = FREE_CLUSTER;
    *popped_cluster = last_cluster_idx;
    return true;
}

bool cgc_CgFsImg::cgc_ClearAllClusters(unsigned int start_cluster)
{
    if (start_cluster < 2 || start_cluster >= END_OF_FILE)
        return false;

    unsigned int cluster_idx = start_cluster;
    unsigned int prev_cluster_idx = start_cluster;
    while (cluster_idx > 1 && cluster_idx < END_OF_FILE)
    {
        prev_cluster_idx = cluster_idx;
        cluster_idx = cluster_map_[cluster_idx];
        cluster_map_[prev_cluster_idx] = FREE_CLUSTER;
    }

    return true;
}

bool cgc_CgFsImg::cgc_FileExists(const char *path, const char *filename)
{
    if (cgc_strlen(filename) > sizeof(((cgc_fs_file *)0)->name))
        return false;

    char *file_path = new char[cgc_strlen(path) + cgc_strlen(filename) + 2];
    cgc_memset(file_path, 0, cgc_strlen(path) + cgc_strlen(filename) + 2);
    unsigned int file_path_len = cgc_strlen(path);
    cgc_memcpy(file_path, path, cgc_strlen(path));
    if(file_path[cgc_strlen(path) - 1] != '/')
        file_path[file_path_len++] = '/';
    cgc_memcpy(&file_path[file_path_len], filename, cgc_strlen(filename));
    cgc_fs_file *found_file = root_directory_.cgc_FindFile(file_path);
    delete[] file_path;

    return found_file;
}

void cgc_CgFsImg::cgc_DeleteDirectoryHelper(cgc_DirectoryTree *dirnode, bool is_root)
{
    if (!dirnode)
        return;

    for (int i = 0; i < dirnode->cgc_subdirectories().cgc_length(); i++)
    {
        cgc_DeleteDirectoryHelper(&dirnode->cgc_subdirectories()[i], false);
    }
    for (int i = 0; i < dirnode->cgc_file_list().cgc_length(); i++)
    {
        cgc_fs_file *pfile = dirnode->cgc_file_list()[i];
        cgc_ClearAllClusters(pfile->starting_cluster);
        CgFsFile::cgc_DeleteMetadataEntry(pfile);
    }

    if (!is_root)
    {
        cgc_fs_file *pdir = dirnode->cgc_directory_entry();
        cgc_ClearAllClusters(pdir->starting_cluster);
        CgFsFile::cgc_DeleteMetadataEntry(pdir);
    }
}

void cgc_CgFsImg::cgc_AddMetadataEntry(cgc_fs_file *new_entry, unsigned int directory_cluster_idx)
{
    cgc_Array<cgc_cluster_data> directory_clusters = cgc_GetFileClusters(directory_cluster_idx);
    for (int i = 0; i < directory_clusters.cgc_length(); i++)
    {
        for (int j = 0; j+sizeof(cgc_fs_file) <= directory_clusters[i].cluster_size; j += sizeof(cgc_fs_file))
        {
            cgc_fs_file *pfile_info = (cgc_fs_file *)&directory_clusters[i].data[j];
            if (CgFsFile::cgc_FreeEntry(pfile_info))
            {
                cgc_memcpy(pfile_info, new_entry, sizeof(cgc_fs_file));
                return;
            }
        }
    }

    unsigned int new_cluster_idx = cgc_AddCluster(directory_cluster_idx);
    cgc_ClearCluster(new_cluster_idx);
    cgc_fs_file *pfile_info = (cgc_fs_file *)&cluster_region_[(new_cluster_idx - 2) * cluster_size_];
    cgc_memcpy(pfile_info, new_entry, sizeof(cgc_fs_file));
}

void cgc_CgFsImg::cgc_ParseDirectoryTree(cgc_DirectoryTree *rootdir)
{
    cgc_ParseDirectoryTree(rootdir, rootdir->cgc_directory_entry()->starting_cluster);
}

void cgc_CgFsImg::cgc_ParseDirectoryTree(cgc_DirectoryTree *directory, unsigned int directory_cluster_idx)
{
    cgc_Array<cgc_cluster_data> directory_clusters = cgc_GetFileClusters(directory_cluster_idx);
    for (int i = 0; i < directory_clusters.cgc_length(); i++)
    {
        for (int j = 0; j+sizeof(cgc_fs_file) <= directory_clusters[i].cluster_size; j += sizeof(cgc_fs_file))
        {
            cgc_fs_file *pfile_info = (cgc_fs_file *)&directory_clusters[i].data[j];
            if (!CgFsFile::cgc_SkipEntry(pfile_info) && !CgFsFile::cgc_IsDotEntry(pfile_info))
                directory->cgc_AddEntry(pfile_info);
        }
    }

    cgc_Array<cgc_DirectoryTree> cgc_subdirectories = directory->cgc_subdirectories();
    for (int i = 0; i < cgc_subdirectories.cgc_length(); i++)
        cgc_ParseDirectoryTree(&cgc_subdirectories[i], cgc_subdirectories[i].cgc_directory_entry()->starting_cluster);
}

void cgc_CgFsImg::cgc_RebuildTree()
{
    root_directory_.cgc_ClearTree(false);
    cgc_ParseDirectoryTree(&root_directory_);
}
