#pragma once
#include "charter.h"

void cgc_sparks(cgc_data_package);

double cgc_spark_divisor(cgc_uint32 max, 
                     cgc_uint32 min, 
                     cgc_uint32 sparkle_count);

cgc_uint32 cgc_spark_pick_index(cgc_uint32 datum,
                        cgc_uint32 min,
                        double divisor);
