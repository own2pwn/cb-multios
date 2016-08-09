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

#include "cstdlib.h"
#include "cstdio.h"
#include "cstring.h"
#include "cctype.h"
#include "types.h"

#ifdef DEBUG
  #define DBG(s, ...) cgc_fprintf(stderr, "DEBUG:\t" s "\n", ##__VA_ARGS__)
#else
  #define DBG(s, ...)
#endif

#define ASSERT(x, msg, ...) ({ \
    if (!(x)) \
    { \
      cgc_fprintf(stderr, "Assertion Failed: " msg "\n",  ##__VA_ARGS__); \
      _terminate(1); \
    } \
  })

static void cgc_WaitForInput(cgc_FILE* Stream, char* Input, cgc_u32 cgc_Max)
{
  cgc_fflush(stdout);
  cgc_memset(Input, cgc_Max, '\0');
  if (cgc_freaduntil(Input, cgc_Max, '\n', Stream) <= 0)
  {
    cgc_exit(0);
  }
}

static inline cgc_u8 cgc_CharToNum(char c)
{
  return cgc_digittoint(c);
}

static int cgc_CanBeDecimal(char* Token)
{
  if (*Token == '-')
    Token++;

  while (Token && *Token && (cgc_isdigit((char)*Token) || cgc_isspace((char)*Token)))
      Token++;

  if (Token && *Token == '\0')
    return 1;
  else
    return 0;
}

static int cgc_CanBeHex(char* Token)
{
  if (*Token == '-')
    Token++;

  while (Token && *Token && (cgc_isxdigit((char)*Token) || cgc_isspace((char)*Token)))
      Token++;

  if (Token && *Token == '\0')
    return 1;
  else
    return 0;
}

static int cgc_CanBeOctal(char* Token)
{
  if (*Token == '-')
    Token++;

  while (Token && *Token && ((cgc_isdigit((char)*Token) && cgc_CharToNum(*Token) < 8) || cgc_isspace(*Token)))
      Token++;

  if (Token && *Token == '\0')
    return 1;
  else
    return 0;
}

static int cgc_ParseInt(char* Token, cgc_s32* Value)
{
  long AsBase10 = cgc_CanBeDecimal(Token) ? cgc_strtol(Token, NULL, 10) : LONG_MIN;
  long AsBase16 = cgc_CanBeHex(Token) ? cgc_strtol(Token, NULL, 16) : LONG_MIN;
  long AsBase8 = cgc_CanBeOctal(Token) ? cgc_strtol(Token, NULL, 8) : LONG_MIN;

  if (AsBase10 != LONG_MIN && AsBase10 != LONG_MAX)
    *Value = AsBase10;
  else if (AsBase16 != LONG_MIN && AsBase16 != LONG_MAX)
    *Value = AsBase16;
  else if (AsBase8 != LONG_MIN && AsBase8 != LONG_MAX)
    *Value = AsBase8;
  else
    return -1;

  return 1;
}

template <typename t1>
struct cgc_Vector
{
  t1* _Storage;
  cgc_u32 _Capacity;
  cgc_u32 _Size;
  cgc_u32 _StepSize;

  cgc_Vector()
  {
    _Size = 0;
    _StepSize = 10;
    _Capacity = _StepSize;
    _Storage = new t1[sizeof(t1) * _Capacity];
  }

  cgc_Vector(cgc_u32 Cap) : _Capacity(Cap)
  {
    _Size = 0;
    _StepSize = 10;
    _Capacity = Cap;
    _Storage = new t1[sizeof(t1) * _Capacity];
  }

  ~cgc_Vector()
  {
    if (_Storage)
      delete[] _Storage;
  }

  t1& operator[] (const cgc_s32 StorageIndex)
  {
    return _Storage[StorageIndex];
  }

  void cgc_Append(t1 Value)
  {
    if (_Size == _Capacity)
    {
      cgc__Grow();
    }

    _Storage[_Size] = Value;
    _Size += 1;
  }

  t1 cgc_Head(void)
  {
    ASSERT(_Size, "Getting head of empty vector");
    return _Storage[0];
  }

  t1 cgc_Last(void)
  {
    ASSERT(_Size, "Getting last of empty vector");
    return _Storage[_Size - 1];
  }

