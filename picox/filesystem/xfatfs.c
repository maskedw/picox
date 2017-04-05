/**
 *       @file  xfatfs.c
 *      @brief
 *
 *    @details
 *
 *     @author  MaskedW
 *
 *   @internal
 *     Created  2016/02/27
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


#include <picox/filesystem/xfatfs.h>
#include <ff.h> /* fatfs */


#define X__ASSERT_TAG(p)        (X_ASSERT(((const XFatFs*)p)->m_fstype_tag == &XFATFS_RTTI_TAG))
#define X__GET_FILE_HANDLE(fp)  (&(((X__File*)fp)->m_filehandle))
#define X__GET_DIR_HANDLE(dir)  (&(((X__Dir*)dir)->m_dirhandle))
#define X__EXIT_IF(cond, v)     X_ASSIGN_AND_GOTO_IF(cond, err, v, x__exit)
#define X__VPOS_INVALID         (0xFFFFFFFF)


typedef struct
{
    XFile       m_vfile;
    FIL         m_filehandle;
    DWORD       m_vpos;
    XOpenMode   m_mode;
} X__File;


typedef struct
{
    XDir    m_vdir;
    DIR     m_dirhandle;
} X__Dir;


static XError X__ToXError(FRESULT fres);


/* fatfs Long file name option */
#if _USE_LFN >= 1
    static char lfn[_MAX_LFN + 1];
    #define X__DECLARE_FINFO(name)     FILINFO name; name.lfname = lfn; name.fsize = sizeof(lfn)
    #define X__GET_FNAME(finfo)        (*(finfo.lfname) ? finfo.lfname : finfo.fname)
#else
    #define X__DECLARE_FINFO(name)     FILINFO name
    #define X__GET_FNAME(finfo)        finfo.fname
#endif


static const XStreamVTable X__fatfs_filestream_vtable = {
    .m_name = "XFatFsFileStream",
    .m_read_func = (XStreamReadFunc)xfatfs_read,
    .m_write_func = (XStreamWriteFunc)xfatfs_write,
    .m_close_func = (XStreamCloseFunc)xfatfs_close,
    .m_flush_func = (XStreamFlushFunc)xfatfs_flush,
    .m_seek_func = (XStreamSeekFunc)xfatfs_seek,
    .m_tell_func = (XStreamTellFunc)xfatfs_tell,
};


static const XVirtualFsVTable X__fatfs_vfs_vtable = {
    .m_name = "XFatFs",
    .m_open_func        = (XVirtualFsOpenFunc)xfatfs_open,
    .m_close_func       = (XVirtualFsCloseFunc)xfatfs_close,
    .m_read_func        = (XVirtualFsReadFunc)xfatfs_read,
    .m_write_func       = (XVirtualFsWriteFunc)xfatfs_write,
    .m_seek_func        = (XVirtualFsSeekFunc)xfatfs_seek,
    .m_tell_func        = (XVirtualFsTellFunc)xfatfs_tell,
    .m_flush_func       = (XVirtualFsFlushFunc)xfatfs_flush,
    .m_mkdir_func       = (XVirtualFsMkdirFunc)xfatfs_mkdir,
    .m_opendir_func     = (XVirtualFsOpendirFunc)xfatfs_opendir,
    .m_readdir_func     = (XVirtualFsReaddirFunc)xfatfs_readdir,
    .m_closedir_func    = (XVirtualFsClosedirFunc)xfatfs_closedir,
    .m_chdir_func       = (XVirtualFsChdirFunc)xfatfs_chdir,
    .m_getcwd_func      = (XVirtualFsGetcwdFunc)xfatfs_getcwd,
    .m_remove_func      = (XVirtualFsRemoveFunc)xfatfs_remove,
    .m_rename_func      = (XVirtualFsRenameFunc)xfatfs_rename,
    .m_stat_func        = (XVirtualFsStatFunc)xfatfs_stat,
    .m_utime_func       = (XVirtualFsUtimeFunc)xfatfs_utime,
};
X_IMPL_RTTI_TAG(XFATFS_RTTI_TAG);


