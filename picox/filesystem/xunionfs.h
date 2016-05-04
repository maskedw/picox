/**
 *       @file  xunionfs.h
 *      @brief
 *
 *    @details
 *
 *     @author  MaskedW
 *
 *   @internal
 *     Created  2016/02/06
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

#ifndef picox_filesystem_xunionfs_h_
#define picox_filesystem_xunionfs_h_


#include <picox/filesystem/xvfs.h>


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/** @addtogroup filesystem
 *  @{
 *  @addtogroup  xunionfs
 *  @brief 複数のファイルシステムを透過的に扱うインターフェースを提供します
 *
 *  Unix系のシステムが"/"以下に異なるファイルシステムを次々にマウントして、一つ
 *  のディレクトリツリーを構成するのと同じアプローチです。
 *  これにより、ファイルシステムをまたいだ操作を簡単に行うことができます。
 *
 *  ただし、FatFs等の組込み向けファイルシステムドライバを直接使用することに比べ
 *  て、抽象化によるオーバーヘッドがそれなりに存在することは意識しておくべきです
 *  。アプリケーションにとって、そのオーバーヘッドが無視できる程度なのかどうかを
 *  見極めて使用しましょう。
 *
 *  @see xvfs
 *  @{
 */


/** @brief xunionfsの初期化を行います
 *
 *  全てのxunionfs_xxx()の呼び出し前にこの関数を呼び出してください。
 *  xunionfs_deinit()による終了処理を行わずに、再度この関数を呼び出すことは禁止です。
 */
void xunionfs_init();


/** @brief xunionfsの終了処理を行います。
 */
void xunionfs_deinit();


/** @brief 仮想ファイルシステムをディレクトリツリーに接続します
 *
 *  @param vfs      初期化済みの有効なXVirtualFsオブジェクト
 *  @param path     マウント先パス
 *  @param realpath vfs基準のマウント元パス
 *  @pre
 *  + vfs       != NULL
 *  + path      != NULL
 *  + realpath  != NULL
 *
 *  "/"以外のパスはすでにxunionfs内に存在しているディレクトリである必要があります。
 *  そのため、必然的にはじめのマウントパスは"/"です。
 *
 *  マウント先にもともと存在するエントリは不可視となるため、通常は空のディレクト
 *  リに対してマウントを行います。
 */
XError xunionfs_mount(XVirtualFs* vfs, const char* path, const char* realpath);


/** @brief ファイルシステムのマウントを解除(アンマウント)します
 *
 *  @pre
 *  + path      != NULL
 */
XError xunionfs_umount(const char* path);


XError xunionfs_open(const char* path, XOpenMode mode, XFile** o_fp);
XError xunionfs_mkdir(const char* path);
XError xunionfs_opendir(const char* path, XDir** o_dir);
XError xunionfs_chdir(const char* path);
XError xunionfs_getcwd(char* buf, size_t size);
XError xunionfs_remove(const char* path);
XError xunionfs_rename(const char* oldpath, const char* newpath);
XError xunionfs_stat(const char* path, XStat* statbuf);
XError xunionfs_utime(const char* path, XTime time);
XError xunionfs_copyfile(const char* src, const char* dst);
XError xunionfs_copytree(const char* src, const char* dst);
XError xunionfs_rmtree(const char* path);
XError xunionfs_makedirs(const char* path, bool exist_ok);
XError xunionfs_walktree(const char* path, XFsTreeWalker walker, void* userptr);
XError xunionfs_exists(const char* path, bool* exists);
XError xunionfs_is_directory(const char* path, bool* isdir);
XError xunionfs_is_regular(const char* path, bool* isreg);

#define xunionfs_init_stream   xvfs_init_stream
#define xunionfs_close         xvfs_close
#define xunionfs_read          xvfs_read
#define xunionfs_write         xvfs_write
#define xunionfs_seek          xvfs_seek
#define xunionfs_tell          xvfs_tell
#define xunionfs_flush         xvfs_flush
#define xunionfs_readdir       xvfs_readdir
#define xunionfs_closedir      xvfs_closedir
#define xunionfs_putc          xvfs_putc
#define xunionfs_puts          xvfs_puts
#define xunionfs_printf        xvfs_printf
#define xunionfs_vprintf       xvfs_vprintf
#define xunionfs_getc          xvfs_getc
#define xunionfs_gets          xvfs_gets
#define xunionfs_copyfile2     xvfs_copyfile2


/** @} end of addtogroup xunionfs
 *  @} end of addtogroup filesystem
 */


#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* picox_filesystem_xunionfs_h_ */
