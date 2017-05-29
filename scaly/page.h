#ifndef __scaly_page__
#define __scaly_page__

static const int scaly_alignment = 8;
static const size_t scaly_pageSize = 0x1000;
static const size_t scaly_maxStackPages = 0x100;

typedef struct scaly_Page scaly_Page; struct scaly_Page {
    struct scaly_Page* currentPage;
    int nextObjectOffset;
    int exclusivePages;
};

scaly_Page* scaly_Page_alloc();
void scaly_Page_free();
void scaly_Page_reset(scaly_Page* this);
void scaly_Page_clear(scaly_Page* this);
void* scaly_Page_allocateObject(scaly_Page* this, size_t size);
scaly_Page* scaly_Page_allocateExclusivePage(scaly_Page* this);
void scaly_Page_forget(scaly_Page* this);
void scaly_Page_deallocateExtensions(scaly_Page* this);
int scaly_Page_reclaimArray(scaly_Page* this, void* address);
scaly_Page* scaly_Page_getPage(void* address);
int scaly_Page_extend(scaly_Page* this, void* address, size_t size);
int scaly_Page_isOversized(scaly_Page* this);
scaly_Page* scaly_Page_allocateExtensionPage(scaly_Page* this);
scaly_Page** scaly_Page_getExtensionPageLocation(scaly_Page* this);
int scaly_Page_deallocateExclusivePage(scaly_Page* this, scaly_Page* page);
void* scaly_Page_getNextObject(scaly_Page* this);
void scaly_Page_setNextObject(scaly_Page* this, void* object);
scaly_Page** scaly_Page_getNextExclusivePageLocation(scaly_Page* this);

#endif // __scaly_page__ 


