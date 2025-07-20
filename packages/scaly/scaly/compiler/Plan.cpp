#include "../../scaly.h"
namespace scaly {
namespace compiler {
namespace Planner {


Type::Type(String name) : name(name) {}

Function::Function(String name, Vector<Type> input, Type output) : name(name), input(input), output(output) {}

Plan::Plan(String path, String name) : path(path), name(name) {}

}

}
}