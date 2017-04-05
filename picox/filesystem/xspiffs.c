/**
 *       @file  xspiffs.c
 *      @brief
 *
 *    @details
 *
 *     @author  MaskedW
 *
 *   @internal
 *     Created  2016/03/06
 * ===================================================================
 */


#include <picox/filesystem/xspiffs.h>
#include <spiffs_nucleus.h>


/* spiffs_fileからファイルディスクリプタを得る前処理をするマクロ。seekの実装の都
 * 合上、内部情報にアクセスする必要があったため、spiffs_hydrogen.cの内部マクロを
 * 持ってきた。
 */
#if SPIFFS_FILEHDL_OFFSET
#define SPIFFS_FH_OFFS(fs, fh)   ((fh) != 0 ? ((fh) + (fs)->cfg.fh_ix_offset) : 0)
#define SPIFFS_FH_UNOFFS(fs, fh) ((fh) != 0 ? ((fh) - (fs)->cfg.fh_ix_offset) : 0)
#else
#define SPIFFS_FH_OFFS(fs, fh)   (fh)
#define SPIFFS_FH_UNOFFS(fs, fh) (fh)
#endif

#define X__ASSERT_TAG(p)        (X_ASSERT(((const XSpiFFs*)p)->m_fstype_tag == &XSPIFFS_RTTI_TAG))
#define X__GET_FILE_HANDLE(fp)  ((((X__File*)fp)->m_filehandle))
#define X__GET_DIR_HANDLE(dir)  (&(((X__Dir*)dir)->m_dirhandle))
#define X__VPOS_INVALID         (0xFFFFFFFFUL)
#define X__GET_ERR()            X__ToXError(SPIFFS_errno(fs->m_spiffs))

typedef spiffs_file     X__FileHandle;
typedef spiffs_DIR*     X__DirHandle;


typedef struct
{
    XFile           m_vfile;
    spiffs_file     m_filehandle;

    XOpenMode       m_mode;

    /* 本来のspiffsはファイルサイズを越えるseekは認められていない。他のファイル
     * システムと同様の挙動をさせるために、ファイルサイズを越えるポジションはこ
     * の変数に保持をさせる。
     */
    XSize           m_vpos;

    /* 同じくファイルサイズを超えるseekのために使用する。write時にファイルサイズ
     * を超えている分はブランクバイトで埋める。
     */
    XSize           m_over_size;
} X__File;


typedef struct
{
    XDir        m_vdir;
    spiffs_DIR  m_dirhandle;
} X__Dir;


static XError X__ToXError(int32_t spiffserrno);


/* ファイルサイズを越えるseekにより、ファイルにブランクスペースを作る場合に使用
 * する。spiffsの内部の実装は把握していないが、SPIフラッシュの特性を考えると、0
 * よりも0xFFで埋める方が望ましいはず。
 */
static const uint8_t X__FFARRAY[128] = {
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255
};


static const XStreamVTable X__spiffs_filestream_vtable = {
    .m_name = "XSpiFFSFileStream",
    .m_read_func = (XStreamReadFunc)xspiffs_read,
    .m_write_func = (XStreamWriteFunc)xspiffs_write,
    .m_close_func = (XStreamCloseFunc)xspiffs_close,
    .m_flush_func = (XStreamFlushFunc)xspiffs_flush,
    .m_seek_func = (XStreamSeekFunc)xspiffs_seek,
    .m_tell_func = (XStreamTellFunc)xspiffs_tell,
};


