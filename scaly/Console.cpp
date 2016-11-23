#include "Scaly.h"
#include <cstdio>
namespace scaly {

size_t print(const char* message) {
    if (fputs(message, stdout) >= 0)
        return 0;
    return 1;
}

size_t print(string* message) {
    return print(message->getNativeString());
}

}