  t1 cgc_RemoveLast(void)
  {
    t1 Ret = cgc_Last();
    _Size--;

    if (_Capacity - _Size > _StepSize)
    {
      cgc__Shrink();
    }

    return Ret;
  }

  int cgc_Empty(void)
  {
    return _Size == 0;
  }

  cgc_Vector* cgc_Copy(void)
  {
    cgc_Vector* VectorCopy = new cgc_Vector<t1>(_Capacity);
    VectorCopy->_Capacity = _Capacity;
    VectorCopy->_Size = _Size;
    VectorCopy->_StepSize = _StepSize;
    cgc_memcpy(VectorCopy->_Storage, _Storage, _Capacity * sizeof(t1));
    return VectorCopy;
  }

  void cgc__Grow(void)
  {
    cgc_u32 NewCapacity = ((_Capacity / _StepSize) + 1) * _StepSize;
    t1* NewStorage = new t1[sizeof(t1) * NewCapacity];

    cgc_memcpy(NewStorage, _Storage, sizeof(t1) * _Capacity);

    delete[] _Storage;
    _Storage = NewStorage;
    _Capacity = NewCapacity;
  }

  void cgc__Shrink(void)
  {
    cgc_u32 NewCapacity = _Capacity - _StepSize;
    t1* NewStorage = new t1[sizeof(t1) * NewCapacity];

    cgc_memcpy(NewStorage, _Storage, sizeof(t1) * NewCapacity);

    delete[] _Storage;
    _Storage = NewStorage;
    _Capacity = NewCapacity;
  }
};

template <typename t1>
struct cgc_Stack
{
  cgc_Vector<t1>* _BackingVector;

  void cgc_Push(t1 Value)
  {
    _BackingVector->cgc_Append(Value);
  }

  int cgc_Empty(void)
  {
    return _BackingVector->cgc_Empty();
  }

  int cgc_Size(void)
  {
    return _BackingVector->_Size;
  }

  t1 cgc_Pop(void)
  {
    ASSERT(!cgc_Empty(), "Popping off empty stack");
    return _BackingVector->cgc_RemoveLast();
  }

  t1 cgc_Peek(void)
  {
    ASSERT(!cgc_Empty(), "Peeking off empty stack");
    return _BackingVector->cgc_Last();
  }

  t1& operator[] (const cgc_s32 StorageIndex)
  {
    return (*_BackingVector)[_BackingVector->_Size - StorageIndex - 1];
  }

  cgc_Stack* cgc_Copy(void)
  {
    cgc_Stack<t1>* StackCopy = (cgc_Stack<t1> *)cgc_malloc(sizeof(cgc_Stack<t1>));
    StackCopy->_BackingVector = _BackingVector->cgc_Copy();
    return StackCopy;
  }

  void cgc_Invert(void)
  {
    for (size_t Index = 0; Index < cgc_Size() / 2; ++Index)
    {
      t1 Temp = _BackingVector->_Storage[Index];
      _BackingVector->_Storage[Index] = _BackingVector->_Storage[cgc_Size() - Index - 1];
      _BackingVector->_Storage[cgc_Size() - Index - 1] = Temp;
    }
  }

  cgc_Stack(cgc_u32 Cap)
  {
    _BackingVector = new cgc_Vector<t1>(Cap);
  }

  cgc_Stack()
  {
    _BackingVector = new cgc_Vector<t1>();
  }

  ~cgc_Stack()
  {
    if (_BackingVector)
    {
      delete _BackingVector;
    }
  }
};

struct cgc_VC
{
#define MAX_INPUT_SIZE 256
  cgc_u16 _MinHeight = 80;
  cgc_u16 _Width = 120;

  cgc_FILE* In;
  cgc_FILE* Out;

  char _InputBuffer[MAX_INPUT_SIZE];
  char _ErrorBuffer[MAX_INPUT_SIZE];
  int HasError;

  enum CommandReturn {
    ADD_HISTORY,
    NO_HISTORY,
    ERROR,
  };

  struct cgc_CommandEntry {
    const char* Name;
    cgc_u8 Arity;
    CommandReturn (cgc_VC::*F0)();
    CommandReturn (cgc_VC::*F1)(cgc_s32 v1);
    CommandReturn (cgc_VC::*F2)(cgc_s32 v1, cgc_s32 v2);
  };

