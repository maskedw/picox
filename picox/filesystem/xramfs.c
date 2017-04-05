/**
 *       @file  xramfs.c
 *      @brief  RAMファイルシステム実装
 *
 *    @details
 *
 *     @author  MaskedW
 *
 *   @internal
 *     Created  2016/02/20
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

#include <picox/filesystem/xramfs.h>
#include <picox/container/xbyte_array.h>
#include <picox/container/xintrusive_list.h>


#define X__TYPE_DIR  (0)
#define X__TYPE_FILE (1)
#define X__ASSERT_TAG(p)        (X_ASSERT(((const XRamFs*)p)->m_fstype_tag == &XRAMFS_RTTI_TAG))
#define X__GET_FILE_HANDLE(fp)  ((X__File*)fp)
#define X__GET_DIR_HANDLE(dir)  ((X__Dir*)dir)
#define X__EXIT_IF(cond, v)     X_ASSIGN_AND_GOTO_IF(cond, err, v, x__exit)


struct X__Entry;
struct X__DirEntry;
struct X__FileEntry;
typedef struct X__Entry X__Entry;
typedef struct X__DirEntry X__DirEntry;
typedef struct X__FileEntry X__FileEntry;


struct X__Entry
{
    X__DirEntry*    m_parent;
    XIntrusiveNode  m_node;
    uint8_t         m_type;
    XTime           m_timestamp;
    char*           m_name;
};


struct X__DirEntry
{
    X__Entry        m_entry;
    XIntrusiveList  m_children;
};


struct X__FileEntry
{
    X__Entry        m_entry;
    uint8_t*        m_data;
    size_t          m_size;
    size_t          m_capacity;
};


typedef struct
{
    XFile           m_vfile;
    X__FileEntry*   m_fileent;
    size_t          m_pos;
    XOpenMode       m_mode;
} X__File;


typedef struct
{
    XDir            m_vdir;
    X__DirEntry*    m_dirent;
    XIntrusiveNode* m_iterator;
} X__Dir;


static X__DirEntry* X__CreateDir(XRamFs* fs, X__DirEntry* parent, const char* name);
static X__FileEntry* X__CreateFile(XRamFs* fs, X__DirEntry* parent, const char* name);
static void X__DestoryEntry(XRamFs* fs, X__Entry* ent);
static char* X__Strdup(XRamFs* fs, const char* src);
static void* X__Malloc(XRamFs* fs, size_t size);
static void* X__Realloc(XRamFs* fs, void* old, size_t size);
static void X__Free(XRamFs* fs, void* ptr);
static XError X__FindEntry(const XRamFs* fs, const char* path, char* name,
                           X__Entry** o_ent, X__DirEntry** o_parent);


static const XStreamVTable X__ramfs_filestream_vtable = {
    .m_name = "XRamFsFileStream",
    .m_read_func = (XStreamReadFunc)xramfs_read,
    .m_write_func = (XStreamWriteFunc)xramfs_write,
    .m_close_func = (XStreamCloseFunc)xramfs_close,
    .m_flush_func = (XStreamFlushFunc)xramfs_flush,
    .m_seek_func = (XStreamSeekFunc)xramfs_seek,
    .m_tell_func = (XStreamTellFunc)xramfs_tell,
};

static const XVirtualFsVTable X__ramfs_vfs_vtable = {
    .m_name = "XRamFs",
    .m_open_func        = (XVirtualFsOpenFunc)xramfs_open,
    .m_close_func       = (XVirtualFsCloseFunc)xramfs_close,
    .m_read_func        = (XVirtualFsReadFunc)xramfs_read,
    .m_write_func       = (XVirtualFsWriteFunc)xramfs_write,
    .m_seek_func        = (XVirtualFsSeekFunc)xramfs_seek,
    .m_tell_func        = (XVirtualFsTellFunc)xramfs_tell,
    .m_flush_func       = (XVirtualFsFlushFunc)xramfs_flush,
    .m_mkdir_func       = (XVirtualFsMkdirFunc)xramfs_mkdir,
    .m_opendir_func     = (XVirtualFsOpendirFunc)xramfs_opendir,
    .m_readdir_func     = (XVirtualFsReaddirFunc)xramfs_readdir,
    .m_closedir_func    = (XVirtualFsClosedirFunc)xramfs_closedir,
    .m_chdir_func       = (XVirtualFsChdirFunc)xramfs_chdir,
    .m_getcwd_func      = (XVirtualFsGetcwdFunc)xramfs_getcwd,
    .m_remove_func      = (XVirtualFsRemoveFunc)xramfs_remove,
    .m_rename_func      = (XVirtualFsRenameFunc)xramfs_rename,
    .m_stat_func        = (XVirtualFsStatFunc)xramfs_stat,
    .m_utime_func       = (XVirtualFsUtimeFunc)xramfs_utime,
};
X_IMPL_RTTI_TAG(XRAMFS_RTTI_TAG);



XError xramfs_init(XRamFs* fs, void* mem, size_t size)
{
    XError err = X_ERR_NONE;
    X__DirEntry* root;

    fs->m_fstype_tag = &XRAMFS_RTTI_TAG;

    /* 具体的に最小何バイト必要というのを決めるのは難しいのだが、とりあえず64バ
     * イトとしておく。
     */
    X__EXIT_IF(size < 64, X_ERR_INVALID);

    /* メモリアロケータの初期化。
     * [TODO]
     * アロケータ自身を引数にした方がいいかなー？？
     */
    X__EXIT_IF(!xpalloc_init(&fs->m_alloc, mem, size, X_ALIGN_OF(XMaxAlign)),
               X_ERR_NO_MEMORY);

    /* ルートディレクトリを作成する */
    root = X__CreateDir(fs, NULL, "/");
    X__EXIT_IF(!root, X_ERR_NO_MEMORY);
    fs->m_rootdir = fs->m_curdir = root;

