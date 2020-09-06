pub mod lexer;
use lexer::Lexer;
use lexer::Literal;
use lexer::Position;
use std::collections::HashSet;

pub struct ProgramSyntax {
    pub files: Vec<FileSyntax>,
}

pub struct FileSyntax {
    pub file_name: String,
    pub intrinsics: Option<Vec<IntrinsicSyntax>>,
    pub statements: Option<Vec<StatementSyntax>>,
}

pub struct IntrinsicSyntax {
    pub start: Position,
    pub end: Position,
    pub name: String,
}

pub enum StatementSyntax {
   Calculation(CalculationSyntax),
}

pub struct CalculationSyntax {
    pub start: Position,
    pub end: Position,
    pub operation: OperationSyntax,
}

pub struct OperationSyntax {
    pub start: Position,
    pub end: Position,
    pub op: Vec<OperandSyntax>,
}

pub struct OperandSyntax {
    pub start: Position,
    pub end: Position,
    pub primary: ExpressionSyntax,
}

pub enum ExpressionSyntax {
   Constant(ConstantSyntax),
}

pub struct ConstantSyntax {
    pub start: Position,
    pub end: Position,
    pub literal: Literal,
}

pub struct ParserError {
    pub file_name: String,
    pub line: usize,
    pub column: usize,
}

pub struct Parser<'a> {
    lexer: Lexer<'a>,
    _keywords: HashSet<String>,
}

impl<'a> Parser<'a> {
    pub fn new(deck: &'a str) -> Parser {
        let mut keywords = HashSet::new();
        keywords.insert(String::from("intrinsic"));
        Parser {
            lexer: Lexer::new(deck),
            _keywords: keywords,
        }
    }

    pub fn parse_file(&mut self) -> Result<Option<FileSyntax>, ParserError> {

        let intrinsics = self.parse_intrinsic_list()?;

        let statements = self.parse_statement_list()?;
        if let Some(_) = statements {
            if !self.is_at_end() {
                let error_pos = self.lexer.get_previous_position();
                return Result::Err(
                    ParserError {
                        file_name: "".to_string(),
                        line: error_pos.line,
                        column: error_pos.column,
                    },
                )
            }
        }

        let ret = FileSyntax {
            file_name: "".to_string(), 
            intrinsics: intrinsics, 
            statements: statements,
            };

        Ok(Some(ret))
    }

    pub fn parse_intrinsic_list(&mut self) -> Result<Option<Vec<IntrinsicSyntax>>, ParserError> {
        let mut array: Option<Vec<IntrinsicSyntax>> = Option::None;
        loop {
            let node = self.parse_intrinsic()?;
            if let Some(node) = node {
                if let None = array {
                    array = Some(Vec::new())
                };
                match &mut array {
                    Some(a) => a.push(node),
                    None => (),
                }
            } else {
                break;
            }
        }

        Ok(array)
    }

    pub fn parse_intrinsic(&mut self) -> Result<Option<IntrinsicSyntax>, ParserError> {
        let start: Position = self.lexer.get_previous_position();

        let success_intrinsic_1 = self.lexer.parse_keyword("intrinsic".to_string());
        if !success_intrinsic_1 {
            return Ok(None);
        }

        let name = self.lexer.parse_identifier();
        match &name {
            Some(name) =>
            if !self.is_identifier(name) {
                return Result::Err(
                    ParserError {
                        file_name: "".to_string(),
                        line: self.lexer.line,
                        column: self.lexer.column,
                    },
                )
           },
           _ =>
            return Result::Err(
                ParserError {
                    file_name: "".to_string(),
                    line: self.lexer.line,
                    column: self.lexer.column,
                },
            ),
        }

        let end: Position = self.lexer.get_position();

        let ret = IntrinsicSyntax {
            start: start,
            end: end, 
            name: name.unwrap(),
            };

        Ok(Some(ret))
    }

