/**
 *       @file  xposixfs.c
 *      @brief
 *
 *    @details
 *
 *     @author  MaskedW
 *
 *   @internal
 *     Created  2016/01/30/
 * ===================================================================
 */

/*
 * License: MIT license
 * Copyright (c) <2016> <MaskedW [maskedw00@gmail.com]>
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * fps (the "Software"), to deal in the Software without
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


#include <picox/filesystem/xposixfs.h>
#include <picox/filesystem/xflib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <utime.h>
#include <errno.h>
#include <unistd.h>


static char* X__ResolvePath(const XPosixFs* fs, const char* path);
static XError X__GetError(void);


XError xposixfs_init(XPosixFs* fs, const char* fakeroot)
{
    X_ASSERT(fs);
    X_ASSERT(fakeroot);

    XError err = X_ERR_NONE;
    char* real = NULL;
    const char* p = fakeroot;

    if (fakeroot[0] != '/')
    {
        real = realpath(fakeroot, NULL);
        if (!real)
        {
            err = X__GetError();
            goto x__exit;
        }

        p = real;
    }

    if (x_strequal(p, "/"))
        fs->m_prefix_len = 0;
    else
    {
        fs->m_prefix_len = strlen(p);
        fs->m_cwd = x_malloc(fs->m_prefix_len + 2);
        if (!fs->m_cwd)
        {
            err = X_ERR_NO_MEMORY;
            goto x__exit;
        }

        memcpy(fs->m_cwd, p, fs->m_prefix_len);
        memcpy(fs->m_cwd + fs->m_prefix_len, "/", 2);
    }

x__exit:
    if (real)
        free(real);

    return err;
}


void xposixfs_deinit(XPosixFs* fs)
{
    x_free(fs->m_cwd);
}


XError xposixfs_open(XPosixFs* fs, XFile* fp, const char* path, const char* mode)
{
    X_ASSERT(fs);
    X_ASSERT(fp);
    X_ASSERT(path);
    X_ASSERT(mode);

    fp->m_tag = X_POSIXFS_TAG;
    fp->m_file_handle= NULL;
    fp->m_fs = fs;

    XError err = X_ERR_NONE;
    const char* const rpath = X__ResolvePath(fs, path);
    if (!rpath)
    {
        err = X_ERR_NO_MEMORY;
        goto x__exit;
    }

    fp->m_file_handle = fopen(rpath, mode);
    if (!fp->m_file_handle)
        err = X__GetError();

x__exit:
    x_free((char*)rpath);

    return err;
}


XError xposixfs_close(XFile* fp)
{
    XError err = X_ERR_NONE;
    if (!fp)
        return err;

    X_ASSERT(fp->m_tag == X_POSIXFS_TAG);

    if (!fp->m_file_handle)
        return err;

    if (fclose(fp->m_file_handle) != 0)
        err = X__GetError();

    fp->m_file_handle = NULL;

    return err;
}


XError xposixfs_write(XFile* fp, const void* src, size_t size, size_t* nwritten)
{
    X_ASSERT(fp);
    X_ASSERT(fp->m_tag == X_POSIXFS_TAG);
    X_ASSERT(fp->m_file_handle);
    X_ASSERT(src);

    XError err = X_ERR_NONE;
    clearerr(fp->m_file_handle);
    const size_t n = fwrite(src, 1, size, fp->m_file_handle);

    X_ASSIGN_NOT_NULL(nwritten, n);
    if ((n != size) && ferror(fp->m_file_handle))
        err = X__GetError();

    return err;
}


XError xposixfs_read(XFile* fp, void* dst, size_t size, size_t* nread)
{
    X_ASSERT(fp);
    X_ASSERT(fp->m_tag == X_POSIXFS_TAG);
    X_ASSERT(fp->m_file_handle);
    X_ASSERT(dst);

    XError err = X_ERR_NONE;
    clearerr(fp->m_file_handle);
    const size_t n = fread(dst, 1, size, fp->m_file_handle);

    X_ASSIGN_NOT_NULL(nread, n);
    if ((n != size) && ferror(fp->m_file_handle))
        err = X__GetError();

    return err;
}


void xposixfs_init_vfs(XPosixFs* fs, XVirtualFs* vfs)
{
    vfs->m_fs_handle        = fs;
    vfs->m_open_func        = (XVirtualFsOpenFunc)xposixfs_open;
    vfs->m_close_func       = (XVirtualFsCloseFunc)xposixfs_close;
    vfs->m_read_func        = (XVirtualFsReadFunc)xposixfs_read;
    vfs->m_write_func       = (XVirtualFsWriteFunc)xposixfs_write;
    vfs->m_seek_func        = (XVirtualFsSeekFunc)xposixfs_seek;
    vfs->m_tell_func        = (XVirtualFsTellFunc)xposixfs_tell;
    vfs->m_flush_func       = (XVirtualFsFlushFunc)xposixfs_flush;
    vfs->m_mkdir_func       = (XVirtualFsMkdirFunc)xposixfs_mkdir;
    vfs->m_opendir_func     = (XVirtualFsOpendirFunc)xposixfs_opendir;
    vfs->m_readdir_func     = (XVirtualFsReaddirFunc)xposixfs_readdir;
    vfs->m_closedir_func    = (XVirtualFsClosedirFunc)xposixfs_closedir;
    vfs->m_chdir_func       = (XVirtualFsChdirFunc)xposixfs_chdir;
    vfs->m_getcwd_func      = (XVirtualFsGetcwdFunc)xposixfs_getcwd;
    vfs->m_remove_func      = (XVirtualFsRemoveFunc)xposixfs_remove;
    vfs->m_rename_func      = (XVirtualFsRenameFunc)xposixfs_rename;
    vfs->m_stat_func        = (XVirtualFsStatFunc)xposixfs_stat;
    vfs->m_utime_func       = (XVirtualFsUtimeFunc)xposixfs_utime;
}


XError xposixfs_seek(XFile* fp, XOffset pos, XSeekMode whence)
{
    X_ASSERT(fp);
    X_ASSERT(fp->m_file_handle);

    XError err = X_ERR_NONE;
    FILE* const file = fp->m_file_handle;
    if (fseek(file, pos, whence) == -1)
        err = X__GetError();

    return err;
}


XError xposixfs_tell(XFile* fp, XSize* pos)
{
    X_ASSERT(fp);
    X_ASSERT(fp->m_file_handle);
    X_ASSERT(pos);

    XError err = X_ERR_NONE;
    FILE* const file = fp->m_file_handle;

    /* XSizeはuint32_tとしており、longがint32_tであった場合、2G以上のファイルに
     * 対するtellの結果は不正になる可能性がある。seekをfsetpos、tellをfgetposを
     * 使用する実装にそればこの問題を回避できるが、このライブラリの利用シーン的
     * に、2G以上のファイルを使うことはほぼありえないので、ひとまず保留。fsetpos
     * にはwhence引数がないので、whence対応がめんどくさいのだ。
     */
    const long ret = ftell(file);
    if (ret == -1)
        err = X__GetError();
    else
        *pos = ret;

    return err;
}


