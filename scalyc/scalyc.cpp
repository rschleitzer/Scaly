#include "scalyc.h"
using namespace scaly;
namespace scaly {

extern __thread _Page* __CurrentPage;
extern __thread _Task* __CurrentTask;

}

int main(int argc, char** argv) {
    // Allocate the root page for the main thread
    _Page* page = 0;
    posix_memalign((void**)&page, _pageSize, _pageSize * _maxStackPages);
    if (!page)
        return -1;
    new (page) _Page();
    __CurrentPage = page;

    _Task* task = new(page) _Task();
    __CurrentTask = task;

    // Collect the arguments into a string Vector
    _Vector<string>* arguments = &_Vector<string>::createUninitialized(__CurrentPage, argc - 1);
    for (int i = 1; i < argc; i++)
        *(*arguments)[i - 1] = new(__CurrentPage) string(argv[i]);

    // Call Scaly's top-level code
    int ret = scalyc::_main(arguments);

    // Only for monitoring, debugging and stuff
    __CurrentTask->dispose();

    // Give back the return code of the top-level code
    return ret;
}

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
    else if (_options_result._getErrorCode() == _OptionsErrorCode_invalidOption) {
        return 2;
    }
    else if (_options_result._getErrorCode() == _OptionsErrorCode_noOutputOption) {
        return 3;
    }
    else if (_options_result._getErrorCode() == _OptionsErrorCode_noFilesToCompile) {
        return 4;
    }
    auto _Compiler_error = Compiler::compileFiles(_p, options);
    if (_Compiler_error) {
        switch (_Compiler_error->_getErrorCode()) {
            default:
                return 5;
        }
    }
}
return 0;

}

}
