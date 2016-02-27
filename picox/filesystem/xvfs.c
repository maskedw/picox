/**
 *       @file  xvfs.c
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

#include <picox/filesystem/xvfs.h>


XError xvfs_open(XVirtualFs* vfs, XFile* fp, const char* path, const char* mode)
{
    X_ASSERT(vfs);
    X_ASSERT(fp);

    const XError err = vfs->m_open_func(vfs->m_fs_handle, fp, path, mode);
    fp->m_vfs = vfs;

    return err;
}


XError xvfs_close(XFile* fp)
{
    if (! fp)
        return X_ERR_NONE;

    X_ASSERT(fp->m_vfs);

    XVirtualFs* const vfs = fp->m_vfs;
    const XError err = vfs->m_close_func(fp);

    return err;
}


XError xvfs_write(XFile* fp, const void* src, size_t size, size_t* nwritten)
{
    X_ASSERT(fp);
    X_ASSERT(fp->m_vfs);

    XVirtualFs* const vfs = fp->m_vfs;
    const XError err = vfs->m_write_func(fp, src, size, nwritten);

    return err;
}


XError xvfs_read(XFile* fp, void* dst, size_t size, size_t* nread)
{
    X_ASSERT(fp);
    X_ASSERT(fp->m_vfs);

    XVirtualFs* const vfs = fp->m_vfs;
    const XError err = vfs->m_read_func(fp, dst, size, nread);

    return err;
}
