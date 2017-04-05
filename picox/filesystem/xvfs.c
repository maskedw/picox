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


#define X__VFUNC(vfs, func)  (vfs->m_vtable->m_##func##_func)


typedef struct
{
    XVirtualFs*     vfs;
    char            path[X_PATH_MAX];
    const char*     srcdir;
    const char*     dstdir;
    int             dstdirlen;
    int             srcdirlen;
    XStat           statbuf;
    XDirEnt         direntbuf;
    XDirEnt*        dirent;
    XFile*          srcfp;
    XFile*          dstfp;
    int             depth;
} X__CopyTreeWorkBuf;


typedef struct
{
    XVirtualFs* vfs;
    char        path[X_PATH_MAX];
    XStat       statbuf;
    XDirEnt     direntbuf;
    XDirEnt*    dirent;
} X__RmTreeWorkBuf;


typedef struct
{
    XVirtualFs*     vfs;
    char            path[X_PATH_MAX];
    XStat           statbuf;
    XDirEnt         direntbuf;
    XDirEnt*        dirent;
    XFsTreeWalker   walker;
    void*           userptr;
} X__WalkTreeWorkBuf;


static XError X__DoCopyTree(X__CopyTreeWorkBuf* work, int tail);
static XError X__DoRmTree(X__RmTreeWorkBuf* work, int tail);
static XError X__DoWalkTree(X__WalkTreeWorkBuf* work, int tail);


static const XStreamVTable X__vfs_filestream_vtable = {
    .m_name = "XVirualFsFileStream",
    .m_read_func = (XStreamReadFunc)xvfs_read,
    .m_write_func = (XStreamWriteFunc)xvfs_write,
    .m_close_func = (XStreamCloseFunc)xvfs_close,
    .m_flush_func = (XStreamFlushFunc)xvfs_flush,
    .m_seek_func = (XStreamSeekFunc)xvfs_seek,
    .m_tell_func = (XStreamTellFunc)xvfs_tell,
};


void xvfs_init(XVirtualFs* vfs)
{
    X_ASSERT(vfs);
    X_RESET_RTTI(vfs);
}


XStream* xvfs_init_stream(XStream* stream, XFile* fp)
{
    X_ASSERT(stream);
    X_ASSERT(fp);

    xstream_init(stream);
    stream->m_rtti_tag = &XFILE_STREAM_RTTI_TAG;
    stream->m_driver = fp;
    stream->m_vtable = &X__vfs_filestream_vtable;

    return stream;
}


XError xvfs_open(XVirtualFs* vfs, const char* path, XOpenMode mode, XFile** o_fp)
{
    XError err;

    X_ASSERT(vfs);
    if (!X__VFUNC(vfs, open))
        return X_ERR_NOT_SUPPORTED;

    err = X__VFUNC(vfs, open)(vfs->m_driver, path, mode, o_fp);
    if (*o_fp)
        (*o_fp)->m_vfs = vfs;

    return err;
}


XError xvfs_close(XFile* fp)
{
    XError err;
    XVirtualFs* vfs;

    if (!fp)
        return X_ERR_NONE;
    X_ASSERT(fp->m_vfs);

    vfs = fp->m_vfs;
    if (!X__VFUNC(vfs, close))
        return X_ERR_NOT_SUPPORTED;

    err = X__VFUNC(vfs, close)(fp);

    return err;
}


XError xvfs_read(XFile* fp, void* dst, size_t size, size_t* nread)
{
    XError err;
    XVirtualFs* vfs;

    X_ASSERT(fp);
    X_ASSERT(fp->m_vfs);

    vfs = fp->m_vfs;
    if (!X__VFUNC(vfs, read))
        return X_ERR_NOT_SUPPORTED;

    err = X__VFUNC(vfs, read)(fp, dst, size, nread);
    return err;
}


