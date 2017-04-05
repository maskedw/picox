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
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <utime.h>
#include <errno.h>
#include <unistd.h>


static XError X__GetError(void);
static bool X__DoRmTree(char* path, int size, int tail, struct stat* statbuf);


#define X__ASSERT_TAG(p)      (X_ASSERT(((const XPosixFs*)p)->m_fstype_tag == &XPOSIXFS_RTTI_TAG))
#define X__GET_REAL_FP(fp)    (((X__FileStorage*)(fp))->realfp)
#define X__GET_REAL_DIR(dir)  (((X__DirStorage*)dir)->realdir)


typedef struct
{
    XFile   vfile;
    FILE*   realfp;
} X__FileStorage;


typedef struct
{
    XDir    vdir;
    DIR*    realdir;
} X__DirStorage;


static const XVirtualFsVTable X__posixfs_vfs_vtable = {
    .m_open_func        = (XVirtualFsOpenFunc)xposixfs_open,
    .m_close_func       = (XVirtualFsCloseFunc)xposixfs_close,
    .m_read_func        = (XVirtualFsReadFunc)xposixfs_read,
    .m_write_func       = (XVirtualFsWriteFunc)xposixfs_write,
    .m_seek_func        = (XVirtualFsSeekFunc)xposixfs_seek,
    .m_tell_func        = (XVirtualFsTellFunc)xposixfs_tell,
    .m_flush_func       = (XVirtualFsFlushFunc)xposixfs_flush,
    .m_mkdir_func       = (XVirtualFsMkdirFunc)xposixfs_mkdir,
    .m_opendir_func     = (XVirtualFsOpendirFunc)xposixfs_opendir,
    .m_readdir_func     = (XVirtualFsReaddirFunc)xposixfs_readdir,
    .m_closedir_func    = (XVirtualFsClosedirFunc)xposixfs_closedir,
    .m_chdir_func       = (XVirtualFsChdirFunc)xposixfs_chdir,
    .m_getcwd_func      = (XVirtualFsGetcwdFunc)xposixfs_getcwd,
    .m_remove_func      = (XVirtualFsRemoveFunc)xposixfs_remove,
    .m_rename_func      = (XVirtualFsRenameFunc)xposixfs_rename,
    .m_stat_func        = (XVirtualFsStatFunc)xposixfs_stat,
    .m_utime_func       = (XVirtualFsUtimeFunc)xposixfs_utime,
};
X_IMPL_RTTI_TAG(XPOSIXFS_RTTI_TAG);


void xposixfs_init(XPosixFs* fs)
{
    X_ASSERT(fs);
    fs->m_fstype_tag = &XPOSIXFS_RTTI_TAG;
}


void xposixfs_deinit(XPosixFs* fs)
{
    X_ASSERT(fs);
}


XVirtualFs* xposixfs_init_vfs(XPosixFs* fs, XVirtualFs* vfs)
{
    X_ASSERT(fs);
    X_ASSERT(vfs);

    xvfs_init(vfs);
    vfs->m_rtti_tag = &XPOSIXFS_RTTI_TAG;
    vfs->m_driver = fs;
    vfs->m_vtable = &X__posixfs_vfs_vtable;

    return vfs;
}


XError xposixfs_open(XPosixFs* fs, const char* path, XOpenMode mode, XFile** o_fp)
{
    X_ASSERT(fs);
    X_ASSERT(o_fp);
    X_ASSERT(path);
    X__ASSERT_TAG(fs);

    *o_fp = NULL;
    XError err = X_ERR_NONE;
    const char* modestr = NULL;
    switch (mode)
    {
        case X_OPEN_MODE_READ:
            modestr = "rb";     break;
        case X_OPEN_MODE_WRITE:
            modestr = "wb";     break;
        case X_OPEN_MODE_APPEND:
            modestr = "ab";     break;
        case X_OPEN_MODE_READ_PLUS:
            modestr = "r+b";    break;
        case X_OPEN_MODE_WRITE_PLUS:
            modestr = "w+b";    break;
        case X_OPEN_MODE_APPEND_PLUS:
            modestr = "a+b";    break;
        default:
            modestr = "";       break;
    }

    FILE* realfp = fopen(path, modestr);
    if (!realfp)
    {
        err = X__GetError();
        goto x__exit;
    }

#ifdef __MINGW32__
    /* https://linuxjm.osdn.jp/html/LDP_man-pages/man3/fopen.3.html
     * 上記リンク先の"a"モードの説明では、オープン時のストリームはファイルの最後
     * に位置されるとあるが、MinGWではftell()で取得する値が0になってしまう。
     * 書き込みの時はちゃんと末尾に書き込まれるので、特に問題ないのだが、他のモ
     * ジュールとの動作の対称性のために、明示的に末尾に移動させておく。
     */
    if (mode == X_OPEN_MODE_APPEND)
    {
        if (fseek(realfp, 0, SEEK_END))
        {
            err = X__GetError();
            goto x__exit;
        }
    }
#endif

    X__FileStorage* buf = x_malloc(sizeof(X__FileStorage));
    if (! buf)
    {
        err = X_ERR_NO_MEMORY;
        goto x__exit;
    }

    buf->realfp = realfp;
    buf->vfile.m_fs = fs;
    realfp = NULL;
    *o_fp = &(buf->vfile);

x__exit:
    if (realfp)
        fclose(realfp);

    return err;
}


