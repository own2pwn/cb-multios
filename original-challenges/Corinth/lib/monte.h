#pragma once

#include "types.h"

#define RANDOM_PAGE_ADDRESS 0x4347C000
#define RANDOM_PAGE_LENGTH 0x1000

void cgc_monte_initialize();
cgc_float64 cgc_monte_initialize_step(cgc_float64 first, cgc_uint64 second);

void cgc_monte_adjust(cgc_float64 new_splatter, cgc_float64 new_scaler);

cgc_uint8 cgc_monte_happy();

cgc_float64 cgc_monte_state();
cgc_float64 cgc_monte_state_step(cgc_float64 running, cgc_float64 cur);

cgc_float64 cgc_monte_gen();
