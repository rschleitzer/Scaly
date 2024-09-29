namespace scaly {
namespace memory {
using namespace scaly::memory;
struct Object {
    Page* get_page();
    void* operator new(size_t size, size_t align, Page* page);
};
}
}