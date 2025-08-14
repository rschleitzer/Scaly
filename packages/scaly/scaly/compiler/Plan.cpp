#include "../../scaly.h"
namespace scaly {
namespace compiler {
namespace Plan {


Compilation::Compilation(String path, String name, HashMap<String, Type> types, HashMap<String, Function> functions) : path(path), name(name), types(types), functions(functions) {}

Type::Type(String name, Vector<String> fields) : name(name), fields(fields) {}

Function::Function(String name, Vector<Argument> input, String output, Vector<Block> blocks) : name(name), input(input), output(output), blocks(blocks) {}

Argument::Argument(String name, String type) : name(name), type(type) {}

Block::Block(String name, Vector<Instruction> instructions) : name(name), instructions(instructions) {}

Instruction::Instruction(String* result, String name, Vector<String> args) : result(result), name(name), args(args) {}

}

}
}