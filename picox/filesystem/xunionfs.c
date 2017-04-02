/**
 *       @file  xunionfs.h
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

#include <picox/filesystem/xunionfs.h>
#include <picox/container/xintrusive_list.h>


struct X__MountPoint;
typedef struct X__MountPoint X__MountPoint;

struct X__MountPoint
{
    X__MountPoint*  m_parent;
    XIntrusiveNode  m_node;
    XVirtualFs*     m_vfs;
    char*           m_vpath;
    char*           m_realpath;
};


typedef struct
{
    XIntrusiveList  m_mplist;
    X__MountPoint*  m_root;
    X__MountPoint*  m_curmp;
    char*           m_curdir;
} X__Fs;


typedef struct
{
    char            vpath[X_PATH_MAX];
    char            realpath[X_PATH_MAX];
    const char*     srcdir;
    const char*     dstdir;
    int             dstdirlen;
    int             srcdirlen;
    XStat           statbuf;
    XDirEnt         direntbuf;
    XDirEnt*        dirent;
    XFile*          srcfp;
    XFile*          dstfp;
    X__MountPoint*  mp;
    int             depth;
} X__CopyTreeWorkBuf;


typedef struct
{
    char        vpath[X_PATH_MAX];
    char        realpath[X_PATH_MAX];
    XStat       statbuf;
    XDirEnt     direntbuf;
    XDirEnt*    dirent;
    bool        allmatch;
} X__RmTreeWorkBuf;


typedef struct
{
    char            vpath[X_PATH_MAX];
    char            realpath[X_PATH_MAX];
    XStat           statbuf;
    XDirEnt         direntbuf;
    XDirEnt*        dirent;
    X__MountPoint*  mp;
    XFsTreeWalker   walker;
    void*           userptr;
} X__WalkTreeWorkBuf;


static XError X__ToRealPath(const X__MountPoint* mp, char* vpath);
static XError X__FindMountPoint(const char* absvpath,  X__MountPoint** o_mp, bool* allmatch);
static XError X__FindMountPoint2(char* workbuf, const char* path,
                                 X__MountPoint** o_mp, bool* allmatch);
static XError X__FindMountPoint3(char* workbuf, const char* path,
                                 X__MountPoint** o_mp, bool* allmatch);
static XError X__CreateMountPoint(XVirtualFs* vfs, const char* vpath,
                                  const char* realpath, X__MountPoint** o_mp);
static void X__DestroyMountPoint(X__MountPoint* mp);
static XError X__DoCopyTree(X__CopyTreeWorkBuf* work, int tail);
static XError X__DoRmTree(X__RmTreeWorkBuf* work, int tail);
static XError X__DoWalkTree(X__WalkTreeWorkBuf* work, int tail);
static XError X__DoStat(const char* path, XStat* statbuf, char* workbuf);


X__Fs  x_g_fs;
static X__Fs* const priv = &x_g_fs;


void xunionfs_init()
{
    xilist_init(&priv->m_mplist);
    priv->m_root = NULL;
    priv->m_curmp = NULL;
    priv->m_curdir = NULL;
}


void xunionfs_deinit()
{
    XIntrusiveNode* ite = xilist_front(&priv->m_mplist);
    XIntrusiveNode* const end = xilist_end(&priv->m_mplist);

    /* 全てのマウントポイントを削除する */
    while (ite != end)
    {
        X__MountPoint* const mp = xnode_entry(ite, X__MountPoint, m_node);
        ite = ite->next;
        X__DestroyMountPoint(mp);
    }

    x_free(priv->m_curdir);
    priv->m_root = NULL;
    priv->m_curmp = NULL;
    priv->m_curdir = NULL;
}


