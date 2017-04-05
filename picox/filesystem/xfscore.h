/**
 *       @file  xfscore.h
 *      @brief
 *
 *    @details
 *
 *     @author  MaskedW
 *
 *   @internal
 *     Created  2016/02/06/
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


#ifndef maskedw_filesystem_xfscore_h_
#define maskedw_filesystem_xfscore_h_


#include <picox/core/xcore.h>
#include <picox/filesystem/xfpath.h>


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/** @addtogroup filesystem
 *  @{
 *  @addtogroup  xfscore
 *  @brief ファイルシステム関連の共通定義
 *  @{
 */


/* 前方宣言 */
struct XVirtualFs;
typedef struct XVirtualFs   XVirtualFs;


/** @brief ファイル操作のハンドル構造体です
 *
 *  ユーザーが直接アクセスしてよいメンバはありません。
 */
typedef struct
{
/** @privatesection */
    void*       m_fs;
    XVirtualFs* m_vfs;
} XFile;


/** @brief ディレクトリ操作のハンドル構造体です
 *
 *  ユーザーが直接アクセスしてよいメンバはありません。
 */
typedef struct
{
/** @privatesection */
    void*       m_fs;
    XVirtualFs* m_vfs;
} XDir;


/** @brief ファイル情報を格納する構造体です
 */
typedef struct
{
/** @publicsection */

    /** @brief ファイルのタイムスタンプ */
    XTime   mtime;

    /** @brief ファイルのバイト数 */
    XSize   size;

    /** @brief ファイルの種別等、属性情報  */
    XMode   mode;
} XStat;


/** @brief ディレクトリエントリ情報を格納する構造体です
 */
typedef struct
{
/** @publicsection */

    /** @brief エントリのファイル名 */
    char name[X_NAME_MAX];
} XDirEnt;


/** @brief ディレクトリ探索関数が呼び出すコールバック関数型です
 *
 *  @retval true    ディレクトリ走査を続行する
 *  @retval false   ディレクトリ走査を中止する
 *
 */
typedef bool (*XFsTreeWalker)(void* userptr, const char* path, const XStat* statbuf, const XDirEnt* dirent);


/** @name filemode
 *  @brief ファイルの属性情報を判定するインターフェースを提供します
 *
 *  判定マクロの引数にはXStat.modeを使用してください。
 *
 *  @{
 */
#define XSTAT_MODE_TYPEMASK         (0x0F)
#define XSTAT_MODE_REGULAR          (0)
#define XSTAT_MODE_DIRECTORY        (1)

/** @brief ファイルが通常ファイルかどうかを判定します
 */
#define XSTAT_IS_REGULAR(mode)      (((mode) & XSTAT_MODE_TYPEMASK) == XSTAT_MODE_REGULAR)


/** @brief ファイルがディレクトリかどうかを判定します
 */
#define XSTAT_IS_DIRECTORY(mode)    (((mode) & XSTAT_MODE_TYPEMASK) == XSTAT_MODE_DIRECTORY)


/** @} end of name filemode
 */

/** @name virtual filesystem interface
 *  @brief 仮想ファイルシステムのインターフェース定義です
 *
 *  これらの定義が要求するインターフェースを実装することで、どんなファイルシステ
 *  ムもxunionfsにマウントできるようになります。
 *
 *  picoxで未提供のファイルシステムを仮想ファイルシステムのアダプトしたい場合、
 *  各関数の要求内容はxunionfsの関数説明を参照してください。
 *
 *  実装はxfatfs, xramfs等の実装を参考にしてください。
 */
typedef XError (*XVirtualFsOpenFunc)(void* fs, const char* path, XOpenMode mode, XFile** o_fp);
typedef XError (*XVirtualFsCloseFunc)(XFile* fp);
typedef XError (*XVirtualFsReadFunc)(XFile* fp, void* dst, size_t size, size_t* nread);
typedef XError (*XVirtualFsWriteFunc)(XFile* fp, const void* src, size_t size, size_t* nwritten);
typedef XError (*XVirtualFsSeekFunc)(XFile* fp, XOffset pos, XSeekMode whence);
typedef XError (*XVirtualFsTellFunc)(XFile* fp, XSize* pos);
typedef XError (*XVirtualFsFlushFunc)(XFile* fp);
typedef XError (*XVirtualFsMkdirFunc)(void* fs, const char* path);
typedef XError (*XVirtualFsOpendirFunc)(void* fs, const char* path, XDir** o_dir);
typedef XError (*XVirtualFsReaddirFunc)(XDir* dir, XDirEnt* dirent, XDirEnt** result);
typedef XError (*XVirtualFsClosedirFunc)(XDir* dir);
typedef XError (*XVirtualFsChdirFunc)(void* fs, const char* path);
typedef XError (*XVirtualFsGetcwdFunc)(void* fs, char* buf, size_t size);
typedef XError (*XVirtualFsRemoveFunc)(void* fs, const char* path);
typedef XError (*XVirtualFsRenameFunc)(void* fs, const char* oldpath, const char* newpath);
typedef XError (*XVirtualFsStatFunc)(void* fs, const char* path, XStat* statbuf);
typedef XError (*XVirtualFsUtimeFunc)(void* fs, const char* path, XTime time);


/** ファイルシステムインターフェースのvtableです
 */
typedef struct XVirtualFsVTable
{
    const char*                 m_name;
    XVirtualFsOpenFunc          m_open_func;
    XVirtualFsCloseFunc         m_close_func;
    XVirtualFsReadFunc          m_read_func;
    XVirtualFsWriteFunc         m_write_func;
    XVirtualFsSeekFunc          m_seek_func;
    XVirtualFsTellFunc          m_tell_func;
    XVirtualFsFlushFunc         m_flush_func;
    XVirtualFsMkdirFunc         m_mkdir_func;
    XVirtualFsOpendirFunc       m_opendir_func;
    XVirtualFsReaddirFunc       m_readdir_func;
    XVirtualFsClosedirFunc      m_closedir_func;
    XVirtualFsChdirFunc         m_chdir_func;
    XVirtualFsGetcwdFunc        m_getcwd_func;
    XVirtualFsRemoveFunc        m_remove_func;
    XVirtualFsRenameFunc        m_rename_func;
    XVirtualFsStatFunc          m_stat_func;
    XVirtualFsUtimeFunc         m_utime_func;
} XVirtualFsVTable;


/** 仮想ファイルシステムインターフェースを表すインターフェース型です
 */
struct XVirtualFs
{
    X_DECLEAR_RTTI(XVirtualFsVTable);
};


/** 仮想ファイルシステムを初期化します
 *
 *  関数テーブルにデフォルト関数をセットします。各ファイルシステムモジュール内の
 *  、vfs初期化関数で使用することを想定しているため、通常はユーザーが自身で呼び
 *  出す必要のない関数です。
 */
void xvfs_init(XVirtualFs* vfs);


X_DECLEAR_RTTI_TAG(XFILE_STREAM_RTTI_TAG);


/** @} end of name virtual filesystem interface
 */


/** @} end of addtogroup xfscore
 *  @} end of addtogroup filesystem
 */


#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* maskedw_filesystem_xfscore_h_ */
