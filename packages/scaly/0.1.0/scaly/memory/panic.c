/* Catch points for a runtime panic (TRAPS.md 0, 5.6, 5.7).
 *
 * Why this is C and not Scaly, by the containment rule in CLAUDE.md:
 *   (a) `jmp_buf` is an OS- and ARCH-specific struct layout, and the committed
 *       seed ships ONE scaly.ll for every target, so its size cannot be a
 *       Scaly declaration.
 *   And a second reason that is not in the rule's list because nothing before
 *   needed it: ★★★`setjmp` CANNOT BE WRAPPED. A context whose function has
 *   already returned is invalid, so the call must stand in the frame of the
 *   catcher itself — a library function that did `setjmp` and returned would
 *   hand out a context that is already dead. That is why scaly_catch_run takes
 *   the guarded work as a CALLBACK: the catcher is this function.
 *
 * What it gives the programmer, which nothing gave before: an out-of-bounds
 * index ended the PROCESS (`exit 15`, eight sites in the containers). With a
 * catch point on the stack the panic ends the guarded CALL instead, and the
 * caller decides what that means. A long-running process — the server this is
 * for — loses one request rather than itself.
 *
 * ★No `long` crosses this boundary (the LLP64 rule): `long long` for values,
 * `size_t` for counts. `__thread` matches the spelling eio.c already uses.
 *
 * ★★It must land in the SCALYC BINARY, not only in libscaly.a, and that is
 * why panic.o stands EXPLICITLY on every scalyc/scalyls link line
 * (build-from-seed.sh, seed.sh, verify-seed.sh, install.sh, tests/lsp/run.sh)
 * and in every archive: a `--jit` run resolves externs out of the HOST
 * PROCESS, where an undefined extern becomes a silently 0-returning stub
 * (tests/dazzle/COMPLETENESS.md carries the account for the ctime shim). For
 * scaly_panic_jump a 0 means "no catch point installed", so the failure mode
 * would not be a crash but something worse: catching would silently never
 * work under --jit, and every panic would end the process as if nobody had
 * asked to catch it.
 *
 * ★★★UNPROVEN ON WINDOWS. Every other target is exercised by the suites, but
 * MSVC's setjmp/longjmp interacts with SEH unwinding, and tools/win-undef.sh
 * reads symbols with grep rather than linking. Rung 3 of the Windows roadmap
 * is what would prove it; until then treat a Windows panic catch as untested.
 */
#include <setjmp.h>
#include <stddef.h>
#include <stdlib.h>   /* malloc/free for the emitter-side frames */

/* ★★★THE CONTRACT WITH scaly/memory/runtime.scaly: these are the variant TAGS
 * of the prelude's `RuntimeFault` union, and a tag is a variant's POSITION in
 * that declaration. Appending a kind means appending it THERE and here in the
 * same order; getting them out of step would make a `when` arm bind another
 * kind's payload, and nothing would say a word. Both files carry this note. */
#define SCALY_FAULT_OUT_OF_BOUNDS   0
#define SCALY_FAULT_NULL_REFERENCE  1
#define SCALY_FAULT_INVALID_REGION  2
#define SCALY_FAULT_SIZE_OVERFLOW   3
#define SCALY_FAULT_OUT_OF_MEMORY   4
#define SCALY_FAULT_RESOURCE        5
#define SCALY_FAULT_DEADLOCK        6

/* The region-stack watermark and the unwind live in Scaly
 * (scaly/memory/root_pages.scaly). They are reached through function POINTERS
 * that the Scaly side passes in, deliberately: writing their Itanium mangled
 * names here would freeze their signatures the way CLAUDE.md warns about, and
 * the eight symbols that already carry that cost are enough. */
typedef void *(*scaly_mark_fn)(void);
typedef void (*scaly_unwind_fn)(void *);
typedef long long (*scaly_body_fn)(void *);

typedef struct scaly_catch_frame {
    jmp_buf buf;
    struct scaly_catch_frame *prev;
    void *mark;                    /* region watermark at entry */
    scaly_unwind_fn unwind;
    const char *what;              /* the panic value, allocation-free: */
    size_t index, length;          /* everything between here and the trap */
    int kind;                      /* which RuntimeFault variant it is */
    int reserve_slot;              /* >=0: a reserve slot, not malloc'd */
} scaly_catch_frame;               /* has been given back by then */

