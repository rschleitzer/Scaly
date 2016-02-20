#ifndef __Scaly_Page__
#define __Scaly_Page__
namespace scaly {

class _Page {
public:
    _Page();
    void reset();
    void clear();
    void* operator new(size_t size, void* location);
    void* allocateObject(size_t size);
    _Page* allocateExtensionPage();
    _Page* allocateExclusivePage();
    static void forget(_Page* page);
    void deallocatePageExtensions();
    bool reclaimArray(void* address);
    static _Page* getPage(void* address);
    bool extend(void* address, size_t size);

private:
    _Page** getLastExtensionPageLocation();
    static void deallocateExtensionsOfPage(_Page* page);
    bool freeExtensionPage(_Page* page);

    _Page* currentPage;
    _Page** nextExtensionPageLocation;
    void* nextObject;
};

}

#endif // __Scaly_Page__

