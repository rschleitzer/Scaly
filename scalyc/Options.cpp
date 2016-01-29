#include "scalyc.h"
using namespace scaly;
namespace scalyc {

Options::Options(Array<String>& files, String& outputName)
:files(files), outputName(outputName) {}


_Result<Options, OptionsError> Options::ParseArguments(_Page* _rp, _Page* _ep, Array<String>& args) {
    if (!args.length())
        return _Result<Options, OptionsError>(new (_ep) OptionsError(noFilesToCompile));
    
    String& outputName = *new(_rp) String("scalyc");
    
    Options* options = new(_rp) Options(args, outputName);
    return _Result<Options, OptionsError>(options);
}

}
