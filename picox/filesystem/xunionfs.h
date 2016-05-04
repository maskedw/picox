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


/** @brief ファイルストリームを初期化します
 */
XStream* xunionfs_init_stream(XStream* stream, XFile* fp);


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


/** @brief ファイルをオープンします
 *
 *  @pre
 *  + path != NULL
 *  + o_fp != NULL
 *
 *  pathが示すファイルをオープンし、そのポインタを*o_fpにセットします。失敗した
 *  場合は、*o_fpにはNULLがセットされます。
 *
 */
XError xunionfs_open(const char* path, XOpenMode mode, XFile** o_fp);


/** @brief ファイルをクローズします
 *
 *  クローズされたファイルは無効となり、以降のあらゆる操作は不正となります。
 *
 *  クローズに失敗した場合も、同様にファイルは無効となるため、書き込みを行ったフ
 *  ァイルはクローズの前にxunionfs_flush()によるバッファのフラッシュを行っておくほう
 *  がエラー処理を行いやすいです。
 *
 *  fpがNULLを指す場合は何も処理をせずに正常終了を返します。
 */
XError xunionfs_close(XFile* fp);


/** @brief ファイルからデータを読み込みます
 *
 *  @pre
 *  + fp    != NULL
 *  + dst   != NULL
 *
 *  dstにsizeバイトの読み込みを試み、nreadがNULLでなければ、読み込みに成功したバ
 *  イト数を格納します。
 *
 *  関数が正常に終了し、size != *nreadであった場合は、ファイル終端に達したことを
 *  意味します。
 *
 */
XError xunionfs_read(XFile* fp, void* dst, size_t size, size_t* nread);


/** @brief ファイルにデータを書き込みます
 *
 *  @pre
 *  + fp    != NULL
 *  + src   != NULL
 *
 *  srcからファイルにsizeバイトの書き込みを試み、nwrittenがNULLでなければ、書き
 *  込みに成功したバイト数を格納します。
 *
 */
XError xunionfs_write(XFile* fp, const void* src, size_t size, size_t* nwritten);


/** @brief ファイル位置を変更します
 *
 *  @pre
 *  + fp    != NULL
 *
 *  新たな位置はwhenceで指定された位置にoffsetバイトを加えることによって与えられ
 *  ます。
 *
 *  ファイル位置が0未満となる場合はエラーを返しますが、ファイルサイズを越える位
 *  置へのシークは有効です。
 *
 *  ファイルサイズを超えた位置に書き込むを行うと、空き空間は不定のデータで埋めら
 *  れます。
 *
 *  @see XSeekMode
 *  @see xvfs
 */
XError xunionfs_seek(XFile* fp, XOffset offset, XSeekMode whence);


/** @brief ファイル位置を取得します
 *
 *  @pre
 *  + fp    != NULL
 *  + pos   != NULL
 *
 *  read, write, seek等で移動するファイル位置の現在値をposに格納します。
 */
XError xunionfs_tell(XFile* fp, XSize* pos);


/** @brief バッファされたデータを出力します
 *
 *  @pre
 *  + fp    != NULL
 *
 *  ファイルシステムの実装次第ですが、通常はIOアクセスを減らすために、writeされ
 *  たデータはある程度メモリ上にバッファリングされます。
 *
 *  ファイルへの書き込みを確実にしたいタイミングではこの関数でバッファリングを強
 *  制的にフラッシュしてください。
 *
 *  クローズやシークの際には暗黙的にフラッシュが行われます。
 */
XError xunionfs_flush(XFile* fp);


/** @brief ディレクトリを作成します
 *
 *  @pre
 *  + path  != NULL
 *
 *  pathが示すディレクトリの作成を試みます。
 */
XError xunionfs_mkdir(const char* path);


/** @brief ディレクトリをオープンします
 *
 *  @pre
 *  + path  != NULL
 *  + o_dir != NULL
 *
 *  pathが示すディレクトリをオープンし、そのポインタを*o_dirにセットします。
 *  失敗した場合は、*o_dirにはNULLがセットされます。
 */
XError xunionfs_opendir(const char* path, XDir** o_dir);


/** @brief ディレクトリから要素を読み込みます
 *
 *  @pre
 *  + dir       != NULL
 *  + dirent    != NULL
 *  + result    != NULL
 *
 *  読み込みはdirentが指す領域に行われ、読み込みに成功すると、*resultにはdirent
 *  が格納されます。
 */
XError xunionfs_readdir(XDir* dir, XDirEnt* dirent, XDirEnt** result);


/** @brief ディレクトリをクローズします
 *
 *  クローズされたディレクトリは無効となり、以降のあらゆる操作は不正となります。
 *
 *  dirがNULLを指す場合は何も処理をせずに正常終了を返します。
 */
XError xunionfs_closedir(XDir* dir);


/** @brief カレントディレクトリを変更します
 *
 *  @pre
 *  +path   != NULL
 */
XError xunionfs_chdir(const char* path);


/** @brief カレントディレクトリを取得します
 *
 *  @pre
 *  + buf   != NULL
 *
 *  カレントディレクトリパスがsizeバイトに収まらない場合はエラーとなります。
 */
