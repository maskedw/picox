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


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


typedef void* (*XFiberFunc)(void*);
typedef int(*XFiberIdleHook)(void);

#define X_FIBER_EVENT_WAIT_OR       (0)
#define X_FIBER_EVENT_WAIT_AND      (1)
#define X_FIBER_EVENT_CLEAR_ON_EXIT (1 << 1)

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

XError xfiber_kernel_init(void* heap, size_t heapsize, XFiberIdleHook idlehook);
XError xfiber_kernel_start_scheduler(void);

XError xfiber_create(XFiber** o_fiber, int priority, const char* name, size_t stack_size, XFiberFunc func, void* arg);
void xfiber_delay(XTicks time);
XError xfiber_suspend(XFiber* fiber);
XError xfiber_resume(XFiber* fiber);
void xfiber_yield();
XFiber* xfiber_self();
const char* xfiber_name(const XFiber* fiber);

XError xfiber_event_create(XFiberEvent** o_event, const char* name);
void xfiber_event_destroy(XFiberEvent* event);
XError xfiber_event_wait(XFiberEvent* event, XMode mode, XBits wait_pattern, XBits* result);
XError xfiber_event_try_wait(XFiberEvent* event, XMode mode, XBits wait_pattern, XBits* result);
XError xfiber_event_timed_wait(XFiberEvent* event, XMode mode, XBits wait_pattern, XBits* result, XTicks timeout);
XError xfiber_event_set(XFiberEvent* event, XBits pattern);
XError xfiber_event_set_isr(XFiberEvent* event, XBits pattern);
XError xfiber_event_clear(XFiberEvent* event, XBits pattern);
const char* xfiber_event_name(const XFiberEvent* event);

XError xfiber_signal_wait(XBits sigs, XBits* result);
XError xfiber_signal_try_wait(XBits sigs, XBits* result);
XError xfiber_signal_timed_wait(XBits sigs, XBits* result, XTicks timeout);
XError xfiber_signal_raise(XFiber* fiber, XBits sigs);
XError xfiber_signal_raise_isr(XFiber* fiber, XBits sigs);

XError xfiber_queue_create(XFiberQueue** o_queue, size_t queue_len, size_t item_size);
void xfiber_queue_destroy(XFiberQueue* queue);
XError xfiber_queue_send_back(XFiberQueue* queue, const void* src);
XError xfiber_queue_send_back_isr(XFiberQueue* queue, const void* src);
XError xfiber_queue_timed_send_back(XFiberQueue* queue, const void* src, XTicks timeout);
XError xfiber_queue_try_send_back(XFiberQueue* queue, const void* src);
XError xfiber_queue_send_front(XFiberQueue* queue, const void* src);
XError xfiber_queue_send_front_isr(XFiberQueue* queue, const void* src);
XError xfiber_queue_timed_send_front(XFiberQueue* queue, const void* src, XTicks timeout);
XError xfiber_queue_try_send_front(XFiberQueue* queue, const void* src);
XError xfiber_queue_receive(XFiberQueue* queue, void* dst);
XError xfiber_queue_timed_receive(XFiberQueue* queue, void* dst, XTicks timeout);
XError xfiber_queue_try_receive(XFiberQueue* queue, void* dst);
XError xfiber_queue_receive_isr(XFiberQueue* queue, void* dst);

XError xfiber_channel_create(XFiberChannel** o_channel, size_t capacity, size_t max_item_size);
void xfiber_channel_destroy(XFiberChannel* channel);
XError xfiber_channel_send(XFiberChannel* channel, const void* src, size_t size);
XError xfiber_channel_try_send(XFiberChannel* channel, const void* src, size_t size);
XError xfiber_channel_timed_send(XFiberChannel* channel, const void* src, size_t size, XTicks timeout);
XError xfiber_channel_send_isr(XFiberChannel* channel, const void* src, size_t size);
XError xfiber_channel_receive(XFiberChannel* channel, void* dst, size_t* o_size);
XError xfiber_channel_try_receive(XFiberChannel* channel, void* dst, size_t* o_size);
XError xfiber_channel_timed_receive(XFiberChannel* channel, void* dst, size_t* o_size, XTicks timeout);
XError xfiber_channel_receive_isr(XFiberChannel* channel, void* dst, size_t* o_size);

XError xfiber_mutex_create(XFiberMutex** o_mutex);
void xfiber_mutex_destroy(XFiberMutex* mutex);
XError xfiber_mutex_lock(XFiberMutex* mutex);
XError xfiber_mutex_timed_lock(XFiberMutex* mutex, XTicks timeout);
XError xfiber_mutex_try_lock(XFiberMutex* mutex);
XError xfiber_mutex_unlock(XFiberMutex* mutex);
XError xfiber_mutex_unlock_isr(XFiberMutex* mutex);

XError xfiber_semaphore_create(XFiberSemaphore** o_semaphore, int initial_count);
void xfiber_semaphore_destroy(XFiberSemaphore* semaphore);
XError xfiber_semaphore_take(XFiberSemaphore* semaphore);
XError xfiber_semaphore_try_take(XFiberSemaphore* semaphore);
XError xfiber_semaphore_timed_take(XFiberSemaphore* semaphore, XTicks timeout);
XError xfiber_semaphore_give(XFiberSemaphore* semaphore);
XError xfiber_semaphore_give_isr(XFiberSemaphore* semaphore);


XError xfiber_mailbox_create(XFiberMailbox** o_mailbox);
void xfiber_mailbox_destory(XFiberMailbox* mailbox);
XError xfiber_mailbox_send(XFiberMailbox* mailbox, XFiberMessage* message);
XError xfiber_mailbox_send_isr(XFiberMailbox* mailbox, XFiberMessage* message);
XError xfiber_mailbox_receive(XFiberMailbox* mailbox, XFiberMessage** o_message);
XError xfiber_mailbox_try_receive(XFiberMailbox* mailbox, XFiberMessage** o_message);
XError xfiber_mailbox_timed_receive(XFiberMailbox* mailbox, XFiberMessage** o_message, XTicks timeout);
XError xfiber_mailbox_receive_isr(XFiberMailbox* mailbox, XFiberMessage** o_message);


XError xfiber_pool_create(XFiberPool** o_pool, size_t block_size, size_t num_blocks);
void xfiber_pool_destroy(XFiberPool* pool);
XError xfiber_pool_get(XFiberPool* pool, void** o_mem);
XError xfiber_pool_try_get(XFiberPool* pool, void** o_mem);
XError xfiber_pool_timed_get(XFiberPool* pool, void** o_mem, XTicks timeout);
XError xfiber_pool_get_isr(XFiberPool* pool, void** o_mem);
XError xfiber_pool_release(XFiberPool* pool, void* mem);
XError xfiber_pool_release_isr(XFiberPool* pool, void* mem);


#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* picox_multitask_xfiber_h_ */
