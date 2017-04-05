/**
 *       @file  xposixfs.h
 *      @brief  POSIX APIによるファイル操作定義
 *
 *    @details
 *
 *     @author  MaskedW
 *
 *   @internal
 *     Created  2016/01/30
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

#ifndef picox_filesystem_xposixfs_h_
#define picox_filesystem_xposixfs_h_


#include <picox/filesystem/xfscore.h>


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/** @addtogroup filesystem
 *  @{
 *  @addtogroup  xposixfs
 *  @brief POSIX APIを使用するファイルシステムモジュールです
 *
 *  C標準ライブラリではFILE*によるファイル操作は定義されていますが、ディレクトリ
 *  操作はC標準では定義されておらず、POSIXの範疇となります。
 *  このモジュールはPOSIX APIをバックエンドとし、picoxファイルシステムインターフ
 *  ェースを実装しています。
 *
 *  このモジュールの目的は以下の通りです。
 *  + 組込みLinuxでもpicoxインターフェースを使用可能にする。
 *  + ifdef等でxposixfsとxfatfs等の組込み向けファイルシステムと切替を行い、ホス
 *    トPC上でのデバッグを行いやすくする。
 *
 *  @see xunionfs
 *  @see xvfs
 *  @{
 */


typedef struct
{
    const void* m_fstype_tag;
    XTag    m_tag;
} XPosixFs;
X_DECLEAR_RTTI_TAG(XPOSIXFS_RTTI_TAG);


/** @brief ファイルシステムを初期化します
 *
 *  @pre
 *  + fs    != NULL
 */
void xposixfs_init(XPosixFs* fs);


/** @brief ファイルシステムの終了処理を行います
 *
 *  @pre
 *  + fs    != NULL
 */
void xposixfs_deinit(XPosixFs* fs);


/** @brief 仮想ファイルシステムを初期化します
 *
 *  @pre
 *  + fs    != NULL
 *  + vfs   != NULL
 */
XVirtualFs* xposixfs_init_vfs(XPosixFs* fs, XVirtualFs* vfs);
XError xposixfs_open(XPosixFs* fs, const char* path, XOpenMode mode, XFile** o_fp);
XError xposixfs_close(XFile* fp);
XError xposixfs_read(XFile* fp, void* dst, size_t size, size_t* nread);
XError xposixfs_write(XFile* fp, const void* src, size_t size, size_t* nwritten);
XError xposixfs_seek(XFile* fp, XOffset pos, XSeekMode whence);
XError xposixfs_tell(XFile* fp, XSize* pos);
XError xposixfs_flush(XFile* fp);
XError xposixfs_mkdir(XPosixFs* fs, const char* path);
XError xposixfs_opendir(XPosixFs* fs, const char* path, XDir** o_dir);
XError xposixfs_readdir(XDir* dir, XDirEnt* dirent, XDirEnt** result);
XError xposixfs_closedir(XDir* dir);
XError xposixfs_chdir(XPosixFs* fs, const char* path);
XError xposixfs_getcwd(XPosixFs* fs, char* buf, size_t size);
XError xposixfs_remove(XPosixFs* fs, const char* path);
XError xposixfs_rename(XPosixFs* fs, const char* oldpath, const char* newpath);
XError xposixfs_stat(XPosixFs* fs, const char* path, XStat* statbuf);
XError xposixfs_utime(XPosixFs* fs, const char* path, XTime time);
XError xposixfs_rmtree(XPosixFs* fs, const char* path);


/** @} end of addtogroup xposixfs
 *  @} end of addtogroup filesystem
 */


#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* picox_filesystem_xposixfs_h_ */
