/**
 *       @file  xstream.h
 *      @brief  入出力を抽象化するストリーム定義
 *
 *    @details
 *
 *     @author  MaskedW
 *
 *   @internal
 *     Created  2016/01/03
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

#ifndef picox_core_detail_xstream_h_
#define picox_core_detail_xstream_h_


/** @addtogroup core
 *  @{
 *  @addtogroup xstream
 *  @brief 入出力を抽象化するストリーム機能を提供します
 *
 *  プログラムの目的とは、どこかから入力を受け取り、処理をし、どこかへ出力をする
 *  ことで、C言語ではそれを関数で実現します。
 *
 *  例えば組込みプログラムでの入出力先は、メモリ、ファイル、UART、シリアルフラッ
 *  シュ等です。(ファイルという概念が既に入出力を抽象化したものであることにも注
 *  目)
 *
 *  入出力先が異なるだけで、内部処理が同じ関数を全て用意することは困難で、ライブ
 *  ラリ実装者としては、精々メモリとファイル用位までしかインターフェースを限定で
 *  きません。
 *
 *  そこで、ストリームという抽象型に対して入出力を行うことでこの問題を解決する
 *  ことができます。
 *
 *  標準CではFILEがストリームの役割を担いますが、FILEストリームは基本的にファイ
 *  ルシステムや実行環境依存の前提条件が多いため、組込み環境では扱いにくいもので
 *  す。
 *
 *  picoxが提供する簡易的なストリームを入出力に使用することで、入出力先に依存し
 *  ない関数を実装できます。
 *
 *  基本的なストリームはpicoxが提供しますが、独自のストリームを定義したい場合は
 *  、まずはXMemStreamの実装を参考にしてみてください。
 *  @{
 */


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/** @name  stream_function_pointers
 *  @brief ストリームにセットする関数ポインタ型のグループです
 *
 *  戻り値のint型はエラーステータスを返し、0は正常終了、0以外はエラーを表します
 *  。
 *  @{
 */


/** @brief ストリームからの読み出し関数ポインタ型です
 *
 *  dstにストリームからsizeバイトの読み出しを試み、nreadには実際に読み出せたバイ
 *  ト数を格納してください。
 */
typedef int (*XStreamReadFunc)(void* driver, void* dst, size_t size, size_t* nread);


/** @brief ストリームへの書き込み関数ポインタ型です
 *
 *  srcからストリームにsizeバイトの書き込みを試み、nwrittenには実際に書き込めた
 *  バイト数を格納してください。
 */
typedef int (*XStreamWriteFunc)(void* driver, const void* src, size_t size, size_t* nwritten);


/** @brief ストリームの現在位置を移動する関数ポインタ型です
 *
 *  modeに応じて、offsetバイトの移動を行ってください。
 */
typedef int (*XStreamSeekFunc)(void* driver, XOffset offset, XSeekMode mode);


/** @brief ストリームの現在位置を取得する関数ポインタ型です
 */
typedef int (*XStreamTellFunc)(void* driver, XSize* pos);


/** @brief ストリームのバイト数を取得する関数ポインタ型です
 */
typedef int (*XStreamSizeFunc)(void* driver, XSize* size);


/** @brief ストリームのバッファリングをフラッシュする関数ポインタ型です
 */
typedef int (*XStreamFlushFunc)(void* driver);


/** @brief ストリームのフラッシュと、リソースの開放を行う関数ポインタ型です
 */
typedef int (*XStreamCloseFunc)(void* driver);


/** @brief ストリームが返したエラーステータスに対応する文字列を返す関数ポインタ型です
 */
typedef const char* (*XStreamErrorStringFunc)(int errcode);


/** @} end of name stream_function_pointers
 */


/** @brief XStreamインターフェースのvtableです
 *
 *  XStreamVTableの各関数(仮想関数)インターフェースの規約に沿った関数でvtableを
 *  初期化することで、XStreamを引数に受け取る関数の入出力先をカスタマイズするこ
 *  とができます。
 *  仮想関数の規約や、XStreamの初期化方法は、各関数ポインタのtypedef定義のコメン
 *  トと、XMemStream等のpicoxのコードをサンプルにしてください。
 *
 *  すべての関数ポインタをセットする必要はありません。例えば入力専用のストリーム
 *  であれば、XStreamWriteFuncの設定は不要です。
 */
typedef struct XStreamVTable
{
    const char*             m_name;
    XStreamReadFunc         m_read_func;
    XStreamWriteFunc        m_write_func;
    XStreamCloseFunc        m_close_func;
    XStreamFlushFunc        m_flush_func;
    XStreamSeekFunc         m_seek_func;
    XStreamTellFunc         m_tell_func;
    XStreamErrorStringFunc  m_error_string_func;
} XStreamVTable;


