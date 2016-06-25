/**
 *       @file  xfiber.h
 *      @brief
 *
 *    @details
 *
 *     @author  MaskedW
 *
 *   @internal
 *     Created  2016/05/06
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


#ifndef picox_multitask_xfiber_h_
#define picox_multitask_xfiber_h_


#include <picox/core/xcore.h>
#include <picox/container/xintrusive_list.h>


/** @addtogroup multitask
 *  @{
 *  @addtogroup xfiber
 *  @brief  非リアルタイム協調型マルチタスクモジュール
 *
 *  協調型のマルチタスクをスレッドと区別して、コルーチンやファイバーと呼ぶことが
 *  多いです。
 *
 *  スクリプト言語を中心に、多くの言語でスレッドとは別にコルーチンやファイバーを
 *  サポートしています。
 *
 *  [コルーチン]
 *  + Lua
 *  + C#
 *  + Python (ジェネレータ)
 *
 *  [ファイバー]
 *  + Win32API
 *  + Ruby
 *
 *  コルーチンやファイバーという用語に明確な定義はありませんが、最低限処理を中断
 *  して、後から再開できる機能を持ちます。
 *
 *  picoxのファイバーでは、RTOS風にタスクの優先順位や待ち合わせ機能を多数用意し
 *  ていることが大きな特徴で、RTOSの代替として使用できることを目的としています。
 *
 *  RTOSの実装はC言語の機能だけでは実現不可能なため、CPUごとコンパイラごとにアセ
 *  ンブリ言語を記述する必要があり大変です。
 *  移植部分が最小限になるように考慮して実装されたフリーのRTOSもありますが、それ
 *  でも敷居は高いものです。
 *
 *  このモジュールはC言語の機能のみでマルチタスクを実現しているため、移植作業な
 *  しで即座に使用することができます。
 *
 *  ただし、以下の制限があります。
 *
 *  + リアルタイム性はなし<br>
 *      RTOSでは割込みハンドラから即座に高優先度のタスクにスイッチできますが、
 *      xfiberでは、タスクコンテキストからのシステムコール呼び出しまでコンテキス
 *      トスイッチが行われません。
 *
 *  + コンテキストスイッチが重い<br>
 *      RTOSではいくつかのCPUレジスタの退避とわずかの処理でコンテキストスイッチ
 *      が完了しますが、xfiberのコンフィギュレーションが
 *      `X_CONF_FIBER_IMPL_TYPE == X_FIBER_IMPL_TYPE_COPY_STACK`である場合、コン
 *      テキストスイッチの度に使用中のスタックのコピーが行われます。
 *      これは非力なCPUでは致命的なオーバーヘッドになる可能性があります。
 *
 *  + C++の例外との共存不可<br>
 *      これは巷のRTOSでも同じですが、コンテキストスイッチと例外スタックの整合性
 *      がとれないため、C++で使用する場合は例外をOFFにする必要があります。
 *
 *  RTOSを移植するまでの一時しのぎや、リソースに余裕のあるプロトタイプ段階でご利
 *  用ください。
 *
 *  @note
 *  C言語で実装可能なコンテキストスイッチということで、C言語の限界に迫るトリッキ
 *  ーで面白い実装になっています。以下の技術資料にあるアイデアを元に実装を行いま
 *  した。
 *
 *  http://www.atdot.net/~ko1/activities/shiba-prosym2010-paper.pdf
 *
 *  興味のある方は是非ソースコードを覗いてみてください。
 *  @{
 */


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/** @brief タスク優先度の最大値です(0 ~ 7の8段階)
 */
#define X_FIBER_PRIORITY_MAX       (8)


/** @brief @see X_CONF_FIBER_ENTER_CRITICAL
 */
#define X_FIBER_ENTER_CRITICAL     X_CONF_FIBER_ENTER_CRITICAL


/** @brief @see X_CONF_FIBER_EXIT_CRITICAL
 */
#define X_FIBER_EXIT_CRITICAL      X_CONF_FIBER_EXIT_CRITICAL


/** @brief タスクのメイン関数ポインタ型です
 *
 *  引数のポインタが指すアドレスはスタック以外である必要があります。
 */
