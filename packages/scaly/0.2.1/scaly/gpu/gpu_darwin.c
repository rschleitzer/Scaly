/* The Metal host of a kernel: one parallel `for` of a program, compiled with
 * --gpu, handed to the graphics card of this machine.
 *
 * Containment: this is C because it touches symbols of ONE system (Metal and
 * the Objective-C runtime), rule (a) of the standard library's shims. It is
 * plain C and not Objective-C on purpose, and it names no framework at link
 * time: Metal and the runtime are opened at the first kernel (dlopen), so a
 * program that never runs one pays nothing and links as it always did.
 *
 * The contract with the compiler:
 *
 *   int scaly_gpu_run(source, count, uniforms, uniform_words,
 *                     buffers, sizes, n_inputs, n_outputs)
 *
 * `source` is the kernel's text in the Metal Shading Language, a constant of
 * the program (its ADDRESS is the key of the compiled pipeline). `count` is
 * the number of rounds. `uniforms` are `uniform_words` 32-bit words; the host
 * appends two words (low, high) per input buffer, its element count. `buffers`
 * holds the inputs, then the outputs; `sizes[2*i]` is the element count of
 * buffer i and `sizes[2*i + 1]` the bytes of one element.
 *
 * Bindings: 0 the uniform words, 1 the fault word, 2.. the inputs, then the
 * outputs. The kernel's entry is `k`.
 *
 * The answer is 1 where the card ran every round and the outputs hold the
 * results, and 0 where it did not -- the caller then runs the loop on the
 * processor, unchanged. 0 is also the answer where a round met what the
 * processor would have stopped at (an index out of bounds, a division by
 * zero): the kernel sets the fault word, the results are thrown away and the
 * processor's loop traps as it always did.
 *
 * SCALY_GPU=off keeps every loop on the processor. SCALY_GPU_TRACE=1 writes
 * one line per call to standard error, what ran or why not.
 */
#include <dlfcn.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef void *id;
typedef void *SEL;
typedef struct { unsigned long width, height, depth; } gpu_size;

static pthread_mutex_t gpu_lock = PTHREAD_MUTEX_INITIALIZER;
static int gpu_state;                 /* 0 untried, 1 ready, 2 unavailable */
static int gpu_trace;
static void *gpu_msg;                 /* objc_msgSend */
static SEL (*gpu_sel)(const char *);
static id (*gpu_class)(const char *);
static void *(*gpu_pool_push)(void);
static void (*gpu_pool_pop)(void *);
static id gpu_device;
static id gpu_queue;

struct gpu_pipeline { const char *source; id state; struct gpu_pipeline *next; };
static struct gpu_pipeline *gpu_pipelines;

#define SEND0(o, s)             ((id (*)(id, SEL))gpu_msg)((o), gpu_sel(s))
#define SEND1(o, s, a)          ((id (*)(id, SEL, id))gpu_msg)((o), gpu_sel(s), (a))
#define SENDV0(o, s)            ((void (*)(id, SEL))gpu_msg)((o), gpu_sel(s))
#define SENDV1(o, s, a)         ((void (*)(id, SEL, id))gpu_msg)((o), gpu_sel(s), (a))
#define SENDVU(o, s, a)         ((void (*)(id, SEL, unsigned long))gpu_msg)((o), gpu_sel(s), (a))
#define SENDU0(o, s)            ((unsigned long (*)(id, SEL))gpu_msg)((o), gpu_sel(s))

static int gpu_no(const char *why)
{
    if (gpu_trace)
        fprintf(stderr, "gpu: processor (%s)\n", why);
    return 0;
}

static const char *gpu_error_text(id error)
{
    if (!error)
        return "no description";
    id text = SEND0(error, "localizedDescription");
    if (!text)
        return "no description";
    return ((const char *(*)(id, SEL))gpu_msg)(text, gpu_sel("UTF8String"));
}