XError xunionfs_mount(XVirtualFs* vfs, const char* vpath, const char* realpath)
{
    X_ASSERT(vfs);
    X_ASSERT(vpath);
    X_ASSERT(realpath);

    XError err;
    char buf[X_PATH_MAX];
    X__MountPoint* mp = NULL;
    XIntrusiveNode* ite;
    X__MountPoint* parent;

    if (!priv->m_curdir)
    {
        /* はじめのマウントパスは必ず"/"でなければならない  */
        if (!x_strequal(vpath, "/"))
        {
            err = X_ERR_NO_ENTRY;
            goto x__exit;
        }
        strcpy(buf, "/");
    }
    else
    {
        /* マウント先のパスを絶対パスに変換しておく */
        X_ASSERT(priv->m_curdir);
        err = xfpath_resolve(buf, priv->m_curdir, vpath, X_PATH_MAX);
        if (err)
            goto x__exit;
    }

    /* 同じパスがすでにマウントされていたらエラー */
    xilist_foreach(&priv->m_mplist, ite)
    {
        const X__MountPoint* const mp = xnode_entry(ite, const X__MountPoint, m_node);
        if (x_strequal(mp->m_vpath, buf))
        {
            err = X_ERR_EXIST;
            goto x__exit;
        }
    }

    /* "/"以外のマウントパスは、ディレクトリとして実体が存在している必要がある */
    parent = NULL;
    if (!x_strequal(buf, "/"))
    {
        XStat statbuf;
        err = X__FindMountPoint3(buf, vpath, &parent, NULL);
        if (err)
            goto x__exit;
        err = xvfs_stat(parent->m_vfs, buf, &statbuf);
        if (err)
            goto x__exit;
        if (!XSTAT_IS_DIRECTORY(statbuf.mode))
        {
            err = X_ERR_NOT_DIRECTORY;
            goto x__exit;
        }

        /* statを呼び出す過程でbufは破壊されているので元に戻す */
        xfpath_resolve(buf, priv->m_curdir, vpath, X_PATH_MAX);
    }

    /* マウントポイントオブジェクトを生成してリストに繋ぎ込む  */
    err = X__CreateMountPoint(vfs, buf, realpath, &mp);
    if (err)
        goto x__exit;
    mp->m_parent = parent;
    xilist_push_back(&priv->m_mplist, &mp->m_node);

    /* 初回のマウント時はいろいろセットアップが必要だ */
    if (!priv->m_curdir)
    {
        priv->m_curdir = x_strdup("/");
        if (!priv->m_curdir)
        {
            err = X_ERR_NO_MEMORY;
            goto x__exit;
        }

        priv->m_root = mp;
        priv->m_curmp = mp;
    }
    mp = NULL;

x__exit:
    X__DestroyMountPoint(mp);

    return err;
}


XError xunionfs_umount(const char* path)
{
    X_ASSERT(path);

    XError err = X_ERR_NONE;
    char buf[X_PATH_MAX];
    const X__MountPoint* found_mp = NULL;
    size_t len;
    XIntrusiveNode* ite;

    err = xfpath_resolve(buf, priv->m_curdir, path, X_PATH_MAX);
    if (err)
        goto x__exit;

    /* カレントディレクトリはアンマウント不可 */
    if (x_strequal(buf, priv->m_curdir))
    {
        err = X_ERR_BUSY;
        goto x__exit;
    }

    len = strlen(buf);
    xilist_foreach(&priv->m_mplist, ite)
    {
        const X__MountPoint* mp = xnode_entry(ite, const X__MountPoint, m_node);

        if (x_strcountcaseequal(mp->m_vpath, path) != len)
            continue;

        /* アンマウントしたいパスの下位にマウントポイントが存在していたら、アン
         * マウントを拒否する
         */
        if (mp->m_vpath[len] != '\0')
        {
            err = X_ERR_BUSY;
            goto x__exit;
        }
        else
        {
            found_mp = mp;
            break;
        }
    }

    if (!found_mp)
    {
        err = X_ERR_NO_ENTRY;
        goto x__exit;
    }

    X__DestroyMountPoint((X__MountPoint*)found_mp);

x__exit:

    return err;
}


XError xunionfs_open(const char* path, XOpenMode mode, XFile** o_fp)
{
    XError err;
    char buf[X_PATH_MAX];
    X__MountPoint* mp;

    err = X__FindMountPoint3(buf, path, &mp, NULL);
    if (err)
        goto x__exit;
    err = xvfs_open(mp->m_vfs, buf, mode, o_fp);

x__exit:
    return err;
}


XError xunionfs_mkdir(const char* path)
{
    XError err;
    char buf[X_PATH_MAX];
    X__MountPoint* mp;

    err = X__FindMountPoint3(buf, path, &mp, NULL);
    if (err)
        goto x__exit;
    err = xvfs_mkdir(mp->m_vfs, buf);

x__exit:
    return err;
}


