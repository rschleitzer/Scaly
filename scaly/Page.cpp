#include "Scaly.h"
namespace scaly {

// Some statistics
static int oversizedPagesAllocated;
static int oversizedBytesAllocated;
static int oversizedPagesDeallocated;
static int oversizedBytesDeallocated;
static int pagesDisposed;
static int extensionPagesAllocated;

extern __thread _Task* __CurrentTask;

_Page::_Page()
: currentPage(this) {
	reset(); }

_Page** _Page::getLastExtensionPageLocation() {
    // Frome where the ordinary extension page is pointed to
    return ((_Page**) ((char*) this + _pageSize)) - 1; }

void* _Page::operator new(size_t size, void* location) {
    return location; }

void* _Page::getNextObject() {
    return (char*)this + nextObjectOffset; }

void _Page::setNextObject(void* object) {
    nextObjectOffset = (char*)object - (char*)this;
}

_Page** _Page::getNextExtensionPageLocation() {
    return getLastExtensionPageLocation() - extensions - 1;
}

void* _Page::allocateObject(size_t size) {
    if (this != currentPage) {
        // We're already known to be full, so we delegate to the current page
        void* newObject = currentPage->allocateObject(size);
        // Possibly our current page was also full so we propagate back the new current page
        _Page* allocatingPage = ((Object*)newObject)->getPage();
        if ((allocatingPage != currentPage) && (newObject != allocatingPage))
            currentPage = allocatingPage;
        return newObject; }

    // Try to allocate from ourselves
    void* nextLocation = align((char*)getNextObject() + size);
    if (nextLocation <= (void*) getNextExtensionPageLocation()) {
        void* location = getNextObject();
        setNextObject(nextLocation);
        return location; }

    // So the space did not fit.

    // Calculate gross size to decide whether we're oversized
    size_t grossSize = sizeof(_Page) + size + sizeof(_Page**) + _alignment;
    if (grossSize > _pageSize) {
        if ((_Page**)getNextObject() == getLastExtensionPageLocation()) {
            // Allocate an extension page with default size
            _Page* defaultExtensionPage = allocateExtensionPage();
            // Make it our new current
            currentPage = defaultExtensionPage;
            // Try again with the new extension page
            return defaultExtensionPage->allocateObject(size); }

        // We allocate and free oversized objects directly.
        void* object;
        posix_memalign(&object, _pageSize, size);
        *getNextExtensionPageLocation() = (_Page*)object;
        extensions++;
        return object; }
    // So we're not oversized. Allocate the standard extension page.
    _Page* extensionPage = allocateExtensionPage();
    this->currentPage = extensionPage;
    // And allocate at last.
    return extensionPage->allocateObject(size); }

_Page* _Page::allocateExtensionPage() {
    _Page* pageLocation = __CurrentTask->getExtensionPage();
    if (!pageLocation)
        return 0;
    _Page* extensionPage = new (pageLocation) _Page();
    *getLastExtensionPageLocation() = pageLocation;
    return extensionPage; }

_Page* _Page::allocateExclusivePage() {
    if (this != currentPage)
        // We're already known to be full, so we delegate to the current page
        return currentPage->allocateExclusivePage();

    // Check first whether we need an ordinary extension
    if ((_Page**)getNextObject() == getLastExtensionPageLocation()) {
        // Allocate an extension page with default size
        _Page* defaultExtensionPage = allocateExtensionPage();
        // Make it our new current
        currentPage = defaultExtensionPage;
        // Try again with the new extension page
        return defaultExtensionPage->allocateExclusivePage(); }

    _Page* pageLocation = __CurrentTask->getExtensionPage();
    if (!pageLocation)
        return 0;

    _Page* exclusivePage = new (pageLocation) _Page();
    *getNextExtensionPageLocation() = pageLocation;
    extensions++;
    return exclusivePage; }

void _Page::reset() {
    // Allocate default extension page pointer and initialize it to zero
    *getLastExtensionPageLocation() = 0;
    extensions = 0;
    // Allocate space for the page itself
    setNextObject(align((char*) this + sizeof(_Page)));
    currentPage = this; }

void _Page::clear() {
    deallocatePageExtensions();
    reset(); }

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

void _Page::deallocatePageExtensions() {
    for (_Page* page = this; page;) {
        _Page* nextExtensionPage = *page->getLastExtensionPageLocation();
        deallocateExtensionsOfPage(page);
        if (page != this)
            forget(page);
        page = nextExtensionPage; } }

void _Page::deallocateExtensionsOfPage(_Page* page) {
    // Deallocate the oversized or exclusive pages
    for (_Page** ppPage = page->getLastExtensionPageLocation() - 1; ppPage > page->getNextExtensionPageLocation(); ppPage--)
        forget(*ppPage); }

void _Page::forget(_Page* page) {
    __CurrentTask->releaseExtensionPage(page); }

bool _Page::reclaimArray(void* address) {
    if (currentPage->freeExtensionPage((_Page*)address))
        return true;
    for (_Page* page = this; page; page = *getLastExtensionPageLocation())
        if (page->freeExtensionPage(page))
            return true; }

_Page* _Page::getPage(void* address) {
    return (_Page*) (((intptr_t)address) & ~(intptr_t)(_pageSize - 1)); }

bool _Page::freeExtensionPage(_Page* _page) {
    // Find the extension Page pointer
    _Page** extensionPosition = getLastExtensionPageLocation() - 1;
    while (extensionPosition > getNextExtensionPageLocation()) {
        if (*extensionPosition == _page)
            break;
        extensionPosition--; }
    if (extensionPosition == getNextExtensionPageLocation())
        return false;

    // Shift the remaining array one position up
    for (_Page** shiftedPosition = extensionPosition; shiftedPosition > getNextExtensionPageLocation(); shiftedPosition--)
        *shiftedPosition = *(shiftedPosition - 1);
    // Make room for one more extension
    extensions--;
    forget(_page);
    return true; }

} // namespace
