/**
 *       @file  xstddef.h
 *      @brief  共通の型定義等
 *
 *    @details
 *
 *     @author  MaskedW
 *
 *   @internal
 *     Created  2015/06/18
 * ===================================================================
 */

/*
 * License: MIT license
 * Copyright (c) <2015> <MaskedW [maskedw00@gmail.com]>
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of self software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and self permission notice shall be
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

#ifndef picox_core_detail_types_h_
#define picox_core_detail_types_h_


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/** 何らかの大きさを表すのに十分なサイズを備えた符号なし整数型です
 *
 *  size_tの型は実装依存であり、例えばsizeof(size_t) == 2の環境では、32bitのアド
 *  レス空間を持つシリアルフラッシュのアドレッシングに使用できませし、16bit以上
 *  のファイルサイズを扱えません。
 *  この型はそういった問題に対処するために使用します。
 */
typedef uint32_t XSize;


/** XSizeの符号あり版です。size_tに対するssize_tと同じ役割です
 */
typedef int32_t XSSize;


/** 何らかのオフセットを表すのに十分な大きさをもった符号あり整数型です
 */
typedef int32_t XOffset;


/** 何らかのIDを格納するための符号なし整数が型です
 */
typedef uint32_t XTag;

/** a, b, c, dからXTagに格納するIDを生成します
 *
 *  @code
 *  XTag tag = X_MAKE_TAG('P', 'I', 'C', 'O')
 *  @endcode
 */
#define X_MAKE_TAG(a,b,c,d) \
    ( ((uint32_t)(a) << 24) \
    | ((uint32_t)(b) << 16) \
    | ((uint32_t)(c) <<  8) \
    | ((uint32_t)(d) <<  0) )


/** システム時刻を格納するための型です
 *
 *  POSIX互換システム風にUNIX時間1970年1月1日0時0分0秒(ただしタイムゾーンは考慮
 *  しない)からの経過秒数を表します。
 *  旧いシステムではtime_tは32bit符号ありで実装されることが多く、2038年問題があ
 *  りますが、XTimeは符号なしなので2106年まで保持可能うです。
 */
typedef uint32_t XTime;


/** 何らかのビットフラグを格納することを意図した型です
 */
typedef uint32_t XMode;


/** qsort()互換の比較関数ポインタ型です */
typedef int (*XCompareFunc)(const void* a, const void* b);


/** malloc()互換のメモリ確保関数ポインタ型です */
typedef void* (*XMallocFunc)(size_t size);


/** free()互換のメモリ確保関数ポインタ型です */
typedef void (*XFreeFunc)(void* ptr);


/** 組込み型の最大アライメント型です。
 */
typedef union XMaxAlign
{
    char c; int i; long l; long long ll; void* p;
    float f; double d; long double ld; void(*fp)(void);
} XMaxAlign;


/** 最大サイズのアライメントでsizeバイト以上の領域を持つ変数nameを定義します。
 */
#define X_DEF_MAX_ALIGNED(name, size) \
    XMaxAlign name[((size) + sizeof(XMaxAlign) - 1) / sizeof(XMaxAlign)]


/** シークの起点を指定する列挙型です
 */
typedef enum
{
    X_SEEK_SET = SEEK_SET, /** 先頭    */
    X_SEEK_CUR = SEEK_CUR, /** 現在地  */
    X_SEEK_END = SEEK_END, /** 終端    */
} XSeekMode;


/** 共通のエラーコードを表す列挙型です
 *
 *  エラーコードを共通とするか、モジュール固有とするかはどちらも一長一短がありま
 *  す。共通化すると、インターフェースを統一しやすくなりますが、モジュール固有に
 *  すると、より詳細な情報を表現できます。
 *  picoxでは主にファイルシステム等の抽象化レイヤのインターフェースの統一のため
 *  にこの型を使用します。
 */
typedef enum
{
    X_ERR_NONE,             /** エラーなし */
    X_ERR_IO,               /** 入出力エラー */
    X_ERR_INVALID,          /** 引数が不正 */
    X_ERR_TIMED_OUT,        /** 操作がタイムアウトした */
    X_ERR_BUSY,             /** リソースが使用中 */
    X_ERR_AGAIN,            /** 送信待ち等によりリソースが一時的に利用不可 */
    X_ERR_CANCELED,         /** 操作がキャンセルされた */
    X_ERR_NO_MEMORY,        /** メモリ確保に失敗した */
    X_ERR_EXIST,            /** 対象がすでに存在している */
    X_ERR_NOT_READY,        /** 操作を行う準備が未完了 */
    X_ERR_ACCESS,           /** 対象を操作する許可がない */
    X_ERR_NO_ENTRY,         /** 対象が存在しない */
    X_ERR_NOT_SUPPORTED,    /** サポートされていない操作が指定された */
    X_ERR_DISCONNECTED,     /** 通信相手との接続が切れた */
    X_ERR_INPROGRESS,       /** 操作は実行中 */
    X_ERR_PROTOCOL,         /** プロトコルエラー */
    X_ERR_MANY,             /** リソース生成の上限を超えている */
    X_ERR_BROKEN,           /** 操作対象が破損している */
    X_ERR_INTERNAL,         /** 予期せぬ内部エラー */
    X_ERR_OTHER,            /** その他のエラー */
    X_ERR_UNKNOWN,          /** 不明なエラー */
} XError;


#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* picox_core_detail_types_h_ */
