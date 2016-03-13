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


/** time_tの代替をするシステム時刻を格納するための型です
 *
 *  POSIX互換システム風にUNIX時間1970年1月1日0時0分0秒(ただしタイムゾーンは考慮
 *  しない)からの経過秒数を表します。
 *  旧いシステムではtime_tは32bit符号ありで実装されることが多く、2038年問題があ
 *  りますが、XTimeは符号なしなので2106年まで保持可能です。
 */
typedef uint32_t XTime;


/** struct timevalの代替をする高精度のシステム時刻を格納するための型です
 */
typedef struct
{
    XTime   tv_sec;     /** 秒 */
    int32_t tv_usec;    /** マイクロ秒 */
} XTimeVal;


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


/** errnoの代替として使用する共通のエラーコードを表す列挙型です
 *
 *  エラーコードを共通とするか、モジュール固有とするかはどちらも一長一短がありま
 *  す。共通化すると、インターフェースを統一しやすくなりますが、モジュール固有に
 *  すると、より詳細な情報を表現できます。
 *  picoxでは主にファイルシステム等の抽象化レイヤのインターフェースの統一のため
 *  にこの型を使用します。
 *
 *  @note
 *  errnoを使用しない理由は、以下の理由により組込み開発では扱いにくいからです。
 *
 *  + グローバル変数であるため、マルチスレッド環境では使いづらい。(Linux等の環境
 *    ではスレッドローカルとして実装されているので問題ない)
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
    X_ERR_OTHER = 25,           /** その他のエラー */
    X_ERR_UNKNOWN = 27,         /** 不明なエラー */
} XError;


/** ファイルオープン等に使用するフラグです
 *
 *  使用できる組み合わせは決まっているため、引数にはXOpenModeを使用してください
 *  。この定義は主に内部実装用です。
 */
typedef enum
{
    X_OPEN_FLAG_READ = (1 << 0),        /** 読み込み */
    X_OPEN_FLAG_WRITE = (1 << 1),       /** 書き込み */
    X_OPEN_FLAG_READ_WRITE = X_OPEN_FLAG_READ | X_OPEN_FLAG_WRITE, /** 読み書き */
    X_OPEN_FLAG_APPEND = (1 << 2),      /** 追記 */
    X_OPEN_FLAG_TRUNCATE = (1 << 3),    /** 既存の内容の破棄 */
} XOpenFlag;


/** ファイルオープン等のモードです
 *
 *  fopen()で使用可能な`"r","r+","w","w+","a","a+"`に対応しています。
 *  picoxでは、"rb"等の指定によるバイナリモード、テキストモードという区別はして
 *  おらず、常にバイナリモードの動作を行います。
 *
 *  各モードの動作は以下リンク先によくまとまっています。
 *
 *  https://docs.oracle.com/cd/E19205-01/820-2985/loc_io/9_3.htm
 */
typedef enum
{
    /** `"r"` 読み込み
     *
     *  + 対象が存在しない場合はエラーになります。
     */
    X_OPEN_MODE_READ = X_OPEN_FLAG_READ,

    /** `"w"` 上書き書き込み
     *
     *  + 対象が存在しない場合は新規作成されます。
     *  + 既存のデータはゼロに切り詰めます。
     */
    X_OPEN_MODE_WRITE = X_OPEN_FLAG_WRITE | X_OPEN_FLAG_TRUNCATE,

    /** `"a"` 追記書き込み
     *
     *  + 対象が存在しない場合は新規作成されます。
     *  + 書き込み位置の初期値が末尾にセットされます。
     */
    X_OPEN_MODE_APPEND = X_OPEN_FLAG_WRITE | X_OPEN_FLAG_APPEND,

    /** `"r+"` 読み書き
     *
     *  + 対象が存在しない場合はエラーになります。
     */
    X_OPEN_MODE_READ_PLUS = X_OPEN_FLAG_READ_WRITE,


    /** `"w+"` 上書き読み書き
     *
     *  + 対象が存在しない場合は新規作成されます。
     *  + 既存のデータはゼロに切り詰めます。
     */
    X_OPEN_MODE_WRITE_PLUS = X_OPEN_FLAG_READ_WRITE | X_OPEN_FLAG_TRUNCATE,


    /** `"a+"` 追記読み書き
     *
     *  + 対象が存在しない場合は新規作成されます。
     *  + 読み書き位置の初期値が末尾にセットされます。
     */
    X_OPEN_MODE_APPEND_PLUS = X_OPEN_FLAG_READ_WRITE | X_OPEN_FLAG_APPEND,
    X_OPEN_MODE_UNKNOWN = 0xFF,
} XOpenMode;


#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* picox_core_detail_types_h_ */
