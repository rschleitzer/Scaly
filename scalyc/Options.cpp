#include "scalyc.h"
using namespace scaly;
namespace scalyc {

Options::Options()
:files(0), outputName() {}


_Result<Options, OptionsError> Options::parseArguments(_Page* _rp, _Page* _ep, _Vector<String>* args) {
    size_t length = args->length();
    Options& options = *new(_rp) Options();
    {
        // Make a region for the current block and get the page
        _Region _region; _Page* _p = _region.get();

        _Array<String>* files = new(_p) _Array<String>();
        for (size_t i = 0; i < length; i++) {

            if (length < 2 || (**(*args)[i])[0] != '-') {
                files->push(*(*args)[i]);
                continue;
            }
            switch ((**(*args)[i])[1])
            {
                case 'o': {
                    i++;
                    if (i == length)
                        return _Result<Options, OptionsError>(new(_ep) OptionsError(new(_ep) _OptionsError_invalidOption(*(*args)[i])));
                    else
                        options.outputName = *(*args)[i];

                    break;
                }
                case 'd': {
                    i++;
                    if (i == length)
                        return _Result<Options, OptionsError>(new(_ep) OptionsError(new(_ep) _OptionsError_invalidOption(*(*args)[i])));
                    else
                        options.directory = *(*args)[i];

                    break;
                }
                default:
                    return _Result<Options, OptionsError>(new(_ep) OptionsError(new(_ep) _OptionsError_unknownOption(*(*args)[i])));
            }
        }

        if (!options.outputName)
            return _Result<Options, OptionsError>(new(_ep) OptionsError(_OptionsErrorCode_noOutputOption));
        if (files->length() == 0)
            return _Result<Options, OptionsError>(new(_ep) OptionsError(_OptionsErrorCode_noFilesToCompile));

        options.files = &_Vector<String>::create(options.getPage(), *files);
    }

    return _Result<Options, OptionsError>(&options);
}

}
