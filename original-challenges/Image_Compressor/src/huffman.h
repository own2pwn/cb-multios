#include <stdint.h>

typedef struct {
    cgc_uint8_t sizes[256];
    cgc_uint16_t codes[256];
} cgc_ht_t;

void cgc_huffman_decode_table(cgc_ht_t *output, const cgc_uint8_t input[]);