static __thread scaly_catch_frame *scaly_catch_top = 0;
static __thread const char *scaly_last_what = 0;
static __thread size_t scaly_last_index = 0;
static __thread size_t scaly_last_length = 0;
static __thread int scaly_last_kind_v = 0;

/* Run `body(arg)` with a catch point installed. On a panic inside, the region
 * stack is popped back to the watermark and *caught is set; the panic's own
 * detail is then read with the three accessors below.
 *
 * ★The chain is unlinked BEFORE the unwind runs, so a panic raised by the
 * unwind itself cannot land back in this frame. */
long long scaly_catch_run(scaly_body_fn body, void *arg,
                          scaly_mark_fn mark, scaly_unwind_fn unwind,
                          int *caught)
{
    scaly_catch_frame f;
    f.prev = scaly_catch_top;
    f.mark = mark ? mark() : 0;
    f.unwind = unwind;
    f.what = 0; f.index = 0; f.length = 0; f.kind = 0; f.reserve_slot = -1;

    if (setjmp(f.buf) == 0) {
        scaly_catch_top = &f;
        long long r = body(arg);
        scaly_catch_top = f.prev;
        if (caught) *caught = 0;
        return r;
    }

    scaly_catch_top = f.prev;
    if (f.unwind) f.unwind(f.mark);
    scaly_last_what = f.what;
    scaly_last_index = f.index;
    scaly_last_length = f.length;
    scaly_last_kind_v = f.kind;
    if (caught) *caught = 1;
    return 0;
}

/* Raised by the panic helper in root_pages.scaly. Answers 0 when NO catch
 * point is installed, and then the caller ends the process as before — the
 * behaviour of every program that does not ask to catch anything stays
 * exactly what it was. */
int scaly_panic_jump(const char *what, size_t index, size_t length)
{
    scaly_catch_frame *t = scaly_catch_top;
    if (!t)
        return 0;
    t->what = what; t->index = index; t->length = length;
    t->kind = SCALY_FAULT_OUT_OF_BOUNDS;
    longjmp(t->buf, 1);
    return 0;                      /* unreachable; longjmp is noreturn */
}

/* The null twin, raised by scaly_panic_null in root_pages.scaly: `x as ref[T]`
 * met a null. Same answer discipline — 0 when no catch point is installed, so
 * a program that catches nothing ends exactly as it did.
 *
 * `where` is the enclosing symbol's MANGLED name, because a run has no source
 * location. It is a static string in the module that raised, so it outlives
 * every region this jump unwinds — which is what lets the payload carry it. */
int scaly_panic_null_jump(const char *where)
{
    scaly_catch_frame *t = scaly_catch_top;
    if (!t)
        return 0;
    t->what = where; t->index = 0; t->length = 0;
    t->kind = SCALY_FAULT_NULL_REFERENCE;
    longjmp(t->buf, 1);
    return 0;                      /* unreachable; longjmp is noreturn */
}

/* A received region did not hold together: a pointer inside it maps to
 * nothing, or the sender's own page walk disagreed with itself
 * (scaly/memory/Region.scaly). Raised through scaly_panic_region.
 *
 * ★It carries the offending ADDRESS in the frame's `index` slot rather than
 * adding a field: `index` and `length` are the two words a fault may carry,
 * and this kind uses one of them. The payload record's second field is that
 * address, so a catcher can print the pointer the walk choked on — which is
 * what tells a wrong field map (a walker bug) from a truncated frame.
 *
 * ★★★It does NOT free anything. Cleanup belongs to whoever owns the
 * half-built region: Region.deserialize frees through its own page TABLE
 * before raising (the links are half-swizzled at that point, so walking them
 * would follow sender addresses), and a fault raised from a generated walker
 * during `fix_graph` leaves a fully built region whose owner holds `rr` and
 * must call `rr.release()` in the catch arm. A jump cannot know which of the
 * two it is. */
int scaly_panic_region_jump(const char *what, size_t address)
{
    scaly_catch_frame *t = scaly_catch_top;
    if (!t)
        return 0;
    t->what = what; t->index = address; t->length = 0;
    t->kind = SCALY_FAULT_INVALID_REGION;
    longjmp(t->buf, 1);
    return 0;                      /* unreachable; longjmp is noreturn */
}

