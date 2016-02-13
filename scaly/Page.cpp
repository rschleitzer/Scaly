#include "Scaly.h"
namespace scaly {
    
// Some statistics
static int oversizedPagesAllocated;
static int oversizedBytesAllocated;
static int oversizedPagesDeallocated;
static int oversizedBytesDeallocated;
static int pagesRecycled;
static int pagesDisposed;

extern __thread _Task* __CurrentTask;

_Page::_Page(size_t size)
: currentPage(this), nextPage(0), previousPage(0), size(size), extendingPage(0) {
	reset();
}

_Page** _Page::getLastExtensionPageLocation() {
    // Frome where the ordinary extension page is pointed to
    return ((_Page**) ((char*) this + size)) - 1;
}

_Page* _Page::next() {
    if (nextPage == 0)
        // This frame is new
        nextPage = allocateNextPage(this);
    else
        // We've already been at that frame, we re-use it
        nextPage->reset();

    return nextPage;
}

_Page* _Page::previous() {
    return previousPage;
}

void* _Page::operator new(size_t size, void* location) {
    return location;
}

void* _Page::allocateObject(size_t size) {
    if (this != currentPage) {
        // We're already known to be full, so we delegate to the current page
        void* newObject = currentPage->allocateObject(size);

        // Possibly our current page was also full so we propagate back the new current page
        _Page* allocatingPage = ((Object*)newObject)->getPage();
        if ((allocatingPage != currentPage) && (allocatingPage->getSize() == _Page::pageSize))
            currentPage = allocatingPage;
        
        return newObject;
    }

    // Try to allocate from ourselves
    void* nextLocation = align((char*) nextObject + size);
    if (nextLocation <= (void*) nextExtensionPageLocation) {
        void* location = nextObject;
        nextObject = nextLocation;
        return location;
    }
            
    // So the space did not fit. 
    
    // Check first whether we need an ordinary extension
    size_t grossSize = sizeof(_Page) + size + sizeof(_Page**) + _alignment;

    if ((_Page**)nextObject == getLastExtensionPageLocation() || (grossSize < pageSize)) {
        // Allocate an extension page with default size
        _Page* defaultExtensionPage = allocateExtensionPage(1);
        // Make it our new current
        currentPage = defaultExtensionPage;
        // Try again with the new extension page
        return defaultExtensionPage->allocateObject(size);
    }
    
    // Make the oversized extension page.
    _Page* extensionPage = allocateExtensionPage(size);
    return extensionPage->allocateObject(size);
}

_Page* _Page::allocateExtensionPage(size_t size) {
    size_t grossSize = sizeof(_Page) + size + sizeof(_Page**) + _alignment;
    if (grossSize < pageSize) {
        grossSize = pageSize;
    }
    else if (grossSize > pageSize) {
        grossSize += _alignment;
        grossSize &= ~(_alignment - 1); 
    }
        
    _Page* pageLocation = allocate(grossSize);
    if (!pageLocation)
        return 0;

    _Page* extensionPage = new (pageLocation) _Page(grossSize);

    if (grossSize > pageSize) {
        *nextExtensionPageLocation = pageLocation;
        nextExtensionPageLocation--;
    }
    else {
        *getLastExtensionPageLocation() = pageLocation;
    }
    
    extensionPage->extendingPage = this;
    return extensionPage;
}

_Page* _Page::allocateNextPage(_Page* previousPage) {
    void* pageLocation = allocate(pageSize);
    if (!pageLocation)
        return 0;
        
    _Page* nextPage = new (pageLocation) _Page(pageSize);
    nextPage->previousPage = previousPage;
    return nextPage;
}

void _Page::reset() {
    // Allocate default extension page pointer and initialize it to zero
    nextExtensionPageLocation = getLastExtensionPageLocation();
    *nextExtensionPageLocation = 0;
    nextExtensionPageLocation--;
    // Allocate space for the page itself
    nextObject = align((char*) this + sizeof(_Page));
    currentPage = this;
}

_Page* _Page::allocate(size_t size) {
    void* pageLocation;
    if (size > _Page::pageSize) {
        if (posix_memalign(&pageLocation, _Page::pageSize, size)) {
            return 0;
        }
        else {
            oversizedPagesAllocated++;
            oversizedBytesAllocated += size;
            return (_Page*)pageLocation;
        }
    }
    else {
        return __CurrentTask->recycle();
    }
}

bool _Page::extend(void* address, size_t size) {
    if (!size)
        size = 1;

    void* nextLocation = align((char*) address + size);

    // If nextObject would not change because of the alignment, that's it
    if (nextLocation == (void*) nextObject)
        return true;

    // If nextObject is already higher, other objects were allocated in the meantime
    if (nextLocation < nextObject)
        return false;
        
    // Now we still have to check whether we still would have space left
    if (nextLocation >= (void*) nextExtensionPageLocation)
        return false;

    // Allocate the extension
    nextObject = nextLocation;
    return true;
}

void _Page::deallocate() {
    // Find the end of the stack Page chain
    _Page* page = this;
    while (page)
        if (page->nextPage == 0)
            break;
        else
            page = page->nextPage;

    // Deallocate the chain starting from the end
    while (page) {
        _Page* prevPage = page->previousPage;
        forget(page);
        page = prevPage;
    }
}

void _Page::deallocatePageExtensions() {
    // Find the end of the extension Page chain
    _Page* page = this;
    while (page)
        if (*(page->getLastExtensionPageLocation()) == 0)
            break;
        else
            page = *(page->getLastExtensionPageLocation());
    
    // Deallocate the extension page chain starting from the end
    while (page) {
        _Page* prevExtPage = page->extendingPage;

        // Deallocate the standard extension page if applicable
        _Page** ppPage = page->getLastExtensionPageLocation();
        if (*ppPage)
            forget(*ppPage);

        // Deallocate the oversized pages        
        for (ppPage--; ppPage > page->nextExtensionPageLocation; ppPage--)
            forget(*ppPage);

        page = prevExtPage;
    }
}

void _Page::forget(_Page* page) {
    if (page->size > pageSize) {
        oversizedBytesDeallocated += page->size;
        free(page);
        oversizedPagesDeallocated++;
    }
    else {
        __CurrentTask->dispose(page);
        pagesDisposed++;
    }
}

void _Page::reclaimArray(void* address) {
    _Page* page = ((Object*)address)->getPage();
    page->extendingPage->freeExtensionPage(page);
}

_Page* _Page::getPage(void* address) {
    return (_Page*)
    (
        ((intptr_t)address)

    & ~(intptr_t)(pageSize - 1));    
}

void _Page::freeExtensionPage(_Page* _page) {
    // Find the extension Page pointer
    _Page** extensionPosition = getLastExtensionPageLocation() - 1;
    while (extensionPosition > nextExtensionPageLocation) {
        if (*extensionPosition == _page)
            break;

        extensionPosition--;
    }

    // Shift the remaining array one position up
    for (_Page** shiftedPosition = extensionPosition; shiftedPosition > nextExtensionPageLocation; shiftedPosition--)
        *shiftedPosition = *(shiftedPosition - 1);
        
    // Make room for one more extension
    nextExtensionPageLocation++;
    
    forget(_page);
}

size_t _Page::getSize() {
    return size;
}

void _Page::initStatistics() {
    pagesRecycled = 0;
    pagesDisposed = 0;
    oversizedBytesAllocated = 0;
}

} // namespace