XError xunionfs_opendir(const char* path, XDir** o_dir)
{
    XError err;
    char buf[X_PATH_MAX];
    X__MountPoint* mp;

    err = X__FindMountPoint3(buf, path, &mp, NULL);
    if (err)
        goto x__exit;
    err = xvfs_opendir(mp->m_vfs, buf, o_dir);

x__exit:
    return err;
}


XError xunionfs_chdir(const char* path)
{
    XError err;
    char buf[X_PATH_MAX];
    XStat statbuf;
    X__MountPoint* mp;
    char* curdir = NULL;

    err = X__FindMountPoint2(buf, path, &mp, NULL);
    if (err)
        goto x__exit;

    curdir = x_strdup(buf);
    if (!curdir)
    {
        err = X_ERR_NO_MEMORY;
        goto x__exit;
    }

    err = X__ToRealPath(mp, buf);
    if (err)
        goto x__exit;

    err = xvfs_stat(mp->m_vfs, buf, &statbuf);
    if (err)
        goto x__exit;

    if (!XSTAT_IS_DIRECTORY(statbuf.mode))
    {
        err = X_ERR_NOT_DIRECTORY;
        goto x__exit;
    }

    x_free(priv->m_curdir);
    priv->m_curmp = mp;
    priv->m_curdir = curdir;
    curdir = NULL;

x__exit:
    x_free(curdir);

    return err;
}


XError xunionfs_getcwd(char* buf, size_t size)
{
    const size_t len = strlen(priv->m_curdir);
    if (len >= size)
        return X_ERR_RANGE;

    memcpy(buf, priv->m_curdir, len);
    buf[len] = '\0';
    return X_ERR_NONE;
}


XError xunionfs_remove(const char* path)
{
    XError err;
    char buf[X_PATH_MAX];
    X__MountPoint* mp;
    bool allmatch;

    err = X__FindMountPoint2(buf, path, &mp, &allmatch);
    if (err)
        goto x__exit;

    /* マウントパスはアンマウントされるまで削除不可 */
    if (allmatch)
    {
        err = X_ERR_BUSY;
        goto x__exit;
    }

    err = X__ToRealPath(mp, buf);
    if (err)
        goto x__exit;

    err = xvfs_remove(mp->m_vfs, buf);
x__exit:

    return err;
}


XError xunionfs_rename(const char* oldpath, const char* newpath)
{
    char oldbuf[X_PATH_MAX];
    char newbuf[X_PATH_MAX];
    X__MountPoint* oldmp;
    X__MountPoint* newmp;
    XStat statbuf;
    bool allmatch;
    XError err;

    err = X__FindMountPoint3(oldbuf, oldpath, &oldmp, &allmatch);
    if (err)
        goto x__exit;
    if (allmatch)
    {
        /* マウントポイントはリネーム不可 */
        err = X_ERR_BUSY;
        goto x__exit;
    }

    err = X__FindMountPoint3(newbuf, newpath, &newmp, NULL);
    if (err)
        goto x__exit;

    if (oldmp == newmp)
    {
        /* マウント元が同じなら直接リネームできる */
        err = xvfs_rename(oldmp->m_vfs, oldbuf, newbuf);
    }
    else
    {
        /* マウント元が異なる場合はコピーしてやる必要がある */

        err = xvfs_stat(oldmp->m_vfs, oldbuf, &statbuf);
        if (err)
            goto x__exit;
        if (XSTAT_IS_DIRECTORY(statbuf.mode))
        {
            err = xunionfs_copytree(oldpath, newpath);
            if (err)
                goto x__exit;
            err = xunionfs_rmtree(oldpath);
        }
        else
        {
            XFile* oldfp = NULL;
            XFile* newfp = NULL;

            err = xvfs_open(oldmp->m_vfs, oldbuf, X_OPEN_MODE_READ, &oldfp);
            if (err)
                goto x__cleanup;

            err = xvfs_open(newmp->m_vfs, newbuf, X_OPEN_MODE_WRITE, &newfp);
            if (err)
                goto x__cleanup;

            err = xunionfs_copyfile2(oldfp, newfp);
            if (err)
                goto x__cleanup;

            err = xvfs_remove(oldmp->m_vfs, oldbuf);
x__cleanup:
            xvfs_close(oldfp);
            xvfs_close(newfp);
        }
    }

x__exit:
    return err;
}


