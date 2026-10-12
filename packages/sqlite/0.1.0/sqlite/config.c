/* sqlite3_config for the sqlite package: the one entry point of the
 * library that cannot be bound from Scaly -- it takes its arguments as C
 * varargs, and on some targets (arm64 macOS) those are passed differently
 * from named ones. It takes and answers integers only. */

#include <sqlite3.h>

/* Before the library's first use: no statistics of its memory. With them
 * every allocation and release takes ONE mutex of the whole process, and
 * the connections of several threads wait for each other on it. The
 * answer is sqlite3_config's (SQLITE_MISUSE once the library is in use). */
int scaly_sqlite_no_memory_statistics(void)
{
    return sqlite3_config(SQLITE_CONFIG_MEMSTATUS, 0);
}