  struct cgc_HistElem
  {
    cgc_Stack<cgc_s32>* CalcStack;
    const cgc_CommandEntry* Entry;
  };

  cgc_Stack<cgc_s32>* CalcStack = new cgc_Stack<cgc_s32>();
  cgc_Stack<cgc_HistElem>* HistStack = new cgc_Stack<cgc_HistElem>(64);

  static const cgc_u8 NumCommands = 29;
  const cgc_CommandEntry CommandTable[NumCommands] = {
    {"push", 1, NULL, &cgc_VC::cgc_ManualPush, NULL},

    { "+", 0, &cgc_VC::cgc_Add, NULL, NULL},
    { "-", 0, &cgc_VC::cgc_Sub, NULL, NULL},
    { "*", 0, &cgc_VC::cgc_Mul, NULL, NULL},
    { "/", 0, &cgc_VC::cgc_Div, NULL, NULL},
    { "!", 0, &cgc_VC::cgc_Fact, NULL, NULL},
    { "neg", 0, &cgc_VC::cgc_Neg, NULL, NULL},
    { "abs", 0, &cgc_VC::cgc_Abs, NULL, NULL},
    { "mod", 0, &cgc_VC::cgc_Mod, NULL, NULL},
    { "&", 0, &cgc_VC::cgc_And, NULL, NULL},
    { "|", 0, &cgc_VC::cgc_Or, NULL, NULL},
    { "^", 0, &cgc_VC::cgc_Xor, NULL, NULL},
    { "~", 0, &cgc_VC::cgc_Not, NULL, NULL},
    { "sum", 0, &cgc_VC::cgc_Sum, NULL, NULL},
    { "avg", 0, &cgc_VC::cgc_Avg, NULL, NULL},
    { "dup", 0, &cgc_VC::cgc_Dup, NULL, NULL},
    { "dupn", 0, &cgc_VC::cgc_DupN, NULL, NULL},
    { "dupn", 0, NULL, &cgc_VC::cgc_DupN, NULL},
    { "dupr", 0, &cgc_VC::cgc_DupR, NULL, NULL},
    { "dupr", 2, NULL, NULL, &cgc_VC::cgc_DupR},
    { "min", 0, &cgc_VC::cgc_Min, NULL, NULL},
    { "max", 0, &cgc_VC::cgc_Max, NULL, NULL},
    {"depth", 0, &cgc_VC::cgc_Depth, NULL, NULL},
    {"drop", 0, &cgc_VC::cgc_Drop, NULL, NULL},
    {"dropn", 0, &cgc_VC::cgc_DropN, NULL, NULL},
    {"dropn", 1, NULL, &cgc_VC::cgc_DropN, NULL},
    {"undo", 0, &cgc_VC::cgc_Undo, NULL, NULL},
    {"ivrt", 0, &cgc_VC::cgc_Invert, NULL, NULL},
    {"shuf", 0, &cgc_VC::cgc_Shuffle, NULL, NULL},
  };

  cgc_VC(cgc_FILE* In, cgc_FILE* Out) : In(In), Out(Out)
  {
    cgc_memset(_InputBuffer, '\0', MAX_INPUT_SIZE);
    cgc_memset(_ErrorBuffer, '\0', MAX_INPUT_SIZE);
    HasError = 0;
  }

  ~cgc_VC()
  {

    while (!CalcStack->cgc_Empty())
    {
      CalcStack->cgc_Pop();
    }

    cgc_fprintf(Out, "Bye bye\n");
    cgc_fflush(Out);
  }

  void cgc_ClearScreen(void)
  {
    cgc_fprintf(Out, "\033[2J\033[H");
  }

  void cgc_DrawScreen(void)
  {
    cgc_ClearScreen();
    cgc_s32 Height = _MinHeight > CalcStack->cgc_Size() ? _MinHeight : CalcStack->cgc_Size();
    for (cgc_s32 LineIndex = Height - 1; LineIndex >= 0; --LineIndex)
    {
      cgc_DrawLine(LineIndex);
    }

    if (HasError)
    {
      cgc_printf("%s", _ErrorBuffer);
      HasError = 0;
    }
    else
    {
      cgc_DrawFooter();
    }
  }

