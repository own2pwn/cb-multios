#ifndef LIST_H_
#define LIST_H_

#define MAX_LIST_SIZE 50000

extern "C"
{
#include <libcgc.h>
};

template <typename T>
class cgc_List
{
private:
    struct cgc_node
    {
        cgc_node *next;
        cgc_node *prev;
        T data;
    };

    size_t len;
    cgc_node *head;
    cgc_node *tail;
public:
    cgc_List();
    ~cgc_List();
    int cgc_length();
    bool cgc_find(T item);
    bool cgc_add(T item);
    bool cgc_remove(size_t idx);
    bool cgc_pop(T &item);
    void cgc_clear_list(bool delete_memory=false);
    bool cgc_is_empty();

    T operator[](size_t idx) const;

};

template <typename T>
cgc_List<T>::cgc_List()
{
    len = 0;
    head = NULL;
    tail = NULL;
}

template <typename T>
cgc_List<T>::~cgc_List()
{
    len = 0;
    cgc_clear_list();
}

template <typename T>
int cgc_List<T>::cgc_length()
{
    return len;
}


template <typename T>
bool cgc_List<T>::cgc_find(T item)
{
    for (cgc_node *iter = head; iter; iter=iter->next) {
        if(iter->data == item)
            return true;
    }

    return false;
}

template <typename T>
bool cgc_List<T>::cgc_add(T item)
{
    cgc_node *new_node = new cgc_node;
    if (!new_node)
        return false;

    new_node->prev = tail;
    new_node->next = NULL;
    new_node->data = item;

    if(!head)
        head = new_node;
    if(tail)
        tail->next = new_node;

    tail = new_node;
    len++;
    return true;
}

template <typename T>
bool cgc_List<T>::cgc_remove(size_t idx)
{
    if (idx >= len || len == 0)
        return false;

    if (len == 1) {
        delete head;
        head = NULL;
        tail = NULL;

        len--;
        return true;
    }

    int i = 0;
    cgc_node *iter = NULL;
    for (iter = head; i != idx; iter=iter->next, i++);

    if (iter == head) {
        head = head->next;
        head->prev = NULL;
    } else if (iter == tail) {
        tail = tail->prev;
        tail->next = NULL;
    } else {
        iter->prev->next = iter->next;
        iter->next->prev = iter->prev;
    }

    delete iter;
    len--;
    return true;
}

template <typename T>
bool cgc_List<T>::cgc_pop(T &item)
{
    if (len == 0)
        return false;

    cgc_node *temp;
    item = tail->data;
    if (len == 1) {
        delete head;
        head = NULL;
        tail = NULL;
        return true;
    }

    temp = tail;
    tail = tail->prev;
    tail->next = NULL;

    delete temp;
    len--;
    return true;
}

template <typename T>
void cgc_List<T>::cgc_clear_list(bool delete_memory)
{
    T temp;
    while (cgc_pop(temp)) {
        if (delete_memory)
            delete temp;
    }
}

template <typename T>
bool cgc_List<T>::cgc_is_empty()
{
    return (len == 0);
}

template <typename T>
T cgc_List<T>::operator[](size_t idx) const
{
    if (idx >= len)
        return T();

    int i = 0;
    cgc_node *iter = NULL;
    for (iter = head; i != idx; iter=iter->next, i++);

    return iter->data;
}

#endif