/* under the lock */
static void gpu_start(void)
{
    gpu_state = 2;
    const char *trace = getenv("SCALY_GPU_TRACE");
    gpu_trace = trace && *trace && strcmp(trace, "0") != 0;
    const char *mode = getenv("SCALY_GPU");
    if (mode && strcmp(mode, "off") == 0)
        return;
    void *objc = dlopen("/usr/lib/libobjc.A.dylib", RTLD_LAZY);
    void *metal = dlopen("/System/Library/Frameworks/Metal.framework/Metal", RTLD_LAZY);
    if (!objc || !metal)
        return;
    gpu_msg = dlsym(objc, "objc_msgSend");
    gpu_sel = (SEL (*)(const char *))dlsym(objc, "sel_registerName");
    gpu_class = (id (*)(const char *))dlsym(objc, "objc_getClass");
    gpu_pool_push = (void *(*)(void))dlsym(objc, "objc_autoreleasePoolPush");
    gpu_pool_pop = (void (*)(void *))dlsym(objc, "objc_autoreleasePoolPop");
    id (*create)(void) = (id (*)(void))dlsym(metal, "MTLCreateSystemDefaultDevice");
    if (!gpu_msg || !gpu_sel || !gpu_class || !gpu_pool_push || !gpu_pool_pop || !create)
        return;
    gpu_device = create();
    if (!gpu_device)
        return;
    gpu_queue = SEND0(gpu_device, "newCommandQueue");
    if (!gpu_queue)
        return;
    gpu_state = 1;
}

/* under the lock, inside a pool: the compiled kernel of this source */
static id gpu_pipeline_of(const char *source)
{
    struct gpu_pipeline *p = gpu_pipelines;
    while (p)
    {
        if (p->source == source)
            return p->state;
        p = p->next;
    }
    id state = 0;
    id options = SEND0(SEND0(gpu_class("MTLCompileOptions"), "alloc"), "init");
    /* The processor's arithmetic, not the card's: no reordering, no fused
     * multiply-add, the precise functions. The source says the same in two
     * pragmas; either alone was measured to change a result. */
    int has_mode = ((int (*)(id, SEL, SEL))gpu_msg)(options, gpu_sel("respondsToSelector:"), gpu_sel("setMathMode:")) & 0xff;
    if (has_mode)
    {
        SENDVU(options, "setMathMode:", 0UL);                       /* safe */
        SENDVU(options, "setMathFloatingPointFunctions:", 1UL);     /* precise */
    }
    else
        ((void (*)(id, SEL, int))gpu_msg)(options, gpu_sel("setFastMathEnabled:"), 0);
    id text = ((id (*)(id, SEL, const char *))gpu_msg)(gpu_class("NSString"), gpu_sel("stringWithUTF8String:"), source);
    id error = 0;
    id library = ((id (*)(id, SEL, id, id, id *))gpu_msg)(gpu_device, gpu_sel("newLibraryWithSource:options:error:"), text, options, &error);
    SENDV0(options, "release");
    if (!library)
    {
        if (gpu_trace)
            fprintf(stderr, "gpu: the kernel does not compile: %s\n", gpu_error_text(error));
    }
    else
    {
        id name = ((id (*)(id, SEL, const char *))gpu_msg)(gpu_class("NSString"), gpu_sel("stringWithUTF8String:"), "k");
        id function = SEND1(library, "newFunctionWithName:", name);
        if (function)
        {
            error = 0;
            state = ((id (*)(id, SEL, id, id *))gpu_msg)(gpu_device, gpu_sel("newComputePipelineStateWithFunction:error:"), function, &error);
            if (!state && gpu_trace)
                fprintf(stderr, "gpu: no pipeline: %s\n", gpu_error_text(error));
            SENDV0(function, "release");
        }
        SENDV0(library, "release");
    }
    /* A source that does not compile is remembered too: it will not compile
     * the next time either. */
    p = (struct gpu_pipeline *)malloc(sizeof(struct gpu_pipeline));
    if (p)
    {
        p->source = source;
        p->state = state;
        p->next = gpu_pipelines;
        gpu_pipelines = p;
    }
    return state;
}

