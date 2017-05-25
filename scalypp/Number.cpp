#include "Scaly.h"
namespace scaly {

string* Number::toString(_Page* _rp, size_t theNumber) {
    char buf[255] = "";
    snprintf(buf, sizeof buf, "%zu", theNumber);
    return new(_rp) string(buf);
}

}
