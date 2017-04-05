/**
 *       @file  xromfs.c
 *      @brief  RAMファイルシステム実装
 *
 *    @details
 *
 *     @author  MaskedW
 *
 *   @internal
 *     Created  2016/05/03
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

#include <picox/filesystem/xromfs.h>


#define X__TYPE_DIR  (0)
#define X__TYPE_FILE (1)
#define X__ASSERT_TAG(p)        (X_ASSERT(((const XRomFs*)p)->m_fstype_tag == &XROMFS_RTTI_TAG))
#define X__GET_FILE_HANDLE(fp)  ((X__File*)fp)
#define X__GET_DIR_HANDLE(dir)  ((X__Dir*)dir)
#define X__EXIT_IF(cond, v)     X_ASSIGN_AND_GOTO_IF(cond, err, v, x__exit)

#define X__FLAGS                (0)
#define X__PARENT_OFFSET        (4)
#define X__SIBLING_OFFSET       (8)
#define X__NAME_OFFSET          (12)
#define X__TIMESTAMP            (16)
#define X__FIRST_CHILD_OFFSET   (20)
#define X__DATA_OFFSET          (20)
#define X__DATA_SIZE            (24)
#define X__INVALID              (UINT32_MAX)

#define X__LOAD_U16(ptr)                 X_LOAD_U16_LIT(ptr)
#define X__LOAD_U32(ptr)                 X_LOAD_U32_LIT(ptr)
#define X__GET_PTR_FROM_OFFSET(ptr)      (fs->m_top + X__LOAD_U32(ptr))
#define X__LOAD_U32_FROM_OFFSET(ptr)     X__LOAD_U32(X__GET_PTR_FROM_OFFSET(ptr))


/* [Memory layout]
 *
 * struct X__Entry
 * {
 *     uint32_t        m_flags;
 *     uint32_t        m_parent_offset;
 *     uint32_t        m_sibling_offset;
 *     uint32_t        m_name_offset;
 *     uint32_t        m_timestamp;
 * };
 *
 *
 *  struct X__DirEntry
 *  {
 *      X__Entry        m_entry;
 *      uint32_t        m_first_child_offset;
 *  };
 *
 *
 *  struct X__FileEntry
 *  {
 *      X__Entry        m_entry;
 *      uint32_t        m_data_offset;
 *      uint32_t        m_size;
 *  };
 */


typedef struct
{
    XFile           m_vfile;
    const char*     m_fileent;
    size_t          m_pos;
} X__File;


typedef struct
{
    XDir            m_vdir;
    const char*     m_dirent;
    uint32_t        m_iterator;
} X__Dir;



static XError X__FindEntry(const XRomFs* fs, const char* path, char const** o_ent);


static const XStreamVTable X__romfs_filestream_vtable = {
    .m_name = "XRomFsFileStream",
    .m_read_func = (XStreamReadFunc)xromfs_read,
    .m_close_func = (XStreamCloseFunc)xromfs_close,
    .m_seek_func = (XStreamSeekFunc)xromfs_seek,
    .m_tell_func = (XStreamTellFunc)xromfs_tell,
};

static const XVirtualFsVTable X__romfs_vfs_vtable = {
    .m_name = "XRomFs",
    .m_open_func        = (XVirtualFsOpenFunc)xromfs_open,
    .m_close_func       = (XVirtualFsCloseFunc)xromfs_close,
    .m_read_func        = (XVirtualFsReadFunc)xromfs_read,
    .m_seek_func        = (XVirtualFsSeekFunc)xromfs_seek,
    .m_tell_func        = (XVirtualFsTellFunc)xromfs_tell,
    .m_opendir_func     = (XVirtualFsOpendirFunc)xromfs_opendir,
    .m_readdir_func     = (XVirtualFsReaddirFunc)xromfs_readdir,
    .m_closedir_func    = (XVirtualFsClosedirFunc)xromfs_closedir,
    .m_chdir_func       = (XVirtualFsChdirFunc)xromfs_chdir,
    .m_getcwd_func      = (XVirtualFsGetcwdFunc)xromfs_getcwd,
    .m_stat_func        = (XVirtualFsStatFunc)xromfs_stat,
};
X_IMPL_RTTI_TAG(XROMFS_RTTI_TAG);


