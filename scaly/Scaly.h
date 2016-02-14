#ifndef __Scaly_Scaly__
#define __Scaly_Scaly__

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <new>
#include <iostream>

#include "Page.h"
#include "Object.h"
#include "Array.h"
#include "Task.h"
#include "Region.h"
#include "Result.h"
#include "LetString.h"
#include "VarString.h"
#include "Path.h"
#include "File.h"
#include "Directory.h"

namespace scaly {

static const int _alignment = 8;
char* align(char*);

}
#endif//__Scaly_Scaly__
