/**
 *       @file  xintrusive_list.h
 *      @brief  ノード侵入型のダブルリンクリストコンテナです。
 *
 *    @details
 *    格納するデータ自身が、リストノードを持っている必要があるという点が一般的な
 *    リストコンテナとの違いです。
 *
 *    使い方に癖はありますが、強力なデータ構造なのでOSの実装には、知る限りでは必
 *    ず使用されています。
 *    注意点は格納するデータの寿命管理をユーザー側で行わなければならないというこ
 *    とです。
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


#ifndef picox_container_xintrusive_list_h_
#define picox_container_xintrusive_list_h_


#include <picox/core/xcore.h>


#ifdef __cplusplus
extern "C" {
#endif // __cplusplus


/** リストノードです。
 *
 *  リストに格納するデータはノードメンバを1つ以上含む必要があります。
 *  @code
 *  typedef struct ListData
 *  {
 *      XIntrusiveNode node; // ノードデータの位置は任意
 *      int       x;
 *  } ListData;
 *
 *  typedef struct ListData2
 *  {
 *      int       x;
 *      // 復数のノードを使って、復数のリストコンテナに格納することも可能
 *      XIntrusiveNode node0;
 *      XIntrusiveNode node1;
 *      XIntrusiveNode node2;
 *  } ListData2;
 *  @endcode
 */
typedef struct XIntrusiveNode
{
    struct XIntrusiveNode* next;
    struct XIntrusiveNode* prev;
} XIntrusiveNode;


/** リストノードデータを格納するコンテナです。
 */
typedef struct XIntrusiveList
{
/// @privatesection
    XIntrusiveNode   head;
} XIntrusiveList;


/** XIntrusiveNodeにオーバーラップする型のポインタを取り出します。
 *
 *  @param ptr      XIntrusiveNode*
 *  @param type     取り出す型
 *  @param member   型に対応するノードメンバ名
 *  @code
 *  typedef struct ListData
 *  {
 *      int         x;
 *      XIntrusiveNode   node;
 *  } ListData;
 *  ListData* p = xnode_entry(ptr, ListData, node);
 *  @endcode
 */
#define xnode_entry(ptr, type, member)  X_CONTAINER_OF(ptr, type, member)


/** ノードのリンクを解除します。
 *
 *  @pre
 *  + nodeはどこかのリストに格納済みであること
 */
static inline void
xnode_unlink(XIntrusiveNode* node)
{
   node->next->prev = node->prev;
   node->prev->next = node->next;
}


/** p1の後ろにp2を挿入します。
 */
static inline void
xnode_insert_prev(XIntrusiveNode* p1, XIntrusiveNode* p2)
{
    p2->prev        = p1->prev;
    p2->next        = p1;
    p1->prev->next  = p2;
    p1->prev        = p2;
}


/** p1の前にp2を挿入します。
 */
static inline void
xnode_insert_next(XIntrusiveNode* p1, XIntrusiveNode* p2)
{
    p2->prev        = p1;
    p2->next        = p1->next;
    p1->next->prev  = p2;
    p1->next        = p2;
}


/** p1のリンクをにp2に置換えます。
 */
static inline void
xnode_replace(XIntrusiveNode* p1, XIntrusiveNode* p2)
{
    p2->next       = p1->next;
    p2->next->prev = p2;
    p2->prev       = p1->prev;
    p2->prev->next = p2;
}


/** prev, next間にlistを連結します。
 */
static inline void
xnode_splice(XIntrusiveNode* prev, XIntrusiveNode* next, XIntrusiveList* list)
{
    XIntrusiveNode* first = list->head.next;
    XIntrusiveNode* last  = list->head.prev;

    first->prev = prev;
    prev->next  = first;
    last->next  = next;
    next->prev  = last;
}


/** コンテナを初期化します。
 */
static inline void
xilist_init(XIntrusiveList* self)
{
    X_ASSERT(self);
    self->head.next = self->head.prev = &self->head;
}


/** コンテナのルートノードのポインタを返します。
 */
