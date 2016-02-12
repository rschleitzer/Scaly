#include "Scaly.h"
namespace scaly {

_Page::_Page(size_t size)
: currentPage(this), nextPage(0), previousPage(0), size(size), extendingPage(0) {
	reset();
}

_Page** _Page::getLastExtensionPageLocation() {
    return ((_Page**) ((char*) this + size)) - 1;
}

_Page* _Page::next() {
    if (nextPage == 0)
        nextPage = allocateNextPage(this);
    else
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
    if ((_Page**)nextObject == nextExtensionPageLocation) {
        // Allocate an extension page with default size
        _Page* defaultExtensionPage = allocateExtensionPage(1);
        // Make it our new current
        currentPage = defaultExtensionPage;
        // Try again with the new extension page
        return defaultExtensionPage->allocateObject(size);
    }
    
    // Make the extension page with the desired size.
    _Page* extensionPage = allocateExtensionPage(size);

    // If it was not an oversize page
    if (extensionPage->getSize() <= _Page::pageSize) {
        // If we are pretty full, make it the new current page
        if (((void*)nextObject) >= (void*)nextExtensionPageLocation )
            currentPage = extensionPage;
    }
    
    return extensionPage->allocateObject(size);
}

_Page* _Page::allocateExtensionPage(size_t size) {
    size_t grossSize = size + sizeof(_Page) + sizeof(_Page**);
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

    *nextExtensionPageLocation = pageLocation;
    nextExtensionPageLocation--;
    _Page* extensionPage = new (pageLocation) _Page(grossSize);
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
    nextExtensionPageLocation = getLastExtensionPageLocation();
    *nextExtensionPageLocation = 0;
    nextObject = align((char*) this + sizeof(_Page));

}

_Page* _Page::allocate(size_t size) {
    void* pageLocation;
    
    if (posix_memalign(&pageLocation, pageSize, size))
        return 0;
    else
        return (_Page*)pageLocation;
}

bool _Page::extend(void* address, size_t size) {
    if (!size)
        size = 1;

    void* nextLocation = align((char*) address + size);

    if (nextLocation == (void*) nextObject)
        return true;

    if (nextLocation < nextObject)
        return false;
        
    if (nextLocation >= (void*) nextExtensionPageLocation)
        return false;

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
        _Page* previousPage = page->previousPage;
        page->deallocatePageExtensions();
        free(page);
        page = previousPage;
    }
}

void _Page::deallocatePageExtensions() {
    _Page** currentExtensionPosition = this->getLastExtensionPageLocation();
    while (currentExtensionPosition > this->nextExtensionPageLocation) {
        _Page* currentExtension = *currentExtensionPosition;
        if (currentExtension)
            currentExtension->deallocate();
        currentExtensionPosition--;
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
    _Page** extensionPosition = getLastExtensionPageLocation();
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
    
    _page->deallocate();
}

size_t _Page::getSize() {
    return size;
}

} // namespace
