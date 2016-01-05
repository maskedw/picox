/**
 *       @file  stream.h
 *      @brief  入出力を抽象化するストリームモジュール
 *
 *    @details
 *    プログラムは入力を受け取り出力を行うことを目的としていますが、入出力先を固
 *    定していまうと、再利用性に問題があります。例えば、入出力先にはメモリ、ファ
 *    イル、UART, シリアルフラッシュ等が考えられますが、それらをすべて別の関数で
 *    実装するのは大変ですし、ライブラリ側としては、メモリ、ファイル以外の入出力
 *    先は限定できないのが普通です。
 *
 *    そこで、ストリームという抽象型に対して入出力を行うことでこの問題を解決する
 *    ことができます。
 *    標準CではFILE*がストリームの役割を担いますが、FILEストリームは基本的にファ
 *    イルシステムや実行環境依存の前提条件が多いため、組込み環境では扱いにくいも
 *    のです。
 *
 *    このライブラリが提供する簡易的なストリーム型を使用することで、環境に依存せ
 *    ずにストリームの恩恵を受けることができます。
 *    独自のストリームを定義したい場合はstream.cにある、XMemStreamの実装を参考に
 *    してみてください。
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

#ifndef picox_core_detail_stream_h_
#define picox_core_detail_stream_h_


/** @defgroup X_STREAM_FUNCTION_POINTERS
 *
 *  ストリームにセットする関数ポインタ型のグループです。戻り値のint型はエラース
 *  テータスを返し、0は正常終了、0以外はエラーを表します。
 *  @{
 */

/** ストリームからの読み出し関数ポインタ型です
 *
 *  dstにストリームからsizeバイトの読み出しを試み、nreadには実際に読み出せたバイ
 *  ト数を格納してください。
 */
typedef int (*XStreamReadFunc)(void* driver, void* dst, size_t size, size_t* nread);


/** ストリームへの書き込み関数ポインタ型です
 *
 *  srcからストリームにsizeバイトの書き込みを試み、nwrittenには実際に書き込めた
 *  バイト数を格納してください。
 */
typedef int (*XStreamWriteFunc)(void* driver, const void* src, size_t size, size_t* nwritten);


/** ストリームの現在位置を移動する関数ポインタ型です
 *
 *  modeに応じて、offsetバイトの移動を行ってください。
 */
typedef int (*XStreamSeekFunc)(void* driver, XOffset offset, XSeekMode mode);


/** ストリームの現在位置を取得する関数ポインタ型です
 */
typedef int (*XStreamPosFunc)(void* driver, XSize* pos);


/** ストリームのバイト数を取得する関数ポインタ型です
 */
typedef int (*XStreamSizeFunc)(void* driver, XSize* size);


/** ストリームのバッファリングをフラッシュする関数ポインタ型です
 */
typedef int (*XStreamFlushFunc)(void* driver);


/** ストリームのフラッシュと、リソースの開放を行う関数ポインタ型です
 */
typedef int (*XStreamCloseFunc)(void* driver);


/** ストリームが返したエラーステータスに対応する文字列を返す関数ポインタ型です
 */
typedef const char* (*XStreamErrorStringFunc)(const void* driver, int errcode);


/** @}*/ /* endgroup X_STREAM_FUNCTION_POINTERS */


/** ストリームを表す抽象型です
 *
 *  独自ストリームを定義する場合は、先頭メンバにこの型を配置してください(スト
 *  リーム型から独自形へ直接キャスト可能にするため)。
 *
 *  独自ストリームの初期化は、はじめにxstream_init()で初期化を行ったあとに必要な
 *  変数セットしてください。
 *  すべての関数ポインタをセットする必要はありません。例えばバッファリングをおこ
 *  わないストリームなら、flush_funcの設定は不要です。
 *  未設定の関数ポインタは初期化時に何も行わず、常に成功を返すデフォルト関数がセ
 *  ットされています。
 */
typedef struct XStream
{
    /** ストリームの識別ID
     *  独自ストリームを定義するときに、このタグをストリームを識別できるようにセ
     *  ットしておくと、ストリームの実際の型に応じた処理の振り分けができて、便利
     *  な場合があります。
     *  また、このメンバは定義位置は先頭から移動させないことを保証します。任意の
     *  型の先頭メンバに同じくtagを定義しておいて、void*からタグ値を取り出すとい
     *  う使い型が可能です。
     */
    XTag                    tag;

    /** 関数ポインタの引数に使用する任意の引数 */
    void*                   driver;

    /** 最後のエラーステータスの格納先 */
    int                     error;
    XStreamReadFunc         read_func;
    XStreamWriteFunc        write_func;
    XStreamCloseFunc        close_func;
    XStreamFlushFunc        flush_func;
    XStreamSeekFunc         seek_func;
    XStreamPosFunc          pos_func;
    XStreamSizeFunc         size_func;
    XStreamErrorStringFunc  error_string_func;
} XStream;
#define X_STREAM_TAG        (X_MAKE_TAG('X', 'S', 'T', 'R'))