static inline XIntrusiveNode*
xilist_head(XIntrusiveList* self)
{
    X_ASSERT(self);
    return &self->head;
}


/** コンテナを空にします。
 */
static inline void
xilist_clear(XIntrusiveList* self)
{
    X_ASSERT(self);
    xilist_init(self);
}


/** コンテナが空かどうかを返します。
 */
static inline bool
xilist_empty(const XIntrusiveList* self)
{
    X_ASSERT(self);
    return self->head.next == &self->head;
}


/** コンテナの終端を指すノードを返します。
 */
static inline XIntrusiveNode*
xilist_end(const XIntrusiveList* self)
{
    X_ASSERT(self);
    return (XIntrusiveNode*)(&self->head);
}


/** コンテナの先頭ノードを返します。
 */
static inline XIntrusiveNode*
xilist_front(const XIntrusiveList* self)
{
    X_ASSERT(self);
    return self->head.next;
}


/** コンテナの末尾ノードを返します。
 */
static inline XIntrusiveNode*
xilist_back(const XIntrusiveList* self)
{
    X_ASSERT(self);
    return self->head.prev;
}


/** コンテナ先頭から順方向走査します。
 *
 *  @param ite  XIntrusiveNode*
 *  ループ毎に、iteratorには次要素が格納されます。
 *
 *  @attention
 *  ループ中に、ノードを除去したり、コンテナを操作する場合はforeach()を使用しな
 *  いでください。iterator自身がコンテナから除去されると、ループが行えなくなるか
 *  らです。イテレータが除去されるループは面倒ですが自分でループを書く必要があり
 *  ます。
 *
 *  @code
 *  // [NG] リンクを解除した時点でイテレートできなくなる
 *  XIntrusiveNode* ite;
 *  xilist_foreach(&list, ite)
 *  {
 *      if (...)
 *          xnode_unlink(ite);
 *  }
 *  @endcode
 *
 *  @code
 *  // [OK] 1つだけ除去してループを抜けるなら問題なし。
 *  XIntrusiveNode* ite;
 *  xilist_foreach(&list, ite)
 *  {
 *      if (...)
 *      {
 *          xnode_unlink(ite);
 *          break;
 *      }
 *  }
 *  @endcode
 *
 *  @code
 *  // [OK] 1つ以上のノードを除去するならイテレータの管理は自分で行う
 *  XIntrusiveNode* ite = xilist_front(&ilist);
 *  const XIntrusiveList* const end = xilist_end(&list);
 *  while (ite != end)
 *  {
 *      if (...)
 *      {
 *          // 次の要素でiteratorを上書きしてから除去する。
 *          XIntrusiveNode* tmp = ite;
 *          ite = ite->next;
 *          xnode_unlink(tmp);
 *      }
 *      else
 *      {
 *          ite = ite->next;
 *      }
 *  }
 *  @endcode
 */
#define xilist_foreach(list, ite)    \
    for (ite  = xilist_front(list);  \
         ite != xilist_end(list);    \
         ite  = ite->next)


/** コンテナ末尾から逆方向走査します。
 *
 *  @see xilist_foreach
 */
#define xilist_rforeach(list, ite)   \
    for (ite  = xilist_back(list);   \
         ite != xilist_end(list);    \
         ite  = ite->prev)


/** ノード数を返します。
 */
static inline size_t
xilist_size(const XIntrusiveList* self)
{
    X_ASSERT(self);
    size_t n = 0;
    XIntrusiveNode* ite;
    xilist_foreach(self, ite)
        n++;

    return n;
}


/** ノード数が1つかどうかを返します。
 */
static inline bool
xilist_is_singular(const XIntrusiveList* self)
{
    X_ASSERT(self);
    return (! xilist_empty(self)) &&  (xilist_front(self) == xilist_back(self));
}


/** ノードを先頭に追加します。
 *
 *  @pre
 *  + node != NULL
 */