x__exit:

    return err;
}


void xramfs_deinit(XRamFs* fs)
{
    X_ASSERT(fs);
    xpalloc_deinit(&fs->m_alloc);
    fs->m_rootdir = fs->m_curdir = NULL;
}


XVirtualFs* xramfs_init_vfs(XRamFs* fs, XVirtualFs* vfs)
{
    X_ASSERT(fs);
    X_ASSERT(vfs);

    xvfs_init(vfs);
    vfs->m_rtti_tag = &XRAMFS_RTTI_TAG;
    vfs->m_driver = fs;
    vfs->m_vtable = &X__ramfs_vfs_vtable;

    return vfs;
}


XStream* xramfs_init_stream(XStream* stream, XFile* fp)
{
    X_ASSERT(stream);
    X_ASSERT(fp);

    xstream_init(stream);
    stream->m_rtti_tag = &XFILE_STREAM_RTTI_TAG;
    stream->m_driver = fp;
    stream->m_vtable = &X__ramfs_filestream_vtable;

    return stream;
}


XError xramfs_open(XRamFs* fs, const char* path, XOpenMode mode, XFile** o_fp)
{
    X_ASSERT(fs);
    X_ASSERT_NOT_NULL(o_fp);
    X_ASSERT_NOT_NULL(path);
    X__ASSERT_TAG(fs);

    X__Entry* ent;
    X__DirEntry* parent;
    char name[X_NAME_MAX];
    XError err;

    *o_fp = NULL;
    err = X__FindEntry(fs, path, name, &ent, &parent);

    if (err == X_ERR_NO_ENTRY)
    {
        if (!parent)
            return err;
    }
    else if (err != X_ERR_NONE)
        return err;

    if (ent && (ent->m_type == X__TYPE_DIR))
        return X_ERR_IS_DIRECTORY;

    X__File* infp = x_malloc(sizeof(X__File));
    if (!infp)
        return X_ERR_NO_MEMORY;

    X__FileEntry* fileent =  (X__FileEntry*)ent;

    if (fileent)
    {
        if (mode & X_OPEN_FLAG_TRUNCATE)
        {
            xpalloc_deallocate(&fs->m_alloc, fileent->m_data);
            fileent->m_data = NULL;
            fileent->m_size = 0;
            fileent->m_capacity = 0;
            fileent->m_entry.m_timestamp = x_gettimeofday2().tv_sec;
        }
    }
    else
    {
        if (!(mode & (X_OPEN_FLAG_APPEND | X_OPEN_FLAG_TRUNCATE)))
        {
            x_free(infp);
            return X_ERR_NO_ENTRY;
        }

        fileent = X__CreateFile(fs, parent, name);
        if (!fileent)
        {
            x_free(infp);
            return X_ERR_NO_MEMORY;
        }
    }

    if (mode == X_OPEN_MODE_APPEND)
        infp->m_pos = fileent->m_size;
    else
        infp->m_pos = 0;

    infp->m_mode = mode;
    infp->m_fileent = fileent;
    infp->m_vfile.m_fs = fs;
    infp->m_vfile.m_vfs = NULL;
    *o_fp = &(infp->m_vfile);

    return X_ERR_NONE;
}



