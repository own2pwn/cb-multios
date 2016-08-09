#include <stdint.h>

typedef void (*cgc_func_output)(cgc_uint8_t byte);
typedef struct {
    cgc_func_output output;
    cgc_int8_t bits;
    cgc_uint8_t partial;
} cgc_bitwriter_t;

static void cgc_bitwriter_init(cgc_bitwriter_t *bw, cgc_func_output f)
{
    bw->output = f;
    bw->bits = 0;
    bw->partial = 0;
}
void cgc_bitwriter_output(cgc_bitwriter_t *bw, cgc_uint32_t value, cgc_int8_t bits);
void cgc_bitwriter_flush(cgc_bitwriter_t *bw, cgc_uint8_t bit);
