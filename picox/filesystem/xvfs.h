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
 *  ムの違いを隠蔽する抽象化層が存在しているため、ユーザーアプリケーションからは
 *  、アプリケーションからはファイルシステムの違いを意識せず扱うことができます。
 *
 *  しかし、特定のランタイムライブラリを持たない小規模な組込みシステムでは、ファ
 *  イルシステムドライバ(FatFs等)は抽象化層なしで直接使用できるように実装されま
 *  す。
 *
 *  これらのドライバをpicoxが定義する仮想ファイルシステムインターフェースにに合
 *  わせてラップすることで、特定のファイルシステムに依存しない共通のファイル操作
 *  を実現します。
 *  @{
 */


/** ストリームをファイルストリームとして初期化します
 */
XStream* xvfs_init_stream(XStream* stream, XFile* fp);


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
XError xvfs_open(XVirtualFs* vfs, const char* path, XOpenMode mode, XFile** o_fp);


/** @brief ファイルをクローズします
 *
 *  クローズされたXFileオブジェクトは無効となり、以降のあらゆる操作は不正となります。
 *  クローズに失敗した場合も、無効となるため、書き込みを行ったファイルはクローズ
 *  の前に明示的にフラッシュを行ったほうがエラー処理を行いやすいです。
 *
 *  fpがNULLを指す場合は何も処理をせずに正常終了を返します。
 */
XError xvfs_close(XFile* fp);


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
XError xvfs_read(XFile* fp, void* dst, size_t size, size_t* nread);


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
XError xvfs_write(XFile* fp, const void* src, size_t size, size_t* nwritten);


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
 */
XError xvfs_seek(XFile* fp, XOffset pos, XSeekMode whence);


/** @brief ファイル位置を取得します
 *
 *  @pre
 *  + fp    != NULL
 *  + pos   != NULL
 *
 *  read, write, seek等で移動するファイル位置の現在値をposに格納します。
 */
XError xvfs_tell(XFile* fp, XSize* pos);


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
 *  クローズやシークの際には暗黙的にフラッシュが呼び出されます。
 */
XError xvfs_flush(XFile* fp);


/** @brief ディレクトリを作成します
 *
 *  @pre
 *  + path  != NULL
 *
 *  pathが示すディレクトリの作成を試みます。
 */
XError xvfs_mkdir(XVirtualFs* vfs, const char* path);


/** @brief ディレクトリをオープンします
 *
 *  @pre
 *  + path  != NULL
 *  + o_dir != NULL
 *
 *  pathが示すディレクトリをオープンし、そのポインタを*o_dirにセットします。
 *  失敗した場合は、*o_dirにはNULLがセットされます。
 */
XError xvfs_opendir(XVirtualFs* vfs, const char* path, XDir** o_dir);


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
XError xvfs_readdir(XDir* dir, XDirEnt* dirent, XDirEnt** result);


/** @brief ディレクトリをクローズします
 *
 *  クローズされたディレクトリは無効となり、以降のあらゆる操作は不正となります。
 *
 *  dirがNULLを指す場合は何も処理をせずに正常終了を返します。
 */
XError xvfs_closedir(XDir* dir);


/** @brief カレントディレクトリを変更します
 *
 *  @pre
 *  +path   != NULL
 */
XError xvfs_chdir(XVirtualFs* vfs, const char* path);


/** @brief カレントディレクトリを取得します
 *
 *  @pre
 *  + buf   != NULL
 *
 *  カレントディレクトリパスがsizeバイトに収まらない場合はエラーとなります。
 */
XError xvfs_getcwd(XVirtualFs* vfs, char* buf, size_t size);


/** @brief エントリを削除します
 *
 *  @pre
 *  + path  != NULL
 *
 *  パスが示すエントリをファイルシステムから削除します。
 */
XError xvfs_remove(XVirtualFs* vfs, const char* path);


/** @brief エントリの名前や位置を変更します
 *
 *  @pre
 *  + oldpath   != NULL
 *  + newpath   != NULL
 *
 *  oldpathが示すエントリをnewpathに移動します。
 *
 *  @note
 *  ファイルシステムにとって名前や位置の変更は、通常はコストの低い処理ですが、
 *  xunionfs_rename()で移動がファイルシステムをまたがる場合、ファイルをコピーし
 *  、元ファイルを削除するという処理が行われます。思わぬ高負荷の要因となることが
 *  あるため注意しましょう。
 */
XError xvfs_rename(XVirtualFs* vfs, const char* oldpath, const char* newpath);


/** @brief エントリの状態を取得します
 *
 *  @pre
 *  + path      != NULL
 *  + statbuf   != NULL
 *
 *  パスが示すエントリの状態を*statbufに格納します。
 */
XError xvfs_stat(XVirtualFs* vfs, const char* path, XStat* statbuf);


/** @brief エントリのタイムスタンプを変更します
 *
 *  @pre
 *  + path      != NULL
 *
 *  パスが示すエントリのタイムスタンプをtimeに変更します
 */
XError xvfs_utime(XVirtualFs* vfs, const char* path, XTime time);


/** @brief ファイルに1文字を書き込みます
 */
int xvfs_putc(XFile* fp, int c);


/** @brief ファイルに文字列を書き込みます
 */
int xvfs_puts(XFile* fp, const char* str);


/** @brief ファイルにprintf形式で文字列を書き込みます
 */
int xvfs_printf(XFile* fp, const char* fmt, ...) X_PRINTF_ATTR(2, 3);


/** @brief ファイルにvprintf形式で文字列を書き込みます
 */
int xvfs_vprintf(XFile* fp, const char* fmt, va_list args);


/** @brief ファイルから1文字を読み込みます
 */
int xvfs_getc(XFile* fp);


/** @brief ファイルから1行を読み込みます
 *
 *  @see xstream_gets
 */
XError xvfs_gets(XFile* fp, char* dst, size_t size, char** result, bool* overflow);


/** @brief パスが存在するかどうかを判定します
 *
 *  @pre
 *  + path      != NULL
 */
XError xvfs_exists(XVirtualFs* vfs, const char* path, bool* exists);


/** @brief パスがディレクトリかどうかを判定します
 *
 *  @pre
 *  + path      != NULL
 */
XError xvfs_is_directory(XVirtualFs* vfs, const char* path, bool* isdir);


/** @brief パスが通常ファイルかどうかを判定します
 *
 *  @pre
 *  + path      != NULL
 */
XError xvfs_is_regular(XVirtualFs* vfs, const char* path, bool* isreg);


/** @brief ファイルをコピーします
 *
 *  @pre
 *  + src      != NULL
 *  + dst      != NULL
 *
 *  srcからdstにコピーを行います。
 */
XError xvfs_copyfile(XVirtualFs* vfs, const char* src, const char* dst);


/** @brief ファイルをコピーします
 *
 *  @pre
 *  + src      != NULL
 *  + dst      != NULL
 *
 *  srcからdstにコピーを行います。
 */
XError xvfs_copyfile2(XFile* src, XFile* dst);


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
XError xvfs_copytree(XVirtualFs* vfs, const char* src, const char* dst);


/** @brief ディレクトリツリーを削除します
 *
 *  @pre
 *  + path  != NULL
 *
 *  失敗した場合でも、途中で削除されたファイルは元にはもどりません。
 */
XError xvfs_rmtree(XVirtualFs* vfs, const char* path);


/** @brief ディレクトリを作成します
 *
 *  @pre
 *  + path  != NULL
 *
 *  パスの末端のディレクトリを作成するために必要な中間のディレクトリも含めて作成
 *  します。
 *
 *  `exist_ok == true`の場合は、末端のディレクトリが既に存在していた場合も正常終
 *  了を返します。
 */
XError xvfs_makedirs(XVirtualFs* vfs, const char* path, bool exist_ok);


/** @brief ディレクトリツリーを走査します
 *
 *  @pre
 *  + path      != NULL
 *  + walker    != NULL
 *
 *  パスが示すディレクトリを再帰的にたどり、各エントリに対してwalker()を呼び出し
 *  ます。
 */
XError xvfs_walktree(XVirtualFs* vfs, const char* path, XFsTreeWalker walker, void* userptr);


/** @} end of addtogroup xvfs
 *  @} end of addtogroup filesystem
 */


#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* picox_filesystem_xvfs_h_ */
