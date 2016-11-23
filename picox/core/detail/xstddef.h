/**
 *       @file  xstddef.h
 *      @brief  共通の型やマクロ定義等
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

#ifndef picox_core_detail_xstddef_h_
#define picox_core_detail_xstddef_h_


/** @addtogroup core
 *  @{
 *  @addtogroup xstddef
 *  @brief picoxで共通に使用する型や定数、マクロの定義
 *  @{
 */


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/** @brief 一次元配列の要素数を返します。
 */
#define X_COUNT_OF(a)      (sizeof(a) / sizeof(*(a)))


/** @brief 二次元配列の行要素数を返します。
 */
#define X_COUNT_OF_ROW(x) (sizeof(x) / sizeof(x[0]))


/** @brief 二次元配列の列要素数を返します。
 */
#define X_COUNT_OF_COL(x) (sizeof(x[0]) / sizeof(x[0][0]))


/** @brief 二次元配列の要素数を返します。
 */
#define X_COUNT_OF_2D(x)   (X_COUNT_OF_ROW(x) * X_COUNT_OF_COL(x))


/** @brief 構造体や共用体メンバのsizeofを返します。
 */
#define X_SIZEOF_MEM(s, m) (sizeof(((s*)0)->m))


/** @brief 構造体や共用体メンバの先頭からのオフセットを返します。
 */
#define X_OFFSET_OF(s, m)   ((uintptr_t)&(((s *)0)->m))


/** @brief 型typeのアライメントを返します。
 */
#define X_ALIGN_OF(type)   X_OFFSET_OF(struct { char c; type member; }, member)


/** @def    X_CONTAINER_OF
 *  @brief  複合型のメンバを指すポインタから、複合型の先頭アドレスを取得します
 *  @param  ptr    複合型typeのmemberを指すポインタ
 *  @param  type   memberをメンバに持つ複合型
 *  @param  member ptrが指す複合型のメンバ名
 *
 *  @code {.c}
 *  typedef struct Foo
 *   {
 *       int          a;
 *       int          b;
 *  } Foo;

 *  struct Foo foo;
 *  int* mem_ptr = &foo.b;
 *  Foo* foo_ptr = X_CONTAINER_OF(mem_ptr, struct Foo, b);
 *  assert(&foo == foo_ptr);
 *  @endcode
 *
 *  @note
 *  正直、使い道や使い方のイメージがしづらいかと思いますが、Linuxカーネルでは同
 *  様のマクロが定義されており、多用されています。
 *  より詳しい情報は、"linux container_of"等でググってみてください。
 */
#if (X_HAS_TYPEOF != 0) && (X_HAS_STATEMENT_EXPRESSIONS != 0)
    #define X_CONTAINER_OF(ptr, type, member)                     \
            ({                                                    \
                const X_TYPEOF(((type*)0)->member)* mptr = (ptr); \
                (type*)((char*)mptr - X_OFFSET_OF(type,member) ); \
            })
#else
    #define X_CONTAINER_OF(ptr, type, member)                     \
            ((type*) ((char*)(ptr) - X_OFFSET_OF(type, member)))
#endif


/// @cond IGNORE
#define X_STATIC_ASSERT_CAT_(a, b)          a ## b
/// @endcond IGNORE


/** @brief コンパイル時アサートを行います。
 */
#define X_STATIC_ASSERT(cond)  \
    enum { X_STATIC_ASSERT_CAT_(X_STATIC_ASSERTION_FAILED, __LINE__) = \
          sizeof( struct { int assertion_failed[(cond) ? 1: -1];})}


/** @name X_SIZEOF_XXX
 *  @brief プリプロセスで使用するためのsizeof定義です
 *
 *  プリプロセスでsizeof()は使用できないのでlimits.hのXXX_MAXの定義から型サイズ
 *  を推測しています。
 *
 *  @note
 *  Cの規格上はintのビット幅が8 or 16 or 32 or 64といった保証はないので本当は
 *  0xXXと比較してサイズを推測するのは問題がある。
 *  しかしこのライブラリは特殊なビット幅のプロセッサは対象としていないのでこれで
 *  よしとする。
 *  @{
 */