XError xposixfs_flush(XFile* fp)
{
    X_ASSERT(fp);
    X_ASSERT(fp->m_file_handle);

    XError err = X_ERR_NONE;
    FILE* const file = fp->m_file_handle;
    if (fflush(file) != 0)
        err = X__GetError();

    return err;
}


XError xposixfs_mkdir(XPosixFs* fs, const char* path)
{
    X_ASSERT(fs);
    X_ASSERT(path);

    XError err = X_ERR_NONE;
    const char* const rpath = X__ResolvePath(fs, path);
    if (!rpath)
    {
        err = X_ERR_NO_MEMORY;
        goto x__exit;
    }

#ifdef _WIN32
    const int result = mkdir(rpath);
#else
    const int result = mkdir(rpath, 0777);
#endif
    if (result != 0)
        err = X__GetError();

x__exit:
    x_free((char*)rpath);

    return err;
}


XError xposixfs_opendir(XPosixFs* fs, XDir* dir, const char* path)
{
    X_ASSERT(fs);
    X_ASSERT(dir);

    XError err = X_ERR_NONE;
    const char* const rpath = X__ResolvePath(fs, path);
    if (!rpath)
    {
        err = X_ERR_NO_MEMORY;
        goto x__exit;
    }

    dir->m_dir_handle = opendir(rpath);
    if (!dir->m_dir_handle)
        err = X__GetError();

x__exit:
    x_free((char*)rpath);

    return err;
}


XError xposixfs_readdir(XDir* dir, XDirEnt* dirent, XDirEnt** result)
{
    X_ASSERT(dir);
    X_ASSERT(dir->m_dir_handle);
    X_ASSERT(dirent);
    X_ASSERT(result);

    XError err = X_ERR_NONE;
    *result = NULL;
    struct dirent* const dirent_handle = x_malloc(sizeof(struct dirent));
    if (!dirent_handle)
    {
        err = X_ERR_NO_MEMORY;
        goto x__exit;
    }

    DIR* const dir_handle = dir->m_dir_handle;
    struct dirent* ret;
    if (readdir_r(dir_handle, dirent_handle, &ret) != 0)
    {
        err = X__GetError();
        goto x__exit;
    }

    if (!ret)
        goto x__exit;

    const size_t len = strlen(dirent_handle->d_name);
    if (len >= X_NAME_MAX)
    {
        err = X_ERR_RANGE;
        goto x__exit;
    }

    strcpy(dirent->name, dirent_handle->d_name);
    *result = dirent;

x__exit:
    x_free(dirent_handle);

    return err;
}


XError xposixfs_closedir(XDir* dir)
{
    XError err = X_ERR_NONE;
    if (!dir)
        return err;

    X_ASSERT(dir->m_tag == X_POSIXFS_TAG);
    if (!dir->m_dir_handle)
        return err;

    if (closedir(dir->m_dir_handle) != 0)
        err = X__GetError();

    dir->m_dir_handle = NULL;

    return err;
}


