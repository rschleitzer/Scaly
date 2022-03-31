#ifndef __Scaly_Page__
#define __Scaly_Page__
namespace scaly {

class _Page {
public:
    void reset();
    void clear();
    void* allocateObject(size_t size);
    _Page* allocateExclusivePage();
    static void forget(_Page* page);
    void deallocateExtensions();
    bool reclaimArray(void* address);
    static _Page* getPage(void* address);
    bool extend(void* address, size_t size);
    bool isOversized();

private:
    _Page* allocateExtensionPage();
    _Page** getExtensionPageLocation();
    bool deallocateExclusivePage(_Page* page);
    void* getNextObject();
    void setNextObject(void* object);
    _Page** getNextExclusivePageLocation();

    _Page* currentPage;
    int nextObjectOffset;
    int exclusivePages;
};

}

#endif // __Scaly_Page__