/** @brief 入出力ストリームを表すインターフェース型です
 */
typedef struct XStream
{
    X_DECLEAR_RTTI(XStreamVTable);

    /** 最後のエラーステータスの格納先 */
    int                     m_error;
} XStream;


/** @brief ストリームの初期化を行います
 */
void xstream_init(XStream* self);


/** @brief ストリームからdstにsizeバイトの読み出しを試みます
 *
 *  nreadには読み出しに成功したバイト数がセットされます。
 *
 *  @retval == 0 正常終了
 *  @retval != 0 エラー
 */
int xstream_read(XStream* self, void* dst, size_t size, size_t* nread);


/** @brief ストリームにsrcからsizeバイトの書き込みを試みます
 *
 *  nwrittenには書き込みに成功したバイト数がセットされます。
 *
 *  @retval == 0 正常終了
 *  @retval != 0 エラー
 */
int xstream_write(XStream* self, const void* src, size_t size, size_t* nwritten);


/** @brief ストリームの終了処理を行います
 *
 *  @retval == 0 正常終了
 *  @retval != 0 エラー
 */
int xstream_close(XStream* self);


/** @brief ストリームの終了処理を行います
 *
 *  @retval == 0 正常終了
 *  @retval != 0 エラー
 */
int xstream_flush(XStream* self);


/** @brief ストリームの現在位置を移動します
 *
 *  @retval == 0 正常終了
 *  @retval != 0 エラー
 */
int xstream_seek(XStream* self, XOffset offset, XSeekMode mode);


/** @brief ストリームの現在位置をposに格納します
 *
 *  @retval == 0 正常終了
 *  @retval != 0 エラー
 */
int xstream_tell(XStream* self, XSize* pos);


/** @brief 最後のエラー値を返します
 */
int xstream_error(const XStream* self);


/** @brief エラー値の文字列表現を返します
 */
const char* xstream_error_string(const XStream* self, int errcode);


/** @brief cをunsigned charにキャストしてストリームに書き込みます
 *
 *  エラー or ストリーム終端に達していた場合はEOFを返し、正常終了ならc自身を返し
 *  ます。
 */
int xstream_putc(XStream* self, int c);


/** @brief ストリームから1バイトを読みだし、intにキャストして返します
 *
 *  エラー or ストリーム終端に達していた場合はEOFを返します。
 */
int xstream_getc(XStream* self);


/** @brief ストリームから1行を取り出します
 *
 *  streamから最大でsize - 1バイトの文字をdstに格納します。読み込みはEOFまたは改
 *  行文字('\n')まで行われます。
 *
 *  std::fgetsとは異なり、改行文字はバッファに含まれません。また、CR('\r')は読み
 *  捨てするので、Mac形式の改行コードには対応していません。
 *
 *  + resultはEOFの時はNULLが、それ以外の時はdstがセットされます。
 *  + overflowは1行がsizeを超えた時にtrueがセットされます。
 *
 *  result, overflowのセットが不要な場合はNULLを渡してください。
 *
 *  真面目に処理をする場合、以下の4つのパターンを適切に判定する必要があります。
 *
 *  + 正常終了       ((戻り値 0) && (result != NULL))
 *  + EOF            ((戻り値 0) && (result == NULL))
 *  + オーバーフロー ((戻り値 0) && (overflow == true))
 *  + IOエラー       (戻り値 非0)
 *
 *   @code {.c}
 *   // サンプルコード
 *   for (;;)
 *   {
 *       err = xstream_gets(stream, buf, sizeof(buf), result, overflow);
 *       X_BREAK_IF(err);
 *       X_BREAK_IF(!result); // EOF
 *       if (overflow)
 *       {
 *           do
 *           {
 *               // オーバーフローした行を読み捨てる。
 *               err = xstream_gets(st, buf, sizeof(buf), result, overflow);
 *               X_BREAK_IF(err);
 *           } while (overflow);
 *       }
 *       X_BREAK_IF(err);
 *   }
 *   @endcode
 */
int xstream_gets(XStream* self, char* dst, size_t size, char** result, bool* overflow);


/** @brief ストリームにvprintf形式の出力を行います
 *
 *  @retval >= 0 書き込んだバイト数
 *  @retval < 0  エラー
 */
int xstream_printf(XStream* self, const char* fmt, ...) X_PRINTF_ATTR(2, 3);


/** @brief ストリームにvprintf形式の出力を行います
 *
 *  @retval >= 0 書き込んだバイト数
 *  @retval < 0  エラー
 */
int xstream_vprintf(XStream* self, const char* fmt, va_list args);


#ifdef __cplusplus
}
#endif /* __cplusplus */


/** @} end of addtogroup xstream
 *  @} end of addtogroup core
 */


#endif /* picox_core_detail_xstream_h_ */
