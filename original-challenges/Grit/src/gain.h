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
#pragma once

class cgc_Gain
{
public:
    inline cgc_Gain() : cgc_value(0)
    {
    }
    static inline cgc_Gain cgc_fromDouble(double d)
    {
        // only positive values are valid
        if (d < 0)
            return cgc_Gain(0);
        if (d <= 0.01)
            return cgc_Gain(INT32_MIN);
        if (d >= 10.0)
            return cgc_Gain(INT32_MAX);
        if (d >= 1)
            return cgc_Gain((d-1) / 10 * INT32_MAX);
        else
            return cgc_Gain(((1/d) - 1) / 10 * -INT32_MAX);
    }
    static inline cgc_Gain cgc_fromRational(cgc_int16_t numerator, cgc_int16_t denominator)
    {
        double tmp = (double)numerator / (double)denominator;
        tmp /= 10;
        tmp *= INT32_MAX;
        return cgc_Gain((int)tmp);
    }
    static inline cgc_Gain cgc_fromPanLeft(cgc_int32_t pan)
    {
        if (pan <= 0)
            return cgc_Gain(0);
        else
            return cgc_Gain(-pan);
    }
    static inline cgc_Gain cgc_fromPanRight(cgc_int32_t pan)
    {
        if (pan >= 0)
            return cgc_Gain(0);
        else
            return cgc_Gain(pan);
    }
    inline cgc_Gain operator*(const cgc_Gain &b) const
    {
        return cgc_fromDouble(cgc_toDouble() * b.cgc_toDouble());
    }
    inline cgc_Gain operator+(const cgc_Gain &b) const
    {
        long long tmp = cgc_value;
        tmp += b.cgc_value;
        if (tmp > INT32_MAX)
            tmp = INT32_MAX;
        if (tmp < INT32_MIN)
            tmp = INT32_MIN;
        return tmp;
    }
    inline cgc_int32_t cgc_adjustSample(cgc_int32_t sample) const
    {
        double tmp = sample * cgc_toDouble();
        if (tmp >= INT32_MAX)
            return INT32_MAX;
        if (tmp <= INT32_MIN)
            return INT32_MIN;
        return tmp;
    }
    inline double cgc_toDouble() const
    {
        double tmp = cgc_value;
        tmp *= 10.0 / INT32_MAX;
        if (tmp >= 0)
            return (tmp + 1);
        else
            return 1 / (-tmp + 1);
    }
private:
    inline cgc_Gain(cgc_int32_t value_) : cgc_value(value_)
    {
    }
    cgc_int32_t cgc_value;
};
