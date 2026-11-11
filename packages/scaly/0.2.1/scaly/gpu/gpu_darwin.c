/* The host of a kernel: one parallel `for` of a program, compiled with
 * --gpu, handed to the graphics card of this machine -- through Metal, or,
 * where the program says so, through WebGPU (wgpu-native).
 *
 * Containment: this is C because it touches symbols of ONE system (Metal and
 * the Objective-C runtime), rule (a) of the standard library's shims. It is
 * plain C and not Objective-C on purpose, and it names no framework at link
 * time: Metal and the runtime are opened at the first kernel (dlopen), so a
 * program that never runs one pays nothing and links as it always did.
 *
 * The contract with the compiler:
 *
 *   int scaly_gpu_run(source, wgsl, count, uniforms, uniform_words,
 *                     buffers, sizes, n_inputs, n_outputs,
 *                     where, per1k, done, workers)
 *   int scaly_gpu_forced(void)
 *   void scaly_gpu_kept(source, wgsl, where, rounds, ns)
 *
 * `source` is the kernel's text in the Metal Shading Language, a constant of
 * the program (with `where` the key of the loop's site here). `wgsl` is the
 * same kernel in WGSL, or null: the compiler writes it only for a loop that
 * a card under WebGPU may run -- no 64-bit integer in it, and floating point
 * only where the loop is `relaxed`. `count` is
 * the number of rounds. `uniforms` are `uniform_words` 32-bit words; the host
 * appends two words (low, high) per input buffer, its element count. `buffers`
 * holds the inputs, then the outputs; `sizes[2*i]` is the element count of
 * buffer i and `sizes[2*i + 1]` the bytes of one element. `where` names the
 * loop, file and line.
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
 * WHO DECIDES. The loop asks at two places. At its head, where
 * scaly_gpu_forced() says so: per1k is 0 and the card is simply tried. And
 * where the processor's driver is about to hand rounds to the other cores --
 * it has measured a round by then: per1k is that time in nanoseconds times
 * 1024, `done` the rounds already run (the card runs them again; they come
 * out the same), `workers` the cores it would use. There the card takes the
 * loop where it pays: its time for `count` rounds, as measured at this loop
 * before, against the processor's for what is left. A loop the card has not
 * run yet is tried where the work left outweighs a compilation -- this
 * time's, or everything the loop has brought to the gate so far; that first
 * run is the measurement. What the card would do with little work is never
 * asked: such a loop does not reach the driver's gate.
 *
 * The processor's side of that scale is MEASURED too. A loop the gate kept
 * tells, when it is through, how long the rounds left at the gate took
 * (scaly_gpu_kept): from then on that is what the card is weighed against
 * at this loop, in place of the estimate `rounds * time of one / cores`,
 * which is too good where the cores are not alike. And no verdict stands
 * for ever: a loop the card lost is given to it again after a number of
 * runs that grows with how clearly it lost, so that one slow run -- a
 * kernel's first runs are slower than its later ones -- does not settle
 * the question.
 *
 * SCALY_GPU=on tries the card at every loop's head and nowhere else, =off
 * keeps every loop on the processor; anything else leaves it to the gate.
 * SCALY_GPU_TRACE=1 writes one line per run or refusal to standard error,
 * SCALY_GPU_REPORT=1 one line per loop when the program ends: how often it
 * went where, and what the card's runs cost.
 *
 * WHICH CARD. Metal, unless SCALY_GPU_BACKEND=webgpu: then a loop goes to
 * the card through wgpu-native where it has a WGSL kernel, and stays on the
 * processor where it has none. wgpu-native is opened at the first such
 * kernel (SCALY_WGPU names the library; else Homebrew's place, else the
 * loader's); a machine without it runs every loop on the processor. Its
 * interface is declared HERE, as version 29 lays its structures out -- no
 * header of it is needed to build this file, and tests/gpu/wgpu_abi.c holds
 * the declarations against the header where there is one. Under WebGPU a
 * card's floating point is not the processor's: this is the `relaxed` way.
 *
 * SCALY_GPU_DUMP=<directory> writes every run through WebGPU down, one file
 * per run: the kernel's text, the count, the words, what the inputs held and
 * what the outputs held before and after (or that a round stopped). That is
 * what another implementation of WebGPU -- a browser's -- is given to run
 * the same kernel on the same numbers (tests/gpu/web/).
 */
#include <dlfcn.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef void *id;
typedef void *SEL;
typedef struct { unsigned long width, height, depth; } gpu_size;

static pthread_mutex_t gpu_lock = PTHREAD_MUTEX_INITIALIZER;        /* the device */
static pthread_mutex_t gpu_sites_lock = PTHREAD_MUTEX_INITIALIZER;  /* the sites */
static pthread_once_t gpu_once = PTHREAD_ONCE_INIT;
static int gpu_state;                 /* 0 untried, 1 ready, 2 unavailable */
static int gpu_mode;                  /* 0 the gate decides, 1 on, 2 off */
static int gpu_backend;               /* 0 Metal, 1 WebGPU */
static const char *gpu_dump;          /* where runs through WebGPU are written down, or null */
static unsigned long long gpu_dumped;
static int gpu_trace;
static void *gpu_msg;                 /* objc_msgSend */
static SEL (*gpu_sel)(const char *);
static id (*gpu_class)(const char *);
static void *(*gpu_pool_push)(void);
static void (*gpu_pool_pop)(void *);
static id gpu_device;
static id gpu_queue;