XError xramfs_close(XFile* fp)
{
    if (!fp)
        return X_ERR_NONE;

    X__ASSERT_TAG(fp->m_fs);
    x_free(fp);
    return X_ERR_NONE;
}


XError xramfs_read(XFile* fp, void* dst, size_t size, size_t* nread)
{
    X_ASSERT(fp);
    X_ASSERT(dst);
    X__ASSERT_TAG(fp->m_fs);

    X__File* const infp = X__GET_FILE_HANDLE(fp);
    X__FileEntry* const fileent = infp->m_fileent;

    if (!(infp->m_mode & X_OPEN_FLAG_READ))
        return X_ERR_ACCESS;

    /* シークでファイルサイズを超えた位置にポインタがある時は読み出せるデータが
     * ないと判断する。エラーを返してもいい気はするが、libcの挙動に合わせておく
     */
    if (infp->m_pos >= fileent->m_size)
    {
        X_ASSIGN_NOT_NULL(nread, 0);
        return X_ERR_NONE;
    }

    const size_t to_read = ((infp->m_pos + size) <= fileent->m_size)
                            ? size : (fileent->m_size - infp->m_pos);
    memcpy(dst, fileent->m_data + infp->m_pos, to_read);
    infp->m_pos += to_read;
    X_ASSIGN_NOT_NULL(nread, to_read);
    return X_ERR_NONE;
}


XError xramfs_write(XFile* fp, const void* src, size_t size, size_t* nwritten)
{
    X_ASSERT(fp);
    X_ASSERT(src);
    X__ASSERT_TAG(fp->m_fs);

    X__File* const infp = X__GET_FILE_HANDLE(fp);
    X__FileEntry* const fileent = infp->m_fileent;

    if (!(infp->m_mode & X_OPEN_FLAG_WRITE))
        return X_ERR_ACCESS;

    XRamFs* const fs = fp->m_fs;
    const size_t pos = (infp->m_mode & X_OPEN_FLAG_APPEND) ? fileent->m_size : infp->m_pos;

    if (pos + size > fileent->m_capacity)
    {
        size_t next_capacity = fileent->m_capacity ? fileent->m_capacity * 1.5f : 32;
        if (next_capacity < pos + size)
            next_capacity = pos + size;

        uint8_t* buf = X__Realloc(fs, fileent->m_data, next_capacity);
        if (!buf)
            return X_ERR_NO_MEMORY;
        fileent->m_data = buf;
        fileent->m_capacity = next_capacity;
    }

    const size_t to_write = ((pos + size) <= fileent->m_capacity)
                            ? size : (fileent->m_capacity - pos);

    memcpy(fileent->m_data + pos, src, to_write);
    infp->m_pos = pos + to_write;
    if (infp->m_pos > fileent->m_size)
        fileent->m_size = infp->m_pos;

    X_ASSIGN_NOT_NULL(nwritten, to_write);

    return X_ERR_NONE;
}


