#ifdef _WIN32
/* MSVC deprecates gmtime, localtime and sscanf in favour of its own _s
 * variants and, under -Werror, refuses to compile them at all. That
 * deprecation is a Microsoft opinion, not a C one: all three are standard C,
 * and this file is a verbatim port of style/primitive.cxx, so switching to the
 * _s forms on Windows alone would make ONE target behave differently from the
 * reference and from the other three — exactly what rule zero forbids.
 *
 * The substance behind the warning is real but not ours: gmtime/localtime
 * return a pointer into a static buffer, so they are not thread-safe. The
 * POSIX build has the identical property, the callers are the DSSSL time
 * primitives on a single evaluation thread, and making Windows the only safe
 * one would hide the shared limitation rather than fix it.
 *
 * Defined here rather than in tools/ctime.sh so that every build path gets it
 * — the CI rungs compile this file directly. It must precede every include. */
#define _CRT_SECURE_NO_WARNINGS 1
#endif

/* Civil-time shim (the DSSSL time primitives, COMPLETENESS.md package 22).
 *
 * Three functions, compiled by tools/ctime.sh into ctime.o and linked beside
 * fcontext.o and eio.o into every libscaly.a AND into every scalyc/scalyls
 * link (the explicit object lists in tools/build-from-seed.sh, tools/seed.sh,
 * tools/verify-seed.sh, tools/install.sh and tests/lsp/run.sh). The compiler
 * itself calls none of them; it carries them so that a dazzle stylesheet run
 * through `--jit` resolves them out of the host process, exactly as an AOT
 * link resolves them out of the archive. An undefined extern under ORC would
 * become a silently 0-returning stub (memory opensp-harness-falsepass).
 *
 * Why C and not Scaly, per the containment rule in CLAUDE.md:
 *   (a) all three touch `struct tm` — a libc STRUCT whose field order is not
 *       guaranteed across the four LP64 targets the single committed seed
 *       serves, so its layout may not be assumed in Scaly; and
 *   (b) timeConv drives the parse with `sscanf` and the formatter with
 *       `sprintf` — VARIADIC libc calls, which a fixed-prototype Scaly extern
 *       cannot make faithfully on arm64.
 *
 * The parsing function is a verbatim port of style/primitive.cxx:timeConv,
 * quirks included: see the comments at each one. Every value crossing the
 * boundary is 64-bit so a Scaly `int` parameter matches without an i32
 * declaration (memory llvm-extern-i32-abi).
 */

#include <stdio.h>
#include <string.h>
#include <time.h>

/* `(time)` — primitive.cxx:1564. "This assumes a Posix compatible time()." */
long long scaly_time_now(void)
{
  return (long long)time(0);
}

/* Process CPU time in MICROSECONDS — the JIT's compile-time accounting
 * (dazzle/Jit.scaly, `--jit-stats`), which used to call `clock()` directly.
 *
 * It cannot stay a direct extern, and the width is only the first of three
 * reasons. (1) `clock_t` is `long`: 64-bit on LP64, 32-bit on LLP64 (Win64),
 * and ONE seed serves every target, so the Scaly declaration cannot be
 * target-conditional — see check 4 of tests/abi/run.sh. (2) CLOCKS_PER_SEC is
 * 1000000 on the LP64 targets, so the raw value already IS microseconds
 * there, but it is 1000 on Windows — the caller's unit would be off by 1000x.
 * (3) Windows' clock() measures WALL time since process start, not CPU time
 * at all.
 *
 * Scaling here settles (1) and (2) for every target. (3) is semantic and
 * belongs to the Windows arm, which is deliberately NOT written yet: it needs
 * GetProcessTimes (user+kernel FILETIME in 100 ns units) and cannot be
 * verified on an LP64 host, so writing it now would ship untested code and
 * hide the gap. Stage 7 brocken 3 owns it; until then a Windows build gets
 * wall time in correct microseconds, which is wrong in the same direction the
 * reference is, and loudly documented rather than silently 1000x off.
 *
 * The identity branch is exact and cannot overflow; the general form is kept
 * for the targets where the scale is not 1. Note this is a RUNTIME compare,
 * not an `#if`: macOS defines CLOCKS_PER_SEC as `((clock_t)1000000)`, and the
 * preprocessor cannot evaluate a cast, so `#if CLOCKS_PER_SEC == 1000000` is
 * a hard error there. Both operands are constants, so -O2 folds it away. */
