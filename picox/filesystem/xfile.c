/**
 *       @file  xfile.c
 *      @brief
 *
 *    @details
 *
 *     @author  MaskedW
 *
 *   @internal
 *     Created  2015/06/14
 * ===================================================================
 */

/*
 * License: MIT license
 * Copyright (c) <2015> <MaskedW [maskedw00@gmail.com]>
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

#include <picox/filesystem/xfile.h>


#if XFILE_ENGINE_TYPE == XFILE_ENGINE_FATFS
    #if _USE_LFN == 1
        static char lfn[_MAX_LFN + 1];
        #define X__DECLARE_FINFO(name)     FILINFO name; name.lfname = lfn; name.fsize = sizeof(lfn)
    #else
        #define X__DECLARE_FINFO(name)     FILINFO name
    #endif // #if _USE_LFN

    #ifndef XFILE_FATFS_HAS_CLOSE_DIR
        #define XFILE_FATFS_HAS_CLOSE_DIR 1
    #endif

    #if XFILE_FATFS_HAS_CLOSE_DIR == 0
        #define f_closedir(dir)         FR_OK
    #endif

    static FRESULT X__RemoveFiles(char* path, int size, int tail, FILINFO* finfo);
    static const char* X__StrFResult(FRESULT fres);


#elif XFILE_ENGINE_TYPE == XFILE_ENGINE_POSIX

    #include <sys/stat.h>
    #include <errno.h>
    #include <unistd.h>
    static bool X__RemoveFiles(char* path, int size, int tail, struct stat* statbuf);
#endif


#define X__ASSIGN_ERR(v)               ((err) ? (*err = v) : (void)0)
#define X__ASSIGN_IF(x, v)             ((x) ? (*x = v) : (void)0)
#define X__EQUAL_STR(s1, s2)           (strcmp(s1, s2) == 0)

XFile* xfile_open(const char* path, const char* mode, int* err)
{
    X_ASSERT(path);
    X_ASSERT(mode);

    XFile* fp = NULL;
    int sub_err;
    if (! err) err = &sub_err;

#if XFILE_ENGINE_TYPE == XFILE_ENGINE_FATFS

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

        if X__EQUAL_STR(buf, "r")
        {
            /*
             * + 読み込み可能
             * + ファイルなし時はエラー
             */
            /* 読み出し専用でファイルが存在しなければエラー　*/
            fsmode = FA_READ | FA_OPEN_EXISTING;
        }
        else if X__EQUAL_STR(buf, "r+")
        {
            /*
             * + 読み書き可能
             * + ファイルなし時はエラー
             */
            fsmode = FA_READ | FA_WRITE | FA_OPEN_EXISTING;
        }
        else if X__EQUAL_STR(buf, "w")
        {
            /*
             * + 書き込み可能
             * + 上書き
             * + ファイルがなければ新規作成
             */
            fsmode = FA_WRITE | FA_CREATE_ALWAYS;
        }
        else if X__EQUAL_STR(buf, "w+")
        {
            /*
             * + 読み書き可能
             * + 上書き
             * + ファイルがなければ新規作成
             */
            fsmode = FA_READ | FA_WRITE | FA_CREATE_ALWAYS;
        }
        else if X__EQUAL_STR(buf, "a")
        {
            /*
             * + 書き込み可能
             * + ファイルがなければ新規作成
             * + ファイルポインタは末尾
             */
            fsmode = FA_WRITE | FA_OPEN_ALWAYS;
            at_end = true;
        }
        else if X__EQUAL_STR(buf, "a+")
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
        fp = X_MALLOC(sizeof(FIL));
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

    if (! ok)
    {
        X_FREE(fp);
        fp = NULL;
    }

#elif XFILE_ENGINE_TYPE == XFILE_ENGINE_POSIX

    fp = fopen(path, mode);
    X__ASSIGN_ERR(errno);

#endif

    return fp;
}


