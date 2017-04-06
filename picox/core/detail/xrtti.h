/**
 *       @file  xrtti.h
 *      @brief
 *
 *    @details
 *
 *     @author  MaskedW
 *
 *   @internal
 *     Created  2017/04/04
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

#ifndef picox_core_detail_xrtti_h_
#define picox_core_detail_xrtti_h_


/** @addtogroup core
 *  @{
 *  @addtogroup xrtti
 *  @brief picoxが定めるRTTI(実行時型情報)のための機能を提供します
 *
 *  picoxではXStreamを代表とした、オブジェクト指向言語で用いられるような所謂
 *  Interface型のような役割の構造体がいくつか存在しています。
 *
 *  Interface型はまれに、実際のクラス(構造体)が何であるのかを知りたいことがあり
 *  ます。例えば、XStreamがファイルなのかUARTなのかで処理を振り分けるとか。
 *
 *  そういったことが必要になった時点で設計が悪い、とはよく言われることですが、な
 *  んだかんだ型を判別できると便利なものです。特に、泥臭い処理の多い組み込みプロ
 *  グラムでは、設計の美しさよりも、実利をとらなければならないことも多々あるはず
 *  です。
 *
 *  C言語では実行時に型を検査する機能はありませんが、picoxでは構造体メンバの並び
 *  や名称に規約を設けること、マクロを使用することで、これを簡易的に実現していま
 *  す。ユーザが独自に定義する構造体も、picoxのルールを守ることで、実行時型情報
 *  を取得することができます。
 *  @{
 */


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/** @brief picoxが規定するInterface型の固定メンバを定義します
 *
 *  構造体メンバの先頭で使用してください。
 *
 *  @code {.c}
 *  struct FooInterfaceVTable
 *  {
 *      ...
 *  };
 *
 *  struct FooInterface
 *  {
 *      X_DECLEAR_RTTI(struct FooInterfaceVTable);
 *  };
 *  @endcode
 *
 */
#define X_DECLEAR_RTTI(VTABLE_TYPE)   \
    const void*        m_rtti_tag;    \
    void*              m_driver;      \
    const VTABLE_TYPE* m_vtable


/** @brief 型チェックに使用するタグのextern宣言を行います
 *
 *  宣言なのでこのマクロはヘッダファイル内で使用し、実体の定義は別途Cソースファ
 *  イルでX_IMPL_RTTI_TAG()を使用してください。
 *
 *  タグ変数のアドレスを識別子にしているため、値は何が入っていても無関係です。
 *
 *  @note
 *  タグを宣言するにあたってこのマクロを使用する必然性はありませんが、役割をコー
 *  ド中で明示することが目的です。X_IMPL_RTTI_TAG()についても同様です。
 */
#define X_DECLEAR_RTTI_TAG(TAG_NAME)    extern const char TAG_NAME


/** @brief 型チェックに使用するタグの定義を行います
 */
#define X_IMPL_RTTI_TAG(TAG_NAME)       const char TAG_NAME


/** @brief Interfaceが型の固定メンバを初期化します
 */
#define X_RESET_RTTI(x) ((x)->m_rtti_tag = NULL, (x)->m_driver = NULL, (x)->m_vtable = NULL)


/** @brief Interface型がrtti_tagと対応した型かどうかを返します
 */
#define x_rtti_equal(x, rtti_tag)               ((x) && ((x)->m_rtti_tag == &rtti_tag))


/** @brief Interface型xの実体をtypeにキャストします
 *
 *  型がtypeと一致しているかは保証していないため、x_rtti_equal()等の事前チェック
 *  が行われていることを前提にしています。
 */
#define x_rtti_cast(x, type)                    (((type)(x))->m_driver)


/** @brief 型が一致していればキャストしたポインタを、不一致であればNULLを返します */
#define x_rtti_cast_or_null(x, type, rtti_tag)  (x_rtti_equal(x, rtti_tag) ? x_rtti_cast(x, type) : NULL)


/** @brief 型名を返します */
#define x_rtti_typename(x)                      ((x)->m_vtable->m_name)


#ifdef __cplusplus
}
#endif /* __cplusplus */


/** @} end of addtogroup xrtti
 *  @} end of addtogroup core
 */


#endif /* picox_core_detail_xrtti_h_ */
