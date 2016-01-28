#include "scalyc.h"
using namespace scaly;
namespace scalyc {

Options::Options(Array<String>& args)
:files(args) {}


_Result<Options, OptionsError> Options::ParseArguments(_Page* _rp, _Page* _ep, Array<String>& args) {
    if (!args.length())
        return _Result<Options, OptionsError>(new (_ep) OptionsError(noFilesToCompile));
    
    Options* options = new(_rp) Options(args);
    options->files = args;
    return _Result<Options, OptionsError>(options);
}

}
