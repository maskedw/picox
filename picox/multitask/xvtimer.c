/**
 *       @file  xvtimer.h
 *      @brief
 *
 *    @details
 *
 *     @author  MaskedW
 *
 *   @internal
 *     Created  2016/05/14
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


#include <picox/multitask/xvtimer.h>


static void X__RemoveRequest(XVTimerRequest* request);
static void X__RemoveAll(XIntrusiveList* requests);


void xvtimer_init(XVTimer* self)
{
    xilist_init(&self->m_requests);
    xilist_init(&self->m_pending_requests);
    self->m_tick_count = 0;
    self->m_in_scheduling = false;
}


void xvtimer_init_request(XVTimerRequest* request)
{
    request->callback = NULL;
    request->arg = NULL;
    request->delay = 0;
    request->interval = 0;
    request->m_node.next = NULL;
    request->m_node.prev = NULL;
    request->m_count = 0;
    request->m_holder = NULL;
    request->m_pending = false;
    request->m_is_delayed = false;
}


void xvtimer_deinit(XVTimer* self)
{
    if (!self)
        return;

    self->m_tick_count = 0;
    self->m_in_scheduling = false;
    X__RemoveAll(&self->m_requests);
    X__RemoveAll(&self->m_pending_requests);
}


void xvtimer_add_request(XVTimer* self,
                         XVTimerRequest* request,
                         XVTimerCallBack callback,
                         void* arg,
                         XTicks delay,
                         XTicks interval,
                         bool once,
                         XDeleter deleter)
{
    X_ASSERT(self);
    X_ASSERT(request);
    X_ASSERT(callback);
    X_ASSERT(!(request->m_holder) || (request->m_holder == self));

    request->callback = callback;
    request->arg = arg;
    request->delay = delay;
    request->interval = interval;
    request->deleter = (!deleter) ? x_null_deleter : deleter;
    request->m_count = 0;
    request->m_has_marked_for_deletion = false;
    request->m_pending = (self->m_in_scheduling);
    request->m_is_delayed = (request->delay != 0);
    request->m_once = once;

    if (!request->m_holder)
    {
        request->m_holder = self;
        xilist_push_back(&self->m_requests, &request->m_node);
    }
}


void xvtimer_remove_requst(XVTimer* self, XVTimerRequest* request)
{
    X_ASSERT(self);
    if (!request)
        return;
    if (!request->m_holder)
        return;

    X_ASSERT(request->m_holder == self);
    if (self->m_in_scheduling)
        request->m_has_marked_for_deletion = true;
    else
        X__RemoveRequest(request);
}


void xvtimer_schedule(XVTimer* self, XTicks step)
{
    if (step == 0)
        return;

    self->m_tick_count += step;
    self->m_in_scheduling = true;

    XIntrusiveNode* ite = xilist_front(&self->m_requests);
    XIntrusiveNode* const end = xilist_end(&self->m_requests);
    while (ite != end)
    {
        XVTimerRequest* const req = xnode_entry(ite, XVTimerRequest, m_node);
        XIntrusiveNode* next = ite->next;

        if (req->m_has_marked_for_deletion)
        {
            X__RemoveRequest(req);
            goto x__next;
        }

        if (req->m_pending)
            goto x__next;

        if (req->m_is_delayed)
        {
            req->m_count += step;
            if (req->m_count >= req->delay)
            {
                req->m_count = 0;
                req->m_is_delayed = false;
            }
        }
        else
        {
            req->m_count += step;
            if (req->m_count >= req->interval)
            {
                req->m_count = 0;
                if (req->m_once)
                    X__RemoveRequest(req);
                req->callback(req->arg);
            }
        }

x__next:
        ite = next;
    }

    ite = xilist_front(&self->m_requests);
    while (ite != end)
    {
        XVTimerRequest* const req = xnode_entry(ite, XVTimerRequest, m_node);
        XIntrusiveNode* next = ite->next;

        if (req->m_has_marked_for_deletion)
            X__RemoveRequest(req);
        else
            req->m_pending = false;
        ite = next;
    }

    self->m_in_scheduling = false;
}


XTicks xvtimer_now(const XVTimer* self)
{
    return self->m_tick_count;
}


static void X__RemoveRequest(XVTimerRequest* request)
{
    request->m_holder = NULL;
    request->m_has_marked_for_deletion = false;
    request->m_pending = false;
    xnode_unlink(&request->m_node);
    request->deleter(request);
}


static void X__RemoveAll(XIntrusiveList* requests)
{
    XIntrusiveNode* ite = xilist_front(requests);
    XIntrusiveNode* const end = xilist_end(requests);

    while (ite != end)
    {
        XVTimerRequest* const req = xnode_entry(ite, XVTimerRequest, m_node);
        XIntrusiveNode* next = ite->next;
        req->deleter(req);
        ite = next;
    }
    xilist_init(requests);
}
