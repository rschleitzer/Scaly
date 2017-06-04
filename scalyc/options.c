#include "scalyc.h"

scalyc_Options* scalyc_Options_new(scaly_Page* _page, scaly_Array* files, scaly_string* outputName, scaly_string* directory) {
    scalyc_Options* this = (scalyc_Options*) scaly_Page_allocateObject(_page, sizeof(scalyc_Options));

    this->files = files;
    this->outputName = outputName;
    this->directory = directory;

    return this;

}

_scalyc_Options_Result scalyc_Options_parseArguments(scaly_Page* _rp, scaly_Page* _ep, scaly_Array* args) {
    scaly_Page* _p = scaly_Page_alloc();

    size_t length = args->_size;
    scaly_string* output = 0;
    scaly_string* dir = 0;
    scaly_Array* input = scaly_Array_new(_p);
    size_t i = 0;
    while (i < length) {
        if (length < 2 || scaly_string_charAt((scaly_string*)(*scaly_Array_elementAt(args, i)), 0) != '-') {
            scaly_Array_push(input, (scaly_string*)(*scaly_Array_elementAt(args, i)));
            i++;
            continue;
        }
        switch (scaly_string_charAt((scaly_string*)(*scaly_Array_elementAt(args, i)), 1)) {
            case 'o': {
                {
                    i++;
                    if (i == length)
                        return (_scalyc_Options_Result){ _scalyc_Options_Result_ErrorCode_invalidOption, _new_scalyc_Options_Result_invalidOption(_ep, (scaly_string*)(*scaly_Array_elementAt(args, i - 1))) };
                    else
                        output = (scaly_string*)(*scaly_Array_elementAt(args, i));
                }
                break;
            }

            case 'd': {
                {
                    i++;
                    if (i == length)
                        return (_scalyc_Options_Result){ _scalyc_Options_Result_ErrorCode_invalidOption, _new_scalyc_Options_Result_invalidOption(_ep, (scaly_string*)(*scaly_Array_elementAt(args, i - 1))) };
                    else
                        dir = (scaly_string*)(*scaly_Array_elementAt(args, i));
                }
                break;
            }

            default: {
                return (_scalyc_Options_Result){ _scalyc_Options_Result_ErrorCode_unknownOption, _new_scalyc_Options_Result_unknownOption(_ep, (scaly_string*)(*scaly_Array_elementAt(args, i))) };
            }
        }
        i++;
    }
    if (output == 0)
        return (_scalyc_Options_Result){ _scalyc_Options_Result_ErrorCode_noOutputOption, 0 };
    if (input->_size == 0)
        return (_scalyc_Options_Result){ _scalyc_Options_Result_ErrorCode_noFilesToCompile, 0 };
    return (_scalyc_Options_Result){ 0, scalyc_Options_new(_rp, scaly_Array_newFromArray(_rp, input), output, dir) };
}

