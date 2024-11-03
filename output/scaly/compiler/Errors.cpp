#include "../../scaly.h"
namespace scaly {
namespace compiler {
using namespace scaly::containers;


InvalidSyntax::InvalidSyntax(size_t start, size_t end, String expected) : start(start), end(end), expected(expected) {}
ParserError::ParserError(DifferentSyntax _Different) : _tag(Different), _Different(_Different) {}

ParserError::ParserError(InvalidSyntax _Invalid) : _tag(Invalid), _Invalid(_Invalid) {}


}
}