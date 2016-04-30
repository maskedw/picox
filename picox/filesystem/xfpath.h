/**
 *       @file  xfpath.h
 *      @brief  ファイルパス操作ユーティリティ定義
 *
 *    @details
 *
 *     @author  MaskedW
 *
 *   @internal
 *     Created  2016/02/28
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

#ifndef picox_filesystem_xfpath_h_
#define picox_filesystem_xfpath_h_


#include <picox/core/xcore.h>


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/** @addtogroup filesystem
 *  @{
 *  @addtogroup  xfpath
 *  @brief ファイルパス文字列の操作を提供します
 *
 *  パスから親ディレクトリを取り出す。パスを連結するといった、地味で面倒な作業を
 *  補助します。
 *
 *  非対応項目
 *  + マルチバイト文字
 *  + C:等のドライブレター
 *  + '/'以外のディレクトリセパレータ
 *  @{
 */


/** @brief sizeバイトの領域を持つパスp1とパスp2を連結して返します
 *
 *  連結した結果がsizeバイトに収まらなかった場合はNULLを返します。
 *
 *  @pre
 *  p1 != NULL
 *  p2 != NULL
 *  size > 0
 *
 *  @code {.txt}
 *  p1 = "/foo/bar"
 *  p2 = "baz/hello.txt"
 *  ret = "/foo/bar/baz/hello.txt"
 *  @endcode
 */
char* xfpath_join(char* p1, const char* p2, size_t size);


/** @brief パス中に含まれる".."や"."を展開した結果を返します
 *
 *  結果が最上位ディレクトリを超えた場合はNULLを返します。
 *
 *  @pre
 *  path != NULL
 *
 *  @code {.txt}
 *  path = "foo/bar/.././././baz"
 *  ret = "foo/baz"
 *  path = "foo/bar/../../baz"
 *  ret = "foo"
 *  path = "../"
 *  ret = NULL
 *  @endcode
 */
char* xfpath_resolve_dot(char* path);


/** @brief パスを正規化します
 *
 *  パスの正規化とは以下の内容を指します。
 *  + ".", ".."の解決
 *  + 相対パスから絶対パスへの変換
 *  + 余分なディレクトリセパレータの除去
 *
 *  @param dst  正規化したパスの格納先
 *  @param size dstが指す領域のバイト数
 *  @param path 正規化を行うパス
 *  @param cwd  絶対パスの基準とするパス(正規化済みであること)
 *
 *  @retval X_ERR_NAME_TOO_LONG 展開後のサイズがsizeを超えた
 *  @retval X_ERR_NO_ENTRY      "../"の展開により最上位ディレクトリを超えた
 *  @retval X_ERR_NONE          正常終了
 */
XError xfpath_resolve(char* dst, const char* cwd, const char* path, size_t size);


/** @brief パスの先頭要素を返します
 *
 *  @pre
 *  + path  != NULL
 *  + o_end != NULL
 *
 *  @param  path   検索対象のパス
 *  @param  o_end  先頭要素の終端を指すポインタの格納先
 *
 *  @retval !=NULL 先頭要素を指すポインタ
 *  @retval NULL   要素なし
 *
 *  @code {.txt}
 *    v--- ret
 *  "/foo/bar/baz\0"
 *       ^--- *o_end
 *   v--- ret
 *  "foo///bar/baz\0"
 *      ^--- *o_end
 *         v--- ret
 *  "//////foo/bar/baz\0"
 *            ^--- *o_end
 *    v--- ret
 *  "/foo\0"
 *       ^--- *o_end
 *    v--- ret
 *  "/foo///\0"
 *       ^--- *o_end
 *  "/"  => NULL
 *  @endcode
 *
 *  @code {.c}
 *  // パスに含まれる要素を先頭からひとつずつ表示するサンプル
 *  const char* next = path;
 *  const char* o_end = NULL;
 *  char name[X_NAME_MAX];
 *
 *  while ((next = xfpath_top(next, (char**)&o_end)))
 *      memcpy(name, next, o_end - next);
 *      name[o_end - next] = '\0';
 *      printf("name => '%s'", name);
 *      next = endptr;
 *  }
 *  @endcode
 */
char* xfpath_top(const char* path, char** o_end);


/** @brief パスの末尾要素を返します
 *
 *  endがNULLを指す場合は`path ~ (path + strlen(path))`の範囲を検索し、
 *  endがNULLでなければ`path ~ (path + (end - path))`の範囲を検索します。
 *
 *  不正なアドレスをendに渡すと、プログラム暴走の原因となるので要注意です。どう
 *  してもミスしやすい部分なので、走査が目的ではなく単に末尾の要素がほしいだけな
 *  らxfpath_name()の使用を推奨します。
 *
 *  @pre
 *  + path  != NULL
 *  + o_end != NULL
 *
 *  @param path  検索対象のパス
 *  @param end   pathの検索範囲の終端
 *  @param o_end 末尾要素の終端を指すポインタの格納先
 *
 *  @retval !=NULL 末尾要素を指すポインタ
 *  @retval NULL   要素なし
 *
 *  @code {.txt}
 *            v--- ret
 *  "/foo/bar/baz\0"
 *               ^--- *o_end
 *            v--- ret
 *  "/foo/bar/baz//\0"
 *               ^--- *o_end
 *  "/"  =>  NULL
 *  @endcode
 *
 *  @code {.c}
 *  // パスに含まれる要素を逆方向からひとつずつ表示するサンプル
 *  const char* endptr;
 *  const char* next = NULL;
 *  char name[X_NAME_MAX];
 *  while ((next = xfpath_tail(path, next, (char**)&endptr)))
 *  {
 *      memcpy(name, next, endptr - next);
 *      name[endptr - next] = '\0';
 *      printf("name => '%s'", name);
 *  }
 *  @endcode
 */