long long scaly_time_cpu_usec(void)
{
  long long ticks = (long long)clock();
  long long per_sec = (long long)CLOCKS_PER_SEC;
  if (per_sec == 1000000LL)
    return ticks;
  return ticks * 1000000LL / per_sec;
}

/* `(time->string k gmt?)` — primitive.cxx:1571. Writes the ISO8601 form into
 * `out` (which must hold 64 bytes, as the reference's stack buffer does) and
 * returns its length. Nonzero `use_gmt` selects gmtime, as the reference's
 * `argc > 1 && argv[1] != makeFalse()` does. */
long long scaly_time_format(long long k, long long use_gmt, char *out)
{
  time_t t = (time_t)k;
  const struct tm *p;
  if (use_gmt)
    p = gmtime(&t);
  else
    p = localtime(&t);
  sprintf(out, "%04d-%02d-%02dT%02d:%02d:%02d",
          p->tm_year + 1900, p->tm_mon + 1, p->tm_mday,
          p->tm_hour, p->tm_min, p->tm_sec);
  return (long long)strlen(out);
}

/* timeConv — primitive.cxx:5191, the parse behind the four comparisons.
 * Returns (time_t)-1 for a string it cannot parse; the caller turns that into
 * notATimeString. `s`/`n` are the StringC's Char array, narrowed here exactly
 * as the reference narrows it (`buf[i] = char(s[i])`, at most 99 chars). */
long long scaly_time_conv(const unsigned int *s, long long n)
{
  char buf[100];
  unsigned i;

  for (i = 0; i < (unsigned)n && i < (sizeof(buf) - 1); i++)
    buf[i] = (char)s[i];
  buf[i] = 0;
  time_t    today_sec = time(NULL);
  struct tm tim, *today;
  int       nparsed;

  today = localtime(&today_sec);

  /* First try to parse as time string without date */
  /* Defaults are same as of today */
  memcpy(&tim, today, sizeof(tim));
  nparsed = sscanf(buf, "%d:%d:%d",
                   &tim.tm_hour,
                   &tim.tm_min,
                   &tim.tm_sec);

  /* If we got only one number, it could be
     a year so try to parse complete format */
  if (nparsed < 2) {
    /* Defaults are set to zero */
    memset(&tim, 0, sizeof(tim));

    /* This accepts any non digit character between
       the date and time spec
    */
    nparsed = sscanf(buf, "%d-%d-%d%*[^0-9]%d:%d:%d",
                     &tim.tm_year,
                     &tim.tm_mon,
                     &tim.tm_mday,
                     &tim.tm_hour,
                     &tim.tm_min,
                     &tim.tm_sec);
    switch (nparsed) {
    case 0:
        /* Invalid parse */
        return (long long)(time_t)-1;
        /* Not reached */
    case 1:
        /* We only got a year set to January First
           Month is already set to 0
        */
        /* Fall through */
    case 2:
        tim.tm_mday = 1;
        /* Fall through to month normalization */
    default:
        /* ★MEASURED: the comment above says "January First", but the
         * decrement runs for the year-only case too, so a bare "1999" is
         * month -1 day 1 = 1998-12-01, and an EMPTY string (sscanf answers
         * EOF, which lands in this default) is month -1 day 0. Pinned in
         * tests/dazzle/prims/time1. */
        tim.tm_mon -= 1;
        break;
    }

    /* ★MEASURED: only a year of 1900 or more is treated as a full year, so
     * "1000-01-01" is the year 2900, not 1000. */
    if (tim.tm_year < 38)
      tim.tm_year += 100; /* Y2K workaround */
    else if (tim.tm_year >= 1900)
      tim.tm_year -= 1900;
  }

  return (long long)mktime(&tim);
}
