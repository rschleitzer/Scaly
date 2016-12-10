#ifndef __Scaly_Scaly__
#define __Scaly_Scaly__

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <new>
#include <iostream>

const int _alignment = 8;
const size_t _pageSize = 0x1000;
const size_t _maxStackPages = 0x100;

#include "Page.h"
#include "Object.h"
#include "Array.h"
#include "Chunk.h"
#include "Pool.h"
#include "Task.h"
#include "Region.h"
#include "Result.h"
#include "LetString.h"
#include "VarString.h"
#include "Number.h"
#include "Path.h"
#include "File.h"
#include "Directory.h"
#include "Console.h"

namespace scaly {
char* align(char*);

}
#endif//__Scaly_Scaly__
