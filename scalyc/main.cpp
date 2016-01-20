#include "scalyc.h"
using namespace scaly;
namespace scaly {

extern __thread _Page* __CurrentPage;

}

int main(int argc, char** argv) {
    // Allocate the root page for the main thread
    __CurrentPage = _Page::allocateNextPage(0);
    if (!__CurrentPage)
        return -1;

    // Collect the arguments into a String Array
    Array<String>& arguments = *new (__CurrentPage) Array<String>(argc - 1);
    for (int i = 1; i < argc; i++)
        *arguments[i - 1] = new(__CurrentPage) String(argv[i]);        

    // Call Scaly's top-level code
    int ret = scalyc::_main(arguments);

    // Only for monitoring, debugging and stuff
    __CurrentPage->deallocate();
    
    // Give back the return code of the top-level code
    return ret;
}