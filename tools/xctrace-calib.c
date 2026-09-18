/* xctrace-calib.c — three workloads of known character, to calibrate the order of
   tools/xctrace-bottleneck.py's columns: `xctrace-calib 0` chases pointers through
   256 MB (back end / memory), `1` runs dependent ALU chains, `2` takes
   unpredictable branches into two noinline functions (front end + discarded).
   Build with `clang -O1`; at -O1 an if/else of two expressions becomes a csel,
   which is why the branch arms are calls. */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
__attribute__((noinline)) static uint64_t f1(uint64_t a, size_t i) { return a + i; }
__attribute__((noinline)) static uint64_t f2(uint64_t a, size_t i) { return a ^ (i * 3); }
int main(int argc, char **argv) {
    int mode = atoi(argv[1]);
    uint64_t acc = 0;
    if (mode == 0) {                       /* A: dependent pointer chase, 256 MB */
        size_t n = (256u << 20) / sizeof(size_t);
        size_t *next = malloc(n * sizeof(size_t));
        for (size_t i = 0; i < n; i++) next[i] = i;
        uint64_t r = 88172645463325252ull;
        for (size_t i = n - 1; i > 0; i--) { r ^= r << 13; r ^= r >> 7; r ^= r << 17; size_t j = r % (i + 1); size_t t = next[i]; next[i] = next[j]; next[j] = t; }
        size_t p = 0;
        for (long k = 0; k < 30000000; k++) p = next[p];
        acc = p;
    } else if (mode == 1) {                /* B: independent ALU work */
        uint64_t a = 1, b = 2, c = 3, d = 4;
        for (long k = 0; k < 1500000000; k++) { a = a * 3 + 1; b = b * 5 + 3; c ^= c << 1; d += a ^ b; }
        acc = a + b + c + d;
    } else {                               /* C: unpredictable branches */
        size_t n = 1 << 20;
        unsigned char *v = malloc(n);
        uint64_t r = 88172645463325252ull;
        for (size_t i = 0; i < n; i++) { r ^= r << 13; r ^= r >> 7; r ^= r << 17; v[i] = r & 1; }
        for (long rep = 0; rep < 300; rep++)
            for (size_t i = 0; i < n; i++) { if (v[i]) acc = f1(acc, i); else acc = f2(acc, i); }
    }
    printf("%llu\n", (unsigned long long)acc);
    return 0;
}