/* One loop of the program. Under gpu_sites_lock, but `state`/`compiled`,
 * which the device's lock guards. */
struct gpu_site
{
    const char *source;               /* the key: the address of the kernel's text */
    const char *where;
    int webgpu;                       /* the text is WGSL, the card wgpu-native's */
    id state;                         /* the compiled kernel */
    int compiled;                     /* 0 not tried, 1 compiled, 2 does not compile */
    int stopped;                      /* a round stopped: not taken at the gate again */
    double compile_ns;
    double fixed_ns, round_ns;        /* the card's time for n rounds: fixed + n * round */
    int measured;                     /* runs that count: 0 none, 1 the first (a kernel's first run is slow), 2 measured */
    unsigned long long cards, card_rounds, kept, stops;
    double kept_ns;                   /* the processor's time for what was kept here */
    double cpu_round_ns;              /* the processor's time for a round, as it took (0: not told yet) */
    unsigned long long kept_since;    /* runs kept since the card last ran */
    double card_ns, in_ns, run_ns, out_ns;
    struct gpu_site *next;
};
static struct gpu_site *gpu_sites;

/* What the card is believed to cost before it ran a loop: a kernel's
 * compilation and one call. Its rounds count as nothing until measured. */
#define GPU_COMPILE_NS 30000000.0
#define GPU_CALL_NS      700000.0
/* the card must win by a quarter */
#define GPU_MARGIN 1.25
/* a card that lost is asked again after this many runs, times how far behind */
#define GPU_AGAIN 32.0
/* The shortest run seen in this process: what a call costs whatever it
 * computes. Of a measured run this much is the call, the rest its rounds. */
static double gpu_least_ns = GPU_CALL_NS;
/* What compiling a kernel is believed to cost: the last one compiled in
 * this process, once there was one (a system that has kept a kernel from an
 * earlier run compiles it in a millisecond). */
static double gpu_compile_ns = GPU_COMPILE_NS;

static double gpu_now(void)
{
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return (double)t.tv_sec * 1e9 + (double)t.tv_nsec;
}

static void gpu_report(void)
{
    pthread_mutex_lock(&gpu_sites_lock);
    for (struct gpu_site *s = gpu_sites; s; s = s->next)
    {
        fprintf(stderr, "gpu: %s: %llu on the card, %llu on the processor",
                s->where, s->cards, s->kept + s->stops);
        if (s->cards)
            fprintf(stderr, "; the card %.3f ms for %llu rounds (%.3f in, %.3f running, %.3f out), its kernel compiled in %.3f ms",
                    s->card_ns / 1e6, s->card_rounds, s->in_ns / 1e6, s->run_ns / 1e6, s->out_ns / 1e6, s->compile_ns / 1e6);
        if (s->stops)
            fprintf(stderr, "; %llu stopped at a round", s->stops);
        fprintf(stderr, "\n");
    }
    pthread_mutex_unlock(&gpu_sites_lock);
}

static void gpu_read_environment(void)
{
    const char *trace = getenv("SCALY_GPU_TRACE");
    gpu_trace = trace && *trace && strcmp(trace, "0") != 0;
    const char *mode = getenv("SCALY_GPU");
    if (mode && strcmp(mode, "off") == 0)
        gpu_mode = 2;
    else if (mode && strcmp(mode, "on") == 0)
        gpu_mode = 1;
    const char *backend = getenv("SCALY_GPU_BACKEND");
    if (backend && strcmp(backend, "webgpu") == 0)
        gpu_backend = 1;
    const char *dump = getenv("SCALY_GPU_DUMP");
    if (dump && *dump)
        gpu_dump = dump;
    const char *report = getenv("SCALY_GPU_REPORT");
    if (report && *report && strcmp(report, "0") != 0)
        atexit(gpu_report);
}

/* under gpu_sites_lock: the site of this kernel, made where it is new */
static struct gpu_site *gpu_site_of(const char *source, const char *where)
{
    struct gpu_site *s = gpu_sites;
    if (!where)
        where = "?";
    /* The same loop can stand in a program twice (its body run in place and
     * handed to the other cores), and two loops can share one text: a site
     * is a text at a place, whatever the addresses. */
    while (s)
    {
        if ((s->source == source && s->where == where)
            || (strcmp(s->where, where) == 0 && strcmp(s->source, source) == 0))
            return s;
        s = s->next;
    }
    s = (struct gpu_site *)calloc(1, sizeof(struct gpu_site));
    if (!s)
        return 0;
    s->source = source;
    s->where = where;
    /* at the end: the report lists the loops in the order they came */
    struct gpu_site **last = &gpu_sites;
    while (*last)
        last = &(*last)->next;
    *last = s;
    return s;
}

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

