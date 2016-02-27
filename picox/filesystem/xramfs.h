/**
 *       @file  xramfs.h
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

#ifndef picox_filesystem_xramfs_h_
#define picox_filesystem_xramfs_h_


#include <picox/filesystem/xfscore.h>
#include <picox/allocator/xpico_allocator.h>
#include <picox/container/xintrusive_list.h>


#define X_RAMFS_TAG     (X_MAKE_TAG('R', 'A', 'F', 'S'))
typedef struct
{
/** @privatesection */
    XPicoAllocator m_allocator;
    void*          m_rootdir;
} XRamFs;


void xramfs_init_vfs(XRamFs* fs, XVirtualFs* vfs);
XError xramfs_init(XRamFs* fs, void* mem, size_t size);
XError xramfs_deinit(XRamFs* fs);
XError xramfs_open(XRamFs* fs, XFile* fp, const char* path, const char* mode);
XError xramfs_close(XFile* fp);
XError xramfs_read(XFile* fp, void* dst, size_t size, size_t* nread);
XError xramfs_write(XFile* fp, const void* src, size_t size, size_t* nwritten);
XError xramfs_seek(XFile* fp, XOffset pos, XSeekMode whence);
XError xramfs_tell(XFile* fp, XSize* pos);
XError xramfs_flush(XFile* fp);
XError xramfs_mkdir(XRamFs* fs, const char* path);
XError xramfs_opendir(XRamFs* fs, XDir* dir);
XError xramfs_readdir(XDir* dir, XDirEnt* dirent);
XError xramfs_closedir(XDir* dir);
XError xramfs_chdir(XRamFs* fs, const char* path);
XError xramfs_getcwd(XRamFs* fs, char* buf, size_t size);
XError xramfs_remove(XRamFs* fs, const char* path);
XError xramfs_rename(XRamFs* fs, const char* oldpath, const char* newpath);
XError xramfs_stat(XRamFs* fs, XStat* stat);
XError xramfs_utime(XRamFs* fs, const char* path, XTime time);


#endif /* picox_filesystem_xramfs_h_ */