typedef void (*XFiberFunc)(void*);


/** @brief アイドル時に呼び出されるフック関数のポインタ型です
 *
 *  0以外を返すと、スケジューリングは終了し、xfiber_kernel_start_scheduler()の呼
 *  び出し直後の地点までジャンプします。
 */
typedef int(*XFiberIdleHook)(void);


/** @name fiber_event_mode
 *  @brief XFiberEventのmode引数に指定可能な値です
 *  @{
 */

/** @brief イベントのどれかを待ちます(OR待ち) */
#define X_FIBER_EVENT_WAIT_OR       (0)

/** @brief イベントの全てが成立するまで待ちます(AND待ち) */
#define X_FIBER_EVENT_WAIT_AND      (1)

/** @brief イベント成立時に、イベントのクリアを行います */
#define X_FIBER_EVENT_CLEAR_ON_EXIT (1 << 1)

/** @} end of name fiber_event_mode
 */


/* 前方宣言
 * fiberオブジェクトはスタックに確保することはできず、xfiber_xxx_create()系の生
 * 成関数を呼び出す必要があります。
 */
struct XFiber;
struct XFiberEvent;
struct XFiberQueue;
struct XFiberPool;
struct XFiberChannel;
struct XFiberMailbox;
struct XFiberSemaphore;
struct XFiberMutex;
typedef struct XFiber XFiber;
typedef struct XFiberEvent XFiberEvent;
typedef struct XFiberQueue XFiberQueue;
typedef struct XFiberPool XFiberPool;
typedef struct XFiberChannel XFiberChannel;
typedef struct XFiberMailbox XFiberMailbox;
typedef struct XFiberSemaphore XFiberSemaphore;
typedef struct XFiberMutex XFiberMutex;
typedef XIntrusiveNode XFiberMessage;


/** @name fiber_kernel_control
 *  @{
 */

/** @brief カーネルを初期化します
 *
 *  @param heap     ワークバッファのアドレス
 *  @param heapsize ワークバッファのサイズ
 *  @param idlehook @see XFiberIdleHook
 *
 *  heapにNULLが指定された場合、x_malloc()でメモリを確保します。スタックやファイ
 *  バーオブジェクトは全てワークバッファから生成されます。
 */
XError xfiber_kernel_init(void* heap, size_t heapsize, XFiberIdleHook idlehook);


/** @brief スケジューリングを開始します
 */
XError xfiber_kernel_start_scheduler(void);


/** @brief スケジューリングを終了します
 *
 *  呼び出し後はxfiber_kernel_start_scheduler()の呼び出し直後の位置までジャンプ
 *  します。
 */
void   xfiber_kernel_end_scheduler(void);


/** @} end of name fiber_kernel_control
 */


/** @name fiber_task_control
 *  @{
 */


/** @brief タスクを生成します
 *
 *  @param o_fiber      生成したタスクのアドレスの格納先
 *  @param priority     タスク優先度
 *  @param name         タスク名
 *  @param stack_size   スタックのバイト数
 *  @param func         メイン関数
 *  @param arg          メイン関数の引数
 */
XError xfiber_create(XFiber** o_fiber, int priority, const char* name, size_t stack_size, XFiberFunc func, void* arg);


/** @brief タスクの実行を指定時間遅延します
 */
void xfiber_delay(XTicks time);


/** @brief タスクをサスペンド状態に遷移させます
 *
 *  サスペンド中のタスクはリジュームされるまで実行状態にはなりません。
 *
 *  既に待ち状態にあるタスクは、サスペンド・ウエイトという2重待ち状態になります
 *  。この場合、サスペンドだけが解除されてもウエイト状態は継続し、ウエイトだけが
 *  解除されてもサスペンド状態は継続します。
 */
XError xfiber_suspend(XFiber* fiber);


/** @brief タスクのサスペンド状態を解除します
 */
XError xfiber_resume(XFiber* fiber);


/** @brief タスクの実行を同一優先度の別のタスクにゆずります
 */
void xfiber_yield();


/** @brief 実行状態のタスクを返します
 */
XFiber* xfiber_self();


/** @brief タスクの名前を返します
 */
