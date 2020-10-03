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
    pub declarations: Option<Vec<DeclarationSyntax>>,
    pub statements: Option<Vec<StatementSyntax>>,
}

pub enum DeclarationSyntax {
    Define(DefineSyntax),
    Function(FunctionSyntax),
}

pub struct DefineSyntax {
    pub start: Position,
    pub end: Position,
    pub name: NameSyntax,
    pub definition: DefinitionSyntax,
}

pub enum DefinitionSyntax {
    Class(ClassSyntax),
    Namespace(NamespaceSyntax),
    Union(UnionSyntax),
    Constant(ConstantSyntax),
}

pub struct ClassSyntax {
    pub start: Position,
    pub end: Position,
    pub structure: StructureSyntax,
    pub body: Option<BodySyntax>,
}

pub struct NamespaceSyntax {
    pub start: Position,
    pub end: Position,
    pub body: BodySyntax,
}

pub struct UnionSyntax {
    pub start: Position,
    pub end: Position,
    pub tags: Vec<TagSyntax>,
}

pub struct TagSyntax {
    pub start: Position,
    pub end: Position,
    pub name: NameSyntax,
    pub item: Option<ItemSyntax>,
}

pub enum ItemSyntax {
    Variant(VariantSyntax),
    Enum(EnumSyntax),
}

pub struct VariantSyntax {
    pub start: Position,
    pub end: Position,
    pub structure: StructureSyntax,
    pub body: Option<BodySyntax>,
}

pub struct EnumSyntax {
    pub start: Position,
    pub end: Position,
    pub literal: Literal,
}

pub struct ConstantSyntax {
    pub start: Position,
    pub end: Position,
    pub literal: Literal,
}

pub struct NameSyntax {
    pub start: Position,
    pub end: Position,
    pub name: String,
}

pub struct StructureSyntax {
    pub start: Position,
    pub end: Position,
    pub components: Option<Vec<ComponentSyntax>>,
}

pub struct BodySyntax {
    pub start: Position,
    pub end: Position,
    pub components: Option<Vec<DeclarationSyntax>>,
}

pub struct ComponentSyntax {
    pub start: Position,
    pub end: Position,
    pub name: String,
    pub type_annotation: Option<TypeAnnotationSyntax>,
}

pub struct FunctionSyntax {
    pub start: Position,
    pub end: Position,
    pub operand: TypeSpecSyntax,
    pub operator: TypeSpecSyntax,
    pub type_annotation: Option<TypeAnnotationSyntax>,
    pub expression: Option<ExpressionSyntax>,
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
    Literal(LiteralSyntax),
    Name(NameSyntax),
    Instruction(InstructionSyntax),
}

pub struct LiteralSyntax {
    pub start: Position,
    pub end: Position,
    pub literal: Literal,
}

pub struct InstructionSyntax {
    pub start: Position,
    pub end: Position,
}

pub struct TypeAnnotationSyntax {
    pub start: Position,
    pub end: Position,
    pub type_spec: TypeSpecSyntax,
}

pub enum TypeSpecSyntax {
    TypeName(TypeNameSyntax),
}

pub struct TypeNameSyntax {
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
    keywords: HashSet<String>,
}

impl<'a> Parser<'a> {
    pub fn new(deck: &'a str) -> Parser {
        let mut keywords = HashSet::new();
        keywords.insert(String::from("def"));
        keywords.insert(String::from("fn"));
        keywords.insert(String::from("instruction"));
        keywords.insert(String::from("let"));
        Parser {
            lexer: Lexer::new(deck),
            keywords: keywords,
        }
    }

    pub fn parse_file(&mut self) -> Result<Option<FileSyntax>, ParserError> {

        let declarations = self.parse_declaration_list()?;

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
            declarations: declarations,
            statements: statements,
        };