XError xramfs_seek(XFile* fp, XOffset pos, XSeekMode whence)
{
    X_ASSERT_NOT_NULL(fp);
    X__ASSERT_TAG(fp->m_fs);

    X__File* const infp = X__GET_FILE_HANDLE(fp);
    X__FileEntry* const fileent = infp->m_fileent;

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
            new_pos = ((int64_t)(fileent->m_size)) + pos;
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


XError xramfs_tell(XFile* fp, XSize* pos)
{
    X_ASSERT(fp);
    X_ASSERT_NOT_NULL(pos);
    X__ASSERT_TAG(fp->m_fs);

    X__File* const infp = X__GET_FILE_HANDLE(fp);
    *pos = infp->m_pos;

    return X_ERR_NONE;
}


XError xramfs_flush(XFile* fp)
{
    X_ASSERT(fp);
    X__ASSERT_TAG(fp->m_fs);

    /* RAMに直接書き出しをしているのでflushはやることなし。*/
    X_UNUSED(fp);

    return X_ERR_NONE;
}


XError xramfs_mkdir(XRamFs* fs, const char* path)
{
    X_ASSERT(fs);
    X_ASSERT_NOT_NULL(path);
    X__ASSERT_TAG(fs);

    XError err;
    X__Entry* ent;
    X__DirEntry* parent;
    char name[X_NAME_MAX];

    err = X__FindEntry(fs, path, name, &ent, &parent);
    if (err == X_ERR_NONE)
        return X_ERR_EXIST;
    if (err != X_ERR_NO_ENTRY)
        return err;
    if (!parent)
        return X_ERR_NO_ENTRY;

    X__DirEntry* dir = X__CreateDir(fs, parent, name);
    if (!dir)
        return X_ERR_NO_MEMORY;

    return X_ERR_NONE;
}


XError xramfs_opendir(XRamFs* fs, const char* path, XDir** o_dir)
{
    X_ASSERT(fs);
    X_ASSERT_NOT_NULL(o_dir);
    X_ASSERT_NOT_NULL(path);
    X__ASSERT_TAG(fs);

    XError err;
    X__Entry* ent;
    char name[X_NAME_MAX];
    *o_dir = NULL;

    err = X__FindEntry(fs, path, name, &ent, NULL);
    if (err != X_ERR_NONE)
        return err;
    if (ent->m_type != X__TYPE_DIR)
        return X_ERR_NOT_DIRECTORY;

    X__Dir* indirp = x_malloc(sizeof(X__Dir));
    if (! indirp)
        return X_ERR_NO_MEMORY;

    indirp->m_vdir.m_fs = fs;
    indirp->m_vdir.m_vfs = NULL;
    indirp->m_dirent = (X__DirEntry*)ent;
    indirp->m_iterator = NULL;
    *o_dir = &(indirp->m_vdir);

    return X_ERR_NONE;
}


XError xramfs_readdir(XDir* dir, XDirEnt* dirent, XDirEnt** result)
{
    X_ASSERT(dir);
    X_ASSERT_NOT_NULL(dirent);
    X_ASSERT_NOT_NULL(result);
    X__ASSERT_TAG(dir->m_fs);

    X__Dir* const indirp = X__GET_DIR_HANDLE(dir);
    const X__DirEntry* const ent = indirp->m_dirent;

    if (!indirp->m_iterator)
        indirp->m_iterator = xilist_front(&ent->m_children);
    else
        indirp->m_iterator = indirp->m_iterator->next;

    if (indirp->m_iterator == xilist_end(&ent->m_children))
        *result = NULL;
    else
    {
        X__Entry* const next = xnode_entry(indirp->m_iterator,
                                           X__Entry,
                                           m_node);
        strcpy(dirent->name, next->m_name);
        *result = dirent;
    }

    return X_ERR_NONE;
}


XError xramfs_closedir(XDir* dir)
{
    if (!dir)
        return X_ERR_NONE;

    X__ASSERT_TAG(dir->m_fs);
    x_free(dir);

    return X_ERR_NONE;
}


XError xramfs_chdir(XRamFs* fs, const char* path)
{
    X_ASSERT(fs);
    X_ASSERT_NOT_NULL(path);
    X__ASSERT_TAG(fs);

    X__Entry* ent;
    X__DirEntry* parent;
    char name[X_NAME_MAX];

    XError err = X__FindEntry(fs, path, name, &ent, &parent);
    if (err != X_ERR_NONE)
        return err;
    if (ent->m_type != X__TYPE_DIR)
        return X_ERR_NOT_DIRECTORY;

    fs->m_curdir = (X__DirEntry*)ent;

    return X_ERR_NONE;
}


XError xramfs_getcwd(XRamFs* fs, char* buf, size_t size)
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
    X__DirEntry* dir = fs->m_curdir;
    char* p = buf;
    for (;;)
    {
        const char* const name = dir->m_entry.m_name;
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
        dir = dir->m_entry.m_parent;
    }
    x_strreverse(buf);

    return X_ERR_NONE;
}


