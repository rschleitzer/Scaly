mod parser;
use parser::Parser;

pub struct Compiler {}

impl Compiler {
    pub fn new() -> Compiler {
        Compiler {}
    }
    pub fn evaluate(&mut self, deck: &str) -> String {
        let mut parser = Parser::new(deck);
        let statement_result = parser.parse_statement();
        match statement_result {
            Ok(statement_option) => {
                match statement_option {
                    None => return String::from("This is no statement.\n"),
                    Some(_statement) => return String::from("Parsed successfully.\n"),
                }
                
            }
            Err(error) => return String::from(format!("Parser error at {}, {}\n", error.line, error.column)),
        }
    }
}
