/* The fiber context switch on WebAssembly: there is none. A wasm function
 * cannot switch to another stack (the stack the engine keeps is not memory a
 * program can address), so the wasm profile runs without fibers -- no task
 * ever parks, since the evented-I/O queue never opens (eio_wasm.c) and every
 * read blocks where it stands. These two exist so the standard library links;
 * reaching one is a broken profile, said and ended, never a jump into
 * nothing. The tool takes this file for fcontext_<arch>.S by its `_wasm`. */
#include <stdlib.h>
#include <unistd.h>

static void scaly_no_fibers(void)
{
    static const char msg[] = "scaly: no fibers on WebAssembly\n";
    ssize_t w = write(2, msg, sizeof msg - 1);
    (void)w;
    _Exit(2);
}

void* scaly_make_context(void* stack_top, void* fn, void* stack_base)
{
    (void)stack_top; (void)fn; (void)stack_base;
    scaly_no_fibers();
    return 0;
}

void* scaly_jump_context(void* to)
{
    (void)to;
    scaly_no_fibers();
    return 0;
}
