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
#include <picox/container/xmessage_buffer.h>
#include <picox/allocator/xpico_allocator.h>
#include <picox/allocator/xfixed_allocator.h>
#include <picox/multitask/xvtimer.h>


#define X__NODE_TO_FIBER(node)     xnode_entry(node, XFiber, m_node)
#define X__CHECK_POLL(timeout)       \
    do                                  \
    {                                   \
        if ((timeout) == 0)             \
        {                               \
            err = X_ERR_TIMED_OUT;      \
            X_FIBER_EXIT_CRITICAL();    \
            goto x__exit;               \
        }                               \
    } while (0)


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
    X_FIBER_STATE_WAITING_SEND_CHANNEL,
    X_FIBER_STATE_WAITING_RECV_CHANNEL,
    X_FIBER_STATE_WAITING_MUTEX,
    X_FIBER_STATE_WAITING_SEMAPHORE,
    X_FIBER_STATE_WAITING_RECV_MAILBOX,
    X_FIBER_STATE_WAITING_POOL,
    X_FIBER_STATE_SUSPEND = (1 << 8),
    X_FIBER_STATE_SUSPEND_AND_WAITING_EVENT        = X_FIBER_STATE_SUSPEND | X_FIBER_STATE_WAITING_EVENT,
    X_FIBER_STATE_SUSPEND_AND_WAITING_DELAY        = X_FIBER_STATE_SUSPEND | X_FIBER_STATE_WAITING_DELAY,
    X_FIBER_STATE_SUSPEND_AND_WAITING_SIGNAL       = X_FIBER_STATE_SUSPEND | X_FIBER_STATE_WAITING_SIGNAL,
    X_FIBER_STATE_SUSPEND_AND_WAITING_SEND_QUEUE   = X_FIBER_STATE_SUSPEND | X_FIBER_STATE_WAITING_SEND_QUEUE,
    X_FIBER_STATE_SUSPEND_AND_WAITING_RECV_QUEUE   = X_FIBER_STATE_SUSPEND | X_FIBER_STATE_WAITING_RECV_QUEUE,
    X_FIBER_STATE_SUSPEND_AND_WAITING_SEND_CHANNEL = X_FIBER_STATE_SUSPEND | X_FIBER_STATE_WAITING_SEND_CHANNEL,
    X_FIBER_STATE_SUSPEND_AND_WAITING_RECV_CHANNEL = X_FIBER_STATE_SUSPEND | X_FIBER_STATE_WAITING_RECV_CHANNEL,
    X_FIBER_STATE_SUSPEND_AND_WAITING_MUTEX        = X_FIBER_STATE_SUSPEND | X_FIBER_STATE_WAITING_MUTEX,
    X_FIBER_STATE_SUSPEND_AND_WAITING_SEMAPHORE    = X_FIBER_STATE_SUSPEND | X_FIBER_STATE_WAITING_SEMAPHORE,
    X_FIBER_STATE_SUSPEND_AND_WAITING_RECV_MAILBOX = X_FIBER_STATE_SUSPEND | X_FIBER_STATE_WAITING_RECV_MAILBOX,
    X_FIBER_STATE_SUSPEND_AND_WAITING_POOL         = X_FIBER_STATE_SUSPEND | X_FIBER_STATE_WAITING_POOL,
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
    X_FIBER_OBJTYPE_CHANNEL,
    X_FIBER_OBJTYPE_MUTEX,
    X_FIBER_OBJTYPE_SEMAPHORE,
    X_FIBER_OBJTYPE_MAILBOX,
    X_FIBER_OBJTYPE_POOL,
    X_FIBER_OBJTYPE_END,
} XFiberObjectType;


#define X_DECLAER_FIBER_OBJECT_COMMON_MEMBERS \
    XIntrusiveNode      m_node;         \
    char                m_name[16];     \
    XFiberObjectType    m_type


#define X_DECLAER_FIBER_WAIT_OBJECT_COMMON_MEMBERS \
    X_DECLAER_FIBER_OBJECT_COMMON_MEMBERS;         \
    XIntrusiveList      m_pending_tasks


struct XFiber
{
/** @privatesection */
    X_DECLAER_FIBER_OBJECT_COMMON_MEMBERS;
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
    size_t              m_channel_item_size;
};


struct XFiberWaitObject
{
    X_DECLAER_FIBER_OBJECT_COMMON_MEMBERS;
    XIntrusiveList      m_pending_tasks;
};


struct XFiberEvent
{
    X_DECLAER_FIBER_WAIT_OBJECT_COMMON_MEMBERS;
    XBits               m_pattern;
};


