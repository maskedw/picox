/**
 *       @file  xfiber.c
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
 * Copyright (c) <2015> <MaskedW [maskedw00@gmail.com]>
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of self software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, X__GetStackDepthribute, sublicense, and/or sell copies
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


#include <picox/multitask/xfiber.h>
#include <picox/container/xintrusive_list.h>
#include <picox/container/xcircular_buffer.h>
#include <picox/allocator/xpico_allocator.h>
#include <picox/multitask/xvtimer.h>


#define X__NODE_TO_FIBER(node)     xnode_entry(node, XFiber, m_node)
#define X_FIBER_ENTER_CRITICAL()
#define X_FIBER_EXIT_CRITICAL()

#define X_FIBER_IMPL_TYPE_COPY_STACK        (0)
#define X_FIBER_IMPL_TYPE_UCONTEXT          (1)
#define X_FIBER_IMPL_TYPE_PLATFORM_DEPEND   (2)

#define X_CONF_FIBER_IMPL_TYPE          X_FIBER_IMPL_TYPE_COPY_STACK
// #define X_CONF_FIBER_IMPL_TYPE          X_FIBER_IMPL_TYPE_UCONTEXT

#ifndef X_CONF_FIBER_IMPL_TYPE
#define X_CONF_FIBER_IMPL_TYPE          X_FIBER_IMPL_TYPE_COPY_STACK
#endif


#if X_CONF_FIBER_IMPL_TYPE == X_FIBER_IMPL_TYPE_COPY_STACK

    #include <setjmp.h>
    struct XFiberContext
    {
        jmp_buf     m_jmpbuf;
        uint8_t*    m_machine_stack_end;
    };

    typedef struct XFiberContext    XFiberContext;


#elif X_CONF_FIBER_IMPL_TYPE == X_FIBER_IMPL_TYPE_UCONTEXT

    #include <ucontext.h>
    typedef ucontext_t              XFiberContext;

#elif X_CONF_FIBER_IMPL_TYPE == X_FIBER_IMPL_TYPE_PLATFORM_DEPEND
    #error not implemented yet
#else
    #error invalid configuration
#endif


typedef enum
{
    X_FIBER_STATE_READY,
    X_FIBER_STATE_RUNNING,
    X_FIBER_STATE_WAITING_EVENT,
    X_FIBER_STATE_WAITING_DELAY,
    X_FIBER_STATE_WAITING_SIGNAL,
    X_FIBER_STATE_WAITING_SEND_QUEUE,
    X_FIBER_STATE_WAITING_RECV_QUEUE,
    X_FIBER_STATE_WAITING_MUTEX,
    X_FIBER_STATE_SUSPEND = (1 << 8),
    X_FIBER_STATE_SUSPEND_AND_WAITING_EVENT = X_FIBER_STATE_SUSPEND | X_FIBER_STATE_WAITING_EVENT,
    X_FIBER_STATE_SUSPEND_AND_WAITING_DELAY = X_FIBER_STATE_SUSPEND | X_FIBER_STATE_WAITING_DELAY,
    X_FIBER_STATE_SUSPEND_AND_WAITING_SIGNAL = X_FIBER_STATE_SUSPEND | X_FIBER_STATE_WAITING_SIGNAL,
    X_FIBER_STATE_SUSPEND_AND_WAITING_SEND_QUEUE = X_FIBER_STATE_SUSPEND | X_FIBER_STATE_WAITING_SEND_QUEUE,
    X_FIBER_STATE_SUSPEND_AND_WAITING_RECV_QUEUE = X_FIBER_STATE_SUSPEND | X_FIBER_STATE_WAITING_RECV_QUEUE,
    X_FIBER_STATE_SUSPEND_AND_WAITING_MUTEX = X_FIBER_STATE_SUSPEND | X_FIBER_STATE_WAITING_MUTEX,
} XFiberState;


#define X_FIBER_EVENT_WAIT_MASK             (X_FIBER_EVENT_WAIT_OR | X_FIBER_EVENT_WAIT_AND)

#define X_FIBER_IS_READY(state)             ((state) == X_FIBER_STATE_READY)
#define X_FIBER_IS_RUNNING(state)           ((state) == X_FIBER_STATE_RUNNING)
#define X_FIBER_IS_SUSPEND(state)           ((state) & X_FIBER_STATE_SUSPEND)
#define X_FIBER_IS_WAITING_SUSPEND(state)   ((state) >= X_FIBER_STATE_SUSPEND_AND_WAITING_EVENT)
#define X_FIBER_IS_WAITING(state)           (((state) & 0xFF) >= X_FIBER_STATE_WAITING_EVENT)


struct XFiberObject;
typedef struct XFiberObject* XFiberObject;
typedef void (*XFiberTimeEventHandler)(XFiber*);

struct XFiberObject
{
    XIntrusiveNode      node;
    char                name;
    int                 type;
};


typedef enum
{
    X_FIBER_OBJTYPE_TASK,
    X_FIBER_OBJTYPE_EVENT,
    X_FIBER_OBJTYPE_QUEUE,
    X_FIBER_OBJTYPE_MUTEX,
} XFiberObjectType;


#define X_DECLAER_FIBER_OBJECT_MEMBERS \
    XIntrusiveNode      m_node;         \
    char                m_name[16];     \
    XFiberObjectType    m_type


struct XFiber
{
/** @privatesection */
    X_DECLAER_FIBER_OBJECT_MEMBERS;
    size_t              m_stack_size;
    int                 m_priority;
    XFiberFunc          m_func;
    void*               m_arg;
    uint8_t*            m_stack;
    XFiberState         m_state;
    XBits               m_wait_sigs;
    XBits               m_recv_sigs;
    XBits               m_result_sigs;

    XFiberContext       m_context;
    XError              m_result_waiting;
    XVTimerRequest      m_timer_request;

    /* TODO unionにまとめる */
    XBits               m_result_event_patten;
    XBits               m_wait_event_pattern;
    XMode               m_wait_event_mode;
    const void*         m_pending_send_src;
    void*               m_pending_recv_dst;
};


