#pragma once

#include <cstdlib.h>

template <class T>
class cgc_vector
{
  public:
    cgc_vector();
    cgc_vector(cgc_size_t sz);
    cgc_vector(const cgc_vector<T> &vec);
    ~cgc_vector();

    bool cgc_empty() const;
    cgc_size_t cgc_size() const;
    cgc_size_t cgc_capacity() const;
    T& cgc_front();
    T& cgc_back();
    void cgc_push_back(const T& val);
    void cgc_pop_back();
    void cgc_reserve(cgc_size_t cap);

    cgc_vector<T>& operator= (const cgc_vector<T> &vec);
    T& operator[] (unsigned int idx);
    void cgc_clear();

  private:
    cgc_size_t m_size;
    cgc_size_t m_capacity;
    T* m_data;
};

template<class T>
cgc_vector<T>::cgc_vector()
{
  m_capacity = 0;
  m_size = 0;
  m_data = 0;
}

template<class T>
cgc_vector<T>::cgc_vector(cgc_size_t sz)
{
  m_capacity = m_size = sz;
  m_data = new T[sz];
}

template<class T>
cgc_vector<T>::cgc_vector(const cgc_vector<T>& vec)
{
  m_capacity = vec.m_capacity;
  m_size = vec.m_size;
  m_data = new T[m_capacity];
  for (unsigned int i = 0; i < m_size; ++i)
    m_data[i] = vec.m_data[i];
}

template<class T>
cgc_vector<T>::~cgc_vector()
{
  delete[] m_data;
}

template<class T>
T& cgc_vector<T>::cgc_front()
{
  return m_data[0];
}

template<class T>
T& cgc_vector<T>::cgc_back()
{
  return m_data[m_size - 1];
}

template<class T>
void cgc_vector<T>::cgc_push_back(const T& val)
{
  if (m_capacity == 0)
    cgc_reserve(16);
  if (m_size == m_capacity)
    cgc_reserve(m_capacity * 2);
  m_data[m_size++] = val;
}

template<class T>
void cgc_vector<T>::cgc_reserve(cgc_size_t cap)
{
  T* d = new T[cap];
  for (unsigned int i = 0; i < m_size; ++i)
    d[i] = m_data[i];
  m_capacity = cap;
  if (m_data)
    delete[] m_data;
  m_data = d;
}

template<class T>
bool cgc_vector<T>::cgc_empty() const
{
  return m_size == 0;
}

template<class T>
cgc_size_t cgc_vector<T>::cgc_size() const
{
  return m_size;
}

template<class T>
cgc_size_t cgc_vector<T>::cgc_capacity() const
{
  return m_capacity;
}

template<class T>
T& cgc_vector<T>::operator[] (unsigned int idx)
{
  return m_data[idx];
}

template<class T>
cgc_vector<T>& cgc_vector<T>::operator= (const cgc_vector<T> &vec)
{
  if (m_data)
    delete[] m_data;
  m_size = vec.m_size;
  m_capacity = vec.m_capacity;
  m_data = new T[m_capacity];
  for (unsigned int i = 0; i < m_size; ++i)
    m_data[i] = vec.m_data[i];
  return *this;
}

template<class T>
void cgc_vector<T>::cgc_clear()
{
  m_capacity = 0;
  m_size = 0;
  if (m_data)
    delete[] m_data;
  m_data = 0;
}
