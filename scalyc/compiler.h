#ifndef __scalyc__compiler__
#define __scalyc__compiler__
#include "scalyc.h"

void scalyc_Compiler_compileFiles(scalyc_Options* options);
_scalyc_Compiler_Result parseUnit(scaly_Page* _rp, scaly_Page* _ep, scaly_string* moduleName, scaly_string* text);

#endif // __scalyc__compiler__