XError xramfs_remove(XRamFs* fs, const char* path)
{
    X_ASSERT(fs);
    X_ASSERT_NOT_NULL(path);
    X__ASSERT_TAG(fs);

    X__Entry* ent;
    char name[X_NAME_MAX];
    XError err = X__FindEntry(fs, path, name, &ent, NULL);
    if (err != X_ERR_NONE)
        return err;

    if (ent->m_type == X__TYPE_DIR)
    {
        X__DirEntry* dir = (X__DirEntry*)ent;
        if (!xilist_empty(&dir->m_children))
            return X_ERR_NOT_EMPTY;
    }

    /* [TODO]
     * カレントディレクトリの上位のフォルダかどうかをチェックせなあかんね。
     */
    if ((ent == fs->m_rootdir) || (ent == fs->m_curdir))
        return X_ERR_BUSY;

    X__DestoryEntry(fs, ent);

    return X_ERR_NONE;
}


XError xramfs_rename(XRamFs* fs, const char* oldpath, const char* newpath)
{
    X_ASSERT(fs);
    X_ASSERT_NOT_NULL(oldpath);
    X_ASSERT_NOT_NULL(newpath);
    X__ASSERT_TAG(fs);

    X__Entry* ent;
    X__DirEntry* parent;

    XError err;
    char name[X_NAME_MAX];
    err = X__FindEntry(fs, oldpath, name, &ent, NULL);
    if (err != X_ERR_NONE)
        return err;

    err = X__FindEntry(fs, newpath, name, NULL, &parent);
    if (err == X_ERR_NONE)
        return X_ERR_EXIST;
    if (err != X_ERR_NO_ENTRY)
        return err;
    if (!parent)
        return X_ERR_NO_ENTRY;

    char* buf = X__Strdup(fs, name);
    if (!buf)
        return X_ERR_NO_MEMORY;

    X__Free(fs, ent->m_name);
    ent->m_name = buf;
    xnode_unlink(&ent->m_node);
    xilist_push_back(&parent->m_children, &ent->m_node);

    return X_ERR_NONE;
}


XError xramfs_stat(XRamFs* fs, const char* path, XStat* statbuf)
{
    X_ASSERT(fs);
    X_ASSERT_NOT_NULL(statbuf);
    X__ASSERT_TAG(fs);

    X__Entry* ent;
    XError err;
    char name[X_NAME_MAX];
    err = X__FindEntry(fs, path, name, &ent, NULL);
    if (err != X_ERR_NONE)
        return err;

    statbuf->mtime = ent->m_timestamp;
    if (ent->m_type == X__TYPE_DIR)
    {
        statbuf->size = 0;
        statbuf->mode = XSTAT_MODE_DIRECTORY;
    }
    else
    {
        statbuf->size = ((X__FileEntry*)ent)->m_size;
        statbuf->mode = XSTAT_MODE_REGULAR;
    }

    return X_ERR_NONE;
}


XError xramfs_utime(XRamFs* fs, const char* path, XTime time)
{
    X_UNUSED(fs);
    X_UNUSED(path);
    X_UNUSED(time);
    return X_ERR_NONE;
}


