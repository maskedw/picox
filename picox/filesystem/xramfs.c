/**
 *       @file  xramfs.c
 *      @brief
 *
 *    @details
 *
 *     @author  MaskedW
 *
 *   @internal
 *     Created  2016/02/20
 * ===================================================================
 */


/*
 * License: MIT license
 * Copyright (c) <2016> <MaskedW [maskedw00@gmail.com]>
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <picox/filesystem/xramfs.h>
#include <picox/container/xbyte_array.h>
#include <picox/container/xintrusive_list.h>


#define X__TYPE_DIR  (0)
#define X__TYPE_FILE (1)


struct X__Dir;
struct X__Entry;
struct X__File;
typedef struct X__Dir X__Dir;
typedef struct X__Entry X__Entry;
typedef struct X__File X__File;


struct X__Entry
{
    X__Dir*         parent;
    XIntrusiveNode  node;
    uint8_t         type;
    XTime           timestamp;
    char*           name;
    uint32_t        hash_of_name;
};


struct X__Dir
{
    X__Entry        entry;
    XIntrusiveList  children;
};


struct X__File
{
    X__Entry        entry;
    uint8_t         mode;
    uint8_t*        data;
    size_t          pos;
    size_t          capacity;
};


X__Entry* X__CreateDir(X__Dir* parent, const char* name)
{
    X__Dir* dir = x_malloc(sizeof(X__Dir));
    X_ASSERT(dir);
    xilist_init(&dir->children);
    dir->entry.parent = parent;
    dir->entry.type = X__TYPE_DIR;
    dir->entry.name = x_strdup(name);

    if (parent)
    {
        xilist_push_back(&parent->children, &dir->entry.node);
    }

    return (X__Entry*)dir;
}


X__Entry* X__CreateFile(X__Dir* parent, const char* name, uint8_t mode)
{
    X__File* file = x_malloc(sizeof(X__File));
    X_ASSERT(file);

    file->entry.parent = parent;
    file->entry.type = X__TYPE_FILE;
    file->entry.name = x_strdup(name);
    file->data = NULL;
    file->pos = 0;
    file->capacity = 0;
    file->mode = mode;

    return (X__Entry*)file;
}


XError xramfs_init(XRamFs* fs, void* mem, size_t size)
{
    X_UNUSED(mem);
    X_UNUSED(size);

    fs->m_rootdir = X__CreateDir(NULL, "/");
    return X_ERR_NONE;
}


XError xramfs_open(XRamFs* fs, XFile* fp, const char* path, const char* mode)
{
    /* rなら既存ファイルを開く
     */
    X_UNUSED(mode);
    fp->m_tag = X_RAMFS_TAG;
    fp->m_file_handle = X__CreateFile(fs->m_rootdir, path, 0);
    fp->m_fs = fs;
    return X_ERR_NONE;
}


XError xramfs_close(XFile* fp)
{
    X_UNUSED(fp);
    return X_ERR_NONE;
}


static size_t X__GetNextCapacity(const X__File* file, size_t size)
{
    const size_t space = file->capacity - file->pos;
    if (space >= size)
        return file->capacity;

    const size_t new_capacity = size > space + file->capacity ?
                                size :
                                file->capacity * 2;
    return new_capacity;
}



XError xramfs_read(XFile* fp, void* dst, size_t size, size_t* nread)
{
    X__File* const file = fp->m_file_handle;

    memcpy(dst, file->data + file->pos, size);
    X_ASSIGN_NOT_NULL(nread, size);
    file->pos += size;

    return X_ERR_NONE;
}


XError xramfs_write(XFile* fp, const void* src, size_t size, size_t* nwritten)
{
    X__File* const file = fp->m_file_handle;
    if (file->pos + size > file->capacity)
    {
        const size_t next_capacity = X__GetNextCapacity(file, size);
        uint8_t* buf = x_realloc2(file->data, file->capacity, next_capacity);
        file->data = buf;
        file->capacity = next_capacity;
        X_ASSERT(buf);
    }
    memcpy(file->data + file->pos, src, size);
    file->pos += size;

    X_ASSIGN_NOT_NULL(nwritten, size);

    return X_ERR_NONE;
}