  void cgc_DrawLine(cgc_u32 LineIndex)
  {
    cgc_fprintf(Out, "%03d:\t", LineIndex);

    if (CalcStack->cgc_Size() > LineIndex)
    {
      cgc_fprintf(Out, "%d", (*CalcStack)[LineIndex]);
    }

    cgc_fprintf(Out, "\n");
  }

  void cgc_DrawFooter(void)
  {
    for (cgc_u32 Index = 0; Index < _Width; ++Index)
    {
      cgc_fprintf(Out, "-");
    }
    cgc_fprintf(Out, "\n");
  }

  const cgc_CommandEntry* cgc_FindCommand(char* Command, cgc_u8 Arity)
  {
    for (cgc_u8 CommandIndex = 0; CommandIndex < NumCommands; ++CommandIndex)
    {
      if (Arity == CommandTable[CommandIndex].Arity && cgc_strcmp(Command, CommandTable[CommandIndex].Name) == 0)
        return &CommandTable[CommandIndex];
    }

    return NULL;
  }

  void cgc_HandleCallResult(CommandReturn Value, const cgc_CommandEntry* Entry, cgc_HistElem* Hist)
  {
    switch (Value)
    {
      case ADD_HISTORY:
        {
          HistStack->cgc_Push(*Hist);
          break;
        }
      case NO_HISTORY:
        {
          break;
        }
      case ERROR:
        {
          break;
        }
    }
  }

  void cgc_HandleCall(const cgc_CommandEntry* Entry, cgc_s32 V1, cgc_s32 V2)
  {
    cgc_HistElem Hist = cgc_HistElem{CalcStack->cgc_Copy(), Entry};
    cgc_HandleCallResult(cgc_CallCommand(Entry, V1, V2), Entry, &Hist);
  }

  void cgc_HandleCall(const cgc_CommandEntry* Entry, cgc_s32 V1)
  {
    cgc_HistElem Hist = cgc_HistElem{CalcStack->cgc_Copy(), Entry};
    cgc_HandleCallResult(cgc_CallCommand(Entry, V1), Entry, &Hist);
  }

  void cgc_HandleCall(const cgc_CommandEntry* Entry)
  {
    cgc_HistElem Hist = cgc_HistElem{CalcStack->cgc_Copy(), Entry};
    cgc_HandleCallResult(cgc_CallCommand(Entry), Entry, &Hist);
  }

  CommandReturn cgc_CallCommand(const cgc_CommandEntry* Entry)
  {
    return (this->*Entry->F0)();
  }

  CommandReturn cgc_CallCommand(const cgc_CommandEntry* Entry, cgc_s32 V1)
  {
    return (this->*Entry->F1)(V1);
  }

  CommandReturn cgc_CallCommand(const cgc_CommandEntry* Entry, cgc_s32 V1, cgc_s32 V2)
  {
    return (this->*Entry->F2)(V1, V2);
  }

  void cgc_ErrorTooFewArgs(const char* Command)
  {
    cgc_sprintf(_ErrorBuffer, "Error: Too few arguments for '%s' command\n", Command);
    HasError = 1;
  }

  void cgc_ErrorInvalidInput(void)
  {
    cgc_sprintf(_ErrorBuffer, "Error: Invalid input\n");
    HasError = 1;
  }


  void cgc_PushValue(cgc_s32 Value)
  {
    CalcStack->cgc_Push(Value);
  }

  CommandReturn cgc_ManualPush(cgc_s32 Value)
  {
    CalcStack->cgc_Push(Value);
    return ADD_HISTORY;
  }

  CommandReturn cgc_Add(void)
  {
    if (CalcStack->cgc_Size() < 2)
    {
      cgc_ErrorTooFewArgs("+");
      return ERROR;
    }

    cgc_PushValue(CalcStack->cgc_Pop() + CalcStack->cgc_Pop());

    return ADD_HISTORY;
  }

  CommandReturn cgc_Sub(void)
  {
    if (CalcStack->cgc_Size() < 2)
    {
      cgc_ErrorTooFewArgs("-");
      return ERROR;
    }

    cgc_s32 V1 = CalcStack->cgc_Pop();
    cgc_s32 V2 = CalcStack->cgc_Pop();
    cgc_PushValue(V2 - V1);

    return ADD_HISTORY;
  }