void xfatfs_init(XFatFs* fs)
{
    X_ASSERT(fs);
    fs->m_fstype_tag = &XFATFS_RTTI_TAG;
}


void xfatfs_deinit(XFatFs* fs)
{
    X_ASSERT(fs);
    X_UNUSED(fs);
}


XVirtualFs* xfatfs_init_vfs(XFatFs* fs, XVirtualFs* vfs)
{
    X_ASSERT(fs);
    X_ASSERT(vfs);

    xvfs_init(vfs);
    vfs->m_rtti_tag = &XFATFS_RTTI_TAG;
    vfs->m_driver = fs;
    vfs->m_vtable = &X__fatfs_vfs_vtable;

    return vfs;
}


XStream* xfatfs_init_stream(XStream* stream, XFile* fp)
{
    X_ASSERT(stream);
    X_ASSERT(fp);

    xstream_init(stream);
    stream->m_rtti_tag = &XFILE_STREAM_RTTI_TAG;
    stream->m_driver = fp;
    stream->m_vtable = &X__fatfs_filestream_vtable;

    return stream;
}


XError xfatfs_open(XFatFs* fs, const char* path, XOpenMode mode, XFile** o_fp)
{
    X_ASSERT(fs);
    X_ASSERT(o_fp);
    X_ASSERT(path);
    X__ASSERT_TAG(fs);

    *o_fp = NULL;
    FRESULT fres;
    XError err = X_ERR_NONE;
    BYTE fsmode = 0;
    FIL* filehandle = NULL;


    switch (mode)
    {
        case X_OPEN_MODE_READ:
            fsmode = FA_READ | FA_OPEN_EXISTING;            break;
        case X_OPEN_MODE_WRITE:
            fsmode = FA_WRITE | FA_CREATE_ALWAYS;           break;
        case X_OPEN_MODE_APPEND:
            fsmode = FA_WRITE | FA_OPEN_ALWAYS;             break;
        case X_OPEN_MODE_READ_PLUS:
            fsmode = FA_READ | FA_WRITE | FA_OPEN_EXISTING; break;
        case X_OPEN_MODE_WRITE_PLUS:
            fsmode = FA_READ | FA_WRITE | FA_CREATE_ALWAYS; break;
        case X_OPEN_MODE_APPEND_PLUS:
            fsmode = FA_READ | FA_WRITE | FA_OPEN_ALWAYS;   break;
        default:
            break;
    }

    X__File* infp = x_malloc(sizeof(X__File));
    X__EXIT_IF(!infp, X_ERR_NO_MEMORY);

    filehandle = X__GET_FILE_HANDLE(infp);
    fres = f_open(filehandle, path, fsmode);

    X__EXIT_IF(fres != FR_OK, X__ToXError(fres));

    if (mode == X_OPEN_MODE_APPEND)
    {
        fres = f_lseek(filehandle, f_size(filehandle));
        X__EXIT_IF(fres != FR_OK, X__ToXError(fres));
    }

    infp->m_vfile.m_fs = fs;
    infp->m_mode = mode;
    infp->m_vpos = X__VPOS_INVALID;
    *o_fp = &(infp->m_vfile);

    infp = NULL;
    filehandle = NULL;

x__exit:
    x_free(infp);
    if (filehandle)
        f_close(filehandle);

    return err;
}


XError xfatfs_close(XFile* fp)
{
    XError err = X_ERR_NONE;
    if (!fp)
        return err;

    X__ASSERT_TAG(fp->m_fs);
    X__File* const infp = (X__File*)fp;
    FIL* const filehandle = X__GET_FILE_HANDLE(infp);

    const FRESULT fres = f_close(filehandle);
    if (fres != FR_OK)
        err = X__ToXError(fres);
    x_free(infp);

    return err;
}