const char* xfiber_name(const XFiber* fiber);


/** @} end of name fiber_task_control
 */


/** @name fiber_event
 *  @brief ビットフラグによるイベント待ちを行いたい場合に使用します
 *  @{
 */


/** @brief イベントオブジェクトを生成します
 */
XError xfiber_event_create(XFiberEvent** o_event);


/** @brief イベントオブジェクトを破棄します
 *
 *  全ての待ちタスクの待ちは解除され、待ちタスクにはX_ERR_CANCELEDが返ります
 */
void xfiber_event_destroy(XFiberEvent* event);


/** @brief 指定のイベントが成立するのをタイムアウト付きで待ちます
 *
 *  @param mode          @see fiber_event_mode
 *  @param wait_pattern  待ちビットパターン
 *  @param result        待ち解除時のビットパターンの格納先
 *  @param timeout       タイムアウト時間
 */
XError xfiber_event_timed_wait(XFiberEvent* event, XMode mode, XBits wait_pattern, XBits* result, XTicks timeout);


/** @brief 指定のイベントが成立するのを待ちます
 *
 *  xfiber_event_timed_wait()でtimeoutにX_TICKS_FOREVERを指定した場合と同じです
 */
XError xfiber_event_wait(XFiberEvent* event, XMode mode, XBits wait_pattern, XBits* result);


/** @brief 指定のイベントが成立しているかをポーリングで確認します
 *
 *  xfiber_event_timed_wait()でtimeoutに0を指定した場合と同じです
 */
XError xfiber_event_try_wait(XFiberEvent* event, XMode mode, XBits wait_pattern, XBits* result);


/** @brief イベントをセットします
 */
XError xfiber_event_set(XFiberEvent* event, XBits pattern);


/** @brief 割込みハンドラから呼び出し可能なxfiber_event_set()です
 */
XError xfiber_event_set_isr(XFiberEvent* event, XBits pattern);


/** @brief 指定ビットをクリアします
 */
XBits xfiber_event_clear(XFiberEvent* event, XBits pattern);


/** @brief 割込みハンドラから呼び出し可能なxfiber_event_clear()です
 */
XBits xfiber_event_clear_isr(XFiberEvent* event, XBits pattern);


/** @brief 現在のビットパターンを返します
 */
XBits xfiber_event_get(XFiberEvent* event);


/** @brief 割込みハンドラから呼び出し可能なxfiber_event_get()です
 */
XBits xfiber_event_get_isr(XFiberEvent* event);


/** @} end of name fiber_event
 */


/** @name fiber_signal
 *  @brief ビットフラグによるイベント待ちを行いたい場合に使用します
 *
 *  イベントと似ていますが、オブジェクトを生成することなく、タスクに直接イベント
 *  を発行できる点が異なります。
 *  簡易的なイベントフラグとして使用できます。
 *  @{
 */


/** @brief シグナルが成立するのをタイムアウト付きで待ちます
 *
 *  @param sigs     待ちビットパターン
 *  @param result   待ち解除時のビットパターンの格納先
 *  @param timeout  タイムアウト時間
 */
XError xfiber_signal_timed_wait(XBits sigs, XBits* result, XTicks timeout);


/** @brief シグナルが成立するのを待ちます
 */
XError xfiber_signal_wait(XBits sigs, XBits* result);


/** @brief シグナルが成立するのをポーリングで待ちます
 */
XError xfiber_signal_try_wait(XBits sigs, XBits* result);


/** @brief タスクにシグナルを発行します
 */
XError xfiber_signal_raise(XFiber* fiber, XBits sigs);


/** @brief 割込みハンドラから呼び出し可能なxfiber_signal_raise()です
 */
XError xfiber_signal_raise_isr(XFiber* fiber, XBits sigs);


/** @brief 現在のビットパターンを返します
 */
XBits xfiber_signal_get(XFiber* fiber);


/** @brief 割込みハンドラから呼び出し可能なxfiber_signal_get()です
 */
XBits xfiber_signal_get_isr(XFiber* fiber);


/** @} end of name fiber_signal
 */