XError xvfs_write(XFile* fp, const void* src, size_t size, size_t* nwritten)
{
    XError err;
    XVirtualFs* vfs;

    X_ASSERT(fp);
    X_ASSERT(fp->m_vfs);

    vfs = fp->m_vfs;
    if (!(X__VFUNC(vfs, write)))
        return X_ERR_NOT_SUPPORTED;

    err = X__VFUNC(vfs, write)(fp, src, size, nwritten);
    return err;
}


XError xvfs_seek(XFile* fp, XOffset pos, XSeekMode whence)
{
    XError err;
    XVirtualFs* vfs;

    X_ASSERT(fp);
    X_ASSERT(fp->m_vfs);

    vfs = fp->m_vfs;
    if (!X__VFUNC(vfs, seek))
        return X_ERR_NOT_SUPPORTED;

    err = X__VFUNC(vfs, seek)(fp, pos, whence);
    return err;
}


XError xvfs_tell(XFile* fp, XSize* pos)
{
    XError err;
    XVirtualFs* vfs;

    X_ASSERT(fp);
    X_ASSERT(fp->m_vfs);

    vfs = fp->m_vfs;
    if (!X__VFUNC(vfs, tell))
        return X_ERR_NOT_SUPPORTED;

    err = X__VFUNC(vfs, tell)(fp, pos);
    return err;
}


XError xvfs_flush(XFile* fp)
{
    XError err;
    XVirtualFs* vfs;

    X_ASSERT(fp);
    X_ASSERT(fp->m_vfs);

    /* flushはVirtualFsの実装によっては不要な関数なので、未登録であれば、正常終
     * 了として扱う。
     */
    vfs = fp->m_vfs;
    if (!X__VFUNC(vfs, flush))
        return X_ERR_NONE;

    err = X__VFUNC(vfs, flush)(fp);
    return err;
}


XError xvfs_mkdir(XVirtualFs* vfs, const char* path)
{
    XError err;

    X_ASSERT(vfs);

    err = X__VFUNC(vfs, mkdir)(vfs->m_driver, path);
    return err;
}


XError xvfs_opendir(XVirtualFs* vfs, const char* path, XDir** o_dir)
{
    XError err;

    X_ASSERT(vfs);

    err = X__VFUNC(vfs, opendir)(vfs->m_driver, path, o_dir);
    if (*o_dir)
        (*o_dir)->m_vfs = vfs;

    return err;
}


XError xvfs_readdir(XDir* dir, XDirEnt* dirent, XDirEnt** result)
{
    XError err;
    XVirtualFs* vfs;

    X_ASSERT(dir);
    X_ASSERT(dir->m_vfs);

    vfs = dir->m_vfs;
    err = X__VFUNC(vfs, readdir)(dir, dirent, result);

    return err;
}


XError xvfs_closedir(XDir* dir)
{
    XError err;
    XVirtualFs* vfs;

    if (!dir)
        return X_ERR_NONE;

    X_ASSERT(dir);
    X_ASSERT(dir->m_vfs);

    vfs = dir->m_vfs;
    err = X__VFUNC(vfs, closedir)(dir);

    return err;
}


XError xvfs_chdir(XVirtualFs* vfs, const char* path)
{
    XError err;

    X_ASSERT(vfs);

    err = X__VFUNC(vfs, chdir)(vfs->m_driver, path);

    return err;
}


XError xvfs_getcwd(XVirtualFs* vfs, char* buf, size_t size)
{
    XError err;

    X_ASSERT(vfs);

    err = X__VFUNC(vfs, getcwd)(vfs->m_driver, buf, size);

    return err;
}


XError xvfs_remove(XVirtualFs* vfs, const char* path)
{
    XError err;

    X_ASSERT(vfs);

    err = X__VFUNC(vfs, remove)(vfs->m_driver, path);

    return err;
}


XError xvfs_rename(XVirtualFs* vfs, const char* oldpath, const char* newpath)
{
    XError err;

    X_ASSERT(vfs);

    err = X__VFUNC(vfs, rename)(vfs->m_driver, oldpath, newpath);

    return err;
}