static const XVirtualFsVTable X__spiffs_vfs_vtable = {
    .m_name = "XSpiFFs",
    .m_open_func        = (XVirtualFsOpenFunc)xspiffs_open,
    .m_close_func       = (XVirtualFsCloseFunc)xspiffs_close,
    .m_read_func        = (XVirtualFsReadFunc)xspiffs_read,
    .m_write_func       = (XVirtualFsWriteFunc)xspiffs_write,
    .m_seek_func        = (XVirtualFsSeekFunc)xspiffs_seek,
    .m_tell_func        = (XVirtualFsTellFunc)xspiffs_tell,
    .m_flush_func       = (XVirtualFsFlushFunc)xspiffs_flush,
    .m_opendir_func     = (XVirtualFsOpendirFunc)xspiffs_opendir,
    .m_readdir_func     = (XVirtualFsReaddirFunc)xspiffs_readdir,
    .m_closedir_func    = (XVirtualFsClosedirFunc)xspiffs_closedir,
    .m_getcwd_func      = (XVirtualFsGetcwdFunc)xspiffs_getcwd,
    .m_remove_func      = (XVirtualFsRemoveFunc)xspiffs_remove,
    .m_rename_func      = (XVirtualFsRenameFunc)xspiffs_rename,
    .m_stat_func        = (XVirtualFsStatFunc)xspiffs_stat,
};
X_IMPL_RTTI_TAG(XSPIFFS_RTTI_TAG);


void xspiffs_init(XSpiFFs* fs, spiffs* src)
{
    X_ASSERT(fs);
    X_ASSERT_NOT_NULL(src);

    fs->m_fstype_tag = &XSPIFFS_RTTI_TAG;
    fs->m_spiffs = src;
}


void xspiffs_deinit(XSpiFFs* fs)
{
    X_ASSERT(fs);
    X_UNUSED(fs);
}


XVirtualFs* xspiffs_init_vfs(XSpiFFs* fs, XVirtualFs* vfs)
{
    X_ASSERT(fs);
    X_ASSERT(vfs);

    xvfs_init(vfs);
    vfs->m_rtti_tag = &XSPIFFS_RTTI_TAG;
    vfs->m_driver = fs;
    vfs->m_vtable = &X__spiffs_vfs_vtable;

    return vfs;
}


XStream* xspiffs_init_stream(XStream* stream, XFile* fp)
{
    X_ASSERT(stream);
    X_ASSERT(fp);

    xstream_init(stream);
    stream->m_rtti_tag = &XFILE_STREAM_RTTI_TAG;
    stream->m_driver = fp;
    stream->m_vtable = &X__spiffs_filestream_vtable;

    return stream;
}


XError xspiffs_open(XSpiFFs* fs, const char* path, XOpenMode mode, XFile** o_fp)
{
    X_ASSERT(fs);
    X_ASSERT_NOT_NULL(path);
    X_ASSERT_NOT_NULL(o_fp);
    X__ASSERT_TAG(fs);

    *o_fp = NULL;
    XError err = X_ERR_NONE;
    spiffs_flags flags;

    /* modeのspiffs形式への変換 */
    switch (mode)
    {
        case X_OPEN_MODE_READ:
            flags = SPIFFS_RDONLY;                                  break;
        case X_OPEN_MODE_WRITE:
            flags = SPIFFS_WRONLY | SPIFFS_CREAT | SPIFFS_TRUNC;    break;
        case X_OPEN_MODE_APPEND:
            flags = SPIFFS_WRONLY | SPIFFS_CREAT | SPIFFS_APPEND;   break;
        case X_OPEN_MODE_READ_PLUS:
            flags = SPIFFS_RDWR;                                    break;
        case X_OPEN_MODE_WRITE_PLUS:
            flags = SPIFFS_RDWR | SPIFFS_CREAT | SPIFFS_TRUNC;      break;
        case X_OPEN_MODE_APPEND_PLUS:
            flags = SPIFFS_RDWR | SPIFFS_CREAT | SPIFFS_APPEND;     break;
        default:
            return X_ERR_INVALID;
    }

    X__FileHandle fd = -1;
    X__File* infp = NULL;

    infp = x_malloc(sizeof(X__File));
    if (!infp)
    {
        err = X_ERR_NO_MEMORY;
        goto x__exit;
    }

    fd = SPIFFS_open(fs->m_spiffs, path, flags, 0);
    if (fd < 0)
    {
        err = X__GET_ERR();
        goto x__exit;
    }

    if (mode == X_OPEN_MODE_APPEND)
    {
        if (SPIFFS_lseek(fs->m_spiffs, fd, 0, SPIFFS_SEEK_END) < 0)
        {
            err = X__GET_ERR();
            goto x__exit;
        }
    }

    /* ファイルポインタの初期化 */
    infp->m_vfile.m_fs = fs;
    infp->m_filehandle = fd;
    infp->m_mode = mode;
    infp->m_vpos = X__VPOS_INVALID;
    infp->m_over_size = 0;
    *o_fp = &(infp->m_vfile);

    infp = NULL;
    fd = -1;

x__exit:
    x_free(infp);
    if (fd >= 0)
        SPIFFS_close(fs->m_spiffs, fd);

    return err;
}


