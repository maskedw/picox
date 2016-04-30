/**
 *       @file  xfpath.c
 *      @brief
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

#include <picox/filesystem/xfscore.h>


char* xfpath_join(char* p1, const char* p2, size_t size)
{
    /* p2が絶対パスなら、丸コピすればいい */
    if (xfpath_is_absolute(p2))
    {
        if (x_strlcpy(p1, p2, size) >= size)
            return NULL;
        return p1;
    }

    size_t len = strlen(p1);
    X_ASSERT(len < size);

    /* p1の長さが0の時も 丸コピすればいい */
    if (len == 0)
    {
        if (x_strlcpy(p1, p2, size) >= size)
            return NULL;
        return p1;
    }

    /* p1の末尾がディレクトリセパレータでなければセパレータを付加する */
    if (p1[len - 1] != '/')
    {
        p1[len++] = '/';
        --size;
    }

    /* p2を結合したら完了 */
    size -= len;
    if (x_strlcpy(p1 + len, p2, size) >= size)
        return NULL;

    return p1;
}


char* xfpath_resolve_dot(char* path)
{
    if (x_strequal(path, "./") || x_strequal(path, "../"))
        return NULL;

    char* root = strchr(path, '/');
    if (!root)
        return path;

    char* left = root + 1;
    char* right = left;
    for (;;)
    {
        if (x_memequal(right, "./", 2))
        {
            right += 2;
        }
        else if (x_memequal(right, "../", 3))
        {
            if (left - 1 == root)
                return NULL;

            left = (char*)(x_memrchr(root, '/', left - root - 1)) + 1;
            right += 3;
        }
        else
        {
            char* p = strchr(right, '/');
            if (p == NULL)
            {
                strcpy(left, right);
                break;
            }

            p += 1;
            if (right > left)
            {
                memmove(left, right, p - right);
            }

            left += (p - right);
            right = p;
        }
    }
    return path;
}


XError xfpath_resolve(char* dst, const char* cwd, const char* path, size_t size)
{
    if (x_strlcpy(dst, cwd, size) >= size)
        return X_ERR_NAME_TOO_LONG;

    if (!xfpath_join(dst, path, X_PATH_MAX))
        return X_ERR_NAME_TOO_LONG;

    /* ルート以外の末尾の'/'は除去する */
    if (!xfpath_is_root(dst))
    {
        const size_t len = strlen(dst);
        if (dst[len - 1] == '/')
            dst[len - 1] = '\0';
    }

    if (!xfpath_resolve_dot(dst))
        return X_ERR_NO_ENTRY;

    return X_ERR_NONE;
}


char* xfpath_top(const char* path, char** o_end)
{
    const char* p = path;
    const char* next;

    while (*p == '/')
        ++p;
    next = p;

    p = x_strchrnul(p, '/');
    *o_end = (char*)p;

    return (*next == '\0') ? NULL : (char*)next;
}


char* xfpath_tail(const char* path, const char* end, char** o_end)
{
    const size_t len = end ? (size_t)(end - path) : strlen(path);

    if (len == 0)
        return NULL;

    /* 末尾にセパレータが付いているケース
     * 1. /              => NULL
     * 2. foo/bar/       => "bar"
     * 3. foo/           => "foo"
     */
    if (path[len - 1] == '/')
    {
        const char* p = path + len - 1;
        while ((p != path) && (*p == '/'))
            --p;

        if (p == path) /* 1. */
            return NULL;

        *o_end = (char*)(p + 1);

        p = x_memrchr(path, '/', *o_end - path);
        if (p) /* 2. */
            return (char*)(p + 1);

        return (char*)path; /* 3. */
    }

    /* 末尾にセパレータが付いていないケース
     * 1. /foo/bar => bar
     * 2. foo      => foo
     */
    *o_end = (char*)(path + len);
    const char* p = x_memrchr(path, '/', len);
    if (p) /* 1. */
        return (char*)(p + 1);
    return (char*)path; /* 2. */
}


char* xfpath_name(const char* path, char** o_end)
{
    return xfpath_tail(path, NULL, o_end);
}


char* xfpath_suffix(const char* path, char** o_end)
{
    const char* p = xfpath_tail(path, NULL, o_end);
    if (!p)
        return NULL;

    const char* p2 = x_memrchr(p, '.', *o_end - p);

    /* '.'に続く文字がなければ拡張子として認めない。
     *
     * "."      => NULL
     * ".."     => NULL
     * "foo."   => NULL
     */
    if ((!p2) || (p2 + 1 == *o_end))
        return NULL;

    /* '.'の前方に文字がなければ拡張子として認めない。
     *
     * ".vim"       => NULL
     * "..vim"      => .vim
     * "foo..bar"   => .bar
     */
    if (p == p2)
        return NULL;

    return (char*)p2;
}


char* xfpath_stem(const char* path, char** o_end)
{
    *o_end = NULL;
    const char* p = xfpath_tail(path, NULL, o_end);
    if (!p)
        return NULL;

    /* [例外パターン] "."には拡張子も、拡張子以外ももたないと解釈する */
    if ((*o_end - p == 1) && (*p == '.'))
        return NULL;

    const char* p2 = x_memrchr(p, '.', *o_end - p);

    /* '.'に続く文字がなければ拡張子として認めない。 */
    if ((!p2) || (p2 + 1 == *o_end))
        return (char*)p;

    /* '.'の前方に文字がなければ拡張子として認めない。 */
    if (p == p2)
        return (char*)p;

    *o_end = (char*)(p2 - 1);

    return (char*)p;
}


char* xfpath_parent(const char* path, const char* end, char** o_end)
{
    /* パスの末尾要素を取得する */
    const char* p = xfpath_tail(path, end, o_end);

    /* 末尾要素がないなら、当然親はいない*/
    if (!p)
        return NULL;

    /* 末尾要素から1階層上がった先が親なので、 path先頭と末尾要素先頭が一致して
     * いるなら、上の階層に上がれない
     */
    if (p == path)
        return NULL;

    /* 末尾要素の上の階層に上がる。*/
    for (--p; (p >= path) && (*p == '/'); --p);

    /* 親がルート"/"かそうでないかで条件わけが必要だ */
    if (p + 1 == path) /* root */
        *o_end = (char*)path + 1;
    else
        *o_end = (char*)(p + 1);

    return (char*)path;
}


char xfpath_drive(const char* path)
{
    if (path[0] == '\0')
        return '\0';

    if ((isalnum((int)(path[0]))) && (path[1] == ':'))
        return path[0];

    return '\0';
}


bool xfpath_is_root(const char* path)
{
    if (path[0] == '\0')
        return false;

    /* skip drive later */
    if ((isalnum((int)(path[0]))) && (path[1] == ':'))
        path += 2;

    while (path[0] == '/') ++path;
    if (path[0] == '\0')
        return true;

    return false;
}


bool xfpath_is_absolute(const char* path)
{
    /* skip drive later */
    if ((isalnum((int)(path[0]))) && (path[1] == ':'))
        path += 2;

    if (path[0] == '/')
        return true;

    return false;
}


bool xfpath_is_relative(const char* path)
{
    return !(xfpath_is_absolute(path));
}