/** @name fiber_queue
 *
 *  固定長のメッセージの受け渡しに使用します。リングバッファで実装されており、デ
 *  ータはバッファにコピーされます。
 *  @{
 */


/** @brief キューを生成します
 *
 *  @param o_queue      生成したキューのアドレスの格納先
 *  @param queue_len    格納可能な要素数
 *  @param item_size    1要素のバイト数
 *
 *  queue_len * item_sizeバイトのバッファが生成されます。
 */
XError xfiber_queue_create(XFiberQueue** o_queue, size_t queue_len, size_t item_size);


/** @brief キューを破棄します
 *
 *  全ての待ちタスクの待ちは解除され、待ちタスクにはX_ERR_CANCELEDが返ります
 */
void xfiber_queue_destroy(XFiberQueue* queue);


/** @brief キューの末尾への要素の転送をタイムアウト付きで試みます
 */
XError xfiber_queue_timed_send_back(XFiberQueue* queue, const void* src, XTicks timeout);


/** @brief キューの末尾への要素の転送を試みます
 */
XError xfiber_queue_send_back(XFiberQueue* queue, const void* src);


/** @brief キューの末尾への要素の転送をポーリングで試みます
 */
XError xfiber_queue_try_send_back(XFiberQueue* queue, const void* src);


/** @brief 割込みハンドラから呼び出し可能なxfiber_queue_try_send_back()です
 */
XError xfiber_queue_send_back_isr(XFiberQueue* queue, const void* src);


/** @brief キューの先頭への要素の転送をタイムアウト付きで試みます
 */
XError xfiber_queue_timed_send_front(XFiberQueue* queue, const void* src, XTicks timeout);


/** @brief キューの先頭への要素の転送を試みます
 */
XError xfiber_queue_send_front(XFiberQueue* queue, const void* src);


/** @brief キューの先頭への要素の転送をポーリングで試みます
 */
XError xfiber_queue_try_send_front(XFiberQueue* queue, const void* src);


/** @brief 割込みハンドラから呼び出し可能なxfiber_queue_try_send_front()です
 */
XError xfiber_queue_send_front_isr(XFiberQueue* queue, const void* src);


/** @brief キューの先頭から要素の受信をタイムアウト付きで試みます
 */
XError xfiber_queue_timed_receive(XFiberQueue* queue, void* dst, XTicks timeout);


/** @brief キューの先頭から要素の受信を試みます
 */
XError xfiber_queue_receive(XFiberQueue* queue, void* dst);


/** @brief キューの先頭から要素の受信をポーリングで試みます
 */
XError xfiber_queue_try_receive(XFiberQueue* queue, void* dst);


/** @brief 割込みハンドラから呼び出し可能なxfiber_queue_try_receive()です
 */
XError xfiber_queue_receive_isr(XFiberQueue* queue, void* dst);


/** @} end of name fiber_queue
 */


/** @name fiber_channel
 *
 *  可変長のメッセージの受け渡しに使用します。リングバッファで実装されており、デ
 *  ータはバッファにコピーされます。
 *
 *  queueとの違いは以下の通りです
 *  + 可変長のため、要素ごとにsizeof(size_t)バイトの管理領域が必要
 *  + 先頭への送信は不可
 *  @{
 */


/** チャンネルを生成します
 *
 *  @param o_channel        生成したチャンネルのアドレスの格納先
 *  @param capacity         チャンネルバッファのバイト数
 *  @param max_item_size    1要素の最大バイト数
 *
 *  1要素ごとに管理領域が必要なため、capacity == 格納可能な総バイト数ではありま
 *  せん。
 *  要素の受信時は、格納先は暗黙的にmax_item_sizeバイト以上の領域を持つことを前
 *  提とします。
 */
XError xfiber_channel_create(XFiberChannel** o_channel, size_t capacity, size_t max_item_size);


/** チャンネル破棄します
 *
 *  全ての待ちタスクの待ちは解除され、待ちタスクにはX_ERR_CANCELEDが返ります
 */
void xfiber_channel_destroy(XFiberChannel* channel);


/** チャンネルの末尾へsizeバイトの要素の転送をタイムアウト付きで試みます
 */
XError xfiber_channel_timed_send(XFiberChannel* channel, const void* src, size_t size, XTicks timeout);


