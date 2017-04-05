/**
 *       @file  xramfs.h
 *      @brief  RAMファイルシステム定義
 *
 *    @details
 *
 *     @author  MaskedW
 *
 *   @internal
 *     Created  2016/02/20
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

#ifndef picox_filesystem_xramfs_h_
#define picox_filesystem_xramfs_h_


#include <picox/filesystem/xfscore.h>
#include <picox/allocator/xpico_allocator.h>


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/** @addtogroup filesystem
 *  @{
 *  @addtogroup  xramfs
 *  @brief RAM上に作成するファイルシステムモジュールです
 *
 *  RAM上に作成するので当然再起動するたびにクリアされます。
 *  毎回クリアされるとしても、ファイルインターフェースを通してデータのやりとり
 *  をしたい、できた方が便利、という局面はそれなりにあるものです。
 *
 *  FatFs等のファイルシステムドライバでRAM上にファイルシステムを構築することも
 *  できますが、通常のファイルシステムはある程度大容量(MByte以上)のブロックデバ
 *  イスの使用が前提に前提になっているため、数KByteのRAMでは使用できません。
 *
 *  このファイルシステムははじめからRAMの特性に合わせて設計されているため、百
 *  バイトの極小のRAMファイルシステムを構築することも可能です。
 *
 *  @see xunionfs
 *  @see xvfs
 *  @{
 */


typedef struct
{
    const void*     m_fstype_tag;
    XPicoAllocator  m_alloc;
    void*           m_rootdir;
    void*           m_curdir;
} XRamFs;
X_DECLEAR_RTTI_TAG(XRAMFS_RTTI_TAG);


/** @brief ファイルシステムを初期化します
 *
 *  @param mem  RAMファイルシステムに使用するメモリ
 *  @param size memのバイト数
 *  @pre
 *  + fs    != NULL
 *  + size   > 0
 *
 *  memがNULLを指すときは、ヒープからsizeバイトのメモリを確保します。
 */
XError xramfs_init(XRamFs* fs, void* mem, size_t size);


/** @brief ファイルシステムの終了処理を行います
 *
 *  @pre
 *  + fs    != NULL
 */
void xramfs_deinit(XRamFs* fs);


/** @brief 仮想ファイルシステムを初期化します
 *
 *  @pre
 *  + fs    != NULL
 *  + vfs   != NULL
 */
XVirtualFs* xramfs_init_vfs(XRamFs* fs, XVirtualFs* vfs);

XStream* xramfs_init_stream(XStream* stream, XFile* fp);
XError xramfs_open(XRamFs* fs, const char* path, XOpenMode mode, XFile** o_fp);
XError xramfs_close(XFile* fp);
XError xramfs_read(XFile* fp, void* dst, size_t size, size_t* nread);
XError xramfs_write(XFile* fp, const void* src, size_t size, size_t* nwritten);
XError xramfs_seek(XFile* fp, XOffset pos, XSeekMode whence);
XError xramfs_tell(XFile* fp, XSize* pos);
XError xramfs_flush(XFile* fp);
XError xramfs_mkdir(XRamFs* fs, const char* path);
XError xramfs_opendir(XRamFs* fs, const char* path, XDir** o_dir);
XError xramfs_readdir(XDir* dir, XDirEnt* dirent, XDirEnt** result);
XError xramfs_closedir(XDir* dir);
XError xramfs_chdir(XRamFs* fs, const char* path);
XError xramfs_getcwd(XRamFs* fs, char* buf, size_t size);
XError xramfs_remove(XRamFs* fs, const char* path);
XError xramfs_rename(XRamFs* fs, const char* oldpath, const char* newpath);
XError xramfs_stat(XRamFs* fs, const char* path, XStat* statbuf);
XError xramfs_utime(XRamFs* fs, const char* path, XTime time);


/** @} end of addtogroup xramfs
 *  @} end of addtogroup filesystem
 */


#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* picox_filesystem_xramfs_h_ */