        Ok(Some(ret))
    }

    pub fn parse_declaration_list(&mut self) -> Result<Option<Vec<DeclarationSyntax>>, ParserError> {
        let mut array: Option<Vec<DeclarationSyntax>> = Option::None;
        loop {
            let node = self.parse_declaration()?;
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

    pub fn parse_declaration(&mut self) -> Result<Option<DeclarationSyntax>, ParserError> {
        {
            let node = self.parse_define()?;
            if let Some(node) = node {
                return Ok(Some(DeclarationSyntax::Define(node)));
            }
        }
        {
            let node = self.parse_function()?;
            if let Some(node) = node {
                return Ok(Some(DeclarationSyntax::Function(node)));
            }
        }
        return Ok(None)
    }

    pub fn parse_define(&mut self) -> Result<Option<DefineSyntax>, ParserError> {
        let start: Position = self.lexer.get_previous_position();

        let success_def_1 = self.lexer.parse_keyword("def".to_string());
        if !success_def_1 {

                return Ok(None)
        }

        let name = self.parse_name()?;
        if let None = name {
            return Err(ParserError {
                file_name: "".to_string(),
                line: self.lexer.line,
                column: self.lexer.column,
            });
        }

        let definition = self.parse_definition()?;
        if let None = definition {
            return Err(ParserError {
                file_name: "".to_string(),
                line: self.lexer.line,
                column: self.lexer.column,
            });
        }

        let success_semicolon_4 = self.lexer.parse_punctuation(";".to_string());
        if !success_semicolon_4 {
            ()
        }

        let end: Position = self.lexer.get_position();

        let ret = DefineSyntax {
            start: start,
            end: end,
            name: name.unwrap(),
            definition: definition.unwrap(),
        };

        Ok(Some(ret))
    }

    pub fn parse_definition(&mut self) -> Result<Option<DefinitionSyntax>, ParserError> {
        {
            let node = self.parse_class()?;
            if let Some(node) = node {
                return Ok(Some(DefinitionSyntax::Class(node)));
            }
        }
        {
            let node = self.parse_namespace()?;
            if let Some(node) = node {
                return Ok(Some(DefinitionSyntax::Namespace(node)));
            }
        }
        {
            let node = self.parse_union()?;
            if let Some(node) = node {
                return Ok(Some(DefinitionSyntax::Union(node)));
            }
        }
        {
            let node = self.parse_constant()?;
            if let Some(node) = node {
                return Ok(Some(DefinitionSyntax::Constant(node)));
            }
        }
        return Ok(None)
    }

    pub fn parse_class(&mut self) -> Result<Option<ClassSyntax>, ParserError> {
        let start: Position = self.lexer.get_previous_position();

        let structure = self.parse_structure()?;
        if let None = structure {
            return Ok(None);
        }

        let body = self.parse_body()?;

        let end: Position = self.lexer.get_position();

        let ret = ClassSyntax {
            start: start,
            end: end,
            structure: structure.unwrap(),
            body: body,
        };

        Ok(Some(ret))
    }

    pub fn parse_namespace(&mut self) -> Result<Option<NamespaceSyntax>, ParserError> {
        let start: Position = self.lexer.get_previous_position();

        let body = self.parse_body()?;
        if let None = body {
            return Ok(None);
        }

        let end: Position = self.lexer.get_position();

        let ret = NamespaceSyntax {
            start: start,
            end: end,
            body: body.unwrap(),
        };

        Ok(Some(ret))
    }

    pub fn parse_union(&mut self) -> Result<Option<UnionSyntax>, ParserError> {
        let start: Position = self.lexer.get_previous_position();

        let tags = self.parse_tag_list()?;
        if let None = tags {
            return Ok(None);
        }

        let end: Position = self.lexer.get_position();

        let ret = UnionSyntax {
            start: start,
            end: end,
            tags: tags.unwrap(),
        };

        Ok(Some(ret))
    }

    pub fn parse_tag_list(&mut self) -> Result<Option<Vec<TagSyntax>>, ParserError> {
        let mut array: Option<Vec<TagSyntax>> = Option::None;
        loop {
            let node = self.parse_tag()?;
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

    pub fn parse_tag(&mut self) -> Result<Option<TagSyntax>, ParserError> {
        let start: Position = self.lexer.get_previous_position();

        let name = self.parse_name()?;
        if let None = name {
            return Ok(None);
        }

        let item = self.parse_item()?;

        let end: Position = self.lexer.get_position();

        let ret = TagSyntax {
            start: start,
            end: end,
            name: name.unwrap(),
            item: item,
        };

        Ok(Some(ret))
    }

    pub fn parse_item(&mut self) -> Result<Option<ItemSyntax>, ParserError> {
        {
            let node = self.parse_variant()?;
            if let Some(node) = node {
                return Ok(Some(ItemSyntax::Variant(node)));
            }
        }
        {
            let node = self.parse_enum()?;
            if let Some(node) = node {
                return Ok(Some(ItemSyntax::Enum(node)));
            }
        }
        return Ok(None)
    }

    pub fn parse_variant(&mut self) -> Result<Option<VariantSyntax>, ParserError> {
        let start: Position = self.lexer.get_previous_position();

        let structure = self.parse_structure()?;
        if let None = structure {
            return Ok(None);
        }

        let body = self.parse_body()?;

        let end: Position = self.lexer.get_position();

        let ret = VariantSyntax {
            start: start,
            end: end,
            structure: structure.unwrap(),
            body: body,
        };

        Ok(Some(ret))
    }

    pub fn parse_enum(&mut self) -> Result<Option<EnumSyntax>, ParserError> {
        let start: Position = self.lexer.get_previous_position();

        let literal = self.lexer.parse_literal();
        if let None = literal {

                return Ok(None)
        }

        let end: Position = self.lexer.get_position();

        let ret = EnumSyntax {
            start: start,
            end: end,
            literal: literal.unwrap(),
        };

        Ok(Some(ret))
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

    pub fn parse_name(&mut self) -> Result<Option<NameSyntax>, ParserError> {
        let start: Position = self.lexer.get_previous_position();

        let name = self.lexer.parse_identifier(&self.keywords);
        match &name {
            Some(name) =>
                if !self.is_identifier(name) {
                return Ok(None)

           },
           _ =>
                return Ok(None)
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

    pub fn parse_structure(&mut self) -> Result<Option<StructureSyntax>, ParserError> {
        let start: Position = self.lexer.get_previous_position();

        let success_left_paren_1 = self.lexer.parse_punctuation("(".to_string());
        if !success_left_paren_1 {

                return Ok(None)
        }

        let components = self.parse_component_list()?;

        let success_right_paren_3 = self.lexer.parse_punctuation(")".to_string());
        if !success_right_paren_3 {

            return Result::Err(
                ParserError {
                    file_name: "".to_string(),
                    line: self.lexer.line,
                    column: self.lexer.column,
                },
            )        }

        let end: Position = self.lexer.get_position();

        let ret = StructureSyntax {
            start: start,
            end: end,
            components: components,
        };

        Ok(Some(ret))
    }

    pub fn parse_body(&mut self) -> Result<Option<BodySyntax>, ParserError> {
        let start: Position = self.lexer.get_previous_position();

        let success_left_brace_1 = self.lexer.parse_punctuation("{".to_string());
        if !success_left_brace_1 {

                return Ok(None)
        }

        let components = self.parse_declaration_list()?;

        let success_right_brace_3 = self.lexer.parse_punctuation("}".to_string());
        if !success_right_brace_3 {

            return Result::Err(
                ParserError {
                    file_name: "".to_string(),
                    line: self.lexer.line,
                    column: self.lexer.column,
                },
            )        }

        let end: Position = self.lexer.get_position();

        let ret = BodySyntax {
            start: start,
            end: end,
            components: components,
        };

        Ok(Some(ret))
    }

    pub fn parse_component_list(&mut self) -> Result<Option<Vec<ComponentSyntax>>, ParserError> {
        let mut array: Option<Vec<ComponentSyntax>> = Option::None;
        loop {
            let node = self.parse_component()?;
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

    pub fn parse_component(&mut self) -> Result<Option<ComponentSyntax>, ParserError> {
        let start: Position = self.lexer.get_previous_position();

        let name = self.lexer.parse_identifier(&self.keywords);
        match &name {
            Some(name) =>
                if !self.is_identifier(name) {
                return Ok(None)

           },
           _ =>
                return Ok(None)
,
        }

        let type_annotation = self.parse_typeannotation()?;

        let success_comma_3 = self.lexer.parse_punctuation(",".to_string());
        if !success_comma_3 {
            ()
        }

        let end: Position = self.lexer.get_position();

        let ret = ComponentSyntax {
            start: start,
            end: end,
            name: name.unwrap(),
            type_annotation: type_annotation,
        };

        Ok(Some(ret))
    }

    pub fn parse_function(&mut self) -> Result<Option<FunctionSyntax>, ParserError> {
        let start: Position = self.lexer.get_previous_position();

        let success_fn_1 = self.lexer.parse_keyword("fn".to_string());
        if !success_fn_1 {

                return Ok(None)
        }

        let operand = self.parse_typespec()?;
        if let None = operand {
            return Err(ParserError {
                file_name: "".to_string(),
                line: self.lexer.line,
                column: self.lexer.column,
            });
        }

        let operator = self.parse_typespec()?;
        if let None = operator {
            return Err(ParserError {
                file_name: "".to_string(),
                line: self.lexer.line,
                column: self.lexer.column,
            });
        }

        let type_annotation = self.parse_typeannotation()?;

        let expression = self.parse_expression()?;

        let end: Position = self.lexer.get_position();

        let ret = FunctionSyntax {
            start: start,
            end: end,
            operand: operand.unwrap(),
            operator: operator.unwrap(),
            type_annotation: type_annotation,
            expression: expression,
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
            return Err(ParserError {
                file_name: "".to_string(),
                line: self.lexer.line,
                column: self.lexer.column,
            });
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

        let name = self.lexer.parse_identifier(&self.keywords);
        match &name {
            Some(name) =>
                if !self.is_identifier(name) {
                return Ok(None)

           },
           _ =>
                return Ok(None)
,
        }

        let type_annotation = self.parse_typeannotation()?;

        let calculation = self.parse_calculation()?;
        if let None = calculation {
            return Err(ParserError {
                file_name: "".to_string(),
                line: self.lexer.line,
                column: self.lexer.column,
            });
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
            let node = self.parse_literal()?;
            if let Some(node) = node {
                return Ok(Some(ExpressionSyntax::Literal(node)));
            }
        }
        {
            let node = self.parse_name()?;
            if let Some(node) = node {
                return Ok(Some(ExpressionSyntax::Name(node)));
            }
        }
        {
            let node = self.parse_instruction()?;
            if let Some(node) = node {
                return Ok(Some(ExpressionSyntax::Instruction(node)));
            }
        }
        return Ok(None)
    }

    pub fn parse_literal(&mut self) -> Result<Option<LiteralSyntax>, ParserError> {
        let start: Position = self.lexer.get_previous_position();

        let literal = self.lexer.parse_literal();
        if let None = literal {

                return Ok(None)
        }

        let end: Position = self.lexer.get_position();

        let ret = LiteralSyntax {
            start: start,
            end: end,
            literal: literal.unwrap(),
        };

        Ok(Some(ret))
    }

    pub fn parse_instruction(&mut self) -> Result<Option<InstructionSyntax>, ParserError> {
        let start: Position = self.lexer.get_previous_position();

        let success_instruction_1 = self.lexer.parse_keyword("instruction".to_string());
        if !success_instruction_1 {

                return Ok(None)
        }

        let end: Position = self.lexer.get_position();

        let ret = InstructionSyntax {
            start: start,
            end: end,
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
            return Err(ParserError {
                file_name: "".to_string(),
                line: self.lexer.line,
                column: self.lexer.column,
            });
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
            let node = self.parse_typename()?;
            if let Some(node) = node {
                return Ok(Some(TypeSpecSyntax::TypeName(node)));
            }
        }
        return Ok(None)
    }

    pub fn parse_typename(&mut self) -> Result<Option<TypeNameSyntax>, ParserError> {
        let start: Position = self.lexer.get_previous_position();

        let name = self.parse_name()?;
        if let None = name {
            return Ok(None);
        }

        let end: Position = self.lexer.get_position();

        let ret = TypeNameSyntax {
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
        if self.keywords.contains(id) {
            false
        } else {
            true
        }
    }
}
