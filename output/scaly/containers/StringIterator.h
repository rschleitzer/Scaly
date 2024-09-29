namespace scaly {
namespace containers {
using namespace scaly::memory;
struct StringIterator : Object {
    char* last;
    char* current;
    StringIterator(String string);
    char* next();
};
}
}