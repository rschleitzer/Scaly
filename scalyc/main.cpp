#include "scalyc.h"
using namespace scaly;
namespace scaly {

extern __thread _Page* __CurrentPage;
extern __thread _Task* __CurrentTask;

}

int main(int argc, char** argv) {

    // Set up statistics counters
    _Page::initStatistics();
    
    // Allocate the root page for the main thread
    __CurrentPage = new(_Task::allocatePage()) _Page(_Page::pageSize);
    if (!__CurrentPage)
        return -1;

    // Set up the root task
    __CurrentTask = new(__CurrentPage) _Task();

    // Collect the arguments into a String Array
    _Array<String>& arguments = *new (__CurrentPage) _Array<String>(argc - 1);
    for (int i = 1; i < argc; i++)
        arguments.push(new(__CurrentPage) String(argv[i]));

    // Call Scaly's top-level code
    int ret = scalyc::_main(arguments);

    // Only for monitoring, debugging and stuff
    __CurrentTask->dispose();
    
    // Give back the return code of the top-level code
    return ret;
}