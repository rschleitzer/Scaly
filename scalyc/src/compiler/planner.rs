use crate::compiler::modeler::ConstantType::Long;

pub struct Planner {}

impl Planner {
    pub fn add_function(_plan: &mut Plan, model_function: &crate::compiler::modeler::Function) {
        let mut function = Function::new(model_function.name.clone());
        let mut block = Block::new();
        for operation in &model_function.operations {
            let mut value: Option<Value> = None;
            for operand in &operation.operands {
                match operand {
                    crate::compiler::modeler::Operand::Constant(constant_type) => {
                        value = match constant_type {
                            Long(value) => Some(Value::Constant(Constant {
                                constant_type: ConstantType::Int64Constant,
                                value: *value,
                            })),
                            _ => panic!("This type is not supported yet."),
                        };
                        break;
                    }
                };
            }

            match value {
                Some(value) => block
                    .instructions
                    .push(Instruction::Ret(Ret { value: value })),
                None => panic!("Nothing to return."),
            };
        }
        function.blocks.push(block);
        _plan.functions.push(function);
    }
}

pub struct Plan {
    pub functions: Vec<Function>,
}

impl Plan {
    pub fn new() -> Plan {
        Plan {
            functions: Vec::new(),
        }
    }
}

pub struct Function {
    pub name: String,
    pub blocks: Vec<Block>,
}

impl Function {
    pub fn new(name: String) -> Function {
        Function {
            blocks: Vec::new(),
            name: name,
        }
    }
}

pub struct Block {
    pub instructions: Vec<Instruction>,
}

impl Block {
    pub fn new() -> Block {
        Block {
            instructions: Vec::new(),
        }
    }
}

pub enum Instruction {
    Ret(Ret),
}

pub struct Ret {
    value: Value,
}

pub enum Value {
    Constant(Constant),
}

pub struct Constant {
    constant_type: ConstantType,
    value: u64,
}

pub enum ConstantType {
    Int64Constant,
}
