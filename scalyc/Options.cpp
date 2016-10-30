#include "scalyc.h"
using namespace scaly;
namespace scalyc {

Options::Options(_Vector<String>* input, String* output, String* dir) {
    files = input;
    outputName = output;
    directory = dir;
}

_Result<Options, OptionsError> Options::parseArguments(_Page* _rp, _Page* _ep, _Vector<String>* args) {
    _Region _region; _Page* _p = _region.get();
    size_t length = args->length();
    String* output = nullptr;
    String* dir = nullptr;
    _Array<String>* input = new(_p) _Array<String>();
    size_t i = 0;
    do {
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
                        return _Result<Options, OptionsError>(new(_ep) OptionsError(new(_ep) _OptionsError_invalidOption(*(*args)[i])));
                    else
                        output = *(*args)[i];
                }
                break;
            }

            case 'd': {
                {
                    i++;
                    if (i == length)
                        return _Result<Options, OptionsError>(new(_ep) OptionsError(new(_ep) _OptionsError_invalidOption(*(*args)[i])));
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
    while (i < length);
    if (output == nullptr)
        return _Result<Options, OptionsError>(new(_ep) OptionsError(_OptionsErrorCode_noOutputOption));
    if (input->length() == 0)
        return _Result<Options, OptionsError>(new(_ep) OptionsError(_OptionsErrorCode_noFilesToCompile));

    return _Result<Options, OptionsError>(new(_rp) Options(&_Vector<String>::create(_rp, *input), output, dir));
}

}
