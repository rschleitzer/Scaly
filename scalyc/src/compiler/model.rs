use crate::compiler::parser::DeclarationSyntax::*;
use crate::compiler::parser::FunctionSyntax;
use crate::compiler::parser::ProcedureSyntax;
use crate::compiler::parser::OperatorSyntax;
use crate::compiler::parser::ProgramSyntax;
use crate::compiler::parser::DeclarationSyntax;
use crate::compiler::parser::StatementSyntax;
use crate::compiler::parser::FileSyntax;
use std::collections::HashMap;

pub struct Module {
    pub name: String,
    pub modules: HashMap<String, Module>,
    pub functions: HashMap<String, Function>,
}

impl Module {
    pub fn new(name: &String) -> Module {
        Module {
            name: name.clone(),
            modules: HashMap::new(),
            functions: HashMap::new(),
        }
    }

    pub fn build_program(&mut self, program: &ProgramSyntax, main_file_syntax: &FileSyntax, file_syntax: &FileSyntax) {
        self.build(program);
        if let Some(declarations) = &main_file_syntax.declarations {
            for declaration in declarations {
                self.process_declaration(&declaration);
            }
        }
        if let Some(declarations) = &file_syntax.declarations {
            for declaration in declarations {
                self.process_declaration(&declaration);
            }
        }
        if let Some(statements) = &file_syntax.statements {
            for statement in statements {
                self.process_statement(&statement);
            }
        }
    }

    pub fn build(&mut self, program: &ProgramSyntax) {
        for file in &program.files {
            if let Some(declarations) = &file.declarations {
                for declaration in declarations {
                    self.process_declaration(&declaration);
                }
            }
            if let Some(statements) = &file.statements {
                for statement in statements {
                    self.process_statement(&statement);
                }
            }
        }
    }

    fn process_declaration(&mut self, declaration: &DeclarationSyntax){
        match declaration {
            Public(_public) => {                
            }
            Definition(_definition) => {                
            }
            Function(function) => {
                self.process_function(function)
            }
            Procedure(procedure) => {
                self.process_procedure(procedure)
            }
            Operator(operator) => {
                self.process_operator(operator)
            }
            Use(_use) => {
            }
            Implement(_implement) => {
            }
            Trait(_trait) => {
            }
            Macro(_macro) => {                
            }
        }
    }

    fn process_function(&mut self, _function_syntax: &FunctionSyntax)
    {
        let _function = Function {};

    }

    fn process_procedure(&mut self, _procedure_syntax: &ProcedureSyntax)
    {
    }

    fn process_operator(&mut self, _operator_syntax: &OperatorSyntax)
    {
    }

    fn process_statement(&mut self, _statement: &StatementSyntax){}
}

pub struct Function {
}

impl Module {
}
