#include <stdint.h>

void cgc_dct(const cgc_int8_t input[], cgc_int16_t output[], const cgc_uint8_t scaler[]);
void cgc_idct(const cgc_int16_t input[], cgc_int8_t output[], const cgc_uint8_t scaler[]);