XError xfatfs_write(XFile* fp, const void* src, size_t size, size_t* nwritten)
{
    X_ASSERT(fp);
    X_ASSERT(src);
    X__ASSERT_TAG(fp->m_fs);

    XError err = X_ERR_NONE;
    FRESULT fres = FR_OK;
    const char* p = src;
    X__File* const infp = (X__File*)fp;
    FIL* const filehandle = X__GET_FILE_HANDLE(fp);

    X_ASSIGN_NOT_NULL(nwritten, 0);

    if (infp->m_mode & X_OPEN_FLAG_APPEND)
    {
        /* 追記モードの時は常にファイルの末尾に書き込む */
        fres = f_lseek(filehandle, f_size(filehandle));
        X__EXIT_IF(fres != FR_OK, X__ToXError(fres));
    }
    else
    {
        if (infp->m_vpos != X__VPOS_INVALID)
        {
            fres = f_lseek(filehandle, infp->m_vpos);
            X__EXIT_IF(fres != FR_OK, X__ToXError(fres));
            infp->m_vpos = X__VPOS_INVALID;
        }
    }

    while (size)
    {
        const UINT to_write = (size > UINT_MAX) ? UINT_MAX : size;
        UINT written;

        fres = f_write(filehandle, p, to_write, &written);
        X__EXIT_IF(fres != FR_OK, X__ToXError(fres));

        size -= to_write;
        p    += to_write;

        /* Disk full チェック */
        X_BREAK_IF(to_write != written);
    }

    X_ASSIGN_NOT_NULL(nwritten, p - (const char*)src);

x__exit:
    return err;
}


XError xfatfs_read(XFile* fp, void* dst, size_t size, size_t* nread)
{
    X_ASSERT(fp);
    X_ASSERT(dst);
    X__ASSERT_TAG(fp->m_fs);

    XError err = X_ERR_NONE;
    FRESULT fres = FR_OK;
    char* p = dst;
    FIL* const filehandle = X__GET_FILE_HANDLE(fp);

    X_ASSIGN_NOT_NULL(nread, 0);

    while (size)
    {
        const UINT to_read = (size > UINT_MAX) ? UINT_MAX : size;
        UINT read;

        fres = f_read(filehandle, p, to_read, &read);
        X__EXIT_IF(fres != FR_OK, X__ToXError(fres));

        size -= read;
        p    += read;

        /* ファイル終端チェック */
        X_BREAK_IF(to_read != read);
    }

    X_ASSIGN_NOT_NULL(nread, p - (char*)dst);

x__exit:
    return err;
}


XError xfatfs_seek(XFile* fp, XOffset pos, XSeekMode whence)
{
    X_ASSERT(fp);
    X__ASSERT_TAG(fp->m_fs);

    XError err = X_ERR_NONE;
    X__File* const infp = (X__File*)fp;
    FIL* const filehandle = X__GET_FILE_HANDLE(fp);
    FRESULT fres = FR_OK;

    const XSize cur_size = f_size(filehandle);
    const XSize cur_pos = (infp->m_vpos == X__VPOS_INVALID) ? f_tell(filehandle) : infp->m_vpos;
    int64_t seek_pos = -1;

    switch (whence)
    {
        case X_SEEK_SET:
            seek_pos = pos;
            break;
        case X_SEEK_CUR:
            seek_pos = ((int64_t)cur_pos) + pos;
            break;
        case X_SEEK_END:
            seek_pos = ((int64_t)cur_size) + pos;
            break;
        default:
            break;
    }

    X__EXIT_IF(seek_pos < 0, X_ERR_INVALID);
    X__EXIT_IF(seek_pos > UINT32_MAX, X_ERR_INVALID);

    /* seek位置がファイルサイズを越えていた場合は、仮想位置のセットだけを行う */
    if (seek_pos > cur_size)
    {
        infp->m_vpos = seek_pos;
    }
    else
    {
        fres = f_lseek(filehandle, seek_pos);
        X__EXIT_IF(fres != FR_OK, X__ToXError(fres));
        infp->m_vpos = X__VPOS_INVALID;
    }

x__exit:

    return err;
}


XError xfatfs_tell(XFile* fp, XSize* pos)
{
    X_ASSERT(fp);
    X_ASSERT(pos);
    X__ASSERT_TAG(fp->m_fs);

    XError err = X_ERR_NONE;
    FIL* const filehandle = X__GET_FILE_HANDLE(fp);

    X__File* const infp = (X__File*)fp;
    if (infp->m_vpos != X__VPOS_INVALID)
        *pos = infp->m_vpos;
    else
        *pos = f_tell(filehandle);

    return err;
}