XError xspiffs_close(XFile* fp)
{
    XError err = X_ERR_NONE;
    if (!fp)
        return err;

    X__ASSERT_TAG(fp->m_fs);

    XSpiFFs* const fs = fp->m_fs;
    X__File* const infp = (X__File*)fp;
    X__FileHandle const filehandle = X__GET_FILE_HANDLE(infp);

    if (SPIFFS_close(fs->m_spiffs, filehandle) != 0)
        err = X__GET_ERR();
    x_free(fp);

    return err;
}


XError xspiffs_write(XFile* fp, const void* src, size_t size, size_t* nwritten)
{
    X_ASSERT(fp);
    X_ASSERT_NOT_NULL(src);
    X__ASSERT_TAG(fp->m_fs);

    X_ASSIGN_NOT_NULL(nwritten, 0);
    XError err = X_ERR_NONE;
    XSpiFFs* const fs = fp->m_fs;
    X__File* const infp = (X__File*)fp;
    X__FileHandle const filehandle = X__GET_FILE_HANDLE(infp);

    /* seek位置がファイルサイズを越えていた場合は空きスペースをブランクバイトで
     * 埋める。
     */
    if (infp->m_vpos != X__VPOS_INVALID)
    {
        size_t over = infp->m_over_size;
        X_ASSERT(over);

        if (SPIFFS_lseek(fs->m_spiffs, filehandle,
                         infp->m_vpos - infp->m_over_size,
                         SPIFFS_SEEK_SET) < 0)
        {
            err = X__GET_ERR();
            goto x__exit;
        }

        while (over)
        {
            const size_t n = (over > sizeof(X__FFARRAY)) ? sizeof(X__FFARRAY) : over;
            if (SPIFFS_write(fs->m_spiffs, filehandle, (void*)X__FFARRAY, n) < 0)
            {
                err = X__GET_ERR();
                goto  x__exit;
            }
            over -= n;
        }
        infp->m_vpos = X__VPOS_INVALID;
        infp->m_over_size = 0;
    }

    const int ret = SPIFFS_write(fs->m_spiffs, filehandle, (void*)src, size);
    if (ret < 0)
    {
        err = X__GET_ERR();
        goto x__exit;
    }

    X_ASSIGN_NOT_NULL(nwritten, ret);

x__exit:

    return err;
}


XError xspiffs_read(XFile* fp, void* dst, size_t size, size_t* nread)
{
    X_ASSERT(fp);
    X_ASSERT_NOT_NULL(dst);
    X__ASSERT_TAG(fp->m_fs);

    X_ASSIGN_NOT_NULL(nread, 0);
    XError err = X_ERR_NONE;
    XSpiFFs* const fs = fp->m_fs;
    X__File* const infp = (X__File*)fp;
    X__FileHandle const filehandle = X__GET_FILE_HANDLE(infp);

    const int32_t ret = SPIFFS_read(fs->m_spiffs, filehandle, dst, size);
    if (ret < 0)
        err = X__GET_ERR();
    else
        X_ASSIGN_NOT_NULL(nread, ret);

    return err;
}


