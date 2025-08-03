#include "../../scaly.h"
namespace scaly {
namespace compiler {
namespace Plan {


Module::Module(String path, String name, Vector<Type> types, HashMap<String, Type> type_map, Vector<Function> functions, HashMap<String, Function> function_map) : path(path), name(name), types(types), type_map(type_map), functions(functions), function_map(function_map) {}

Type::Type(String name, Vector<String> fields) : name(name), fields(fields) {}

Function::Function(String name, Vector<Argument> input, String output, Vector<Block> blocks) : name(name), input(input), output(output), blocks(blocks) {}

Argument::Argument(String name, String type) : name(name), type(type) {}

Block::Block(String name, Vector<Instruction> instructions) : name(name), instructions(instructions) {}

Instruction::Instruction(String* result, String name, Vector<String> args) : result(result), name(name), args(args) {}

}

}
}