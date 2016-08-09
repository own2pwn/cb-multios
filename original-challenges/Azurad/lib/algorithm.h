#ifndef ALGORITHM_H_
#define ALGORITHM_H_

namespace std
{
    template <typename T>
    inline const T &cgc_min (const T &a, const T &b)
    {
        if (a < b)
            return a;
        else
            return b;
    }

    template <typename T>
    inline const T &cgc_max (const T &a, const T &b)
    {
        if (a > b)
            return a;
        else
            return b;
    }

    template <typename T>
    inline const T&& cgc_move (const T &&a)
    {
        return a;
    }

    template <typename T>
    inline T&& cgc_move (T &&a)
    {
        return a;
    }

    template <typename T>
    inline void cgc_swap (T &a, T &b)
    {
        T tmp = cgc_move(a);
        a = cgc_move(b);
        b = cgc_move(tmp);
    }
};

#endif