XError xunionfs_stat(const char* path, XStat* statbuf)
{
    char buf[X_PATH_MAX];
    return X__DoStat(path, statbuf, buf);
}


XError xunionfs_utime(const char* path, XTime time)
{
    XError err;
    char buf[X_PATH_MAX];
    X__MountPoint* mp;

    if ((err = X__FindMountPoint3(buf, path, &mp, NULL)) != X_ERR_NONE)
        return err;

    err = xvfs_utime(mp->m_vfs, buf, time);
    return err;
}


XError xunionfs_copyfile(const char* src, const char* dst)
{
    XError err;
    XFile* sfp = NULL;
    XFile* dfp = NULL;

    err = xunionfs_open(src, X_OPEN_MODE_READ, &sfp);
    if (err)
        goto x__exit;

    err = xunionfs_open(dst, X_OPEN_MODE_WRITE, &dfp);
    if (err)
        goto x__exit;

    err = xunionfs_copyfile2(sfp, dfp);

x__exit:
    xvfs_close(sfp);
    xvfs_close(dfp);

    return err;
}


XError xunionfs_copytree(const char* src, const char* dst)
{
    X_ASSERT(dst);
    X_ASSERT(src);

    X__CopyTreeWorkBuf* work = x_malloc(sizeof(X__CopyTreeWorkBuf));
    if (!work)
        return X_ERR_NO_MEMORY;

    XError err = X_ERR_NONE;
    memset(work, 0, sizeof(*work));
    work->srcdir = src;
    work->dstdir = dst;
    work->srcdirlen = strlen(src);
    work->dstdirlen = strlen(dst);
    work->depth = 0;
    x_strlcpy(work->vpath, src, X_PATH_MAX);

    /* srcはディレクトリでなければならない */
    err = X__FindMountPoint3(work->realpath, src, &work->mp, NULL);
    if (err)
        goto x__exit;
    err = xvfs_stat(work->mp->m_vfs, work->realpath, &work->statbuf);
    if (err)
        goto x__exit;
    if (!XSTAT_IS_DIRECTORY(work->statbuf.mode))
    {
        err = X_ERR_NOT_DIRECTORY;
        goto x__exit;
    }

    /* dstはディレクトリとして作成可能でなければならない */
    err = X__FindMountPoint3(work->realpath, dst, &work->mp, NULL);
    if (err)
        goto x__exit;
    err = xvfs_mkdir(work->mp->m_vfs, work->realpath);
    if (err)
        goto x__exit;

    /* コピー処理本体 */
    err = X__DoCopyTree(work, strlen(work->vpath));

x__exit:
    if (work)
        x_free(work);

    return err;
}


XError xunionfs_rmtree(const char* path)
{
    X_ASSERT(path);

    XError err = X_ERR_NONE;

    X__RmTreeWorkBuf* work = x_malloc(sizeof(X__RmTreeWorkBuf));
    if (!work)
    {
        err = X_ERR_NO_MEMORY;
        goto x__exit;
    }

    x_strlcpy(work->vpath, path, X_PATH_MAX);
    err = X__DoRmTree(work, strlen(work->vpath));

x__exit:
    x_free(work);

    return err;
}


XError xunionfs_makedirs(const char* path, bool exist_ok)
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

        err = xunionfs_mkdir(buf);
        if ((err != X_ERR_NONE) && (err != X_ERR_EXIST))
            goto x__exit;
    }

    if ((err == X_ERR_EXIST) && exist_ok)
        err = X_ERR_NONE;

x__exit:
    return err;
}


XError xunionfs_walktree(const char* path, XFsTreeWalker walker, void* userptr)
{
    X_ASSERT(path);
    X_ASSERT(walker);

    X__WalkTreeWorkBuf* work = x_malloc(sizeof(X__WalkTreeWorkBuf));
    char* tmp;
    char* endptr;

    if (!work)
        return X_ERR_NO_MEMORY;

    XError err = X_ERR_NONE;
    memset(work, 0, sizeof(*work));
    work->walker = walker;
    work->userptr = userptr;
    work->dirent = &work->direntbuf;

    x_strlcpy(work->vpath, path, X_PATH_MAX);

    err = X__DoStat(work->realpath, &work->statbuf, work->realpath);
    if (err)
        goto x__exit;
    if (!XSTAT_IS_DIRECTORY(work->statbuf.mode))
    {
        err = X_ERR_NOT_DIRECTORY;
        goto x__exit;
    }

    tmp = work->realpath;
    xfpath_resolve(tmp, priv->m_curdir, path, X_PATH_MAX);

    if (xfpath_is_root(tmp))
        strcpy(work->dirent->name, tmp);
    else
    {
        tmp = xfpath_name(tmp, &endptr);
        memcpy(work->dirent->name, tmp, endptr - tmp);
        work->dirent->name[(size_t)(endptr - tmp)] = '\0';
    }

    err = X__DoWalkTree(work, strlen(work->vpath));

x__exit:
    if (work)
        x_free(work);

    return err;
}