XError xunionfs_getcwd(char* buf, size_t size);


/** @brief エントリを削除します
 *
 *  @pre
 *  + path  != NULL
 *
 *  パスが示すエントリをファイルシステムから削除します。
 */
XError xunionfs_remove(const char* path);


/** @brief エントリの名前や位置を変更します
 *
 *  @pre
 *  + oldpath   != NULL
 *  + newpath   != NULL
 *
 *  oldpathが示すエントリをnewpathに移動します。
 *
 *  @note
 *  ファイルシステムにとって名前や位置の変更は、通常はコストの低い処理ですが、フ
 *  ァイルシステムをまたがった移動の場合は、ファイルをコピーし、元ファイルを削除
 *  するという処理が行われます。
 *
 *  思わぬ高負荷の要因となることがあるため注意しましょう。
 */
XError xunionfs_rename(const char* oldpath, const char* newpath);


/** @brief エントリの状態を取得します
 *
 *  @pre
 *  + path      != NULL
 *  + statbuf   != NULL
 *
 *  パスが示すエントリの状態を*statbufに格納します。
 *
 *  @ref
 *  XStat
 */
XError xunionfs_stat(const char* path, XStat* statbuf);


/** @brief エントリのタイムスタンプを変更します
 *
 *  @pre
 *  + path      != NULL
 *
 *  パスが示すエントリのタイムスタンプをtimeに変更します
 */
XError xunionfs_utime(const char* path, XTime time);


/** @brief ファイルをコピーします
 *
 *  @pre
 *  + src      != NULL
 *  + dst      != NULL
 *
 *  srcからdstにコピーを行います。
 */
XError xunionfs_copyfile(const char* src, const char* dst);


/** @brief ディレクトリツリーをコピーします
 *
 *  @pre
 *  + src      != NULL
 *  + dst      != NULL
 *
 *  dstが示すターゲットディレクトリは、既存のもので無い必要があります。
 *
 *  失敗した場合でも、途中で作成されたファイルはそのまま残ります。
 */
XError xunionfs_copytree(const char* src, const char* dst);


/** @brief ディレクトリツリーを削除します
 *
 *  @pre
 *  + path  != NULL
 *
 *  失敗した場合でも、途中で削除されたファイルは元にはもどりません。
 */
XError xunionfs_rmtree(const char* path);


/** @brief ディレクトリを作成します
 *
 *  @pre
 *  + path  != NULL
 *
 *  xunionfs_mkdir()と違い、パスの末端のディレクトリを作成するために必要な中間のディ
 *  レクトリも作成します。
 *
 *  `exist_ok == true`の場合は、末端のディレクトリが既に存在していた場合も正常終
 *  了を返します。
 */
XError xunionfs_makedirs(const char* path, bool exist_ok);


/** @brief ディレクトリツリーを走査します
 *
 *  @pre
 *  + path      != NULL
 *  + walker    != NULL
 *
 *  パスが示すディレクトリを再帰的にたどり、各エントリに対してwalker()を呼び出し
 *  ます。
 */
XError xunionfs_walktree(const char* path, XFsTreeWalker walker, void* userptr);


/** @brief パスが存在するかどうかを判定します
 *
 *  @pre
 *  + path      != NULL
 */
XError xunionfs_exists(const char* path, bool* exists);


/** @brief パスがディレクトリかどうかを判定します
 *
 *  @pre
 *  + path      != NULL
 */
XError xunionfs_is_directory(const char* path, bool* isdir);


/** @brief パスが通常ファイルかどうかを判定します
 *
 *  @pre
 *  + path      != NULL
 */
XError xunionfs_is_regular(const char* path, bool* isreg);


/** @brief ファイルに1文字を書き込みます
 */
#define xunionfs_putc        xvfs_putc


/** @brief ファイルに文字列を書き込みます
 */
#define xunionfs_puts        xvfs_puts


/** @brief ファイルにprintf形式で文字列を書き込みます
 */
#define xunionfs_printf      xvfs_printf


/** @brief ファイルにvprintf形式で文字列を書き込みます
 */
#define xunionfs_vprintf     xvfs_vprintf


/** @brief ファイルから1文字を読み込みます
 */
#define xunionfs_getc        xvfs_getc


/** @brief ファイルから1行を読み込みます
 */
#define xunionfs_gets    xvfs_gets


/** @brief ファイルをコピーします
 *
 *  @pre
 *  + src      != NULL
 *  + dst      != NULL
 *
 *  パスからではなく、ファイルオブジェクトからコピーを行います。
 *
 *  srcは読み込み可能、dstは書き込み可能である必要があります。
 *
 *  srcやdstの読み書きのスタート位置は0バイト目からではなく、引数として渡された
 *  時点の位置から終端に達するまでです。
 */
#define xunionfs_copyfile2   xvfs_copyfile2




/** @} end of addtogroup xunionfs
 *  @} end of addtogroup filesystem
 */


#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* picox_filesystem_xunionfs_h_ */