static XError X__FindEntry(const XRamFs* fs, const char* path, char* name,
                           X__Entry** o_ent, X__DirEntry** o_parent)
{
    XError err = X_ERR_NONE;
    const X__DirEntry* dir = (path[0] == '/')  ? fs->m_rootdir : fs->m_curdir;
    const X__Entry* ent = NULL;
    const char* next;
    const char* endptr = path;

    X__EXIT_IF(path[0] == '\0', X_ERR_INVALID_NAME);
    if ((path[0] == '/') && (path[1] == '\0'))
    {
        ent = (X__Entry*)dir;
        dir = NULL;
        goto x__exit;
    }

    for (;;)
    {
        next = xfpath_top(endptr, (char**)&endptr);
        X__EXIT_IF(next == NULL, X_ERR_NO_ENTRY);
        X__EXIT_IF(endptr - next >= X_NAME_MAX, X_ERR_NAME_TOO_LONG);

        memcpy(name, next, endptr - next);
        name[(size_t)(endptr - next)] = '\0';

        if (x_strequal(name, "."))
        {
            strcpy(name, dir->m_entry.m_name);
            continue;
        }

        if (x_strequal(name, ".."))
        {
            /* ルートディレクトリを越えて遡ることはできない。ルートを越えたらエ
             * ラーにしてもいい気がするけど、他のシステムの挙動にならっておく。
             */
            if (dir != fs->m_rootdir)
            {
                dir = dir->m_entry.m_parent;
                strcpy(name, dir->m_entry.m_name);
            }
            continue;
        }

        ent = NULL;
        /* ":"は別のファイルシステムがドライブレターで使用する可能性があるので、
         * 禁止。"\"もトラブルの元になりそうなので禁止。
         */
        X__EXIT_IF(strpbrk(name, ":\\"), X_ERR_INVALID_NAME);

        /* 現在のディレクトリから要素を探す */
        XIntrusiveNode* ite;
        xilist_foreach(&dir->m_children, ite)
        {
            X__Entry* const p = xnode_entry(ite, X__Entry, m_node);
            if (x_strequal(name, p->m_name))
            {
                ent = p;
                break;
            }
        }

        endptr = x_strskipchr(endptr, '/');
        if (!ent)
        {
            if (*endptr != '\0')
                dir = NULL;
            err = X_ERR_NO_ENTRY;
            goto x__exit;
        }

        if (*endptr != '\0')
        {
            /* 次の要素があるなら、エントリはディレクトリでなければおかしい */
            if (ent->m_type != X__TYPE_DIR)
            {
                dir = NULL;
                err = X_ERR_NO_ENTRY;
                goto x__exit;
            }

            dir = (X__DirEntry*)ent;
            continue;
        }

        /* 最終要素まで見つかった */
        break;
    }

x__exit:
    X_ASSIGN_NOT_NULL(o_ent, (X__Entry*)ent);
    X_ASSIGN_NOT_NULL(o_parent, (X__DirEntry*)dir);

    return err;
}


static X__DirEntry* X__CreateDir(XRamFs* fs, X__DirEntry* parent, const char* name)
{
    X__DirEntry* dir = X__Malloc(fs, sizeof(X__DirEntry));
    if (!dir)
        return NULL;

    xilist_init(&dir->m_children);
    dir->m_entry.m_parent = parent;
    dir->m_entry.m_type = X__TYPE_DIR;
    dir->m_entry.m_name = X__Strdup(fs, name);
    if (!(dir->m_entry.m_name))
    {
        X__Free(fs, dir);
        return NULL;
    }

    if (parent)
    {
        xilist_push_back(&parent->m_children, &dir->m_entry.m_node);
    }

    return dir;
}


static X__FileEntry* X__CreateFile(XRamFs* fs, X__DirEntry* parent, const char* name)
{
    X__FileEntry* file = X__Malloc(fs, sizeof(X__FileEntry));
    X_ASSERT(file);

    file->m_entry.m_parent = parent;
    file->m_entry.m_type = X__TYPE_FILE;
    file->m_entry.m_name = X__Strdup(fs, name);
    if (!(file->m_entry.m_name))
    {
        X__Free(fs, file);
        return NULL;
    }
    file->m_data = NULL;
    file->m_size = 0;
    file->m_capacity = 0;

    if (parent)
    {
        xilist_push_back(&parent->m_children, &file->m_entry.m_node);
    }

    return file;
}


static void X__DestoryEntry(XRamFs* fs, X__Entry* ent)
{
    X__Free(fs, ent->m_name);
    if (ent->m_parent)
        xnode_unlink(&ent->m_node);

    if (ent->m_type == X__TYPE_FILE)
    {
        X__FileEntry* fp = (X__FileEntry*)ent;
        X__Free(fs, fp->m_data);
    }
    X__Free(fs, ent);
}


static void* X__Malloc(XRamFs* fs, size_t size)
{
    return xpalloc_allocate(&fs->m_alloc, size);
}


static void* X__Realloc(XRamFs* fs, void* old, size_t size)
{
    return xpalloc_reallocate(&fs->m_alloc, old, size);
}


static void X__Free(XRamFs* fs, void* ptr)
{
    xpalloc_deallocate(&fs->m_alloc, ptr);
}


static char* X__Strdup(XRamFs* fs, const char* src)
{
    const size_t len = strlen(src);
    char* p = X__Malloc(fs, len + 1);
    if (p)
    {
        memcpy(p, src, len);
        p[len] = '\0';
    }
    return p;
}
