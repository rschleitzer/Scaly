#ifndef __Scaly__Pool__
#define __Scaly__Pool__
#include "Scaly.h"
namespace scaly {

class _Pool : public Object {
public:
    _Pool();
    _Page* allocatePage();
    bool deallocatePage(_Page* page);
    void dispose();

private:
    _Chunk* chunk;
};

}

#endif // __Scaly__Pool__
