namespace scaly {
namespace memory {
using namespace scaly::memory;
struct Page : Object {
    PageList* exclusive_pages;
    void* next_object;
    Page* next_page;
    Page* current_page;
};
}
}