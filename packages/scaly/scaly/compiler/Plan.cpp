#include "../../scaly.h"
namespace scaly {
namespace compiler {
namespace Plan {


Type::Type(String name, Vector<String> fields) : name(name), fields(fields) {}

Function::Function(String name, Vector<String> input, String* output) : name(name), input(input), output(output) {}

Module::Module(String path, String name, HashMap<String, Function> functions) : path(path), name(name), functions(functions) {}

}

}
}