static id gpu_buffer(const void *bytes, unsigned long length)
{
    if (length == 0)
    {
        static const unsigned int nothing = 0;
        bytes = &nothing;
        length = 4;
    }
    return ((id (*)(id, SEL, const void *, unsigned long, unsigned long))gpu_msg)(
        gpu_device, gpu_sel("newBufferWithBytes:length:options:"), bytes, length, 0UL);
}

#define GPU_MAX_BUFFERS 29            /* 31 bindings, two are the host's */

static int gpu_run_locked(const char *source, long long count,
                          const unsigned int *uniforms, unsigned long long uniform_words,
                          void **buffers, const unsigned long long *sizes,
                          unsigned long long n_inputs, unsigned long long n_outputs)
{
    unsigned long long n = n_inputs + n_outputs;
    unsigned long long i, j;
    if (count <= 0)
        return gpu_no("no round");
    if (count >= 0x7fffffffLL)
        return gpu_no("more rounds than a kernel counts");
    if (n > GPU_MAX_BUFFERS)
        return gpu_no("more buffers than a kernel binds");
    for (i = 0; i < n; i++)
    {
        if (sizes[2 * i + 1] == 0 || sizes[2 * i] > 0x7fffffffffffULL / sizes[2 * i + 1])
            return gpu_no("a buffer without a size");
        if (i >= n_inputs && sizes[2 * i] < (unsigned long long)count)
            return gpu_no("an output shorter than the rounds");
    }
    /* What a round writes, no other round and no input may hold: the card
     * reads its inputs as they were before the loop. */
    for (i = n_inputs; i < n; i++)
    {
        const char *lo = (const char *)buffers[i];
        const char *hi = lo + (unsigned long long)count * sizes[2 * i + 1];
        for (j = 0; j < n; j++)
        {
            if (j == i)
                continue;
            const char *other_lo = (const char *)buffers[j];
            unsigned long long other_n = j < n_inputs ? sizes[2 * j] : (unsigned long long)count;
            const char *other_hi = other_lo + other_n * sizes[2 * j + 1];
            if (lo < other_hi && other_lo < hi)
                return gpu_no("an output overlaps another buffer");
        }
    }
    id pipeline = gpu_pipeline_of(source);
    if (!pipeline)
        return gpu_no("no kernel");

    unsigned long long words = uniform_words + 2 * n_inputs;
    unsigned int *u = (unsigned int *)malloc((size_t)(words ? words : 1) * sizeof(unsigned int));
    if (!u)
        return gpu_no("no memory");
    if (uniform_words)
        memcpy(u, uniforms, (size_t)uniform_words * sizeof(unsigned int));
    for (i = 0; i < n_inputs; i++)
    {
        u[uniform_words + 2 * i] = (unsigned int)(sizes[2 * i] & 0xffffffffULL);
        u[uniform_words + 2 * i + 1] = (unsigned int)(sizes[2 * i] >> 32);
    }
    id held[GPU_MAX_BUFFERS + 2];
    unsigned long long made = 0;
    int ok = 1;
    unsigned int fault = 0;
    held[made] = gpu_buffer(u, (unsigned long)(words * sizeof(unsigned int)));
    free(u);
    ok = ok && held[made] != 0;
    made++;
    held[made] = gpu_buffer(&fault, sizeof(fault));
    ok = ok && held[made] != 0;
    made++;
    for (i = 0; i < n; i++)
    {
        /* an input whole (an index may name any element), an output as far
         * as the rounds reach -- with what it holds now, which a round that
         * writes nothing leaves there */
        unsigned long long elements = i < n_inputs ? sizes[2 * i] : (unsigned long long)count;
        held[made] = gpu_buffer(buffers[i], (unsigned long)(elements * sizes[2 * i + 1]));
        ok = ok && held[made] != 0;
        made++;
    }
    int ran = 0;
    const char *why = "no buffer";
    if (ok)
    {
        id commands = SEND0(gpu_queue, "commandBuffer");
        id encoder = commands ? SEND0(commands, "computeCommandEncoder") : 0;
        why = "no command buffer";
        if (encoder)
        {
            SENDV1(encoder, "setComputePipelineState:", pipeline);
            for (i = 0; i < made; i++)
                ((void (*)(id, SEL, id, unsigned long, unsigned long))gpu_msg)(
                    encoder, gpu_sel("setBuffer:offset:atIndex:"), held[i], 0UL, (unsigned long)i);
            unsigned long width = SENDU0(pipeline, "threadExecutionWidth");
            if (width == 0)
                width = 1;
            gpu_size grid = { (unsigned long)count, 1, 1 };
            gpu_size group = { width, 1, 1 };
            ((void (*)(id, SEL, gpu_size, gpu_size))gpu_msg)(
                encoder, gpu_sel("dispatchThreads:threadsPerThreadgroup:"), grid, group);
            SENDV0(encoder, "endEncoding");
            SENDV0(commands, "commit");
            SENDV0(commands, "waitUntilCompleted");
            why = "the card did not finish";
            if (SENDU0(commands, "status") == 4)        /* completed */
            {
                const unsigned int *flag = (const unsigned int *)SEND0(held[1], "contents");
                why = "a round stopped";
                if (flag && *flag == 0)
                {
                    ran = 1;
                    for (i = n_inputs; i < n; i++)
                    {
                        const void *result = (const void *)SEND0(held[2 + i], "contents");
                        if (!result)
                            ran = 0;
                    }
                    why = "no result";
                    /* all or nothing: a result is copied only where every
                     * output can be */
                    for (i = n_inputs; ran && i < n; i++)
                        memcpy(buffers[i], (const void *)SEND0(held[2 + i], "contents"),
                               (size_t)((unsigned long long)count * sizes[2 * i + 1]));
                }
            }
        }
    }
    for (i = 0; i < made; i++)
        if (held[i])
            SENDV0(held[i], "release");
    if (!ran)
        return gpu_no(why);
    if (gpu_trace)
        fprintf(stderr, "gpu: card (%lld rounds)\n", count);
    return 1;
}

