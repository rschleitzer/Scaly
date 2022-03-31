#include "scalycpp.h"
using namespace scaly;
namespace scalycpp {

Options::Options(_Array<string>* input, string* output, string* dir) {
    files = input;
    outputName = output;
    directory = dir;
}

_Result<Options, OptionsError> Options::parseArguments(_Page* _rp, _Page* _ep, _Array<string>* args) {
    _Region _region; _Page* _p = _region.get();
    size_t length = args->length();
    string* output = nullptr;
    string* dir = nullptr;
    _Array<string>* input = new(_p) _Array<string>();
    size_t i = 0;
    while (i < length) {
        if (length < 2 || (*(*args)[i])->charAt(0) != '-') {
            input->push(*(*args)[i]);
            i++;
            continue;
        }
        switch ((*(*args)[i])->charAt(1)) {
            case 'o': {
                {
                    i++;
                    if (i == length)
                        return _Result<Options, OptionsError>(new(_ep) OptionsError(new(_ep) _OptionsError_invalidOption(*(*args)[i - 1])));
                    else
                        output = *(*args)[i];
                }
                break;
            }

            case 'd': {
                {
                    i++;
                    if (i == length)
                        return _Result<Options, OptionsError>(new(_ep) OptionsError(new(_ep) _OptionsError_invalidOption(*(*args)[i - 1])));
                    else
                        dir = *(*args)[i];
                }
                break;
            }

            default: {
                return _Result<Options, OptionsError>(new(_ep) OptionsError(new(_ep) _OptionsError_unknownOption(*(*args)[i])));
            }
        }
        i++;
    }
    if (output == nullptr)
        return _Result<Options, OptionsError>(new(_ep) OptionsError(_OptionsErrorCode_noOutputOption));
    if (input->length() == 0)
        return _Result<Options, OptionsError>(new(_ep) OptionsError(_OptionsErrorCode_noFilesToCompile));
    return _Result<Options, OptionsError>(new(_rp) Options(new(_rp) _Array<string>(input), output, dir));
}


}