char* xfpath_tail(const char* path, const char* end, char** o_end);


/** @brief パスのファイル名を返します
 *
 *  `xfpath_tail(path, NULL, o_end)`と同じ振る舞いをします。末尾要素がほしいだけ
 *  ならこちらを使用するのが安全です。
 *
 *  @pre
 *  + path  != NULL
 *  + o_end != NULL
 *
 *  @param path  検索対象のパス
 *  @param o_end 末尾要素を指すポインタの格納先
 *
 *  @retval !=NULL 末尾要素を指すポインタ
 *  @retval NULL   要素なし
 */
char* xfpath_name(const char* path, char** o_end);


/** @brief パスのファイル名の拡張子を返します
 *
 *  @pre
 *  + path  != NULL
 *  + o_end != NULL
 *
 *  @param path  検索対象のパス
 *  @param o_end 末尾要素を指すポインタの格納先
 *
 *  @retval !=NULL 拡張子を指すポインタ
 *  @retval NULL   拡張子なし
 *
 *  @code {.txt}
 *               v----ret
 *  "/foo/bar/baz.txt\0"
 *                   ^-----o_end
 *                   v----ret
 *  "/foo/bar/baz.tar.gz\0"
 *                      ^-----o_end
 *                   v----ret
 *  "/foo/bar/baz.tar.gz////\0"
 *                      ^-----o_end
 *  先頭'.'は拡張子としては扱わない
 *  "/foo/bar/.baz\0" ret => NULL
 *  "/foo/bar/baz\0"  ret => NULL
 *  @endcode
 *
 */
char* xfpath_suffix(const char* path, char** o_end);


/** @brief パスのファイル名から拡張子を除いた部分を返します
 *
 *  @pre
 *  + path  != NULL
 *  + o_end != NULL
 *
 *  @param path  検索対象のパス
 *  @param o_end 末尾要素を指すポインタの格納先
 *
 *  @retval !=NULL を指すポインタ
 *  @retval NULL   拡張子なし
 *
 *  @code {.txt}
 *            v----ret
 *  "/foo/bar/baz.txt\0"
 *               ^-----o_end
 *            v----ret
 *  "/foo/bar/baz.tar.gz\0"
 *                   ^-----o_end
 *            v----ret
 *  "/foo/bar/baz.tar.gz////\0"
 *                   ^-----o_end
 *            v----ret
 *  "/foo/bar/baz\0"
 *               ^------o_end
 *  先頭'.'は拡張子としては扱わない
 *            v----ret
 *  "/foo/bar/.baz\0"
 *                ^------o_end
 *  @endcode
 */
char* xfpath_stem(const char* path, char** o_end);


/** @brief パスの上位パスを返します
 *
 *  endがNULLを指す場合は`path ~ (path + strlen(path))`の範囲を検索し、
 *  endがNULLでなければ`path ~ (path + (end - path))`の範囲を検索します。
 *
 *  @param path  検索対象のパス
 *  @param end   pathの検索範囲の終端
 *  @param o_end 上位パスの終端を指すポインタの格納先
 *
 *  @code {.txt}
 *  v----ret
 *  /foo/bar/baz.txt\0
 *          ^-----o_end
 *  v----ret
 *  /foo\0
 *   ^-----o_end
 *  /\0     ret => NULL
 *  @endcode
 *
 *  @code {.c}
 *  // 最上位まで順に親ディレクトリに上がっていくサンプル
 *  const char* endptr = NULL;
 *  const char* parent;
 *  char parentpath[X_PATH_MAX];
 *
 *  while ((parent = xfpath_parent(path, endptr, (char**)&endptr)))
 *  {
 *      memcpy(parentpath, parent, endptr - parent);
 *      parentpath[endptr - parent] = '\0';
 *      printf("name => '%s'", parentpath);
 *  }
 *  @endcode
 */
char* xfpath_parent(const char* path, const char* end, char** o_end);


/** パスのドライブ文字を返します
 *
 *  ドライブ文字として有効な文字は[0-9a-zA-Z]です。
 *
 *  @pre
 *  + path != NULL
 *  + 解決済みのパスであること
 *
 *  @code {.c}
 *  xfpath_drive("0:/foo") => '0'
 *  xfpath_drive("C:/foo") => 'C'
 *  xfpath_drive("/foo")   => '\0'
 *  @endcode
 */
char xfpath_drive(const char* path);


/** パスがルートディレクトリを指しているかどうかを返します
 *
 *  @pre
 *  + path != NULL
 *  + 解決済みのパスであること
 */
bool xfpath_is_root(const char* path);


/** パスが絶対パスかどうかを返します
 *
 *  @pre
 *  + path != NULL
 */
bool xfpath_is_absolute(const char* path);


/** パスが相対パスかどうかを返します
 *
 *  @pre
 *  + path != NULL
 */
bool xfpath_is_relative(const char* path);



/** @} end of addtogroup xfpath
 *  @} end of addtogroup filesystem
 */


#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* picox_filesystem_xfpath_h_ */