/* A size computation wrapped around: a container was asked to grow by an
 * amount that makes its new length smaller than its old one
 * (scaly/containers/Array.scaly). Raised through scaly_panic_size.
 *
 * ★It is the guard BEFORE the corruption, not after: the wrapped length would
 * under-size the allocation and the following memcpy would write past it. The
 * two words it carries are the addition and the current length — the wrapped
 * sum itself says nothing. */
int scaly_panic_size_jump(const char *what, size_t added, size_t length)
{
    scaly_catch_frame *t = scaly_catch_top;
    if (!t)
        return 0;
    t->what = what; t->index = added; t->length = length;
    t->kind = SCALY_FAULT_SIZE_OVERFLOW;
    longjmp(t->buf, 1);
    return 0;                      /* unreachable; longjmp is noreturn */
}

/* The allocator could not get memory (scaly/memory/{Page,root_pages}.scaly,
 * and Region.deserialize for a received oversized page). Raised through
 * scaly_panic_oom.
 *
 * ★★★This is the one kind where catching does more than CONTAIN the damage:
 * dropping a region gives the memory back, so a catcher can actually recover
 * and carry on. Which is also why the reserve below exists -- a catch point
 * that cannot be installed for want of memory would make this kind
 * uncatchable exactly when it fires.
 *
 * `bytes` is what the failed request asked for. */
int scaly_panic_oom_jump(const char *what, size_t bytes)
{
    scaly_catch_frame *t = scaly_catch_top;
    if (!t)
        return 0;
    t->what = what; t->index = bytes; t->length = 0;
    t->kind = SCALY_FAULT_OUT_OF_MEMORY;
    longjmp(t->buf, 1);
    return 0;                      /* unreachable; longjmp is noreturn */
}

/* An OS resource the runtime asked for was refused: a fiber stack (mmap), its
 * guard page, a thread, a pipe, or one of the node's fixed tables
 * (scaly/fiber.scaly, scaly/cluster.scaly).
 *
 * ★★★Unlike the four kinds before it, these sites keep THEIR OWN exit codes
 * (103, 104, 109, 111, 112, 113) and only gain the jump. The earlier
 * consolidations collapsed several codes that meant ONE condition; here the
 * conditions are different and the code is the only thing that says which --
 * and `tests/fiber/channel_deadlock.scaly` reads one of them
 * (`ExpectedExit: 106`). A code that carries information is not noise.
 *
 * `requested` is what was asked for where that is a number (stack bytes, the
 * table's limit), 0 where it is not (a pipe, a thread). */
int scaly_panic_resource_jump(const char *what, size_t requested)
{
    scaly_catch_frame *t = scaly_catch_top;
    if (!t)
        return 0;
    t->what = what; t->index = requested; t->length = 0;
    t->kind = SCALY_FAULT_RESOURCE;
    longjmp(t->buf, 1);
    return 0;                      /* unreachable; longjmp is noreturn */
}

/* Nothing can make progress: every fiber of this scheduler is blocked, or a
 * join waits on a step that cannot happen (scaly/fiber.scaly, exit 106).
 *
 * ★It is a PROGRAM error and not a substrate failure, which is why it is
 * catchable while a failed `munmap` (105) or a poll syscall (107) is not:
 * those say the runtime's own bookkeeping or the OS is broken, and there is
 * nothing a catcher could sensibly do. A deadlock says THIS unit of work is
 * stuck -- exactly the boundary a server wants to lose. */
int scaly_panic_deadlock_jump(const char *what)
{
    scaly_catch_frame *t = scaly_catch_top;
    if (!t)
        return 0;
    t->what = what; t->index = 0; t->length = 0;
    t->kind = SCALY_FAULT_DEADLOCK;
    longjmp(t->buf, 1);
    return 0;                      /* unreachable; longjmp is noreturn */
}

