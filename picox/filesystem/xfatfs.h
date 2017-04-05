/**
 *       @file  xfatfs.h
 *      @brief  FatFsをバックエンドとするファイル操作定義
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

#ifndef picox_filesystem_xfatfs_h_
#define picox_filesystem_xfatfs_h_


#include <picox/filesystem/xfscore.h>


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/** @addtogroup filesystem
 *  @{
 *  @addtogroup  xfatfs
 *  @brief FatFsをバックエンドとするファイルシステムモジュールです
 *
 *  組込み向けファイルシステムでお馴染みの
 *  [FatFs](http://elm-chan.org/fsw/ff/00index_j.html)をバックエンドとし、picox
 *  ファイルシステムインターフェースを実装しています。
 *
 *  FatFsを使用するために必要な下位ドライバ実装には関知していないので、新規ター
 *  ゲットへの移植時は、まずはFatFsを直接使用して、R/Wが正常に行えることを確認
 *  することからはじめてください。
 *
 *  @see xunionfs
 *  @see xvfs
 *  @{
 */

typedef struct
{
    const void* m_fstype_tag;
} XFatFs;
X_DECLEAR_RTTI_TAG(XFATFS_RTTI_TAG);


/** @brief ファイルシステムを初期化します
 *
 *  @pre
 *  + fs    != NULL
 *
 *  FatFsの本体を利用可能にするためには、別途f_mount()等の呼び出しが必要です。
 */
void xfatfs_init(XFatFs* fs);


/** @brief ファイルシステムの終了処理を行います
 *
 *  @pre
 *  + fs    != NULL
 */
void xfatfs_deinit(XFatFs* fs);


/** @brief 仮想ファイルシステムを初期化します
 *
 *  @pre
 *  + fs    != NULL
 *  + vfs   != NULL
 */
XVirtualFs* xfatfs_init_vfs(XFatFs* fs, XVirtualFs* vfs);
XStream* xfatfs_init_stream(XStream* stream, XFile* fp);
XError xfatfs_open(XFatFs* fs, const char* path, XOpenMode mode, XFile** o_fp);
XError xfatfs_close(XFile* fp);
XError xfatfs_read(XFile* fp, void* dst, size_t size, size_t* nread);
XError xfatfs_write(XFile* fp, const void* src, size_t size, size_t* nwritten);
XError xfatfs_seek(XFile* fp, XOffset pos, XSeekMode whence);
XError xfatfs_tell(XFile* fp, XSize* pos);
XError xfatfs_flush(XFile* fp);
XError xfatfs_mkdir(XFatFs* fs, const char* path);
XError xfatfs_opendir(XFatFs* fs, const char* path, XDir** o_dir);
XError xfatfs_readdir(XDir* dir, XDirEnt* dirent, XDirEnt** result);
XError xfatfs_closedir(XDir* dir);
XError xfatfs_chdir(XFatFs* fs, const char* path);
XError xfatfs_getcwd(XFatFs* fs, char* buf, size_t size);
XError xfatfs_remove(XFatFs* fs, const char* path);
XError xfatfs_rename(XFatFs* fs, const char* oldpath, const char* newpath);
XError xfatfs_stat(XFatFs* fs, const char* path, XStat* statbuf);
XError xfatfs_utime(XFatFs* fs, const char* path, XTime time);


/** @} end of addtogroup xfatfs
 *  @} end of addtogroup filesystem
 */


#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* picox_filesystem_xfatfs_h_ */
