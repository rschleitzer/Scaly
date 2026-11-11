/* The host of a kernel on wasm32: the page the program runs in.
 *
 * Containment: this is C because it names symbols of ONE system -- the
 * imports of the module "scaly_gpu", which only a WebAssembly host has --,
 * rule (a) of the standard library's shims. A program compiled with --gpu
 * for wasm32 carries the kernel of a parallel `for` in WGSL and calls the
 * same routines as everywhere (gpu_darwin.c beside this file says what they
 * mean); here they hand the question on to the host:
 *
 *   scaly_gpu.forced() -> i32           is the card tried at every loop's head?
 *   scaly_gpu.run(request) -> i32       1: the card ran every round
 *   scaly_gpu.kept(wgsl, where, rounds, ns)
 *
 * `request` is the address of a scaly_gpu_request in the program's memory:
 * every field 32 or 64 bits wide at a fixed place, so that a host written in
 * JavaScript reads it without knowing C. The buffers are the program's own
 * memory; the host copies the inputs out and, where the card ran, the
 * outputs in. WHO DECIDES is the host too -- whether the card is tried at
 * all, and at a gate whether it pays: it is the page that knows its card
 * and what a call to it costs.
 *
 * A host without a card answers 0 to all of it (the page host of
 * packages/scaly/<version>/web/wasi.mjs does unless it is given one, and
 * node's runner): the program then runs on the processor, as one built
 * without --gpu. What no card is asked to run is refused here, before the
 * host hears of it, in the native host's words.
 *
 * Only a program that names these routines imports the module: one built
 * without --gpu asks its host for nothing.
 */
#include <stdlib.h>

struct scaly_gpu_request
{
    const char *wgsl;                   /*  0 */
    const char *where;                  /*  4: the loop, file and line */
    const unsigned int *uniforms;       /*  8 */
    void **buffers;                     /* 12: the inputs, then the outputs */
    const unsigned long long *sizes;    /* 16: per buffer its element count, then the bytes of one */
    unsigned int uniform_words;         /* 20 */
    unsigned int n_inputs;              /* 24 */
    unsigned int n_outputs;             /* 28 */
    long long count;                    /* 32 */
    long long per1k;                    /* 40: a round's time on the processor, ns times 1024; 0 at the head */
    long long done;                     /* 48 */
    long long workers;                  /* 56 */
};

__attribute__((import_module("scaly_gpu"), import_name("forced")))
extern int scaly_gpu_page_forced(void);
__attribute__((import_module("scaly_gpu"), import_name("run")))
extern int scaly_gpu_page_run(const struct scaly_gpu_request *request);
__attribute__((import_module("scaly_gpu"), import_name("kept")))
extern void scaly_gpu_page_kept(const char *wgsl, const char *where, double rounds, double ns);

#define GPU_MAX_BUFFERS 29            /* 31 bindings, two are the host's */

/* What no card under WebGPU is asked to run: 0 where the loop may go. */
static int gpu_refused(long long count, void **buffers, const unsigned long long *sizes,
                       unsigned long long n_inputs, unsigned long long n_outputs)
{
    unsigned long long n = n_inputs + n_outputs;
    unsigned long long i, j;
    if (count <= 0 || count >= 0x7fffffffLL || n > GPU_MAX_BUFFERS)
        return 1;
    for (i = 0; i < n; i++)
    {
        /* its numbers are 32 bits wide, and a length is one word */
        if (sizes[2 * i + 1] != 4 || sizes[2 * i] >= 0x40000000ULL)
            return 1;
        if (!buffers[i] && (i >= n_inputs || sizes[2 * i]))
            return 1;
        if (i >= n_inputs && sizes[2 * i] < (unsigned long long)count)
            return 1;
    }
    /* What a round writes, no other round and no input may hold: the card
     * reads its inputs as they were before the loop. */
    for (i = n_inputs; i < n; i++)
    {
        const char *lo = (const char *)buffers[i];
        const char *hi = lo + (unsigned long long)count * 4;
        for (j = 0; j < n; j++)
        {
            if (j == i)
                continue;
            const char *other_lo = (const char *)buffers[j];
            unsigned long long other_n = j < n_inputs ? sizes[2 * j] : (unsigned long long)count;
            const char *other_hi = other_lo + other_n * 4;
            if (lo < other_hi && other_lo < hi)
                return 1;
        }
    }
    return 0;
}

int scaly_gpu_forced(void)
{
    return scaly_gpu_page_forced();
}

int scaly_gpu_run(const char *source, const char *wgsl, long long count,
                  const unsigned int *uniforms, unsigned long long uniform_words,
                  void **buffers, const unsigned long long *sizes,
                  unsigned long long n_inputs, unsigned long long n_outputs,
                  const char *where, long long per1k, long long done, long long workers)
{
    (void)source;                       /* Metal's text: no page reads it */
    if (!wgsl || gpu_refused(count, buffers, sizes, n_inputs, n_outputs))
        return 0;
    struct scaly_gpu_request request = {
        wgsl, where, uniforms, buffers, sizes,
        (unsigned int)uniform_words, (unsigned int)n_inputs, (unsigned int)n_outputs,
        count, per1k, done, workers,
    };
    return scaly_gpu_page_run(&request) == 1;
}

void scaly_gpu_kept(const char *source, const char *wgsl, const char *where, long long rounds, long long ns)
{
    (void)source;
    if (wgsl && rounds > 0 && ns > 0)
        scaly_gpu_page_kept(wgsl, where, (double)rounds, (double)ns);
}

/* The block a loop's reductions are handed back in, and its release: a pair,
 * because what takes memory must give it back on the same side. */
void *scaly_gpu_parts(long long bytes)
{
    return bytes > 0 && bytes < 0x7fffffffLL ? malloc((size_t)bytes) : 0;
}

void scaly_gpu_parts_free(void *parts)
{
    free(parts);
}
