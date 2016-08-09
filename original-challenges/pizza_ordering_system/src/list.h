#ifndef LIST_H_
#define LIST_H_

#define MAX_LIST_SIZE 1024

extern "C"
{
#include <libcgc.h>
#include <stdlib.h>
#include <string.h>
};

template <typename T>
class cgc_List
{
private:
    T data[MAX_LIST_SIZE];
    size_t len;
public:
    cgc_List();
    ~cgc_List();
    int cgc_length();
    bool cgc_add(T item);
    bool cgc_remove(size_t idx);
    bool cgc_pop(T &item);
    bool cgc_is_empty();
    bool cgc_is_full();
    void cgc_print_list(void (*print_func)(void *));

    T operator[](size_t idx) const;
    T &operator[](size_t idx);

};

template <typename T>
cgc_List<T>::cgc_List()
{
    len = 0;
}

template <typename T>
cgc_List<T>::~cgc_List()
{
    len = 0;
}

template <typename T>
int cgc_List<T>::cgc_length()
{
    return len;
}

template <typename T>
bool cgc_List<T>::cgc_add(T item)
{
    if (len == MAX_LIST_SIZE)
        return false;

    data[len++] = item;
    return true;
}

template <typename T>
bool cgc_List<T>::cgc_remove(size_t idx)
{
    if (idx >= len)
        return false;

    cgc_memcpy(&data[idx], &data[idx+1], (len - idx) * sizeof(T));
    len--;
    return true;
}

template <typename T>
bool cgc_List<T>::cgc_pop(T &item)
{
    if (len == 0)
        return false;

    item = data[--len];
    return true;
}


template <typename T>
bool cgc_List<T>::cgc_is_empty()
{
    return (len == 0);
}

template <typename T>
bool cgc_List<T>::cgc_is_full()
{
    return (len == MAX_LIST_SIZE);
}

template <typename T>
T cgc_List<T>::operator[](size_t idx) const
{
    if (idx >= len)
        return data[0];

    return data[idx];
}

template <typename T>
T &cgc_List<T>::operator[](size_t idx)
{
    if (idx >= len)
        return data[len];

    return data[idx];
}

template <typename T>
void cgc_List<T>::cgc_print_list(void (*print_func)(void *))
{
    int i;
    for (i = 0; i < len; i++) {
        printf("\t%d. ", i+1);
        print_func(data[i]);
    }
}

#endif