/** チャンネルの末尾へsizeバイトの要素の転送を試みます
 */
XError xfiber_channel_send(XFiberChannel* channel, const void* src, size_t size);


/** チャンネルの末尾へsizeバイトの要素の転送をポーリングで試みます
 */
XError xfiber_channel_try_send(XFiberChannel* channel, const void* src, size_t size);


/** 割込みハンドラから呼び出し可能なxfiber_channel_try_send()です
 */
XError xfiber_channel_send_isr(XFiberChannel* channel, const void* src, size_t size);


/** チャンネル先頭から要素の受信をタイムアウト付きで試みます
 */
XError xfiber_channel_timed_receive(XFiberChannel* channel, void* dst, size_t* o_size, XTicks timeout);


/** チャンネル先頭から要素の受信を試みます
 */
XError xfiber_channel_receive(XFiberChannel* channel, void* dst, size_t* o_size);


/** チャンネル先頭から要素の受信をポーリングで試みます
 */
XError xfiber_channel_try_receive(XFiberChannel* channel, void* dst, size_t* o_size);


/** 割込みハンドラから呼び出し可能なxfiber_channel_try_receive()です
 */
XError xfiber_channel_receive_isr(XFiberChannel* channel, void* dst, size_t* o_size);


/** @} end of name fiber_channel
 */


/** @name fiber_semaphore
 *  @brief 競合資源の排他制御に使用します
 *
 *  ミューテックスとの違いは、資源が複数ある場合に使用するカウントを持っているこ
 *  とです。
 *  @{
 */


/** @brief セマフォを生成します
 *
 *  @param o_semaphore      生成したセマフォのアドレスの格納先
 *  @param initial_count    カウンタの初期値
 */
XError xfiber_semaphore_create(XFiberSemaphore** o_semaphore, int initial_count);


/** @brief セマフォを破棄します
 *
 *  全ての待ちタスクの待ちは解除され、待ちタスクにはX_ERR_CANCELEDが返ります
 */
void xfiber_semaphore_destroy(XFiberSemaphore* semaphore);


/** @brief セマフォの獲得をタイムアウト付きで試みます
 */
XError xfiber_semaphore_timed_take(XFiberSemaphore* semaphore, XTicks timeout);


/** @brief セマフォの獲得を試みます
 */
XError xfiber_semaphore_take(XFiberSemaphore* semaphore);


/** @brief セマフォの獲得をポーリングで試みます
 */
XError xfiber_semaphore_try_take(XFiberSemaphore* semaphore);


/** @brief セマフォを返却します
 */
XError xfiber_semaphore_give(XFiberSemaphore* semaphore);


/** @brief 割込みハンドラで呼び出し可能なxfiber_semaphore_give()です
 */
XError xfiber_semaphore_give_isr(XFiberSemaphore* semaphore);


/** @} end of name fiber_semaphore
 */


/** @name fiber_mutex
 *  @brief 競合資源の排他制御に使用します
 *
 *  セマフォとの違いはロックされたままタスクが終了すると、自動的にロック解除を行
 *  う点と、優先度逆転防止機構を持つことです。
 *
 *  @attention
 *  自動ロック解除と、優先度逆転防止機構はまだ未実装です
 *  @{
 */


/** @brief ミューテックスを生成します
 */
XError xfiber_mutex_create(XFiberMutex** o_mutex);


/** @brief ミューテックスを破棄します
 */
void xfiber_mutex_destroy(XFiberMutex* mutex);


/** @brief ミューテックスを獲得をタイムアウト付きで試みます
 */
XError xfiber_mutex_timed_lock(XFiberMutex* mutex, XTicks timeout);


/** @brief ミューテックスを獲得を試みます
 */
XError xfiber_mutex_lock(XFiberMutex* mutex);


/** @brief ミューテックスを獲得をタイムアウト付きで試みます
 */
XError xfiber_mutex_try_lock(XFiberMutex* mutex);


/** @brief ミューテックスのロック解除を行います
 */
XError xfiber_mutex_unlock(XFiberMutex* mutex);


/** @brief 割込みハンドラから呼び出し可能なxfiber_mutex_unlock()です
 */