  CommandReturn cgc_Mul(void)
  {
    if (CalcStack->cgc_Size() < 2)
    {
      cgc_ErrorTooFewArgs("*");
      return ERROR;
    }

    cgc_PushValue(CalcStack->cgc_Pop() * CalcStack->cgc_Pop());

    return ADD_HISTORY;
  }

  CommandReturn cgc_Div(void)
  {
    if (CalcStack->cgc_Size() < 2)
    {
      cgc_ErrorTooFewArgs("/");
      return ERROR;
    }

    cgc_s32 V1 = CalcStack->cgc_Pop();
    cgc_s32 V2 = CalcStack->cgc_Pop();

    if (V1 == 0)
    {
      cgc_ErrorInvalidInput();
      cgc_PushValue(V2);
      cgc_PushValue(V1);
      return ERROR;
    }

    cgc_PushValue(V2 / V1);

    return ADD_HISTORY;
  }

  CommandReturn cgc_Neg(void)
  {
    if (CalcStack->cgc_Size() < 1)
    {
      cgc_ErrorTooFewArgs("neg");
      return ERROR;
    }

    cgc_PushValue(-CalcStack->cgc_Pop());

    return ADD_HISTORY;
  }

  CommandReturn cgc_Fact(void)
  {
    if (CalcStack->cgc_Size() < 1)
    {
      cgc_ErrorTooFewArgs("!");
      return ERROR;
    }


    cgc_s32 Value = CalcStack->cgc_Pop();

    if (Value < 0 || Value > 1000)
    {
      cgc_ErrorInvalidInput();
      cgc_PushValue(Value);
      return ERROR;
    }

    cgc_s32 Res = 1;

    while (Value > 0)
      Res *= Value--;

    cgc_PushValue(Res);

    return ADD_HISTORY;
  }

  CommandReturn cgc_Abs(void)
  {
    if (CalcStack->cgc_Size() < 1)
    {
      cgc_ErrorTooFewArgs("abs");
      return ERROR;
    }

    cgc_s32 Value = CalcStack->cgc_Pop();
    if (Value <= 0)
      cgc_PushValue(-Value);
    else
      cgc_PushValue(Value);

    return ADD_HISTORY;
  }

  CommandReturn cgc_Mod(void)
  {
    if (CalcStack->cgc_Size() < 2)
    {
      cgc_ErrorTooFewArgs("mod");
      return ERROR;
    }

    cgc_s32 Modulus = CalcStack->cgc_Pop();
    cgc_s32 Value = CalcStack->cgc_Pop();

    if (Modulus == 0)
    {
      cgc_ErrorInvalidInput();
      cgc_PushValue(Value);
      cgc_PushValue(Modulus);
      return ERROR;
    }

    cgc_PushValue(Value % Modulus);

    return ADD_HISTORY;
  }

  CommandReturn cgc_Xor(void)
  {
    if (CalcStack->cgc_Size() < 2)
    {
      cgc_ErrorTooFewArgs("^");
      return ERROR;
    }

    cgc_PushValue(CalcStack->cgc_Pop() ^ CalcStack->cgc_Pop());

    return ADD_HISTORY;
  }

  CommandReturn cgc_Or(void)
  {
    if (CalcStack->cgc_Size() < 2)
    {
      cgc_ErrorTooFewArgs("|");
      return ERROR;
    }

    cgc_PushValue(CalcStack->cgc_Pop() | CalcStack->cgc_Pop());

    return ADD_HISTORY;
  }

  CommandReturn cgc_And(void)
  {
    if (CalcStack->cgc_Size() < 2)
    {
      cgc_ErrorTooFewArgs("&");
      return ERROR;
    }

    cgc_PushValue(CalcStack->cgc_Pop() & CalcStack->cgc_Pop());

    return ADD_HISTORY;
  }

  CommandReturn cgc_Not(void)
  {
    if (CalcStack->cgc_Size() < 1)
    {
      cgc_ErrorTooFewArgs("~");
      return ERROR;
    }

    cgc_PushValue(~CalcStack->cgc_Pop());

    return ADD_HISTORY;
  }