/* --- the three halves a `try` needs, for the EMITTER ---------------------
 *
 * scaly_catch_run above is the library form: it owns the catcher, so the frame
 * can live on its C stack. A `try` in Scaly source cannot work that way —
 * the catcher is the user's function, and the emitter has to call `setjmp`
 * THERE (a context whose function returned is dead). But the emitter must not
 * need to know how big a `jmp_buf` is, or the OS-specific layout would leak
 * into the compiler and from there into the ONE committed seed.
 *
 * So the split: push allocates the frame HERE and hands back a pointer to its
 * buffer; the emitter calls `setjmp` on that pointer in the catcher's own
 * frame; pop leaves on the success path; caught runs the unwind on the jump
 * path. The emitted shape is
 *
 *     %buf = call ptr @scaly_catch_push(ptr @mark, ptr @unwind)
 *     %sj  = call i32 @setjmp(ptr %buf)        ; returns_twice
 *     br i1 (%sj == 0), label %try.body, label %try.catch
 *
 * ★The frame is malloc'd rather than stack-allocated, and that is a
 * deliberate first cut: a dynamic alloca of scaly_catch_frame_size() would be
 * free but must then sit in the function's ENTRY block, or a `try` inside a
 * loop would grow the stack every iteration. One malloc per guarded region is
 * paid only where somebody catches; a `try` on a hot path is its own topic.
 */
/* ★★★A RESERVE of frames per thread, and it is used FIRST -- malloc is the
 * fallback for deeper nesting, not the other way round. Two reasons, and the
 * second one is why the order matters:
 *
 *   1. The out-of-memory kind (SCALY_FAULT_OUT_OF_MEMORY) would be
 *      uncatchable at the one moment it fires if arming a `try` needed the
 *      allocator that just failed.
 *   2. Reserve-first means EVERY `try` in the tree exercises this path, so it
 *      is covered by every catch gate. A defensive path that only runs when
 *      memory is short is a path no test ever takes.
 *
 * ★Slots carry a BUSY FLAG and the frame remembers its slot INDEX, rather
 * than a used-counter: with fibers the pops are not globally LIFO (fiber A
 * can pop while main still holds a later slot), and a counter would hand the
 * same slot out twice. The chain itself is per fiber (Fiber.swap_allocator);
 * this array is per thread, which is why it must not assume an order.
 *
 * ★The residual is stated rather than hidden: past the reserve AND with
 * malloc failing, push still answers 0, and the emitted `setjmp` does not
 * check it. Closing that needs a conditional pop in the Emitter (or the
 * dynamic alloca the neighbouring comment describes); it is not closed. */
#define SCALY_CATCH_RESERVE 4
static __thread scaly_catch_frame scaly_reserve[SCALY_CATCH_RESERVE];
static __thread unsigned char scaly_reserve_busy[SCALY_CATCH_RESERVE];

void *scaly_catch_push(scaly_mark_fn mark, scaly_unwind_fn unwind)
{
    scaly_catch_frame *f = 0;
    int slot = -1;
    for (int i = 0; i < SCALY_CATCH_RESERVE; i++) {
        if (!scaly_reserve_busy[i]) {
            scaly_reserve_busy[i] = 1;
            f = &scaly_reserve[i];
            slot = i;
            break;
        }
    }
    if (!f)
        f = (scaly_catch_frame *)malloc(sizeof *f);
    if (!f)
        return 0;                  /* the emitted setjmp on a null pointer
                                    * would fault, so the caller checks */
    f->reserve_slot = slot;
    f->prev = scaly_catch_top;
    f->mark = mark ? mark() : 0;
    f->unwind = unwind;
    f->what = 0; f->index = 0; f->length = 0; f->kind = 0;
    scaly_catch_top = f;
    return (void *)&f->buf;
}

/* Success path: the guarded work returned normally. */
void scaly_catch_pop(void)
{
    scaly_catch_frame *f = scaly_catch_top;
    if (!f)
        return;
    scaly_catch_top = f->prev;
    if (f->reserve_slot >= 0)
        scaly_reserve_busy[f->reserve_slot] = 0;
    else
        free(f);
}

/* Jump path: unlink FIRST (so a panic raised by the unwind cannot land back
 * in this frame), then give the regions back, then keep the detail where the
 * accessors can read it. */
void scaly_catch_caught(void)
{
    scaly_catch_frame *f = scaly_catch_top;
    if (!f)
        return;
    scaly_catch_top = f->prev;
    if (f->unwind)
        f->unwind(f->mark);
    scaly_last_what = f->what;
    scaly_last_index = f->index;
    scaly_last_length = f->length;
    scaly_last_kind_v = f->kind;   /* the arm's switch reads this */
    if (f->reserve_slot >= 0)
        scaly_reserve_busy[f->reserve_slot] = 0;
    else
        free(f);
}

