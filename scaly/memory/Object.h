namespace scaly {
namespace memory {

struct Page;
struct Region;

struct Object {
    void* operator new(size_t size, size_t align, Page* page);
    Page* get_page();
};

}

}