XError xposixfs_chdir(XPosixFs* fs, const char* path)
{
    X_ASSERT(fs);
    X_ASSERT(path);

    XError err = X_ERR_NONE;
    char* rpath = X__ResolvePath(fs, path);
    if (!rpath)
    {
        err = X_ERR_NO_MEMORY;
        goto x__exit;
    }

    if (chdir(rpath) != 0)
    {
        err = X__GetError();
        goto x__exit;
    }

    x_free(fs->m_cwd);
    fs->m_cwd = rpath;
    rpath = NULL;

x__exit:
    x_free((char*)rpath);

    return err;
}


XError xposixfs_getcwd(XPosixFs* fs, char* buf, size_t size)
{
    X_ASSERT(fs);
    X_ASSERT(buf);

    const size_t len = strlen(fs->m_cwd);
    if (size <= len - fs->m_prefix_len)
        return X_ERR_RANGE;

    strcpy(buf, fs->m_cwd + fs->m_prefix_len);

    return X_ERR_NONE;
}


XError xposixfs_remove(XPosixFs* fs, const char* path)
{
    X_ASSERT(fs);
    X_ASSERT(path);

    XError err = X_ERR_NONE;
    const char* const rpath = X__ResolvePath(fs, path);
    if (!rpath)
    {
        err = X_ERR_NO_MEMORY;
        goto x__exit;
    }

    if (remove(rpath) != 0)
        err = X__GetError();

x__exit:
    x_free((char*)rpath);

    return err;
}


XError xposixfs_rename(XPosixFs* fs, const char* oldpath, const char* newpath)
{
    X_ASSERT(fs);
    X_ASSERT(oldpath);
    X_ASSERT(newpath);

    XError err = X_ERR_NONE;
    const char* const oldrpath = X__ResolvePath(fs, oldpath);
    const char* const newrpath = X__ResolvePath(fs, newpath);

    if ((!oldrpath) || (!newrpath))
    {
        err = X_ERR_NO_MEMORY;
        goto x__exit;
    }

    if (rename(oldrpath, newrpath) != 0)
        err = X__GetError();

x__exit:
    x_free((char*)oldrpath);
    x_free((char*)newrpath);

    return err;
}


XError xposixfs_stat(XPosixFs* fs, XStat* statbuf, const char* path)
{
    X_ASSERT(fs);
    X_ASSERT(statbuf);
    X_ASSERT(path);

    XError err = X_ERR_NONE;
    const char* const rpath = X__ResolvePath(fs, path);
    if (!rpath)
    {
        err = X_ERR_NO_MEMORY;
        goto x__exit;
    }

    struct stat buf;
    if (stat(path, &buf) != 0)
    {
        err = X__GetError();
        goto x__exit;
    }

    /* [TODO] mode */
    statbuf->size = buf.st_size;
    statbuf->mtime = buf.st_mtime;

x__exit:
    x_free((char*)rpath);

    return err;
}


XError xposixfs_utime(XPosixFs* fs, const char* path, XTime time)
{
    X_ASSERT(fs);
    X_ASSERT(path);

    XError err = X_ERR_NONE;
    const char* const rpath = X__ResolvePath(fs, path);
    if (!rpath)
    {
        err = X_ERR_NO_MEMORY;
        goto x__exit;
    }

    struct utimbuf times;
    times.actime = time;
    times.modtime = time;

    if (utime(rpath, &times) != 0)
        err = X__GetError();

x__exit:
    x_free((char*)rpath);

    return err;
}


static XError X__GetError(void)
{
    XError err;
    switch (errno)
    {
        case EROFS:
        case EACCES:    err = X_ERR_ACCESS;     break;
        case EEXIST:    err = X_ERR_EXIST;      break;
        case EBUSY:     err = X_ERR_BUSY;       break;
        case EINVAL:    err = X_ERR_INVALID;    break;
        case EIO:       err = X_ERR_IO;         break;
        case ETIMEDOUT: err = X_ERR_TIMED_OUT;  break;
        case ENOMEM:    err = X_ERR_NO_MEMORY;  break;
        case ENFILE:
        case EMFILE:    err = X_ERR_MANY;       break;
        case ENOENT:    err = X_ERR_NO_ENTRY;   break;
        case ERANGE:    err = X_ERR_RANGE;      break;
        default:        err = X_ERR_OTHER;      break;
    }
    return err;
}


static char* X__ResolvePath(const XPosixFs* fs, const char* path)
{
    const size_t size = fs->m_prefix_len + X_PATH_MAX;
    char* buf = x_malloc(size);
    if (! buf)
        return NULL;

    if (! xflib_resolve(buf, size, fs->m_cwd, fs->m_prefix_len, path))
    {
        x_free(buf);
        return NULL;
    }

    return buf;
}