XError xvfs_stat(XVirtualFs* vfs, const char* path, XStat* statbuf)
{
    XError err;

    X_ASSERT(vfs);

    err = X__VFUNC(vfs, stat)(vfs->m_driver, path, statbuf);

    return err;
}


XError xvfs_utime(XVirtualFs* vfs, const char* path, XTime time)
{
    XError err;

    X_ASSERT(vfs);

    err = X__VFUNC(vfs, utime)(vfs->m_driver, path, time);

    return err;
}


XError xvfs_exists(XVirtualFs* vfs, const char* path, bool* exists)
{
    XStat statbuf;
    XError err;

    X_ASSERT(exists);
    err = xvfs_stat(vfs, path, &statbuf);

    *exists = (err == X_ERR_NONE);

    /* 存在チェックの時のNO_ENTRYは文脈的にエラーではない*/
    if (err == X_ERR_NO_ENTRY)
        err = X_ERR_NONE;

    return err;
}


XError xvfs_is_directory(XVirtualFs* vfs, const char* path, bool* isdir)
{
    XStat statbuf;
    XError err;

    X_ASSERT(isdir);
    err = xvfs_stat(vfs, path, &statbuf);
    *isdir = ((err == X_ERR_NONE) && (XSTAT_IS_DIRECTORY(statbuf.mode)));

    return err;
}


XError xvfs_is_regular(XVirtualFs* vfs, const char* path, bool* isreg)
{
    XStat statbuf;
    XError err;

    X_ASSERT(isreg);

    err = xvfs_stat(vfs, path, &statbuf);
    *isreg = ((err == X_ERR_NONE) && (XSTAT_IS_REGULAR(statbuf.mode)));

    return err;
}


int xvfs_putc(XFile* fp, int c)
{
    uint8_t b = c;
    const XError err = xvfs_write(fp, &b, sizeof(b), NULL);
    return (err == X_ERR_NONE) ? c : EOF;
}


int xvfs_puts(XFile* fp, const char* str)
{
    const XError err = xvfs_write(fp, str, strlen(str), NULL);
    return (err == X_ERR_NONE) ? 0 : EOF;
}


int xvfs_printf(XFile* fp, const char* fmt, ...)
{
    va_list args;
    int len;

    va_start(args, fmt);
    len = xvfs_vprintf(fp, fmt, args);
    va_end(args);
    return len;
}


int xvfs_vprintf(XFile* fp, const char* fmt, va_list args)
{
    XStream fstream;
    xvfs_init_stream(&fstream, fp);
    return x_vprintf_to_stream(&fstream, fmt, args);
}


int xvfs_getc(XFile* fp)
{
    uint8_t b;
    size_t nread;
    const XError err = xvfs_read(fp, &b, sizeof(b), &nread);
    return ((err == X_ERR_NONE) && (nread == sizeof(b))) ? b : EOF;
}


XError xvfs_gets(XFile* fp, char* dst, size_t size, char** result, bool* overflow)
{
    XStream fstream;
    xvfs_init_stream(&fstream, fp);
    return (XError)xstream_gets(&fstream, dst, size, result, overflow);
}


XError xvfs_copyfile(XVirtualFs* vfs, const char* src, const char* dst)
{
    XError err;
    XFile* sfp = NULL;
    XFile* dfp = NULL;

    err = xvfs_open(vfs, src, X_OPEN_MODE_READ, &sfp);
    if (err)
        goto x__exit;

    err = xvfs_open(vfs, dst, X_OPEN_MODE_WRITE, &dfp);
    if (err)
        goto x__exit;

    err = xvfs_copyfile2(sfp, dfp);

x__exit:
    xvfs_close(sfp);
    xvfs_close(dfp);

    return err;
}