struct XFiberEvent
{
    X_DECLAER_FIBER_OBJECT_MEMBERS;
    XIntrusiveList      m_pending_tasks;
    XBits               m_pattern;
};


struct XFiberQueue
{
    X_DECLAER_FIBER_OBJECT_MEMBERS;
    XIntrusiveList      m_pending_tasks;
    XCircularBuffer     m_buffer;
    size_t              m_item_size;
};


struct XFiberMutex
{
    X_DECLAER_FIBER_OBJECT_MEMBERS;
    XIntrusiveList      m_pending_tasks;
    XFiber*             m_holder;
};


typedef struct X__Kernel
{
    XFiber*             m_cur_task;
    XIntrusiveList      m_ready_queue;
    XIntrusiveList      m_delay_queue;
    XPicoAllocator      m_alloc;
    XFiberIdleHook      m_idlehook;
    XFiberContext       m_return_ctx;
    XTicks              m_timepoint;
    XVTimer             m_vtimer;

#if X_CONF_FIBER_IMPL_TYPE == X_FIBER_IMPL_TYPE_COPY_STACK
    uint8_t*            m_machine_stack_begin;
#endif
} X__Kernel;


static void X__PushToReadyQueue(XFiber* fiber);
static XFiber* X__PopFromReadyQueue(void);
static void X__Schedule(void);
static void X__ReleaseWaiting(XFiber* fiber, XError result);
static void* X__Malloc(size_t size);
static void X__Free(void* ptr);
static void X__FiberMain(XFiber* fiber);
static bool X__TestEvent(XFiberEvent* event, XMode mode, XBits wait_pattern, XBits* result);
static void X__TimeoutHandler(XFiber* fiber);
static void X__AddTimerEvent(XFiber* fiber, XFiberTimeEventHandler handler, XTicks time);

static void X__StartSchedule(void);
static void X__EndSchedule();
static void X__MakeContext(XFiber* fiber, XFiberFunc func, void* arg, void* stack, size_t stack_size);
static void X__SwapContext(XFiber* from, XFiber* to);
static void X__SetContext(XFiber* to);
static void* X__ResolvePtr(const XFiber* fiber, const void* ptr);

#if X_CONF_FIBER_IMPL_TYPE == X_FIBER_IMPL_TYPE_COPY_STACK
static void X__GetStackPtr(uint8_t** volatile dst);
static void X__RestoreStack(XFiber* fiber, uint8_t* addr_in_prev_frame);
static void X__SaveStack(XFiber* fiber, const uint8_t* stack_end);
#endif


#define X__LOG      X_LOG_NOTI
static const char* const X__TAG = "XFiber";


X__Kernel        x_g_fiber_kernel;
#define priv    (&x_g_fiber_kernel)


XError xfiber_kernel_init(void* heap, size_t heapsize, XFiberIdleHook idlehook)
{
    xilist_init(&priv->m_ready_queue);
    xilist_init(&priv->m_delay_queue);
    xpalloc_init(&priv->m_alloc, heap, heapsize, X_ALIGN_OF(XMaxAlign));
    xvtimer_init(&priv->m_vtimer);
    priv->m_idlehook = idlehook;

    return X_ERR_NONE;
}


XFiber* xfiber_self(void)
{
    return priv->m_cur_task;
}


void xfiber_yield()
{
    X__Schedule();
}



XError xfiber_create(XFiber** o_fiber,
                     int priority,
                     const char* name,
                     size_t stack_size,
                     XFiberFunc func,
                     void* arg)
{
    XError err = X_ERR_NONE;
    uint8_t* stack = NULL;

    XFiber* fiber = X__Malloc(sizeof(XFiber));
    if (!fiber)
    {
        err = X_ERR_NO_MEMORY;
        goto x__exit;
    }

    stack = X__Malloc(stack_size);
    if (!stack)
    {
        err = X_ERR_NO_MEMORY;
        goto x__exit;
    }

    if (name)
        x_strlcpy(fiber->m_name, name, sizeof(fiber->m_name));
    else
        fiber->m_name[0] = '\0';

    fiber->m_func = func;
    fiber->m_arg = arg;
    fiber->m_stack = stack;
    fiber->m_stack_size = stack_size;
    fiber->m_priority = priority;
    fiber->m_type = X_FIBER_OBJTYPE_TASK;
    fiber->m_wait_sigs = 0;
    fiber->m_recv_sigs = 0;

    xvtimer_init_request(&fiber->m_timer_request);

    X__MakeContext(fiber, func, arg, stack, stack_size);

    X_FIBER_ENTER_CRITICAL();
    {
        X__PushToReadyQueue(fiber);
    }
    X_FIBER_EXIT_CRITICAL();

    *o_fiber = fiber;
    fiber = NULL;
    stack = NULL;

x__exit:
    X__Free(fiber);
    X__Free(stack);

    return err;
}


XError xfiber_kernel_start_scheduler(void)
{
    X__LOG((X__TAG, "start schedule"));

    X_FIBER_ENTER_CRITICAL();
    {
        XFiber* fiber = X__PopFromReadyQueue();
        fiber->m_state = X_FIBER_STATE_RUNNING;
        priv->m_cur_task = fiber;
    }
    X_FIBER_EXIT_CRITICAL();

    priv->m_timepoint = x_ticks_now();
    X__StartSchedule();

    X__LOG((X__TAG, "end schedule"));

    return X_ERR_NONE;
}


