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


XStream* xvfs_init_stream(XStream* stream, XFile* fp)
{
    X_ASSERT_NULL(stream);
    X_ASSERT_NULL(fp);

    xstream_init(stream);
    stream->driver = fp;
    stream->tag = X_VFS_TAG;
    stream->write_func = (XStreamWriteFunc)xvfs_write;
    stream->read_func = (XStreamReadFunc)xvfs_read;
    stream->seek_func = (XStreamSeekFunc)xvfs_seek;
    stream->tell_func = (XStreamTellFunc)xvfs_tell;

    return stream;
}


XError xvfs_open(XVirtualFs* vfs, const char* path, XOpenMode mode, XFile** o_fp)
{
    X_ASSERT_SELF(vfs);

    if (!vfs->m_open_func)
    {
        X_ASSERT_NULL(o_fp);
        *o_fp = NULL;
        return X_ERR_NOT_SUPPORTED;
    }

    const XError err = vfs->m_open_func(vfs->m_realfs, path, mode, o_fp);
    if (*o_fp)
        (*o_fp)->m_vfs = vfs;

    return err;
}


XError xvfs_close(XFile* fp)
{
    if (!fp)
        return X_ERR_NONE;

    X_ASSERT_SELF(fp->m_vfs);

    XVirtualFs* const vfs = fp->m_vfs;
    if (!vfs->m_close_func)
        return X_ERR_NOT_SUPPORTED;

    const XError err = vfs->m_close_func(fp);
    return err;
}


XError xvfs_read(XFile* fp, void* dst, size_t size, size_t* nread)
{
    X_ASSERT_SELF(fp);
    X_ASSERT_SELF(fp->m_vfs);

    XVirtualFs* const vfs = fp->m_vfs;
    if (!vfs->m_read_func)
    {
        X_ASSIGN_NOT_NULL(nread, 0);
        return X_ERR_NOT_SUPPORTED;
    }

    const XError err = vfs->m_read_func(fp, dst, size, nread);
    return err;
}


XError xvfs_write(XFile* fp, const void* src, size_t size, size_t* nwritten)
{
    X_ASSERT_SELF(fp);
    X_ASSERT_SELF(fp->m_vfs);

    XVirtualFs* const vfs = fp->m_vfs;
    if (!vfs->m_write_func)
    {
        X_ASSIGN_NOT_NULL(nwritten, 0);
        return X_ERR_NOT_SUPPORTED;
    }

    const XError err = vfs->m_write_func(fp, src, size, nwritten);
    return err;
}


XError xvfs_seek(XFile* fp, XOffset pos, XSeekMode whence)
{
    X_ASSERT_SELF(fp);
    X_ASSERT_SELF(fp->m_vfs);

    XVirtualFs* const vfs = fp->m_vfs;
    if (!vfs->m_seek_func)
        return X_ERR_NOT_SUPPORTED;

    const XError err = vfs->m_seek_func(fp, pos, whence);
    return err;
}


XError xvfs_tell(XFile* fp, XSize* pos)
{
    X_ASSERT_SELF(fp);
    X_ASSERT_SELF(fp->m_vfs);

    XVirtualFs* const vfs = fp->m_vfs;
    if (!vfs->m_tell_func)
    {
        X_ASSERT_NULL(pos);
        *pos = 0;
        return X_ERR_NOT_SUPPORTED;
    }

    const XError err = vfs->m_tell_func(fp, pos);
    return err;
}


XError xvfs_flush(XFile* fp)
{
    X_ASSERT_SELF(fp);
    X_ASSERT_SELF(fp->m_vfs);

    XVirtualFs* const vfs = fp->m_vfs;
    /* フラッシュ関数は特にやることがない、ということも多いので非登録でも正常終
     * 了扱いとする */
    if (!vfs->m_flush_func)
        return X_ERR_NONE;

    const XError err = vfs->m_flush_func(fp);
    return err;
}


XError xvfs_mkdir(XVirtualFs* vfs, const char* path)
{
    X_ASSERT_SELF(vfs);

    if (!vfs->m_mkdir_func)
        return X_ERR_NOT_SUPPORTED;

    const XError err = vfs->m_mkdir_func(vfs->m_realfs, path);
    return err;
}


XError xvfs_opendir(XVirtualFs* vfs, const char* path, XDir** o_dir)
{
    X_ASSERT_SELF(vfs);

    if (!vfs->m_opendir_func)
    {
        X_ASSERT_NULL(o_dir);
        *o_dir = NULL;
        return X_ERR_NOT_SUPPORTED;
    }

    const XError err = vfs->m_opendir_func(vfs->m_realfs, path, o_dir);
    if (*o_dir)
        (*o_dir)->m_vfs = vfs;

    return err;
}


XError xvfs_readdir(XDir* dir, XDirEnt* dirent, XDirEnt** result)
{
    X_ASSERT_SELF(dir);
    X_ASSERT_SELF(dir->m_vfs);

    XVirtualFs* const vfs = dir->m_vfs;
    if (!vfs->m_readdir_func)
    {
        X_ASSERT_NULL(result);
        *result = NULL;
        return X_ERR_NOT_SUPPORTED;
    }

    const XError err = vfs->m_readdir_func(dir, dirent, result);
    return err;
}


XError xvfs_closedir(XDir* dir)
{
    if (!dir)
        return X_ERR_NONE;

    X_ASSERT_SELF(dir);
    X_ASSERT_SELF(dir->m_vfs);

    XVirtualFs* const vfs = dir->m_vfs;
    if (!vfs->m_closedir_func)
        return X_ERR_NOT_SUPPORTED;

    const XError err = vfs->m_closedir_func(dir);
    return err;
}


XError xvfs_chdir(XVirtualFs* vfs, const char* path)
{
    X_ASSERT_SELF(vfs);
    if (!vfs->m_chdir_func)
        return X_ERR_NOT_SUPPORTED;

    const XError err = vfs->m_chdir_func(vfs->m_realfs, path);
    return err;
}


XError xvfs_getcwd(XVirtualFs* vfs, char* buf, size_t size)
{
    X_ASSERT_SELF(vfs);
    if (!vfs->m_getcwd_func)
        return X_ERR_NOT_SUPPORTED;

    const XError err = vfs->m_getcwd_func(vfs->m_realfs, buf, size);
    return err;
}


XError xvfs_remove(XVirtualFs* vfs, const char* path)
{
    X_ASSERT_SELF(vfs);
    if (!vfs->m_remove_func)
        return X_ERR_NOT_SUPPORTED;

    const XError err = vfs->m_remove_func(vfs->m_realfs, path);
    return err;
}


XError xvfs_rename(XVirtualFs* vfs, const char* oldpath, const char* newpath)
{
    X_ASSERT_SELF(vfs);
    if (!vfs->m_rename_func)
        return X_ERR_NOT_SUPPORTED;

    const XError err = vfs->m_rename_func(vfs->m_realfs, oldpath, newpath);
    return err;
}


XError xvfs_stat(XVirtualFs* vfs, const char* path, XStat* statbuf)
{
    X_ASSERT_SELF(vfs);
    if (!vfs->m_stat_func)
        return X_ERR_NOT_SUPPORTED;

    const XError err = vfs->m_stat_func(vfs->m_realfs, path, statbuf);
    return err;
}


XError xvfs_utime(XVirtualFs* vfs, const char* path, XTime time)
{
    X_ASSERT_SELF(vfs);
    if (!vfs->m_utime_func)
        return X_ERR_NOT_SUPPORTED;

    const XError err = vfs->m_utime_func(vfs->m_realfs, path, time);
    return err;
}
