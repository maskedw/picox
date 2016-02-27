/**
 *       @file  xposixfs.h
 *      @brief
 *
 *    @details
 *
 *     @author  MaskedW
 *
 *   @internal
 *     Created  2016/01/30
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

#ifndef picox_filesystem_xposixfs_h_
#define picox_filesystem_xposixfs_h_


#include <picox/filesystem/xfscore.h>


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


#define X_POSIXFS_TAG   (X_MAKE_TAG('X', 'P', 'O', 'F'))
typedef struct
{
/** @privatesection */
    char*  m_cwd;
    size_t m_prefix_len;
} XPosixFs;


XError xposixfs_init(XPosixFs* fs, const char* rootdir);
void xposixfs_deinit(XPosixFs* fs);
void xposixfs_init_vfs(XPosixFs* fs, XVirtualFs* vfs);
XError xposixfs_open(XPosixFs* fs, XFile* fp, const char* path, const char* mode);
XError xposixfs_close(XFile* fp);
XError xposixfs_read(XFile* fp, void* dst, size_t size, size_t* nread);
XError xposixfs_write(XFile* fp, const void* src, size_t size, size_t* nwritten);
XError xposixfs_seek(XFile* fp, XOffset pos, XSeekMode whence);
XError xposixfs_tell(XFile* fp, XSize* pos);
XError xposixfs_flush(XFile* fp);
XError xposixfs_mkdir(XPosixFs* fs, const char* path);
XError xposixfs_opendir(XPosixFs* fs, XDir* dir, const char* path);
XError xposixfs_readdir(XDir* dir, XDirEnt* dirent, XDirEnt** result);
XError xposixfs_closedir(XDir* dir);
XError xposixfs_chdir(XPosixFs* fs, const char* path);
XError xposixfs_getcwd(XPosixFs* fs, char* buf, size_t size);
XError xposixfs_remove(XPosixFs* fs, const char* path);
XError xposixfs_rename(XPosixFs* fs, const char* oldpath, const char* newpath);
XError xposixfs_stat(XPosixFs* fs, XStat* stat, const char* path);
XError xposixfs_utime(XPosixFs* fs, const char* path, XTime time);


#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* picox_filesystem_xposixfs_h_ */
