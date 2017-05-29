#include "scaly.h"

extern scaly_Task* __CurrentTask;
extern scaly_Page* __CurrentPage;

extern size_t pagesAllocated;

scaly_Page* scaly_Page_alloc() {
    __CurrentPage = (scaly_Page*)(((char*)__CurrentPage) + scaly_pageSize);
    return __CurrentPage;
}

void scaly_Page_free() {
    scaly_Page_deallocateExtensions(__CurrentPage);
    __CurrentPage = (scaly_Page*)(((char*)__CurrentPage) - scaly_pageSize);
}

void scaly_Page_reset(scaly_Page* this) {
    // Allocate default extension page pointer and initialize it to zero
    *scaly_Page_getExtensionPageLocation(this) = 0;
    this->exclusivePages = 0;
    this->nextObjectOffset = sizeof(scaly_Page);
    this->currentPage = this;
}

void scaly_Page_clear(scaly_Page* this) {
    scaly_Page_deallocateExtensions(this);
    scaly_Page_reset(this);
}
    
int scaly_Page_isOversized(scaly_Page* this) {
    return this->currentPage == 0;
}

scaly_Page** scaly_Page_getExtensionPageLocation(scaly_Page* this) {
    // The extension page address is store at the very end of the page
    return ((scaly_Page**) ((char*) this + scaly_pageSize)) - 1;
}

void* scaly_Page_getNextObject(scaly_Page* this) {
    return (char*)this + this->nextObjectOffset;
}

void scaly_Page_setNextObject(scaly_Page* this, void* object) {
    this->nextObjectOffset = (char*)object - (char*)this;
}

scaly_Page** scaly_Page_getNextExclusivePageLocation(scaly_Page* this) {
    return scaly_Page_getExtensionPageLocation(this) - this->exclusivePages - 1;
}

void* scaly_Page_allocateObject(scaly_Page* this, size_t size) {
    if (this != this->currentPage) {
        // We're already known to be full, so we delegate to the current page
        void* newObject = scaly_Page_allocateObject(this->currentPage, size);
        // Possibly our current page was also full so we propagate back the new current page
        scaly_Page* allocatingPage = scaly_Page_getPage(newObject);
        if ((allocatingPage != this->currentPage) && (!scaly_Page_isOversized(allocatingPage)))
            this->currentPage = allocatingPage;
        return newObject;
    }

    // Try to allocate from ourselves
    void* location = scaly_Page_getNextObject(this);
    void* nextLocation = scaly_align((char*)location + size);
    if (nextLocation <= (void*) scaly_Page_getNextExclusivePageLocation(this)) {
        scaly_Page_setNextObject(this, nextLocation);
        return location;
    }

    // So the space did not fit.

    // Calculate gross size to decide whether we're oversized
    size_t grossSize = sizeof(scaly_Page) + size + sizeof(scaly_Page**) + scaly_alignment;
    if (grossSize > scaly_pageSize) {
        if ((scaly_Page**)scaly_Page_getNextObject(this) == scaly_Page_getExtensionPageLocation(this)) {
            // Allocate an extension page with default size
            scaly_Page* extensionPage = scaly_Page_allocateExtensionPage(this);
            // Try again with the new extension page
            return scaly_Page_allocateObject(extensionPage, size);
        }

        // We allocate oversized objects directly.
        scaly_Page* page;
        posix_memalign((void**)&page, scaly_pageSize, size + sizeof(scaly_Page));
        scaly_Page_reset(page);
        page->currentPage = 0;
        *scaly_Page_getNextExclusivePageLocation(this) = page;
        this->exclusivePages++;
        return ((char*)page) + sizeof(scaly_Page);
    }

    // So we're not oversized. Create extension page and let it allocate.
    return scaly_Page_allocateObject(scaly_Page_allocateExtensionPage(this), size);
}

scaly_Page* scaly_Page_allocateExtensionPage(scaly_Page* this) {
    scaly_Page* extensionPage = scaly_Task_getExtensionPage(__CurrentTask);
    if (!extensionPage)
        return 0;
    scaly_Page_reset(extensionPage);
    *scaly_Page_getExtensionPageLocation(this) = extensionPage;
    this->currentPage = extensionPage;
    return extensionPage;
}

