/**
 *       @file  xtokenizer.c
 *      @brief
 *
 *    @details
 *
 *     @author  MaskedW
 *
 *   @internal
 *     Created  2015/03/06
 * ===================================================================
 */

/*
 * License: MIT license
 * Copyright (c) <2014> <MaskedW [maskedw00@gmail.com]>
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

#include <picox/misc/xtokenizer.h>


bool xtok_init(XTokenizer* self, const char* row, char separater, int max_tokens)
{
    X_ASSERT(self);
    X_ASSERT(row);
    X_ASSERT(max_tokens > 0);

    memset(self, 0, sizeof(*self));
    char* tmp_row = NULL;
    char** tmp_tokens = NULL;
    bool ok = false;

    do
    {
        X_BREAK_IF(!(tmp_row = x_strdup(row)));

        char* p1 = tmp_row;
        char* p2;
        int ntokens = 1;
        while ((p2 = strchr(p1, separater)) != NULL)
        {
            p1 = p2 + 1;
            ntokens++;
        }

        X_BREAK_IF(ntokens > max_tokens);
        X_BREAK_IF(!(tmp_tokens = x_malloc(sizeof(char*) * ntokens)));

        p1 = tmp_row;
        tmp_tokens[0] = p1 = x_strstrip(p1, NULL);
        ntokens = 1;

        while ((p2 = strchr(p1, separater)) != NULL)
        {
            *p2 = '\0';
            p1 = p2 + 1;
            p1 = x_strstrip(p1, NULL);
            tmp_tokens[ntokens] = p1;
            ntokens++;
        }

        self->row = tmp_row;
        self->tokens = tmp_tokens;
        self->ntokens = ntokens;
        ok = true;
    } while (0);

    if (! ok)
    {
        x_free(tmp_row);
        x_free(tmp_tokens);
    }

    return ok;
}


void xtok_release(XTokenizer* self)
{
    X_ASSERT(self);
    X_SAFE_FREE(self->row);
    X_SAFE_FREE(self->tokens);
    self->ntokens = 0;
}
