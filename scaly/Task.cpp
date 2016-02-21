#include "Scaly.h"
namespace scaly{

__thread _Task* __CurrentTask = 0;

_Task::_Task() {
chunk = new(getPage()) _Chunk(); }

_Page* _Task::getExtensionPage() {
    _Page* page = chunk->allocatePage();
    if (!page)
        return 0;
    return page; }

void _Task::releaseExtensionPage(_Page* page) {
    chunk->deallocatePage(page); }

void _Task::dispose() {
    chunk->dispose(); }

}