#include "../../scaly.h"
namespace scaly {
namespace compiler {
namespace Plan {


Module::Module(String path, String name, Vector<Type> types, HashMap<String, Type> type_map, Vector<Function> functions, HashMap<String, Function> function_map) : path(path), name(name), types(types), type_map(type_map), functions(functions), function_map(function_map) {}

Type::Type(String name, Vector<String> fields) : name(name), fields(fields) {}

Function::Function(String name, Vector<Argument> input, String output, Vector<Block> blocks) : name(name), input(input), output(output), blocks(blocks) {}

Argument::Argument(String name, String type) : name(name), type(type) {}

Block::Block(String name, Vector<Instruction> instructions) : name(name), instructions(instructions) {}

FMul::FMul(String l, String r, String result) : l(l), r(r), result(result) {}

Ret::Ret(String v) : v(v) {}
Instruction::Instruction(struct FMul _FMul) : _tag(FMul), _FMul(_FMul) {}

Instruction::Instruction(struct Ret _Ret) : _tag(Ret), _Ret(_Ret) {}

Instruction::Instruction(struct RetVoid _RetVoid) : _tag(RetVoid), _RetVoid(_RetVoid) {}


}

}
}