bool xfile_close(XFile* fp, int* err)
{
    if (!fp)
        return true;

#if XFILE_ENGINE_TYPE == XFILE_ENGINE_FATFS

    const FRESULT result = f_close(fp);
    X_FREE(fp);
    const bool ok = (result == FR_OK);
    X__ASSIGN_ERR(result);

#elif XFILE_ENGINE_TYPE == XFILE_ENGINE_POSIX

    const int result = fclose(fp);
    const bool ok = (result == 0);
    X__ASSIGN_ERR(errno);

#endif
    return ok;
}


bool xfile_write(XFile* fp, const void* src, size_t size, size_t* nwritten, int* err)
{
    X_ASSERT(fp);
    X_ASSERT(src);

#if XFILE_ENGINE_TYPE == XFILE_ENGINE_FATFS
    const char* p = src;
    FRESULT result = FR_OK;

    while (size)
    {
        const UINT to_write = (size > UINT_MAX) ? UINT_MAX : size;
        UINT written;
        result = f_write(fp, p, to_write, &written);

        X_BREAK_IF(result != FR_OK);

        size -= to_write;
        p    += to_write;

        /* Disk full チェック */
        X_BREAK_IF(to_write != written);
    }
    X__ASSIGN_ERR(result);
    X__ASSIGN_IF(nwritten, p - (const char*)src);
    const bool ok = (result == FR_OK);

#elif XFILE_ENGINE_TYPE == XFILE_ENGINE_POSIX

    clearerr(fp);
    const size_t ret = fwrite(src, 1, size, fp);
    X__ASSIGN_IF(nwritten, ret);
    X__ASSIGN_ERR(errno);
    const bool ok = (ferror(fp) == 0);

#endif

    return ok;
}


bool xfile_read(XFile* fp, void* dst, size_t size, size_t* nread, int* err)
{
    X_ASSERT(fp);
    X_ASSERT(dst);

#if XFILE_ENGINE_TYPE == XFILE_ENGINE_FATFS
    char* p = dst;
    FRESULT result = FR_OK;
    size_t total = 0;

    while (size)
    {
        const UINT to_read = (size > UINT_MAX) ? UINT_MAX : size;
        UINT read;
        result = f_read(fp, p, to_read, &read);

        X_BREAK_IF(result != FR_OK);

        size    -= to_read;
        p       += to_read;
        total   += to_read;

        /* ファイル終端チェック */
        X_BREAK_IF(to_read != read);
    }
    X__ASSIGN_ERR(result);
    X__ASSIGN_IF(nread, total);
    const bool ok = (result == FR_OK);

#elif XFILE_ENGINE_TYPE == XFILE_ENGINE_POSIX

    clearerr(fp);
    const size_t ret = fread(dst, 1, size, fp);
    X__ASSIGN_IF(nread, ret);
    X__ASSIGN_ERR(errno);
    const bool ok = (ferror(fp) == 0);

#endif

    return ok;
}


bool xfile_size(XFile* fp, XFileSize* dst, int* err)
{
    X_ASSERT(fp);
    X_ASSERT(dst);

#if XFILE_ENGINE_TYPE == XFILE_ENGINE_FATFS
    *dst = f_size(fp);
    X__ASSIGN_ERR(FR_OK);

    const bool ok = true;

#elif XFILE_ENGINE_TYPE == XFILE_ENGINE_POSIX
    long save_pos;
    long seek_ret;
    bool ok = false;
    long size = 0;

    do
    {
        save_pos = ftell(fp);
        X_BREAK_IF(save_pos == -1);

        seek_ret = fseek(fp, 0, SEEK_END);
        X_BREAK_IF(seek_ret != 0);

        size = ftell(fp);
        X_BREAK_IF(size == -1);

        seek_ret = fseek(fp, save_pos, SEEK_SET);
        X_BREAK_IF(seek_ret != 0);

        *dst = size;
        ok = true;

    } while (0);

    X__ASSIGN_ERR(errno);
#endif

    return ok;
}


bool xfile_size_with_path(const char* path, XFileSize* dst, int* err)
{
    X_ASSERT(path);
    X_ASSERT(dst);

    bool ok = false;
    XFile* fp = NULL;

    do
    {
        fp = xfile_open(path, "r", err);
        X_BREAK_IF(! fp);

        ok = xfile_size(fp, dst, err);
    } while (0);

    if (fp)
        (void)xfile_close(fp, NULL);

    if (! ok)
        *dst = 0;

    return ok;
}


