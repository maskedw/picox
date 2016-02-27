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
#include <picox/filesystem/xflib.h>
#include <ff.h> /* fatfs */


#define X__ASSERT_TAG(p)    (X_ASSERT(((const XFatFs*)p)->m_tag == X_FATFS_TAG))

typedef struct
{
} X__FileStorage;


#define X__GET_REAL_FP(p)

// o_fp;
//
// XFile** o_fp, o_nread; o_nwritten,


XError xfatfs_init(XFatFs* fs)
{
    X_ASSERT(fs);
    fs->m_tag = X_FATFS_TAG;

    return X_ERR_NONE;
}


void xfatfs_deinit(XFatFs* fs)
{
    X_ASSERT(fs);
    X_UNUSED(fs);
}


XError xfatfs_open(XFatFs* fs, XFile* fp, const char* path, const char* mode)
{
    X_ASSERT(fs);
    X_ASSERT(fp);

    X_ASSERT(path);
    X_ASSERT(mode);

    bool ok = false;
    do
    {
        *err = FR_INVALID_PARAMETER;

        /* 最大長はw+b, r+b, a+bなので3文字 */
        const size_t len = strlen(mode);
        X_BREAK_IF(len > 3);
        X_BREAK_IF(len == 0);

        /* fatfsではb指定は無視 */
        char buf[4];
        strcpy(buf, mode);
        if (buf[len - 1] == 'b')
            buf[len - 1] = '\0';

        bool at_end = false;
        BYTE fsmode = 0;

        if x_strequal(buf, "r")
        {
            /*
             * + 読み込み可能
             * + ファイルなし時はエラー
             */
            /* 読み出し専用でファイルが存在しなければエラー　*/
            fsmode = FA_READ | FA_OPEN_EXISTING;
        }
        else if x_strequal(buf, "r+")
        {
            /*
             * + 読み書き可能
             * + ファイルなし時はエラー
             */
            fsmode = FA_READ | FA_WRITE | FA_OPEN_EXISTING;
        }
        else if x_strequal(buf, "w")
        {
            /*
             * + 書き込み可能
             * + 上書き
             * + ファイルがなければ新規作成
             */
            fsmode = FA_WRITE | FA_CREATE_ALWAYS;
        }
        else if x_strequal(buf, "w+")
        {
            /*
             * + 読み書き可能
             * + 上書き
             * + ファイルがなければ新規作成
             */
            fsmode = FA_READ | FA_WRITE | FA_CREATE_ALWAYS;
        }
        else if x_strequal(buf, "a")
        {
            /*
             * + 書き込み可能
             * + ファイルがなければ新規作成
             * + ファイルポインタは末尾
             */
            fsmode = FA_WRITE | FA_OPEN_ALWAYS;
            at_end = true;
        }
        else if x_strequal(buf, "a+")
        {
            /*
             * + 読み書き可能
             * + ファイルがなければ新規作成
             * + ファイルポインタは末尾
             */
            fsmode = FA_READ | FA_WRITE | FA_OPEN_ALWAYS;
            at_end = true;
        }
        else
        {
            break;
        }

        X__ASSIGN_ERR(FR_NOT_ENOUGH_CORE);
        fp = x_malloc(sizeof(FIL));
        X_BREAK_IF(! fp);

        FRESULT result = f_open(fp, path, fsmode);
        X__ASSIGN_ERR(result);
        X_BREAK_IF(result != FR_OK);

        if (at_end)
        {
            result = f_lseek(fp, f_size(fp));
            if (result != FR_OK)
            {
                X__ASSIGN_ERR(result);
                f_close(fp);
                break;
            }
        }

        ok = true;
    } while (0);

    return err;
}


XError xfatfs_close(XFile* fp)
{
    XError err = X_ERR_NONE;

    return err;
}


XError xfatfs_write(XFile* fp, const void* src, size_t size, size_t* nwritten)
{
    X_ASSERT(fp);
    X_ASSERT(fp->m_tag == X_FATFS_TAG);
    X_ASSERT(fp->m_file_handle);
    X_ASSERT(src);

    XError err = X_ERR_NONE;

    return err;
}


XError xfatfs_read(XFile* fp, void* dst, size_t size, size_t* nread)
{
    X_ASSERT(fp);
    X_ASSERT(fp->m_tag == X_FATFS_TAG);
    X_ASSERT(fp->m_file_handle);
    X_ASSERT(dst);

    XError err = X_ERR_NONE;

    return err;
}