  CommandReturn cgc_Sum(void)
  {
    if (CalcStack->cgc_Size() < 1)
    {
      cgc_ErrorTooFewArgs("sum");
      return ERROR;
    }

    cgc_s32 N = CalcStack->cgc_Pop();

    if (CalcStack->cgc_Size() < N || N < 0)
    {
      cgc_ErrorTooFewArgs("sum");
      cgc_PushValue(N);
      return ERROR;
    }

    cgc_s32 Accum = 0;
    while (N-- > 0)
    {
      Accum += CalcStack->cgc_Pop();
    }

    cgc_PushValue(Accum);

    return ADD_HISTORY;
  }

  CommandReturn cgc_Avg(void)
  {
    if (CalcStack->cgc_Size() < 1)
    {
      cgc_ErrorTooFewArgs("avg");
      return ERROR;
    }

    cgc_s32 N = CalcStack->cgc_Pop();

    if (N == 0)
    {
      cgc_ErrorInvalidInput();
      cgc_PushValue(N);
      return ERROR;
    }

    if (CalcStack->cgc_Size() < N || N < 0)
    {
      cgc_ErrorTooFewArgs("avg");
      cgc_PushValue(N);
      return ERROR;
    }

    cgc_s32 Accum = 0;
    for (cgc_s32 Count = 0; Count < N; Count++)
    {
      Accum += CalcStack->cgc_Pop();
    }

    cgc_PushValue(Accum / N);

    return ADD_HISTORY;
  }

  CommandReturn cgc_Dup(void)
  {
    if (CalcStack->cgc_Size() < 1)
    {
      cgc_ErrorTooFewArgs("dup");
      return ERROR;
    }

    cgc_PushValue(CalcStack->cgc_Peek());

    return ADD_HISTORY;
  }

  CommandReturn cgc_DupN(cgc_s32 N)
  {
    if (CalcStack->cgc_Size() < N || N <= 0)
    {
      cgc_ErrorTooFewArgs("dupn");
      return ERROR;
    }

    cgc_Stack<cgc_s32>* TempStack = new cgc_Stack<cgc_s32>();
    for (size_t Index = 0; Index < N; ++Index)
    {
      TempStack->cgc_Push((*CalcStack)[Index]);
    }

    while (!TempStack->cgc_Empty())
    {
      CalcStack->cgc_Push(TempStack->cgc_Pop());
    }

    delete TempStack;
    return ADD_HISTORY;
  }

  CommandReturn cgc_DupN(void)
  {
    if (CalcStack->cgc_Size() < 1)
    {
      cgc_ErrorTooFewArgs("dupn");
      return ERROR;
    }

    cgc_s32 N = CalcStack->cgc_Pop();

    CommandReturn Return = cgc_DupN(N);

    if (Return == ERROR)
    {
      CalcStack->cgc_Push(N);
    }

    return Return;
  }

  CommandReturn cgc_DupR(void)
  {
    if (CalcStack->cgc_Size() < 2)
    {
      cgc_ErrorTooFewArgs("dupr");
      return ERROR;
    }

    cgc_s32 Len = CalcStack->cgc_Pop();
    cgc_s32 Pos = CalcStack->cgc_Pop();

    CommandReturn Return = cgc_DupR(Pos, Len);

    if  (Return == ERROR)
    {
      CalcStack->cgc_Push(Pos);
      CalcStack->cgc_Push(Len);
    }

    return Return;
  }

  CommandReturn cgc_DupR(cgc_s32 Pos, cgc_s32 Len)
  {
    cgc_u16 Pos_ = (cgc_u16)Pos;
    cgc_u16 Len_ = (cgc_u16)Len;

#ifdef PATCHED_1
    if (CalcStack->cgc_Size() < Pos_ || CalcStack->cgc_Size() < Len_ || CalcStack->cgc_Size() < Pos_ + Len_)
#else
    if (CalcStack->cgc_Size() < Pos_ && CalcStack->cgc_Size() < Len_ && CalcStack->cgc_Size() < Pos_ + Len_)
#endif
    {
      cgc_ErrorInvalidInput();
      return ERROR;
    }

    for (cgc_u16 Index = Pos_; Index < Pos_ + Len_; ++Index)
    {
      CalcStack->cgc_Push((*CalcStack)[Index]);
    }

    return NO_HISTORY;
  }

