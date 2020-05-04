use crate::compiler::parser::lexer::Literal::Character;
use crate::compiler::parser::lexer::Literal::FloatingPoint;
use crate::compiler::parser::lexer::Literal::Hex;
use crate::compiler::parser::lexer::Literal::Integer;
use crate::compiler::parser::lexer::Literal::StringLiteral;
use crate::compiler::parser::CalculationSyntax;
use crate::compiler::parser::ConstantSyntax;
use crate::compiler::parser::ExpressionSyntax::Constant;
use crate::compiler::parser::OperandSyntax;
use std::collections::HashMap;

pub struct Modeler {}

impl Modeler {
    pub fn build_operation(calculation: &CalculationSyntax) -> Operation {
        let mut operation = Operation::new();
        for operand in &calculation.operation.op {
            operation.operands.push(Modeler::build_operand(&operand))
        }
        operation
    }

    pub fn build_operand(operand: &OperandSyntax) -> Operand {
        let primary = &operand.primary;
        match primary {
            Constant(constant_type) => Operand::Constant(Modeler::build_constant(&constant_type)),
        }
    }

    pub fn build_constant(constant_type: &ConstantSyntax) -> ConstantType {
        match &constant_type.literal {
            FloatingPoint(number) => ConstantType::Double(number.parse().unwrap()),
            Integer(number) => ConstantType::Long(number.parse().unwrap()),
            Hex(number) => ConstantType::Long(u64::from_str_radix(number, 16).unwrap()),
            StringLiteral(string) => ConstantType::StringConstant(string.to_string()),
            Character(character) => ConstantType::Character(*character),
        }
    }
}

pub struct Module {
    pub name: String,
    functions: HashMap<String, Function>,
}

impl Module {
    pub fn new(name: String) -> Module {
        Module {
            name: name,
            functions: HashMap::new(),
        }
    }

    pub fn add_function(&mut self, function: Function) {
        self.functions.insert(function.name.clone(), function);
    }
}

pub struct Function {
    pub name: String,
    pub operations: Vec<Operation>,
}

impl Function {
    pub fn new(name: String) -> Function {
        Function {
            name: name,
            operations: Vec::new(),
        }
    }
}

pub struct Operation {
    pub operands: Vec<Operand>,
}

impl Operation {
    pub fn new() -> Operation {
        Operation {
            operands: Vec::new(),
        }
    }
}

pub enum Operand {
    Constant(ConstantType),
}

pub enum ConstantType {
    Double(f64),
    Long(u64),
    StringConstant(String),
    Character(char),
}