XError xfatfs_flush(XFile* fp)
{
    X_ASSERT(fp);
    X__ASSERT_TAG(fp->m_fs);

    XError err = X_ERR_NONE;
    FIL* const filehandle = X__GET_FILE_HANDLE(fp);
    const FRESULT fres = f_sync(filehandle);
    X_ASSIGN_IF(fres != FR_OK, err, X__ToXError(fres));

    return err;
}


XError xfatfs_mkdir(XFatFs* fs, const char* path)
{
    X_ASSERT(fs);
    X_ASSERT(path);
    X__ASSERT_TAG(fs);

    XError err = X_ERR_NONE;
    const FRESULT fres = f_mkdir(path);
    X_ASSIGN_IF(fres != FR_OK, err, X__ToXError(fres));

    return err;
}


XError xfatfs_opendir(XFatFs* fs, const char* path, XDir** o_dir)
{
    X_ASSERT(fs);
    X_ASSERT(o_dir);
    X__ASSERT_TAG(fs);

    *o_dir = NULL;
    XError err = X_ERR_NONE;
    X__Dir* indirp = x_malloc(sizeof(X__Dir));
    X__EXIT_IF(!indirp, X_ERR_NO_MEMORY);

    DIR* dirhandle = &(indirp->m_dirhandle);
    const FRESULT fres = f_opendir(dirhandle, path);
    X__EXIT_IF(fres != FR_OK, X__ToXError(fres));

    indirp->m_vdir.m_fs = fs;
    *o_dir = &(indirp->m_vdir);
    indirp = NULL;

x__exit:
    if (indirp)
        x_free(indirp);

    return err;
}


XError xfatfs_readdir(XDir* dir, XDirEnt* dirent, XDirEnt** result)
{
    X_ASSERT(dir);
    X_ASSERT(dirent);
    X_ASSERT(result);
    X__ASSERT_TAG(dir->m_fs);

    *result = NULL;
    X__DECLARE_FINFO(finfo);
    XError err = X_ERR_NONE;
    DIR* dirhandle = X__GET_DIR_HANDLE(dir);

    const FRESULT fres = f_readdir(dirhandle, &finfo);
    X__EXIT_IF(fres != FR_OK, X__ToXError(fres));

    const char* const fname = X__GET_FNAME(finfo);
    if (fname[0] == '\0')
        goto x__exit;

    X__EXIT_IF(strlen(fname) >= X_NAME_MAX, X_ERR_RANGE);

    strcpy(dirent->name, fname);
    *result = dirent;

x__exit:

    return err;
}


XError xfatfs_closedir(XDir* dir)
{
    XError err = X_ERR_NONE;
    if (!dir)
        return err;

    X__ASSERT_TAG(dir->m_fs);
    X__Dir* const indirp = (X__Dir*)dir;
    DIR* dirhandle = X__GET_DIR_HANDLE(indirp);

    const FRESULT fres = f_closedir(dirhandle);
    X_ASSIGN_IF(fres != FR_OK, err, X__ToXError(fres));
    x_free(indirp);

    return err;
}


XError xfatfs_chdir(XFatFs* fs, const char* path)
{
    X_ASSERT(fs);
    X_ASSERT(path);
    X__ASSERT_TAG(fs);

    XError err = X_ERR_NONE;
    const FRESULT fres = f_chdir(path);
    X_ASSIGN_IF(fres != FR_OK, err, X__ToXError(fres));

    return err;
}


XError xfatfs_getcwd(XFatFs* fs, char* buf, size_t size)
{
    X_ASSERT(fs);
    X_ASSERT(buf);
    X__ASSERT_TAG(fs);

#if _VOLUMES == 1
    /* ボリューム数が1の時は、f_getcwd()はボリュームラベルを付加しないが、xunionfsの
     * 実装上、常にボリュームラベルがついているほうが都合がいい。
     */
    if (size < 3)
        return X_ERR_NAME_TOO_LONG;
    strcpy(buf, "0:");
    buf += 2;
    size -= 2;
#endif

    XError err = X_ERR_NONE;
    const FRESULT fres = f_getcwd(buf, size);
    X_ASSIGN_IF(fres != FR_OK, err, X__ToXError(fres));

    return err;
}


