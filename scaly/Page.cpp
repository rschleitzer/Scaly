#include "Scaly.h"
namespace scaly {

extern __thread _Task* __CurrentTask;

extern size_t pagesAllocated;

void _Page::reset() {
    // Allocate default extension page pointer and initialize it to zero
    *getExtensionPageLocation() = 0;
    exclusivePages = 0;
    nextObjectOffset = sizeof(_Page);
    currentPage = this;
}

void _Page::clear() {
    deallocateExtensions();
    reset();
}
    
bool _Page::isOversized() {
    return currentPage == nullptr;
}

_Page** _Page::getExtensionPageLocation() {
    // The extension page address is store at the very end of the page
    return ((_Page**) ((char*) this + _pageSize)) - 1;
}

void* _Page::getNextObject() {
    return (char*)this + nextObjectOffset;
}

void _Page::setNextObject(void* object) {
    nextObjectOffset = (char*)object - (char*)this;
}

_Page** _Page::getNextExclusivePageLocation() {
    return getExtensionPageLocation() - exclusivePages - 1;
}

void* _Page::allocateObject(size_t size) {
    if (this != currentPage) {
        // We're already known to be full, so we delegate to the current page
        void* newObject = currentPage->allocateObject(size);
        // Possibly our current page was also full so we propagate back the new current page
        _Page* allocatingPage = ((Object*)newObject)->_getPage();
        if ((allocatingPage != currentPage) && (!allocatingPage->isOversized()))
            currentPage = allocatingPage;
        return newObject;
    }

    // Try to allocate from ourselves
    void* location = getNextObject();
    void* nextLocation = align((char*)location + size);
    if (nextLocation <= (void*) getNextExclusivePageLocation()) {
        setNextObject(nextLocation);
        return location;
    }

    // So the space did not fit.

    // Calculate gross size to decide whether we're oversized
    size_t grossSize = sizeof(_Page) + size + sizeof(_Page**) + _alignment;
    if (grossSize > _pageSize) {
        if ((_Page**)getNextObject() == getExtensionPageLocation()) {
            // Allocate an extension page with default size
            _Page* extensionPage = allocateExtensionPage();
            // Try again with the new extension page
            return extensionPage->allocateObject(size);
        }

        // We allocate oversized objects directly.
        _Page* page;
        posix_memalign((void**)&page, _pageSize, size + sizeof(_Page));
        page->reset();
        page->currentPage = nullptr;
        *getNextExclusivePageLocation() = page;
        exclusivePages++;
        return ((char*)page) + sizeof(_Page);
    }

    // So we're not oversized. Create extension page and let it allocate.
    return allocateExtensionPage()->allocateObject(size);
}

_Page* _Page::allocateExtensionPage() {
    _Page* extensionPage = __CurrentTask->getExtensionPage();
    if (!extensionPage)
        return 0;
    extensionPage->reset();
    *getExtensionPageLocation() = extensionPage;
    currentPage = extensionPage;
    return extensionPage;
}

_Page* _Page::allocateExclusivePage() {
    if (this != currentPage) {
        // We're already known to be full, so we delegate to the current page
        return currentPage->allocateExclusivePage();
    }

    // Check first whether we need an ordinary extension
    if ((_Page**)getNextObject() >= getNextExclusivePageLocation()) {
        // Allocate an extension page with default size
        allocateExtensionPage()->allocateExclusivePage();
    }

    _Page* exclusivePage = __CurrentTask->getExtensionPage();
    if (!exclusivePage)
        return 0;

    exclusivePage->reset();
    *getNextExclusivePageLocation() = exclusivePage;
    exclusivePages++;
    return exclusivePage;
}

bool _Page::extend(void* address, size_t size) {
    if (!size)
        size = 1;
    void* nextLocation = align((char*) address + size);
    // If nextObject would not change because of the alignment, that's it
    if (nextLocation == (void*)getNextObject())
        return true;
    // If nextObject is already higher, other objects were allocated in the meantime
    if (nextLocation < getNextObject())
        return false;
    // Now we still have to check whether we still would have space left
    if (nextLocation >= (void*) getNextExclusivePageLocation())
        return false;
    // Allocate the extension
    setNextObject(nextLocation);
    return true; 
}

void _Page::deallocateExtensions() {
    for (_Page* page = this; page;) {
        _Page* nextExtensionPage = *page->getExtensionPageLocation();
        // Deallocate oversized or exclusive pages
        _Page** ppPage = page->getExtensionPageLocation() - 1;
        for (int i = 0; i < page->exclusivePages; i++) {
            if (!(*ppPage)->isOversized())
                (*ppPage)->deallocateExtensions();
            forget(*ppPage);
            ppPage--; }
        if (page != this)
            forget(page);
        page = nextExtensionPage;
    }
}

void _Page::forget(_Page* page) {
    __CurrentTask->releaseExtensionPage(page);
}

bool _Page::reclaimArray(void* address) {
    // Quick attempt to find it at tue current page
    if (currentPage->deallocateExclusivePage((_Page*)address))
        return true;
    // Second attempt scanning the rest of the chain
    for (_Page* page = this; page != currentPage; page = *page->getExtensionPageLocation())
        if (page->deallocateExclusivePage((_Page*)address))
            return true;
    
    // If we arrive here, we have a memory leak.
    return false;
}

_Page* _Page::getPage(void* address) {
    return (_Page*) (((intptr_t)address) & ~(intptr_t)(_pageSize - 1));
}

bool _Page::deallocateExclusivePage(_Page* _page) {
    // Find the extension Page pointer
    _Page** ppPage = getExtensionPageLocation() - 1;
    _Page** nextExtensionPageLocation = getNextExclusivePageLocation();
    while (ppPage > nextExtensionPageLocation) {
        if (*ppPage == _page)
            break;
        ppPage--;
    }

    // Report if we could not find it
    if (ppPage == nextExtensionPageLocation)
        return false;

    // Shift the remaining array one position up
    for (; ppPage > nextExtensionPageLocation; ppPage--)
        *ppPage = *(ppPage - 1);
    // Make room for one more extension
    exclusivePages--;
    forget(_page);
    return true;
}

} // namespace
