/**
 *       @file  xsinglefs.h
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

#ifndef picox_filesystem_xsinglefs_h_
#define picox_filesystem_xsinglefs_h_


#include <picox/filesystem/xvfs.h>


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/** @addtogroup filesystem
 *  @{
 *  @addtogroup  xsinglefs
 *  @brief 単一のXVirtualFsを保持し、インターフェースを簡素化します
 *
 *  xvfsは複数の異なるファイルシステムを同一のインターフェースで扱えるようにしま
 *  すが、一部関数にはXVirtualFsオブジェクトを引数として渡す必要があり、若干面倒
 *  です。そこで、このモジュールにオブジェクトを保持させておくことでインターフェ
 *  ースを簡素化します。
 *
 *  xunionfsも似たような機能を提供しますが、複数のファイルシステムを登録できるよ
 *  うにしているため、それなりにオーバーヘッドがかかります。
 *  組込みシステムではファイルシステムは不要か、一つで十分なことが多いため、その
 *  場合はこちらのモジュールを使用した方がリソースの節約になります。
 *
 *  @see xvfs
 *  @{
 */


void xsinglefs_init(XVirtualFs* vfs);
void xsinglefs_deinit();
XError xsinglefs_open(const char* path, XOpenMode mode, XFile** o_fp);
XError xsinglefs_mkdir(const char* path);
XError xsinglefs_opendir(const char* path, XDir** o_dir);
XError xsinglefs_chdir(const char* path);
XError xsinglefs_getcwd(char* buf, size_t size);
XError xsinglefs_remove(const char* path);
XError xsinglefs_rename(const char* oldpath, const char* newpath);
XError xsinglefs_stat(const char* path, XStat* statbuf);
XError xsinglefs_utime(const char* path, XTime time);
XError xsinglefs_copyfile(const char* src, const char* dst);
XError xsinglefs_copytree(const char* src, const char* dst);
XError xsinglefs_rmtree(const char* path);
XError xsinglefs_makedirs(const char* path, bool exist_ok);
XError xsinglefs_walktree(const char* path, XFsTreeWalker walker, void* userptr);
XError xsinglefs_exists(const char* path, bool* exists);
XError xsinglefs_is_directory(const char* path, bool* isdir);
XError xsinglefs_is_regular(const char* path, bool* isreg);

#define xsinglefs_init_stream   xvfs_init_stream
#define xsinglefs_close         xvfs_close
#define xsinglefs_read          xvfs_read
#define xsinglefs_write         xvfs_write
#define xsinglefs_seek          xvfs_seek
#define xsinglefs_tell          xvfs_tell
#define xsinglefs_flush         xvfs_flush
#define xsinglefs_readdir       xvfs_readdir
#define xsinglefs_closedir      xvfs_closedir
#define xsinglefs_putc          xvfs_putc
#define xsinglefs_puts          xvfs_puts
#define xsinglefs_printf        xvfs_printf
#define xsinglefs_vprintf       xvfs_vprintf
#define xsinglefs_getc          xvfs_getc
#define xsinglefs_gets          xvfs_gets
#define xsinglefs_copyfile2     xvfs_copyfile2


/** @} end of addtogroup xsinglefs
 *  @} end of addtogroup filesystem
 */


#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* picox_filesystem_xsinglefs_h_ */
