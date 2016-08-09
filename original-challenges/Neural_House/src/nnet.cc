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
#include "nnet.h"

cgc_NeuralNet::cgc_NeuralNet(cgc_vector<unsigned int> &t)
{
  unsigned int i, j;
  for (i = 0; i < t.cgc_size(); ++i)
  {
    m_layers.cgc_push_back(cgc_Layer());
    for (j = 0; j <= t[i]; ++j)
      m_layers.cgc_back().cgc_push_back(cgc_Neuron(j, i == t.cgc_size() - 1 ? 0 : t[i+1]));
    m_layers.cgc_back().cgc_back().cgc_setValue(BIAS_VAL);
  }
}

void cgc_NeuralNet::cgc_feedForward(cgc_vector<double> &in)
{
  unsigned int i, j;
  cgc_Layer &inputLayer = m_layers.cgc_front();
  for (i = 0; i < in.cgc_size(); ++i)
    inputLayer[i].cgc_setValue(in[i]);
  for (i = 1; i < m_layers.cgc_size(); ++i)
  {
    cgc_Layer &prev = m_layers[i - 1];
    for (j = 0; j < m_layers[i].cgc_size() - 1; ++j)
      m_layers[i][j].cgc_feedForward(prev);
  }
}

void cgc_NeuralNet::cgc_backProp(cgc_vector<double> &target)
{
  unsigned int i, j;
  cgc_Layer &outputLayer = m_layers.cgc_back();
  for (i = 0; i < outputLayer.cgc_size() - 1; ++i)
    outputLayer[i].cgc_computeOutGrad(target[i]);
  for (i = m_layers.cgc_size() - 2; i > 0; --i)
  {
    cgc_Layer &hiddenLayer = m_layers[i];
    cgc_Layer &next = m_layers[i+1];
    for (j = 0; j < hiddenLayer.cgc_size(); ++j)
      hiddenLayer[j].cgc_computeHiddenGrad(next);
  }
  for (i = m_layers.cgc_size() - 1; i > 0; --i)
  {
    cgc_Layer &l = m_layers[i];
    cgc_Layer &prev = m_layers[i-1];
    for (j = 0; j < l.cgc_size() - 1; ++j)
      l[j].cgc_updateWeights(prev);
  }
}

void cgc_NeuralNet::cgc_getOutput(cgc_vector<double> &out)
{
  unsigned int i;
  out.cgc_clear();
  cgc_Layer &outputLayer = m_layers.cgc_back();
  for (i = 0; i < outputLayer.cgc_size() - 1; ++i)
    out.cgc_push_back(outputLayer[i].cgc_getValue());
}
