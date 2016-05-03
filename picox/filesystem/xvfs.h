/**
 *       @file  xvfs.h
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

#ifndef picox_filesystem_xvfs_h_
#define picox_filesystem_xvfs_h_


#include <picox/filesystem/xfscore.h>


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/** @addtogroup filesystem
 *  @{
 *  @addtogroup  xvfs
 *  @brief 異なるファイルシステムを共通のインターフェースで操作するための抽象化層です
 *
 *  LinuxやWindowsではfopen()等の標準のファイル操作APIの下位実装にファイルシステ
 *  ムの違いを隠蔽する抽象化層が存在し、ファイルシステムは仮想ファイルシステムの
 *  インターフェースに合わせて実装され、ユーザーアプリケーションからはファイルシ
 *  ステムの違いを意識せず扱うことができます。
 *
 *  しかし、特定のランタイムライブラリを持たない小規模な組込みシステムでは、ファ
 *  イルシステムドライバ(FatFs等)は抽象化層なしで直接使用できるように実装されま
 *  す。
 *
 *  これらのドライバをpicoxが定義する仮想ファイルシステムインターフェースにに合
 *  わせてラップすることで、共通のファイル操作を実現します。
 *  @{
 */


#define X_VFS_TAG     (X_MAKE_TAG('X', 'V', 'I', 'F'))

XStream* xvfs_init_stream(XStream* stream, XFile* fp);
XError xvfs_open(XVirtualFs* vfs, const char* path, XOpenMode mode, XFile** o_fp);
XError xvfs_close(XFile* fp);
XError xvfs_read(XFile* fp, void* dst, size_t size, size_t* nread);
XError xvfs_write(XFile* fp, const void* src, size_t size, size_t* nwritten);
XError xvfs_seek(XFile* fp, XOffset pos, XSeekMode whence);
XError xvfs_tell(XFile* fp, XSize* pos);
XError xvfs_flush(XFile* fp);
XError xvfs_mkdir(XVirtualFs* vfs, const char* path);
XError xvfs_opendir(XVirtualFs* vfs, const char* path, XDir** o_dir);
XError xvfs_readdir(XDir* dir, XDirEnt* dirent, XDirEnt** result);
XError xvfs_closedir(XDir* dir);
XError xvfs_chdir(XVirtualFs* vfs, const char* path);
XError xvfs_getcwd(XVirtualFs* vfs, char* buf, size_t size);
XError xvfs_remove(XVirtualFs* vfs, const char* path);
XError xvfs_rename(XVirtualFs* vfs, const char* oldpath, const char* newpath);
XError xvfs_stat(XVirtualFs* vfs, const char* path, XStat* statbuf);
XError xvfs_utime(XVirtualFs* vfs, const char* path, XTime time);



/** @} end of addtogroup xvfs
 *  @} end of addtogroup filesystem
 */


#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* picox_filesystem_xvfs_h_ */
