#ifndef TIME_H_
#define TIME_H_

typedef enum { M=1, T=2, W=4, R=8, F=16, S=32, U=64, H=128 } cgc_days_t;

typedef struct {
    char hour;
    char minute;
} cgc_time_t;

cgc_time_t cgc_time(char hour, char minute);
int cgc_timecmp(cgc_time_t t1, cgc_time_t t2);
int cgc_days_intersect(cgc_days_t d1, cgc_days_t d2);
void cgc_daystostr(cgc_days_t days, char *output);

#endif