const char* xfiber_name(const XFiber* fiber)
{
    if (!fiber)
        fiber = priv->m_cur_task;
    return fiber->m_name;
}


XError xfiber_event_create(XFiberEvent** o_event, const char* name)
{
    XError err = X_ERR_NONE;
    XFiberEvent* event = X__Malloc(sizeof(*event));
    if (!event)
    {
        err = X_ERR_NO_MEMORY;
        goto x__exit;
    }

    strcpy(event->m_name, name);
    event->m_pattern = 0;
    xilist_init(&event->m_pending_tasks);
    event->m_type = X_FIBER_OBJTYPE_EVENT;
    *o_event = event;

x__exit:

    return err;
}


void xfiber_event_destroy(XFiberEvent* event)
{
    X__Free(event);
}


XError xfiber_event_wait(XFiberEvent* event, XMode mode, XBits wait_pattern, XBits* result)
{
    return xfiber_event_timed_wait(event, mode, wait_pattern, result, X_TICKS_FOREVER);
}


XError xfiber_event_try_wait(XFiberEvent* event, XMode mode, XBits wait_pattern, XBits* result)
{
    return xfiber_event_timed_wait(event, mode, wait_pattern, result, 0);
}


XError xfiber_event_timed_wait(XFiberEvent* event, XMode mode, XBits wait_pattern, XBits* result, XTicks timeout)
{
    volatile XError err = X_ERR_NONE;
    XFiber* const fiber = xfiber_self();

    X_FIBER_ENTER_CRITICAL();
    {
        if (X__TestEvent(event, mode, wait_pattern, result))
        {
            X_FIBER_EXIT_CRITICAL();
            goto x__exit;
        }

        if (timeout == 0)
        {
            err = X_ERR_TIMED_OUT;
            X_FIBER_EXIT_CRITICAL();
            goto x__exit;
        }

        xilist_push_back(&event->m_pending_tasks, &fiber->m_node);
        fiber->m_wait_event_pattern = wait_pattern;
        fiber->m_wait_event_mode = mode;
        fiber->m_state = X_FIBER_STATE_WAITING_EVENT;

        if (timeout > 0)
            X__AddTimerEvent(fiber, X__TimeoutHandler, timeout);
    }
    X_FIBER_EXIT_CRITICAL();

    X__Schedule();

    *result = fiber->m_result_event_patten;
    err = fiber->m_result_waiting;

x__exit:
    return err;
}


XError xfiber_event_set_isr(XFiberEvent* event, XBits pattern)
{
    XError err = X_ERR_NONE;

    event->m_pattern |= pattern;
    XIntrusiveNode* ite = xilist_front(&event->m_pending_tasks);
    XIntrusiveNode* const end = xilist_end(&event->m_pending_tasks);

    while (ite != end)
    {
        XFiber* const fiber = xnode_entry(ite, XFiber, m_node);
        XIntrusiveNode* const next = ite->next;

        if (X__TestEvent(event,
                         fiber->m_wait_event_mode,
                         fiber->m_wait_event_pattern,
                         &fiber->m_result_event_patten))
        {
            X__ReleaseWaiting(fiber, X_ERR_NONE);
            if (fiber->m_wait_event_mode & X_FIBER_EVENT_CLEAR_ON_EXIT)
                break;
        }
        ite = next;
    }

    return err;
}


XError xfiber_event_set(XFiberEvent* event, XBits pattern)
{
    XError err = X_ERR_NONE;
    bool scheduling_request = false;

    X_FIBER_ENTER_CRITICAL();
    {
        event->m_pattern |= pattern;
        XIntrusiveNode* ite = xilist_front(&event->m_pending_tasks);
        XIntrusiveNode* const end = xilist_end(&event->m_pending_tasks);

        while (ite != end)
        {
            XFiber* const fiber = xnode_entry(ite, XFiber, m_node);
            XIntrusiveNode* const next = ite->next;

            if (X__TestEvent(event,
                             fiber->m_wait_event_mode,
                             fiber->m_wait_event_pattern,
                             &fiber->m_result_event_patten))
            {
                X__ReleaseWaiting(fiber, X_ERR_NONE);
                scheduling_request = true;
                if (fiber->m_wait_event_mode & X_FIBER_EVENT_CLEAR_ON_EXIT)
                    break;
            }
            ite = next;
        }
    }
    X_FIBER_EXIT_CRITICAL();

    if (scheduling_request)
        X__Schedule();

    return err;
}


XError xfiber_event_clear(XFiberEvent* event, XBits pattern)
{
    X_FIBER_ENTER_CRITICAL();
    {
        event->m_pattern &= ~pattern;
    }
    X_FIBER_EXIT_CRITICAL();

    return X_ERR_NONE;
}


XError xfiber_signal_wait(XBits sigs, XBits* result)
{
    return xfiber_signal_timed_wait(sigs, result, X_TICKS_FOREVER);
}


XError xfiber_signal_try_wait(XBits sigs, XBits* result)
{
    return xfiber_signal_timed_wait(sigs, result, 0);
}