#define X_SIZEOF_CHAR       (1)
#define X_SIZEOF_SHORT      (2)

#if UINT_MAX == 0xFF
    #define X_SIZEOF_INT    (1)
#elif UINT_MAX == 0xFFFF
    #define X_SIZEOF_INT    (2)
#elif UINT_MAX == 0xFFFFFFFF
    #define X_SIZEOF_INT    (4)
#elif !defined(X_COMPILER_NO_64BIT_INT)
    #if UINT_MAX == 0xFFFFFFFFFFFFFFFF
        #define X_SIZEOF_INT    (8)
    #else
        #error unspported platform
    #endif
#else
    #error unspported platform
#endif

#if ULONG_MAX == 0xFFFFFFFF
    #define X_SIZEOF_LONG   (4)
#elif !defined(X_COMPILER_NO_64BIT_INT)
    #if ULONG_MAX == 0xFFFFFFFFFFFFFFFF
        #define X_SIZEOF_LONG   (8)
    #else
        #error unspported platform
    #endif
#else
    #error unspported platform
#endif

#if UINTPTR_MAX == 0xFFFF
    #define X_SIZEOF_INTPTR    (2)
#elif UINTPTR_MAX == 0xFFFFFFFF
    #define X_SIZEOF_INTPTR    (4)
#elif !defined(X_COMPILER_NO_64BIT_INT)
    #if UINTPTR_MAX == 0xFFFFFFFFFFFFFFFF
        #define X_SIZEOF_INTPTR    (8)
    #else
        #error unspported platform
    #endif
#else
    #error unspported platform
#endif

#if SIZE_MAX == 0xFFFF
    #define X_SIZEOF_SIZE    (2)
#elif SIZE_MAX == 0xFFFFFFFF
    #define X_SIZEOF_SIZE    (4)
#elif !defined(X_COMPILER_NO_64BIT_INT)
    #if SIZE_MAX == 0xFFFFFFFFFFFFFFFF
        #define X_SIZEOF_SIZE    (8)
    #else
        #error unspported platform
    #endif
#else
    #error unspported platform
#endif


/** @} end of name X_SIZEOF_XXX
 */


/** @name X_MSBOF_XXX
 *  @brief 各型の最上位ビットです
 *  @{
 */
#define X_MSBOF_CHAR     (X_BIT(CHAR_BIT))
#define X_MSBOF_SHORT    (USHORT_MAX  & (~(USHORT_MAX  >> 1)))
#define X_MSBOF_INT      (UINT_MAX    & (~(UINT_MAX    >> 1)))
#define X_MSBOF_LONG     (ULONG_MAX   & (~(ULONG_MAX   >> 1)))
#define X_MSBOF_INTPTR   (UINTPTR_MAX & (~(UINTPTR_MAX >> 1)))
#define X_MSBOF_SIZE     (SIZE_MAX    & (~(SIZE_MAX    >> 1)))
#define X_MSBOF_INT8     (1U << 7)
#define X_MSBOF_INT16    (1U << 15)
#define X_MSBOF_INT32    (X_UINT32_C(1) << 31)


/** @} endof name X_MSBOF_XXX
 */


/** @name X_XXX_C
 *  @brief 定数サフィックスを補完するためのマクロです。
 *
 *  末尾の`_C`はConstantを表します
 *  @{
 */

#if X_SIZEOF_LONG <= 4
    #define X_INT32_C(c)    c ## L
    #define X_UINT32_C(c)   c ## UL
#else
    #define X_INT32_C(c)    c
    #define X_UINT32_C(c)   c ## U
#endif


#if X_SIZEOF_LONG >= 8
    #define X_INT64_C(c)    c ## L
    #define X_UINT64_C(c)   c ## UL
