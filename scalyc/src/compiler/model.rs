use crate::compiler::parser::ProgramSyntax;
use crate::compiler::parser::DeclarationSyntax;
use crate::compiler::parser::StatementSyntax;
use std::collections::HashMap;

pub struct Model {
    pub modules: Vec<Item>,
}

impl Model {
    pub fn new() -> Model {
        Model {
            modules: Vec::new(),
        }
    }

    pub fn build(&mut self, program: ProgramSyntax) {
        for file in program.files {
            if let Some(declarations) = file.declarations {
                for declaration in declarations {
                    self.process_declaration(&declaration);
                }
           }
           if let Some(statements) = file.statements {
                for statement in statements {
                    self.process_statement(&statement);
                }
            }
        }
    }

    fn process_declaration(&mut self, _declaration: &DeclarationSyntax){}

    fn process_statement(&mut self, _statement: &StatementSyntax){}
}

pub struct Item {
    pub name: String,
    pub items: HashMap<String, Item>,
}

impl Item {
}