XError xfiber_signal_timed_wait(XBits sigs, XBits* result, XTicks timeout)
{
    volatile XError err = X_ERR_NONE;
    if (!sigs)
    {
        *result = 0;
        goto x__exit;
    }

    XFiber* const fiber = xfiber_self();
    X_FIBER_ENTER_CRITICAL();
    {
        *result = sigs & fiber->m_recv_sigs;
        if (*result)
        {
            fiber->m_recv_sigs &= ~(*result);
            X_FIBER_EXIT_CRITICAL();
            goto x__exit;
        }

        if (timeout == 0)
        {
            err = X_ERR_TIMED_OUT;
            X_FIBER_EXIT_CRITICAL();
            goto x__exit;
        }

        xilist_push_back(&priv->m_delay_queue, &fiber->m_node);
        fiber->m_wait_sigs = sigs;
        fiber->m_state = X_FIBER_STATE_WAITING_SIGNAL;

        if (timeout > 0)
            X__AddTimerEvent(fiber, X__TimeoutHandler, timeout);
    }
    X_FIBER_EXIT_CRITICAL();

    X__Schedule();

    err = fiber->m_result_waiting;

    if (err == X_ERR_NONE)
    {
        X_FIBER_ENTER_CRITICAL();
        {
            *result = sigs & fiber->m_recv_sigs;
            X_ASSERT(*result);
            fiber->m_recv_sigs &= ~(*result);
        }
        X_FIBER_EXIT_CRITICAL();
    }

x__exit:
    return err;
}


XError xfiber_signal_raise(XFiber* fiber, XBits sigs)
{
    XError err = X_ERR_NONE;
    bool scheduling_request = false;

    X_FIBER_ENTER_CRITICAL();
    {
        fiber->m_recv_sigs |= sigs;
        if (fiber->m_wait_sigs & sigs)
        {
            X__ReleaseWaiting(fiber, X_ERR_NONE);
            fiber->m_wait_sigs = 0;
            scheduling_request = true;
        }
    }
    X_FIBER_EXIT_CRITICAL();

    if (scheduling_request)
        X__Schedule();

    return err;
}


XError xfiber_signal_raise_isr(XFiber* fiber, XBits sigs)
{
    XError err = X_ERR_NONE;

    fiber->m_recv_sigs |= sigs;
    if (fiber->m_wait_sigs & fiber->m_recv_sigs)
    {
        X__ReleaseWaiting(fiber, X_ERR_NONE);
        fiber->m_wait_sigs = 0;
    }

    return err;
}


void xfiber_delay(XTicks time)
{
    if (time == 0)
        return;

    X_FIBER_ENTER_CRITICAL();
    {
        XFiber* const self = priv->m_cur_task;
        self->m_state= X_FIBER_STATE_WAITING_DELAY;
        xilist_push_back(&priv->m_delay_queue, &self->m_node);
        X__AddTimerEvent(self, X__TimeoutHandler, time);
    }
    X_FIBER_EXIT_CRITICAL();

    X__Schedule();
}


XError xfiber_suspend(XFiber* fiber)
{
    XError err = X_ERR_NONE;
    bool scheduling_request = false;

    if (!fiber)
        fiber = priv->m_cur_task;

    X_FIBER_ENTER_CRITICAL();
    {
        if (X_FIBER_IS_SUSPEND(fiber->m_state))
        {
            X_FIBER_EXIT_CRITICAL();
            goto x__exit;
        }

        if (X_FIBER_IS_WAITING(fiber->m_state))
        {
            fiber->m_state |= X_FIBER_STATE_SUSPEND;
        }
        else
        {
            fiber->m_state =  X_FIBER_STATE_SUSPEND;
            xilist_push_back(&priv->m_delay_queue, &fiber->m_node);
            scheduling_request = (fiber == priv->m_cur_task);
        }
    }
    X_FIBER_EXIT_CRITICAL();

    if (scheduling_request)
        X__Schedule();

x__exit:
    return err;
}


XError xfiber_resume(XFiber* fiber)
{
    XError err = X_ERR_NONE;
    bool scheduling_request = false;

    X_FIBER_ENTER_CRITICAL();
    {
        if (!X_FIBER_IS_SUSPEND(fiber->m_state))
        {
            X_FIBER_EXIT_CRITICAL();
            goto x__exit;
        }

        if (X_FIBER_IS_WAITING_SUSPEND(fiber->m_state))
        {
            fiber->m_state &= ~X_FIBER_STATE_SUSPEND;
        }
        else
        {
            xnode_unlink(&fiber->m_node);
            X__PushToReadyQueue(fiber);
            scheduling_request = true;
        }
    }
    X_FIBER_EXIT_CRITICAL();

    if (scheduling_request)
        X__Schedule();

x__exit:
    return err;
}


XError xfiber_queue_create(XFiberQueue** o_queue, size_t queue_len, size_t item_size)
{
    if (queue_len == 0)
        return X_ERR_INVALID;
    if (item_size == 0)
        return X_ERR_INVALID;
    if (!o_queue)
        return X_ERR_INVALID;

    XFiberQueue* queue = X__Malloc(sizeof(XFiberQueue) + queue_len * item_size);
    if (!queue)
        return X_ERR_NO_MEMORY;

    xcbuf_init(&queue->m_buffer, (uint8_t*)queue + sizeof(XFiberQueue), queue_len * item_size);
    xilist_init(&queue->m_pending_tasks);
    queue->m_type = X_FIBER_OBJTYPE_QUEUE;
    queue->m_item_size = item_size;
    *o_queue = queue;

    return X_ERR_NONE;
}


XError xfiber_queue_send_back(XFiberQueue* queue, const void* src)
{
    return xfiber_queue_timed_send_back(queue, src, X_TICKS_FOREVER);
}


XError xfiber_queue_try_send_back(XFiberQueue* queue, const void* src)
{
    return xfiber_queue_timed_send_back(queue, src, 0);
}


