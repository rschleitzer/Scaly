#include "Scaly.h"
namespace scaly{

__thread _Task* __CurrentTask = 0;

_Task::_Task() {
pool = new(getPage()) _Pool(); }

_Page* _Task::getExtensionPage() {
    _Page* page = pool->allocatePage();
    if (!page)
        return 0;
    return page; }

void _Task::releaseExtensionPage(_Page* page) {
    if (!pool->deallocatePage(page))
        // This is an oversized page which has to be free'd directly
        free(page); }

void _Task::dispose() {
    pool->dispose(); }

}