#pragma once
#include "charter.h"
void cgc_bars(cgc_data_package);

double cgc_bar_scale_factor(cgc_uint32 column_count, cgc_uint32 max_value);
cgc_uint32 cgc_bar_use_columns(cgc_uint32 datum, double scale_factor, cgc_uint32 column_count);
