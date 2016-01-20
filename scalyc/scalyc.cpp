#include "scalyc.h"
namespace scalyc {

int _main(Array<String>& args) {
    // Make a region for the current block and get the page
    _Region _region; _Page* _p = _region.get();

    // If we don't get an argument then it is Error 1
    if (args.length() < 1)
        return 1;

    // If any error occurs then it is Error 2
    CompilerError* compilerError = Compiler::compileFiles(_p, args);
    if (compilerError) {
        return 2;
    }

    return 0;
}

}