XError xfatfs_remove(XFatFs* fs, const char* path)
{
    X_ASSERT(fs);
    X_ASSERT(path);
    X__ASSERT_TAG(fs);

    XError err = X_ERR_NONE;
    const FRESULT fres = f_unlink(path);
    X_ASSIGN_IF(fres != FR_OK, err, X__ToXError(fres));

    return err;
}


XError xfatfs_rename(XFatFs* fs, const char* oldpath, const char* newpath)
{
    X_ASSERT(fs);
    X_ASSERT(oldpath);
    X_ASSERT(newpath);
    X__ASSERT_TAG(fs);

    XError err = X_ERR_NONE;
    const FRESULT fres = f_rename(oldpath, newpath);
    X_ASSIGN_IF(fres != FR_OK, err, X__ToXError(fres));

    return err;
}


XError xfatfs_stat(XFatFs* fs, const char* path, XStat* statbuf)
{
    X_ASSERT(fs);
    X_ASSERT(statbuf);
    X_ASSERT(path);
    X__ASSERT_TAG(fs);

    X__DECLARE_FINFO(finfo);
    XError err = X_ERR_NONE;
    const FRESULT fres = f_stat(path, &finfo);
    X__EXIT_IF(fres != FR_OK, X__ToXError(fres));

    /* [TODO] fdate, ftime, */
    statbuf->size = finfo.fsize;
    statbuf->mode = (finfo.fattrib & AM_DIR) ? XSTAT_MODE_DIRECTORY : XSTAT_MODE_REGULAR;
x__exit:

    return err;
}


XError xfatfs_utime(XFatFs* fs, const char* path, XTime time)
{
    X_ASSERT(fs);
    X_ASSERT(path);
    X__ASSERT_TAG(fs);

    /* [TODO] settime */
    X__DECLARE_FINFO(finfo);
    XError err = X_ERR_NONE;

    finfo.fdate = time;
    finfo.ftime = time;

    const FRESULT fres = f_utime(path, &finfo);
    X__EXIT_IF(fres != FR_OK, X__ToXError(fres));

x__exit:

    return err;
}


static XError X__ToXError(FRESULT fres)
{
    XError err;
    switch (fres)
    {
        case FR_OK:
            err = X_ERR_NONE;         break;
        case FR_DISK_ERR:
            err = X_ERR_IO;           break;
        case FR_INT_ERR:
            err = X_ERR_INTERNAL;     break;
        case FR_NOT_READY:
        case FR_NOT_ENABLED:
            err = X_ERR_NOT_READY;    break;
        case FR_NO_FILE:
        case FR_NO_PATH:
            err = X_ERR_NO_ENTRY;     break;
        case FR_DENIED:
        case FR_WRITE_PROTECTED:
            err = X_ERR_ACCESS;       break;
        case FR_EXIST:
            err = X_ERR_EXIST;        break;
        case FR_INVALID_OBJECT:
        case FR_INVALID_DRIVE:
        case FR_INVALID_PARAMETER:
            err = X_ERR_INVALID;      break;
        case FR_INVALID_NAME:
            err = X_ERR_INVALID_NAME; break;
        case FR_MKFS_ABORTED:
        case FR_NO_FILESYSTEM:
            err = X_ERR_PROTOCOL;     break;
        case FR_TIMEOUT:
            err = X_ERR_TIMED_OUT;    break;
        case FR_LOCKED:
            err = X_ERR_BUSY;         break;
        case FR_NOT_ENOUGH_CORE:
            err = X_ERR_NO_MEMORY;    break;
        case FR_TOO_MANY_OPEN_FILES:
            err = X_ERR_MANY;         break;
        default:
            err = X_ERR_UNKNOWN;      break;
    }
    return err;
}
