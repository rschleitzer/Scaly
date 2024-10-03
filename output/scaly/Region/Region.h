
using namespace scaly::memory;
struct Region : Object {
    Page* page;
    Region();
    ~Region();
};
