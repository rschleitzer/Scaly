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
/* ---- the pre-1970 gap, and why it is arithmetic rather than a library call
 *
 * ★MSVC's mktime/gmtime/localtime REFUSE any time before 1970-01-01: mktime
 * answers (time_t)-1 and gmtime/localtime answer NULL, where glibc and Apple
 * libc handle a negative time_t normally. It is a C-library limit, not a port
 * defect — and it is observable: `(time<? "38" "37")` in tests/dazzle/prims
 * asks about 1938 (the Y2K window maps a bare "38" to 1938, "37" to 2037), and
 * on Windows the primitive answered `not an ISO8601 time string` where the
 * other three targets compare two dates. `time->string` has the same gap the
 * other way round, for the 1969 dates the same fixture pins.
 *
 * The two helpers below are Howard Hinnant's civil-from-days pair, valid for
 * any year and exact by construction (they are pure integer arithmetic, no
 * table and no epoch limit). They are compiled on EVERY target so that they
 * can be unit-tested where a Windows CRT is not available — but they are USED
 * only where the CRT gives up, so POSIX behaviour is unchanged, byte for byte,
 * and the reference stays the oracle on the platforms that have one.
 *
 * ★Hence the SCALY_CTIME_TEST guard: on POSIX nothing calls them, and this file
 * is compiled -Wall -Wextra -Werror, so an unconditional definition is a build
 * error rather than dead weight. Defining that macro is how the host checks
 * them — 6001 civil round-trips and 4719 mktime comparisons, 0 mismatches. */
#if defined(_WIN32) || defined(SCALY_CTIME_TEST)
static long long sc_days_from_civil(long long y, unsigned m, unsigned d)
{
  y -= m <= 2;
  {
    const long long era = (y >= 0 ? y : y - 399) / 400;
    const unsigned yoe = (unsigned)(y - era * 400);              /* [0, 399] */
    const unsigned doy = (153u * (m + (m > 2 ? -3 : 9)) + 2u) / 5u + d - 1u;
    const unsigned doe = yoe * 365u + yoe / 4u - yoe / 100u + doy;
    return era * 146097LL + (long long)doe - 719468LL;
  }
}

static void sc_civil_from_days(long long z, int *y, unsigned *m, unsigned *d)
{
  z += 719468LL;
  {
    const long long era = (z >= 0 ? z : z - 146096) / 146097;
    const unsigned doe = (unsigned)(z - era * 146097);           /* [0, 146096] */
    const unsigned yoe = (doe - doe / 1460u + doe / 36524u - doe / 146096u) / 365u;
    const long long yy = (long long)yoe + era * 400;
    const unsigned doy = doe - (365u * yoe + yoe / 4u - yoe / 100u);
    const unsigned mp = (5u * doy + 2u) / 153u;
    *d = doy - (153u * mp + 2u) / 5u + 1u;
    *m = mp + (mp < 10 ? 3 : -9);
    *y = (int)(yy + (*m <= 2));
  }
}

/* Seconds WEST of UTC for standard time, as mktime/localtime would apply them.
 * ★Deliberately standard time only: which DST rule was in force in 1938 is a
 * question the CRT cannot answer either (its own tables start at 1970), and
 * the fixture's zone is `EST5` — a POSIX offset with no DST rule — so this is
 * exact where it is tested and documented where it is not. */
static long long sc_utc_offset_west(void)
{
#ifdef _WIN32
  long tz = 0;
  _tzset();
  if (_get_timezone(&tz) != 0)
    tz = 0;
  return (long long)tz;
#else
  return 0;
#endif
}

/* What mktime would answer for a `struct tm` in LOCAL standard time, without
 * the CRT's epoch floor. ★It reproduces mktime's NORMALISATION, which the
 * caller depends on: the month arriving here may be -1 (see the January-First
 * comment in scaly_time_conv), so the year/month pair is folded into a single
 * month count and floor-divided back, exactly as mktime carries the borrow. */
static long long sc_mktime_fallback(const struct tm *t)
{
  long long months = (long long)t->tm_year * 12 + t->tm_mon;
  long long ym = months >= 0 ? months / 12 : -(((-months) + 11) / 12);
  long long mo = months - ym * 12;                          /* [0, 11] */
  long long days = sc_days_from_civil(1900 + ym, (unsigned)(mo + 1), 1)
                   + (t->tm_mday - 1);
  return days * 86400LL
         + (long long)t->tm_hour * 3600
         + (long long)t->tm_min * 60
         + (long long)t->tm_sec
         + sc_utc_offset_west();
}

#endif /* _WIN32 || SCALY_CTIME_TEST */

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
#ifdef _WIN32
  /* ★THE FALLBACK IS WINDOWS-ONLY, and that guard is not caution but
   * correctness: POSIX gmtime also declines far-out values, and answering them
   * here would change what macOS and Linux print — the two platforms whose
   * output is currently byte-identical to the reference. The gap being closed
   * is Windows' alone (it starts at 1970, where the others start near 1901),
   * so the repair belongs where the gap is. Measured on the host before the
   * guard existed: 292 of 5011 sampled values are ones POSIX mktime declines. */
  if (p == 0) {
    /* The CRT declined the range (Windows, pre-1970). Same answer, computed. */
    long long secs = k - (use_gmt ? 0 : sc_utc_offset_west());
    long long days = secs / 86400;
    long long rem  = secs % 86400;
    int yy; unsigned mm, dd;
    if (rem < 0) { rem += 86400; days -= 1; }
    sc_civil_from_days(days, &yy, &mm, &dd);
    sprintf(out, "%04d-%02d-%02dT%02d:%02d:%02d",
            yy, (int)mm, (int)dd,
            (int)(rem / 3600), (int)((rem / 60) % 60), (int)(rem % 60));
    return (long long)strlen(out);
  }
#endif
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

  {
    time_t r = mktime(&tim);
    if (r != (time_t)-1)
      return (long long)r;
    /* ★(time_t)-1 is BOTH "cannot represent" and the legitimate value for
     * 1969-12-31T23:59:59 UTC, and the caller reads it as "not a time string".
     * That ambiguity is the reference's, inherited deliberately — what is NOT
     * the reference's is Windows answering it for every date before 1970. So
     * the fallback recomputes rather than trusting the flag: mktime normalises
     * a tm in place, and this reproduces that normalisation (the month may be
     * -1 here — see the January-First comment above — which is exactly why the
     * month count is folded before the civil conversion rather than after). */
#ifdef _WIN32
    return sc_mktime_fallback(&tim);
#else
    /* Unchanged on the platforms whose answer the reference defines. */
    return (long long)r;
#endif
  }
}
