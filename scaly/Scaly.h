#ifndef __Scaly_Scaly__
#define __Scaly_Scaly__

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <new>
#include <iostream>

#include "Page.h"
#include "Region.h"
#include "Object.h"
#include "Result.h"
#include "Array.h"
#include "ScalyString.h"
#include "File.h"
#include "Directory.h"

namespace scaly {

static const int _alignment = 8;
char* align(char*);

}
#endif//__Scaly_Scaly__
