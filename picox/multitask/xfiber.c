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
#include <picox/allocator/xpico_allocator.h>


#define X_FIBER_IMPL_TYPE_COPY_STACK        (0)
#define X_FIBER_IMPL_TYPE_UCONTEXT          (1)
#define X_FIBER_IMPL_TYPE_PLATFORM_DEPEND   (2)

// #define X_CONF_FIBER_IMPL_TYPE          X_FIBER_IMPL_TYPE_COPY_STACK
#define X_CONF_FIBER_IMPL_TYPE          X_FIBER_IMPL_TYPE_UCONTEXT

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
} XFiberState;


#define X_FIBER_EVENT_WAIT_MASK             (X_FIBER_EVENT_WAIT_OR | X_FIBER_EVENT_WAIT_AND)
#define X_FIBER_STATE_IS_WAITING(state)     ((state) >= X_FIBER_STATE_WAITING_EVENT)


struct XFiberObject;
typedef struct XFiberObject* XFiberObject;

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
    XBits               m_wait_event_pattern;
    XMode               m_wait_event_mode;
    XFiberContext       m_context;
    XBits               m_result_event_patten;
    XError              m_result_waiting;
    XTick               m_waiting_time;
};


struct XFiberEvent
{
    X_DECLAER_FIBER_OBJECT_MEMBERS;
    XIntrusiveList      m_queue;
    XBits               m_pattern;
};


typedef struct X__Kernel
{
    XFiber*             m_cur_task;
    XIntrusiveList      m_ready_queue;
    XPicoAllocator      m_alloc;
    XFiberIdleHook      m_idlehook;
    XFiberContext       m_return_ctx;

#if X_CONF_FIBER_IMPL_TYPE == X_FIBER_IMPL_TYPE_COPY_STACK
    uint8_t*            m_machine_stack_begin;
#endif
} X__Kernel;


static void X__PushReadyQueue(XFiber* fiber);
static XFiber* X__PopReadyQueue(void);
static void X__Schedule(void);
static void* X__Malloc(size_t size);
static void X__Free(void* ptr);
static void X__FiberMain(XFiber* fiber);
static bool X__TestEvent(XFiberEvent* event, XMode mode, XBits wait_pattern, XBits* result);

static void X__StartSchedule(void);
static void X__EndSchedule();
static void X__MakeContext(XFiber* fiber, XFiberFunc func, void* arg, void* stack, size_t stack_size);
static void X__SwapContext(XFiber* from, XFiber* to);
static void X__SetContext(XFiber* to);


#if X_CONF_FIBER_IMPL_TYPE == X_FIBER_IMPL_TYPE_COPY_STACK
static void X__RestoreStack(XFiber* fiber, uint8_t* addr_in_prev_frame);
static void X__GetStackEnd(uint8_t** volatile dst);
static void X__SaveStack(XFiber* fiber, const uint8_t* stack_end);
#endif


#define X__LOG      X_LOG_NOTI
static const char* const X__TAG = "XFiber";


X__Kernel        x_g_fiber_kernel;
#define priv    (&x_g_fiber_kernel)


XError xfiber_kernel_init(void* heap, size_t heapsize, XFiberIdleHook idlehook)
{
    xilist_init(&priv->m_ready_queue);
    xpalloc_init(&priv->m_alloc, heap, heapsize, X_ALIGN_OF(XMaxAlign));
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


XError xfiber_create(XFiber** o_fiber, const char* name, size_t stack_size,
                     XFiberFunc func, void* arg, int priority)
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

    X__MakeContext(fiber, func, arg, stack, stack_size);
    X__PushReadyQueue(fiber);

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

    XFiber* fiber = X__PopReadyQueue();
    fiber->m_state = X_FIBER_STATE_RUNNING;
    priv->m_cur_task = fiber;
    X__StartSchedule();

    X__LOG((X__TAG, "end schedule"));

    return X_ERR_NONE;
}


const char* xfiber_name(const XFiber* fiber)
{
    return fiber->m_name;
}