XError xfiber_mutex_unlock_isr(XFiberMutex* mutex);


/** @} end of name fiber_mutex
 */


/** @name fiber_mailbox
 *
 *  大きなデータの受け渡しに使用する通信機能です。リンクリストで実装しているので
 *  、データのコピーが行われません。また、送信待ちが絶対に発生しないのも大きな特
 *  徴です。
 *
 *  ただし、スタック上のデータを送信することができないという制限があり、データの
 *  寿命管理にも気をつける必要があります。
 *  @{
 */


/** @brief メールボックスを生成します
 */
XError xfiber_mailbox_create(XFiberMailbox** o_mailbox);


/** @brief メールボックスを生成します
 *
 *  全ての待ちタスクの待ちは解除され、待ちタスクにはX_ERR_CANCELEDが返ります
 */
void xfiber_mailbox_destroy(XFiberMailbox* mailbox);


/** @brief メールボックス末尾にメッセージを追加します
 */
XError xfiber_mailbox_send(XFiberMailbox* mailbox, XFiberMessage* message);


/** @brief 割込みハンドラから呼び出し可能なxfiber_mailbox_send()です
 */
XError xfiber_mailbox_send_isr(XFiberMailbox* mailbox, XFiberMessage* message);


/** @brief メールボックス先頭からメッセージの受信をタイムアウト付きで試みます
 */
XError xfiber_mailbox_timed_receive(XFiberMailbox* mailbox, XFiberMessage** o_message, XTicks timeout);


/** @brief メールボックス先頭からメッセージの受信を試みます
 */
XError xfiber_mailbox_receive(XFiberMailbox* mailbox, XFiberMessage** o_message);


/** @brief メールボックス先頭からメッセージの受信をポーリングで試みます
 */
XError xfiber_mailbox_try_receive(XFiberMailbox* mailbox, XFiberMessage** o_message);


/** @brief 割込みハンドラから呼び出し可能なxfiber_mailbox_try_receive()です
 */
XError xfiber_mailbox_receive_isr(XFiberMailbox* mailbox, XFiberMessage** o_message);


/** @} end of name fiber_mailbox
 */


/** @name fiber_pool
 *
 *  @brief 固定長のメモリブロックを確保と解放を行えます
 *
 *  メールボックスのメッセージのメモリ確保と相性がいいです。固定長なので、確保と
 *  解放は超高速です。
 *  @{
 */


/** @brief プールを生成します
 *
 *  @param o_pool       生成したプールのアドレスの格納先
 *  @param block_size   1ブロックのサイズ
 *  @param num_blocks   ブロック数
 */
XError xfiber_pool_create(XFiberPool** o_pool, size_t block_size, size_t num_blocks);


/** @brief プールを解放します
 *
 *  全ての待ちタスクの待ちは解除され、待ちタスクにはX_ERR_CANCELEDが返ります
 */
void xfiber_pool_destroy(XFiberPool* pool);


/** @brief プールからメモリの確保をタイムアウト付きで試みます
 */
XError xfiber_pool_timed_get(XFiberPool* pool, void** o_mem, XTicks timeout);


/** @brief プールからメモリの確保を試みます
 */
XError xfiber_pool_get(XFiberPool* pool, void** o_mem);


/** @brief プールからメモリの確保をポーリングで試みます
 */
XError xfiber_pool_try_get(XFiberPool* pool, void** o_mem);


/** @brief 割込みハンドラから呼び出し可能なxfiber_pool_try_get()です
 */
XError xfiber_pool_get_isr(XFiberPool* pool, void** o_mem);


/** @brief プールにメモリを返却します
 */
XError xfiber_pool_release(XFiberPool* pool, void* mem);


/** @brief 割込みハンドラから呼び出し可能なxfiber_pool_release()です
 */
XError xfiber_pool_release_isr(XFiberPool* pool, void* mem);


/** @} end of name fiber_pool
 */


#ifdef __cplusplus
}
#endif /* __cplusplus */


/** @} end of addtogroup xfiber
 *  @} end of addtogroup multitask
 */


#endif /* picox_multitask_xfiber_h_ */