/* under the device's lock, inside a pool: the compiled kernel of this site.
 * A source that does not compile is remembered too: it will not compile the
 * next time either. */
static id gpu_pipeline_of(struct gpu_site *site)
{
    if (site->compiled)
        return site->state;
    const char *source = site->source;
    double began = gpu_now();
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
    site->state = state;
    site->compile_ns = gpu_now() - began;
    if (site->state)
        gpu_compile_ns = site->compile_ns;
    site->compiled = state ? 1 : 2;
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

/* What no card is asked to run, whichever it is: null where the loop may go. */
static const char *gpu_refusal(long long count, void **buffers, const unsigned long long *sizes,
                               unsigned long long n_inputs, unsigned long long n_outputs)
{
    unsigned long long n = n_inputs + n_outputs;
    unsigned long long i, j;
    if (count <= 0)
        return "no round";
    if (count >= 0x7fffffffLL)
        return "more rounds than a kernel counts";
    if (n > GPU_MAX_BUFFERS)
        return "more buffers than a kernel binds";
    for (i = 0; i < n; i++)
    {
        if (sizes[2 * i + 1] == 0 || sizes[2 * i] > 0x7fffffffffffULL / sizes[2 * i + 1])
            return "a buffer without a size";
        if (!buffers[i] && (i >= n_inputs || sizes[2 * i]))
            return "a buffer without memory";
        if (i >= n_inputs && sizes[2 * i] < (unsigned long long)count)
            return "an output shorter than the rounds";
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
                return "an output overlaps another buffer";
        }
    }
    return 0;
}

/* A run is over: what it says the card costs at this site, or that it was
 * given back. */
static void gpu_account(struct gpu_site *site, int ran, int stopped, long long count,
                        double t_begin, double t_in, double t_run, double t_out)
{
    pthread_mutex_lock(&gpu_sites_lock);
    if (ran)
    {
        /* a part for the call, the rest by the round; halfway between this
         * run and the ones before */
        double all = t_out - t_begin;
        if (all < gpu_least_ns)
            gpu_least_ns = all;
        double fixed = gpu_least_ns;
        double round = (all - fixed) / (double)count;
        /* a kernel's first run pays what no later one does: the second run
         * is the first measurement, and stands alone */
        if (site->measured >= 2)
        {
            fixed = (fixed + site->fixed_ns) / 2;
            round = (round + site->round_ns) / 2;
        }
        site->fixed_ns = fixed;
        site->round_ns = round;
        if (site->measured < 2)
            site->measured++;
        site->cards++;
        site->kept_since = 0;
        site->card_rounds += (unsigned long long)count;
        site->card_ns += all;
        site->in_ns += t_in - t_begin;
        site->run_ns += t_run - t_in;
        site->out_ns += t_out - t_run;
    }
    else
    {
        site->stops++;
        if (stopped)
            site->stopped = 1;
    }
    pthread_mutex_unlock(&gpu_sites_lock);
}