XError xspiffs_seek(XFile* fp, XOffset pos, XSeekMode whence)
{
    X_ASSERT(fp);
    X__ASSERT_TAG(fp->m_fs);

    XError err = X_ERR_NONE;
    XSpiFFs* const fs = fp->m_fs;
    X__File* const infp = (X__File*)fp;
    X__FileHandle const filehandle = X__GET_FILE_HANDLE(infp);
    spiffs_fd* fd = NULL;

    if (SPIFFS_fflush(fs->m_spiffs, filehandle) < 0)
    {
        err = X__GET_ERR();
        goto x__exit;
    }

    if (spiffs_fd_get(fs->m_spiffs, SPIFFS_FH_UNOFFS(fs->m_spiffs, filehandle), &fd) < 0)
    {
        err = X__GET_ERR();
        goto x__exit;
    }

    const XOffset cur_size = fd->size == (XSize)SPIFFS_UNDEFINED_LEN ? 0 : fd->size;
    const XOffset cur_pos = (infp->m_vpos == X__VPOS_INVALID) ? fd->fdoffset : infp->m_vpos;
    XOffset seek_pos = -1;

    switch (whence)
    {
        case X_SEEK_SET:
            seek_pos = pos;
            break;
        case X_SEEK_CUR:
            seek_pos = cur_pos + pos;
            break;
        case X_SEEK_END:
            seek_pos = cur_size + pos;
            break;
        default:
            break;
    }

    if (seek_pos < 0)
    {
        err = X_ERR_INVALID;
        goto x__exit;
    }

    /* seek位置がファイルサイズを越えていた場合は、仮想位置のセットだけを行う */
    if (seek_pos > cur_size)
    {
        infp->m_vpos = seek_pos;
        infp->m_over_size = seek_pos - cur_size;
    }
    else
    {
        if (SPIFFS_lseek(fs->m_spiffs, filehandle, seek_pos, SPIFFS_SEEK_SET) < 0)
        {
            err = X__GET_ERR();
            goto x__exit;
        }
        infp->m_vpos = X__VPOS_INVALID;
        infp->m_over_size = 0;
    }

x__exit:

    return err;
}


XError xspiffs_tell(XFile* fp, XSize* pos)
{
    X_ASSERT(fp);
    X_ASSERT_NOT_NULL(pos);

    *pos = 0;
    XError err = X_ERR_NONE;
    XSpiFFs* const fs = fp->m_fs;
    X__File* const infp = (X__File*)fp;
    X__FileHandle const filehandle = X__GET_FILE_HANDLE(infp);

    /* seek位置がファイルサイズを越えていた場合は、仮想位置を返す */
    if (infp->m_vpos != X__VPOS_INVALID)
        *pos = infp->m_vpos;
    else
    {
        const int32_t ret = SPIFFS_tell(fs->m_spiffs, filehandle);
        if (ret < 0)
            err = X__GET_ERR();
        else
            *pos = ret;
    }

    return err;
}


XError xspiffs_flush(XFile* fp)
{
    X_ASSERT(fp);

    XError err = X_ERR_NONE;
    XSpiFFs* const fs = fp->m_fs;
    X__File* const infp = (X__File*)fp;
    X__FileHandle const filehandle = X__GET_FILE_HANDLE(infp);

    const int32_t ret = SPIFFS_fflush(fs->m_spiffs, filehandle);
    if (ret < 0)
        err = X__GET_ERR();

    return err;
}


