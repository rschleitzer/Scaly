#include "Scaly.h"
#include <cstdio>
namespace scaly {

void print(const char* message) {
    fputs(message, stdout);
}

void print(string* message) {
    return print(message->getNativeString());
}

}