bool xfile_exists(const char* path, bool* exists, int* err)
{
    X_ASSERT(path);
    X_ASSERT(exists);

#if XFILE_ENGINE_TYPE == XFILE_ENGINE_FATFS

    X__DECLARE_FINFO(finfo);
    const FRESULT result = f_stat(path, &finfo);
    const bool ok = ((result == FR_OK) || (result == FR_NO_PATH));
    *exists = (result == FR_OK);

    X__ASSIGN_ERR(result);

#elif XFILE_ENGINE_TYPE == XFILE_ENGINE_POSIX

    struct stat statbuf;
    errno = 0;
    const int result = stat(path, &statbuf);
    const bool ok = ((result == 0) || (errno == ENOENT));
    *exists = (result == 0);
    X__ASSIGN_ERR(errno);

#endif

    return ok;
}


bool xfile_seek(XFile* fp, XFileSize pos, int* err)
{
    X_ASSERT(fp);

#if XFILE_ENGINE_TYPE == XFILE_ENGINE_FATFS

    FRESULT result = f_lseek(fp, pos);
    const bool ok = (result == FR_OK);
    X__ASSIGN_ERR(result);

#elif XFILE_ENGINE_TYPE == XFILE_ENGINE_POSIX

    const int result = fseek(fp, pos, SEEK_SET);
    const bool ok = (result != -1);

    X__ASSIGN_ERR(errno);

#endif

    return ok;
}


bool xfile_tell(XFile* fp, XFileSize* pos, int* err)
{
    X_ASSERT(fp);
    X_ASSERT(pos);

#if XFILE_ENGINE_TYPE == XFILE_ENGINE_FATFS

    *pos = f_tell(fp);
    const bool ok = true;
    X__ASSIGN_ERR(FR_OK);

#elif XFILE_ENGINE_TYPE == XFILE_ENGINE_POSIX

    *pos = ftell(fp);
    const bool ok = true;
    X__ASSIGN_ERR(0);

#endif

    return ok;
}


bool xfile_flush(XFile* fp, int* err)
{
    X_ASSERT(fp);

#if XFILE_ENGINE_TYPE == XFILE_ENGINE_FATFS

    const FRESULT result = f_sync(fp);
    const bool ok = (result == FR_OK);
    X__ASSIGN_ERR(result);

#elif XFILE_ENGINE_TYPE == XFILE_ENGINE_POSIX
    const int result = fflush(fp);
    const bool ok = (result == 0);
    X__ASSIGN_ERR(errno);

#endif

    return ok;
}


bool xfile_mkdir(const char* path, int* err)
{
    X_ASSERT(path);
#if XFILE_ENGINE_TYPE == XFILE_ENGINE_FATFS

#elif XFILE_ENGINE_TYPE == XFILE_ENGINE_POSIX

    const int result = mkdir(path);
    const bool ok = (result == 0);
    X__ASSIGN_ERR(errno);

#endif

    return ok;
}



bool xfile_chdir(const char* path, int* err)
{
    X_ASSERT(path);

#if XFILE_ENGINE_TYPE == XFILE_ENGINE_FATFS

    const FRESULT result = f_chdir(path);
    const bool ok = (result == FR_OK);
    X__ASSIGN_ERR(result);

#elif XFILE_ENGINE_TYPE == XFILE_ENGINE_POSIX

    const int result = chdir(path);
    const bool ok = (result == 0);
    X__ASSIGN_ERR(errno);

#endif
    return ok;
}


bool xfile_cwd(char* dst, size_t size, int* err)
{
    X_ASSERT(dst);
    X_ASSERT(size > 0);

#if XFILE_ENGINE_TYPE == XFILE_ENGINE_FATFS

    const FRESULT result = f_getcwd(dst, size);
    const bool ok = (result == FR_OK);
    X__ASSIGN_ERR(result);

#elif XFILE_ENGINE_TYPE == XFILE_ENGINE_POSIX

    const char* result = getcwd(dst, size);
    const bool ok = (result != NULL);
    X__ASSIGN_ERR(errno);

#endif
    return ok;
}


