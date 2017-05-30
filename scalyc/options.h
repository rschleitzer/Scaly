#ifndef __scalyc__options__
#define __scalyc__options__
#include "scalyc.h"

typedef struct scalyc_Options scalyc_Options; struct scalyc_Options {
    scaly_Array* files;
    scaly_string* outputName;
    scaly_string* directory;
};

scalyc_Options* _scalyc_Options_new(scaly_Page* _rp, scaly_Array* input, scaly_string* output, scaly_string* dir);
_scalyc_Options_Result scalyc_Options_parseArguments(scaly_Page* _rp, scaly_Page* _ep, scaly_Array* args);

#endif // __scalyc__options__
