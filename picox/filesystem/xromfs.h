/**
 *       @file  xromfs.h
 *      @brief  ROMファイルシステム定義
 *
 *    @details
 *
 *     @author  MaskedW
 *
 *   @internal
 *     Created  2016/05/03
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

#ifndef picox_filesystem_xromfs_h_
#define picox_filesystem_xromfs_h_


#include <picox/filesystem/xfscore.h>


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/** @addtogroup filesystem
 *  @{
 *  @addtogroup  xromfs
 *  @brief ROM上に作成するファイルシステムモジュールです
 *
 *  予め作成したROMファイルシステムイメージを、なんらかの方法でROM領域に埋め込ん
 *  で(コンパイラの拡張機能かpicoxが提供するxbin2c.pyを使用して配列に変換する)使
 *  用することを想定したリードオンリーのファイルシステムです。
 *
 *  通常のファイルシステムは、ブロックアクセスが必要な外部記憶装置の特性に合わし
 *  た構造をしていますが、このファイルシステムはランダムアクセス可能かつリードオ
 *  ンリーであることを前提にしているため、使用領域に無駄がありません。
 *
 *  最近のMPUはRAMサイズに比べてROMはわりと潤沢にあるので、画像データや初期設定
 *  ファイル等のリソースを埋め込んで使用することを想定しています。
 *
 *  1ファイルずつを個別に埋め込んだ場合に比べて、多少のオーバーヘッドはあります
 *  が、ファイルシステムのインターフェースを通してアクセスできるようになることと
 *  、リソースデータだけを差し替えることが比較的容易になることが利点です。
 *
 *  @see xunionfs
 *  @see xvfs
 *  @{
 */


typedef struct
{
    const void* m_fstype_tag;
    const char* m_top;
    const char* m_rootdir;
    const char* m_curdir;
} XRomFs;
X_DECLEAR_RTTI_TAG(XROMFS_RTTI_TAG);


/** @brief ファイルシステムを初期化します
 *
 *  @param romimage ROMファイルシステムのディスクイメージが格納された領域
 *  @pre
 *  + fs       != NULL
 *  + romimage != NULL
 */
XError xromfs_init(XRomFs* fs, const void* romimage);


/** @brief ファイルシステムの終了処理を行います
 *
 *  @pre
 *  + fs    != NULL
 */
void xromfs_deinit(XRomFs* fs);


/** @brief 仮想ファイルシステムを初期化します
 *
 *  @pre
 *  + fs    != NULL
 *  + vfs   != NULL
 */
XVirtualFs* xromfs_init_vfs(XRomFs* fs, XVirtualFs* vfs);


XStream* xromfs_init_stream(XStream* stream, XFile* fp);
XError xromfs_open(XRomFs* fs, const char* path, XOpenMode mode, XFile** o_fp);
XError xromfs_close(XFile* fp);
XError xromfs_read(XFile* fp, void* dst, size_t size, size_t* nread);
XError xromfs_seek(XFile* fp, XOffset pos, XSeekMode whence);
XError xromfs_tell(XFile* fp, XSize* pos);
XError xromfs_opendir(XRomFs* fs, const char* path, XDir** o_dir);
XError xromfs_readdir(XDir* dir, XDirEnt* dirent, XDirEnt** result);
XError xromfs_closedir(XDir* dir);
XError xromfs_chdir(XRomFs* fs, const char* path);
XError xromfs_getcwd(XRomFs* fs, char* buf, size_t size);
XError xromfs_stat(XRomFs* fs, const char* path, XStat* statbuf);


/** @} end of addtogroup xromfs
 *  @} end of addtogroup filesystem
 */


#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* picox_filesystem_xromfs_h_ */