/* --- what a `when` arm needs ---------------------------------------------
 *
 * The kind, as the variant TAG of the prelude's RuntimeFault. ★★★The mapping
 * is a contract with scaly/memory/runtime.scaly: the tag is the variant's
 * position in that union's declaration, so a new trap kind must be appended
 * THERE and here in the same order, or an arm would bind the wrong payload.
 * The constants below are that contract written down once.
 */
int scaly_panic_last_kind(void) { return scaly_last_kind_v; }

/* Fill the arm's payload record in place. The emitter allocates it (it knows
 * the type from the arm) and hands the pointer here, so the FIELD LAYOUT stays
 * in ONE place instead of being rebuilt with GEPs in the compiler.
 *
 * ★★★It dispatches on the KIND rather than being one function per variant,
 * and that is what keeps the Emitter kind-agnostic: the switch it emits sends
 * each arm to the SAME filler, and the arm it runs is by construction the one
 * whose tag equals this kind. A third trap kind therefore appends a variant in
 * the prelude and a case here, and touches no compiler code at all.
 *
 * The layouts, each in declaration order:
 *   OutOfBoundsFault   (index: size_t, length: size_t)
 *   NullReferenceFault (where: pointer[const_char])
 *   InvalidRegionFault (what: pointer[const_char], address: size_t)
 *   SizeOverflowFault  (added: size_t, length: size_t)
 *   OutOfMemoryFault   (what: pointer[const_char], bytes: size_t)
 *   ResourceExhaustedFault (what: pointer[const_char], requested: size_t)
 *   DeadlockFault      (what: pointer[const_char])
 */
void scaly_panic_fill(void *dst)
{
    if (!dst)
        return;
    switch (scaly_last_kind_v) {
    case SCALY_FAULT_DEADLOCK:
    case SCALY_FAULT_NULL_REFERENCE: {
        const char **out = (const char **)dst;
        out[0] = scaly_last_what ? scaly_last_what : "";
        break;
    }
    case SCALY_FAULT_INVALID_REGION:
    case SCALY_FAULT_OUT_OF_MEMORY:
    case SCALY_FAULT_RESOURCE: {
        const char **out = (const char **)dst;
        out[0] = scaly_last_what ? scaly_last_what : "";
        ((size_t *)dst)[1] = scaly_last_index;
        break;
    }
    case SCALY_FAULT_OUT_OF_BOUNDS:
    case SCALY_FAULT_SIZE_OVERFLOW:
    default: {
        size_t *out = (size_t *)dst;
        out[0] = scaly_last_index;
        out[1] = scaly_last_length;
        break;
    }
    }
}

int scaly_catch_active(void) { return scaly_catch_top != 0; }

/* --- the chain head, for the FIBER crossings -----------------------------
 *
 * ★★★A catch frame lives on the stack of the routine that pushed it, and a
 * fiber has its own stack. The chain must therefore be per FIBER, not per
 * thread: while fiber A is suspended, its frames must be invisible, or a
 * panic raised by whoever runs next would longjmp into a stack that is not
 * running. Measured before the fix (TRAPS.md 3.16): a fiber armed a catch
 * point and yielded, main then violated a bound, and the FIBER's else arm ran
 * -- on the suspended stack -- and died in Fiber.finish with exit 21 because
 * `current_fiber` was not it.
 *
 * `Fiber.swap_allocator` already swaps the StackBucket allocator state at
 * every crossing, keeping the invariant that the record holds the NON-running
 * side's state; these two let it do the same for the chain head. It is a
 * get/set pair rather than one swap function because the Scaly side owns the
 * record field and the invariant. */
void *scaly_catch_top_get(void) { return (void *)scaly_catch_top; }

void scaly_catch_top_set(void *top)
{
    scaly_catch_top = (scaly_catch_frame *)top;
}

const char *scaly_panic_last_what(void)
{
    return scaly_last_what ? scaly_last_what : "";
}
long long scaly_panic_last_index(void)  { return (long long)scaly_last_index; }
long long scaly_panic_last_length(void) { return (long long)scaly_last_length; }
