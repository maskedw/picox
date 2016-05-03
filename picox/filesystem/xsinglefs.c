/**
 *       @file  xsinglefs.h
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

#include <picox/filesystem/xsinglefs.h>


XVirtualFs* x_g_vfs;


void xsinglefs_init(XVirtualFs* vfs)
{
    x_g_vfs = vfs;
}


void xsinglefs_deinit()
{
}


XError xsinglefs_open(const char* path, XOpenMode mode, XFile** o_fp)
{
    return xvfs_open(x_g_vfs, path, mode, o_fp);
}


XError xsinglefs_mkdir(const char* path)
{
    return xvfs_mkdir(x_g_vfs, path);
}


XError xsinglefs_opendir(const char* path, XDir** o_dir)
{
    return xvfs_opendir(x_g_vfs, path, o_dir);
}


XError xsinglefs_chdir(const char* path)
{
    return xvfs_chdir(x_g_vfs, path);
}


XError xsinglefs_getcwd(char* path, size_t size)
{
    return xvfs_getcwd(x_g_vfs, path, size);
}


XError xsinglefs_remove(const char* path)
{
    return xvfs_remove(x_g_vfs, path);
}


XError xsinglefs_rename(const char* oldpath, const char* newpath)
{
    return xvfs_rename(x_g_vfs, oldpath, newpath);
}


XError xsinglefs_stat(const char* path, XStat* statbuf)
{
    return xvfs_stat(x_g_vfs, path, statbuf);
}


XError xsinglefs_utime(const char* path, XTime time)
{
    return xvfs_utime(x_g_vfs, path, time);
}


XError xsinglefs_exists(const char* path, bool* exists)
{
    return xvfs_exists(x_g_vfs, path, exists);
}


XError xsinglefs_is_directory(const char* path, bool* isdir)
{
    return xvfs_is_directory(x_g_vfs, path, isdir);
}


XError xsinglefs_is_regular(const char* path, bool* isreg)
{
    return xvfs_is_regular(x_g_vfs, path, isreg);
}


XError xsinglefs_copyfile(const char* src, const char* dst)
{
    return xvfs_copyfile(x_g_vfs, src, dst);
}


XError xsinglefs_copytree(const char* src, const char* dst)
{
    return xvfs_copytree(x_g_vfs, src, dst);
}


XError xsinglefs_rmtree(const char* path)
{
    return xvfs_rmtree(x_g_vfs, path);
}


XError xsinglefs_makedirs(const char* path, bool exist_ok)
{
    return xvfs_makedirs(x_g_vfs, path, exist_ok);
}


XError xsinglefs_walktree(const char* path, XFsTreeWalker walker, void* userptr)
{
    return xvfs_walktree(x_g_vfs, path, walker, userptr);
}
