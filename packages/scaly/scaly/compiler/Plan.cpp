#include "../../scaly.h"
namespace scaly {
namespace compiler {
namespace Plan {


Module::Module(String path, String name, Vector<Type> types, HashMap<String, Type> type_map, Vector<Function> functions, HashMap<String, Function> function_map) : path(path), name(name), types(types), type_map(type_map), functions(functions), function_map(function_map) {}

Type::Type(String name, Vector<String> fields) : name(name), fields(fields) {}

Instruction::Instruction(String name, String type, Vector<String> input) : name(name), type(type), input(input) {}

Block::Block(Vector<Instruction> instructions) : instructions(instructions) {}

Function::Function(String name, Vector<String> input, String output, Vector<Block> blocks) : name(name), input(input), output(output), blocks(blocks) {}

}

}
}