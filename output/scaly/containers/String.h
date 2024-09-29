namespace scaly {
namespace containers {
using namespace scaly::memory;
struct String : Object {
    char* data;
    String(size_t length);
};
}
}