XError xromfs_init(XRomFs* fs, const void* romimage)
{
    X_ASSERT_NOT_NULL(fs);
    X_ASSERT_NOT_NULL(romimage);

    XError err = X_ERR_NONE;
    const char* rootdir;

    /* check magic number */
    if (memcmp(romimage, "ROMF", 4) != 0)
    {
        err = X_ERR_PROTOCOL;
        goto x__exit;
    }

    /* get root directory */
    fs->m_top = romimage;
    rootdir = fs->m_top + 4;

    if (X__LOAD_U32(rootdir + X__FLAGS)  != X__TYPE_DIR)
    {
        err = X_ERR_PROTOCOL;
        goto x__exit;
    }

    if (!x_strequal(X__GET_PTR_FROM_OFFSET(rootdir + X__NAME_OFFSET), "/"))
    {
        err = X_ERR_PROTOCOL;
        goto x__exit;
    }

    fs->m_fstype_tag = &XROMFS_RTTI_TAG;
    fs->m_rootdir = fs->m_curdir = rootdir;

x__exit:

    return err;
}


void xromfs_deinit(XRomFs* fs)
{
    X_ASSERT(fs);
    fs->m_rootdir = fs->m_curdir = NULL;
}


XVirtualFs* xromfs_init_vfs(XRomFs* fs, XVirtualFs* vfs)
{
    X_ASSERT(fs);
    X_ASSERT(vfs);

    xvfs_init(vfs);
    vfs->m_rtti_tag = &XROMFS_RTTI_TAG;
    vfs->m_driver = fs;
    vfs->m_vtable = &X__romfs_vfs_vtable;

    return vfs;
}


XStream* xromfs_init_stream(XStream* stream, XFile* fp)
{
    X_ASSERT(stream);
    X_ASSERT(fp);

    xstream_init(stream);
    stream->m_rtti_tag = &XFILE_STREAM_RTTI_TAG;
    stream->m_driver = fp;
    stream->m_vtable = &X__romfs_filestream_vtable;

    return stream;
}


XError xromfs_open(XRomFs* fs, const char* path, XOpenMode mode, XFile** o_fp)
{
    X_ASSERT(fs);
    X_ASSERT_NOT_NULL(o_fp);
    X_ASSERT_NOT_NULL(path);
    X__ASSERT_TAG(fs);

    XError err;
    const char* ent;
    X__File* infp;

    *o_fp = NULL;
    err = X__FindEntry(fs, path, &ent);
    if (err)
        goto x__exit;

    if (X__LOAD_U32(ent + X__FLAGS) == X__TYPE_DIR)
    {
        err = X_ERR_IS_DIRECTORY;
        goto x__exit;
    }

    if (mode != X_OPEN_MODE_READ)
    {
        err = X_ERR_ACCESS;
        goto x__exit;
    }

    infp = x_malloc(sizeof(X__File));
    if (!infp)
    {
        err = X_ERR_NO_MEMORY;
        goto x__exit;
    }

    infp->m_fileent = ent;
    infp->m_pos = 0;
    infp->m_vfile.m_fs = fs;
    infp->m_vfile.m_vfs = NULL;
    *o_fp = &(infp->m_vfile);

x__exit:

    return err;
}


XError xromfs_close(XFile* fp)
{
    if (!fp)
        return X_ERR_NONE;

    X__ASSERT_TAG(fp->m_fs);
    x_free(fp);
    return X_ERR_NONE;
}


XError xromfs_read(XFile* fp, void* dst, size_t size, size_t* nread)
{
    X_ASSERT(fp);
    X_ASSERT(dst);
    X__ASSERT_TAG(fp->m_fs);

    X__File* const infp = X__GET_FILE_HANDLE(fp);
    XRomFs* const fs = fp->m_fs;
    const char* const fileent = infp->m_fileent;

    const size_t fsize = X__LOAD_U32(fileent + X__DATA_SIZE);
    const char* const data = X__GET_PTR_FROM_OFFSET(fileent + X__DATA_OFFSET);

    /* シークでファイルサイズを超えた位置にポインタがある時は読み出せるデータが
     * ないと判断する。エラーを返してもいい気はするが、libcの挙動に合わせておく
     */
    if (infp->m_pos >= fsize)
    {
        X_ASSIGN_NOT_NULL(nread, 0);
        return X_ERR_NONE;
    }

    const size_t to_read = ((infp->m_pos + size) <= fsize)
                            ? size : (fsize - infp->m_pos);

    memcpy(dst, data + infp->m_pos, to_read);
    infp->m_pos += to_read;
    X_ASSIGN_NOT_NULL(nread, to_read);

    return X_ERR_NONE;
}


