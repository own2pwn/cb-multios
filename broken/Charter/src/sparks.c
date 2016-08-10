#include <libcgc.h>
#include "libc.h"
#include "charter.h"
#include "sparks.h"
#include "data.h"
#include "block.h"

cgc_uint32* sparkles = NULL;
cgc_uint32 sparkle_count = 0;

void cgc_sparks(cgc_data_package data) {
  cgc_uint32 candidate_count;

  cgc_uint32 bar_buf[data.size];
  __block cgc_uint32* bar_buf_ptr = (cgc_uint32*)bar_buf;

  cgc_read(&candidate_count, sizeof(candidate_count));

  if ((candidate_count == 0 && sparkle_count == 0) ||
   (candidate_count > MAX_ALLOCATE_SIZE)) {
    _terminate(-1);
  }

  if (candidate_count != 0) {
    if (sparkles != NULL) {
      deallocate(sparkles, sparkle_count );
    }

    sparkle_count = candidate_count;

#ifdef PATCHED
    int alloc_error = allocate( (candidate_count + 1)*4, 0, (void**)&sparkles);
#else
    int alloc_error = allocate(candidate_count + 1, 0, (void**)&sparkles);
#endif

    if (alloc_error) _terminate(-1);

    for (cgc_uint32 cur = 0; cur < sparkle_count; cur++) {
      cgc_uint32 sparkle;
      cgc_read(&sparkle, sizeof(sparkle));
      sparkles[cur] = sparkle;
    }
  }

  __block cgc_uint32 min = UINT32_MAX;
  __block cgc_uint32 max = 0;

  cgc_each(data, ^(cgc_uint32 idx, cgc_uint32 datum){
      if (datum > max) max = datum;
      if (datum < min) min = datum;
    });

  if (min == max) {
    cgc_each(data, ^(cgc_uint32 cur, cgc_uint32 datum) {
        bar_buf_ptr[cur] = sparkles[sparkle_count - 1];
      });
  } else {
    double div = cgc_spark_divisor(max, min, sparkle_count);
    
    cgc_each(data, ^(cgc_uint32 cur, cgc_uint32 datum){
        cgc_uint32 idx = cgc_spark_pick_index(datum, min, div);
      
        bar_buf_ptr[cur] = sparkles[idx];
      });
  }

  cgc_transmit_all(STDOUT, (char*)(bar_buf), data.size * sizeof(cgc_uint32));

  return;  
}

double cgc_spark_divisor(cgc_uint32 max, 
                     cgc_uint32 min, 
                     cgc_uint32 sparkle_count) {
  return ((double)max - (double)min) / (sparkle_count - 1);
  }

cgc_uint32 cgc_spark_pick_index(cgc_uint32 datum,
                        cgc_uint32 min,
                        double divisor) {
  return (cgc_uint32)(((double)datum - min) / divisor);
}