/* One call, handed to a thread of its own: Metal and the Objective-C runtime
 * want more stack than a fiber of the program has (the caller may be one --
 * a round of an enclosing parallel loop). The caller waits for it. */
struct gpu_call
{
    const char *source;
    long long count;
    const unsigned int *uniforms;
    unsigned long long uniform_words;
    void **buffers;
    const unsigned long long *sizes;
    unsigned long long n_inputs, n_outputs;
    int ran;
};

static void *gpu_call_run(void *arg)
{
    struct gpu_call *c = (struct gpu_call *)arg;
    pthread_mutex_lock(&gpu_lock);
    if (gpu_state == 0)
        gpu_start();
    if (gpu_state == 1)
    {
        void *pool = gpu_pool_push();
        c->ran = gpu_run_locked(c->source, c->count, c->uniforms, c->uniform_words,
                                c->buffers, c->sizes, c->n_inputs, c->n_outputs);
        gpu_pool_pop(pool);
    }
    else
        gpu_no("no card");
    pthread_mutex_unlock(&gpu_lock);
    return 0;
}

int scaly_gpu_run(const char *source, long long count,
                  const unsigned int *uniforms, unsigned long long uniform_words,
                  void **buffers, const unsigned long long *sizes,
                  unsigned long long n_inputs, unsigned long long n_outputs)
{
    struct gpu_call call = { source, count, uniforms, uniform_words, buffers, sizes, n_inputs, n_outputs, 0 };
    pthread_attr_t attr;
    pthread_t thread;
    if (pthread_attr_init(&attr) != 0)
        return 0;
    pthread_attr_setstacksize(&attr, 4UL << 20);
    int made = pthread_create(&thread, &attr, gpu_call_run, &call) == 0;
    pthread_attr_destroy(&attr);
    if (!made)
        return 0;
    pthread_join(thread, 0);
    return call.ran;
}