XError xfiber_queue_timed_send_back(XFiberQueue* queue, const void* src, XTicks timeout)
{
    XError err = X_ERR_NONE;
    bool scheduling_request = false;
    XFiber* cur_task = priv->m_cur_task;

    X_FIBER_ENTER_CRITICAL();
    {
        if (xcbuf_empty(&queue->m_buffer) && (!xilist_empty(&queue->m_pending_tasks)))
        {
            XFiber* const pend_task = X__NODE_TO_FIBER(xilist_pop_front(&queue->m_pending_tasks));
            memcpy(pend_task->m_pending_recv_dst, src, queue->m_item_size);
            X__ReleaseWaiting(pend_task, X_ERR_NONE);
            scheduling_request = true;
        }
        else if (xcbuf_reserve(&queue->m_buffer) >= queue->m_item_size)
        {
            xcbuf_push_back_n(&queue->m_buffer, src, queue->m_item_size);
        }
        else if (timeout == 0)
        {
            err = X_ERR_TIMED_OUT;
            X_FIBER_EXIT_CRITICAL();
            goto x__exit;
        }
        else
        {
            scheduling_request = true;
            xilist_push_back(&queue->m_pending_tasks, &cur_task->m_node);
            cur_task->m_pending_send_src = X__ResolvePtr(cur_task, src);
            cur_task->m_state = X_FIBER_STATE_WAITING_SEND_QUEUE;
            if (timeout > 0)
                X__AddTimerEvent(cur_task, X__TimeoutHandler, timeout);
        }
    }
    X_FIBER_EXIT_CRITICAL();

    if (scheduling_request)
    {
        X__Schedule();
        err = cur_task->m_result_waiting;
    }

x__exit:
    return err;
}


XError xfiber_queue_send_back_isr(XFiberQueue* queue, const void* src)
{
    XError err = X_ERR_NONE;

    if (xcbuf_empty(&queue->m_buffer) && (!xilist_empty(&queue->m_pending_tasks)))
    {
        XFiber* const pend_task = X__NODE_TO_FIBER(xilist_pop_front(&queue->m_pending_tasks));
        memcpy(pend_task->m_pending_recv_dst, src, queue->m_item_size);
        X__ReleaseWaiting(pend_task, X_ERR_NONE);
    }
    else if (xcbuf_reserve(&queue->m_buffer) >= queue->m_item_size)
    {
        xcbuf_push_back_n(&queue->m_buffer, src, queue->m_item_size);
    }
    else
    {
        err = X_ERR_TIMED_OUT;
    }

    return err;
}


XError xfiber_queue_send_front(XFiberQueue* queue, const void* src)
{
    return xfiber_queue_timed_send_front(queue, src, X_TICKS_FOREVER);
}


XError xfiber_queue_try_send_front(XFiberQueue* queue, const void* src)
{
    return xfiber_queue_timed_send_front(queue, src, 0);
}


XError xfiber_queue_timed_send_front(XFiberQueue* queue, const void* src, XTicks timeout)
{
    XError err = X_ERR_NONE;
    bool scheduling_request = false;
    XFiber* cur_task = priv->m_cur_task;

    X_FIBER_ENTER_CRITICAL();
    {
        if (xcbuf_empty(&queue->m_buffer) && (!xilist_empty(&queue->m_pending_tasks)))
        {
            XFiber* const pend_task = X__NODE_TO_FIBER(xilist_pop_front(&queue->m_pending_tasks));
            memcpy(pend_task->m_pending_recv_dst, src, queue->m_item_size);
            X__ReleaseWaiting(pend_task, X_ERR_NONE);
            scheduling_request = true;
        }
        else if (xcbuf_reserve(&queue->m_buffer) >= queue->m_item_size)
        {
            xcbuf_push_front_n(&queue->m_buffer, src, queue->m_item_size);
        }
        else if (timeout == 0)
        {
            err = X_ERR_TIMED_OUT;
            X_FIBER_EXIT_CRITICAL();
            goto x__exit;
        }
        else
        {
            scheduling_request = true;
            xilist_push_back(&queue->m_pending_tasks, &cur_task->m_node);
            cur_task->m_pending_send_src = X__ResolvePtr(cur_task, src);
            cur_task->m_state = X_FIBER_STATE_WAITING_SEND_QUEUE;
            if (timeout > 0)
                X__AddTimerEvent(cur_task, X__TimeoutHandler, timeout);
        }
    }
    X_FIBER_EXIT_CRITICAL();

    if (scheduling_request)
    {
        X__Schedule();
        err = cur_task->m_result_waiting;
    }

x__exit:
    return err;
}


XError xfiber_queue_send_front_isr(XFiberQueue* queue, const void* src)
{
    XError err = X_ERR_NONE;

    if (xcbuf_empty(&queue->m_buffer) && (!xilist_empty(&queue->m_pending_tasks)))
    {
        XFiber* const pend_task = X__NODE_TO_FIBER(xilist_pop_front(&queue->m_pending_tasks));
        memcpy(pend_task->m_pending_recv_dst, src, queue->m_item_size);
        X__ReleaseWaiting(pend_task, X_ERR_NONE);
    }
    else if (xcbuf_reserve(&queue->m_buffer) >= queue->m_item_size)
    {
        xcbuf_push_front_n(&queue->m_buffer, src, queue->m_item_size);
    }
    else
    {
        err = X_ERR_TIMED_OUT;
    }

    return err;
}


XError xfiber_queue_receive(XFiberQueue* queue, void* dst)
{
    return xfiber_queue_timed_receive(queue, dst, X_TICKS_FOREVER);
}


XError xfiber_queue_try_receive(XFiberQueue* queue, void* dst)
{
    return xfiber_queue_timed_receive(queue, dst, 0);
}


