/**
 *       @file  xpath.h
 *      @brief
 *
 *    @details
 *
 *     @author  MaskedW
 *
 *   @internal
 *     Created  2015/07/27
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

#ifndef picox_xpath_h_
#define picox_xpath_h_


/** パスの末尾の文字列を返します
 *
 * foo/bar.bin => bar.bin
 */
char* xpath_name(const char* path);


/** パスの拡張子を返します
 *
 * + foo/bar.bin    => .bin
 * + foo/bar        => ""
 * + foo/bar.tar.gz => .gz
 *
 * @note
 * 正直いって、"xxx.tar.gz"とかの時は、".gz"ではなく".tar.gz"を返した方が便利な
 * 気がするのだが、似たような機能を提供する他のライブラリが".gz"を返すという実装
 * になっているのでそれにしたがっておく。
 */
char* xpath_suffix(const char* path);


/** パスの上位パスを返します
 *
 * @attention
 * pathに対して破壊的変更が行われます。
 */
char* xpath_parent(char* path);


/** パスの末尾から拡張子を取り除いた文字列を返します
 *
 * foo/bar.bin => bar
 *
 * @attention
 * pathに対して破壊的変更が行われます。
 */
char* xpath_stem(char* path);


#endif // picox_xpath_h_