XError xposixfs_close(XFile* fp)
{
    XError err = X_ERR_NONE;
    if (!fp)
        return err;

    X__ASSERT_TAG(fp->m_fs);
    X__FileStorage* const storage = (X__FileStorage*)fp;

    if (fclose(storage->realfp) != 0)
        err = X__GetError();
    x_free(storage);

    return err;
}


XError xposixfs_write(XFile* fp, const void* src, size_t size, size_t* nwritten)
{
    X_ASSERT(fp);
    X_ASSERT(src);
    X__ASSERT_TAG(fp->m_fs);

    XError err = X_ERR_NONE;
    FILE* const realfp = X__GET_REAL_FP(fp);

    clearerr(realfp);
    const size_t n = fwrite(src, 1, size, realfp);

    X_ASSIGN_NOT_NULL(nwritten, n);
    if ((n != size) && ferror(realfp))
        err = X__GetError();

    return err;
}


XError xposixfs_read(XFile* fp, void* dst, size_t size, size_t* nread)
{
    X_ASSERT(fp);
    X_ASSERT(dst);
    X__ASSERT_TAG(fp->m_fs);

    XError err = X_ERR_NONE;
    FILE* const realfp = X__GET_REAL_FP(fp);

    clearerr(realfp);
    const size_t n = fread(dst, 1, size, realfp);

    X_ASSIGN_NOT_NULL(nread, n);
    if ((n != size) && ferror(realfp))
        err = X__GetError();

    return err;
}


XError xposixfs_seek(XFile* fp, XOffset pos, XSeekMode whence)
{
    X_ASSERT(fp);
    X__ASSERT_TAG(fp->m_fs);

    XError err = X_ERR_NONE;
    FILE* const realfp = X__GET_REAL_FP(fp);
    if (fseek(realfp, pos, whence) == -1)
        err = X__GetError();

    return err;
}


XError xposixfs_tell(XFile* fp, XSize* pos)
{
    X_ASSERT(fp);
    X_ASSERT(pos);
    X__ASSERT_TAG(fp->m_fs);

    *pos = 0;
    XError err = X_ERR_NONE;
    FILE* const realfp = X__GET_REAL_FP(fp);

    /* XSizeはuint32_tとしており、longがint32_tであった場合、2G以上のファイルに
     * 対するtellの結果は不正になる可能性がある。seekをfsetpos、tellをfgetposを
     * 使用する実装にそればこの問題を回避できるが、このライブラリの利用シーン的
     * に、2G以上のファイルを使うことはほぼありえないので、ひとまず保留。fsetpos
     * にはwhence引数がないので、whence対応がめんどくさいのだ。
     */
    const long ret = ftell(realfp);
    if (ret == -1)
        err = X__GetError();
    else
        *pos = ret;

    return err;
}


XError xposixfs_flush(XFile* fp)
{
    X_ASSERT(fp);
    X__ASSERT_TAG(fp->m_fs);

    XError err = X_ERR_NONE;
    FILE* const realfp = X__GET_REAL_FP(fp);

    if (fflush(realfp) != 0)
        err = X__GetError();

    return err;
}


XError xposixfs_mkdir(XPosixFs* fs, const char* path)
{
    X_ASSERT(fs);
    X_ASSERT(path);
    X__ASSERT_TAG(fs);

    XError err = X_ERR_NONE;

#ifdef __MINGW32__
    const int result = mkdir(path);
#else
    const int result = mkdir(path, 0777);
#endif
    if (result != 0)
        err = X__GetError();

    return err;
}


