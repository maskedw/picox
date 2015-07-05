/**
 *       @file  xfile.h
 *      @brief  File interface wrapper
 *
 *    @details
 *    FileSystemによる違いや、扱いにくいインターフェースをラップします。
 *
 *    小規模な組み込みシステムでファイルシステムを採用する場合、以下に上げる問題
 *    があります。
 *
 *    + stdioのファイル関数がほぼ使用できない。もちろん依存部を移植すれば使用可
 *      能だが、不必要に複雑になってしまうのでオススメできない。
 *
 *    + そもそもstdioのファイル関数のインターフェースは適当に使う分にはいいが、
 *      厳密なエラーチェックはとてもやりにくい。
 *
 *    + stdioのファイル関数は機能が少なすぎる。
 *
 *    + FatFs等の組込み用ファイルシステムライブラリを使用した場合、ホストPCでの
 *      デバッグが困難になる。
 *    ------------------------------------------------------------
 *
 *    このモジュールではその全てを解決します。
 *
 *    + 全ての関数のエラーはBool型の戻り値で判定可能です。
 *    + ファイルの一括削除等の便利関数を提供します。
 *    + コンパイル時に、ファイルエンジンを切替可能なので、実機ではFatfs、テスト
 *      ではPOSIXという切替が簡単にできます。
 *
 *    ただし、通常のファイル関数より厳密な処理を行うために、多数の引数を要求しま
 *    す。
 *
 *    @note
 *    ファイルエンジンの全てをラップすることは目的としていません。
 *    小規模組込みシステムにおいて、過度な隠蔽化は余計に扱いにくくなってしまい、
 *    労力も割りに合いません。あくまで上位層のアプリケーションの統一的なイン
 *    ターフェースの提供に注力します。
 *
 *    @todo
 *    + ディレクトリ走査
 *
 *     @author  MaskedW
 *
 *   @internal
 *     Created  2015/06/14
 * ===================================================================
 */

/*
 * License: MIT license
 * Copyright (c) <2015> <MaskedW [maskedw00@gmail.com]>
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


#ifndef picox_xfile_h_
#define picox_xfile_h_


#include <picox/core/xcore.h>


typedef uint32_t    XFileSize;


#ifdef XFILE_CONF_PATH_MAX
    #define XFILE_PATH_MAX      XFILE_CONF_PATH_MAX
#else
    #define XFILE_PATH_MAX      128
#endif


#define XFILE_ENGINE_FATFS       (0)
#define XFILE_ENGINE_POSIX       (1)


/** ファイルエンジンの種類を指定します。
 *
 *  0: FatFs(http://elm-chan.org/fsw/ff/00index_j.html)
 *  1: POSIX(fopen, fcloseとか)
 */
#ifdef XFILE_CONF_ENGINE_TYPE
    #define XFILE_ENGINE_TYPE   XFILE_CONF_ENGINE_TYPE
#else
    #define XFILE_ENGINE_TYPE   XFILE_ENGINE_POSIX
#endif


#if XFILE_ENGINE_TYPE == XFILE_ENGINE_FATFS

    #include <ff.h>

#elif XFILE_ENGINE_TYPE == XFILE_ENGINE_POSIX

    /*
     * MinGWのgcc4.8のヘッダにはバグ(Typo)があり、以下のマクロを除去することで
     * off_t絡みのバグを回避する。
     */
    #if  defined(__MINGW32__) || defined(__MINGW64__) && (__GNUC__ == 4) && (__GNUC_MINOR__ == 8)
        #undef __STRICT_ANSI__
    #endif
    #include <dirent.h>
#endif