#elif !defined(X_COMPILER_NO_LONGLONG)
    #define X_INT64_C(c)    c ## LL
    #define X_UINT64_C(c)   c ## ULL
#endif

/** @} end of name X_XXX_C
 */


/** @brief 何らかの大きさを表すのに十分なサイズを備えた符号なし整数型です
 *
 *  size_tの型は実装依存であり、例えばsizeof(size_t) == 2の環境では、32bitのアド
 *  レス空間を持つシリアルフラッシュのアドレッシングに使用できませんし、16bit以
 *  上のファイルサイズを扱えません。
 *
 *  この型はそういった外部デバイスのサイズ情報の格納にも適した型で定義しています
 *  。
 */
typedef uint32_t XSize;


/** @brief XSizeの符号あり版です。size_tに対するssize_tと同じ役割です
 */
typedef int32_t XSSize;


/** @brief 何らかのオフセットを表すのに十分な大きさをもった符号あり整数型です
 */
typedef int32_t XOffset;


/** @brief 何らかのIDを格納するための符号なし整数が型です
 *
 *  IDには決まった使い道があるわけではないですが、例えばCでC++の仮想クラスのよう
 *  な機能を実現したい時の型チェックに使用するといった例があります。
 */
typedef uint32_t XTag;


/** @brief a, b, c, dからXTagに格納するIDを生成します
 *
 *  @code {.c}
 *  XTag tag = X_MAKE_TAG('P', 'I', 'C', 'O')
 *  @endcode
 */
#define X_MAKE_TAG(a,b,c,d) \
    ( ((uint32_t)(a) << 24) \
    | ((uint32_t)(b) << 16) \
    | ((uint32_t)(c) <<  8) \
    | ((uint32_t)(d) <<  0) )


/** @brief 何らかのビットフラグを格納することを意図した型です
 */
typedef uint32_t XMode;


/** @brief XModeと同じくビットフラグを格納しますが、よりビットを意識した型です
 */
typedef uint32_t XBits;


/** @brief qsort()互換の比較関数ポインタ型です */
typedef int (*XCompareFunc)(const void* a, const void* b);


/** @brief malloc()互換のメモリ確保関数ポインタ型です */
typedef void* (*XMallocFunc)(size_t size);


/** @brief free()互換のメモリ開放関数ポインタ型です */
typedef void (*XFreeFunc)(void* ptr);


/** @brief オブジェクトを削除する関数ポインタ型です
 *
 *  XFreeFuncと同じインターフェースですが、XFreeFuncがメモリの開放だけを意図して
 *  いるのに対して、XDeleterはオブジェクトの終了処理を意識しているという、文脈の
 *  違いがあります。
 */
typedef void (*XDeleter)(void* ptr);


/** @brief 何もしないデリータです*/
void x_null_deleter(void* ptr);


/** @brief 組込み型の最大アライメント型です。
 */
typedef union XMaxAlign
{
    char c; int i; long l; void* p;
    float f; double d; long double ld; void(*fp)(void);
#ifndef X_COMPILER_NO_LONGLONG
    long long ll;
#endif
} XMaxAlign;


/** @brief 最大サイズのアライメントでsizeバイト以上の領域を持つ変数nameを定義します。
 */
#define X_DEF_MAX_ALIGNED(name, size) \
    XMaxAlign name[((size) + sizeof(XMaxAlign) - 1) / sizeof(XMaxAlign)]


/** @brief シークの起点を指定する列挙型です
 */
typedef enum
{
    /** @brief ストリームの先頭 */
    X_SEEK_SET = SEEK_SET,
    /** @brief ストリームポインタの現在地  */
    X_SEEK_CUR = SEEK_CUR,
    /** @brief ストリームサイズの末尾 */
    X_SEEK_END = SEEK_END
} XSeekMode;


