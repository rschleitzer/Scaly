#include "scalyc.h"
using namespace scaly;
namespace scalyc {

int _main(_Vector<String>* args) {
_Region _rp; _Page* _p = _rp.get();
_Region _rep; _Page* _ep = _rep.get();

if (args->length() < 1) {
    return 1;
}
else {
    auto _options_result = Options::parseArguments(_p, _ep, args);
    Options* options = nullptr;
    if (_options_result.succeeded()) {
        options = _options_result.getResult();
    }
    else {
        switch (_options_result.getErrorCode()) {
            case _OptionsErrorCode_invalidOption: {
                return 2;
            }
            case _OptionsErrorCode_noOutputOption: {
                return 3;
            }
            case _OptionsErrorCode_noFilesToCompile: {
                return 4;
            }
        }
    }

    CompilerError* compilerError = Compiler::compileFiles(_p, options);
    if (compilerError) {
        return 2;
    }
}

return 0;
}

}
