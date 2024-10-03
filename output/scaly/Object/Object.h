
using namespace scaly::memory;
struct Object {
    void* operator new(size_t size, size_t align, Page* page);
};
