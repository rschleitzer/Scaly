pub mod lexer;
use lexer::Lexer;
use lexer::Literal;
use lexer::Position;
use std::collections::HashSet;

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
    fn _is_identifier(&self, id: String) -> bool {
        if self._keywords.contains(&id) {
            false
        } else {
            true
        }
    }
}