XError xunionfs_exists(const char* path, bool* exists)
{
    X_ASSERT_NOT_NULL(exists);
    XStat statbuf;
    const XError err = xunionfs_stat(path, &statbuf);
    *exists = (err == X_ERR_NONE);
    return err;
}


XError xunionfs_is_directory(const char* path, bool* isdir)
{
    X_ASSERT_NOT_NULL(isdir);
    XStat statbuf;
    const XError err = xunionfs_stat(path, &statbuf);
    *isdir = ((err == X_ERR_NONE) && (XSTAT_IS_DIRECTORY(statbuf.mode)));
    return err;
}


XError xunionfs_is_regular(const char* path, bool* isreg)
{
    X_ASSERT_NOT_NULL(isreg);
    XStat statbuf;
    const XError err = xunionfs_stat(path, &statbuf);
    *isreg = ((err == X_ERR_NONE) && (XSTAT_IS_REGULAR(statbuf.mode)));
    return err;
}


static XError X__ToRealPath(const X__MountPoint* mp, char* vpath)
{
    const size_t vl = strlen(mp->m_vpath);
    const size_t rl = strlen(mp->m_realpath);
    const size_t l = strlen(vpath);
    const bool is_vroot = xfpath_is_root(mp->m_vpath);
    const bool is_rroot = xfpath_is_root(mp->m_realpath);
    char* p;

    /* xunionfsの仮想ファイルパスを、本当のファイルパスに変換したい。基本的には、
     * vpath中の、mp->m_vpath部分を、mp->m_realpathに置き換えればいいのだが、
     * いくつか例外があり、わかりづらい。
     *
     * (1)
     * vpath "/foo/bar.txt"  mp->m_vpath "/" mp->m_realpath "/baz"
     * これを単純に置き換えてしまうと、
     *
     * "/bazfoo/bar.txt"になってしまう。 "/baz/foo/bar.txt"にしたい。
     * vl - 1とすると、 "/"の前に、mp->m_realpathが挿入される形になるので辻褄が
     * あう。
     *
     * (2)
     * vpath "/foo/bar.txt"  mp->m_vpath "/foo" mp->m_realpath "/"
     * これを単純に置き換えてしまうと、
     * "//bar.txt"になる。 連続するセパレータは認められるはずなので、まあこのま
     * までもいいのだが、綺麗に"/bar.txt"としたい。
     * vl + 1とすると、vpathのセパレータ部分も含めてmp->m_realpathに上書きされる
     * ので辻褄が合う。
     *
     * (3)
     * それ以外は普通に置き換えればよし。
     */
    if (is_vroot && !is_rroot) /* (1) */
        p = x_strreplace(vpath, X_PATH_MAX, l, vl - 1, mp->m_realpath, rl);
    else if ((!is_vroot) && (is_rroot) && (vl != l)) /* (2) */
        p = x_strreplace(vpath, X_PATH_MAX, l, vl + 1, mp->m_realpath, rl);
    else /* (3) */
        p = x_strreplace(vpath, X_PATH_MAX, l, vl, mp->m_realpath, rl);

    if (!p)
        return X_ERR_NAME_TOO_LONG;

    return X_ERR_NONE;
}


static XError X__FindMountPoint(const char* absvpath,  X__MountPoint** o_mp, bool* allmatch)
{
    /* マウントポイントを走査し、最もパスと長く一致するマウントポイントを対象と
     * する。
     */
    X__MountPoint* mp = NULL;
    size_t max_equal = 0;
    XIntrusiveNode* ite;
    xilist_foreach(&priv->m_mplist, ite)
    {
        X__MountPoint* const p = xnode_entry(ite, X__MountPoint, m_node);
        const size_t n = x_strcountcaseequal(absvpath, p->m_vpath);
        if (n > max_equal)
        {
            if (n != strlen(p->m_vpath))
                continue;
            max_equal = n;
            mp = p;
        }
    }

    if (! mp)
        return X_ERR_NO_ENTRY;

    *o_mp = mp;
    if (allmatch)
        *allmatch = x_strequal(absvpath, mp->m_vpath);

    return X_ERR_NONE;
}