XError xfiber_queue_timed_receive(XFiberQueue* queue, void* dst, XTicks timeout)
{
    XError err = X_ERR_NONE;
    bool scheduling_request = false;
    XFiber* const cur_task = priv->m_cur_task;

    X_FIBER_ENTER_CRITICAL();
    {
        if (xcbuf_size(&queue->m_buffer) >= queue->m_item_size)
        {
            xcbuf_pop_front_n(&queue->m_buffer, dst, queue->m_item_size);

            /* 送信待ちのタスクがあれば、バッファに格納して待ちを解除する */
            if (!xilist_empty(&queue->m_pending_tasks))
            {
                XFiber* const pend_task = xnode_entry(
                        xilist_pop_front(&queue->m_pending_tasks),
                        XFiber, m_node);

                xcbuf_push_back_n(&queue->m_buffer,
                                  pend_task->m_pending_send_src,
                                  queue->m_item_size);

                X__ReleaseWaiting(pend_task, X_ERR_NONE);
                scheduling_request = true;
            }
        }
        else if (timeout == 0)
        {
            err = X_ERR_TIMED_OUT;
            X_FIBER_EXIT_CRITICAL();
            goto x__exit;
        }
        else
        {
            scheduling_request = true;
            xilist_push_back(&queue->m_pending_tasks, &cur_task->m_node);
            cur_task->m_pending_recv_dst = X__ResolvePtr(cur_task, dst);
            cur_task->m_state = X_FIBER_STATE_WAITING_RECV_QUEUE;
            if (timeout > 0)
                X__AddTimerEvent(cur_task, X__TimeoutHandler, timeout);
        }
    }
    X_FIBER_EXIT_CRITICAL();

    if (scheduling_request)
    {
        X__Schedule();
        err = cur_task->m_result_waiting;
    }

x__exit:
    return err;
}


XError xfiber_queue_receive_isr(XFiberQueue* queue, void* dst)
{
    XError err = X_ERR_NONE;

    if (xcbuf_size(&queue->m_buffer) >= queue->m_item_size)
    {
        xcbuf_pop_front_n(&queue->m_buffer, dst, queue->m_item_size);

        if (!xilist_empty(&queue->m_pending_tasks))
        {
            XFiber* const pend_task = xnode_entry(
                    xilist_pop_front(&queue->m_pending_tasks),
                    XFiber, m_node);

            xcbuf_push_back_n(&queue->m_buffer,
                              pend_task->m_pending_send_src,
                              queue->m_item_size);

            X__ReleaseWaiting(pend_task, X_ERR_NONE);
        }
    }
    else
    {
        err = X_ERR_TIMED_OUT;
    }

    return err;
}


XError xfiber_mutex_create(XFiberMutex** o_mutex)
{
    if (!o_mutex)
        return X_ERR_INVALID;

    XFiberMutex* mutex = X__Malloc(sizeof(XFiberMutex));
    if (!mutex)
        return X_ERR_NO_MEMORY;

    xilist_init(&mutex->m_pending_tasks);
    mutex->m_type = X_FIBER_OBJTYPE_MUTEX;
    mutex->m_holder = NULL;
    *o_mutex = mutex;

    return X_ERR_NONE;
}


XError xfiber_mutex_lock(XFiberMutex* mutex)
{
    return xfiber_mutex_timed_lock(mutex, X_TICKS_FOREVER);
}


XError xfiber_mutex_try_lock(XFiberMutex* mutex)
{
    return xfiber_mutex_timed_lock(mutex, 0);
}


XError xfiber_mutex_timed_lock(XFiberMutex* mutex, XTicks timeout)
{
    XError err = X_ERR_NONE;
    bool scheduling_request = false;
    XFiber* const cur_task = priv->m_cur_task;

    X_FIBER_ENTER_CRITICAL();
    {
        if (!mutex->m_holder)
        {
            mutex->m_holder = cur_task;
        }
        else if (timeout == 0)
        {
            err = X_ERR_TIMED_OUT;
            X_FIBER_EXIT_CRITICAL();
            goto x__exit;
        }
        else
        {
            scheduling_request = true;
            xilist_push_back(&mutex->m_pending_tasks, &cur_task->m_node);
            cur_task->m_state = X_FIBER_STATE_WAITING_MUTEX;
            if (timeout > 0)
                X__AddTimerEvent(cur_task, X__TimeoutHandler, timeout);
        }
    }
    X_FIBER_EXIT_CRITICAL();

    if (scheduling_request)
    {
        X__Schedule();
        err = cur_task->m_result_waiting;
    }

x__exit:
    return err;
}


XError xfiber_mutex_unlock(XFiberMutex* mutex)
{
    XError err = X_ERR_NONE;
    bool scheduling_request = false;

    X_FIBER_ENTER_CRITICAL();
    {
        if (!mutex->m_holder)
        {
            /* ロックされていない */
            err = X_ERR_INVALID;
            X_FIBER_EXIT_CRITICAL();
            goto x__exit;
        }
        else
        {
            mutex->m_holder = NULL;
            if (xilist_empty(&mutex->m_pending_tasks))
                goto x__exit;

            XFiber* const pend_task = X__NODE_TO_FIBER(
                    xilist_pop_front(&mutex->m_pending_tasks));
            mutex->m_holder = pend_task;
            X__ReleaseWaiting(pend_task, X_ERR_NONE);
            scheduling_request = true;
        }
    }
    X_FIBER_EXIT_CRITICAL();

    if (scheduling_request)
        X__Schedule();

x__exit:
    return err;
}


XError xfiber_mutex_unlock_isr(XFiberMutex* mutex)
{
    XError err = X_ERR_NONE;
    if (!mutex->m_holder)
        return X_ERR_INVALID;

    mutex->m_holder = NULL;
    if (xilist_empty(&mutex->m_pending_tasks))
        return X_ERR_NONE;

    XFiber* const pend_task = X__NODE_TO_FIBER(
            xilist_pop_front(&mutex->m_pending_tasks));
    mutex->m_holder = pend_task;
    X__ReleaseWaiting(pend_task, X_ERR_NONE);

    return err;
}


