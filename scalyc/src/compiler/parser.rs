pub mod lexer;
use lexer::Lexer;
use lexer::Literal;
use lexer::Position;
use std::collections::HashSet;

pub struct NameSyntax {
    pub start: Position,
    pub end: Position,
    pub name: String,
}

pub enum StatementSyntax {
   Calculation(CalculationSyntax),
   Let(LetSyntax),
}

pub struct LetSyntax {
    pub start: Position,
    pub end: Position,
    pub binding: BindingSyntax,
}

pub struct BindingSyntax {
    pub start: Position,
    pub end: Position,
    pub name: String,
    pub type_annotation: Option<TypeAnnotationSyntax>,
    pub calculation: CalculationSyntax,
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

pub struct TypeAnnotationSyntax {
    pub start: Position,
    pub end: Position,
    pub type_spec: TypeSpecSyntax,
}

pub enum TypeSpecSyntax {
   Type(TypeSyntax),
}

pub struct TypeSyntax {
    pub start: Position,
    pub end: Position,
    pub name: NameSyntax,
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
        keywords.insert(String::from("let"));
        Parser {
            lexer: Lexer::new(deck),
            _keywords: keywords,
        }
    }

    pub fn parse_name(&mut self) -> Result<Option<NameSyntax>, ParserError> {
        let start: Position = self.lexer.get_previous_position();

        let name = self.lexer.parse_identifier();
        match &name {
            Some(name) =>
            if !self.is_identifier(name) {            return Ok(None)

           },
           _ =>            return Ok(None)
,
        }

        let end: Position = self.lexer.get_position();

        let ret = NameSyntax {
            start: start,
            end: end, 
            name: name.unwrap(),
            };

        Ok(Some(ret))
    }

    pub fn parse_statement(&mut self) -> Result<Option<StatementSyntax>, ParserError> {
        {
            let node = self.parse_calculation()?;
            if let Some(node) = node {
                return Ok(Some(StatementSyntax::Calculation(node)));
            }
        }
        {
            let node = self.parse_let()?;
            if let Some(node) = node {
                return Ok(Some(StatementSyntax::Let(node)));
            }
        }
        return Ok(None)
    }

    pub fn parse_let(&mut self) -> Result<Option<LetSyntax>, ParserError> {
        let start: Position = self.lexer.get_previous_position();

        let success_let_1 = self.lexer.parse_keyword("let".to_string());
        if !success_let_1 {
            return Ok(None)
        }

        let binding = self.parse_binding()?;
        if let None = binding {
            return Err(ParserError { file_name: "".to_string(), line: self.lexer.line, column: self.lexer.column })
        }

        let end: Position = self.lexer.get_position();

        let ret = LetSyntax {
            start: start,
            end: end, 
            binding: binding.unwrap(),
            };

        Ok(Some(ret))
    }

    pub fn parse_binding(&mut self) -> Result<Option<BindingSyntax>, ParserError> {
        let start: Position = self.lexer.get_previous_position();

        let name = self.lexer.parse_identifier();
        match &name {
            Some(name) =>
            if !self.is_identifier(name) {            return Ok(None)

           },
           _ =>            return Ok(None)
,
        }

        let type_annotation = self.parse_typeannotation()?;

        let calculation = self.parse_calculation()?;
        if let None = calculation {
            return Err(ParserError { file_name: "".to_string(), line: self.lexer.line, column: self.lexer.column })
        }

        let end: Position = self.lexer.get_position();

        let ret = BindingSyntax {
            start: start,
            end: end, 
            name: name.unwrap(), 
            type_annotation: type_annotation, 
            calculation: calculation.unwrap(),
            };

        Ok(Some(ret))
    }

    pub fn parse_calculation(&mut self) -> Result<Option<CalculationSyntax>, ParserError> {
        let start: Position = self.lexer.get_previous_position();

        let operation = self.parse_operation()?;
        if let None = operation {
            return Ok(None)
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
            return Ok(None)
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
            return Ok(None)
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
        return Ok(None)
    }

    pub fn parse_constant(&mut self) -> Result<Option<ConstantSyntax>, ParserError> {
        let start: Position = self.lexer.get_previous_position();

        let literal = self.lexer.parse_literal();
        if let None = literal {
            return Ok(None)
        }

        let end: Position = self.lexer.get_position();

        let ret = ConstantSyntax {
            start: start,
            end: end, 
            literal: literal.unwrap(),
            };

        Ok(Some(ret))
    }

    pub fn parse_typeannotation(&mut self) -> Result<Option<TypeAnnotationSyntax>, ParserError> {
        let start: Position = self.lexer.get_previous_position();

        let success_colon_1 = self.lexer.parse_punctuation(":".to_string());
        if !success_colon_1 {
            return Ok(None)
        }

        let type_spec = self.parse_typespec()?;
        if let None = type_spec {
            return Err(ParserError { file_name: "".to_string(), line: self.lexer.line, column: self.lexer.column })
        }

        let end: Position = self.lexer.get_position();

        let ret = TypeAnnotationSyntax {
            start: start,
            end: end, 
            type_spec: type_spec.unwrap(),
            };

        Ok(Some(ret))
    }

    pub fn parse_typespec(&mut self) -> Result<Option<TypeSpecSyntax>, ParserError> {
        {
            let node = self.parse_type()?;
            if let Some(node) = node {
                return Ok(Some(TypeSpecSyntax::Type(node)));
            }
        }
        return Ok(None)
    }

    pub fn parse_type(&mut self) -> Result<Option<TypeSyntax>, ParserError> {
        let start: Position = self.lexer.get_previous_position();

        let name = self.parse_name()?;
        if let None = name {
            return Ok(None)
        }

        let end: Position = self.lexer.get_position();

        let ret = TypeSyntax {
            start: start,
            end: end, 
            name: name.unwrap(),
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
