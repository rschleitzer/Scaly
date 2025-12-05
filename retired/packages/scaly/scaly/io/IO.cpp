#ifndef __Scaly_IO__
#define __Scaly_IO__

#include "../containers/Containers.cpp"

extern "C" int * __error(void);
#define ENOENT 2
#define	EOF	(-1)

#include "File.h"
#include "Directory.h"
#include "Console.h"
#include "Path.h"

#endif // __Scaly_IO__