/** @brief errnoの代替として使用する共通のエラーコードを表す列挙型です
 *
 *  エラーコードを共通とするか、モジュール固有とするかはどちらも一長一短があり、
 *  難しい問題です。共通化すると、インターフェースを統一しやすくなりますが、モジ
 *  ュール固有にすると、より詳細な情報を表現できます。
 *
 *  picoxでは主にファイルシステム等の抽象化レイヤのインターフェースの統一のため
 *  にこの型を使用します。
 *
 *  @note
 *  errnoを使用しない理由は、以下の理由により組込み開発では扱いにくいからです。
 *
 *  + グローバル変数であるため、マルチスレッド環境では使いづらい。(Linux等の一部
 *    実行環境ではスレッドローカル変数として実装されているので問題ない)
 *  + エラーコードとしてどの値が提供されているかはCライブラリの実装による
 */
typedef enum
{
    X_ERR_NONE = 0,             /** エラーなし */
    X_ERR_IO = 1,               /** 入出力エラー */
    X_ERR_INVALID = 2,          /** 引数が不正 */
    X_ERR_TIMED_OUT = 3,        /** 操作がタイムアウトした */
    X_ERR_BUSY = 4,             /** リソースが使用中 */
    X_ERR_AGAIN = 5,            /** 送信待ち等によりリソースが一時的に利用不可 */
    X_ERR_CANCELED = 6,         /** 操作がキャンセルされた */
    X_ERR_NO_MEMORY = 7,        /** メモリ確保に失敗した */
    X_ERR_EXIST = 8,            /** 対象がすでに存在している */
    X_ERR_NOT_READY = 9,        /** 操作を行う準備が未完了 */
    X_ERR_ACCESS = 10,          /** 対象を操作する許可がない */
    X_ERR_NO_ENTRY = 11,        /** 対象が存在しない */
    X_ERR_NOT_SUPPORTED = 12,   /** サポートされていない操作が指定された */
    X_ERR_DISCONNECTED = 13,    /** 通信相手との接続が切れた */
    X_ERR_INPROGRESS = 14,      /** 操作は実行中 */
    X_ERR_PROTOCOL = 15,        /** プロトコルエラー(操作を行う要件を満たしていない) */
    X_ERR_MANY = 16,            /** リソース生成の上限を超えている */
    X_ERR_RANGE = 17,           /** 結果が大きすぎる */
    X_ERR_BROKEN = 18,          /** 操作対象が破損している */
    X_ERR_NAME_TOO_LONG = 19,   /** 名前が長すぎる */
    X_ERR_INVALID_NAME = 20,    /** 名前が不正( 使用不可の文字が含まれている等)  */
    X_ERR_IS_DIRECTORY = 21,    /** ディレクトリである(ディレクトリを指定してはいけない操作) */
    X_ERR_NOT_DIRECTORY = 22,   /** ディレクトリではない (ディレクトリを指定すべき操作) */
    X_ERR_NOT_EMPTY = 23,       /** ディレクトリが空ではない */
    X_ERR_NO_SPACE = 24,        /** 空き容量なし */
    X_ERR_INTERNAL = 25,        /** 予期せぬ内部エラー */
    X_ERR_OTHER = 26,           /** その他のエラー */
    X_ERR_UNKNOWN = 27,         /** 不明なエラー */
} XError;


/** @brief ファイルオープン等に使用するフラグです
 *
 *  使用できる組み合わせは決まっているため、引数にはXOpenModeを使用してください
 *  。この定義を直接使用するのはfilesystem等の下位モジュールのです。
 */
typedef enum
{
    X_OPEN_FLAG_READ = (1 << 0),        /** 読み込み */
    X_OPEN_FLAG_WRITE = (1 << 1),       /** 書き込み */
    X_OPEN_FLAG_READ_WRITE = X_OPEN_FLAG_READ | X_OPEN_FLAG_WRITE, /** 読み書き */
    X_OPEN_FLAG_APPEND = (1 << 2),      /** 追記 */
    X_OPEN_FLAG_TRUNCATE = (1 << 3),    /** 既存の内容の破棄 */
} XOpenFlag;


