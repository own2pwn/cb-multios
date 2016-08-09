#pragma once

#define UINT8_MAX 0xff
typedef unsigned char cgc_uint8;
typedef   signed char cgc_sint8;

#define UINT16_MAX 0xffff
typedef unsigned short int cgc_uint16;
typedef   signed short int cgc_sint16;

#define UINT32_MAX 0xffffffff
typedef unsigned int cgc_uint32;
typedef   signed int cgc_sint32;

#define UINT64_MAX 0xffffffffffffffff
typedef unsigned long long cgc_uint64;
typedef   signed long long cgc_sint64;

typedef float  cgc_float32;
typedef double cgc_float64;

void cgc_types_check();
