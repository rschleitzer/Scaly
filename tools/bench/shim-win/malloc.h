/* <malloc.h> with memalign for the Windows box: spectralnorm #5 asks for
   16-byte-aligned blocks and releases them with free(). On x64 the UCRT's
   malloc already returns 16-byte-aligned blocks, and _aligned_malloc would
   need _aligned_free — so a larger alignment is refused loudly instead. */
#pragma once
#include_next <malloc.h>
#include <stdio.h>
#include <stdlib.h>
static inline void *memalign(size_t a, size_t n)
{
    if (a > 16) { fputs("memalign shim: only 16-byte alignment\n", stderr); abort(); }
    return malloc(n);
}
