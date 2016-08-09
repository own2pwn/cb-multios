/*
 * Copyright (c) 2016 Kaprica Security, Inc.
 *
 * Permission is hereby granted, cgc_free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */
#include "deque.h"
#include <cstdlib.h>
#include <cstring.h>
#include <cstdio.h>

cgc_Deque::cgc_Deque()
{
    idx_0_ = 0;
    size_ = 0;
    cap_ = 8;
    tickets_ = new cgc_Ticket*[cap_];
}

cgc_Deque::~cgc_Deque()
{
    for (int i = 0; i < size_; i++)
    {
        int idx = (idx_0_ + i) % cap_;
        cgc_Ticket::cgc_DeleteTicket(tickets_[idx]);
    }
    delete tickets_;
}

cgc_Ticket* cgc_Deque::cgc_Find(cgc_uint32_t ID)
{
  for (cgc_size_t i = 0; i < cap_; i++)
  {
    if (tickets_[i] && tickets_[i]->cgc_id() == ID)
    {
      return tickets_[i];
    }
  }

  return nullptr;
}

bool cgc_Deque::cgc_Remove(cgc_Ticket* ticket)
{
  return false;
}

void cgc_Deque::cgc_Append(cgc_Ticket *ticket)
{
    if (!ticket)
        return;

    if (size_ == cap_)
        cgc_Expand();

    cgc_uint32_t idx_f = (idx_0_ + size_) % cap_;
    tickets_[idx_f] = ticket;
    ++size_;
}

void cgc_Deque::cgc_AppendLeft(cgc_Ticket *ticket)
{
    if (!ticket)
        return;

    if (size_ == cap_)
        cgc_Expand();

    idx_0_ = (idx_0_ + (cap_ - 1)) % cap_;
    tickets_[idx_0_] = ticket;
    ++size_;
}

cgc_Ticket *cgc_Deque::cgc_Pop()
{
    if (!size_)
        return nullptr;

    cgc_uint32_t idx_f = (idx_0_ + (size_ - 1)) % cap_;
    cgc_Ticket *ticket = tickets_[idx_f];
    --size_;
    cgc_TryShrink();
    return ticket;
}

cgc_Ticket *cgc_Deque::cgc_PopLeft()
{
    if (!size_)
      return nullptr;
    cgc_Ticket *ticket = tickets_[idx_0_];
    idx_0_ = (idx_0_ + 1) % cap_;
    --size_;
    cgc_TryShrink();
    return ticket;
}

cgc_uint32_t cgc_Deque::cgc_Count()
{
    return size_;
}

void cgc_Deque::cgc_Expand()
{
    cgc_uint32_t new_cap = cap_ << 1;
    cgc_Ticket **new_array = new cgc_Ticket*[new_cap];

    if (idx_0_ + size_ > cap_)
    {
        cgc_memcpy(new_array, &tickets_[idx_0_], sizeof(cgc_Ticket *) * (cap_ - idx_0_));
        cgc_memcpy(&new_array[cap_ - idx_0_], tickets_, sizeof(cgc_Ticket *) * (size_ - (cap_ - idx_0_)));
    }
    else
    {
        cgc_memcpy(new_array, &tickets_[idx_0_], sizeof(cgc_Ticket *) * size_);
    }
    delete tickets_;
    tickets_ = new_array;
    idx_0_ = 0;
    cap_ = new_cap;
}

void cgc_Deque::cgc_TryShrink()
{
    if (size_ >= (cap_ >> 1) || cap_ <= 8)
        return;

    cgc_uint32_t new_cap = cap_ >> 1;
    cgc_Ticket **new_array = new cgc_Ticket*[new_cap];

    if (idx_0_ + size_ > cap_)
    {
        cgc_memcpy(new_array, &tickets_[idx_0_], sizeof(cgc_Ticket *) * (cap_ - idx_0_));
        cgc_memcpy(&new_array[cap_ - idx_0_], tickets_, sizeof(cgc_Ticket *) * (size_ - (cap_ - idx_0_)));
    }
    else
    {
        cgc_memcpy(new_array, &tickets_[idx_0_], sizeof(cgc_Ticket *) * size_);
    }

    delete tickets_;
    tickets_ = new_array;
    idx_0_ = 0;
    cap_ = new_cap;
}
