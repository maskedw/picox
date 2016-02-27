/**
 *       @file  xflib.c
 *      @brief
 *
 *    @details
 *
 *     @author  MaskedW
 *
 *   @internal
 *     Created  2016/02/27
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


#include <picox/filesystem/xflib.h>



char* xflib_resolve(char* dst, size_t size, const char* cur,
                    size_t rootprefix_len, const char* path)
{
    char* p = dst;
    char* tmp;
    int up_count;
    size_t len;

    /* pathが'/'から始まる絶対パスだった場合 */
    if (path[0] == '/')
    {
        if (rootprefix_len == 0)
        {
            /* パス解決の必要がないためそのままコピーする */
            len = x_strlcpy(p, path, size);
            if (len >= size)
                return NULL;
        }
        else
        {
            len = strlen(path);
            if (rootprefix_len + len >= size)
                return NULL;

            /* ルートディレクトリのプレフィックスとpathを結合する */
            memcpy(p, cur, rootprefix_len);
            strcpy(p + rootprefix_len, path);
        }

        return dst;
    }

    /* 以降は相対パスだった場合 */


    /* カレントディレクトリをコピーする */
    len = x_strlcpy(p, cur, size);
    if (len >= size)
        return NULL;
    p += len;
    size -= len;

    /* "./", "../"はパスの先頭にある場合だけ解決を認める。パスの途中のものまで認
     * めても非効率な割に実用性がないからだ。
     */
    up_count = 0;
    for (;;)
    {
        if (x_memequal(path, "./", 2))
        {
            path += 2;
            break;
        }
        else if (x_memequal(path, "../", 2))
        {
            path += 3;
            up_count++;
        }
        else
        {
            break;
        }
    }

    /* "../"の回数分カレントディレクトリパスを遡る */
    if (up_count)
    {
        while (up_count--)
        {
            tmp = x_memrchr(p, '/', len);
            if (tmp == NULL)
                return NULL;

            /* 先頭より遡ることはできない */
            if (tmp <= dst)
                return NULL;

            *(--tmp) = '\0';

            const size_t n = (p + len) - tmp;
            size += n;
            len -= n;

            size += (p + len) - tmp;
            len -= (p + len) - tmp;
        }
    }

    /* 相対パスを結合したらパス解決完了 */
    len = x_strlcpy(p + len, path, size);
    if (len >= size)
        return NULL;

    return dst;
}
