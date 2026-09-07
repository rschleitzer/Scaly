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
} scaly_catch_frame;               /* has been given back by then */

static __thread scaly_catch_frame *scaly_catch_top = 0;
static __thread const char *scaly_last_what = 0;
static __thread size_t scaly_last_index = 0;
static __thread size_t scaly_last_length = 0;

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
    f.what = 0; f.index = 0; f.length = 0;

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
void *scaly_catch_push(scaly_mark_fn mark, scaly_unwind_fn unwind)
{
    scaly_catch_frame *f = (scaly_catch_frame *)malloc(sizeof *f);
    if (!f)
        return 0;                  /* the emitted setjmp on a null pointer
                                    * would fault, so the caller checks */
    f->prev = scaly_catch_top;
    f->mark = mark ? mark() : 0;
    f->unwind = unwind;
    f->what = 0; f->index = 0; f->length = 0;
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
    free(f);
}

/* --- what a `when` arm needs ---------------------------------------------
 *
 * The kind, as the variant TAG of the prelude's RuntimeFault. ★★★The mapping
 * is a contract with scaly/memory/runtime.scaly: 0 is OutOfBounds because it
 * is that union's first variant, and a new trap kind must be appended THERE
 * and here in the same order, or an arm would bind the wrong payload. There is
 * one kind today, which is why this is a constant.
 */
int scaly_panic_last_kind(void) { return 0; }

/* Fill an OutOfBoundsFault in place. The emitter allocates the record (it
 * knows the type from the arm) and hands the pointer here, so the FIELD
 * LAYOUT stays in one place instead of being rebuilt with GEPs in the
 * compiler: two size_t in declaration order, index then length. */
void scaly_panic_fill_bounds(void *dst)
{
    size_t *out = (size_t *)dst;
    if (!out)
        return;
    out[0] = scaly_last_index;
    out[1] = scaly_last_length;
}

int scaly_catch_active(void) { return scaly_catch_top != 0; }

const char *scaly_panic_last_what(void)
{
    return scaly_last_what ? scaly_last_what : "";
}
long long scaly_panic_last_index(void)  { return (long long)scaly_last_index; }
long long scaly_panic_last_length(void) { return (long long)scaly_last_length; }