XError xposixfs_opendir(XPosixFs* fs, const char* path, XDir** o_dir)
{
    X_ASSERT(fs);
    X_ASSERT(o_dir);
    X__ASSERT_TAG(fs);

    *o_dir = NULL;
    XError err = X_ERR_NONE;
    X__DirStorage* buf = NULL;
    DIR* realdir = opendir(path);
    if (! realdir)
    {
        err = X__GetError();
        goto x__exit;
    }

    buf = x_malloc(sizeof(X__DirStorage));
    if (! buf)
    {
        err = X_ERR_NO_MEMORY;
        goto x__exit;
    }

    buf->vdir.m_fs = fs;
    buf->realdir = realdir;
    *o_dir = &(buf->vdir);
    realdir = NULL;
    buf = NULL;

x__exit:
    if (realdir)
        closedir(realdir);
    if (buf)
        x_free(buf);

    return err;
}


XError xposixfs_readdir(XDir* dir, XDirEnt* dirent, XDirEnt** result)
{
    X_ASSERT(dir);
    X_ASSERT(dirent);
    X_ASSERT(result);
    X__ASSERT_TAG(dir->m_fs);

    XError err = X_ERR_NONE;
    DIR* const realdir = X__GET_REAL_DIR(dir);

    *result = NULL;
    errno = 0;
    struct dirent* const realent = readdir(realdir);

    if (!realent)
    {
        if (errno != 0)
            err = X__GetError();
        goto x__exit;
    }

    const size_t len = strlen(realent->d_name);
    if (len >= X_NAME_MAX)
    {
        err = X_ERR_RANGE;
        goto x__exit;
    }

    strcpy(dirent->name, realent->d_name);
    *result = dirent;

x__exit:

    return err;
}


XError xposixfs_closedir(XDir* dir)
{
    XError err = X_ERR_NONE;
    if (!dir)
        return err;

    X__ASSERT_TAG(dir->m_fs);
    X__DirStorage* const storage = (X__DirStorage*)dir;

    if (closedir(storage->realdir) != 0)
        err = X__GetError();
    x_free(storage);

    return err;
}


XError xposixfs_chdir(XPosixFs* fs, const char* path)
{
    X_ASSERT(fs);
    X_ASSERT(path);
    X__ASSERT_TAG(fs);

    XError err = X_ERR_NONE;
    if (chdir(path) != 0)
        err = X__GetError();

    return err;
}


XError xposixfs_getcwd(XPosixFs* fs, char* buf, size_t size)
{
    X_ASSERT(fs);
    X_ASSERT(buf);
    X__ASSERT_TAG(fs);

    XError err = X_ERR_NONE;
    if (!getcwd(buf, size))
        err = X__GetError();

    return err;
}


XError xposixfs_remove(XPosixFs* fs, const char* path)
{
    X_ASSERT(fs);
    X_ASSERT(path);
    X__ASSERT_TAG(fs);

    XError err = X_ERR_NONE;

#ifdef __MINGW32__
    /* MinGWだと通常ファイルを作って削除 => 同名のディレクトリを作って削除とする
     * とき(普通はそんなことしないが)に失敗する。
     * 自分でrmdir()とunlink()を呼び分けると成功する。
     */
    struct stat buf;
    int ret;
    if ((ret = stat(path, &buf)) != 0)
    {
        err = X__GetError();
        goto x__exit;
    }

    if (S_ISDIR(buf.st_mode))
        ret = rmdir(path);
    else
        ret = unlink(path);

    if (ret)
        err = X__GetError();
x__exit:
#else
    if (remove(path) != 0)
        err = X__GetError();
#endif

    return err;
}


XError xposixfs_rename(XPosixFs* fs, const char* oldpath, const char* newpath)
{
    X_ASSERT(fs);
    X_ASSERT(oldpath);
    X_ASSERT(newpath);
    X__ASSERT_TAG(fs);

    XError err = X_ERR_NONE;
    if (rename(oldpath, newpath) != 0)
        err = X__GetError();

    return err;
}


