#include "scalyc.h"
using namespace scaly;
namespace scaly {

extern __thread _Page* __CurrentPage;
extern __thread _Task* __CurrentTask;

}

int main(int argc, char** argv) {
    // Allocate the root page for the main thread
    posix_memalign((void**)&__CurrentPage, _pageSize, _pageSize * _maxStackPages);
    if (!__CurrentPage)
        return -1;
    __CurrentPage->reset();
    _Task* task = new(__CurrentPage) _Task();
    __CurrentTask = task;

    // Collect the arguments into a string Vector
    _Array<string>* arguments = new(__CurrentPage) _Array<string>(argc - 1);
    for (int i = 1; i < argc; i++)
        arguments->push(new(__CurrentPage) string(argv[i]));

    // Call Scaly's top-level code
    auto _File_error = scalyc::_main(__CurrentPage, arguments);
    int ret = 0;

    // Convert Scaly's error enum back to OS errno values
    if (_File_error) {
        switch(_File_error->_getErrorCode()) {
            case _FileErrorCode_noSuchFileOrDirectory:
                ret = ENOENT;
                break;
            default:
                ret = -1;
                break;
        }
    }

    // Only for monitoring, debugging and stuff
    __CurrentTask->dispose();

    // Give back the return code of the top-level code
    return ret;
}

namespace scalyc {

FileError* _main(_Page* _ep,  _Array<string>* arguments) {
    _Region _rp; _Page* _p = _rp.get();

    auto _options_result = Options::parseArguments(_p, _p, arguments);
    Options* options = nullptr;
    if (_options_result.succeeded()) {
        options = _options_result.getResult();
    }
    else if (_options_result._getErrorCode() == _OptionsErrorCode_invalidOption) {
        string* option = _options_result.getError()->get_invalidOption()->option;
        {
            _Region _region; _Page* _p = _region.get();
            VarString* msg = new(_p) VarString("Invalid option ");
            msg->append(option);
            msg->append(" .\n");
            string* message = new(_p) string(msg);
            auto _print_error = print(_p, message);
            if (_print_error) { switch (_print_error->_getErrorCode()) {
                default: {
                return nullptr;
                }
            } }
            return nullptr;
        }
    }
    else if (_options_result._getErrorCode() == _OptionsErrorCode_noOutputOption) {
        {
            print(_ep, "No output option.\n");
            return nullptr;
        }
    }
    else if (_options_result._getErrorCode() == _OptionsErrorCode_noFilesToCompile) {
        {
            print(_ep, "No files to compile.\n");
            return nullptr;
        }
    }
    Compiler::compileFiles(options);

    return nullptr;

}

}