XError xromfs_seek(XFile* fp, XOffset pos, XSeekMode whence)
{
    X_ASSERT_NOT_NULL(fp);
    X__ASSERT_TAG(fp->m_fs);

    X__File* const infp = X__GET_FILE_HANDLE(fp);
    const char* const fileent = infp->m_fileent;
    const size_t fsize = X__LOAD_U32(fileent + X__DATA_SIZE);

    int64_t new_pos = -1;
    switch (whence)
    {
        case X_SEEK_SET:
            new_pos = pos;
            break;
        case X_SEEK_CUR:
            new_pos = ((int64_t)(infp->m_pos)) + pos;
            break;
        case X_SEEK_END:
            new_pos = ((int64_t)fsize) + pos;
            break;
        default:
            break;
    }

    if (new_pos < 0)
        return X_ERR_INVALID;

    if ((uint64_t)new_pos > SIZE_MAX)
        return X_ERR_INVALID;

    infp->m_pos = new_pos;
    return X_ERR_NONE;
}


XError xromfs_tell(XFile* fp, XSize* pos)
{
    X_ASSERT(fp);
    X_ASSERT_NOT_NULL(pos);
    X__ASSERT_TAG(fp->m_fs);

    X__File* const infp = X__GET_FILE_HANDLE(fp);
    *pos = infp->m_pos;

    return X_ERR_NONE;
}


XError xromfs_flush(XFile* fp)
{
    X_ASSERT(fp);
    X__ASSERT_TAG(fp->m_fs);
    X_UNUSED(fp);

    return X_ERR_NONE;
}


XError xromfs_opendir(XRomFs* fs, const char* path, XDir** o_dir)
{
    X_ASSERT(fs);
    X_ASSERT_NOT_NULL(o_dir);
    X_ASSERT_NOT_NULL(path);
    X__ASSERT_TAG(fs);

    XError err;
    const char* ent;
    X__Dir* indirp;

    *o_dir = NULL;

    err = X__FindEntry(fs, path, &ent);
    if (err)
        goto x__exit;

    if (X__LOAD_U32(ent + X__FLAGS) != X__TYPE_DIR)
    {
        err = X_ERR_NOT_DIRECTORY;
        goto x__exit;
    }

    indirp = x_malloc(sizeof(X__Dir));
    if (! indirp)
    {
        err = X_ERR_NO_MEMORY;
        goto x__exit;
    }

    indirp->m_vdir.m_fs = fs;
    indirp->m_vdir.m_vfs = NULL;
    indirp->m_dirent = ent;
    indirp->m_iterator = X__LOAD_U32(ent + X__FIRST_CHILD_OFFSET);
    *o_dir = &(indirp->m_vdir);

x__exit:

    return err;
}


XError xromfs_readdir(XDir* dir, XDirEnt* dirent, XDirEnt** result)
{
    X_ASSERT(dir);
    X_ASSERT_NOT_NULL(dirent);
    X_ASSERT_NOT_NULL(result);
    X__ASSERT_TAG(dir->m_fs);

    X__Dir* const indirp = X__GET_DIR_HANDLE(dir);
    XRomFs* const fs = dir->m_fs;

    if (indirp->m_iterator == X__INVALID)
        *result = NULL;
    else
    {
        const char* const p = fs->m_top + indirp->m_iterator;
        strcpy(dirent->name, X__GET_PTR_FROM_OFFSET(p + X__NAME_OFFSET));
        indirp->m_iterator = X__LOAD_U32(p + X__SIBLING_OFFSET);
        *result = dirent;
    }

    return X_ERR_NONE;
}


XError xromfs_closedir(XDir* dir)
{
    if (!dir)
        return X_ERR_NONE;

    X__ASSERT_TAG(dir->m_fs);
    x_free(dir);

    return X_ERR_NONE;
}


XError xromfs_chdir(XRomFs* fs, const char* path)
{
    X_ASSERT(fs);
    X_ASSERT_NOT_NULL(path);
    X__ASSERT_TAG(fs);

    XError err;
    const char* ent;

    err = X__FindEntry(fs, path, &ent);
    if (err != X_ERR_NONE)
        return err;

    if (X__LOAD_U32(ent + X__FLAGS) != X__TYPE_DIR)
        return X_ERR_NOT_DIRECTORY;

    fs->m_curdir = ent;

    return X_ERR_NONE;
}