void xfatfs_init_vfs(XFatFs* fs, XVirtualFs* vfs)
{
    vfs->m_fs_handle        = fs;
    vfs->m_open_func        = (XVirtualFsOpenFunc)xfatfs_open;
    vfs->m_close_func       = (XVirtualFsCloseFunc)xfatfs_close;
    vfs->m_read_func        = (XVirtualFsReadFunc)xfatfs_read;
    vfs->m_write_func       = (XVirtualFsWriteFunc)xfatfs_write;
    vfs->m_seek_func        = (XVirtualFsSeekFunc)xfatfs_seek;
    vfs->m_tell_func        = (XVirtualFsTellFunc)xfatfs_tell;
    vfs->m_flush_func       = (XVirtualFsFlushFunc)xfatfs_flush;
    vfs->m_mkdir_func       = (XVirtualFsMkdirFunc)xfatfs_mkdir;
    vfs->m_opendir_func     = (XVirtualFsOpendirFunc)xfatfs_opendir;
    vfs->m_readdir_func     = (XVirtualFsReaddirFunc)xfatfs_readdir;
    vfs->m_closedir_func    = (XVirtualFsClosedirFunc)xfatfs_closedir;
    vfs->m_chdir_func       = (XVirtualFsChdirFunc)xfatfs_chdir;
    vfs->m_getcwd_func      = (XVirtualFsGetcwdFunc)xfatfs_getcwd;
    vfs->m_remove_func      = (XVirtualFsRemoveFunc)xfatfs_remove;
    vfs->m_rename_func      = (XVirtualFsRenameFunc)xfatfs_rename;
    vfs->m_stat_func        = (XVirtualFsStatFunc)xfatfs_stat;
    vfs->m_utime_func       = (XVirtualFsUtimeFunc)xfatfs_utime;
}


XError xfatfs_seek(XFile* fp, XOffset pos, XSeekMode whence)
{
    X_ASSERT(fp);
    X_ASSERT(fp->m_file_handle);

    XError err = X_ERR_NONE;

    return err;
}


XError xfatfs_tell(XFile* fp, XSize* pos)
{
    X_ASSERT(fp);
    X_ASSERT(fp->m_file_handle);
    X_ASSERT(pos);

    XError err = X_ERR_NONE;

    return err;
}


XError xfatfs_flush(XFile* fp)
{
    X_ASSERT(fp);
    X_ASSERT(fp->m_file_handle);

    XError err = X_ERR_NONE;

    return err;
}


XError xfatfs_mkdir(XFatFs* fs, const char* path)
{
    X_ASSERT(fs);
    X_ASSERT(path);

    XError err = X_ERR_NONE;

    return err;
}


XError xfatfs_opendir(XFatFs* fs, XDir* dir, const char* path)
{
    X_ASSERT(fs);
    X_ASSERT(dir);

    XError err = X_ERR_NONE;

    return err;
}


XError xfatfs_readdir(XDir* dir, XDirEnt* dirent, XDirEnt** result)
{
    X_ASSERT(dir);
    X_ASSERT(dir->m_dir_handle);
    X_ASSERT(dirent);
    X_ASSERT(result);

    XError err = X_ERR_NONE;
    *result = NULL;

    return err;
}


XError xfatfs_closedir(XDir* dir)
{
    XError err = X_ERR_NONE;
    if (!dir)
        return err;

    X_ASSERT(dir->m_tag == X_FATFS_TAG);

    return err;
}


XError xfatfs_chdir(XFatFs* fs, const char* path)
{
    X_ASSERT(fs);
    X_ASSERT(path);

    XError err = X_ERR_NONE;

    return err;
}


XError xfatfs_getcwd(XFatFs* fs, char* buf, size_t size)
{
    X_ASSERT(fs);
    X_ASSERT(buf);

    return X_ERR_NONE;
}


XError xfatfs_remove(XFatFs* fs, const char* path)
{
    X_ASSERT(fs);
    X_ASSERT(path);

    XError err = X_ERR_NONE;

    return err;
}


XError xfatfs_rename(XFatFs* fs, const char* oldpath, const char* newpath)
{
    X_ASSERT(fs);
    X_ASSERT(oldpath);
    X_ASSERT(newpath);

    XError err = X_ERR_NONE;

    return err;
}


XError xfatfs_stat(XFatFs* fs, XStat* statbuf, const char* path)
{
    X_ASSERT(fs);
    X_ASSERT(statbuf);
    X_ASSERT(path);

    XError err = X_ERR_NONE;

    return err;
}


XError xfatfs_utime(XFatFs* fs, const char* path, XTime time)
{
    X_ASSERT(fs);
    X_ASSERT(path);

    XError err = X_ERR_NONE;

    return err;
}
