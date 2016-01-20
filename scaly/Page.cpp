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
    void* location = currentPage->allocateLocal(size);

    if (!location)
    {
        currentPage = allocateExtensionPage(size);
        location = currentPage->allocateLocal(size);
    }

    return location;
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

_Page* _Page::allocateExtensionPage(size_t size) {
    size_t grossSize = size + sizeof(_Page) + sizeof(_Page**) + 2 * _alignment;
    if (grossSize < pageSize)
        grossSize = pageSize;

    _Page* pageLocation = allocate(grossSize);
    if (!pageLocation)
        return 0;

    *nextExtensionPageLocation = pageLocation;
    nextExtensionPageLocation--;
    _Page* extensionPage = new (pageLocation) _Page(grossSize);
    extensionPage->extendingPage = this;
    return extensionPage;
}

_Page* _Page::allocate(size_t size) {
    void* pageLocation;
    
    if (posix_memalign(&pageLocation, pageSize, size))
        return 0;
    else
        return (_Page*)pageLocation;
}

void* _Page::allocateLocal(size_t size) {
    if (!size)
        size = 1;

    void* nextLocation = align((char*) nextObject + size);

    if (nextLocation >= (void*) nextExtensionPageLocation)
        return 0;

    void* location = nextObject;
    nextObject = nextLocation;
    return location;
}

void _Page::deallocate() {
    // Find the end of the stack Page chain
    _Page* currentPage = this;
    while (currentPage)
        if (currentPage->nextPage == 0)
            break;
        else
            currentPage = currentPage->nextPage;

    // Deallocate the chain starting from the end
    while (currentPage) {
        _Page* previousPage = currentPage->previousPage;
        currentPage->deallocatePageExtensions();
        free(currentPage);
        currentPage = previousPage;
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
    _Page* _Page = ((Object*)address)->getPage();
    _Page->extendingPage->freeExtensionPage(_Page);
}

_Page* _Page::getPage(void* address) {
    return (_Page*)
    (
        ((intptr_t)address)

    & ~(intptr_t)(pageSize - 1));    
}

bool _Page::extend(void* address, size_t size) {
    if (!size)
        size = 1;

    void* nextLocation = align((char*) address + size);

    if (nextLocation >= (void*) nextExtensionPageLocation)
        return false;

    nextObject = nextLocation;
    return true;
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
    _Page** lastExtensionPosition = getLastExtensionPageLocation();
    _Page** firstExtensionPosition =nextExtensionPageLocation;
    for (_Page** shiftedPosition = extensionPosition; extensionPosition > firstExtensionPosition; extensionPosition--)
        *shiftedPosition = *(shiftedPosition - 1);
        
    // Make room for one more extension
    nextExtensionPageLocation++;
    
    _page->deallocate();
}

} // namespace