bool xfile_remove(const char* path, int* err)
{
    X_ASSERT(path);
    bool ok;
    bool exists;
    ok = xfile_exists(path, &exists, err);
    if (ok && (!exists))
        return true;

#if XFILE_ENGINE_TYPE == XFILE_ENGINE_FATFS

    const FRESULT result = f_unlink(path);
    ok = (result == FR_OK);
    X__ASSIGN_ERR(result);

#elif XFILE_ENGINE_TYPE == XFILE_ENGINE_POSIX

    struct stat statbuf;
    ok = false;

    do
    {
        X_BREAK_IF(stat(path, &statbuf) != 0);

        if (S_ISDIR(statbuf.st_mode))
        {
            X_BREAK_IF(rmdir(path) != 0);
        }
        else
        {
            X_BREAK_IF(unlink(path) != 0);
        }

        ok = true;
    } while (0);

    X__ASSIGN_ERR(errno);

#endif
    return ok;
}


bool xfile_remove_all(const char* path, int* err)
{
    X_ASSERT(path);
    X_ASSERT(strlen(path) <= XFILE_PATH_MAX);

    bool ok;
    bool exists;
    ok = xfile_exists(path, &exists, err);
    if (ok && (!exists))
        return true;


#if XFILE_ENGINE_TYPE == XFILE_ENGINE_FATFS

    X__DECLARE_FINFO(finfo);
    char* buf = X_MALLOC(XFILE_PATH_MAX + 1);
    if (! buf)
    {
        X__ASSIGN_ERR(FR_NOT_ENOUGH_CORE);
        return false;
    }

    strcpy(buf, path);
    const int tail = strlen(buf);
    const FRESULT result = X__RemoveFiles(buf, XFILE_PATH_MAX + 1, tail, &finfo);
    ok = (result == FR_OK);
    X__ASSIGN_ERR(result);

#elif XFILE_ENGINE_TYPE == XFILE_ENGINE_POSIX

    struct stat statbuf;
    char* buf = X_MALLOC(XFILE_PATH_MAX + 1);
    if (! buf)
    {
        X__ASSIGN_ERR(ENOMEM);
        return false;
    }

    strcpy(buf, path);
    const int tail = strlen(buf);

    ok = X__RemoveFiles(buf, XFILE_PATH_MAX + 1, tail, &statbuf);
    X__ASSIGN_ERR(errno);

#endif

    if (buf)
        X_FREE(buf);

    return ok;
}


bool xfile_read_line(XFile* fp, char* dst, size_t size, char** result, bool* overflow, int* err)
{
    X_ASSERT(fp);
    X_ASSERT(dst);
    X_ASSERT(result);
    X_ASSERT(overflow);
    X_ASSERT(size > 1);

#if XFILE_ENGINE_TYPE == XFILE_ENGINE_FATFS

    FRESULT fres = FR_OK;
    size_t total = 0;

    *result = NULL;
    dst[0] = '\0';

    while (total < size - 1)
    {
        UINT nread;
        char c;

        fres = f_read(fp, &c, 1, &nread);
        X_BREAK_IF(fres != FR_OK);
        X_BREAK_IF(nread != 1);

        X_CONTINUE_IF(c == '\r');
        X_BREAK_IF(c == '\n');
        dst[total++] = c;
    }
    dst[total] = '\0';
    *result = (dst[0] != '\0') ? dst : NULL;
    *overflow = ((c != '\n') && (!!f_eof(fp)));

    const bool ok = (fres == FR_OK);
    X__ASSIGN_ERR(fres);

#elif XFILE_ENGINE_TYPE == XFILE_ENGINE_POSIX

    clearerr(fp);
    *result = fgets(dst, size, fp);
    X__ASSIGN_ERR(errno);

    const bool ok = (ferror(fp) == 0);

    /* 改行文字は取り除く。*/
    if (result)
    {
        bool has_crlf = false;
        const size_t len = strlen(dst);
        if ((len >= 2) && (dst[len - 2] == '\r'))
        {
            if (dst[len - 1] == '\n')
                has_crlf = true;
            dst[len - 2] = '\0';
        }
        else if ((len >= 1) && (dst[len - 1] == '\n'))
        {
            has_crlf = true;
            dst[len - 1] = '\0';
        }

        if (has_crlf)
            *overflow = !!feof(fp);
    }
#endif

    return ok;
}