XError xspiffs_opendir(XSpiFFs* fs, const char* path, XDir** o_dir)
{
    X_ASSERT(fs);
    X_ASSERT_NOT_NULL(path);
    X_ASSERT_NOT_NULL(o_dir);
    X__ASSERT_TAG(fs);

    XError err = X_ERR_NONE;
    *o_dir = NULL;

    /* spiffsはフラットファイル構造なのでルート以外のディレクトリ指定は無効 */
    if (!x_strequal(path, "/"))
        return X_ERR_NO_ENTRY;

    X__Dir* indirp = x_malloc(sizeof(X__Dir));
    if (!indirp)
        return X_ERR_NO_MEMORY;

    if (!SPIFFS_opendir(fs->m_spiffs, path, X__GET_DIR_HANDLE(indirp)))
    {
        err = X__GET_ERR();
        goto x__exit;
    }

    indirp->m_vdir.m_fs = fs;
    *o_dir = &(indirp->m_vdir);
    indirp = NULL;

x__exit:
    if (indirp)
        x_free(indirp);

    return err;
}


XError xspiffs_readdir(XDir* dir, XDirEnt* dirent, XDirEnt** result)
{
    X_ASSERT(dir);
    X_ASSERT_NOT_NULL(dirent);
    X_ASSERT_NOT_NULL(result);
    X__ASSERT_TAG(dir->m_fs);

    *result = NULL;
    XError err = X_ERR_NONE;
    XSpiFFs* const fs = dir->m_fs;
    X__DirHandle const dirhandle = X__GET_DIR_HANDLE(dir);
    struct spiffs_dirent spiffs_dirent_buf;

    if (!SPIFFS_readdir(dirhandle, &spiffs_dirent_buf))
    {
        const int16_t spiffserr = SPIFFS_errno(fs->m_spiffs);
        err = (spiffserr == SPIFFS_VIS_END) ? X_ERR_NONE : X__ToXError(spiffserr);
        goto x__exit;
    }

    *result = dirent;
    x_strlcpy(dirent->name, (char*)(spiffs_dirent_buf.name), X_NAME_MAX);

x__exit:

    return err;
}


XError xspiffs_closedir(XDir* dir)
{
    X_ASSERT(dir);
    X__ASSERT_TAG(dir->m_fs);

    XError err = X_ERR_NONE;
    XSpiFFs* const fs = dir->m_fs;
    X__DirHandle const dirhandle = X__GET_DIR_HANDLE(dir);

    if (SPIFFS_closedir(dirhandle) < 0)
        err = X__GET_ERR();

    x_free(dir);

    return err;
}


XError xspiffs_getcwd(XSpiFFs* fs, char* buf, size_t size)
{
    X_ASSERT(fs);
    X_ASSERT_NOT_NULL(buf);
    X__ASSERT_TAG(fs);

    XError err = X_ERR_NONE;
    if (size <= 1)
        return X_ERR_RANGE;

    strcpy(buf, "/");
    return err;
}


XError xspiffs_remove(XSpiFFs* fs, const char* path)
{
    X_ASSERT(fs);
    X_ASSERT_NOT_NULL(path);
    X__ASSERT_TAG(fs);

    XError err = X_ERR_NONE;
    if (SPIFFS_remove(fs->m_spiffs, path) < 0)
        err = X__GET_ERR();

    return err;
}


XError xspiffs_rename(XSpiFFs* fs, const char* oldpath, const char* newpath)
{
    X_ASSERT(fs);
    X_ASSERT_NOT_NULL(oldpath);
    X_ASSERT_NOT_NULL(newpath);
    X__ASSERT_TAG(fs);

    XError err = X_ERR_NONE;
    if (SPIFFS_rename(fs->m_spiffs, oldpath, newpath) < 0)
        err = X__GET_ERR();

    return err;
}


XError xspiffs_stat(XSpiFFs* fs, const char* path, XStat* statbuf)
{
    X_ASSERT(fs);
    X_ASSERT(statbuf);
    X_ASSERT(path);
    X__ASSERT_TAG(fs);

    XError err = X_ERR_NONE;
    spiffs_stat spiffs_statbuf;

    if (SPIFFS_stat(fs->m_spiffs, path, &spiffs_statbuf) < 0)
    {
        err = X__GET_ERR();
        goto x__exit;
    }

    statbuf->size = spiffs_statbuf.size;
    statbuf->mtime = 0;
    statbuf->mode = 0;

x__exit:
    return err;
}


