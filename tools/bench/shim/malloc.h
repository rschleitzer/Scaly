/* <malloc.h> with memalign, which macOS lacks (spectralnorm #4 and #5 include
   it). On the include path on Darwin only: glibc has the real one. */
#include <stdlib.h>
static inline void *memalign(size_t a, size_t n) { void *p = 0; return posix_memalign(&p, a, n) ? 0 : p; }
