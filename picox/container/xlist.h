/**
 *       @file  xlist.h
 *      @brief  ノード侵入型のダブルリンクリストコンテナです。
 *
 *    @details
 *    英語でこのデータ構造について調べたい時はIntrusive listでググってみてくださ
 *    い。格納するデータ自身が、Listのノードを持っている必要があるという点が普通
 *    のListと違います。
 *
 *    使い方に癖はありますが、かなり強力なのでOSの実装にはよく使われています。
 *    一番の注意点は格納するデータの寿命管理をユーザー側で行わなければならない
 *    ということです。
 *    stackのデータを格納する際は特に注意してください。
 *
 *     @author  MaskedW
 *
 *   @internal
 *     Created  2015/06/13
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


#ifndef xlist_h_
#define xlist_h_


#include <stddef.h>
#include <stdbool.h>


#ifdef __cplusplus
extern "C" {
#endif // __cplusplus


/** リストノードです。
 *
 *  リストに格納するデータはノードメンバを1つ以上含む必要があります。
 *  @code
 *  typedef struct ListData
 *  {
 *      XListNode node; // ノードデータの位置は任意
 *      int       x;
 *  } ListData;
 *
 *  typedef struct ListData2
 *  {
 *      int       x;
 *      // 復数のノードを使って、復数のリストコンテナに格納することも可能
 *      XListNode node0;
 *      XListNode node1;
 *      XListNode node2;
 *  } ListData2;
 *  @endcode
 */
typedef struct XListNode
{
    struct XListNode* next;
    struct XListNode* prev;
} XListNode;


/** リストノードデータを格納するコンテナです。
 */
typedef struct XList
{
/// @privatesection
    XListNode   head;
} XList;


/** XListNodeにオーバーラップする型のポインタを取り出します。
 *
 *  @param ptr      XListNode*
 *  @param type     取り出す型
 *  @param member   型に対応するノードメンバ名
 *  @code
 *  typedef struct ListData
 *  {
 *      int         x;
 *      XListNode   node;
 *  } ListData;
 *  ListData* p = xnode_entry(ptr, ListData, node);
 *  @endcode
 */
#define xnode_entry(ptr, type, member)                      \
    ({                                                      \
        const XListNode* p = (ptr);                         \
        (type*)((char*) p - offsetof(type, member));        \
     })


/** ノードのリンクを解除します。
 *
 *  @pre
 *  + nodeはどこかのリストに格納済みであること
 */
static inline void
xnode_unlink(XListNode* node)
{
   node->next->prev = node->prev;
   node->prev->next = node->next;
}


/** p1の後ろにp2を挿入します。
 */
static inline void
xnode_insert_prev(XListNode* p1, XListNode* p2)
{
    p2->prev        = p1->prev;
    p2->next        = p1;
    p1->prev->next  = p2;
    p1->prev        = p2;
}


/** p1の前にp2を挿入します。
 */
static inline void
xnode_insert_next(XListNode* p1, XListNode* p2)
{
    p2->prev        = p1->prev;
    p2->next        = p1;
    p1->prev->next  = p2;
    p1->prev        = p2;
}


/** oldのリンクをnewに置換えます。
 */
static inline void
xnode_replace(XListNode* old, XListNode* new)
{
    new->next       = old->next;
    new->next->prev = new;
    new->prev       = old->prev;
    new->prev->next = new;
}


/** prev, next間にlistを連結します。
 */
static inline void
xnode_splice(XListNode* prev, XListNode* next, XList* list)
{
    XListNode* first = list->head.next;
    XListNode* last  = list->head.prev;

    first->prev = prev;
    prev->next  = first;
    last->next  = next;
    next->prev  = last;
}


/** コンテナを初期化します。
 */
static inline void
xlist_init(XList* self)
{
    self->head.next = self->head.prev = &self->head;
}


/** コンテナを空にします。
 */
static inline void
xlist_clear(XList* self)
{
    xlist_init(self);
}


/** コンテナが空かどうかを返します。
 */