scaly_Page* scaly_Page_allocateExclusivePage(scaly_Page* this) {
    if (this != this->currentPage) {
        // We're already known to be full, so we delegate to the current page
        return scaly_Page_allocateExclusivePage(this->currentPage);
    }

    // Check first whether we need an ordinary extension
    if ((scaly_Page**) scaly_Page_getNextObject(this) >= scaly_Page_getNextExclusivePageLocation(this)) {
        // Allocate an extension page with default size
        scaly_Page_allocateExclusivePage(scaly_Page_allocateExtensionPage(this));
    }

    scaly_Page* exclusivePage = scaly_Task_getExtensionPage(__CurrentTask);
    if (!exclusivePage)
        return 0;

    scaly_Page_reset(exclusivePage);
    *scaly_Page_getNextExclusivePageLocation(this) = exclusivePage;
    this->exclusivePages++;
    return exclusivePage;
}

int scaly_Page_extend(scaly_Page* this, void* address, size_t size) {
    if (!size)
        size = 1;
    void* nextLocation = scaly_align((char*) address + size);
    // If nextObject would not change because of the alignment, that's it
    if (nextLocation == (void*)scaly_Page_getNextObject(this))
        return 1;
    // If nextObject is already higher, other objects were allocated in the meantime
    if (nextLocation < scaly_Page_getNextObject(this))
        return 0;
    // Now we still have to check whether we still would have space left
    if (nextLocation >= (void*) scaly_Page_getNextExclusivePageLocation(this))
        return 0;
    // Allocate the extension
    scaly_Page_setNextObject(this, nextLocation);
    return 1; 
}

void scaly_Page_deallocateExtensions(scaly_Page* this) {
    for (scaly_Page* page = this; page;) {
        scaly_Page* nextExtensionPage = *scaly_Page_getExtensionPageLocation(page);
        // Deallocate oversized or exclusive pages
        scaly_Page** ppPage = scaly_Page_getExtensionPageLocation(page) - 1;
        for (int i = 0; i < page->exclusivePages; i++) {
            if (!scaly_Page_isOversized(*ppPage))
                scaly_Page_deallocateExtensions(*ppPage);
            scaly_Page_forget(*ppPage);
            ppPage--; }
        if (page != this)
            scaly_Page_forget(page);
        page = nextExtensionPage;
    }
}

void scaly_Page_forget(scaly_Page* this) {
    scaly_Task_releaseExtensionPage(__CurrentTask, this);
}

int scaly_Page_reclaimArray(scaly_Page* this, void* address) {
    // Quick attempt to find it at tue current page
    if (scaly_Page_deallocateExclusivePage(this->currentPage, (scaly_Page*)address))
        return 1;
    // Second attempt scanning the rest of the chain
    for (scaly_Page* page = this; page != this->currentPage; page = *scaly_Page_getExtensionPageLocation(page))
        if (scaly_Page_deallocateExclusivePage(page, (scaly_Page*)address))
            return 1;
    
    // If we arrive here, we have a memory leak.
    return 0;
}

scaly_Page* scaly_Page_getPage(void* address) {
    return (scaly_Page*) (((intptr_t)address) & ~(intptr_t)(scaly_pageSize - 1));
}

int scaly_Page_deallocateExclusivePage(scaly_Page* this, scaly_Page* page) {
    // Find the extension Page pointer
    scaly_Page** ppPage = scaly_Page_getExtensionPageLocation(this) - 1;
    scaly_Page** nextExtensionPageLocation = scaly_Page_getNextExclusivePageLocation(this);
    while (ppPage > nextExtensionPageLocation) {
        if (*ppPage == page)
            break;
        ppPage--;
    }

    // Report if we could not find it
    if (ppPage == nextExtensionPageLocation)
        return 0;

    // Shift the remaining array one position up
    for (; ppPage > nextExtensionPageLocation; ppPage--)
        *ppPage = *(ppPage - 1);
    // Make room for one more extension
    this->exclusivePages--;
    scaly_Page_forget(page);
    return 1;
}
