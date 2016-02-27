/**
 *       @file  xfatfs.h
 *      @brief
 *
 *    @details
 *
 *     @author  MaskedW
 *
 *   @internal
 *     Created  2016/02/27
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

#ifndef picox_filesystem_xfatfs_h_
#define picox_filesystem_xfatfs_h_


#include <picox/filesystem/xfscore.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


#define X_FATFS_TAG     (X_MAKE_TAG('F', 'A', 'F', 'S'))
typedef struct
{
/** @privatesection */
    XTag            m_tag;
} XFatFs;



// #define X_CONF_MAIN_FS_TYPE  X_FS_TYPE_FATFS


XError xfatfs_init(XFatFs* fs);
void xfatfs_deinit(XFatFs* fs);
void xfatfs_init_vfs(XFatFs* fs, XVirtualFs* vfs);
XError xfatfs_open(XFatFs* fs, XFile* fp, const char* path, const char* mode);
XError xfatfs_close(XFile* fp);
XError xfatfs_read(XFile* fp, void* dst, size_t size, size_t* nread);
XError xfatfs_write(XFile* fp, const void* src, size_t size, size_t* nwritten);
XError xfatfs_seek(XFile* fp, XOffset pos, XSeekMode whence);
XError xfatfs_tell(XFile* fp, XSize* pos);
XError xfatfs_flush(XFile* fp);
XError xfatfs_mkdir(XFatFs* fs, const char* path);
XError xfatfs_opendir(XFatFs* fs, XDir* dir, const char* path);
XError xfatfs_readdir(XDir* dir, XDirEnt* dirent, XDirEnt** result);
XError xfatfs_closedir(XDir* dir);
XError xfatfs_chdir(XFatFs* fs, const char* path);
XError xfatfs_getcwd(XFatFs* fs, char* buf, size_t size);
XError xfatfs_remove(XFatFs* fs, const char* path);
XError xfatfs_rename(XFatFs* fs, const char* oldpath, const char* newpath);
XError xfatfs_stat(XFatFs* fs, XStat* stat, const char* path);
XError xfatfs_utime(XFatFs* fs, const char* path, XTime time);


#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* picox_filesystem_xfatfs_h_ */