XError xfiber_event_create(XFiberEvent** o_event, const char* name)
{
    XError err = X_ERR_NONE;
    XFiberEvent* event = X__Malloc(sizeof(*event));
    if (!event)
    {
        err = X_ERR_NONE;
        goto x__exit;
    }

    strcpy(event->m_name, name);
    event->m_pattern = 0;
    xilist_init(&event->m_queue);
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
    volatile XError err = X_ERR_NONE;

    if (X__TestEvent(event, mode, wait_pattern, result))
        goto x__exit;

    XFiber* const volatile fiber = xfiber_self();
    fiber->m_wait_event_pattern = wait_pattern;
    fiber->m_wait_event_mode = mode;
    fiber->m_state = X_FIBER_STATE_WAITING_EVENT;
    xilist_push_front(&event->m_queue, &fiber->m_node);
    X__Schedule();

    *result = fiber->m_result_event_patten;
    err = fiber->m_result_waiting;

x__exit:
    return err;
}



XError xfiber_event_set(XFiberEvent* event, XBits pattern)
{
    XError err = X_ERR_NONE;
    bool scheduling_request = false;

    event->m_pattern |= pattern;
    XIntrusiveNode* ite = xilist_front(&event->m_queue);
    XIntrusiveNode* const end = xilist_end(&event->m_queue);

    while (ite != end)
    {
        XFiber* const fiber = xnode_entry(ite, XFiber, m_node);
        XIntrusiveNode* const next = ite->next;

        if (X__TestEvent(event,
                         fiber->m_wait_event_mode,
                         fiber->m_wait_event_pattern,
                         &fiber->m_result_event_patten))
        {
            xnode_unlink(&fiber->m_node);
            X__PushReadyQueue(fiber);
            scheduling_request = true;
            if (fiber->m_wait_event_mode & X_FIBER_EVENT_CLEAR_ON_EXIT)
                break;
        }
        ite = next;
    }

    if (scheduling_request)
        X__Schedule();

    return err;
}


XError xfiber_event_clear(XFiberEvent* event, XBits pattern)
{
    event->m_pattern &= ~pattern;
    return X_ERR_NONE;
}


static void X__PushReadyQueue(XFiber* fiber)
{
    /* priority */
    fiber->m_state = X_FIBER_STATE_READY;
    xilist_push_back(&priv->m_ready_queue, &fiber->m_node);
}


static XFiber* X__PopReadyQueue(void)
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
    if (prev && (prev->m_state == X_FIBER_STATE_RUNNING))
        X__PushReadyQueue(prev);

    while (xilist_empty(&priv->m_ready_queue))
    {
        const int ret = priv->m_idlehook();
        if (ret != 0)
            X__EndSchedule();
    }

    XFiber* const next = X__PopReadyQueue();
    next->m_state = X_FIBER_STATE_RUNNING;
    priv->m_cur_task = next;

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
    X__GetStackEnd(&priv->m_machine_stack_begin);

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
    X__GetStackEnd(&machine_stack_end);
    X__RestoreStack(to, machine_stack_end);
}


static void X__SwapContext(XFiber* from, XFiber* to)
{
    X__GetStackEnd(&(from->m_context.m_machine_stack_end));
    X__SaveStack(from, from->m_context.m_machine_stack_end);

    if (setjmp(from->m_context.m_jmpbuf) == 0)
    {
        X__RestoreStack(to, to->m_context.m_machine_stack_end);
        longjmp(to->m_context.m_jmpbuf, 1);
    }
}


static void X__GetStackEnd(uint8_t** volatile dst)
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
            memcpy(fiber->m_stack, stack_end, size);
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
               fiber->m_stack,
               priv->m_machine_stack_begin - fiber->m_context.m_machine_stack_end);
    }
    else
    {
        if (addr_in_prev_frame <
                priv->m_machine_stack_begin + (
                fiber->m_context.m_machine_stack_end - priv->m_machine_stack_begin))
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


#elif X_CONF_FIBER_IMPL_TYPE == X_FIBER_IMPL_TYPE_PLATFORM_DEPEND
    #error not implemented yet
#else
    #error invalid configuration
#endif /* if X_CONF_FIBER_IMPL_TYPE */