XError xramfs_seek(XFile* fp, XOffset pos, XSeekMode whence)
{
    X_UNUSED(whence);
    X__File* const file = fp->m_file_handle;
    file->pos = pos;

    return X_ERR_NONE;
}


XError xramfs_tell(XFile* fp, XSize* pos)
{
    X_ASSERT(fp);
    X_ASSERT(pos);

    X__File* const file = fp->m_file_handle;
    *pos = file->pos;

    return X_ERR_NONE;
}


XError xramfs_flush(XFile* fp)
{
    X_ASSERT(fp);

    X_UNUSED(fp);
    return X_ERR_NONE;
}


XError xramfs_mkdir(XRamFs* fs, const char* path)
{
    X_UNUSED(fs);
    X_UNUSED(path);
    return X_ERR_NONE;
}


XError xramfs_opendir(XRamFs* fs, XDir* dir)
{
    X_UNUSED(fs);
    X_UNUSED(dir);
    return X_ERR_NONE;
}


XError xramfs_readdir(XDir* dir, XDirEnt* dirent)
{
    X_UNUSED(dir);
    X_UNUSED(dirent);
    return X_ERR_NONE;
}


XError xramfs_closedir(XDir* dir)
{
    X_UNUSED(dir);
    return X_ERR_NONE;
}


XError xramfs_chdir(XRamFs* fs, const char* path)
{
    X_UNUSED(fs);
    X_UNUSED(path);
    return X_ERR_NONE;
}


XError xramfs_getcwd(XRamFs* fs, char* buf, size_t size)
{
    X_UNUSED(fs);
    X_UNUSED(buf);
    X_UNUSED(size);
    return X_ERR_NONE;
}


XError xramfs_remove(XRamFs* fs, const char* path)
{
    X_UNUSED(fs);
    X_UNUSED(path);
    return X_ERR_NONE;
}


XError xramfs_rename(XRamFs* fs, const char* oldpath, const char* newpath)
{
    X_UNUSED(fs);
    X_UNUSED(oldpath);
    X_UNUSED(newpath);
    return X_ERR_NONE;
}


XError xramfs_stat(XRamFs* fs, XStat* stat)
{
    X_UNUSED(fs);
    X_UNUSED(stat);
    return X_ERR_NONE;
}


XError xramfs_utime(XRamFs* fs, const char* path, XTime time)
{
    X_UNUSED(fs);
    X_UNUSED(path);
    X_UNUSED(time);
    return X_ERR_NONE;
}


void xramfs_init_vfs(XRamFs* fs, XVirtualFs* vfs)
{
    vfs->m_fs_handle        = fs;
    vfs->m_open_func        = (XVirtualFsOpenFunc)xramfs_open;
    vfs->m_close_func       = (XVirtualFsCloseFunc)xramfs_close;
    vfs->m_read_func        = (XVirtualFsReadFunc)xramfs_read;
    vfs->m_write_func       = (XVirtualFsWriteFunc)xramfs_write;
    vfs->m_seek_func        = (XVirtualFsSeekFunc)xramfs_seek;
    vfs->m_tell_func        = (XVirtualFsTellFunc)xramfs_tell;
    vfs->m_flush_func       = (XVirtualFsFlushFunc)xramfs_flush;
    vfs->m_mkdir_func       = (XVirtualFsMkdirFunc)xramfs_mkdir;
    vfs->m_opendir_func     = (XVirtualFsOpendirFunc)xramfs_opendir;
    vfs->m_readdir_func     = (XVirtualFsReaddirFunc)xramfs_readdir;
    vfs->m_closedir_func    = (XVirtualFsClosedirFunc)xramfs_closedir;
    vfs->m_chdir_func       = (XVirtualFsChdirFunc)xramfs_chdir;
    vfs->m_getcwd_func      = (XVirtualFsGetcwdFunc)xramfs_getcwd;
    vfs->m_remove_func      = (XVirtualFsRemoveFunc)xramfs_remove;
    vfs->m_rename_func      = (XVirtualFsRenameFunc)xramfs_rename;
    vfs->m_stat_func        = (XVirtualFsStatFunc)xramfs_stat;
    vfs->m_utime_func       = (XVirtualFsUtimeFunc)xramfs_utime;
}
