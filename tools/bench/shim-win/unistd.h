/* <unistd.h> for the Windows box (clang, MSVC runtime): mandelbrot #6 includes
   it for write() alone, which the UCRT declares in <io.h> (the POSIX name
   resolves through oldnames.lib). On the include path on Windows only. */
#pragma once
#include <io.h>
