#include "Scaly.h"
namespace scaly {

extern __thread _Task* __CurrentTask;

extern size_t pagesAllocated;

_Page::_Page() {
	reset(); }

void _Page::reset() {
    // Allocate default extension page pointer and initialize it to zero
    *getLastExtensionPageLocation() = 0;
    extensions = 0;
    nextObjectOffset = sizeof(_Page);
    currentPage = this; }

void _Page::clear() {
    deallocateExtensions();
    reset(); }

_Page** _Page::getLastExtensionPageLocation() {
    // From where the ordinary extension page is pointed to
    return ((_Page**) ((char*) this + _pageSize)) - 1; }

void* _Page::operator new(size_t size, void* location) {
    return location; }

void* _Page::getNextObject() {
    return (char*)this + nextObjectOffset; }

void _Page::setNextObject(void* object) {
    nextObjectOffset = (char*)object - (char*)this; }

_Page** _Page::getNextExtensionPageLocation() {
    return getLastExtensionPageLocation() - extensions - 1; }

void* _Page::allocateObject(size_t size) {
    if (this != currentPage) {
        // We're already known to be full, so we delegate to the current page
        void* newObject = currentPage->allocateObject(size);
        // Possibly our current page was also full so we propagate back the new current page
        _Page* allocatingPage = ((Object*)newObject)->_getPage();
        if ((allocatingPage != currentPage) && (newObject != allocatingPage))
            currentPage = allocatingPage;
        return newObject; }

    // Try to allocate from ourselves
    void* location = getNextObject();
    void* nextLocation = align((char*)location + size);
    if (nextLocation <= (void*) getNextExtensionPageLocation()) {
        setNextObject(nextLocation);
        return location; }

    // So the space did not fit.

    // Calculate gross size to decide whether we're oversized
    size_t grossSize = sizeof(_Page) + size + sizeof(_Page**) + _alignment;
    if (grossSize > _pageSize) {
        if ((_Page**)getNextObject() == getLastExtensionPageLocation()) {
            // Allocate an extension page with default size
            _Page* defaultExtensionPage = allocateExtensionPage();
            // Try again with the new extension page
            return defaultExtensionPage->allocateObject(size); }

        // We allocate oversized objects directly.
        void* object;
        posix_memalign(&object, _pageSize, size + sizeof(_Page));
        *getNextExtensionPageLocation() = (_Page*)object;
        extensions++;
        _Page* page = new (object) _Page();
        page->currentPage = nullptr;
        return ((char*)object) + sizeof(_Page); }
    // So we're not oversized. Allocate the standard extension page.
    _Page* extensionPage = allocateExtensionPage();
    // And allocate at last.
    return extensionPage->allocateObject(size); }

_Page* _Page::allocateExtensionPage() {
    _Page* pageLocation = __CurrentTask->getExtensionPage();
    if (!pageLocation)
        return 0;
    _Page* extensionPage = new (pageLocation) _Page();
    *getLastExtensionPageLocation() = pageLocation;
    currentPage = extensionPage;
    return extensionPage; }

_Page* _Page::allocateExclusivePage() {
    if (this != currentPage)
        // We're already known to be full, so we delegate to the current page
        return currentPage->allocateExclusivePage();

    // Check first whether we need an ordinary extension
    if ((_Page**)getNextObject() == getNextExtensionPageLocation()) {
        // Allocate an extension page with default size
        _Page* defaultExtensionPage = allocateExtensionPage();
        // Try again with the new extension page
        return defaultExtensionPage->allocateExclusivePage(); }

    _Page* pageLocation = __CurrentTask->getExtensionPage();
    if (!pageLocation)
        return 0;

    _Page* exclusivePage = new (pageLocation) _Page();
    *getNextExtensionPageLocation() = pageLocation;
    extensions++;
    return exclusivePage; }

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
    if (nextLocation >= (void*) getNextExtensionPageLocation())
        return false;
    // Allocate the extension
    setNextObject(nextLocation);
    return true; }

void _Page::deallocateExtensions() {
    for (_Page* page = this; page;) {
        _Page* nextExtensionPage = *page->getLastExtensionPageLocation();
        // Deallocate oversized or exclusive pages
        _Page** ppPage = page->getLastExtensionPageLocation() - 1;
        for (int i = 0; i < page->extensions; i++) {
            if ((*ppPage)->currentPage != nullptr)
                (*ppPage)->deallocateExtensions();
            forget(*ppPage);
            ppPage--; }
        if (page != this)
            forget(page);
        page = nextExtensionPage; } }

void _Page::forget(_Page* page) {
    __CurrentTask->releaseExtensionPage(page); }

bool _Page::reclaimArray(void* address) {
    // Quick attempt to find it at tue current page
    if (currentPage->freeExtensionPage((_Page*)address))
        return true;
    // Second attempt scanning the rest of the chain
    for (_Page* page = this; page != currentPage; page = *page->getLastExtensionPageLocation())
        if (page->freeExtensionPage((_Page*)address))
            return true; }

_Page* _Page::getPage(void* address) {
    return (_Page*) (((intptr_t)address) & ~(intptr_t)(_pageSize - 1)); }

bool _Page::freeExtensionPage(_Page* _page) {
    // Find the extension Page pointer
    _Page** ppPage = getLastExtensionPageLocation() - 1;
    _Page** nextExtensionPageLocation = getNextExtensionPageLocation();
    while (ppPage > nextExtensionPageLocation) {
        if (*ppPage == _page)
            break;
        ppPage--; }

    // Report if we could not find it
    if (ppPage == nextExtensionPageLocation)
        return false;

    // Shift the remaining array one position up
    for (; ppPage > nextExtensionPageLocation; ppPage--)
        *ppPage = *(ppPage - 1);
    // Make room for one more extension
    extensions--;
    forget(_page);
    return true; }

} // namespace