static void X__AddTimerEvent(XFiber* fiber, XFiberTimeEventHandler handler, XTicks time)
{
    const XTicks now = x_ticks_now();
    const XTicks cur_step = now - priv->m_timepoint;
    time += cur_step;
    xvtimer_add_request(&priv->m_vtimer,
                        &fiber->m_timer_request,
                        (XVTimerCallBack)handler,
                        fiber,
                        0,
                        time,
                        true);
}


static void X__ReleaseWaiting(XFiber* fiber, XError result)
{
    xnode_unlink(&fiber->m_node);
    xvtimer_remove_requst(&priv->m_vtimer, &fiber->m_timer_request);
    fiber->m_result_waiting = result;

    if (X_FIBER_IS_WAITING_SUSPEND(fiber->m_state))
    {
        fiber->m_state = X_FIBER_STATE_SUSPEND;
        xilist_push_back(&priv->m_delay_queue, &fiber->m_node);
    }
    else
    {
        X__PushToReadyQueue(fiber);
    }
}


static void X__TimeoutHandler(XFiber* fiber)
{
    X__ReleaseWaiting(fiber, X_ERR_TIMED_OUT);
}


static void X__PushToReadyQueue(XFiber* fiber)
{
    /* priority */
    fiber->m_state = X_FIBER_STATE_READY;
    xilist_push_back(&priv->m_ready_queue, &fiber->m_node);
}


static XFiber* X__PopFromReadyQueue(void)
{
    XIntrusiveNode* const next = xilist_front(&priv->m_ready_queue);
    X_ASSERT(next != xilist_end(&priv->m_ready_queue) && "dead lock!!");

    xnode_unlink(next);
    XFiber* const fiber = xnode_entry(next, XFiber, m_node);
    return fiber;
}


static void X__Schedule(void)
{
    XFiber* const prev = priv->m_cur_task;
    XFiber* next = NULL;

    XTicks now = x_ticks_now();
    XTicks step = now - priv->m_timepoint;
    priv->m_timepoint = now;

    X_FIBER_ENTER_CRITICAL();
    {
        if (prev && (prev->m_state == X_FIBER_STATE_RUNNING))
            X__PushToReadyQueue(prev);

        xvtimer_schedule(&priv->m_vtimer, step);

        while (xilist_empty(&priv->m_ready_queue))
        {
            X_FIBER_EXIT_CRITICAL();

            const int ret = priv->m_idlehook();
            if (ret != 0)
                X__EndSchedule();

            now = x_ticks_now();
            step = now - priv->m_timepoint;
            priv->m_timepoint = now;

            X_FIBER_ENTER_CRITICAL();
            xvtimer_schedule(&priv->m_vtimer, step);
        }

        next = X__PopFromReadyQueue();
        next->m_state = X_FIBER_STATE_RUNNING;
        priv->m_cur_task = next;
    }
    X_FIBER_EXIT_CRITICAL();

    if (prev)
        X__SwapContext(prev, next);
    else
        X__SetContext(next);
}


static bool X__TestEvent(XFiberEvent* event, XMode mode, XBits wait_pattern, XBits* result)
{
    bool ok = false;
    const XBits pattern = event->m_pattern;

    if ((mode & X_FIBER_EVENT_WAIT_MASK) == X_FIBER_EVENT_WAIT_OR)
        ok = (pattern & wait_pattern) != 0;
    else
        ok = (pattern & wait_pattern) == wait_pattern;

    if (ok)
    {
        *result = pattern;
        if (mode & X_FIBER_EVENT_CLEAR_ON_EXIT)
            event->m_pattern = 0;
    }

    return ok;
}


static void* X__Malloc(size_t size)
{
    return xpalloc_allocate(&priv->m_alloc, size);
}


static void X__Free(void* ptr)
{
    xpalloc_deallocate(&priv->m_alloc, ptr);
}


static void X__FiberMain(XFiber* fiber)
{
    X__LOG((X__TAG, "start '%s' %p", fiber->m_name, fiber));

    fiber->m_func(fiber->m_arg);

    X__LOG((X__TAG, "end '%s' %p", fiber->m_name, fiber));

    X__Free(fiber);
    priv->m_cur_task = NULL;
    X__Schedule();
}


#if X_CONF_FIBER_IMPL_TYPE == X_FIBER_IMPL_TYPE_COPY_STACK

static void X__StartSchedule(void)
{
    X__GetStackPtr(&priv->m_machine_stack_begin);

    setjmp(priv->m_return_ctx.m_jmpbuf);
    if (priv->m_cur_task)
        X__FiberMain(priv->m_cur_task);
}


static void X__EndSchedule()
{
    priv->m_cur_task = NULL;
    longjmp(priv->m_return_ctx.m_jmpbuf, 1);
}


static void X__MakeContext(XFiber* fiber, XFiberFunc func, void* arg, void* stack, size_t stack_size)
{
    X_UNUSED(func);
    X_UNUSED(arg);
    X_UNUSED(stack);
    X_UNUSED(stack_size);
    fiber->m_context.m_machine_stack_end = NULL;
}


static void X__SetContext(XFiber* to)
{
    uint8_t* machine_stack_end;
    X__GetStackPtr(&machine_stack_end);
    X__RestoreStack(to, machine_stack_end);
}