static inline bool
xlist_empty(const XList* self)
{
    return self->head.next == &self->head;
}


/** コンテナの終端を指すノードを返します。
 */
static inline const XListNode*
xlist_end(const XList* self)
{
    return &self->head;
}


/** コンテナの先頭ノードを返します。
 */
static inline XListNode*
xlist_front(const XList* self)
{
    return self->head.next;
}


/** コンテナの末尾ノードを返します。
 */
static inline XListNode*
xlist_back(const XList* self)
{
    return self->head.prev;
}


/** コンテナ先頭から順方向走査します。
 *
 *  @param ite  XListNode*
 *  ループ毎に、iteratorには次要素が格納されます。
 *
 *  @attention
 *  ループ中に、ノードを除去したり、コンテナを操作する場合は十分注意してくださ
 *  い。iteratorが除去対象となった場合、リンクは無効になってしまう為、工夫が必要
 *  です。
 *
 *  @code
 *  XListNode* ite;
 *  xlist_foreach(&list, ite) {
 *      if (Predicate(ite)) {
 *          XListNode* tmp = ite;
 *          // 次の要素でiteratorを上書きしてから除去する。
 *          ite = ite->next;
 *          xnode_unlink(tmp);
 *      }
 *  }
 *  @endcode
 */
#define xlist_foreach(list, ite)    \
    for (ite  = xlist_front(list);  \
         ite != xlist_end(list);    \
         ite  = ite->next)


/** コンテナ末尾から逆方向走査します。
 *
 *  @see xlist_foreach
 */
#define xlist_rforeach(list, ite)   \
    for (ite  = xlist_back(list);   \
         ite != xlist_end(list);    \
         ite  = ite->prev)


/** ノード数を返します。
 */
static inline size_t
xlist_size(const XList* self)
{
    size_t n = 0;
    XListNode* ite;
    xlist_foreach(self, ite) {
        n++;
    }

    return n;
}


/** ノード数が1つかどうかを返します。
 */
static inline bool
xlist_is_singular(const XList* self)
{
    return (! xlist_empty(self)) &&  (xlist_front(self) == xlist_back(self));
}


/** ノードを先頭に追加します。
 */
static inline void
xlist_push_front(XList* self, XListNode* entry)
{
    xnode_insert_next(&self->head, entry);
}


/** ノードを末尾に追加します。
 */
static inline void
xlist_push_back(XList* self, XListNode* node)
{
    xnode_insert_prev(&self->head, node);
}


/** ノードのリンクを解除してから先頭に追加します。
 */
static inline void
xlist_move_front(XList* self, XListNode* node)
{
    xnode_unlink(node);
    xlist_push_front(self, node);
}


/** ノードのリンクを解除してから末尾に追加します。
 */
static inline void
xlist_move_back(XList* self, XListNode* node)
{
    xnode_unlink(node);
    xlist_push_back(self, node);
}


/** listを先頭に連結します。
 */
static inline void
xlist_splice_front(XList* self, XList* list)
{
    if (! xlist_empty(list)) {
        xnode_splice(&self->head, self->head.next, list);
        xlist_clear(list);
    }
}


/** listを末尾に連結します。
 */
static inline void
xlist_splice_back(XList* self, XList* list)
{
    if (! xlist_empty(list)) {
        xnode_splice(self->head.prev, &self->head, list);
        xlist_clear(list);
    }
}


/** リストを分割します
 *
 * selfの先頭からposまで(pos自身も含む)をlistに移します。
 * posがself内に含まれる要素であることを前提とします。
 */
static inline void
xlist_cut(XList* self, XListNode* pos, XList* list)
{
    if (xlist_empty(self))
        xlist_init(list);
    else {
        XListNode* const new_first = pos->next;
        list->head.next       = self->head.next;
        list->head.next->prev = &list->head;
        list->head.prev       = pos;
        pos->next             = &list->head;
        self->head.next       = new_first;
        new_first->prev       = &self->head;
    }
}


#ifdef __cplusplus
}
#endif // __cplusplus


#endif // xlist_h_