struct XFiberQueue
{
    X_DECLAER_FIBER_WAIT_OBJECT_COMMON_MEMBERS;
    XCircularBuffer     m_buffer;
    size_t              m_item_size;
};


struct XFiberChannel
{
    X_DECLAER_FIBER_WAIT_OBJECT_COMMON_MEMBERS;
    XMessageBuffer      m_buffer;
    size_t              m_max_item_size;
};


struct XFiberMutex
{
    X_DECLAER_FIBER_WAIT_OBJECT_COMMON_MEMBERS;
    XFiber*             m_holder;
};


struct XFiberSemaphore
{
    X_DECLAER_FIBER_WAIT_OBJECT_COMMON_MEMBERS;
    int                 m_count;
};


struct XFiberMailbox
{
    X_DECLAER_FIBER_WAIT_OBJECT_COMMON_MEMBERS;
    XIntrusiveList      m_messages;
};


struct XFiberPool
{
    X_DECLAER_FIBER_WAIT_OBJECT_COMMON_MEMBERS;
    XFixedAllocator     m_allocator;
};


typedef struct X__Kernel
{
    XFiber*             m_cur_task;
    uint_fast8_t        m_priority_map;
    XIntrusiveList      m_ready_queue[X_FIBER_PRIORITY_MAX];
    XIntrusiveList      m_delay_queue;
    XPicoAllocator      m_alloc;
    XFiberIdleHook      m_idlehook;
    XFiberContext       m_return_ctx;
    XTicks              m_timepoint;
    XVTimer             m_vtimer;
    int                 m_num_objects[X_FIBER_OBJTYPE_END];
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
static void X__TransitionIntoWaitState(XIntrusiveList* list, XFiber* fiber, XFiberState state, XTicks timeout);
static void X__StartSchedule(void);
static void X__EndSchedule();
static void X__MakeContext(XFiber* fiber, XFiberFunc func, void* arg, void* stack, size_t stack_size);
static void X__SwapContext(XFiber* from, XFiber* to);
static void X__SetContext(XFiber* to);
static void* X__ResolvePtr(const XFiber* fiber, const void* ptr);
static void X__DestroyFiber(XFiber* fiber);
static void X__PargePendingTasks(XIntrusiveList* list);

#if X_CONF_FIBER_IMPL_TYPE == X_FIBER_IMPL_TYPE_COPY_STACK
static void X__GetStackPtr(uint8_t** volatile dst);
static void X__RestoreStack(XFiber* fiber, uint8_t* addr_in_prev_frame);
static void X__SaveStack(XFiber* fiber, const uint8_t* stack_end);
#endif


// #define X__DEBUG_FIBER
#ifdef X__DEBUG_FIBER
    #define X__LOG          X_LOG_NOTI
    #define X__HEXDUMP      X_LOG_HEXDUMP_NOTI
    static const char* const X__TAG = "XFiber";
#else
    #define X__LOG(x)       (void)0
    #define X__HEXDUMP(x)   (void)0
    #define X__TAG          0
#endif


X__Kernel        x_g_fiber_kernel;
#define priv    (&x_g_fiber_kernel)


XError xfiber_kernel_init(void* heap, size_t heapsize, XFiberIdleHook idlehook)
{
    int i;

    for (i = 0; i < X_FIBER_PRIORITY_MAX; ++i)
        xilist_init(&priv->m_ready_queue[i]);

    xilist_init(&priv->m_delay_queue);
    xpalloc_init(&priv->m_alloc, heap, heapsize, X_ALIGN_OF(XMaxAlign));
    xvtimer_init(&priv->m_vtimer);
    priv->m_idlehook = idlehook;
    priv->m_priority_map = 0;
    memset(priv->m_num_objects, 0, sizeof(priv->m_num_objects));

    return X_ERR_NONE;
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


void xfiber_kernel_end_scheduler(void)
{
    X__EndSchedule();
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
    uint8_t* stack;
    XFiber* fiber = X__Malloc(x_roundup_multiple(
                sizeof(XFiber), X_ALIGN_OF(XMaxAlign)) + stack_size);

    if (!fiber)
    {
        err = X_ERR_NO_MEMORY;
        goto x__exit;
    }

    stack = ((uint8_t*)fiber) + x_roundup_multiple(
            sizeof(XFiber), X_ALIGN_OF(XMaxAlign));

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
        priv->m_num_objects[X_FIBER_OBJTYPE_TASK]++;
    }
    X_FIBER_EXIT_CRITICAL();

    X_ASSIGN_NOT_NULL(o_fiber, fiber);
    fiber = NULL;
    stack = NULL;

x__exit:
    X__Free(fiber);

    return err;
}


const char* xfiber_name(const XFiber* fiber)
{
    if (!fiber)
        fiber = priv->m_cur_task;
    return fiber->m_name;
}


XError xfiber_event_create(XFiberEvent** o_event)
{
    XError err = X_ERR_NONE;
    XFiberEvent* event = X__Malloc(sizeof(*event));
    if (!event)
    {
        err = X_ERR_NO_MEMORY;
        goto x__exit;
    }

    event->m_pattern = 0;
    xilist_init(&event->m_pending_tasks);
    event->m_type = X_FIBER_OBJTYPE_EVENT;
    *o_event = event;

x__exit:

    return err;
}


void xfiber_event_destroy(XFiberEvent* event)
{
    X_FIBER_ENTER_CRITICAL();
    {
        X__PargePendingTasks(&event->m_pending_tasks);
        X__Free(event);
    }
    X_FIBER_EXIT_CRITICAL();
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
    XError err = X_ERR_NONE;
    XFiber* const fiber = xfiber_self();

    X_ASSIGN_NOT_NULL(result, 0);
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
    err = fiber->m_result_waiting;
    X_ASSIGN_NOT_NULL(result, fiber->m_result_event_patten);

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


XBits xfiber_event_clear(XFiberEvent* event, XBits pattern)
{
    XBits prev;
    X_FIBER_ENTER_CRITICAL();
    {
        prev = event->m_pattern;
        event->m_pattern &= ~pattern;
    }
    X_FIBER_EXIT_CRITICAL();

    return prev;
}


XBits xfiber_event_clear_isr(XFiberEvent* event, XBits pattern)
{
    const XBits prev = event->m_pattern;
    event->m_pattern &= ~pattern;
    return prev;
}


XBits xfiber_event_get(XFiberEvent* event)
{
    XBits cur;
    X_FIBER_ENTER_CRITICAL();
    {
        cur = event->m_pattern;
    }
    X_FIBER_EXIT_CRITICAL();

    return cur;
}


XBits xfiber_event_get_isr(XFiberEvent* event)
{
    const XBits cur = event->m_pattern;
    return cur;
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
    XFiber* fiber;

    if (!sigs)
    {
        *result = 0;
        goto x__exit;
    }

    fiber = xfiber_self();
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


XBits xfiber_signal_get(XFiber* fiber)
{
    XBits ret;
    X_FIBER_ENTER_CRITICAL();
    {
        ret = fiber->m_recv_sigs;
    }
    X_FIBER_EXIT_CRITICAL();

    return ret;
}


XBits xfiber_signal_get_isr(XFiber* fiber)
{
    const XBits ret = fiber->m_recv_sigs;
    return ret;
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


void xfiber_queue_destroy(XFiberQueue* queue)
{
    X_FIBER_ENTER_CRITICAL();
    {
        X__PargePendingTasks(&queue->m_pending_tasks);
        X__Free(queue);
    }
    X_FIBER_EXIT_CRITICAL();
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
    XFiber* const cur_task = priv->m_cur_task;

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
        else
        {
            X__CHECK_POLL(timeout);
            scheduling_request = true;
            cur_task->m_pending_send_src = X__ResolvePtr(cur_task, src);
            X__TransitionIntoWaitState(&queue->m_pending_tasks, cur_task,
                                       X_FIBER_STATE_WAITING_SEND_QUEUE, timeout);
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
        else
        {
            X__CHECK_POLL(timeout);
            scheduling_request = true;
            cur_task->m_pending_send_src = X__ResolvePtr(cur_task, src);
            X__TransitionIntoWaitState(&queue->m_pending_tasks, cur_task,
                                       X_FIBER_STATE_WAITING_SEND_QUEUE, timeout);
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
        else
        {
            X__CHECK_POLL(timeout);
            scheduling_request = true;
            cur_task->m_pending_recv_dst = X__ResolvePtr(cur_task, dst);
            X__TransitionIntoWaitState(&queue->m_pending_tasks, cur_task,
                                       X_FIBER_STATE_WAITING_RECV_QUEUE, timeout);
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
            XFiber* const pend_task = X__NODE_TO_FIBER(
                    xilist_pop_front(&queue->m_pending_tasks));

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


XError xfiber_channel_create(XFiberChannel** o_channel, size_t capacity, size_t max_item_size)
{
    if (!o_channel)
        return X_ERR_INVALID;

    XFiberChannel* channel = X__Malloc(sizeof(XFiberChannel) + capacity);
    if (!channel)
        return X_ERR_NO_MEMORY;

    xmsgbuf_init(&channel->m_buffer, (uint8_t*)channel + sizeof(XFiberChannel), capacity);
    xilist_init(&channel->m_pending_tasks);
    channel->m_type = X_FIBER_OBJTYPE_CHANNEL;
    channel->m_max_item_size = max_item_size;
    *o_channel = channel;

    return X_ERR_NONE;
}


void xfiber_channel_destroy(XFiberChannel* channel)
{
    X_FIBER_ENTER_CRITICAL();
    {
        X__PargePendingTasks(&channel->m_pending_tasks);
        X__Free(channel);
    }
    X_FIBER_EXIT_CRITICAL();
}


XError xfiber_channel_send(XFiberChannel* channel, const void* src, size_t size)
{
    return xfiber_channel_timed_send(channel, src, size, X_TICKS_FOREVER);
}


XError xfiber_channel_try_send(XFiberChannel* channel, const void* src, size_t size)
{
    return xfiber_channel_timed_send(channel, src, size, 0);
}


XError xfiber_channel_timed_send(XFiberChannel* channel, const void* src, size_t size, XTicks timeout)
{
    XError err = X_ERR_NONE;
    bool scheduling_request = false;
    XFiber* const cur_task = priv->m_cur_task;

    X_FIBER_ENTER_CRITICAL();
    {
        if (xmsgbuf_empty(&channel->m_buffer) && (!xilist_empty(&channel->m_pending_tasks)))
        {
            XFiber* const pend_task = X__NODE_TO_FIBER(xilist_pop_front(&channel->m_pending_tasks));
            memcpy(pend_task->m_pending_recv_dst, src, size);
            pend_task->m_channel_item_size = size;
            X__ReleaseWaiting(pend_task, X_ERR_NONE);
            scheduling_request = true;
        }
        else if (xmsgbuf_reserve(&channel->m_buffer) >= size + sizeof(XMessageHeader))
        {
            xmsgbuf_push(&channel->m_buffer, src, size);
        }
        else
        {
            X__CHECK_POLL(timeout);
            scheduling_request = true;
            cur_task->m_pending_send_src = X__ResolvePtr(cur_task, src);
            cur_task->m_channel_item_size = size;
            X__TransitionIntoWaitState(&channel->m_pending_tasks, cur_task,
                                       X_FIBER_STATE_WAITING_SEND_CHANNEL, timeout);
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


XError xfiber_channel_send_isr(XFiberChannel* channel, const void* src, size_t size)
{
    XError err = X_ERR_NONE;

    if (xmsgbuf_empty(&channel->m_buffer) && (!xilist_empty(&channel->m_pending_tasks)))
    {
        XFiber* const pend_task = X__NODE_TO_FIBER(xilist_pop_front(&channel->m_pending_tasks));
        memcpy(pend_task->m_pending_recv_dst, src, size);
        pend_task->m_channel_item_size = size;
        X__ReleaseWaiting(pend_task, X_ERR_NONE);
    }
    else if (xmsgbuf_reserve(&channel->m_buffer) >= size + sizeof(XMessageHeader))
    {
        xmsgbuf_push(&channel->m_buffer, src, size);
    }
    else
    {
        err = X_ERR_TIMED_OUT;
    }

    return err;
}



XError xfiber_channel_receive(XFiberChannel* channel, void* dst, size_t* o_size)
{
    return xfiber_channel_timed_receive(channel, dst, o_size, X_TICKS_FOREVER);
}


XError xfiber_channel_try_receive(XFiberChannel* channel, void* dst, size_t* o_size)
{
    return xfiber_channel_timed_receive(channel, dst, o_size, 0);
}


XError xfiber_channel_timed_receive(XFiberChannel* channel, void* dst, size_t* o_size, XTicks timeout)
{
    XError err = X_ERR_NONE;
    bool scheduling_request = false;
    XFiber* const cur_task = priv->m_cur_task;

    X_FIBER_ENTER_CRITICAL();
    {
        if (!xmsgbuf_empty(&channel->m_buffer))
        {
            *o_size = xmsgbuf_pull(&channel->m_buffer, dst);
            if (!xilist_empty(&channel->m_pending_tasks))
            {
                XFiber* const pend_task = X__NODE_TO_FIBER(xilist_front(&channel->m_pending_tasks));
                if (xmsgbuf_reserve(&channel->m_buffer) >=
                                    pend_task->m_channel_item_size + sizeof(XMessageHeader))
                {
                    xilist_pop_front(&channel->m_pending_tasks);
                    xmsgbuf_push(&channel->m_buffer,
                                 pend_task->m_pending_send_src,
                                 pend_task->m_channel_item_size);
                    X__ReleaseWaiting(pend_task, X_ERR_NONE);
                    scheduling_request = true;
                }

            }
        }
        else
        {
            X__CHECK_POLL(timeout);
            scheduling_request = true;
            cur_task->m_pending_recv_dst = X__ResolvePtr(cur_task, dst);
            X__TransitionIntoWaitState(&channel->m_pending_tasks, cur_task,
                                       X_FIBER_STATE_WAITING_RECV_CHANNEL, timeout);
        }
    }
    X_FIBER_EXIT_CRITICAL();

    if (scheduling_request)
    {
        X__Schedule();
        err = cur_task->m_result_waiting;
        *o_size = cur_task->m_channel_item_size;
    }

x__exit:
    return err;
}


XError xfiber_channel_receive_isr(XFiberChannel* channel, void* dst, size_t* o_size)
{
    XError err = X_ERR_NONE;

    if (!xmsgbuf_empty(&channel->m_buffer))
    {
        *o_size = xmsgbuf_pull(&channel->m_buffer, dst);
        if (!xilist_empty(&channel->m_pending_tasks))
        {
            XFiber* const pend_task = X__NODE_TO_FIBER(xilist_front(&channel->m_pending_tasks));
            if (xmsgbuf_reserve(&channel->m_buffer) >=
                                pend_task->m_channel_item_size + sizeof(XMessageHeader))
            {
                xilist_pop_front(&channel->m_pending_tasks);
                xmsgbuf_push(&channel->m_buffer,
                             pend_task->m_pending_send_src,
                             pend_task->m_channel_item_size);
                X__ReleaseWaiting(pend_task, X_ERR_NONE);
            }
        }
    }
    else
    {
        err = X_ERR_TIMED_OUT;
        goto x__exit;
    }

x__exit:
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


void xfiber_mutex_destroy(XFiberMutex* mutex)
{
    X_FIBER_ENTER_CRITICAL();
    {
        X__PargePendingTasks(&mutex->m_pending_tasks);
        X__Free(mutex);
    }
    X_FIBER_EXIT_CRITICAL();
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
        else
        {
            X__CHECK_POLL(timeout);
            scheduling_request = true;
            X__TransitionIntoWaitState(&mutex->m_pending_tasks, cur_task,
                                       X_FIBER_STATE_WAITING_MUTEX, timeout);
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
            err = X_ERR_PROTOCOL;
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
        return X_ERR_PROTOCOL;

    mutex->m_holder = NULL;
    if (xilist_empty(&mutex->m_pending_tasks))
        return X_ERR_NONE;

    XFiber* const pend_task = X__NODE_TO_FIBER(
            xilist_pop_front(&mutex->m_pending_tasks));
    mutex->m_holder = pend_task;
    X__ReleaseWaiting(pend_task, X_ERR_NONE);

    return err;
}


XError xfiber_semaphore_create(XFiberSemaphore** o_semaphore, int initial_count)
{
    if (!o_semaphore)
        return X_ERR_INVALID;

    XFiberSemaphore* semaphore = X__Malloc(sizeof(XFiberSemaphore));
    if (!semaphore)
        return X_ERR_NO_MEMORY;

    xilist_init(&semaphore->m_pending_tasks);
    semaphore->m_type = X_FIBER_OBJTYPE_SEMAPHORE;
    semaphore->m_count = initial_count;
    *o_semaphore = semaphore;

    return X_ERR_NONE;
}


void xfiber_semaphore_destroy(XFiberSemaphore* semaphore)
{
    X_FIBER_ENTER_CRITICAL();
    {
        X__PargePendingTasks(&semaphore->m_pending_tasks);
        X__Free(semaphore);
    }
    X_FIBER_EXIT_CRITICAL();
}


XError xfiber_semaphore_take(XFiberSemaphore* semaphore)
{
    return xfiber_semaphore_timed_take(semaphore, X_TICKS_FOREVER);
}


XError xfiber_semaphore_try_take(XFiberSemaphore* semaphore)
{
    return xfiber_semaphore_timed_take(semaphore, 0);
}


XError xfiber_semaphore_timed_take(XFiberSemaphore* semaphore, XTicks timeout)
{
    XError err = X_ERR_NONE;
    bool scheduling_request = false;
    XFiber* const cur_task = priv->m_cur_task;

    X_FIBER_ENTER_CRITICAL();
    {
        if (semaphore->m_count > 0)
        {
            semaphore->m_count--;
        }
        else
        {
            X__CHECK_POLL(timeout);
            scheduling_request = true;
            X__TransitionIntoWaitState(&semaphore->m_pending_tasks, cur_task,
                                       X_FIBER_STATE_WAITING_SEMAPHORE, timeout);
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


XError xfiber_semaphore_give(XFiberSemaphore* semaphore)
{
    XError err = X_ERR_NONE;
    bool scheduling_request = false;

    X_FIBER_ENTER_CRITICAL();
    {
        if (xilist_empty(&semaphore->m_pending_tasks))
        {
            semaphore->m_count++;
        }
        else
        {
            XFiber* const pend_task = X__NODE_TO_FIBER(
                    xilist_pop_front(&semaphore->m_pending_tasks));
            X__ReleaseWaiting(pend_task, X_ERR_NONE);
            scheduling_request = true;
        }
    }
    X_FIBER_EXIT_CRITICAL();

    if (scheduling_request)
        X__Schedule();

    return err;
}


XError xfiber_semaphore_give_isr(XFiberSemaphore* semaphore)
{
    XError err = X_ERR_NONE;

    if (xilist_empty(&semaphore->m_pending_tasks))
    {
        semaphore->m_count++;
    }
    else
    {
        XFiber* const pend_task = X__NODE_TO_FIBER(
                xilist_pop_front(&semaphore->m_pending_tasks));
        X__ReleaseWaiting(pend_task, X_ERR_NONE);
    }

    return err;
}


XError xfiber_mailbox_create(XFiberMailbox** o_mailbox)
{
    XError err = X_ERR_NONE;
    XFiberMailbox* mailbox = X__Malloc(sizeof(*mailbox));
    if (!mailbox)
        return X_ERR_NO_MEMORY;

    xilist_init(&mailbox->m_pending_tasks);
    xilist_init(&mailbox->m_messages);
    mailbox->m_type = X_FIBER_OBJTYPE_MAILBOX;
    *o_mailbox = mailbox;

    return err;
}


void xfiber_mailbox_destroy(XFiberMailbox* mailbox)
{
    X_FIBER_ENTER_CRITICAL();
    {
        X__PargePendingTasks(&mailbox->m_pending_tasks);
        X__Free(mailbox);
    }
    X_FIBER_EXIT_CRITICAL();
}


XError xfiber_mailbox_send(XFiberMailbox* mailbox, XFiberMessage* message)
{
    XError err = X_ERR_NONE;
    bool scheduling_request = false;

    X_FIBER_ENTER_CRITICAL();
    {
        if (xilist_empty(&mailbox->m_messages) && !xilist_empty(&mailbox->m_pending_tasks))
        {
            XFiber* const pend_task = X__NODE_TO_FIBER(xilist_pop_front(&mailbox->m_pending_tasks));
            *((XFiberMessage**)(pend_task->m_pending_recv_dst)) = message;
            X__ReleaseWaiting(pend_task, X_ERR_NONE);
            scheduling_request = true;
        }
        else
        {
            xilist_push_back(&mailbox->m_messages, message);
        }
    }
    X_FIBER_EXIT_CRITICAL();

    if (scheduling_request)
        X__Schedule();

    return err;
}


XError xfiber_mailbox_send_isr(XFiberMailbox* mailbox, XFiberMessage* message)
{
    XError err = X_ERR_NONE;

    if (xilist_empty(&mailbox->m_messages) && !xilist_empty(&mailbox->m_pending_tasks))
    {
        XFiber* const pend_task = X__NODE_TO_FIBER(xilist_pop_front(&mailbox->m_pending_tasks));
        *((XFiberMessage**)(pend_task->m_pending_recv_dst)) = message;
        X__ReleaseWaiting(pend_task, X_ERR_NONE);
    }
    else
    {
        xilist_push_back(&mailbox->m_messages, message);
    }

    return err;
}


XError xfiber_mailbox_receive(XFiberMailbox* mailbox, XFiberMessage** o_message)
{
    return xfiber_mailbox_timed_receive(mailbox, o_message, X_TICKS_FOREVER);
}


XError xfiber_mailbox_try_receive(XFiberMailbox* mailbox, XFiberMessage** o_message)
{
    return xfiber_mailbox_timed_receive(mailbox, o_message, 0);
}


XError xfiber_mailbox_timed_receive(XFiberMailbox* mailbox, XFiberMessage** o_message, XTicks timeout)
{
    XError err = X_ERR_NONE;
    bool scheduling_request = false;
    XFiber* cur_task = priv->m_cur_task;

    X_FIBER_ENTER_CRITICAL();
    {
        if (!xilist_empty(&mailbox->m_messages))
        {
            *o_message = xilist_pop_front(&mailbox->m_messages);
        }
        else
        {
            X__CHECK_POLL(timeout);
            scheduling_request = true;
            cur_task->m_pending_recv_dst = X__ResolvePtr(cur_task, (void*)o_message);
            X__TransitionIntoWaitState(&mailbox->m_pending_tasks, cur_task,
                                       X_FIBER_STATE_WAITING_RECV_MAILBOX, timeout);
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


XError xfiber_mailbox_receive_isr(XFiberMailbox* mailbox, XFiberMessage** o_message)
{
    if (!xilist_empty(&mailbox->m_messages))
    {
        *o_message = xilist_pop_front(&mailbox->m_messages);
        return X_ERR_NONE;
    }

    return X_ERR_TIMED_OUT;
}


XError xfiber_pool_create(XFiberPool** o_pool, size_t block_size, size_t num_blocks)
{
    XError err = X_ERR_NONE;
    XFiberPool* pool = X__Malloc(x_roundup_multiple(
                sizeof(*pool), X_ALIGN_OF(XMaxAlign)) +
                block_size * num_blocks);
    if (!pool)
        return X_ERR_NO_MEMORY;

    xilist_init(&pool->m_pending_tasks);
    xfalloc_init(&pool->m_allocator,
                 (uint8_t*)pool + x_roundup_multiple(sizeof(*pool), X_ALIGN_OF(XMaxAlign)),
                 block_size * num_blocks,
                 block_size);

    pool->m_type = X_FIBER_OBJTYPE_POOL;
    *o_pool = pool;

    return err;
}


void xfiber_pool_destroy(XFiberPool* pool)
{
    X_FIBER_ENTER_CRITICAL();
    {
        X__PargePendingTasks(&pool->m_pending_tasks);
        X__Free(pool);
    }
    X_FIBER_EXIT_CRITICAL();
}


XError xfiber_pool_get(XFiberPool* pool, void** o_mem)
{
    return xfiber_pool_timed_get(pool, o_mem, X_TICKS_FOREVER);
}


XError xfiber_pool_try_get(XFiberPool* pool, void** o_mem)
{
    return xfiber_pool_timed_get(pool, o_mem, 0);
}


XError xfiber_pool_timed_get(XFiberPool* pool, void** o_mem, XTicks timeout)
{
    XError err = X_ERR_NONE;
    bool scheduling_request = false;
    XFiber* cur_task = priv->m_cur_task;

    X_FIBER_ENTER_CRITICAL();
    {
        if (xfalloc_remain_blocks(&pool->m_allocator) > 0)
        {
            *o_mem = xfalloc_allocate(&pool->m_allocator);
        }
        else
        {
            X__CHECK_POLL(timeout);
            scheduling_request = true;
            cur_task->m_pending_recv_dst = X__ResolvePtr(cur_task, (void*)o_mem);
            X__TransitionIntoWaitState(&pool->m_pending_tasks, cur_task,
                                       X_FIBER_STATE_WAITING_POOL, timeout);
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


XError xfiber_pool_get_isr(XFiberPool* pool, void** o_mem)
{
    if (xfalloc_remain_blocks(&pool->m_allocator) > 0)
    {
        *o_mem = xfalloc_allocate(&pool->m_allocator);
        return X_ERR_NONE;
    }

    return X_ERR_TIMED_OUT;
}


XError xfiber_pool_release(XFiberPool* pool, void* mem)
{
    XError err = X_ERR_NONE;
    bool scheduling_request = false;

    X_FIBER_ENTER_CRITICAL();
    {
        if ((xfalloc_remain_blocks(&pool->m_allocator) == 0) && !xilist_empty(&pool->m_pending_tasks))
        {
            XFiber* const pend_task = X__NODE_TO_FIBER(xilist_pop_front(&pool->m_pending_tasks));
            *((void**)(pend_task->m_pending_recv_dst)) = mem;
            X__ReleaseWaiting(pend_task, X_ERR_NONE);
            scheduling_request = true;
        }
        else
        {
            xfalloc_deallocate(&pool->m_allocator, mem);
        }
    }
    X_FIBER_EXIT_CRITICAL();

    if (scheduling_request)
        X__Schedule();

    return err;
}


XError xfiber_pool_release_isr(XFiberPool* pool, void* mem)
{
    if ((xfalloc_remain_blocks(&pool->m_allocator) == 0) && !xilist_empty(&pool->m_pending_tasks))
    {
        XFiber* const pend_task = X__NODE_TO_FIBER(xilist_pop_front(&pool->m_pending_tasks));
        *((void**)(pend_task->m_pending_recv_dst)) = mem;
        X__ReleaseWaiting(pend_task, X_ERR_NONE);
    }
    else
    {
        xfalloc_deallocate(&pool->m_allocator, mem);
    }

    return X_ERR_NONE;
}



static void X__TransitionIntoWaitState(XIntrusiveList* list, XFiber* fiber, XFiberState state, XTicks timeout)
{
    xilist_push_back(list, &fiber->m_node);
    fiber->m_state = state;
    if (timeout > 0)
        X__AddTimerEvent(fiber, X__TimeoutHandler, timeout);
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
                        true,
                        NULL);
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
    const int priority = fiber->m_priority;
    XIntrusiveList* const ready_queue = &priv->m_ready_queue[priority];

    fiber->m_state = X_FIBER_STATE_READY;
    xilist_push_back(ready_queue, &fiber->m_node);
    priv->m_priority_map |= 1 << priority;

}


static XFiber* X__PopFromReadyQueue(void)
{
    /* Check dead lock */
    X_ASSERT(priv->m_priority_map);

    const int priority = x_find_msb_pos8(priv->m_priority_map);
    XIntrusiveList* const ready_queue = &priv->m_ready_queue[priority];
    XIntrusiveNode* const next = xilist_pop_front(ready_queue);
    XFiber* const fiber = xnode_entry(next, XFiber, m_node);

    if (xilist_empty(ready_queue))
        priv->m_priority_map &= ~(1 << priority);

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

        while (!priv->m_priority_map)
        {
            X_FIBER_EXIT_CRITICAL();

            if (priv->m_idlehook)
            {
                const int ret = priv->m_idlehook();
                if (ret != 0)
                    X__EndSchedule();
            }

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
    {
        X__LOG((X__TAG, "swap context from %s to %s\n", prev->m_name, next->m_name));
        X__SwapContext(prev, next);
    }
    else
    {
        X__LOG((X__TAG, "set context to %s\n", next->m_name));
        X__SetContext(next);
    }
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

    /* @attention
     * なんかこの辺のスタックの復帰処理がGCCのバージョンによって上手くいかない。
     * m_func()の実行後、fierの値が変わっていたりするので、無理やり再代入してお
     * く。
     * かなり危険な実装だが、今のところこの部分以外のスタックは問題ない。
     */
    fiber = priv->m_cur_task;

    X__LOG((X__TAG, "end '%s' %p", fiber->m_name, fiber));

    X__DestroyFiber(fiber);
    priv->m_cur_task = NULL;
    X__Schedule();
}


static void X__DestroyFiber(XFiber* fiber)
{
    X_FIBER_ENTER_CRITICAL();
    xvtimer_remove_requst(&priv->m_vtimer, &fiber->m_timer_request);
    X__Free(fiber);
    priv->m_num_objects[X_FIBER_OBJTYPE_TASK]--;
    X_FIBER_EXIT_CRITICAL();
}


static void X__PargePendingTasks(XIntrusiveList* list)
{
    XIntrusiveNode* ite = xilist_front(list);
    XIntrusiveNode* const end = xilist_end(list);

    while (ite != end)
    {
        XFiber* const fiber = X__NODE_TO_FIBER(ite);
        XIntrusiveNode* const next = ite->next;
        X__ReleaseWaiting(fiber, X_ERR_CANCELED);
        ite = next;
    }
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
        X__RestoreStack(to, from->m_context.m_machine_stack_end);
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
            X__HEXDUMP((
                X__TAG, stack_end, size, 16,
                "%s saved stack %d[Bytes]", fiber->m_name, size));
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
        X__HEXDUMP((X__TAG, stack_end, size, 16,
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
    if (priv->m_machine_stack_begin > fiber->m_context.m_machine_stack_end)
    {
        if (addr_in_prev_frame > fiber->m_context.m_machine_stack_end)
        {
            X__RestoreStack(fiber, &padding[0]);
        }
        memcpy(fiber->m_context.m_machine_stack_end,
               (fiber->m_stack + fiber->m_stack_size) -
               (priv->m_machine_stack_begin - fiber->m_context.m_machine_stack_end),
               (priv->m_machine_stack_begin - fiber->m_context.m_machine_stack_end));

        X__HEXDUMP((
                X__TAG,
                fiber->m_context.m_machine_stack_end,
                priv->m_machine_stack_begin - fiber->m_context.m_machine_stack_end,
                16,
                "%s restore stack %d[Bytes]", fiber->m_name,
                priv->m_machine_stack_begin - fiber->m_context.m_machine_stack_end));
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
