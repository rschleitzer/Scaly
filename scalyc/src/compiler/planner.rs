use crate::compiler::modeler::ConstantType::Long;

pub struct Planner {}

impl Planner {
    pub fn add_function(plan: &mut Plan, model_function: &crate::compiler::modeler::Function) {
        let mut function = Function::new(model_function.name.clone());
        let mut block = Block::new(String::from("entry"));
        for operation in &model_function.operations {
            let mut value: Option<Value> = None;
            for operand in &operation.operands {
                match operand {
                    crate::compiler::modeler::Operand::Constant(constant_type) => {
                        value = match constant_type {
                            Long(value) => Some(Value::Constant(Constant {
                                _constant_type: ConstantType::Int64Constant,
                                _value: *value,
                            })),
                            _ => panic!("This type is not supported yet."),
                        };
                        break;
                    },
                    crate::compiler::modeler::Operand::Instruction => (),
                };
            }

            match value {
                Some(value) => block.instructions.push(Instruction::Ret(value)),
                None => panic!("Nothing to return."),
            };
        }
        function.blocks.push(block);
        plan.functions.push(function);
    }
}

pub struct Plan {
    pub name: String,
    pub functions: Vec<Function>,
}

impl Plan {
    pub fn new(name: String) -> Plan {
        Plan {
            name: name,
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
            name: name,
            blocks: Vec::new(),
        }
    }
}

pub struct Block {
    pub name: String,
    pub instructions: Vec<Instruction>,
}

impl Block {
    pub fn new(name: String) -> Block {
        Block {
            name: name,
            instructions: Vec::new(),
        }
    }
}

pub enum Instruction {
    Ret(Value),
}

pub enum Value {
    Constant(Constant),
}

pub struct Constant {
    _constant_type: ConstantType,
    _value: u64,
}

pub enum ConstantType {
    Int64Constant,
}
