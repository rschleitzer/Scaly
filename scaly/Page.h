#ifndef __Scaly_Page__
#define __Scaly_Page__
namespace scaly {

class _Page {
public:
    _Page(size_t size);
    _Page* next();
    _Page* previous();
    void reset();
    void* operator new(size_t size, void* location);
    void* allocateObject(size_t size);
    static _Page* allocateNextPage(_Page* previousPage);
    _Page* allocateExtensionPage(size_t size);
    void deallocate();
    static void reclaimArray(void* address);
    static _Page* getPage(void* address);
    bool extend(void* address, size_t size);

    static const int pageSize = 0x2000;

private:
    void* allocateLocal(size_t size);
    void deallocatePageExtensions();
    _Page** getLastExtensionPageLocation();
    void freeExtensionPage(_Page* page);
    static _Page* allocate(size_t size);

    _Page* currentPage;
    _Page* nextPage;
    _Page* previousPage;
    size_t size;
    _Page* extendingPage;
    _Page** nextExtensionPageLocation;

    void* nextObject;
};

}

#endif // __Scaly_Page__

