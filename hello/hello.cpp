#include "hello.h"
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
    auto _File_error = hello::_main(page, arguments);
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

namespace hello {

FileError* _main(_Page* _ep,  _Vector<string>* args) {
_Region _rp; _Page* _p = _rp.get();

{
    auto _File_error = print(_p, "Hello world!\n");;
    if (_File_error)
        return _File_error;
}

return 0;

}

}
