use crate::compiler::parser::lexer::Literal::Numeric;
use crate::compiler::parser::CalculationSyntax;
use crate::compiler::parser::ConstantSyntax;
use crate::compiler::parser::ExpressionSyntax::Constant;
use std::collections::HashMap;

pub struct Modeler {}

impl Modeler {
    pub fn build_expression(calculation: &CalculationSyntax) -> Expression {
        let operation = &calculation.operation;
        let primary = &operation.op[0].primary;
        match primary {
            Constant(constant_type) => Expression::Constant(Modeler::build_constant(constant_type)),
        }
    }

    pub fn build_constant(constant_type: &ConstantSyntax) -> ConstantType {
        match &constant_type.literal {
            Numeric(number) => ConstantType::Number(number.parse().unwrap()),
            _ => panic!("Only numbers."),
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
    pub expressions: Vec<Expression>,
}

impl Function {
    pub fn new(name: String) -> Function {
        Function {
            name: name,
            expressions: Vec::new(),
        }
    }
}

pub enum Expression {
    Constant(ConstantType),
}

pub enum ConstantType {
    Number(f64),
}
