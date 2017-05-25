#include "scaly.h"

scaly_Task* new_scaly_Task(scaly_Page* _page) {
    scaly_Task* this = (scaly_Task*) scaly_Page_allocateObject(_page, sizeof(scaly_Task));

    this->pool = new_scaly_Pool(scaly_Page_getPage(this));

    return this;
}

scaly_Page* scaly_Task_getExtensionPage(scaly_Task* this) {
    scaly_Page* page = scaly_Pool_allocatePage(this->pool);
    if (!page)
        return 0;
    return page; }

void scaly_Task_releaseExtensionPage(scaly_Task* this, scaly_Page* page) {
    if (!scaly_Pool_deallocatePage(this->pool, page))
        // This is an oversized page which has to be free'd directly
        free(page);
}

void scaly_Task_dispose(scaly_Task* this) {
    scaly_Pool_dispose(this->pool);
}
