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
#pragma once

#include "vector.h"

unsigned int cgc_g_prng(void);

struct cgc_Edge
{
    double weight;
    double delta;
};

class cgc_Neuron
{
  public:
    cgc_Neuron() {};
    cgc_Neuron(unsigned int n, unsigned int outLen);
    void cgc_setValue(double v) { m_value = v; }
    double cgc_getValue() { return m_value; }
    void cgc_feedForward(cgc_vector<cgc_Neuron> &prev);
    void cgc_computeOutGrad(double target);
    void cgc_computeHiddenGrad(cgc_vector<cgc_Neuron> &next);
    void cgc_updateWeights(cgc_vector<cgc_Neuron> &prev);

    double cgc_sigmoid(double x);
    double cgc_dsigmoid(double x);

  private:
    static double k_eta;
    static double k_alpha;
    double m_value;
    double m_gradient;
    unsigned int m_n;
    cgc_vector<cgc_Edge> m_weights;
};
