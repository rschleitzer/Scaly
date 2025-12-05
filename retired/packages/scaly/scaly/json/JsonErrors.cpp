#include "../../scaly.h"
namespace scaly {
namespace json {
using namespace scaly::containers;


InvalidSyntax::InvalidSyntax(size_t start, size_t end, String expected) : start(start), end(end), expected(expected) {}
ParserError::ParserError(struct DifferentSyntax _Different) : _tag(Different), _Different(_Different) {}

ParserError::ParserError(struct InvalidSyntax _Invalid) : _tag(Invalid), _Invalid(_Invalid) {}


}
}