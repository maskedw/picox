/**
 *       @file  xfscore.h
 *      @brief
 *
 *    @details
 *
 *     @author  MaskedW
 *
 *   @internal
 *     Created  2016/02/06/
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


#ifndef maskedw_filesystem_xfscore_h_
#define maskedw_filesystem_xfscore_h_


#include <picox/core/xcore.h>


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


#ifndef X_CONFIG_FILE_NAME_MAX
#define X_CONFIG_FILE_NAME_MAX (32)
#endif


#ifndef X_CONFIG_FILE_PATH_MAX
#define X_CONFIG_FILE_PATH_MAX (128)
#endif


#define X_PATH_MAX (X_CONFIG_FILE_PATH_MAX)
#define X_NAME_MAX (X_CONFIG_FILE_NAME_MAX)


struct XVirtualFs;
typedef struct XVirtualFs XVirtualFs;


typedef struct
{
/** @privatesection */
    XTag        m_tag;
    void*       m_file_handle;
    void*       m_fs;
    XVirtualFs* m_vfs;
} XFile;


typedef struct
{
/** @privatesection */
    XTag        m_tag;
    void*       m_dir_handle;
    void*       m_fs;
    XVirtualFs* m_vfs;
} XDir;


typedef struct
{
    XTime   mtime;
    XSize   size;
    XMode   mode;
} XStat;


typedef struct
{
    char name[X_CONFIG_FILE_NAME_MAX];
} XDirEnt;


typedef XError (*XVirtualFsOpenFunc)(void* fs, XFile* fp, const char* path, const char* mode);
typedef XError (*XVirtualFsCloseFunc)(XFile* fp);
typedef XError (*XVirtualFsReadFunc)(XFile* fp, void* dst, size_t size, size_t* nread);
typedef XError (*XVirtualFsWriteFunc)(XFile* fp, const void* src, size_t size, size_t* nwritten);
typedef XError (*XVirtualFsSeekFunc)(XFile* fp, XOffset pos, XSeekMode whence);
typedef XError (*XVirtualFsTellFunc)(XFile* fp, XSize* pos);
typedef XError (*XVirtualFsFlushFunc)(XFile* fp);
typedef XError (*XVirtualFsMkdirFunc)(void* fs, const char* path);
typedef XError (*XVirtualFsOpendirFunc)(void* fs, XDir* dir, const char* path);
typedef XError (*XVirtualFsReaddirFunc)(XDir* dir, XDirEnt* dirent, XDirEnt** result);
typedef XError (*XVirtualFsClosedirFunc)(XDir* dir);
typedef XError (*XVirtualFsChdirFunc)(void* fs, const char* path);
typedef XError (*XVirtualFsGetcwdFunc)(void* fs, char* buf, size_t size);
typedef XError (*XVirtualFsRemoveFunc)(void* fs, const char* path);
typedef XError (*XVirtualFsRenameFunc)(void* fs, const char* oldpath, const char* newpath);
typedef XError (*XVirtualFsStatFunc)(void* fs, XStat* stat, const char* path);
typedef XError (*XVirtualFsUtimeFunc)(void* fs, const char* path, XTime time);


struct XVirtualFs
{
/** @privatesection */
    void*                       m_fs_handle;
    XVirtualFsOpenFunc          m_open_func;
    XVirtualFsCloseFunc         m_close_func;
    XVirtualFsReadFunc          m_read_func;
    XVirtualFsWriteFunc         m_write_func;
    XVirtualFsSeekFunc          m_seek_func;
    XVirtualFsTellFunc          m_tell_func;
    XVirtualFsFlushFunc         m_flush_func;
    XVirtualFsMkdirFunc         m_mkdir_func;
    XVirtualFsOpendirFunc       m_opendir_func;
    XVirtualFsReaddirFunc       m_readdir_func;
    XVirtualFsClosedirFunc      m_closedir_func;
    XVirtualFsChdirFunc         m_chdir_func;
    XVirtualFsGetcwdFunc        m_getcwd_func;
    XVirtualFsRemoveFunc        m_remove_func;
    XVirtualFsRenameFunc        m_rename_func;
    XVirtualFsStatFunc          m_stat_func;
    XVirtualFsUtimeFunc         m_utime_func;
};


#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* maskedw_filesystem_xfscore_h_ */
