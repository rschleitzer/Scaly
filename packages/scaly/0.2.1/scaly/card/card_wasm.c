/* The graphics card's vectors on wasm32: the page the program runs in
 * (scaly/card.scaly, ROADMAP-card.md).
 *
 * Containment: this is C because it names symbols of ONE system -- the
 * imports of the module "scaly_card", which only a WebAssembly host has --,
 * rule (a) of the standard library's shims. The host is the page's
 * packages/scaly/<version>/web/card-host.mjs:
 *
 *   scaly_card.available() -> i32                 1: the page has a card
 *   scaly_card.create(bytes) -> id                a vector of that many bytes, zero
 *   scaly_card.write(id, at, from, bytes)         listed: the bytes are copied now
 *   scaly_card.load(id, at, path, path_length, offset, bytes) -> i32
 *                                                 listed: from the host's copy of the file
 *   scaly_card.run(request)                       listed: a kernel's run
 *   scaly_card.read(id, at, into, bytes, why, why_capacity) -> i32
 *                                                 every listed thing done, as ONE
 *                                                 command buffer, then the bytes;
 *                                                 0, or the length of why it stopped
 *
 * A size is 32 bits wide at the import (a page's memory is below 4 GB, and
 * JavaScript sees a 64-bit parameter as a BigInt). `request` is a
 * scaly_card_request in the program's memory: every field 32 bits wide at a
 * fixed place, so that JavaScript reads it without knowing C.
 *
 * Elsewhere eio.c and eio_windows.c answer that there is no card. */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

struct scaly_card_slot
{
    unsigned int address;               /*  0: an ordinary vector's elements (its id 0) */
    unsigned int length;                /*  4: elements */
    unsigned int element;               /*  8: bytes of one */
    unsigned int id;                    /* 12: a CardVector's id, 0 for an ordinary vector */
};

struct scaly_card_request
{
    const char *wgsl;                   /*  0 */
    const char *where;                  /*  4: the loop, file and line */
    const unsigned int *words;          /*  8 */
    unsigned int n_words;               /* 12 */
    unsigned int n_inputs;              /* 16 */
    unsigned int n_outputs;             /* 20 */
    unsigned int count;                 /* 24 */
    const struct scaly_card_slot *slots; /* 28: the inputs, then the outputs */
};

__attribute__((import_module("scaly_card"), import_name("available")))
extern int scaly_card_page_available(void);
__attribute__((import_module("scaly_card"), import_name("create")))
extern unsigned int scaly_card_page_create(unsigned int bytes);
__attribute__((import_module("scaly_card"), import_name("write")))
extern void scaly_card_page_write(unsigned int id, unsigned int at, const void *from, unsigned int bytes);
__attribute__((import_module("scaly_card"), import_name("load")))
extern int scaly_card_page_load(unsigned int id, unsigned int at, const char *path, unsigned int path_length, unsigned int offset, unsigned int bytes);
__attribute__((import_module("scaly_card"), import_name("run")))
extern void scaly_card_page_run(const struct scaly_card_request *request);
__attribute__((import_module("scaly_card"), import_name("read")))
extern int scaly_card_page_read(unsigned int id, unsigned int at, void *into, unsigned int bytes, char *why, unsigned int why_capacity);

static void scaly_card_stop(const char *what)
{
    fprintf(stderr, "card: %s\n", what);
    fflush(stderr);
    exit(15);
}

static unsigned int scaly_card_narrow(unsigned long long n, const char *what)
{
    if (n > 0xffffffffULL)
        scaly_card_stop(what);
    return (unsigned int)n;
}

int scaly_card_available(void)
{
    return scaly_card_page_available();
}

unsigned int scaly_card_create(unsigned long long bytes)
{
    return scaly_card_page_create(scaly_card_narrow(bytes, "a CardVector of 4 GB or more"));
}

void scaly_card_write(unsigned int id, unsigned long long at, const void *from, unsigned long long bytes)
{
    scaly_card_page_write(id, scaly_card_narrow(at, "write past 4 GB"), from, scaly_card_narrow(bytes, "write past 4 GB"));
}

void scaly_card_read(unsigned int id, unsigned long long at, void *into, unsigned long long bytes)
{
    char why[512];
    int n = scaly_card_page_read(id, scaly_card_narrow(at, "read past 4 GB"), into, scaly_card_narrow(bytes, "read past 4 GB"), why, sizeof why - 1);
    if (n > 0)
    {
        why[n < (int)sizeof why - 1 ? n : (int)sizeof why - 1] = 0;
        scaly_card_stop(why);
    }
}

int scaly_card_load(unsigned int id, unsigned long long at, const char *path, unsigned long long offset, unsigned long long bytes)
{
    return scaly_card_page_load(id, scaly_card_narrow(at, "load past 4 GB"), path, (unsigned int)strlen(path),
                                scaly_card_narrow(offset, "a file offset past 4 GB"), scaly_card_narrow(bytes, "load past 4 GB"));
}

/* A kernel's run, as the compiler hands it (Emitter.emit_card_run#): per
 * buffer its address, element count and bytes of an element, and its id. */
void scaly_card_run(const char *wgsl, const char *where, long long count, const unsigned int *words, unsigned long long n_words,
                    void **buffers, const unsigned long long *sizes, const unsigned int *ids, unsigned long long n_inputs, unsigned long long n_outputs)
{
    unsigned long long n = n_inputs + n_outputs;
    struct scaly_card_slot slots[32];
    unsigned long long i;
    int on_card = 0, in_memory = 0;
    if (count <= 0)
        return;
    if (!wgsl)
        scaly_card_stop("a loop over CardVectors without a kernel for this card");
    if (count >= 0x7fffffffLL || n > 29)
        scaly_card_stop("a loop over CardVectors the card cannot run: too many rounds or vectors");
    for (i = 0; i < n; i++)
    {
        slots[i].address = (unsigned int)(uintptr_t)buffers[i];
        slots[i].length = scaly_card_narrow(sizes[2 * i], "a vector of 4 G elements or more");
        slots[i].element = (unsigned int)sizes[2 * i + 1];
        slots[i].id = ids[i];
        if (slots[i].element != 4)
            scaly_card_stop("a vector of no 32-bit number on the card");
        if (ids[i])
            on_card = 1;
        else if (i >= n_inputs)
            in_memory = 1;
    }
    /* every output of such a loop is a CardVector (GpuKernel); one that lives
     * in memory beside one on the card is a mix of homes */
    if (!on_card || in_memory)
        scaly_card_stop("a loop over CardVectors that live in memory and on the card at once");
    struct scaly_card_request request = {
        wgsl, where, words, (unsigned int)n_words, (unsigned int)n_inputs, (unsigned int)n_outputs,
        (unsigned int)count, slots,
    };
    scaly_card_page_run(&request);
}

void scaly_panic_card(const char *what)
{
    scaly_card_stop(what);
}