const char* xfile_strerror(int err)
{
    const char* ret;

#if XFILE_ENGINE_TYPE == XFILE_ENGINE_FATFS

    ret = X__StrFResult(err);

#elif XFILE_ENGINE_TYPE == XFILE_ENGINE_POSIX
    /*
     * 本当はstrerrorが返す文字列はconstではない(man strerror参照)。
     * しかし、インターフェースをstrerror_r相当にすると、使い勝手が大きく損なわ
     * れてしまう。現在の実装ではマルチスレッド環境で上書きの危険性が存在する
     * が、この関数はエラー時の簡易的なエラーレポートに利用することを意図してい
     * るので、そこまで厳密性は重視していない。よってこれでよしとする。
     */
    ret = strerror(err);
#endif

    return ret;
}


bool xfile_is_directory(const char* path, bool* is_dir, int* err)
{
    X_ASSERT(path);
    X_ASSERT(is_dir);

#if XFILE_ENGINE_TYPE == XFILE_ENGINE_FATFS

    X__DECLARE_FINFO(finfo);
    const FRESULT result = f_stat(path, &finfo);
    const bool ok = ((result == FR_OK) || (result == FR_NO_PATH));
    *is_dir = (ok && (finfo.fattrib & AM_DIR));

    X__ASSIGN_ERR(result);

#elif XFILE_ENGINE_TYPE == XFILE_ENGINE_POSIX

    struct stat statbuf;
    errno = 0;
    const int result = stat(path, &statbuf);
    const bool ok = ((result == 0) || (errno == ENOENT));
    *is_dir = (ok && (S_ISDIR(statbuf.st_mode)));

    X__ASSIGN_ERR(errno);

#endif

    return ok;
}


bool xfile_printf(XFile* fp, size_t* nprint, int* err, const char* fmt, ...)
{
    va_list arg;

    va_start(arg, fmt);
    const bool result = xfile_vprintf(fp, nprint, err, fmt, arg);
    va_end(arg);

    return result;
}


bool xfile_vprintf(XFile* fp, size_t* nprint, int* err, const char* fmt, va_list arg)
{
    X_ASSERT(fp);
    X_ASSERT(fmt);

    char* buf = NULL;
    X__ASSIGN_IF(nprint, 0);
    X__ASSIGN_ERR(0);

    bool ok = false;
    do
    {
        const int vsret  = vsnprintf(buf, 0, fmt, arg);
        X_BREAK_IF(vsret < 0);

        buf = X_MALLOC(vsret + 1);

#if XFILE_ENGINE_TYPE == XFILE_ENGINE_FATFS
        X__ASSIGN_ERR(FR_NOT_ENOUGH_CORE);
#elif XFILE_ENGINE_TYPE == XFILE_ENGINE_POSIX
        X__ASSIGN_ERR(ENOMEM);
#endif
        X_BREAK_IF(! buf);

        vsnprintf(buf, vsret + 1, fmt, arg);

        size_t nwritten;
        X_BREAK_IF(! xfile_write(fp, buf, vsret, &nwritten, err));

        *nprint = nwritten;
        ok = true;
    } while (0);

    if (buf)
        X_FREE(buf);

    return ok;
}


char* xfile_path_name(char* dst, const char* path, size_t size)
{
    X_ASSERT(dst);
    X_ASSERT(path);
    X_ASSERT(size > 0);

    const char* p = strrchr(path, '/');
    const char* name = p ? p + 1 : path;
    strncpy(dst, name, size);

    return dst;
}


#if XFILE_ENGINE_TYPE == XFILE_ENGINE_FATFS


