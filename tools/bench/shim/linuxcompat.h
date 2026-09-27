/* The Linux CPU-affinity calls spectralnorm #4 uses, answered with the online
   CPU count (macOS has no sched_getaffinity). Force-included for that one
   program on Darwin by tools/bench/build.sh. */
#include <unistd.h>
typedef struct { unsigned long bits; } cpu_set_t;
#define CPU_ZERO(s) ((s)->bits = 0)
#define CPU_ISSET(i, s) (((s)->bits >> (i)) & 1ul)
static inline int sched_getaffinity(int pid, unsigned long n, cpu_set_t *s)
{ (void)pid; (void)n; long c = sysconf(_SC_NPROCESSORS_ONLN); s->bits = (c >= 64) ? ~0ul : ((1ul << c) - 1); return 0; }