static inline void
xilist_push_front(XIntrusiveList* self, XIntrusiveNode* node)
{
    X_ASSERT(self);
    X_ASSERT(node);
    xnode_insert_next(&self->head, node);
}


/** ノードを末尾に追加します。
 *
 *  @pre
 *  + node != NULL
 */
static inline void
xilist_push_back(XIntrusiveList* self, XIntrusiveNode* node)
{
    X_ASSERT(self);
    X_ASSERT(node);
    xnode_insert_prev(&self->head, node);
}


/** ノードのリンクを解除してから先頭に追加します。
 *
 *  @pre
 *  + node != NULL
 */
static inline void
xilist_move_front(XIntrusiveList* self, XIntrusiveNode* node)
{
    X_ASSERT(self);
    X_ASSERT(node);
    xnode_unlink(node);
    xilist_push_front(self, node);
}


/** ノードのリンクを解除してから末尾に追加します。
 *
 *  @pre
 *  + node != NULL
 */
static inline void
xilist_move_back(XIntrusiveList* self, XIntrusiveNode* node)
{
    X_ASSERT(self);
    X_ASSERT(node);
    xnode_unlink(node);
    xilist_push_back(self, node);
}


/** otherを先頭に連結します。
 *
 *  @pre
 *  + other != NULL
 */
static inline void
xilist_splice_front(XIntrusiveList* self, XIntrusiveList* other)
{
    X_ASSERT(self);
    X_ASSERT(other);
    if (! xilist_empty(other))
    {
        xnode_splice(&self->head, self->head.next, other);
        xilist_clear(other);
    }
}


/** otherを末尾に連結します。
 *
 *  @pre
 *  + other != NULL
 */
static inline void
xilist_splice_back(XIntrusiveList* self, XIntrusiveList* other)
{
    X_ASSERT(self);
    X_ASSERT(other);
    if (! xilist_empty(other))
    {
        xnode_splice(self->head.prev, &self->head, other);
        xilist_clear(other);
    }
}


/** 2つのリストの中身を入れ替えます。
 *
 *  @pre
 *  + other != NULL
 */
static inline void
xilist_swap(XIntrusiveList* self, XIntrusiveList* other)
{
    X_ASSERT(self);
    X_ASSERT(other);

    if (self == other)
        return;

    const bool empty_self = xilist_empty(self);
    const bool empty_other = xilist_empty(other);

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


/** 先頭にotherの先頭要素からpos(pos自身も含む)までを転送します。
 *
 *  @pre
 *  + other != NULL
 *  + pos   != NULL
 *  + posはotherに含まれる要素であること
 */
static inline void
xilist_transfer_front(XIntrusiveList* self, XIntrusiveList* other, XIntrusiveNode* pos)
{
    X_ASSERT(self);
    X_ASSERT(pos);
    X_ASSERT(other);
    X_ASSERT(!xilist_empty(other));

    XIntrusiveNode* const new_first = pos->next;
    pos->next                       = self->head.next;
    self->head.next                 = other->head.next;
    self->head.next->prev           = &self->head;
    self->head.prev                 = pos;
    other->head.next                = new_first;
    new_first->prev                 = &other->head;
}


/** 末尾にotherの先頭要素からpos(pos自身も含む)までを転送します。
 *
 *  @pre
 *  + other != NULL
 *  + pos   != NULL
 *  + posはotherに含まれる要素であること
 */
static inline void
xilist_transfer_back(XIntrusiveList* self, XIntrusiveList* other, XIntrusiveNode* pos)
{
    X_ASSERT(self);
    X_ASSERT(pos);
    X_ASSERT(other);
    X_ASSERT(!xilist_empty(other));

    XIntrusiveNode* const new_first = pos->next;
    pos->next                       = &self->head;
    pos->prev                       = self->head.prev;
    self->head.prev->next           = other->head.next;
    self->head.prev                 = pos;
    other->head.next                = new_first;
    new_first->prev                 = &other->head;
}


#ifdef __cplusplus
}
#endif // __cplusplus


#endif // picox_container_xintrusive_list_h_