static FRESULT X__RemoveFiles(char* path, int size, int tail, FILINFO* finfo)
{
    XDir dir;
    FRESULT fret;

    do
    {
        X_BREAK_IF((fret = f_stat(path, finfo)) != FR_OK);

        if (finfo->fattrib & AM_DIR)
        {
            X_BREAK_IF((fret = f_opendir(&dir, path)) != FR_OK);
            for (;;)
            {
                X_BREAK_IF((fret = f_readdir(&dir, finfo)) != FR_OK);

                /* ディレクトリ要素の終端 */
                X_BREAK_IF(finfo->fname[0] == '\0');

                if (X__EQUAL_STR(".", finfo->fname) || X__EQUAL_STR("..", finfo->fname))
                    continue;
#if _USE_LFN
                const int ret = snprintf(&path[tail], size - tail, "/%s",
                              (*(finfo->lfname)) ? finfo->lfname : finfo->fname);
#else
                const int ret = snprintf(&path[tail], size - tail, "/%s", finfo->fname);
#endif
                X_BREAK_IF(ret == -1);

                if (ret > size - tail)
                {
                    fret = FR_NOT_ENOUGH_CORE;
                    break;
                }

                /* 再帰呼び出し */
                X_BREAK_IF((fret = X__RemoveFiles(path, size, tail + ret, finfo)) != FR_OK);
            }

            if (fret != FR_OK)
            {
                (void)f_closedir(&dir);
                break;
            }

            fret = f_closedir(&dir);
            X_BREAK_IF(fret != FR_OK);

            path[tail]  = '\0';
            X_BREAK_IF((fret = f_unlink(path)) != FR_OK);
        }
        else
        {
            X_BREAK_IF((fret = f_unlink(path)) != FR_OK);
        }
    } while (0);

    return fret;
}

static const char* X__StrFResult(FRESULT fres)
{
    switch (fres)
    {
        case FR_OK:
            return "FR_OK";
        case FR_DISK_ERR:
            return "FR_DISK_ERR";
        case FR_INT_ERR:
            return "FR_INT_ERR";
        case FR_NOT_READY:
            return "FR_NOT_READY";
        case FR_NO_FILE:
            return "FR_NO_FILE";
        case FR_NO_PATH:
            return "FR_NO_PATH";
        case FR_INVALID_NAME:
            return "FR_INVALID_NAME";
        case FR_DENIED:
            return "FR_DENIED";
        case FR_EXIST:
            return "FR_EXIST";
        case FR_INVALID_OBJECT:
            return "FR_INVALID_OBJECT";
        case FR_WRITE_PROTECTED:
            return "FR_WRITE_PROTECTED";
        case FR_INVALID_DRIVE:
            return "FR_INVALID_DRIVE";
        case FR_NOT_ENABLED:
            return "FR_NOT_ENABLED";
        case FR_NO_FILESYSTEM:
            return "FR_NO_FILESYSTEM";
        case FR_MKFS_ABORTED:
            return "FR_MKFS_ABORTED";
        case FR_TIMEOUT:
            return "FR_TIMEOUT";
        case FR_LOCKED:
            return "FR_LOCKED";
        case FR_NOT_ENOUGH_CORE:
            return "FR_NOT_ENOUGH_CORE";
        case FR_TOO_MANY_OPEN_FILES:
            return "FR_TOO_MANY_OPEN_FILES";
        case FR_INVALID_PARAMETER:
            return "FR_INVALID_PARAMETER";
        default:
            return "UNKNOWN FRESULT CODE";
    }
}

#elif XFILE_ENGINE_TYPE == XFILE_ENGINE_POSIX


static bool X__RemoveFiles(char* path, int size, int tail, struct stat* statbuf)
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

            errno = 0;
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

                if (X__EQUAL_STR(".", ent->d_name) || X__EQUAL_STR("..", ent->d_name))
                    continue;

                ret = snprintf(&path[tail], size - tail, "/%s", ent->d_name);

                X_BREAK_IF(ret == -1);

                /*
                 * snprintfはbufサイズに空きがなかった場合も、本来書き込めたであ
                 * ろう文字数を返す。 よって、bufサイズより戻り値が大きかった場
                 * 合、スペースが足りなかったことを意味する。
                 */
                if (ret > size - tail)
                {
                    errno = ENOMEM;
                    break;
                }

                /* 再帰呼び出し */
                X_BREAK_IF(! X__RemoveFiles(path, size, tail + ret, statbuf));
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


#endif // if XFILE_ENGINE_TYPE
