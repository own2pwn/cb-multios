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
#include "enums.h"
#include "num_gen.h"

class cgc_Gridder
{
  public:
    cgc_Gridder();
    cgc_Gridder(const cgc_Gridder &copy);
    void cgc_ResetBoard();
    bool cgc_CopyGridder(unsigned int *pboard, int size);
    bool cgc_GenerateNewGridder(int cell_idx, cgc_NumGen *pngen);
    void cgc_MakeGridderSolveable(int difficulty, cgc_NumGen *pngen);
    cgc_Gridder *cgc_FindSolution(int cell_idx);
    bool cgc_HasUniqueSolution(int cell_idx, const cgc_Gridder *s1, bool *cgc_exit);
    bool cgc_ValidateGridder(const cgc_Gridder &master);
    void cgc_Serialize(cgc_FILE *out);
    unsigned int *cgc_GetRawGridder(int *size);
    void cgc_Debug();

  private:
    bool cgc_ValidAddNumber(int val, int row, int col);
    bool cgc_ValidCell(int row, int col);

    unsigned int board_[SET_SIZE][SET_SIZE];
};