XError xvfs_copyfile2(XFile* src, XFile* dst)
{
    const size_t X__BLOCK_SIZE = 512;
    uint8_t* buf;
    size_t nread;
    size_t nwritten;
    XError err;

    X_ASSERT(dst);
    X_ASSERT(src);


    buf = x_malloc(X__BLOCK_SIZE);
    if (!buf)
    {
        err = X_ERR_NO_MEMORY;
        goto x__exit;
    }


    for (;;)
    {

        err = xvfs_read(src, buf, X__BLOCK_SIZE, &nread);
        if (err)
            goto x__exit;

        if (nread == 0)
            break;

        err = xvfs_write(dst, buf, nread, &nwritten);
        if (err)
            goto x__exit;

        if (nread != nwritten)
        {
            err = X_ERR_NO_SPACE;
            goto x__exit;
        }
    }

    err = xvfs_flush(dst);

x__exit:
    x_free(buf);

    return err;
}


XError xvfs_copytree(XVirtualFs* vfs, const char* src, const char* dst)
{
    X__CopyTreeWorkBuf* work;
    XError err;

    X_ASSERT(dst);
    X_ASSERT(src);

    work = x_malloc(sizeof(X__CopyTreeWorkBuf));
    if (!work)
        return X_ERR_NO_MEMORY;

    err = X_ERR_NONE;
    memset(work, 0, sizeof(*work));
    work->vfs = vfs;
    work->srcdir = src;
    work->dstdir = dst;
    work->srcdirlen = strlen(src);
    work->dstdirlen = strlen(dst);
    work->depth = 0;

    /* srcはディレクトリでなければならない */
    err = xvfs_stat(work->vfs, src, &work->statbuf);
    if (err)
        goto x__exit;
    if (!XSTAT_IS_DIRECTORY(work->statbuf.mode))
    {
        err = X_ERR_NOT_DIRECTORY;
        goto x__exit;
    }

    /* dstはディレクトリとして作成可能でなければならない */
    err = xvfs_mkdir(work->vfs, dst);
    if (err)
        goto x__exit;

    /* コピー処理本体 */
    x_strlcpy(work->path, src, X_PATH_MAX);
    err = X__DoCopyTree(work, strlen(work->path));

x__exit:
    if (work)
        x_free(work);

    return err;
}


XError xvfs_rmtree(XVirtualFs* vfs, const char* path)
{
    XError err = X_ERR_NONE;
    X__RmTreeWorkBuf* work;

    X_ASSERT(path);

    work = x_malloc(sizeof(X__RmTreeWorkBuf));
    if (!work)
    {
        err = X_ERR_NO_MEMORY;
        goto x__exit;
    }

    work->vfs = vfs;
    x_strlcpy(work->path, path, X_PATH_MAX);
    err = X__DoRmTree(work, strlen(work->path));

x__exit:
    x_free(work);

    return err;
}


XError xvfs_makedirs(XVirtualFs* vfs, const char* path, bool exist_ok)
{
    XError err = X_ERR_NONE;
    char buf[X_PATH_MAX];
    const char* endptr = path;

    if (strlen(path) >= X_PATH_MAX)
        return X_ERR_NAME_TOO_LONG;

    while (xfpath_top(endptr, (char**)&endptr))
    {
        memcpy(buf, path, endptr - path);
        buf[(size_t)(endptr - path)] = '\0';

        err = xvfs_mkdir(vfs, buf);
        if ((err != X_ERR_NONE) && (err != X_ERR_EXIST))
            goto x__exit;
    }

    if ((err == X_ERR_EXIST) && exist_ok)
        err = X_ERR_NONE;

x__exit:
    return err;
}


