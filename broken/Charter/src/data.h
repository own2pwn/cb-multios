#pragma once
#include "charter.h"
#include "block.h"

void cgc_each(cgc_data_package, void (^block)(cgc_uint32 index, cgc_uint32 value));

