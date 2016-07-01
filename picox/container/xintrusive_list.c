/**
 *       @file  xintrusive_list.c
 *      @brief  ノード侵入型のダブルリンクリストコンテナです。
 *
 *    @details
 *
 *     @author  MaskedW
 *
 *   @internal
 *     Created  2016/06/25
 * ===================================================================
 */

/*
 * License: MIT license
 * Copyright (c) <2016> <MaskedW [maskedw00@gmail.com]>
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


#include <picox/container/xintrusive_list.h>


#ifdef X_COMPILER_NO_INLINE


void xnode_unlink(XIntrusiveNode* node)
{
   node->next->prev = node->prev;
   node->prev->next = node->next;
}


void xnode_insert_prev(XIntrusiveNode* p1, XIntrusiveNode* p2)
{
    p2->prev        = p1->prev;
    p2->next        = p1;
    p1->prev->next  = p2;
    p1->prev        = p2;
}


void xnode_insert_next(XIntrusiveNode* p1, XIntrusiveNode* p2)
{
    p2->prev        = p1;
    p2->next        = p1->next;
    p1->next->prev  = p2;
    p1->next        = p2;
}


void xnode_replace(XIntrusiveNode* p1, XIntrusiveNode* p2)
{
    p2->next       = p1->next;
    p2->next->prev = p2;
    p2->prev       = p1->prev;
    p2->prev->next = p2;
}


void xnode_splice(XIntrusiveNode* prev, XIntrusiveNode* next, XIntrusiveList* list)
{
    XIntrusiveNode* first = list->head.next;
    XIntrusiveNode* last  = list->head.prev;

    first->prev = prev;
    prev->next  = first;
    last->next  = next;
    next->prev  = last;
}


void xilist_init(XIntrusiveList* self)
{
    self->head.next = self->head.prev = &self->head;
}


XIntrusiveNode* xilist_head(XIntrusiveList* self)
{
    return &self->head;
}


void xilist_clear(XIntrusiveList* self)
{
    xilist_init(self);
}


bool xilist_empty(const XIntrusiveList* self)
{
    return self->head.next == &self->head;
}


XIntrusiveNode* xilist_end(const XIntrusiveList* self)
{
    return (XIntrusiveNode*)(&self->head);
}


XIntrusiveNode* xilist_front(const XIntrusiveList* self)
{
    return self->head.next;
}


XIntrusiveNode* xilist_pop_front(const XIntrusiveList* self)
{
    XIntrusiveNode* front = self->head.next;
    xnode_unlink(front);
    return front;
}


XIntrusiveNode* xilist_back(const XIntrusiveList* self)
{
    return self->head.prev;
}


XIntrusiveNode* xilist_pop_back(const XIntrusiveList* self)
{
    XIntrusiveNode* back = self->head.prev;
    xnode_unlink(back);
    return back;
}


size_t xilist_size(const XIntrusiveList* self)
{
    size_t n = 0;
    XIntrusiveNode* ite;
    xilist_foreach(self, ite)
        n++;
    return n;
}


bool xilist_is_singular(const XIntrusiveList* self)
{
    return (! xilist_empty(self)) &&  (xilist_front(self) == xilist_back(self));
}


void xilist_push_front(XIntrusiveList* self, XIntrusiveNode* node)
{
    xnode_insert_next(&self->head, node);
}


void xilist_push_back(XIntrusiveList* self, XIntrusiveNode* node)
{
    xnode_insert_prev(&self->head, node);
}


void xilist_move_front(XIntrusiveList* self, XIntrusiveNode* node)
{
    xnode_unlink(node);
    xilist_push_front(self, node);
}


void xilist_move_back(XIntrusiveList* self, XIntrusiveNode* node)
{
    xnode_unlink(node);
    xilist_push_back(self, node);
}


void xilist_splice_front(XIntrusiveList* self, XIntrusiveList* other)
{
    if (! xilist_empty(other))
    {
        xnode_splice(&self->head, self->head.next, other);
        xilist_clear(other);
    }
}


void xilist_splice_back(XIntrusiveList* self, XIntrusiveList* other)
{
    if (! xilist_empty(other))
    {
        xnode_splice(self->head.prev, &self->head, other);
        xilist_clear(other);
    }
}


void xilist_swap(XIntrusiveList* self, XIntrusiveList* other)
{
    bool empty_self, empty_other;

    if (self == other)
        return;

    empty_self = xilist_empty(self);
    empty_other = xilist_empty(other);

    X_SWAP(self->head, other->head, XIntrusiveNode);

    if (empty_other)
        xilist_init(self);
    else
        self->head.next->prev = self->head.prev->next = &(self->head);

    if (empty_self)
        xilist_init(other);
    else
        other->head.next->prev = other->head.prev->next = &(other->head);
}


void xilist_transfer_front(XIntrusiveList* self, XIntrusiveList* other, XIntrusiveNode* pos)
{
    XIntrusiveNode* new_first;

    new_first               = pos->next;
    pos->next               = self->head.next;
    self->head.next         = other->head.next;
    self->head.next->prev   = &self->head;
    self->head.prev         = pos;
    other->head.next        = new_first;
    new_first->prev         = &other->head;
}


void xilist_transfer_back(XIntrusiveList* self, XIntrusiveList* other, XIntrusiveNode* pos)
{
    XIntrusiveNode* new_first;

    new_first               = pos->next;
    pos->next               = &self->head;
    pos->prev               = self->head.prev;
    self->head.prev->next   = other->head.next;
    self->head.prev         = pos;
    other->head.next        = new_first;
    new_first->prev         = &other->head;
}


#endif /* ifdef X_COMPILER_NO_INLINE */