XError xvfs_walktree(XVirtualFs* vfs, const char* path, XFsTreeWalker walker, void* userptr)
{
    X__WalkTreeWorkBuf* work;
    XError err;
    char tmp[X_PATH_MAX];

    X_ASSERT(path);
    X_ASSERT(walker);

    work = x_malloc(sizeof(X__WalkTreeWorkBuf));
    if (!work)
        return X_ERR_NO_MEMORY;

    err = X_ERR_NONE;
    memset(work, 0, sizeof(*work));
    work->vfs = vfs;
    work->walker = walker;
    work->userptr = userptr;
    work->dirent = &work->direntbuf;

    err = xvfs_stat(vfs, path, &work->statbuf);
    if (err)
        goto x__exit;
    if (!XSTAT_IS_DIRECTORY(work->statbuf.mode))
    {
        err = X_ERR_NOT_DIRECTORY;
        goto x__exit;
    }

    err = xvfs_getcwd(vfs, work->path, X_PATH_MAX);
    if (err)
        goto x__exit;

    xfpath_resolve(tmp, work->path, path, X_PATH_MAX);
    if (xfpath_is_root(tmp))
        strcpy(work->dirent->name, tmp);
    else
    {
        char* endptr;
        char* p = xfpath_name(tmp, &endptr);
        memcpy(work->dirent->name, p, endptr - p);
        work->dirent->name[(size_t)(endptr - p)] = '\0';
    }

    x_strlcpy(work->path, path, X_PATH_MAX);
    err = X__DoWalkTree(work, strlen(work->path));

x__exit:
    if (work)
        x_free(work);

    return err;

}


static XError X__DoCopyTree(X__CopyTreeWorkBuf* work, int tail)
{
    XError err = X_ERR_NONE;
    XDir* dir = NULL;
    int len;

    err = xvfs_stat(work->vfs, work->path, &work->statbuf);
    if (err)
        goto x__exit;

    if (!XSTAT_IS_DIRECTORY(work->statbuf.mode))
    {
        /* ファイルコピー */
        work->srcfp = NULL;
        work->dstfp = NULL;
        do
        {
            err = xvfs_open(work->vfs, work->path, X_OPEN_MODE_READ, &work->srcfp);
            if (err)
                break;

            /* パスをdst側に置き換える */
            if (!x_strreplace(work->path, X_PATH_MAX, strlen(work->path),
                              work->srcdirlen, work->dstdir, work->dstdirlen))
            {
                err = X_ERR_NAME_TOO_LONG;
                break;
            }

            err = xvfs_open(work->vfs, work->path, X_OPEN_MODE_WRITE, &work->dstfp);
            if (err)
                break;

            err = xvfs_copyfile2(work->srcfp, work->dstfp);
            if (err)
                break;

            err = xvfs_close(work->dstfp);
            if (err)
                break;
            work->dstfp = NULL;

            err = xvfs_close(work->srcfp);
            if (err)
                break;
            work->srcfp = NULL;

            /* パスをsrc基準に戻す。成功するのは確定しているからエラーチェックは不要だ  */
            x_strreplace(work->path, X_PATH_MAX, strlen(work->path),
                         work->dstdirlen, work->srcdir, work->srcdirlen);
        } while (0);

        if (work->dstfp)
            xvfs_close(work->dstfp);
        if (work->srcfp)
            xvfs_close(work->srcfp);
    }
    else
    {
        /* ディレクトリコピー */
        err = xvfs_opendir(work->vfs, work->path, &dir);
        if (err)
            goto x__exit;

        /* パスをdst側に置き換える */
        if (!x_strreplace(work->path, X_PATH_MAX, strlen(work->path),
                          work->srcdirlen, work->dstdir, work->dstdirlen))
        {
            err = X_ERR_NAME_TOO_LONG;
            goto x__exit;
        }


        /* 1階層目のディレクトリは作成済みであることが前提 */
        if (work->depth != 0)
        {
            err = xvfs_mkdir(work->vfs, work->path);
            if (err)
                goto x__exit;
        }

        /* パスをsrc基準に戻す。成功するのは確定しているからエラーチェックは不要だ  */
        x_strreplace(work->path, X_PATH_MAX, strlen(work->path),
                     work->dstdirlen, work->srcdir, work->srcdirlen);

        for (;;)
        {
            err = xvfs_readdir(dir, &work->direntbuf, &work->dirent);
            if (err)
                goto x__exit;

            if (!work->dirent)
                break;

            if (x_strequal(".", work->dirent->name))
                continue;

            if (x_strequal("..", work->dirent->name))
                continue;

            len = x_snprintf(work->path + tail,
                             X_PATH_MAX - tail,
                             "/%s",
                             work->dirent->name);

            /* 再帰呼出し */
            work->depth += 1;
            err = X__DoCopyTree(work, tail + len);
            if (err)
                goto x__exit;
        }

        err = xvfs_closedir(dir);
        if (err)
        {
            dir = NULL;
            goto x__exit;
        }

        dir = NULL;
        work->path[tail] = '\0';
    }

x__exit:
    if (dir)
        xvfs_closedir(dir);

    return err;
}


