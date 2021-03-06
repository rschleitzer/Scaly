#include "shortest.h"
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
    page->reset();
    __CurrentPage = page;

    _Task* task = new(page) _Task();
    __CurrentTask = task;

    // Collect the arguments into a string Vector
    _Array<string>* arguments = new(__CurrentPage) _Array<string>(argc - 1);
    for (int i = 1; i < argc; i++)
        arguments->push(new(__CurrentPage) string(argv[i]));

    // Call Scaly's top-level code
    int ret = shortest::_main(arguments);

    // Only for monitoring, debugging and stuff
    __CurrentTask->dispose();

    // Give back the return code of the top-level code
    return ret;
}

namespace shortest {

int _main(_Array<string>* args) {
//_Region _rp; _Page* _p = _rp.get();

return 0;

}

}
