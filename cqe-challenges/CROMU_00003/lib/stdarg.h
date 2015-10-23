/*

Author: Jason Williams <jdw@cromulence.com>

Copyright (c) 2014 Cromulence LLC

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

*/

#ifdef _WIN32
#pragma once

#ifndef _INC_STDARG
#define _INC_STDARG

#include <crtdefs.h>

#if !defined (_WIN32)
#error ERROR: Only Win32 target supported!
#endif  /* !defined (_WIN32) */

#include <vadefs.h>

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

#define va_start _crt_va_start
#define va_arg _crt_va_arg
#define va_end _crt_va_end

void _CRTIMP __cdecl _vacopy(_Out_ va_list *, _In_ va_list);
#define va_copy(apd, aps) _vacopy(&(apd), aps)

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif  /* _INC_STDARG */

#else

#ifndef __STDARG_H__
#define __STDARG_H__

typedef __builtin_va_list __gnuc_va_list;

#define va_start(v,l)   __builtin_va_start(v,l)
#define va_end(v)       __builtin_va_end(v)
#define va_arg(v,l)     __builtin_va_arg(v,l)

typedef __gnuc_va_list va_list;

#endif // __STDARG_H__
#endif