static XError X__DoRmTree(X__RmTreeWorkBuf* work, int tail)
{
    XError err = X_ERR_NONE;
    XDir* dir = NULL;
    int len = 0;

    err = xvfs_stat(work->vfs, work->path, &work->statbuf);
    if (err)
        goto x__exit;

    if (!XSTAT_IS_DIRECTORY(work->statbuf.mode))
    {
        err = xvfs_remove(work->vfs, work->path);
        if (err)
            goto x__exit;
    }
    else
    {
        err = xvfs_opendir(work->vfs, work->path, &dir);
        if (err)
            goto x__exit;

        for (;;)
        {
            err = xvfs_readdir(dir, &work->direntbuf, &work->dirent);
            if (err)
                goto x__exit;

            if (!work->dirent)
                break;

            if (x_strequal(".", work->dirent->name) ||
                x_strequal("..", work->dirent->name))
                continue;

            len = x_snprintf(work->path + tail, X_PATH_MAX - tail,
                             "/%s", work->dirent->name);

            /* 再帰呼出し */
            err = X__DoRmTree(work, tail + len);
            if (err)
                goto x__exit;
        }

        err = xvfs_closedir(dir);
        dir = NULL;
        if (err)
            goto x__exit;

        work->path[tail] = '\0';
        err = xvfs_remove(work->vfs, work->path);
    }

x__exit:
    if (dir)
        xvfs_closedir(dir);

    return err;
}


static XError X__DoWalkTree(X__WalkTreeWorkBuf* work, int tail)
{
    XError err = X_ERR_NONE;
    XDir* dir = NULL;
    int len;

    err = xvfs_stat(work->vfs, work->path, &work->statbuf);
    if (err)
        goto x__exit;

    /* コールバック呼び出し */
    if (!work->walker(work->userptr, work->path, &work->statbuf, work->dirent))
        goto x__exit;

    if (XSTAT_IS_DIRECTORY(work->statbuf.mode))
    {
        err = xvfs_opendir(work->vfs, work->path, &dir);
        if (err)
            goto x__exit;

        for (;;)
        {
            err = xvfs_readdir(dir, &work->direntbuf, &work->dirent);
            if (err)
                goto x__exit;

            if (!work->dirent)
                break;

            if (x_strequal(".", work->dirent->name))
                continue;

            if (x_strequal("..", work->dirent->name))
                continue;

            if (tail == 1)
            {
                len = x_snprintf(work->path + tail,
                                 X_PATH_MAX - tail,
                                 "%s",
                                 work->dirent->name);
            }
            else
            {
                len = x_snprintf(work->path + tail,
                                 X_PATH_MAX - tail,
                                 "/%s",
                                 work->dirent->name);
            }
            err = X__DoWalkTree(work, tail + len);
            if (err)
                goto x__exit;
        }

        err = xvfs_closedir(dir);
        dir = NULL;
        if (err)
            goto x__exit;
        work->path[tail] = '\0';
    }

x__exit:
    if (dir)
        xvfs_closedir(dir);

    return err;
}
