typedef __SIZE_TYPE__ size_t;

namespace scaly {
namespace memory {

struct Page;

struct Object {
    Page* get_page();
    void* operator new(size_t size, size_t align, Page* page);
};

}

}