/** メモリに対して入出力を行うストリーム型です
 */
typedef struct XMemStream
{
    XStream         stream;
    uint8_t*        mem;
    XSize           pos;
    XSize           size;
    XSize           capacity;
} XMemStream;
#define X_MEMSTREAM_TAG     (X_MAKE_TAG('X', 'M', 'M', 'S'))


/** ストリームの初期化を行います
 */
void xstream_init(XStream* self);


/** ストリームからdstにsizeバイトの読み出しを試みます
 *
 *  nreadには読み出しに成功したバイト数がセットされます。
 *
 *  @retval == 0 正常終了
 *  @retval != 0 エラー
 */
int xstream_read(XStream* self, void* dst, size_t size, size_t* nread);


/** ストリームにsrcからsizeバイトの書き込みを試みます
 *
 *  nwrittenには書き込みに成功したバイト数がセットされます。
 *
 *  @retval == 0 正常終了
 *  @retval != 0 エラー
 */
int xstream_write(XStream* self, const void* src, size_t size, size_t* nwritten);


/** ストリームの終了処理を行います
 *
 *  @retval == 0 正常終了
 *  @retval != 0 エラー
 */
int xstream_close(XStream* self);


/** ストリームの終了処理を行います
 *
 *  @retval == 0 正常終了
 *  @retval != 0 エラー
 */
int xstream_flush(XStream* self);


/** ストリームの現在位置を移動します
 *
 *  @retval == 0 正常終了
 *  @retval != 0 エラー
 */
int xstream_seek(XStream* self, XOffset offset, XSeekMode mode);


/** ストリームの現在位置をposにセットします
 *
 *  @retval == 0 正常終了
 *  @retval != 0 エラー
 */
int xstream_pos(XStream* self, XSize* pos);


/** ストリームのバイト数をsizeにセットします
 *
 *  @retval == 0 正常終了
 *  @retval != 0 エラー
 */
int xstream_size(XStream* self, XSize* size);


/** 最後のエラー値を返します
 */
int xstream_error(const XStream* self);


/** エラー値の文字列表現を返します
 */
const char* xstream_error_string(const XStream* self, int errcode);


/** cをunsigned charにキャストしてストリームに書き込みます
 *
 *  エラー or ストリーム終端に達していた場合はEOFを返し、正常終了ならc自身を返し
 *  ます。
 */
int xstream_putc(XStream* self, int c);


/** ストリームから1バイトを読みだし、intにキャストして返します
 *
 *  エラー or ストリーム終端に達していた場合はEOFを返します。
 */
int xstream_getc(XStream* self);


/** ストリームから1行を取り出します
 *
 *  ストリームにはテキストデータが格納されていることが前提です。
 *  行サイズがsizeを超えてる場合は、size - 1までのデータが格納され(末尾は常に
 *  '\0'終端される)、overflowがセットされます。
 *  resultには正常終了時には、dst自身が、エラー時にはNULLがセットされます。
 */
int xstream_read_line(XStream* self, char* dst, size_t size, char** result, bool* overflow);


/** ストリームにvprintf形式の出力を行います
 *
 *  @retval >= 0 書き込んだバイト数
 *  @retval < 0  エラー
 */
int xstream_printf(XStream* self, const char* fmt, ...);


/** ストリームにvprintf形式の出力を行います
 *
 *  @retval >= 0 書き込んだバイト数
 *  @retval < 0  エラー
 */
int xstream_vprintf(XStream* self, const char* fmt, va_list args);


/** メモリストリームを初期化します
 *
 *  @param mem      ストリーム対象のメモリ先頭アドレス
 *  @param size     初期有効バイト数
 *  @param capacity writeやseekで拡張可能なバイト数
 *
 *  例えば、sizeが10でcapacityが20の時、readは10バイトまでしか行えませんが、
 *  writeで5バイト追記した場合は、sizeが拡張され、15バイトまでreadできるようにな
 *  ります。さらに6バイトを追記しようとすると、capacityは20なので、最後の1バイト
 *  は捨てられ、sizeは20となり、capacity以上の拡張を行うことはできません。
 */
void xmemstream_init(XMemStream* self, void* mem, size_t size, size_t capacity);


/** tagがXMemStreamと一致していなければNULLを返します
 */
static inline XMemStream* x_memstream_cast(XStream* stream)
{
    X_ASSERT(stream);
    return (stream->tag == X_MEMSTREAM_TAG) ? (XMemStream*)stream : NULL;
}


#endif /* picox_core_detail_stream_h_ */
