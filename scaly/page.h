#ifndef __scaly_Page__
#define __scaly_Page__

typedef struct scaly_Page scaly_Page;
struct scaly_Page {
    scaly_Page* currentPage;
    int nextObjectOffset;
    int exclusivePages;
};

void scaly_Page_reset();
void scaly_Page_clear();
void* allocateObject(size_t size);
scaly_Page* scaly_Page_allocateExclusivePage();
void scaly_Page_forget(scaly_Page* page);
void scaly_Page_deallocateExtensions();
int scaly_Page_reclaimArray(void* address);
scaly_Page* getPage(void* address);
int scaly_Page_extend(void* address, size_t size);
int scaly_Page_isOversized();
scaly_Page* scaly_Page_allocateExtensionPage();
scaly_Page** scaly_Page_getExtensionPageLocation();
int scaly_Page_deallocateExclusivePage(scaly_Page* page);
void* getNextObject();
void setNextObject(void* object);
scaly_Page** scaly_Page_getNextExclusivePageLocation();

#endif // __scaly_Page__ 