static XError X__FindMountPoint2(char* workbuf, const char* path, X__MountPoint** o_mp, bool* allmatch)
{
    XError err;

    err = xfpath_resolve(workbuf, priv->m_curdir, path, X_PATH_MAX);
    if (err)
        goto x__exit;

    err = X__FindMountPoint(workbuf, o_mp, allmatch);

x__exit:
    return err;
}


static XError X__FindMountPoint3(char* workbuf, const char* path, X__MountPoint** o_mp, bool* allmatch)
{
    XError err;

    err = X__FindMountPoint2(workbuf, path, o_mp, allmatch);
    if (err)
        goto x__exit;

    err = X__ToRealPath(*o_mp, workbuf);

x__exit:
    return err;
}


static XError X__CreateMountPoint(XVirtualFs* vfs, const char* vpath,
                                  const char* realpath, X__MountPoint** o_mp)
{
    XError err = X_ERR_NONE;
    X__MountPoint* mp = NULL;
    char* s1 = NULL;
    char* s2 = NULL;
    char* s3 = NULL;

    *o_mp = NULL;
    mp = x_malloc(sizeof(X__MountPoint));
    if (!mp)
    {
        err = X_ERR_NO_MEMORY;
        goto x__exit;
    }

    s1 = x_strdup(vpath);
    if (!s1)
    {
        err = X_ERR_NO_MEMORY;
        goto x__exit;
    }

    s2 = x_malloc(X_PATH_MAX * 2);
    if (!s2)
    {
        err = X_ERR_NO_MEMORY;
        goto x__exit;
    }

    err = xvfs_getcwd(vfs, s2 + X_PATH_MAX, X_PATH_MAX);
    if (err)
        goto x__exit;
    err = xfpath_resolve(s2, s2 + X_PATH_MAX, realpath, X_PATH_MAX);
    if (err)
        goto x__exit;

    s3 = x_strdup(s2);
    if (!s3)
    {
        err = X_ERR_NO_MEMORY;
        goto x__exit;
    }

    mp->m_vfs = vfs;
    mp->m_vpath = s1;
    mp->m_realpath = s3;
    *o_mp = mp;
    mp = NULL;
    s1 = NULL;
    s3 = NULL;

x__exit:
    x_free(mp);
    x_free(s1);
    x_free(s2);
    x_free(s3);

    return err;
}


static void X__DestroyMountPoint(X__MountPoint* mp)
{
    if (mp)
    {
        xnode_unlink(&mp->m_node);
        x_free(mp->m_vpath);
        x_free(mp->m_realpath);
        x_free(mp);
    }
}


static XError X__DoStat(const char* path, XStat* statbuf, char* workbuf)
{
    XError err;
    X__MountPoint* mp;
    bool allmatch;

    err = X__FindMountPoint2(workbuf, path, &mp, &allmatch);
    if (err)
        goto x__exit;

    if (allmatch)
    {
        mp = mp->m_parent;
        if (!mp)
        {
            memset(statbuf, 0, sizeof(*statbuf));
            statbuf->mode = XSTAT_MODE_DIRECTORY;
            goto x__exit;
        }
    }

    err = X__ToRealPath(mp, workbuf);
    if (err)
        goto x__exit;

    err = xvfs_stat(mp->m_vfs, workbuf, statbuf);

x__exit:
    return err;
}


