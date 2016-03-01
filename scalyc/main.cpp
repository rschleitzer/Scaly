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

    // Collect the arguments into a String Vector
    _Vector<_LetString>& arguments = _Vector<_LetString>::createUninitialized(__CurrentPage, argc - 1);
    for (int i = 1; i < argc; i++)
        *arguments[i - 1] = &_LetString::create(__CurrentPage, argv[i]);

    // Call Scaly's top-level code
    int ret = scalyc::_main(arguments);

    // Only for monitoring, debugging and stuff
    __CurrentTask->dispose();

    // Give back the return code of the top-level code
    return ret;
}