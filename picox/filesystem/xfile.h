/**
 *       @file  xfile.h
 *      @brief  XFile interface wrapper
 *
 *    @details
 *    XFileSystemによる違いや、扱いにくいインターフェースをラップするモジュールで
 *    す。
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


#ifndef picox_xfile_h_
#define picox_xfile_h_


#include <stdbool.h>
#include <stdlib.h>
#include <stdarg.h>


#define XFILE_ENGINE_FATFS       (0)
#define XFILE_ENGINE_POSIX       (1)

/** ファイルエンジンの種類を指定します。
 *
 *  0: FatFs(http://elm-chan.org/fsw/ff/00index_j.html)
 *  1: POSIX(fopen, fcloseとか)
 */
#ifndef XFILE_ENGINE_TYPE

    #define XFILE_ENGINE_TYPE   XFILE_ENGINE_POSIX

#endif


#if XFILE_ENGINE_TYPE == XFILE_ENGINE_FATFS

    #include <ff.h>

#elif XFILE_ENGINE_TYPE == XFILE_ENGINE_POSIX

    /*
     * MinGWのgcc4.8のヘッダにはバグ(Typo)があり、以下のマクロを除去することで
     * off_t絡みのバグを回避する。
     */
    #if  defined(__MINGW32__) || defined(__MINGW64__) && (__GNUC__ == 4) && (__GNUC_MINOR__ == 8)
        #undef __STRICT_ANSI__
    #endif
    #include <dirent.h>
    #include <stdio.h>
    #include <stdlib.h>

#endif


#ifdef __cplusplus
extern "C" {
#endif


#if XFILE_ENGINE_TYPE == 0

    typedef FIL     XFile;
    typedef DIR     Dir;

    #ifndef XFILE_ABORT_LOG
        #define XFILE_ABORT_LOG(fmt, ...)    for(;;)
    #endif

#elif XFILE_ENGINE_TYPE == 1

    typedef FILE    XFile;
    typedef DIR     Dir;

    #ifndef XFILE_ABORT_LOG
        #define XFILE_ABORT_LOG(fmt, ...) do { fprintf(stderr, fmt, __VA_ARGS__); exit(1); } while (0)
    #endif

#endif

#ifndef XFILE_ASSERT

    #define XFILE_ASSERT(expr)   do { if (! expr) for(;;); } while (0)

#endif

#ifndef XFILE_WORKING_BUF_SIZE

    #define XFILE_WORKING_BUF_SIZE 256

#endif

#ifndef XFILE_ABORT_FAIL

    #define XFILE_ABORT_FAIL(x...)                                          \
    do {                                                                   \
        if (! x)                                                           \
        {                                                                  \
            XFILE_ABORT_LOG("[Error] %s:%s\n", #x, xfile_strerror(ferr));    \
        }                                                                  \
    } while (0)

#endif

#ifndef XFILE_MEM_ALLOC

    #define XFILE_MEM_ALLOC(size)    malloc(size)

#endif

#ifndef XFILE_MEM_FREE

    #define XFILE_MEM_FREE(ptr)      free(ptr)

#endif


/** 指定パスをstd::fopen()相当のmode指定でopenします。
 */
XFile* xfile_open(const char* path, const char* mode, int* err);


/** std::fclose()相当の処理を行います。
 */
bool xfile_close(XFile* fp, int* err);


/** sizeバイトをfpに書き出します。
 */
bool xfile_write(XFile* fp, const void* src, size_t size, size_t* nwritten, int* err);


/** sizeバイトをfpから読み出します。
 */
bool xfile_read(XFile* fp, void* dst, size_t size, size_t* nread, int* err);


/** ファイルサイズを返します。
 */
bool xfile_size(XFile* fp, size_t* dst, int* err);


/** pathからファイルサイズを返します。
 */
bool xfile_size_with_path(const char* path, size_t* dst, int* err);


/** patuが存在するかどうかを返します。
 */
bool xfile_exists(const char* path, int* err);


/** ファイルポインタが終端かどうかを返します。
 */
bool xfile_eof(XFile* fp);


/** ファイルポインタをposに移動します。
 */
bool xfile_seek(XFile* fp, size_t pos, int* err);


/** バッファをフラッシュします。
 */
bool xfile_flush(XFile* fp, int* err);


/** カレントディレクトリをpathに移動します。
 */
bool xfile_chdir(const char* path, int* err);


/** カレントディレクトリを返します。
 */
bool xfile_cwd(char* dst, size_t size, int* err);


/** pathを削除します。
 */
bool xfile_remove(const char* path, int* err);


/** path以下を全て削除します。
 */
bool xfile_remove_all(const char* path, int* err);


/** ファイルから1行分のデータを取得します。
 *
 *  行データには改行文字自身も含まれます。
 */
bool xfile_read_line(XFile* fp, char* dst, char** result, size_t size, int* err);


/** エラーコードの文字列表現を返します。
 */
const char* xfile_strerror(int err);


/** pathがディレクトリかどうかを返します。
 */
bool xfile_is_directory(const char* path, int* err);


/** ファイルにprintf相当の書き込みを行います。
 */
bool xfile_printf(XFile* fp, size_t* nprint, int* err, const char* fmt, ...);


/** ファイルにvprintf相当の書き込みを行います。
 */
bool xfile_vprintf(XFile* fp, size_t* nprint, int* err, const char* fmt, va_list arg);


/** パスの末尾を表す文字列をdstにコピーします。
 *
 * @note
 * /foo/bar/baz.txt => baz.txt<br>
 * foo              => foo<br>
 */
char* xfile_path_name(char* dst, const char* path, size_t size);


#ifdef __cplusplus
}
#endif


#endif // picox_xfile_h_
