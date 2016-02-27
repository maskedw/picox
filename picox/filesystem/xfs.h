/**
 *       @file  xfs.h
 *      @brief
 *
 *    @details
 *
 *     @author  MaskedW
 *
 *   @internal
 *     Created  2016/02/06
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

#ifndef picox_filesystem_xfs_h_
#define picox_filesystem_xfs_h_


#include <picox/filesystem/xvfs.h>


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


XError xfs_mount(XVirtualFs* vfs, const char* path);
XError xfs_umount(const char* path);
XError xfs_open(XFile* fp, const char* path, const char* mode);
XError xfs_close(XFile* fp);
XError xfs_write(XFile* fp, const void* src, size_t size, size_t* nwritten);
XError xfs_read(XFile* fp, void* dst, size_t size, size_t* nread);


#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* picox_filesystem_xfs_h_ */
