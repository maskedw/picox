/**
 *       @file  xvfs.h
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

#ifndef picox_filesystem_xvfs_h_
#define picox_filesystem_xvfs_h_


#include <picox/filesystem/xfscore.h>


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


#define X_FATFS_TAG     (X_MAKE_TAG('F', 'A', 'F', 'S'))
#define X_SPIFFS_TAG    (X_MAKE_TAG('S', 'P', 'F', 'S'))
#define X_ROMFS_TAG     (X_MAKE_TAG('R', 'O', 'F', 'S'))


XError xvfs_open(XVirtualFs* vfs, XFile* fp, const char* path, const char* mode);
XError xvfs_close(XFile* fp);
XError xvfs_write(XFile* fp, const void* src, size_t size, size_t* nwritten);
XError xvfs_read(XFile* fp, void* dst, size_t size, size_t* nread);


#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* picox_filesystem_xvfs_h_ */