static void X__SwapContext(XFiber* from, XFiber* to)
{
    X__GetStackPtr(&(from->m_context.m_machine_stack_end));
    X__SaveStack(from, from->m_context.m_machine_stack_end);

    if (setjmp(from->m_context.m_jmpbuf) == 0)
    {
        X__RestoreStack(to, to->m_context.m_machine_stack_end);
        longjmp(to->m_context.m_jmpbuf, 1);
    }
}


static void X__GetStackPtr(uint8_t** volatile dst)
{
    volatile uint8_t stack_end;
    *dst = (uint8_t*)&stack_end;
}


static void X__SaveStack(XFiber* fiber, const uint8_t* stack_end)
{
    bool stackoverflow = false;
    size_t size;

    if (priv->m_machine_stack_begin > stack_end)
    {
        size = priv->m_machine_stack_begin - stack_end;
        if (size > fiber->m_stack_size)
            stackoverflow = true;
        else
        {
            memcpy(fiber->m_stack + fiber->m_stack_size - size,
                   stack_end,
                   size);
        }
    }
    else
    {
        size = stack_end - priv->m_machine_stack_begin;
        if (size > fiber->m_stack_size)
            stackoverflow = true;
        else
            memcpy(fiber->m_stack, priv->m_machine_stack_begin, size);
    }

    if (stackoverflow)
    {
        X_LOG_HEXDUMP_ERR((X__TAG, stack_end, size, 16,
                          "'%s' %p stack overflow",
                          fiber->m_name, fiber));
        X_ABORT(NULL);
        X_UNUSED(stackoverflow);
    }
}


static void X__RestoreStack(XFiber* fiber, uint8_t* addr_in_prev_frame)
{
    uint8_t padding[64];

    /* 初回の呼び出し時は、X__StartSchedule()まで戻って、X__FiberMain()で
     * ファイバーのメイン関数を呼び出す。
     */
    if (fiber->m_context.m_machine_stack_end == NULL)
    {
        longjmp(priv->m_return_ctx.m_jmpbuf, 1);
    }

    /* スタックはアドレスの大きい方から小さい方へ伸長する(下方伸長)か? */
    if (addr_in_prev_frame > &padding[0])
    {
        if (addr_in_prev_frame > fiber->m_context.m_machine_stack_end)
        {
            X__RestoreStack(fiber, &padding[0]);
        }
        memcpy(fiber->m_context.m_machine_stack_end,
               (fiber->m_stack + fiber->m_stack_size) -
               (priv->m_machine_stack_begin - fiber->m_context.m_machine_stack_end),
               (priv->m_machine_stack_begin - fiber->m_context.m_machine_stack_end));
    }
    else
    {
        if (addr_in_prev_frame < (
                priv->m_machine_stack_begin + (
                fiber->m_context.m_machine_stack_end - priv->m_machine_stack_begin)))
        {
            X__RestoreStack(fiber, &padding[sizeof(padding) - 1]);
        }

        memcpy(priv->m_machine_stack_begin,
               fiber->m_stack,
               fiber->m_context.m_machine_stack_end- priv->m_machine_stack_begin);
    }

    /* 前回の実行位置に戻る */
    longjmp(fiber->m_context.m_jmpbuf, 1);
}


static void* X__ResolvePtr(const XFiber* fiber, const void* ptr)
{
    uint8_t* stack_cur;
    bool is_stack;
    const void* ret = ptr;

    X__GetStackPtr(&stack_cur);
    if (stack_cur < priv->m_machine_stack_begin)
    {
        is_stack = x_is_within_ptr(ptr,
                                   priv->m_machine_stack_begin - fiber->m_stack_size,
                                   priv->m_machine_stack_begin);
        if (is_stack)
        {
            ptrdiff_t offset = priv->m_machine_stack_begin - (const uint8_t*)ptr;
            ret = fiber->m_stack + fiber->m_stack_size - offset;
        }
    }
    else
    {
        /* TODO */
        X_ASSERT(0);
    }
    return (void*)ret;
}


#elif X_CONF_FIBER_IMPL_TYPE == X_FIBER_IMPL_TYPE_UCONTEXT


static void X__StartSchedule(void)
{
    static bool first = true;
    getcontext(&priv->m_return_ctx);
    if (first)
    {
        first = false;
        setcontext(&priv->m_cur_task->m_context);
    }
    first = true;
}


static void X__MakeContext(XFiber* fiber, XFiberFunc func, void* arg, void* stack, size_t stack_size)
{
    X_UNUSED(func);
    X_UNUSED(arg);

    getcontext(&fiber->m_context);
    fiber->m_context.uc_link = NULL;
    fiber->m_context.uc_stack.ss_sp = stack;
    fiber->m_context.uc_stack.ss_size = stack_size;

#if (X_SIZEOF_INTPTR > X_SIZEOF_INT)
    makecontext(&fiber->m_context, (void(*)(void))X__FiberMain, 2,
                (int)(uintptr_t)fiber,
                (int)((uintptr_t)fiber >> (8 * X_SIZEOF_INT)));
#else
    makecontext(&fiber->m_context, X__FiberMain, 1, (int)fiber);
#endif
}


static void X__SwapContext(XFiber* from, XFiber* to)
{
    swapcontext(&from->m_context, &to->m_context);
}


static void X__SetContext(XFiber* to)
{
    setcontext(&to->m_context);
}


static void X__EndSchedule()
{
    setcontext(&priv->m_return_ctx);
}


static void* X__ResolvePtr(const XFiber* fiber, const void* ptr)
{
    X_UNUSED(fiber);
    return ptr;
}


#elif X_CONF_FIBER_IMPL_TYPE == X_FIBER_IMPL_TYPE_PLATFORM_DEPEND
    #error not implemented yet
#else
    #error invalid configuration
#endif /* if X_CONF_FIBER_IMPL_TYPE */