    pub fn parse_statement_list(&mut self) -> Result<Option<Vec<StatementSyntax>>, ParserError> {
        let mut array: Option<Vec<StatementSyntax>> = Option::None;
        loop {
            let node = self.parse_statement()?;
            if let Some(node) = node {
                if let None = array {
                    array = Some(Vec::new())
                };
                match &mut array {
                    Some(a) => a.push(node),
                    None => (),
                }
            } else {
                break;
            }
        }

        Ok(array)
    }

    pub fn parse_statement(&mut self) -> Result<Option<StatementSyntax>, ParserError> {
        {
            let node = self.parse_calculation()?;
            if let Some(node) = node {
                return Ok(Some(StatementSyntax::Calculation(node)));
            }
        }
        return Ok(None);
    }

    pub fn parse_calculation(&mut self) -> Result<Option<CalculationSyntax>, ParserError> {
        let start: Position = self.lexer.get_previous_position();

        let operation = self.parse_operation()?;
        if let None = operation {
            return Ok(None);
        }

        let success_semicolon_2 = self.lexer.parse_punctuation(";".to_string());
        if !success_semicolon_2 {
            ()
        }

        let end: Position = self.lexer.get_position();

        let ret = CalculationSyntax {
            start: start,
            end: end, 
            operation: operation.unwrap(),
            };

        Ok(Some(ret))
    }

    pub fn parse_operation(&mut self) -> Result<Option<OperationSyntax>, ParserError> {
        let start: Position = self.lexer.get_previous_position();

        let op = self.parse_operand_list()?;
        if let None = op {
            return Ok(None);
        }

        let end: Position = self.lexer.get_position();

        let ret = OperationSyntax {
            start: start,
            end: end, 
            op: op.unwrap(),
            };

        Ok(Some(ret))
    }

    pub fn parse_operand_list(&mut self) -> Result<Option<Vec<OperandSyntax>>, ParserError> {
        let mut array: Option<Vec<OperandSyntax>> = Option::None;
        loop {
            let node = self.parse_operand()?;
            if let Some(node) = node {
                if let None = array {
                    array = Some(Vec::new())
                };
                match &mut array {
                    Some(a) => a.push(node),
                    None => (),
                }
            } else {
                break;
            }
        }

        Ok(array)
    }

    pub fn parse_operand(&mut self) -> Result<Option<OperandSyntax>, ParserError> {
        let start: Position = self.lexer.get_previous_position();

        let primary = self.parse_expression()?;
        if let None = primary {
            return Ok(None);
        }

        let end: Position = self.lexer.get_position();

        let ret = OperandSyntax {
            start: start,
            end: end, 
            primary: primary.unwrap(),
            };

        Ok(Some(ret))
    }

    pub fn parse_expression(&mut self) -> Result<Option<ExpressionSyntax>, ParserError> {
        {
            let node = self.parse_constant()?;
            if let Some(node) = node {
                return Ok(Some(ExpressionSyntax::Constant(node)));
            }
        }
        return Ok(None);
    }

    pub fn parse_constant(&mut self) -> Result<Option<ConstantSyntax>, ParserError> {
        let start: Position = self.lexer.get_previous_position();

        let literal = self.lexer.parse_literal();
        if let None = literal {
            return Ok(None);
        }

        let end: Position = self.lexer.get_position();

        let ret = ConstantSyntax {
            start: start,
            end: end, 
            literal: literal.unwrap(),
            };

        Ok(Some(ret))
    }

    pub fn is_at_end(&mut self) -> bool {
        self.lexer.is_at_end()
    }

    pub fn get_current_line(&self) -> usize {
        self.lexer.get_position().line
    }

    pub fn get_current_column(&self) -> usize {
        self.lexer.get_position().column
    }

    pub fn get_previous_line(&self) -> usize {
        self.lexer.get_previous_position().line
    }

    pub fn get_previous_column(&self) -> usize {
        self.lexer.get_previous_position().column
    }
    fn is_identifier(&self, id: &String) -> bool {
        if self._keywords.contains(id) {
            false
        } else {
            true
        }
    }
}
