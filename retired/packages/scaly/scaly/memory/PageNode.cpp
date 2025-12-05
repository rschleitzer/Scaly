#include "../../scaly.h"
namespace scaly {
namespace memory {

PageNode::PageNode(Page* page, PageNode* next) : page(page), next(next) {}

}
}