XError xposixfs_stat(XPosixFs* fs, const char* path, XStat* statbuf)
{
    X_ASSERT(fs);
    X_ASSERT(statbuf);
    X_ASSERT(path);
    X__ASSERT_TAG(fs);

    XError err = X_ERR_NONE;

    struct stat buf;
    if (stat(path, &buf) != 0)
    {
        err = X__GetError();
        goto x__exit;
    }

    statbuf->size = buf.st_size;
    statbuf->mtime = buf.st_mtime;
    if (S_ISDIR(buf.st_mode))
        statbuf->mode = XSTAT_MODE_DIRECTORY;
    else
        statbuf->mode = XSTAT_MODE_REGULAR;

x__exit:
    return err;
}


XError xposixfs_utime(XPosixFs* fs, const char* path, XTime time)
{
    X_ASSERT(fs);
    X_ASSERT(path);
    X__ASSERT_TAG(fs);

    XError err = X_ERR_NONE;
    struct utimbuf times;
    times.actime = time;
    times.modtime = time;

    if (utime(path, &times) != 0)
        err = X__GetError();

    return err;
}


XError xposixfs_rmtree(XPosixFs* fs, const char* path)
{
    X_ASSERT(fs);
    X_ASSERT(path);
    X__ASSERT_TAG(fs);

    struct stat statbuf;
    char buf[X_PATH_MAX];
    strcpy(buf, path);
    const int tail = strlen(buf);

    XError err = X_ERR_NONE;
    errno = 0;
    if (!X__DoRmTree(buf, X_PATH_MAX, tail, &statbuf))
        err = X__GetError();

    return err;
}


static bool X__DoRmTree(char* path, int size, int tail, struct stat* statbuf)
{
    DIR  *dir = NULL;
    bool ok = false;
    int ret;

    do
    {
        X_BREAK_IF(stat(path, statbuf) != 0);

        if (S_ISDIR(statbuf->st_mode))
        {
            X_BREAK_IF((dir = opendir(path)) == NULL);
            for (;;)
            {
                struct dirent* ent;
                ent = readdir(dir);

                /* ディレクトリ要素の終端 or error */
                if (ent == NULL)
                {
                    if (errno == 0)
                        ok = true;
                    break;
                }

                if (x_strequal(".", ent->d_name) || x_strequal("..", ent->d_name))
                    continue;

                ret = snprintf(&path[tail], size - tail, "/%s", ent->d_name);

                X_BREAK_IF(ret == -1);

                /* snprintfはbufサイズに空きがなかった場合も、本来書き込めたであ
                 * ろう文字数を返す。 よって、bufサイズより戻り値が大きかった場
                 * 合、空きがが足りなかったことを意味する。
                 */
                if (ret > size - tail)
                {
                    errno = ENOMEM;
                    break;
                }

                /* 再帰呼び出し */
                X_BREAK_IF(! X__DoRmTree(path, size, tail + ret, statbuf));
            }

            ret = closedir(dir);
            X_BREAK_IF(! ok);

            ok = false;
            X_BREAK_IF(ret != 0);

            path[tail] = '\0';
            ret = rmdir(path);
            X_BREAK_IF(ret != 0);
        }
        else
        {
            ret = unlink(path);
            X_BREAK_IF(ret != 0);
        }

        ok = true;
    } while (0);

    return ok;
}


static XError X__GetError(void)
{
    XError err;
    switch (errno)
    {
        case EROFS:
        case EACCES:    err = X_ERR_ACCESS;         break;
        case EEXIST:    err = X_ERR_EXIST;          break;
        case EBUSY:     err = X_ERR_BUSY;           break;
        case EINVAL:    err = X_ERR_INVALID;        break;
        case EIO:       err = X_ERR_IO;             break;
        case ETIMEDOUT: err = X_ERR_TIMED_OUT;      break;
        case ENOMEM:    err = X_ERR_NO_MEMORY;      break;
        case ENFILE:
        case EMFILE:    err = X_ERR_MANY;           break;
        case ENOENT:    err = X_ERR_NO_ENTRY;       break;
        case ERANGE:    err = X_ERR_RANGE;          break;
        case ENOSPC:    err = X_ERR_NO_SPACE;       break;
        case ENOTDIR:   err = X_ERR_NOT_DIRECTORY;  break;
        case EISDIR:    err = X_ERR_IS_DIRECTORY;   break;
        case ENOTEMPTY: err = X_ERR_NOT_EMPTY;      break;
        default:        err = X_ERR_OTHER;          break;
    }
    return err;
}