static XError X__DoCopyTree(X__CopyTreeWorkBuf* work, int tail)
{
    XError err = X_ERR_NONE;
    XDir* dir = NULL;
    int len;

    err = X__DoStat(work->vpath, &work->statbuf, work->realpath);
    if (err)
        goto x__exit;

    err = X__FindMountPoint3(work->realpath, work->vpath, &work->mp, NULL);
    if (err)
        goto x__exit;

    if (!XSTAT_IS_DIRECTORY(work->statbuf.mode))
    {
        /* ファイルコピー */
        work->srcfp = NULL;
        work->dstfp = NULL;
        do
        {
            err = xvfs_open(work->mp->m_vfs, work->realpath, X_OPEN_MODE_READ, &work->srcfp);
            if (err)
                break;

            if (!x_strreplace(work->vpath, X_PATH_MAX, strlen(work->vpath),
                              work->srcdirlen, work->dstdir, work->dstdirlen))
            {
                err = X_ERR_NAME_TOO_LONG;
                break;
            }

            err = X__FindMountPoint3(work->realpath, work->vpath, &work->mp, NULL);
            if (err)
                goto x__exit;

            err = xvfs_open(work->mp->m_vfs, work->realpath, X_OPEN_MODE_WRITE, &work->dstfp);
            if (err)
                break;

            err = xunionfs_copyfile2(work->srcfp, work->dstfp);
            if (err)
                break;

            err = xvfs_close(work->dstfp);
            work->dstfp = NULL;
            if (err)
                break;

            err = xvfs_close(work->srcfp);
            work->srcfp = NULL;
            if (err)
                break;
        } while (0);

        if (work->dstfp)
            xvfs_close(work->dstfp);
        if (work->srcfp)
            xvfs_close(work->srcfp);
    }
    else
    {
        /* ディレクトリコピー */
        err = xvfs_opendir(work->mp->m_vfs, work->realpath, &dir);
        if (err)
            goto x__exit;

        /* パスをdst側に置き換える */
        if (!x_strreplace(work->vpath, X_PATH_MAX, strlen(work->vpath),
                          work->srcdirlen, work->dstdir, work->dstdirlen))
        {
            err = X_ERR_NAME_TOO_LONG;
            goto x__exit;
        }


        err = X__FindMountPoint3(work->realpath, work->vpath, &work->mp, NULL);
        if (err)
            goto x__exit;

        /* 1階層目のディレクトリは作成済みであることが前提 */
        if (work->depth != 0)
        {
            err = xvfs_mkdir(work->mp->m_vfs, work->realpath);
            if (err)
                goto x__exit;
        }

        /* パスをsrc基準に戻す。成功するのは確定しているからエラーチェックは不要だ  */
        x_strreplace(work->vpath, X_PATH_MAX, strlen(work->vpath),
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

            len = x_snprintf(work->vpath + tail,
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
        dir = NULL;
        if (err)
            goto x__exit;
        work->vpath[tail] = '\0';
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
    X__MountPoint* mp = NULL;
    int len = 0;

    err = X__DoStat(work->vpath, &work->statbuf, work->realpath);
    if (err)
        goto x__exit;

    err = X__FindMountPoint3(work->realpath, work->vpath, &mp, &work->allmatch);
    if (err)
        goto x__exit;
    if (work->allmatch)
    {
        err = X_ERR_BUSY;
        goto x__exit;
    }

    if (!XSTAT_IS_DIRECTORY(work->statbuf.mode))
    {
        err = xvfs_remove(mp->m_vfs, work->realpath);
        if (err)
            goto x__exit;
    }
    else
    {
        err = xvfs_opendir(mp->m_vfs, work->realpath, &dir);
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

            len = x_snprintf(work->vpath + tail, X_PATH_MAX - tail,
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
        work->vpath[tail] = '\0';
        work->realpath[strlen(work->realpath) - len] = '\0';
        err = xvfs_remove(mp->m_vfs, work->realpath);
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

    err = X__DoStat(work->vpath, &work->statbuf, work->realpath);
    if (err)
        goto x__exit;

    err = X__FindMountPoint3(work->realpath, work->vpath, &work->mp, NULL);
    if (err)
        goto x__exit;

    /* コールバック呼び出し */
    if (!work->walker(work->userptr, work->vpath, &work->statbuf, work->dirent))
        goto x__exit;

    if (XSTAT_IS_DIRECTORY(work->statbuf.mode))
    {
        err = xvfs_opendir(work->mp->m_vfs, work->realpath, &dir);
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
                len = x_snprintf(work->vpath + tail,
                                 X_PATH_MAX - tail,
                                 "%s",
                                 work->dirent->name);
            }
            else
            {
                len = x_snprintf(work->vpath + tail,
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
        work->vpath[tail] = '\0';
    }

x__exit:
    if (dir)
        xvfs_closedir(dir);

    return err;
}
