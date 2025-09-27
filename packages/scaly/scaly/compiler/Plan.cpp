#include "../../scaly.h"
namespace scaly {
namespace compiler {
namespace Plan {


Source::Source(String path, String name) : path(path), name(name) {}

Type::Type(String name, Vector<String> fields) : name(name), fields(fields) {}

Function::Function(Source* source, String name, Vector<Argument> input, String output, Vector<Block> blocks) : source(source), name(name), input(input), output(output), blocks(blocks) {}

Argument::Argument(String name, String type) : name(name), type(type) {}

Block::Block(String name, Vector<Instruction> instructions) : name(name), instructions(instructions) {}

Instruction::Instruction(String* result, String name, Vector<String> args) : result(result), name(name), args(args) {}

Program::Program(Source* source, HashMap<String, Type> types, HashMap<String, Function> functions) : source(source), types(types), functions(functions) {}

}

}
}