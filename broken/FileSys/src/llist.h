/*
 * Copyright (c) 2015 Kaprica Security, Inc.
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

#ifndef LLIST_H
#define LLIST_H

extern "C" {
  #include <stdlib.h>
  #include <string.h>
};

template <typename T>
class cgc_List
{
  struct cgc_Element
  {
    T obj;
    cgc_Element *next;
    cgc_Element(T o, cgc_Element *n = 0) : obj(o), next(n) {}
  };
  cgc_Element *head;
  unsigned int len;

  public:
    cgc_List(cgc_Element *h = 0) : head(h), len(0) {}
    ~cgc_List();

    void cgc_add(T o);
    void cgc_remove(T o);
    cgc_Element* cgc_find(T o);
    T cgc_get(int idx);
    unsigned int cgc_length() { return len; }
    void cgc_debug();
};

template <typename T>
cgc_List<T>::~cgc_List()
{
  cgc_Element *tmp;
  while (head)
  {
    tmp = head;
    head = tmp->next;
    delete tmp;
  }
}

template <typename T>
void cgc_List<T>::cgc_add(T o)
{
  cgc_Element *item = new cgc_Element(o, 0);
  if (!head)
    head = item;
  else
  {
    cgc_Element *tmp = head;
    while (tmp && tmp->next) tmp = tmp->next;
    tmp->next = item;
  }
  len++;
}

template <typename T>
void cgc_List<T>::cgc_remove(T o)
{
  cgc_Element *item = cgc_find(o), *tmp;
  if (!item)
    return;
  if (item == head)
  {
    tmp = head->next;
    delete head;
    head = tmp;
  }
  else
  {
    tmp = head;
    while (tmp && tmp->next != item) tmp = tmp->next;
    tmp->next = item->next;
    delete item;
  }
  len--;
}

template <typename T>
typename cgc_List<T>::cgc_Element* cgc_List<T>::cgc_find(T o)
{
  if (!head)
    return 0;
  cgc_Element *tmp = head;
  while (tmp && tmp->obj != o) tmp = tmp->next;
  return tmp;
}

template <typename T>
T cgc_List<T>::cgc_get(int idx)
{
  if (!head || idx >= len)
    return 0;
  cgc_Element *tmp = head;
  while (idx--) tmp = tmp->next;
  return tmp->obj;
}

template <typename T>
void cgc_List<T>::cgc_debug()
{
  if (!head)
  {
    printf("Empty list!\n");
    return;
  }
  cgc_Element *tmp = head;
  while (tmp)
  {
    printf("[%d (%08x)] ", tmp->obj, tmp->obj);
    tmp = tmp->next;
  }
  printf("\n");
}

#endif