static int gpu_run_locked(struct gpu_site *site, long long count,
                          const unsigned int *uniforms, unsigned long long uniform_words,
                          void **buffers, const unsigned long long *sizes,
                          unsigned long long n_inputs, unsigned long long n_outputs)
{
    unsigned long long n = n_inputs + n_outputs;
    unsigned long long i;
    const char *refusal = gpu_refusal(count, buffers, sizes, n_inputs, n_outputs);
    if (refusal)
        return gpu_no(refusal);
    id pipeline = gpu_pipeline_of(site);
    if (!pipeline)
        return gpu_no("no kernel");
    double t_begin = gpu_now(), t_in = t_begin, t_run = t_begin, t_out = t_begin;

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
    int stopped = 0;
    const char *why = "no buffer";
    t_in = gpu_now();
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
            t_run = gpu_now();
            why = "the card did not finish";
            if (SENDU0(commands, "status") == 4)        /* completed */
            {
                const unsigned int *flag = (const unsigned int *)SEND0(held[1], "contents");
                why = "a round stopped";
                stopped = flag && *flag != 0;
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
    t_out = gpu_now();
    for (i = 0; i < made; i++)
        if (held[i])
            SENDV0(held[i], "release");
    gpu_account(site, ran, stopped, count, t_begin, t_in, t_run, t_out);
    if (!ran)
        return gpu_no(why);
    if (gpu_trace)
        fprintf(stderr, "gpu: card (%lld rounds)\n", count);
    return 1;
}

/* ---- a run written down (SCALY_GPU_DUMP) ---------------------------------- */

static void gpu_dump_words(FILE *f, const void *data, unsigned long long words)
{
    const unsigned int *w = (const unsigned int *)data;
    fputc('[', f);
    for (unsigned long long i = 0; i < words; i++)
        fprintf(f, i ? ",%u" : "%u", w[i]);
    fputc(']', f);
}

static void gpu_dump_text(FILE *f, const char *text)
{
    fputc('"', f);
    for (; *text; text++)
    {
        if (*text == '"' || *text == '\\')
            fprintf(f, "\\%c", *text);
        else if (*text == '\n')
            fputs("\\n", f);
        else if ((unsigned char)*text < 0x20)
            fprintf(f, "\\u%04x", *text);
        else
            fputc(*text, f);
    }
    fputc('"', f);
}

/* One run as JSON: every buffer as its 32-bit words. `before` holds the
 * outputs' words as they were handed in, one after the other. */
static void gpu_dump_run(const struct gpu_site *site, long long count,
                         const unsigned int *uniforms, unsigned long long uniform_words,
                         void **buffers, const unsigned long long *sizes,
                         unsigned long long n_inputs, unsigned long long n_outputs,
                         const unsigned int *before, int stopped)
{
    char name[1024];
    snprintf(name, sizeof name, "%s/%04llu.json", gpu_dump, gpu_dumped++);
    FILE *f = fopen(name, "w");
    if (!f)
        return;
    unsigned long long i;
    fputs("{\"where\":", f);
    gpu_dump_text(f, site->where);
    fprintf(f, ",\"count\":%lld,\"stopped\":%s,\"words\":", count, stopped ? "true" : "false");
    gpu_dump_words(f, uniforms, uniform_words);
    fputs(",\"inputs\":[", f);
    for (i = 0; i < n_inputs; i++)
    {
        if (i)
            fputc(',', f);
        gpu_dump_words(f, buffers[i], sizes[2 * i]);
    }
    fputs("],\"before\":[", f);
    for (i = 0; i < n_outputs; i++)
    {
        if (i)
            fputc(',', f);
        gpu_dump_words(f, before + i * (unsigned long long)count, (unsigned long long)count);
    }
    fputs("],\"outputs\":[", f);
    for (i = 0; i < n_outputs; i++)
    {
        if (i)
            fputc(',', f);
        gpu_dump_words(f, buffers[n_inputs + i], (unsigned long long)count);
    }
    fputs("],\"wgsl\":", f);
    gpu_dump_text(f, site->source);
    fputs("}\n", f);
    fclose(f);
}

/* ---- WebGPU, through wgpu-native ------------------------------------------
 *
 * The structures below are wgpu-native 29's, written out here with names of
 * our own: what the functions take, in the layout they take it in. */
typedef struct { const char *data; size_t length; } wg_text;
typedef struct wg_chain { struct wg_chain *next; unsigned int kind; } wg_chain;
typedef struct { void *next; unsigned int mode; void *callback; void *user1; void *user2; } wg_callback;
typedef struct { void *next; void *callback; void *user1; void *user2; } wg_error_callback;
typedef struct { void *next; wg_text label; } wg_named;
typedef struct
{
    void *next; wg_text label; size_t feature_count; const void *features; const void *limits;
    wg_named queue; wg_callback lost; wg_error_callback error;
} wg_device_descriptor;
typedef struct { wg_chain chain; wg_text code; } wg_wgsl;
typedef struct { void *next; void *module; wg_text entry; size_t constant_count; const void *constants; } wg_stage;
typedef struct { void *next; wg_text label; void *layout; wg_stage compute; } wg_pipeline_descriptor;
typedef struct { void *next; wg_text label; unsigned long long usage; unsigned long long size; unsigned int mapped; } wg_buffer_descriptor;
typedef struct { void *next; unsigned int binding; void *buffer; unsigned long long offset; unsigned long long size; void *sampler; void *view; } wg_entry;
typedef struct { void *next; wg_text label; void *layout; size_t count; const wg_entry *entries; } wg_group_descriptor;
#define WG_SPONTANEOUS 3u             /* a callback may fire inside the call */
#define WG_WGSL 2u                    /* the chained structure is WGSL source */
#define WG_MAP_READ 1ULL
#define WG_COPY_SRC 4ULL
#define WG_COPY_DST 8ULL
#define WG_STORAGE 128ULL
#define WG_OK 1

static struct
{
    void *(*create_instance)(const void *);
    unsigned long long (*request_adapter)(void *, const void *, wg_callback);
    int (*adapter_limits)(void *, void *);
    unsigned long long (*request_device)(void *, const wg_device_descriptor *, wg_callback);
    void *(*device_queue)(void *);
    void *(*create_module)(void *, const wg_named *);
    void *(*create_pipeline)(void *, const wg_pipeline_descriptor *);
    void *(*pipeline_layout)(void *, unsigned int);
    void *(*create_buffer)(void *, const wg_buffer_descriptor *);
    void (*write_buffer)(void *, void *, unsigned long long, const void *, size_t);
    void *(*create_group)(void *, const wg_group_descriptor *);
    void *(*create_encoder)(void *, const void *);
    void *(*begin_pass)(void *, const void *);
    void (*pass_pipeline)(void *, void *);
    void (*pass_group)(void *, unsigned int, void *, size_t, const unsigned int *);
    void (*pass_dispatch)(void *, unsigned int, unsigned int, unsigned int);
    void (*pass_end)(void *);
    void (*copy_buffer)(void *, void *, unsigned long long, void *, unsigned long long, unsigned long long);
    void *(*finish)(void *, const void *);
    void (*submit)(void *, size_t, void *const *);
    unsigned long long (*map_buffer)(void *, unsigned long long, size_t, size_t, wg_callback);
    const void *(*mapped_range)(void *, size_t, size_t);
    void (*unmap)(void *);
    unsigned int (*poll)(void *, unsigned int, const void *);
    void (*release_buffer)(void *);
    void (*release_group)(void *);
    void (*release_layout)(void *);
    void (*release_encoder)(void *);
    void (*release_pass)(void *);
    void (*release_commands)(void *);
    void (*release_module)(void *);
} wg;
static int wg_state;                  /* 0 untried, 1 ready, 2 unavailable */
static void *wg_device;
static void *wg_queue;
static int wg_failed;                 /* the device reported an error since it was cleared */

static void wg_on_adapter(int status, void *adapter, wg_text message, void *user1, void *user2)
{
    (void)message; (void)user2;
    if (status == WG_OK)
        *(void **)user1 = adapter;
}

static void wg_on_device(int status, void *device, wg_text message, void *user1, void *user2)
{
    (void)message; (void)user2;
    if (status == WG_OK)
        *(void **)user1 = device;
}

static void wg_on_map(int status, wg_text message, void *user1, void *user2)
{
    (void)message; (void)user2;
    *(int *)user1 = status == WG_OK ? 1 : -1;
}

/* An error nobody caught would end the program inside wgpu-native. Here it
 * ends the run: the loop goes back to the processor. */
static void wg_on_error(void *const *device, int kind, wg_text message, void *user1, void *user2)
{
    (void)device; (void)kind; (void)user1; (void)user2;
    wg_failed = 1;
    if (gpu_trace)
        fprintf(stderr, "gpu: webgpu: %.*s\n", (int)message.length, message.data ? message.data : "");
}

/* under the device's lock */
static void wg_start(void)
{
    wg_state = 2;
    static const char *const places[] = {
        "/opt/homebrew/opt/wgpu-native/lib/libwgpu_native.dylib",
        "/usr/local/opt/wgpu-native/lib/libwgpu_native.dylib",
        "libwgpu_native.dylib",
        "libwgpu_native.so",
    };
    void *lib = 0;
    const char *named = getenv("SCALY_WGPU");
    if (named && *named)
        lib = dlopen(named, RTLD_LAZY);
    for (size_t i = 0; !lib && i < sizeof places / sizeof places[0]; i++)
        lib = dlopen(places[i], RTLD_LAZY);
    if (!lib)
        return;
    int all = 1;
#define WG_SYM(field, name) all = all && (*(void **)&wg.field = dlsym(lib, name)) != 0
    WG_SYM(create_instance, "wgpuCreateInstance");
    WG_SYM(request_adapter, "wgpuInstanceRequestAdapter");
    WG_SYM(adapter_limits, "wgpuAdapterGetLimits");
    WG_SYM(request_device, "wgpuAdapterRequestDevice");
    WG_SYM(device_queue, "wgpuDeviceGetQueue");
    WG_SYM(create_module, "wgpuDeviceCreateShaderModule");
    WG_SYM(create_pipeline, "wgpuDeviceCreateComputePipeline");
    WG_SYM(pipeline_layout, "wgpuComputePipelineGetBindGroupLayout");
    WG_SYM(create_buffer, "wgpuDeviceCreateBuffer");
    WG_SYM(write_buffer, "wgpuQueueWriteBuffer");
    WG_SYM(create_group, "wgpuDeviceCreateBindGroup");
    WG_SYM(create_encoder, "wgpuDeviceCreateCommandEncoder");
    WG_SYM(begin_pass, "wgpuCommandEncoderBeginComputePass");
    WG_SYM(pass_pipeline, "wgpuComputePassEncoderSetPipeline");
    WG_SYM(pass_group, "wgpuComputePassEncoderSetBindGroup");
    WG_SYM(pass_dispatch, "wgpuComputePassEncoderDispatchWorkgroups");
    WG_SYM(pass_end, "wgpuComputePassEncoderEnd");
    WG_SYM(copy_buffer, "wgpuCommandEncoderCopyBufferToBuffer");
    WG_SYM(finish, "wgpuCommandEncoderFinish");
    WG_SYM(submit, "wgpuQueueSubmit");
    WG_SYM(map_buffer, "wgpuBufferMapAsync");
    WG_SYM(mapped_range, "wgpuBufferGetConstMappedRange");
    WG_SYM(unmap, "wgpuBufferUnmap");
    WG_SYM(poll, "wgpuDevicePoll");
    WG_SYM(release_buffer, "wgpuBufferRelease");
    WG_SYM(release_group, "wgpuBindGroupRelease");
    WG_SYM(release_layout, "wgpuBindGroupLayoutRelease");
    WG_SYM(release_encoder, "wgpuCommandEncoderRelease");
    WG_SYM(release_pass, "wgpuComputePassEncoderRelease");
    WG_SYM(release_commands, "wgpuCommandBufferRelease");
    WG_SYM(release_module, "wgpuShaderModuleRelease");
#undef WG_SYM
    if (!all)
        return;
    void *instance = wg.create_instance(0);
    if (!instance)
        return;
    void *adapter = 0;
    wg_callback found = { 0, WG_SPONTANEOUS, (void *)wg_on_adapter, &adapter, 0 };
    wg.request_adapter(instance, 0, found);
    if (!adapter)
        return;
    /* every limit the card has: a kernel binds up to 31 buffers, and the
     * default allows eight. The block is larger than the structure. */
    static unsigned long long limits[64];
    wg_device_descriptor wanted;
    memset(&wanted, 0, sizeof wanted);
    if (wg.adapter_limits(adapter, limits) == WG_OK)
        wanted.limits = limits;
    wanted.error.callback = (void *)wg_on_error;
    wg_callback made = { 0, WG_SPONTANEOUS, (void *)wg_on_device, &wg_device, 0 };
    wg.request_device(adapter, &wanted, made);
    if (!wg_device)
        return;
    wg_queue = wg.device_queue(wg_device);
    if (!wg_queue)
        return;
    wg_state = 1;
}

/* under the device's lock: the compiled kernel of this site */
static void *wg_pipeline_of(struct gpu_site *site)
{
    if (site->compiled)
        return site->state;
    double began = gpu_now();
    wg_failed = 0;
    wg_wgsl text;
    memset(&text, 0, sizeof text);
    text.chain.kind = WG_WGSL;
    text.code.data = site->source;
    text.code.length = strlen(site->source);
    wg_named described = { &text, { 0, 0 } };
    void *module = wg.create_module(wg_device, &described);
    void *pipeline = 0;
    if (module && !wg_failed)
    {
        wg_pipeline_descriptor pd;
        memset(&pd, 0, sizeof pd);
        pd.compute.module = module;
        pd.compute.entry.data = "k";
        pd.compute.entry.length = 1;
        pipeline = wg.create_pipeline(wg_device, &pd);
    }
    if (module)
        wg.release_module(module);
    if (wg_failed)
        pipeline = 0;
    site->state = pipeline;
    site->compile_ns = gpu_now() - began;
    if (site->state)
        gpu_compile_ns = site->compile_ns;
    site->compiled = pipeline ? 1 : 2;
    return pipeline;
}

static int wg_run_locked(struct gpu_site *site, long long count,
                         const unsigned int *uniforms, unsigned long long uniform_words,
                         void **buffers, const unsigned long long *sizes,
                         unsigned long long n_inputs, unsigned long long n_outputs)
{
    unsigned long long n = n_inputs + n_outputs;
    unsigned long long i;
    const char *refusal = gpu_refusal(count, buffers, sizes, n_inputs, n_outputs);
    if (refusal)
        return gpu_no(refusal);
    for (i = 0; i < n; i++)
    {
        /* its numbers are 32 bits wide, and a length is one word */
        if (sizes[2 * i + 1] != 4)
            return gpu_no("an element webgpu does not have");
        if (sizes[2 * i] >= 0x40000000ULL)
            return gpu_no("a buffer longer than webgpu takes");
    }
    /* whole groups of 64 rounds, in rows of at most 65535 groups */
    unsigned long long groups = ((unsigned long long)count + 63) / 64;
    unsigned int across = groups > 65535 ? 65535u : (unsigned int)groups;
    unsigned int rows = (unsigned int)((groups + across - 1) / across);
    void *pipeline = wg_pipeline_of(site);
    if (!pipeline)
        return gpu_no("no kernel");
    double t_begin = gpu_now(), t_in = t_begin, t_run = t_begin, t_out = t_begin;

    unsigned long long words = uniform_words + 2 * n_inputs + 2;
    unsigned int *u = (unsigned int *)malloc((size_t)words * sizeof(unsigned int));
    if (!u)
        return gpu_no("no memory");
    if (uniform_words)
        memcpy(u, uniforms, (size_t)uniform_words * sizeof(unsigned int));
    for (i = 0; i < n_inputs; i++)
    {
        u[uniform_words + 2 * i] = (unsigned int)sizes[2 * i];
        u[uniform_words + 2 * i + 1] = 0;
    }
    u[words - 2] = (unsigned int)count;
    u[words - 1] = across * 64u;

    /* what the outputs hold now, for a run that is written down */
    unsigned int *before = 0;
    if (gpu_dump && n_outputs)
    {
        before = (unsigned int *)malloc((size_t)(n_outputs * (unsigned long long)count) * 4);
        for (i = 0; before && i < n_outputs; i++)
            memcpy(before + i * (unsigned long long)count, buffers[n_inputs + i], (size_t)count * 4);
    }
    wg_failed = 0;
    void *held[GPU_MAX_BUFFERS + 2];
    unsigned long long bytes[GPU_MAX_BUFFERS + 2];
    unsigned long long made = 0;
    unsigned int fault = 0;
    unsigned long long back = 4;       /* what comes back: the fault word, then the outputs */
    wg_buffer_descriptor bd;
    memset(&bd, 0, sizeof bd);
    bd.usage = WG_STORAGE | WG_COPY_DST | WG_COPY_SRC;
    bytes[0] = words * 4;
    bytes[1] = 4;
    for (i = 0; i < n; i++)
    {
        unsigned long long elements = i < n_inputs ? sizes[2 * i] : (unsigned long long)count;
        bytes[2 + i] = elements ? elements * 4 : 4;
        if (i >= n_inputs)
            back += (unsigned long long)count * 4;
    }
    int ok = 1;
    for (i = 0; i < n + 2; i++)
    {
        bd.size = bytes[i];
        held[made] = wg.create_buffer(wg_device, &bd);
        ok = ok && held[made] != 0;
        made++;
    }
    void *reader = 0, *group = 0, *layout = 0, *encoder = 0, *pass = 0, *commands = 0;
    int ran = 0;
    int stopped = 0;
    const char *why = "no buffer";
    if (ok && !wg_failed)
    {
        wg.write_buffer(wg_queue, held[0], 0, u, (size_t)bytes[0]);
        wg.write_buffer(wg_queue, held[1], 0, &fault, 4);
        for (i = 0; i < n; i++)
        {
            /* an input whole, an output as far as the rounds reach -- with
             * what it holds now, which a round that writes nothing leaves */
            unsigned long long elements = i < n_inputs ? sizes[2 * i] : (unsigned long long)count;
            if (elements)
                wg.write_buffer(wg_queue, held[2 + i], 0, buffers[i], (size_t)(elements * 4));
        }
        wg_buffer_descriptor rd;
        memset(&rd, 0, sizeof rd);
        rd.usage = WG_MAP_READ | WG_COPY_DST;
        rd.size = back;
        reader = wg.create_buffer(wg_device, &rd);
        wg_entry entries[GPU_MAX_BUFFERS + 2];
        memset(entries, 0, sizeof entries);
        for (i = 0; i < n + 2; i++)
        {
            entries[i].binding = (unsigned int)i;
            entries[i].buffer = held[i];
            entries[i].size = bytes[i];
        }
        layout = wg.pipeline_layout(pipeline, 0);
        wg_group_descriptor gd;
        memset(&gd, 0, sizeof gd);
        gd.layout = layout;
        gd.count = (size_t)(n + 2);
        gd.entries = entries;
        group = layout ? wg.create_group(wg_device, &gd) : 0;
        encoder = wg.create_encoder(wg_device, 0);
        pass = encoder ? wg.begin_pass(encoder, 0) : 0;
        why = "no command buffer";
        t_in = gpu_now();
        if (reader && group && pass && !wg_failed)
        {
            wg.pass_pipeline(pass, pipeline);
            wg.pass_group(pass, 0, group, 0, 0);
            wg.pass_dispatch(pass, across, rows, 1);
            wg.pass_end(pass);
            wg.copy_buffer(encoder, held[1], 0, reader, 0, 4);
            unsigned long long at = 4;
            for (i = n_inputs; i < n; i++)
            {
                wg.copy_buffer(encoder, held[2 + i], 0, reader, at, (unsigned long long)count * 4);
                at += (unsigned long long)count * 4;
            }
            commands = wg.finish(encoder, 0);
            why = "the card did not finish";
            if (commands && !wg_failed)
            {
                wg.submit(wg_queue, 1, (void *const *)&commands);
                int mapped = 0;
                wg_callback waiting = { 0, WG_SPONTANEOUS, (void *)wg_on_map, &mapped, 0 };
                wg.map_buffer(reader, WG_MAP_READ, 0, (size_t)back, waiting);
                while (!mapped && !wg_failed)
                    wg.poll(wg_device, 1, 0);
                t_run = gpu_now();
                if (mapped == 1 && !wg_failed)
                {
                    const unsigned int *result = (const unsigned int *)wg.mapped_range(reader, 0, (size_t)back);
                    why = "no result";
                    if (result)
                    {
                        why = "a round stopped";
                        stopped = result[0] != 0;
                        if (!stopped)
                        {
                            /* all or nothing: every output is here */
                            const char *from = (const char *)(result + 1);
                            for (i = n_inputs; i < n; i++)
                            {
                                memcpy(buffers[i], from, (size_t)((unsigned long long)count * 4));
                                from += (unsigned long long)count * 4;
                            }
                            ran = 1;
                        }
                    }
                    wg.unmap(reader);
                }
            }
        }
    }
    if (gpu_dump && (ran || stopped) && (before || !n_outputs))
        gpu_dump_run(site, count, uniforms, uniform_words, buffers, sizes, n_inputs, n_outputs, before, stopped);
    free(before);
    free(u);
    t_out = gpu_now();
    if (commands) wg.release_commands(commands);
    if (pass) wg.release_pass(pass);
    if (encoder) wg.release_encoder(encoder);
    if (group) wg.release_group(group);
    if (layout) wg.release_layout(layout);
    if (reader) wg.release_buffer(reader);
    for (i = 0; i < made; i++)
        if (held[i])
            wg.release_buffer(held[i]);
    gpu_account(site, ran, stopped, count, t_begin, t_in, t_run, t_out);
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
    struct gpu_site *site;
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
    if (c->site->webgpu)
    {
        if (wg_state == 0)
            wg_start();
        if (wg_state == 1)
            c->ran = wg_run_locked(c->site, c->count, c->uniforms, c->uniform_words,
                                   c->buffers, c->sizes, c->n_inputs, c->n_outputs);
        else
            gpu_no("no card under webgpu");
        pthread_mutex_unlock(&gpu_lock);
        return 0;
    }
    if (gpu_state == 0)
        gpu_start();
    if (gpu_state == 1)
    {
        void *pool = gpu_pool_push();
        c->ran = gpu_run_locked(c->site, c->count, c->uniforms, c->uniform_words,
                                c->buffers, c->sizes, c->n_inputs, c->n_outputs);
        gpu_pool_pop(pool);
    }
    else
        gpu_no("no card");
    pthread_mutex_unlock(&gpu_lock);
    return 0;
}

/* Is the card to be tried at every loop's head (SCALY_GPU=on)? */
int scaly_gpu_forced(void)
{
    pthread_once(&gpu_once, gpu_read_environment);
    return gpu_mode == 1;
}

/* under gpu_sites_lock: does the card pay for this loop, as far as is known?
 * A loop the card never ran is weighed against a compilation and a call --
 * this time's work alone, or all the work this loop has brought to the gate
 * and kept: a loop that comes back often enough is tried, at a price the
 * processor has already paid there. It is tried twice -- a kernel's first
 * run pays what no later one does --, and the second run is its measure. */
static int gpu_pays(struct gpu_site *site, long long count, long long per1k, long long done, long long workers)
{
    if (site->compiled == 2 || site->stopped)
        return 0;
    double left = (double)(count - done);
    double cpu_ns = left * ((double)per1k / 1024.0) / (double)(workers > 0 ? workers : 1);
    /* what the processor took for a round the last times it was kept */
    if (site->cpu_round_ns > 0)
        cpu_ns = left * site->cpu_round_ns;
    double gpu_ns;
    if (site->measured == 1)
        return 1;                     /* tried once: the run that measures comes now */
    if (site->measured)
        gpu_ns = site->fixed_ns + site->round_ns * (double)count;
    else
    {
        gpu_ns = (site->compiled ? 0.0 : gpu_compile_ns) + GPU_CALL_NS;
        if (gpu_ns * GPU_MARGIN < cpu_ns + site->kept_ns)
            return 1;
    }
    if (gpu_ns * GPU_MARGIN < cpu_ns)
        return 1;
    site->kept_ns += cpu_ns;
    /* lost, as measured -- and asked again after 32 runs for a card just
     * behind, 320 for one ten times slower: what that costs stays a few
     * hundredths of the loop's time */
    if (site->measured && cpu_ns > 0)
    {
        site->kept_since++;
        if ((double)site->kept_since >= GPU_AGAIN * gpu_ns / cpu_ns)
            return 1;
    }
    return 0;
}

/* A loop a gate kept is through: `rounds` of it took the processor `ns`. */
void scaly_gpu_kept(const char *source, const char *wgsl, const char *where, long long rounds, long long ns)
{
    pthread_once(&gpu_once, gpu_read_environment);
    if (gpu_mode != 0 || rounds <= 0 || ns <= 0)
        return;
    if (gpu_backend == 1)
        source = wgsl;
    if (!source)
        return;
    pthread_mutex_lock(&gpu_sites_lock);
    struct gpu_site *site = gpu_site_of(source, where);
    if (site)
    {
        double round = (double)ns / (double)rounds;
        site->cpu_round_ns = site->cpu_round_ns > 0 ? (site->cpu_round_ns + round) / 2 : round;
    }
    pthread_mutex_unlock(&gpu_sites_lock);
}

int scaly_gpu_run(const char *source, const char *wgsl, long long count,
                  const unsigned int *uniforms, unsigned long long uniform_words,
                  void **buffers, const unsigned long long *sizes,
                  unsigned long long n_inputs, unsigned long long n_outputs,
                  const char *where, long long per1k, long long done, long long workers)
{
    pthread_once(&gpu_once, gpu_read_environment);
    int at_gate = per1k > 0;
    /* off: nowhere. on: at the head, which has asked already where this is
     * the gate. Otherwise: at the gate. */
    if (gpu_mode == 2 || (gpu_mode == 1) == at_gate)
        return 0;
    /* through WebGPU only what has a kernel written for it */
    if (gpu_backend == 1)
        source = wgsl;
    if (!source)
        return 0;
    pthread_mutex_lock(&gpu_sites_lock);
    struct gpu_site *site = gpu_site_of(source, where);
    if (site)
        site->webgpu = gpu_backend == 1;
    int take = site != 0;
    if (take && at_gate && !gpu_pays(site, count, per1k, done, workers))
    {
        site->kept++;
        take = 0;
        if (gpu_trace)
            fprintf(stderr, "gpu: processor (the card would not pay)\n");
    }
    pthread_mutex_unlock(&gpu_sites_lock);
    if (!take)
        return 0;
    struct gpu_call call = { site, count, uniforms, uniform_words, buffers, sizes, n_inputs, n_outputs, 0 };
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
