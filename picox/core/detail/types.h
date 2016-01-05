/**
 *       @file  xcore.h
 *      @brief
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


/** ファイルサイズを表現するために十分なサイズを備えた符号なし整数型です */
typedef uint32_t XFileSize;


/** 何らかの大きさを表すのに十分なサイズを備えた符号なし整数型です
 *
 *  size_tの型は実装依存であり、例えばsizeof(size_t) == 2の環境では、32bitのアド
 *  レス空間を持つシリアルフラッシュのアドレッシングに使用できません。
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


/** qsort()互換の比較関数ポインタ型です */
typedef int (*XCompareFunc)(const void* a, const void* b);


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
    X_SEEK_SET, /** 先頭    */
    X_SEEK_CUR, /** 現在地  */
    X_SEEK_END, /** 終端    */
} XSeekMode;


#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* picox_core_detail_types_h_ */
