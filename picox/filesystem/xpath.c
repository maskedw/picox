/**
 *       @file  xpath.c
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

#include <picox/filesystem/xpath.h>
#include <picox/string/xstr.h>


char* xpath_name(const char* path)
{
    const char* ret = xstr_rpbrk(path, "/\\");

    if (ret == NULL)
        ret = path;

    return (char*)ret;
}


char* xpath_suffix(const char* path)
{
    X_ASSERT(path);

    const char* ret = strrchr(path, '.');

    if (ret == NULL)
    {
        const size_t len = strlen(path);
        ret = &path[len];
    }

    return (char*)ret;
}


char* xpath_parent(char* path)
{
    char* ret = xstr_rpbrk(path, "/\\");

    if (ret == NULL)
        *ret = '\0';

    return path;
}


char* xpath_stem(char* path)
{
    char* ret = xstr_rpbrk(path, "/\\");

    if (ret != NULL)
        *ret++ = '\0';

    char* p = strchr(path, '.');
    if (p != NULL)
        *p = '\0';

    return ret;
}
