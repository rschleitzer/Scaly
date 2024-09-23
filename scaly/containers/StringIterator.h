namespace scaly {
namespace containers {

using namespace scaly::memory;

struct StringIterator : Object {
    char* current;
    char* last;
    StringIterator(String string);
    char* next();
};

}
}