static XError X__ToXError(int32_t spiffserrno)
{
    XError err;
    switch (spiffserrno)
    {
        /* EOFは正常終了として扱う */
        case SPIFFS_ERR_END_OF_OBJECT:
        case SPIFFS_OK:
            err = X_ERR_NONE; break;

        /* mountされていない */
        case SPIFFS_ERR_NOT_MOUNTED:
            err = X_ERR_NOT_READY; break;

        /* open()等で対象が発見できず */
        case SPIFFS_ERR_NOT_FOUND:
            err = X_ERR_NO_ENTRY; break;

        /* 読み込み専用でopenしたのに書き込みを行おうとした時等 */
        case SPIFFS_ERR_NOT_WRITABLE:
        case SPIFFS_ERR_NOT_READABLE:
            err = X_ERR_ACCESS; break;


        /* ファイルディスクリプタの生成上限を超えた */
        case SPIFFS_ERR_OUT_OF_FILE_DESCS:
            err = X_ERR_MANY; break;

        /* 空き容量なし */
        case SPIFFS_ERR_FULL:
            err = X_ERR_NO_SPACE; break;

        /* 作成対象の名前がすでに存在している */
        case SPIFFS_ERR_FILE_EXISTS:
        case SPIFFS_ERR_CONFLICTING_NAME:
            err = X_ERR_EXIST; break;

        /* ブロック消去に失敗した */
        case SPIFFS_ERR_ERASE_FAIL:
            err = X_ERR_IO; break;

        /* コンフィグ設定によって、使用不可なオペレーションがある */
        case SPIFFS_ERR_NOT_CONFIGURED:
        case SPIFFS_ERR_RO_NOT_IMPL:
        case SPIFFS_ERR_RO_ABORTED_OPERATION:
            err = X_ERR_NOT_SUPPORTED; break;

        /* 内部エラー */
        case SPIFFS_ERR_INTERNAL:
            err = X_ERR_INTERNAL; break;


        /* この辺のエラーは引数が不正でなければ発生しないはず */
        case SPIFFS_ERR_FILE_CLOSED:
        case SPIFFS_ERR_FILE_DELETED:
        case SPIFFS_ERR_BAD_DESCRIPTOR:
        case SPIFFS_ERR_NOT_FINALIZED:
        case SPIFFS_ERR_NOT_INDEX:
        case SPIFFS_ERR_INDEX_REF_FREE:
        case SPIFFS_ERR_INDEX_REF_LU:
        case SPIFFS_ERR_INDEX_REF_INVALID:
        case SPIFFS_ERR_INDEX_FREE:
        case SPIFFS_ERR_INDEX_LU:
        case SPIFFS_ERR_INDEX_INVALID:
        case SPIFFS_ERR_IS_INDEX:
        case SPIFFS_ERR_IS_FREE:
        case SPIFFS_ERR_INDEX_SPAN_MISMATCH:
        case SPIFFS_ERR_DATA_SPAN_MISMATCH:
        case SPIFFS_ERR_NOT_A_FILE:
            err = X_ERR_INVALID;    break;

        /* 以下エラーコードの発生要因となり得る関数はこのモジュールでは提供して
         * いない。
         */
        case SPIFFS_ERR_MAGIC_NOT_POSSIBLE:
        case SPIFFS_ERR_NOT_A_FS:
        case SPIFFS_ERR_PROBE_TOO_FEW_BLOCKS:
        case SPIFFS_ERR_PROBE_NOT_A_FS:
        case SPIFFS_ERR_NO_DELETED_BLOCKS:
        case SPIFFS_ERR_MOUNTED:
        default:
            err = X_ERR_OTHER;      break;
    }
    return err;
}
