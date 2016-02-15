#include "scalyc.h"
namespace scalyc {

int _main(_Array<_LetString>& args) {
_Region _region; _Page* _p = _region.get();

if (args.length() < 1) {
    return 1;
}
else {
    Options* options = 0;
    _Result<Options, OptionsError> _result_options = Options::parseArguments(_p, _p, args);
    if (_result_options.succeeded()) {
        options = _result_options.getResult();
    }
    else {
        switch (_result_options.getErrorCode()) {
            case invalidOption: {
                return 2;
            }
            case noOutputOption: {
                return 3;
            }
            case noFilesToCompile: {
                return 4;
            }
        }
    }

    CompilerError* compilerError = Compiler::compileFiles(_p, *options);
    if (compilerError) {
        return 2;
    }
}

return 0;
}

}
