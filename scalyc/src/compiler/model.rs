use crate::compiler::parser::ProgramSyntax;
use crate::compiler::parser::DeclarationSyntax;
use crate::compiler::parser::StatementSyntax;
use crate::compiler::parser::FileSyntax;
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

    fn process_declaration(&mut self, _declaration: &DeclarationSyntax){}

    fn process_statement(&mut self, _statement: &StatementSyntax){}
}

pub struct Item {
    pub name: String,
    pub items: HashMap<String, Item>,
}

impl Item {
}
