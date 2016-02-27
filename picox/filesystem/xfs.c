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

#include <picox/filesystem/xfs.h>
#include <picox/container/xintrusive_list.h>


typedef struct X__MountPoint
{
    XIntrusiveNode  m_node;
    XVirtualFs*     m_vfs;
    char*           m_path;
} X__MountPoint;


typedef struct X__Fs
{
    XIntrusiveList  m_mountpoints;
    char*           m_currentdir;
    bool            m_inited;
} X__Fs;


X__Fs  x_g_fs;
static X__Fs* const priv = &x_g_fs;


static void X__DestroyMountPoint(X__MountPoint* mp)
{
    xnode_unlink(&mp->m_node);
    x_free(mp->m_path);
    x_free(mp);
}


static X__MountPoint* X__CreateMountPoint(XVirtualFs* vfs, const char* path)
{
    X__MountPoint* mp;
    if (! (mp = x_malloc(sizeof(X__MountPoint))))
        return NULL;

    if (! (mp->m_path = x_strdup(path)))
    {
        x_free(mp);
        return  NULL;
    }

    mp->m_vfs = vfs;
    return mp;
}


static const char* X__AbsolutePath(char* buf, const char* currentdir, const char* path)
{
    size_t len;
    char* p = buf;

    *p = '\0';

    if (path[0] == '/')
    {
        strcpy(p, path);
    }
    else
    {
        if (currentdir)
        {
            len = strlen(currentdir);
            memcpy(p, currentdir, len);
            p += len;
        }

        len = strlen(path);
        memcpy(p, "/", 1);
        p += 1;

        memcpy(p, path, len);
        p[len] = '\0';
    }

    return buf;
}


static const char* X__ResolvePath(const char* abspath, XVirtualFs** dst_vfs)
{
    /* マウントポイントの中から最も長く一致するパスをベースパスとする。
     */
    size_t max_equal = 0;
    XIntrusiveNode* ite;
    xilist_foreach(&priv->m_mountpoints, ite)
    {
        const X__MountPoint* const p = xnode_entry(ite, const X__MountPoint, m_node);
        const size_t n = x_strcountequal(abspath, p->m_path);
        if (n > max_equal)
        {
            max_equal = n;
            *dst_vfs = p->m_vfs;
        }
    }

    /* 絶対パスからマウントポイントパス部分と'/'を取り除いたものがvfsに対応する
     * パスとなる。vfsに対応するパスとなる。
     */
    const char* rpath = abspath;
    if (max_equal == 1) /* "/" */
    {
        rpath += 1;
    }
    else
    {
        rpath += max_equal + 1;
    }
    return rpath;
}


XError xfs_mount(XVirtualFs* vfs, const char* path)
{
    X_ASSERT(vfs);
    X_ASSERT(path);

    /* 初回限定の処理 */
    if (! priv->m_inited)
    {
        /* マウントポイントリストを初期化する */
        xilist_init(&priv->m_mountpoints);
        priv->m_inited = true;
    }

    /* 絶対パスを取得する */
    char buf[X_CONFIG_FILE_PATH_MAX + 1];
    const char* const abspath = X__AbsolutePath(buf, priv->m_currentdir, path);

    XError err = X_ERR_NONE;
    do
    {
        /* 同じパスがマウントされていないかチェックする */
        XIntrusiveNode* ite;
        xilist_foreach(&priv->m_mountpoints, ite)
        {
            const X__MountPoint* const mp = xnode_entry(ite, const X__MountPoint, m_node);
            if (x_strequal(mp->m_path, abspath))
            {
                err = X_ERR_EXIST;
                break;
            }
        }
        X_BREAK_IF(err != X_ERR_NONE);

        /* "/"以外の場合は指定パスがディレクトリとして存在していることがマウントの条
         * 件となる。指定パス配下にファイルが存在していた場合、そのファイルは不可視
         * になることに注意。
         */
        X__MountPoint* mp;
        if (x_strequal(path, "/"))
        {
             mp = X__CreateMountPoint(vfs, path);
             if (! mp)
             {
                 err = X_ERR_NO_MEMORY;
                 break;
             }
        }
        else
        {
            /* [TODO]
             * 存在チェック。ディレクトリチェック
             */
            XVirtualFs* vfs;
            const char* const rpath = X__ResolvePath(abspath, &vfs);
            mp = X__CreateMountPoint(vfs, rpath);
            if (! mp)
            {
                err = X_ERR_NO_MEMORY;
                break;
            }
        }

        xilist_push_back(&priv->m_mountpoints, &mp->m_node);

    } while (0);

    return err;
}


XError xfs_umount(const char* path)
{
    X_ASSERT(path);

    XError err = X_ERR_NO_ENTRY;
    const size_t len = strlen(path);
    const X__MountPoint* found_mp = NULL;
    XIntrusiveNode* ite;

    xilist_foreach(&priv->m_mountpoints, ite)
    {
        const X__MountPoint* mp = xnode_entry(ite, const X__MountPoint, m_node);

        if (x_strcountequal(mp->m_path, path) != len)
            continue;

        /* アンマウントしたいパスの下位にマウントポイントが存在していたら、アン
         * マウントを拒否する
         */
        if (mp->m_path[len] != '\0')
        {
            err = X_ERR_BUSY;
            break;
        }
        else
        {
            found_mp = mp;
            err = X_ERR_NONE;
        }
    }

    if (err == X_ERR_NONE)
        X__DestroyMountPoint((X__MountPoint*)found_mp);


    return err;
}


XError xfs_open(XFile* fp, const char* path, const char* mode)
{
    XVirtualFs* vfs = NULL;
    char buf[X_CONFIG_FILE_PATH_MAX + 1];
    const char* const abspath = X__AbsolutePath(buf, priv->m_currentdir, path);
    const char* const p =  X__ResolvePath(abspath, &vfs);

    XError err = xvfs_open(vfs, fp, p, mode);
    return err;
}


XError xfs_close(XFile* fp)
{
    return xvfs_close(fp);
}


XError xfs_write(XFile* fp, const void* src, size_t size, size_t* nwritten)
{
    return xvfs_write(fp, src, size, nwritten);
}


XError xfs_read(XFile* fp, void* dst, size_t size, size_t* nread)
{
    return xvfs_read(fp, dst, size, nread);
}