XError xromfs_getcwd(XRomFs* fs, char* buf, size_t size)
{
    X_ASSERT(fs);
    X_ASSERT_NOT_NULL(buf);
    X__ASSERT_TAG(fs);
    if (size == 0)
        return X_ERR_INVALID;

    /* カレントディレクトリから上方向に向かって、名前を逆方向にコピーしていき、最後に文
     * 字列を逆転させることで辻褄を合わせる。
     *
     * カレントディレクトリパスを保存していればこんな面倒なことはしなくていいの
     * だが、とりあえずメモリ節約のための実装をしている。
     */
    const char* dir = fs->m_curdir;
    char* p = buf;
    for (;;)
    {
        const char* const name = X__GET_PTR_FROM_OFFSET(dir + X__NAME_OFFSET);
        const size_t len = strlen(name);
        if (len >= size - (p - buf))
            return X_ERR_RANGE;

        p = x_stprcpy(p, name);
        if (dir == fs->m_curdir)
            break;

        if (dir != fs->m_rootdir)
        {
            if (1 >= size - (p - buf))
                return X_ERR_RANGE;
            *p++ = '/';
        }
        dir = X__GET_PTR_FROM_OFFSET(dir + X__PARENT_OFFSET);
    }
    x_strreverse(buf);

    return X_ERR_NONE;
}


XError xromfs_stat(XRomFs* fs, const char* path, XStat* statbuf)
{
    X_ASSERT(fs);
    X_ASSERT_NOT_NULL(statbuf);
    X__ASSERT_TAG(fs);

    XError err;
    const char* ent;

    err = X__FindEntry(fs, path, &ent);
    if (err != X_ERR_NONE)
        return err;

    statbuf->mtime = X__LOAD_U32(ent + X__TIMESTAMP);

    if (X__LOAD_U32(ent + X__FLAGS) != X__TYPE_DIR)
    {
        statbuf->size = 0;
        statbuf->mode = XSTAT_MODE_DIRECTORY;
    }
    else
    {
        statbuf->size = X__LOAD_U32(ent + X__DATA_SIZE);
        statbuf->mode = XSTAT_MODE_REGULAR;
    }

    return X_ERR_NONE;
}


static XError X__FindEntry(const XRomFs* fs, const char* path, char const** o_ent)
{
    XError err = X_ERR_NONE;
    const char* dir = (path[0] == '/')  ? fs->m_rootdir : fs->m_curdir;
    const char* ent = NULL;
    const char* next;
    const char* endptr = path;
    char name[X_NAME_MAX];

    if (path[0] == '\0')
    {
        err = X_ERR_INVALID_NAME;
        goto x__exit;
    }

    if ((path[0] == '/') && (path[1] == '\0'))
    {
        ent = dir;
        goto x__exit;
    }

    for (;;)
    {
        ent = NULL;
        next = xfpath_top(endptr, (char**)&endptr);
        if (!next)
        {
            err = X_ERR_NO_ENTRY;
            goto x__exit;
        }


        if (endptr - next >= X_NAME_MAX)
        {
            err = X_ERR_NAME_TOO_LONG;
            goto x__exit;
        }

        memcpy(name, next, endptr - next);
        name[(size_t)(endptr - next)] = '\0';

        if (x_strequal(name, "."))
        {
            continue;
        }

        if (x_strequal(name, ".."))
        {
            /* ルートディレクトリを越えて遡ることはできない。ルートを越えたらエ
             * ラーにしてもいい気がするけど、他のシステムの挙動にならっておく。
             */
            if (dir != fs->m_rootdir)
                dir = X__GET_PTR_FROM_OFFSET(dir + X__PARENT_OFFSET);
            continue;
        }

        /* ":"は別のファイルシステムがドライブレターで使用する可能性があるので、
         * 禁止。"\"もトラブルの元になりそうなので禁止。
         */
        if (strpbrk(name, ":\\"))
        {
            err = X_ERR_INVALID_NAME;
            goto x__exit;
        }

        /* 現在のディレクトリから要素を探す */
        uint32_t ite = X__LOAD_U32(dir + X__FIRST_CHILD_OFFSET);
        while (ite != X__INVALID)
        {
            const char* p = fs->m_top + ite;
            if (x_strequal(name, X__GET_PTR_FROM_OFFSET(p + X__NAME_OFFSET)))
            {
                ent = p;
                break;
            }
            ite = X__LOAD_U32(p + X__SIBLING_OFFSET);
        }

        if (!ent)
        {
            err = X_ERR_NO_ENTRY;
            goto x__exit;
        }

        endptr = x_strskipchr(endptr, '/');
        if (*endptr != '\0')
        {
            /* 次の要素があるなら、エントリはディレクトリでなければおかしい */
            if (X__LOAD_U32(ent + X__FLAGS) != X__TYPE_DIR)
            {
                err = X_ERR_NO_ENTRY;
                goto x__exit;
            }

            dir = ent;
            continue;
        }

        /* 最終要素まで見つかった */
        break;
    }

x__exit:
    *o_ent = ent;

    return err;
}