#ifdef __cplusplus
extern "C" {
#endif


#if XFILE_ENGINE_TYPE == 0

    typedef FIL     XFile;
    typedef DIR     XDir;

#elif XFILE_ENGINE_TYPE == 1

    typedef FILE    XFile;
    typedef DIR     XDir;

#endif


/** 指定パスをオープンします。
 *
 *  @param path     ファイルパス
 *  @param mode     オープンモード(fopen()と同じ)
 *  @param err      エラーステータス格納先
 *  @pre
 *  + path != NULL
 *  + mode != NULL
 *  @note
 *  openモードについては以下サイトが詳しいです。
 *
 *  http://www9.plala.or.jp/sgwr-t/c/sec17.html
 */
XFile* xfile_open(const char* path, const char* mode, int* err);


/** ファイルをクローズします。
 *
 *  @note
 *  fclose()にはNULLは渡せませんが、この関数ではNULLを許容します。
 */
bool xfile_close(XFile* fp, int* err);


/** srcからsizeバイトをfpに書き出します。
 *
 *  @param fp       有効なファイルポインタ
 *  @param src      書き込むデータ
 *  @param size     srcから書き込むバイト数
 *  @param nwritten 実際に書き込めたバイト数の格納先
 *  @param err      エラーステータス格納先
 *  @pre
 *  + fp  != NULL
 *  + src != NULL
 */
bool xfile_write(XFile* fp, const void* src, size_t size, size_t* nwritten, int* err);


/** fpからdstにsizeバイトを読み出します。
 *
 *  @param fp       有効なファイルポインタ
 *  @param dst      読み込みデータ格納先
 *  @param size     読み込むバイト数
 *  @param nread    実際に読み込めたバイト数の格納先
 *  @param err      エラーステータス格納先
 *  @pre
 *  + fp  != NULL
 *  + dst != NULL
 */
bool xfile_read(XFile* fp, void* dst, size_t size, size_t* nread, int* err);


/** ファイルサイズを返します。
 *
 *  @param fp   有効なファイルポインタ
 *  @param dst  ファイルサイズ格納先
 *  @param err  エラーステータス格納先
 *  @pre
 *  + fp  != NULL
 *  + dst != NULL
 */
bool xfile_size(XFile* fp, XFileSize* dst, int* err);


/** pathからファイルサイズを取得して返します。
 *
 *  @param path ファイルパス
 *  @param dst  ファイルサイズ格納先
 *  @param err  エラーステータス格納先
 *  @pre
 *  + path != NULL
 *  + dst  != NULL
 */
bool xfile_size_with_path(const char* path, XFileSize* dst, int* err);


/** pathが存在するかどうかを返します。
 *
 *  @param path     ファイルパス
 *  @param exists   存在チェック結果格納先
 *  @param err      エラーステータス格納先
 *  @pre
 *  + path   != NULL
 *  + exists != NULL
 *  @note
 *  戻り値が返すのは、あくまで判定処理を行えたかどうかです。
 *
 *  つまり結果は以下の3パターンに分かれます。
 *  +  (戻り値 == true) && (exists == true)  => ファイルは存在する
 *  +  (戻り値 == true) && (exists == false) => ファイルは存在しない
 *  +  (戻り値 == false)                     => 判定不能
 */
bool xfile_exists(const char* path, bool* exists, int* err);


/** ファイルポインタをposに移動します。
 *
 *  @param fp   有効なファイルポインタ
 *  @param pos  先頭からのファイルオフセットバイト数
 *  @param err  エラーステータス格納先
 *  @pre
 *  + fp  != NULL
 */
bool xfile_seek(XFile* fp, XFileSize pos, int* err);


/** ファイルポインタの現在位置をかえします。
 *
 *  @param fp   有効なファイルポインタ
 *  @param pos  ファイルオフセットバイト数格納先
 *  @param err  エラーステータス格納先
 *  @pre
 *  + fp  != NULL
 *  + pos != NULL
 */
bool xfile_tell(XFile* fp, XFileSize* pos, int* err);


/** バッファをフラッシュします。
 *
 *  @param fp   有効なファイルポインタ
 *  @param err  エラーステータス格納先
 *  @pre
 *  + fp  != NULL
 */
bool xfile_flush(XFile* fp, int* err);


/** 指定パスにディレクトリを作成します。
 *
 *  @param path     ディレクトリパス
 *  @param exists   存在チェック結果格納先
 *  @param err      エラーステータス格納先
 *  @pre
 *  + path != NULL
 *  @note
 *  既に指定ディレクトリが存在したり、同名のファイルが存在した場合はfalseを返し
 *  ます。
 */
bool xfile_mkdir(const char* path, int* err);


/** カレントディレクトリをpathに移動します。
 *
 *  @param path     ディレクトリパス
 *  @param err      エラーステータス格納先
 *  @pre
 *  + path  != NULL
 */
bool xfile_chdir(const char* path, int* err);


/** カレントディレクトリを返します。
 *
 *  @param dst      カレントディレクトリ格納先
 *  @param size     dstに格納する最大バイト数
 *  @param err      エラーステータス格納先
 *  @pre
 *  + dst  != NULL
 *  + size  > 0
 */
bool xfile_cwd(char* dst, size_t size, int* err);


/** pathを削除します。
 *
 *  @param path     削除対象のパス
 *  @param err      エラーステータス格納先
 *  @pre
 *  + path != NULL
 *  @note
 *  指定パスが存在しなかった場合もtrueを返します。
 *  引数でエラーとするかOKとするかを選択させたり、別関数にする手もありますが、目
 *  的はpathを除去したいということなので、その目的が達成させているなら成功とする
 *  という方針を取ります。
 */
bool xfile_remove(const char* path, int* err);


/** path以下を全て削除します。
 *
 *  @param path     削除対象のパス
 *  @param err      エラーステータス格納先
 *  @pre
 *  + path != NULL
 *  @note
 *  xfile_remove()と同じ理由で、パスが存在しなかった場合でもtrueを返します。
 *  途中で削除に失敗した場合のロールバックはできないので注意してください。
 */
bool xfile_remove_all(const char* path, int* err);


/** ファイルから1行分のデータを取得します。
 *
 *  @param fp       有効なファイルポインタ
 *  @param dst      行データ格納先
 *  @param size     dstに読み込める最大バイト数
 *  @param result   行データ取得結果格納先
 *  @param overflow 行バイト数がsizeバイトを超えたかどうかの格納先
 *  @param err      エラーステータス格納先
 *  @pre
 *  + fp        != NULL
 *  + dst       != NULL
 *  + overflow  != NULL
 *  + size       > 1
 *
 *  @note
 *  resultは読み込みに成功した場合はdstを、ファイル終端に達した場合はNULLを指し
 *  ます。
 *
 *  overflowは戻り値trueかつ、resultがNULLでない場合のみ有効な値となります。
 *
 *  改行はCR+LFかLFに対応しています。取得した文字列にはCR, LRは含まれません。
 *
 */
bool xfile_read_line(XFile* fp, char* dst, size_t size, char** result, bool* overflow, int* err);


/** pathがディレクトリかどうかを返します。
 *
 *  @param path     ファイルパス
 *  @param is_dir   チェック結果格納先
 *  @param err      エラーステータス格納先
 *  @pre
 *  + path   != NULL
 *  + is_dir != NULL
 *  @note
 *  処理結果は、xfile_exists()と同様に3パターンに分かれます。
 *
 *  +  (戻り値 == true) && (is_dir == true)  => ディレクトリである
 *  +  (戻り値 == true) && (is_dir == false) => ディレクトリではない
 *  +  (戻り値 == false)                     => 判定不能
 */
bool xfile_is_directory(const char* path, bool* is_dir, int* err);


/** ファイルにprintf相当の書き込みを行います。
 *
 *  @param fp       有効なファイルポインタ
 *  @param nprint   書き込みたバイト数
 *  @param err      エラーステータス格納先
 *  @param fmt      フォーマット指定文字列
 *  @pre
 *  + fp     != NULL
 *  + nprint != NULL
 *  + fmt    != NULL
 */
bool xfile_printf(XFile* fp, size_t* nprint, int* err, const char* fmt, ...);


/** ファイルにvprintf相当の書き込みを行います。
 *
 *  @param fp       有効なファイルポインタ
 *  @param nprint   書き込みたバイト数
 *  @param err      エラーステータス格納先
 *  @param fmt      フォーマット指定文字列
 *  @param arg      可変引数リスト
 *  @pre
 *  + fp     != NULL
 *  + nprint != NULL
 *  + fmt    != NULL
 */
bool xfile_vprintf(XFile* fp, size_t* nprint, int* err, const char* fmt, va_list arg);


/** パスの末尾を表す文字列をdstにコピーします。
 *
 * @note
 * /foo/bar/baz.txt => baz.txt<br>
 * foo              => foo<br>
 */
char* xfile_path_name(char* dst, const char* path, size_t size);


/** エラーコードの文字列表現を返します。
 */
const char* xfile_strerror(int err);


#ifdef __cplusplus
}
#endif


#endif // picox_xfile_h_
