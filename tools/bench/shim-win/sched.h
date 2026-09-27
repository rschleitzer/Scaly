/* <sched.h> for the Windows box: the Linux CPU-affinity calls spectralnorm #4
   uses, answered with the processor count Windows exports in
   NUMBER_OF_PROCESSORS (the Darwin twin is ../shim/linuxcompat.h). */
#pragma once
#include <stdlib.h>
typedef struct { unsigned long long bits; } cpu_set_t;
#define CPU_ZERO(s) ((s)->bits = 0)
#define CPU_ISSET(i, s) (((s)->bits >> (i)) & 1ull)
static inline int sched_getaffinity(int pid, unsigned long n, cpu_set_t *s)
{
    const char *e = getenv("NUMBER_OF_PROCESSORS");
    long c = e ? atol(e) : 1;
    (void)pid; (void)n;
    if (c < 1) c = 1;
    s->bits = (c >= 64) ? ~0ull : ((1ull << c) - 1);
    return 0;
}