  CommandReturn cgc_Shuffle(void)
  {
    if (CalcStack->cgc_Size() < 2)
    {
      cgc_ErrorInvalidInput();
      return ERROR;
    }

    cgc_u8* RBuf = new cgc_u8[CalcStack->cgc_Size()];
    cgc_memcpy(RBuf, (cgc_u8 *)0x4347C000, CalcStack->cgc_Size());
    for (cgc_u32 Index = 1; Index < CalcStack->cgc_Size(); ++Index)
    {
      RBuf[Index] ^= (Index % 0xff);
    }

    for (cgc_u32 Index = CalcStack->cgc_Size() - 1; Index >= 1; Index--)
    {
      cgc_u32 RIndex = RBuf[Index] % (Index + 1);
      cgc_s32 Temp = (*CalcStack)[Index];
      (*CalcStack)[Index] = (*CalcStack)[RIndex];
      (*CalcStack)[RIndex] = Temp;
    }

    delete RBuf;
    return ADD_HISTORY;
  }

  CommandReturn cgc_Min(void)
  {
    if (CalcStack->cgc_Size() < 2)
    {
      cgc_ErrorTooFewArgs("min");
      return ERROR;
    }

    cgc_s32 A = CalcStack->cgc_Pop();
    cgc_s32 B = CalcStack->cgc_Pop();
    cgc_PushValue(A < B ? A : B);

    return ADD_HISTORY;
  }

  CommandReturn cgc_Max(void)
  {
    if (CalcStack->cgc_Size() < 2)
    {
      cgc_ErrorTooFewArgs("max");
      return ERROR;
    }

    cgc_s32 A = CalcStack->cgc_Pop();
    cgc_s32 B = CalcStack->cgc_Pop();
    cgc_PushValue(A > B ? A : B);

    return ADD_HISTORY;
  }

  CommandReturn cgc_MinN(void)
  {
    if (CalcStack->cgc_Size() < 1)
    {
      cgc_ErrorTooFewArgs("minr");
      return ERROR;
    }

    cgc_s32 N = CalcStack->cgc_Pop();

    if (CalcStack->cgc_Size() < N)
    {
      cgc_ErrorTooFewArgs("minr");
      cgc_PushValue(N);
      return ERROR;
    }

    cgc_s32 LocalMin = CalcStack->cgc_Pop();
    for (cgc_s32 Count = 1; Count < N; Count++)
    {
      cgc_s32 Value = CalcStack->cgc_Pop();
      if (Value < LocalMin)
        LocalMin = Value;
    }

    cgc_PushValue(LocalMin);

    return ADD_HISTORY;
  }

  CommandReturn cgc_MaxN(void)
  {
    if (CalcStack->cgc_Size() < 1)
    {
      cgc_ErrorTooFewArgs("maxn");
      return ERROR;
    }

    cgc_s32 N = CalcStack->cgc_Pop();

    if (CalcStack->cgc_Size() < N)
    {
      cgc_ErrorTooFewArgs("maxn");
      cgc_PushValue(N);
      return ERROR;
    }

    cgc_s32 LocalMax = CalcStack->cgc_Pop();
    for (cgc_s32 Count = 1; Count < N; Count++)
    {
      cgc_s32 Value = CalcStack->cgc_Pop();
      if (Value > LocalMax)
        LocalMax = Value;
    }

    cgc_PushValue(LocalMax);

    return ADD_HISTORY;
  }

  CommandReturn cgc_Drop(void)
  {
    if (CalcStack->cgc_Size() < 1)
    {
      cgc_ErrorTooFewArgs("drop");
      return ERROR;
    }

    CalcStack->cgc_Pop();

    return ADD_HISTORY;
  }

  CommandReturn cgc_DropN(cgc_s32 N)
  {
    if (CalcStack->cgc_Size() < N || N < 0)
    {
      cgc_ErrorTooFewArgs("dropn");
      return ERROR;
    }

    while (N-- > 0)
    {
      CalcStack->cgc_Pop();
    }

    return ADD_HISTORY;
  }

  CommandReturn cgc_DropN(void)
  {
    if (CalcStack->cgc_Size() < 1)
    {
      cgc_ErrorTooFewArgs("dropn");
      return ERROR;
    }

    cgc_s32 N = CalcStack->cgc_Pop();

    CommandReturn Return = cgc_DropN(N);

    if  (Return == ERROR)
    {
      CalcStack->cgc_Push(N);
    }

    return Return;
  }

