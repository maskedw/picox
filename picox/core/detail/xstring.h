/**
 *       @file  xstring.h
 *      @brief  標準Cライブラリstring.hの拡張定義
 *
 *    @details
 *
 *     @author  MaskedW
 *
 *   @internal
 *     Created  2016/02/26
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


#ifndef picox_core_detail_xstring_h_
#define picox_core_detail_xstring_h_


/** @addtogroup core
 *  @{
 *  @addtogroup xstring
 *  @brief 標準Cライブラリstring.hの拡張機能を提供します
 *
 *  POSIX標準や、glibc, BSD libcでは標準のCライブラリ関数を補完する多数の拡張関
 *  数が用意されています。
 *
 *  ここではそれら拡張と互換機能をもつものと、picox独自の拡張を提供しています。
 *  @{
 */


#ifdef __cplusplus
extern "C" {
#endif


/** @brief 文字列s1とs2が一致するかどうかを返します。
 *
 *  (strcmp(s1, s2) == 0)と同じです。
 */
bool x_strequal(const char* s1, const char* s2);


/** @brief 大文字小文字の違いを無視したx_strequal()です
 */
bool x_strcaseequal(const char* s1, const char* s2);


/** @brief 大文字小文字の違いを無視したstrcmp()です。
 *
 *  + https://linuxjm.osdn.jp/html/LDP_man-pages/man3/strcasecmp.3.html
 */
int x_strcasecmp(const char* s1, const char* s2);


/** @brief 大文字小文字の違いを無視したstrncmp()です。
 *
 *  + https://linuxjm.osdn.jp/html/LDP_man-pages/man3/strcasecmp.3.html
 */
int x_strncasecmp(const char* s1, const char* s2, size_t n);


/** @brief 長さ制限付きのstrstr()です
 *
 *  + https://www.freebsd.org/cgi/man.cgi?query=strnstr&sektion=3
 */
char* x_strnstr(const char* s1, const char* s2, size_t n);


/** @brief 大文字小文字の違いを無視したstrstr()です。
 *
 *  + https://linuxjm.osdn.jp/html/LDP_man-pages/man3/strstr.3.html
 */
char* x_strcasestr(const char* s1, const char* s2);


/** @brief 大文字小文字の違いを無視したx_strnstr()です
 *
 *  + https://linuxjm.osdn.jp/html/LDP_man-pages/man3/strstr.3.html
 */
char* x_strncasestr(const char* s1, const char* s2, size_t n);


/** @brief 文字列の複製をヒープから生成して返します
 *
 *  + https://linuxjm.osdn.jp/html/LDP_man-pages/man3/strdup.3.html
 *
 *  不要になった文字列はx_free()でメモリを解放してください。
 */
char* x_strdup(const char* str);


/** @brief 指定のメモリ確保関数を使用して複製した文字列を返します
 *
 *  不要になった文字列はmalloc_func()に対応するメモリ解放関数でメモリを解放して
 *  ください。
 */
char* x_strdup2(const char* str, XMallocFunc malloc_func);


/** @brief 複製する最大バイト数を指定できるstrdup()です
 *
 *  + https://linuxjm.osdn.jp/html/LDP_man-pages/man3/strdup.3.html
 */
char* x_strndup(const char* str, size_t n);


/** @brief メモリ確保関数と最大バイト数を指定して複製した文字列を返します
 *
 *
 *  不要になった文字列はmalloc_func()に対応するメモリ解放関数でメモリを解放して
 *  ください。
 */
char* x_strndup2(const char* str, size_t n, XMallocFunc malloc_func);


/** @brief 文字列を逆転させた結果を返します。
 *
 *  str自身が破壊されることに注意してください。
 */
char* x_strreverse(char* str);


/** @brief 文字列の前後からspaceに含まれる文字列を取り除いた結果を返します。
 *
 *  + str自身が破壊されることに注意してください。
 *  + space == NULLの場合はisspace()が使用されます。
 */
char* x_strstrip(char* str, const char* space);


/** @brief 文字列の前からspaceに含まれる文字列を取り除いた結果を返します。
 *
 *  + str自身が破壊されることに注意してください。
 *  + space == NULLの場合はisspace()が使用されます。
 */
char* x_strlstrip(char* str, const char* space);


/** @brief 文字列の後ろからspaceに含まれる文字列を取り除いた結果を返します。
 *
 *  + str自身が破壊されることに注意してください。
 *  + space == NULLの場合はisspace()が使用されます。
 */
char* x_strrstrip(char* str, const char* space);


/** @brief 文字列をint32_tに変換して返します。
 *
 *  + 文字列先頭の任意の数の空白(std::isspace())は無視されます。
 *  + +-の符号は数字の直前のみ許可されます。
 *  + 数字直前の"0[xX]"は16進数に、"0[bB]"は2進数に解釈されます。
 *  + 16進数、2進数解釈時に+-符号がついていた場合は変換失敗とします。
 *  + okにNULLが渡されることは許容します。
 *
 *  @param str 変換対象の文字列
 *  @param def 変換に失敗した場合に返すデフォルト値
 *  @param ok  変換に成功したかどうか
 */
int32_t x_strtoint32(const char* str, int32_t def, bool* ok);


/** @brief 文字列をuint32_tに変換して返します。
 *
 *  + -の符号がついていた場合は常時変換失敗となります。
 *  + その他の条件はx_strtoint32()を参照してください。
 */
uint32_t x_strtouint32(const char* str, uint32_t def, bool* ok);


/** @brief 文字列をfloatに変換して返します。
 *
 *  c99のstrtof()が使用できない場合はstrtod()の結果を返します。
 */
float x_strtofloat(const char* str, float def, bool* ok);


/** @brief 文字列をdoubleに変換して返します。
 *
 *  strtod()の結果を返します。
 */
double x_strtodouble(const char* str, double def, bool* ok);


/** @brief 文字列をboolに変換して返します。
 *
 *  大文字小文字の違いは無視し、("y", "yes", "true", "1")のいづれかであればtrue。
 *  ("n", "no", "false", "0")のいづれかであればfalseと解釈します。
 */
bool x_strtobool(const char* str, bool def, bool* ok);


/** @brief strpbrk()の逆走査版です。
 */
char* x_strrpbrk(const char* str, const char* accept);


/** @brief 大文字小文字の違いを無視したstrpbrk()です。
 */
char* x_strcasepbrk(const char* str, const char* accept);


/** @brief 大文字小文字の違いを無視したx_strrpbrk()です。
 */
char* x_strcaserpbrk(const char* str, const char* accept);


/** @brief strに含まれるアルファベットを全て小文字に変換して返します
 *
 *  + str自身が破壊されることに注意してください。
 */
char* x_strtolower(char* str);


/** @brief strに含まれるアルファベットを全て大文字に変換して返します
 *
 *  + str自身が破壊されることに注意してください。
 */
char* x_strtoupper(char* str);


/** @brief srcから最大でn - 1バイトをdstにコピーします
 *
 *  最大サイズを指定する標準の文字列コピー関数strncpy()は、最大サイズを超えた場
 *  合にNULL終端されないという扱いにくい仕様があります。この問題に対処するために
 *  、BSD系には非標準拡張としてstrlcpy()という関数が用意されました。
 *
 *  + n == 0の時は何もせずに0を返す
 *  + n > 0であればdstは必ずNULL終端される。
 *  + 戻り値はstrlen(src)と同じ。
 *  + 戻り値が >= nなら切り詰めが発生したことを意味する。
 *
 *  + http://www.wdic.org/w/TECH/strlcpy
 *  + https://www.freebsd.org/cgi/man.cgi?query=strlcpy&sektion=3
 */
size_t x_strlcpy(char* dst, const char* src, size_t n);


/** @brief srcから最大でdsize - strlen(dst) - 1バイトの文字列をdstに連結します
 *
 *  x_strlcpy()と同じく、BSD拡張の移植です。strncat()との違いがわかりづらいです
 *  が、以下リンク先にstrlcpy()の振る舞いについての記述があります。
 *
 *  + https://linuxjm.osdn.jp/html/LDP_man-pages/man3/strcat.3.html
 *  + https://ja.wikipedia.org/wiki/Strlcat
 *
 *  要点は以下の通りです。
 *
 *  + 戻り値はstrlen(src) + min(dsize, strlen(dst))
 *  + strncpy()はsrcから何バイトをコピーするかを指定する。strlcpy()はdstの領域の
 *    サイズを指定する
 *  + retval >= dsizeの時はデータが切り詰められたことを意味する
 */
size_t x_strlcat(char* dst, const char* src, size_t dsize);


/** @brief a,bを比較し、一致するバイト数を返します
 */
size_t x_strcountequal(const char* a, const char* b);


/** @brief 大文字小文字の違いを無視したx_strcountequal()です。
 */
size_t x_strcountcaseequal(const char* a, const char* b);


/** @brief srcをdstにコピーし、コピーした文字列の終端のポインタを返します
 *
 *  コピーした文字列の後ろに続けて処理をしたい時に、strcpy()よりもこちらを使用す
 *  ると処理に無駄がありません。
 *
 *  + https://linuxjm.osdn.jp/html/LDP_man-pages/man3/stpcpy.3.html
 */
char* x_stpcpy(char* dst, const char* src);


/** @brief dstにsrcを逆順でコピーし、コピーした文字列の終端のポインタを返します
 */
char* x_stprcpy(char* dst, const char* src);


/** @brief 長さ制限付きのx_stpcpy()です
 *
 *  + https://linuxjm.osdn.jp/html/LDP_man-pages/man3/stpncpy.3.html
 */
char* x_stpncpy(char* dst, const char* src, size_t n);


/** @brief 0埋め無し版のx_stpncpy()です
 *
 *  stpncpy()はsrcのバイト数がnより小さい場合、dstの残りを全て'\0'で埋めるという
 *  謎の仕様がありますが、こちらはコピー後のdstの末尾にだけ'\0'をセットします。
 */
char* x_stpncpy2(char* dst, const char* src, size_t n);


/** @brief 0埋め無し版のx_strncpy()です
 *
 *  strncpy()はsrcのバイト数がnより小さい場合、dstの残りを全て'\0'で埋めるという
 *  謎の仕様がありますが、こちらはコピー後のdstの末尾にだけ'\0'をセットします。
 *  strncpy()の謎の仕様にも何か意味があるのだと思うので、別の関数として用意しま
 *  した。
 */
char* x_strncpy2(char* dst, const char* src, size_t n);


/** @brief 文字列s中にはじめに文字cが現れた位置へのポインタを返します
 *
 *  strchr()は文字が見つからなかった時にNULLを返しますが、この関数はsの末尾を返
 *  します。
 *
 *  + https://linuxjm.osdn.jp/html/LDP_man-pages/man3/strchr.3.html
 */
char* x_strchrnul(const char* s, int c);


/** @brief 長さ指定付きのstrlen()です
 */
size_t x_strnlen(const char* s, size_t n);


/** @brief `"r", "r+"`等のfopen()で使用するモード指定文字列をXOpenModeに変換して返します
 */
XOpenMode x_strtomode(const char* modestr);


/** @brief 文字列sの先頭にcが続く間読み飛ばし、そのポインタを返します
 */
char* x_strskipchr(const char* s, int c);


/** @brief n1バイトの大きさを持つp1からn2バイトのp2が最初に現れた位置のポインタを返します
 *
 *  + https://linuxjm.osdn.jp/html/LDP_man-pages/man3/memmem.3.html
 */
void* x_memmem(const void* p1, size_t n1, const void* p2, size_t n2);


/** @brief memchr()の逆走査版です
 *
 *  + https://linuxjm.osdn.jp/html/LDP_man-pages/man3/memchr.3.html
 */
void* x_memrchr(const void* p, int c, size_t n);


/** @brief aとbからsizeバイトを入れ替えます。
 */
void x_memswap(void *a, void *b, size_t size);


/** @brief sizeバイトの要素n個からなる配列pの要素を逆順に並べ替えます。
 */
void x_memreverse(void *p, size_t size, size_t n);


/** @brief sizeバイトの要素n個からなる配列pの要素を右方向にshift個分回転移動させます。
 */
void x_memrrot(void *p, size_t shift, size_t size, size_t n);


/** @brief sizeバイトの要素n個からなる配列pの要素を左方向にshift個分回転移動させます。
 */
void x_memlrot(void *p, size_t shift, size_t size, size_t n);


/** @brief srcからdstにメモリの矩形コピーを行います。
 *
 *  @param dst      転送先
 *  @param src      転送元
 *  @param width    転送幅
 *  @param height   転送高さ
 *  @param dstride  転送先の次のラインまでの幅
 *  @param sstride  転送元の次のラインまでの幅
 *
 *  @attention
 *  srcとdstの領域は重なっていないことを前提とします。
 */
void x_memblt(void* dst, const void* src,
              size_t width, size_t height,
              size_t dstride, size_t sstride);


/** @brief (memcmp(p1, p2) == 0)と同じです
 */
bool x_memequal(const void* p1, const void* p2, size_t n);


/** @brief sizeバイトの領域を持つ長さlenのdstの先頭からdnバイトをsnバイトのsrcで上書きします
 *
 *  振る舞いがわかりづらいと思うので、3パターンの例を示します。<br>
 *  + dn == sn (dn = sn = 3, dst="ABCDEF", src="GHI" result => "GHIDEF")
 *  + dn >  sn (dn = 3 sn = 2, dst="ABCDEF", src="GH" result => "GHDEF")
 *  + dn <  sn (dn = 2 sn = 3, dst="ABCDEF", src="GHI" result => "GHICDEF)
 *
 *  dn < snで、sizeが足りずに、領域をずらせなかった場合はNULLを返します。
 *  それ以外の場合はdstを返します。
 *
 */
char* x_strreplace(char* dst, size_t size, size_t len, size_t dn, const char* src, size_t sn);


/** @brief nバイトの領域を持つpをランダムな値で埋めます
 */
void x_memrandom(void* p, size_t n);


/** @brief nバイトの領域を持つpをランダムなアルファベット[a-zA-Z]で埋めます
 */
void x_memrandom_alpha(void* p, size_t n);


/** @brief 真偽値に対応する"true" or "false"の文字列を返します
 */
const char* x_btos(bool cond);


/** @brief エラーコードに対応する文字列を返します
 */
const char* x_strerror(XError err);


#ifdef __cplusplus
}
#endif


/** @} end of addtogroup xstring
 *  @} end of addtogroup core
 */

#endif /* picox_core_detail_xstring_h_ */
