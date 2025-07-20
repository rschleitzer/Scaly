#include "../../scaly.h"
namespace scaly {
namespace compiler {
namespace Plan {


Type::Type(String name) : name(name) {}

Function::Function(String name, Vector<Type> input, Type output) : name(name), input(input), output(output) {}

Module::Module(String path, String name) : path(path), name(name) {}

}

}
}