/** @brief ファイルオープン等のモードです
 *
 *  fopen()で使用可能な`"r","r+","w","w+","a","a+"`に対応しています。
 *  picoxでは、"rb"等の指定によるバイナリモード、テキストモードという区別はして
 *  おらず、常にバイナリモードの動作を行います。
 *
 *  各モードの動作は下記リンクを参考にしてください。
 *
 *  + https://linuxjm.osdn.jp/html/LDP_man-pages/man3/fopen.3.html
 *  + https://docs.oracle.com/cd/E19205-01/820-2985/loc_io/9_3.htm
 *
 *  @note
 *  ANSI-Cで定義された各モードの大雑把な動きはそれほど複雑ではないですが、シーク
 *  や追記が絡んだ場合の詳細な動作に対して、プラットフォームごとの互換性は期待で
 *  きません。そもそも未定義の部分も多いように思います。
 *
 *  picoxでは基本的にglibcの動作を踏襲するように各モジュールを実装していますが、
 *  複雑な操作を行う場合は、SEEK_SET以外のシークと、追記は使わない方が無難かもし
 *  れません。
 */
typedef enum
{
    /** @brief `"r"` 読み込み
     *
     *  + 対象が存在しない場合はエラーになります。
     */
    X_OPEN_MODE_READ = X_OPEN_FLAG_READ,

    /** @brief `"w"` 上書き書き込み
     *
     *  + 対象が存在しない場合は新規作成されます。
     *  + 既存のデータはゼロに切り詰めます。
     */
    X_OPEN_MODE_WRITE = X_OPEN_FLAG_WRITE | X_OPEN_FLAG_TRUNCATE,

    /** @brief `"a"` 追記書き込み
     *
     *  + 対象が存在しない場合は新規作成されます。
     *  + 書き込み位置の初期値は末尾にセットされます。
     *  + 書き込みは常に末尾に行われます。
     */
    X_OPEN_MODE_APPEND = X_OPEN_FLAG_WRITE | X_OPEN_FLAG_APPEND,

    /** @brief `"r+"` 読み書き
     *
     *  + 対象が存在しない場合はエラーになります。
     */
    X_OPEN_MODE_READ_PLUS = X_OPEN_FLAG_READ_WRITE,


    /** @brief `"w+"` 上書き読み書き
     *
     *  + 対象が存在しない場合は新規作成されます。
     *  + 既存のデータはゼロに切り詰めます。
     */
    X_OPEN_MODE_WRITE_PLUS = X_OPEN_FLAG_READ_WRITE | X_OPEN_FLAG_TRUNCATE,


    /** @brief `"a+"` 追記読み書き
     *
     *  + 対象が存在しない場合は新規作成されます。
     *  + 読み込み位置の初期値は先頭にセットされます。
     *  + 書き込みは常に末尾に行われます。
     */
    X_OPEN_MODE_APPEND_PLUS = X_OPEN_FLAG_READ_WRITE | X_OPEN_FLAG_APPEND,
    X_OPEN_MODE_UNKNOWN = 0xFF,
} XOpenMode;


/** @see X_CONF_FILE_PATH_MAX
 */
#define X_PATH_MAX (X_CONF_FILE_PATH_MAX)


/** @see X_CONF_FILE_NAME_MAX
 */
#define X_NAME_MAX (X_CONF_FILE_NAME_MAX)


/** @see X_CONF_LINE_MAX
 */
#define X_LINE_MAX (X_CONF_LINE_MAX)


/** @see X_CONF_XFS_TYPE
 */
#define X_XFS_TYPE  (X_CONF_XFS_TYPE)


#ifdef __cplusplus
}
#endif /* __cplusplus */


/** @} end of addtogroup xstddef
 *  @} end of addtogroup core
 */


#endif /* picox_core_detail_xstddef_h_ */