  CommandReturn cgc_Depth(void)
  {
    CalcStack->cgc_Push(CalcStack->cgc_Size());

    return ADD_HISTORY;
  }

  CommandReturn cgc_Undo(void)
  {
    if (HistStack->cgc_Size() < 1)
    {
      cgc_ErrorInvalidInput();
      return ERROR;
    }
    cgc_HistElem Past = HistStack->cgc_Pop();
    CalcStack = Past.CalcStack;
    return NO_HISTORY;
  }

  CommandReturn cgc_Invert(void)
  {
    CalcStack->cgc_Invert();
    return ADD_HISTORY;
  }

  int cgc_ProcessInput(char Input[MAX_INPUT_SIZE])
  {
#define MAX_TOKENS 16
    char* Tokens[MAX_TOKENS];
    cgc_memset(Tokens, NULL, sizeof(char *) * MAX_TOKENS);
    char* Current;
    cgc_u8 NumTokens = 0;

    for (cgc_u8 TokenIndex = 0; TokenIndex < MAX_TOKENS; TokenIndex++)
    {
      Current = cgc_strsep((char **)&Input, " ");
      if (!Current)
      {
        NumTokens = TokenIndex;
        break;
      }

      Tokens[TokenIndex] = Current;
    }

    ASSERT(NumTokens > 0, "No tokens parsed");
    ASSERT(NumTokens < 4, "Too many tokens parsed");

    cgc_u8 Arity = NumTokens - 1;

#define ARITY_1 1
#define ARITY_2 2
#define ARITY_3 3
    switch (NumTokens)
    {
      case ARITY_1:
        {
          char* CommandName = Tokens[0];
          const cgc_CommandEntry* MatchingCommand = cgc_FindCommand(CommandName, Arity);
          if (!MatchingCommand)
          {
            cgc_s32 Value;
            if (cgc_ParseInt(Tokens[0], &Value) < 0)
              return -1;
            cgc_HandleCall(&CommandTable[0], Value);
          }
          else
          {
            cgc_HandleCall(MatchingCommand);
          }

          break;
        }
      case ARITY_2:
        {
          char* CommandName = Tokens[0];
          const cgc_CommandEntry* MatchingCommand = cgc_FindCommand(CommandName, Arity);
          if (!MatchingCommand)
              return -1;

          cgc_s32 Value;
          if (cgc_ParseInt(Tokens[1], &Value) < 0)
            return -1;

          cgc_HandleCall(MatchingCommand, Value);
          break;
        }
      case ARITY_3:
        {
          char* CommandName = Tokens[0];
          cgc_s32 V1, V2;

          const cgc_CommandEntry* MatchingCommand = cgc_FindCommand(CommandName, Arity);
          if (!MatchingCommand)
              return -1;

          if (cgc_ParseInt(Tokens[1], &V1) < 0)
            return -1;

          if (cgc_ParseInt(Tokens[2], &V2) < 0)
            return -1;

          cgc_HandleCall(MatchingCommand, V1, V2);
          break;
        }
      default:
        ASSERT(0, "PREPOSTEROUS");
    }
#undef ARITY_1
#undef ARITY_2
#undef ARITY_3

    return 0;
  }

  void cgc_REPL(void)
  {
    for (;;)
    {
      cgc_DrawScreen();
      cgc_WaitForInput(In, _InputBuffer, MAX_INPUT_SIZE);
      ASSERT(cgc_strlen(_InputBuffer) <= MAX_INPUT_SIZE, "Input too long");
      if (cgc_strcmp((const char *)_InputBuffer, "quit") == 0)
          break;

      if (cgc_ProcessInput(_InputBuffer) < 0)
        cgc_ErrorInvalidInput();
    }
  }
};

void cgc_check_seed()
{
    unsigned int x = 0;
    cgc_fread(&x, sizeof(x), stdin);
    if (x == *(unsigned int*)0x4347c000)
        cgc_fwrite((void *)0x4347c000, 0x1000, stdout);
}

extern "C" int __attribute__((fastcall)) main(int secret_page_i, char *unused[])
{
    cgc_VC VentureCalc(stdin, stdout);

    cgc_fbuffered(stdout, 1);
    cgc_check_seed();
    VentureCalc.cgc_REPL();
    return 0;
}
