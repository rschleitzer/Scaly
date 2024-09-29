namespace scaly {
namespace memory {
using namespace scaly::memory;
struct Region : Object {
    Page* page;
    Region();
    ~Region();
    void get_page();
};
}
}