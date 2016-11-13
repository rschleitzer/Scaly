#include "scalyc.h"
using namespace scaly;
namespace scalyc {

int _main(_Vector<string>* args) {
_Region _rp; _Page* _p = _rp.get();

if (args->length() < 1) {
    return 1;
}
else {
    auto _options_result = Options::parseArguments(_p, _p, args);
    Options* options = nullptr;
    if (_options_result.succeeded()) {
        options = _options_result.getResult();
    }
    else if (_options_result.getErrorCode() == _OptionsErrorCode_invalidOption) {
        return 2;
    }
    else if (_options_result.getErrorCode() == _OptionsErrorCode_noOutputOption) {
        return 3;
    }
    else if (_options_result.getErrorCode() == _OptionsErrorCode_noFilesToCompile) {
        return 4;
    }
    auto _Compiler_error = Compiler::compileFiles(_p, options);
    if (_Compiler_error) {
        switch (_Compiler_error->getErrorCode()) {
            default:
                return 5;
        }
    }
}

return 0;

}

}
