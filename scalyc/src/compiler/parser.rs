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
    Public(PublicSyntax),
    Definition(DefinitionSyntax),
    Function(FunctionSyntax),
    Use(UseSyntax),
    Implement(ImplementSyntax),
    Trait(TraitSyntax),
    Macro(MacroSyntax),
}

pub struct PublicSyntax {
    pub start: Position,
    pub end: Position,
    pub export: ExportSyntax,
}

pub enum ExportSyntax {
    Definition(DefinitionSyntax),
    Function(FunctionSyntax),
    Use(UseSyntax),
    Implement(ImplementSyntax),
    Trait(TraitSyntax),
    Macro(MacroSyntax),
}

pub struct DefinitionSyntax {
    pub start: Position,
    pub end: Position,
    pub name: NameSyntax,
    pub attributes: Option<Vec<AttributeSyntax>>,
    pub concept: ConceptSyntax,
}

pub struct NameSyntax {
    pub start: Position,
    pub end: Position,
    pub name: String,
    pub extensions: Option<Vec<ExtensionSyntax>>,
}

pub struct ExtensionSyntax {
    pub start: Position,
    pub end: Position,
    pub name: String,
}

pub struct AttributeSyntax {
    pub start: Position,
    pub end: Position,
    pub attribute: String,
    pub value: ModelSyntax,
}

pub enum ConceptSyntax {
    Class(ClassSyntax),
    Namespace(NamespaceSyntax),
    Union(UnionSyntax),
    Constant(ConstantSyntax),
    Delegate(DelegateSyntax),
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
    pub name: String,
    pub attributes: Option<Vec<AttributeSyntax>>,
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

pub struct DelegateSyntax {
    pub start: Position,
    pub end: Position,
    pub operator: TypeSpecSyntax,
    pub attributes: Option<Vec<AttributeSyntax>>,
    pub result: Option<ReturnsSyntax>,
    pub error: Option<ThrowsSyntax>,
}

pub struct StructureSyntax {
    pub start: Position,
    pub end: Position,
    pub components: Option<Vec<ComponentSyntax>>,
}

pub struct BodySyntax {
    pub start: Position,
    pub end: Position,
    pub declarations: Option<Vec<DeclarationSyntax>>,
}

pub struct ComponentSyntax {
    pub start: Position,
    pub end: Position,
    pub name: String,
    pub attributes: Option<Vec<AttributeSyntax>>,
    pub annotation: Option<TypeAnnotationSyntax>,
}

pub struct TypeAnnotationSyntax {
    pub start: Position,
    pub end: Position,
    pub spec: TypeSpecSyntax,
}

pub enum TypeSpecSyntax {
    Structure(StructureSyntax),
    Type(TypeSyntax),
}

pub struct TypeSyntax {
    pub start: Position,
    pub end: Position,
    pub name: NameSyntax,
    pub generics: Option<GenericArgumentsSyntax>,
    pub optional: Option<OptionalSyntax>,
}

pub struct GenericArgumentsSyntax {
    pub start: Position,
    pub end: Position,
    pub generics: Option<Vec<GenericArgumentSyntax>>,
}

pub struct GenericArgumentSyntax {
    pub start: Position,
    pub end: Position,
    pub spec: TypeSyntax,
}

pub struct OptionalSyntax {
    pub start: Position,
    pub end: Position,
}

pub struct FunctionSyntax {
    pub start: Position,
    pub end: Position,
    pub operand: TypeSpecSyntax,
    pub operator: Option<TypeSpecSyntax>,
    pub attributes: Option<Vec<AttributeSyntax>>,
    pub result: Option<ReturnsSyntax>,
    pub error: Option<ThrowsSyntax>,
    pub exception: Option<Vec<AttributeSyntax>>,
    pub implementation: ImplementationSyntax,
}

pub struct ReturnsSyntax {
    pub start: Position,
    pub end: Position,
    pub spec: TypeSpecSyntax,
    pub attributes: Option<Vec<AttributeSyntax>>,
}

pub struct ThrowsSyntax {
    pub start: Position,
    pub end: Position,
    pub spec: TypeSpecSyntax,
    pub attributes: Option<Vec<AttributeSyntax>>,
}

pub enum ImplementationSyntax {
    Operation(OperationSyntax),
    Extern(ExternSyntax),
    Instruction(InstructionSyntax),
    Intrinsic(IntrinsicSyntax),
}

pub struct ExternSyntax {
    pub start: Position,
    pub end: Position,
}

pub struct InstructionSyntax {
    pub start: Position,
    pub end: Position,
}

pub struct IntrinsicSyntax {
    pub start: Position,
    pub end: Position,
}

pub struct UseSyntax {
    pub start: Position,
    pub end: Position,
    pub name: NameSyntax,
}

pub struct ImplementSyntax {
    pub start: Position,
    pub end: Position,
    pub name: NameSyntax,
    pub attributes: Option<Vec<AttributeSyntax>>,
    pub functions: Option<Vec<FunctionSyntax>>,
}

pub struct TraitSyntax {
    pub start: Position,
    pub end: Position,
    pub name: NameSyntax,
    pub extension: ExtendsSyntax,
    pub attributes: Option<Vec<AttributeSyntax>>,
    pub functions: Option<Vec<FunctionSyntax>>,
}

pub struct ExtendsSyntax {
    pub start: Position,
    pub end: Position,
    pub extensions: Option<Vec<ExtendSyntax>>,
}

pub struct ExtendSyntax {
    pub start: Position,
    pub end: Position,
    pub spec: TypeSpecSyntax,
}

pub struct MacroSyntax {
    pub start: Position,
    pub end: Position,
    pub name: String,
    pub model: ModelSyntax,
    pub rule: OperationSyntax,
}

pub enum ModelSyntax {
    Literal(LiteralSyntax),
    Name(NameSyntax),
    Object(ObjectSyntax),
    Array(ArraySyntax),
}

pub enum StatementSyntax {
    Operation(OperationSyntax),
    Let(LetSyntax),
    Var(VarSyntax),
    Mutable(MutableSyntax),
    Set(SetSyntax),
    Continue(ContinueSyntax),
    Break(BreakSyntax),
    Return(ReturnSyntax),
    Throw(ThrowSyntax),
}

pub struct LetSyntax {
    pub start: Position,
    pub end: Position,
    pub binding: BindingSyntax,
}

pub struct VarSyntax {
    pub start: Position,
    pub end: Position,
    pub binding: BindingSyntax,
}

pub struct MutableSyntax {
    pub start: Position,
    pub end: Position,
    pub binding: BindingSyntax,
}

pub struct BindingSyntax {
    pub start: Position,
    pub end: Position,
    pub name: String,
    pub annotation: Option<TypeAnnotationSyntax>,
    pub calculation: OperationSyntax,
}

pub struct SetSyntax {
    pub start: Position,
    pub end: Position,
    pub target: OperationSyntax,
    pub source: OperationSyntax,
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
    pub postfixes: Option<Vec<PostfixSyntax>>,
}

pub enum PostfixSyntax {
    MemberAccess(MemberAccessSyntax),
    Catcher(CatcherSyntax),
}

pub struct MemberAccessSyntax {
    pub start: Position,
    pub end: Position,
    pub member: NameSyntax,
}

pub struct CatcherSyntax {
    pub start: Position,
    pub end: Position,
    pub catchers: Vec<CatchSyntax>,
    pub dropper: Option<DropSyntax>,
}

pub struct CatchSyntax {
    pub start: Position,
    pub end: Position,
    pub condition: OperationSyntax,
    pub handler: OperationSyntax,
}

pub struct DropSyntax {
    pub start: Position,
    pub end: Position,
    pub handler: OperationSyntax,
}

pub struct ContinueSyntax {
    pub start: Position,
    pub end: Position,
    pub name: Option<LoopSyntax>,
}

pub struct LoopSyntax {
    pub start: Position,
    pub end: Position,
    pub name: String,
}

pub struct BreakSyntax {
    pub start: Position,
    pub end: Position,
    pub name: Option<LoopSyntax>,
    pub result: OperationSyntax,
}

pub struct ReturnSyntax {
    pub start: Position,
    pub end: Position,
    pub result: Option<OperationSyntax>,
}

pub struct ThrowSyntax {
    pub start: Position,
    pub end: Position,
    pub result: Option<OperationSyntax>,
}

pub enum ExpressionSyntax {
    Literal(LiteralSyntax),
    Name(NameSyntax),
    Object(ObjectSyntax),
    Array(ArraySyntax),
    Block(BlockSyntax),
    If(IfSyntax),
    Match(MatchSyntax),
    Lambda(LambdaSyntax),
    For(ForSyntax),
    While(WhileSyntax),
    Repeat(RepeatSyntax),
    SizeOf(SizeOfSyntax),
}

pub struct LiteralSyntax {
    pub start: Position,
    pub end: Position,
    pub literal: Literal,
}

pub struct ObjectSyntax {
    pub start: Position,
    pub end: Position,
    pub fields: Option<Vec<FieldSyntax>>,
}

pub struct FieldSyntax {
    pub start: Position,
    pub end: Position,
    pub operation: OperationSyntax,
    pub attributes: Option<Vec<AttributeSyntax>>,
    pub value: Option<ValueSyntax>,
}

pub struct ValueSyntax {
    pub start: Position,
    pub end: Position,
    pub value: OperationSyntax,
    pub attributes: Option<Vec<AttributeSyntax>>,
}

pub struct ArraySyntax {
    pub start: Position,
    pub end: Position,
    pub elements: Vec<ElementSyntax>,
}

pub struct ElementSyntax {
    pub start: Position,
    pub end: Position,
    pub operation: OperationSyntax,
    pub attributes: Option<Vec<AttributeSyntax>>,
}

pub struct BlockSyntax {
    pub start: Position,
    pub end: Position,
    pub statements: Option<Vec<StatementSyntax>>,
}

pub struct IfSyntax {
    pub start: Position,
    pub end: Position,
    pub condition: OperationSyntax,
    pub consequent: OperationSyntax,
    pub alternative: Option<ElseSyntax>,
}

pub struct ElseSyntax {
    pub start: Position,
    pub end: Position,
    pub alternative: OperationSyntax,
}

pub struct MatchSyntax {
    pub start: Position,
    pub end: Position,
    pub scrutinee: OperationSyntax,
    pub cases: Vec<CaseSyntax>,
    pub alternative: Option<DefaultSyntax>,
}

pub struct CaseSyntax {
    pub start: Position,
    pub end: Position,
    pub condition: OperationSyntax,
    pub consequent: OperationSyntax,
}

pub struct DefaultSyntax {
    pub start: Position,
    pub end: Position,
    pub alternative: Option<OperationSyntax>,
}

pub struct LambdaSyntax {
    pub start: Position,
    pub end: Position,
    pub input: OperationSyntax,
    pub block: OperationSyntax,
}

pub struct ForSyntax {
    pub start: Position,
    pub end: Position,
    pub condition: OperationSyntax,
    pub expression: OperationSyntax,
    pub name: Option<LabelSyntax>,
    pub action: OperationSyntax,
}

pub struct LabelSyntax {
    pub start: Position,
    pub end: Position,
    pub name: String,
}

pub struct WhileSyntax {
    pub start: Position,
    pub end: Position,
    pub condition: OperationSyntax,
    pub name: Option<LabelSyntax>,
    pub action: OperationSyntax,
}

pub struct RepeatSyntax {
    pub start: Position,
    pub end: Position,
    pub name: Option<LabelSyntax>,
    pub action: OperationSyntax,
}

pub struct SizeOfSyntax {
    pub start: Position,
    pub end: Position,
    pub spec: TypeSyntax,
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
        keywords.insert(String::from("break"));
        keywords.insert(String::from("catch"));
        keywords.insert(String::from("case"));
        keywords.insert(String::from("continue"));
        keywords.insert(String::from("def"));
        keywords.insert(String::from("default"));
        keywords.insert(String::from("drop"));
        keywords.insert(String::from("else"));
        keywords.insert(String::from("extends"));
        keywords.insert(String::from("extern"));
        keywords.insert(String::from("fn"));
        keywords.insert(String::from("for"));
        keywords.insert(String::from("if"));
        keywords.insert(String::from("implement"));
        keywords.insert(String::from("in"));
        keywords.insert(String::from("instruction"));
        keywords.insert(String::from("intrinsic"));
        keywords.insert(String::from("label"));
        keywords.insert(String::from("lambda"));
        keywords.insert(String::from("let"));
        keywords.insert(String::from("loop"));
        keywords.insert(String::from("macro"));
        keywords.insert(String::from("match"));
        keywords.insert(String::from("mutable"));
        keywords.insert(String::from("pub"));
        keywords.insert(String::from("return"));
        keywords.insert(String::from("returns"));
        keywords.insert(String::from("repeat"));
        keywords.insert(String::from("set"));
        keywords.insert(String::from("sizeof"));
        keywords.insert(String::from("throw"));
        keywords.insert(String::from("throws"));
        keywords.insert(String::from("trait"));
        keywords.insert(String::from("use"));
        keywords.insert(String::from("var"));
        keywords.insert(String::from("while"));
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
            let node = self.parse_public()?;
            if let Some(node) = node {
                return Ok(Some(DeclarationSyntax::Public(node)));
            }
        }
        {
            let node = self.parse_definition()?;
            if let Some(node) = node {
                return Ok(Some(DeclarationSyntax::Definition(node)));
            }
        }
        {
            let node = self.parse_function()?;
            if let Some(node) = node {
                return Ok(Some(DeclarationSyntax::Function(node)));
            }
        }
        {
            let node = self.parse_use()?;
            if let Some(node) = node {
                return Ok(Some(DeclarationSyntax::Use(node)));
            }
        }
        {
            let node = self.parse_implement()?;
            if let Some(node) = node {
                return Ok(Some(DeclarationSyntax::Implement(node)));
            }
        }
        {
            let node = self.parse_trait()?;
            if let Some(node) = node {
                return Ok(Some(DeclarationSyntax::Trait(node)));
            }
        }
        {
            let node = self.parse_macro()?;
            if let Some(node) = node {
                return Ok(Some(DeclarationSyntax::Macro(node)));
            }
        }
        return Ok(None)
    }

    pub fn parse_public(&mut self) -> Result<Option<PublicSyntax>, ParserError> {
        let start: Position = self.lexer.get_previous_position();

        let success_pub_1 = self.lexer.parse_keyword("pub".to_string());
        if !success_pub_1 {

                return Ok(None)
        }

        let export = self.parse_export()?;
        if let None = export {
            return Err(ParserError {
                file_name: "".to_string(),
                line: self.lexer.line,
                column: self.lexer.column,
            });
        }

        let end: Position = self.lexer.get_position();

        let ret = PublicSyntax {
            start: start,
            end: end,
            export: export.unwrap(),
        };

        Ok(Some(ret))
    }

    pub fn parse_export(&mut self) -> Result<Option<ExportSyntax>, ParserError> {
        {
            let node = self.parse_definition()?;
            if let Some(node) = node {
                return Ok(Some(ExportSyntax::Definition(node)));
            }
        }
        {
            let node = self.parse_function()?;
            if let Some(node) = node {
                return Ok(Some(ExportSyntax::Function(node)));
            }
        }
        {
            let node = self.parse_use()?;
            if let Some(node) = node {
                return Ok(Some(ExportSyntax::Use(node)));
            }
        }
        {
            let node = self.parse_implement()?;
            if let Some(node) = node {
                return Ok(Some(ExportSyntax::Implement(node)));
            }
        }
        {
            let node = self.parse_trait()?;
            if let Some(node) = node {
                return Ok(Some(ExportSyntax::Trait(node)));
            }
        }
        {
            let node = self.parse_macro()?;
            if let Some(node) = node {
                return Ok(Some(ExportSyntax::Macro(node)));
            }
        }
        return Ok(None)
    }

    pub fn parse_definition(&mut self) -> Result<Option<DefinitionSyntax>, ParserError> {
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

        let attributes = self.parse_attribute_list()?;

        let concept = self.parse_concept()?;
        if let None = concept {
            return Err(ParserError {
                file_name: "".to_string(),
                line: self.lexer.line,
                column: self.lexer.column,
            });
        }

        let end: Position = self.lexer.get_position();

        let ret = DefinitionSyntax {
            start: start,
            end: end,
            name: name.unwrap(),
            attributes: attributes,
            concept: concept.unwrap(),
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

        let extensions = self.parse_extension_list()?;

        let end: Position = self.lexer.get_position();

        let ret = NameSyntax {
            start: start,
            end: end,
            name: name.unwrap(),
            extensions: extensions,
        };

        Ok(Some(ret))
    }

    pub fn parse_extension_list(&mut self) -> Result<Option<Vec<ExtensionSyntax>>, ParserError> {
        let mut array: Option<Vec<ExtensionSyntax>> = Option::None;
        loop {
            let node = self.parse_extension()?;
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

    pub fn parse_extension(&mut self) -> Result<Option<ExtensionSyntax>, ParserError> {
        let start: Position = self.lexer.get_previous_position();

        let success_dot_1 = self.lexer.parse_punctuation(".".to_string());
        if !success_dot_1 {

                return Ok(None)
        }

        let name = self.lexer.parse_identifier(&self.keywords);
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

        let ret = ExtensionSyntax {
            start: start,
            end: end,
            name: name.unwrap(),
        };

        Ok(Some(ret))
    }

    pub fn parse_attribute_list(&mut self) -> Result<Option<Vec<AttributeSyntax>>, ParserError> {
        let mut array: Option<Vec<AttributeSyntax>> = Option::None;
        loop {
            let node = self.parse_attribute()?;
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

    pub fn parse_attribute(&mut self) -> Result<Option<AttributeSyntax>, ParserError> {
        let start: Position = self.lexer.get_previous_position();

        let attribute = self.lexer.parse_attribute();
        if let None = attribute {

                return Ok(None)
        }

        let value = self.parse_model()?;
        if let None = value {
            return Err(ParserError {
                file_name: "".to_string(),
                line: self.lexer.line,
                column: self.lexer.column,
            });
        }

        let end: Position = self.lexer.get_position();

        let ret = AttributeSyntax {
            start: start,
            end: end,
            attribute: attribute.unwrap(),
            value: value.unwrap(),
        };

        Ok(Some(ret))
    }

    pub fn parse_concept(&mut self) -> Result<Option<ConceptSyntax>, ParserError> {
        {
            let node = self.parse_class()?;
            if let Some(node) = node {
                return Ok(Some(ConceptSyntax::Class(node)));
            }
        }
        {
            let node = self.parse_namespace()?;
            if let Some(node) = node {
                return Ok(Some(ConceptSyntax::Namespace(node)));
            }
        }
        {
            let node = self.parse_union()?;
            if let Some(node) = node {
                return Ok(Some(ConceptSyntax::Union(node)));
            }
        }
        {
            let node = self.parse_constant()?;
            if let Some(node) = node {
                return Ok(Some(ConceptSyntax::Constant(node)));
            }
        }
        {
            let node = self.parse_delegate()?;
            if let Some(node) = node {
                return Ok(Some(ConceptSyntax::Delegate(node)));
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

        let attributes = self.parse_attribute_list()?;

        let item = self.parse_item()?;

        let end: Position = self.lexer.get_position();

        let ret = TagSyntax {
            start: start,
            end: end,
            name: name.unwrap(),
            attributes: attributes,
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

    pub fn parse_delegate(&mut self) -> Result<Option<DelegateSyntax>, ParserError> {
        let start: Position = self.lexer.get_previous_position();

        let success_fn_1 = self.lexer.parse_keyword("fn".to_string());
        if !success_fn_1 {

                return Ok(None)
        }

        let operator = self.parse_typespec()?;
        if let None = operator {
            return Err(ParserError {
                file_name: "".to_string(),
                line: self.lexer.line,
                column: self.lexer.column,
            });
        }

        let attributes = self.parse_attribute_list()?;

        let result = self.parse_returns()?;

        let error = self.parse_throws()?;

        let end: Position = self.lexer.get_position();

        let ret = DelegateSyntax {
            start: start,
            end: end,
            operator: operator.unwrap(),
            attributes: attributes,
            result: result,
            error: error,
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

        let success_left_curly_1 = self.lexer.parse_punctuation("{".to_string());
        if !success_left_curly_1 {

                return Ok(None)
        }

        let declarations = self.parse_declaration_list()?;

        let success_right_curly_3 = self.lexer.parse_punctuation("}".to_string());
        if !success_right_curly_3 {

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
            declarations: declarations,
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

        let attributes = self.parse_attribute_list()?;

        let annotation = self.parse_typeannotation()?;

        let success_comma_4 = self.lexer.parse_punctuation(",".to_string());
        if !success_comma_4 {
            ()
        }

        let end: Position = self.lexer.get_position();

        let ret = ComponentSyntax {
            start: start,
            end: end,
            name: name.unwrap(),
            attributes: attributes,
            annotation: annotation,
        };

        Ok(Some(ret))
    }

    pub fn parse_typeannotation(&mut self) -> Result<Option<TypeAnnotationSyntax>, ParserError> {
        let start: Position = self.lexer.get_previous_position();

        let success_colon_1 = self.lexer.parse_colon();
        if !success_colon_1 {

                return Ok(None)
        }

        let spec = self.parse_typespec()?;
        if let None = spec {
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
            spec: spec.unwrap(),
        };

        Ok(Some(ret))
    }

    pub fn parse_typespec(&mut self) -> Result<Option<TypeSpecSyntax>, ParserError> {
        {
            let node = self.parse_structure()?;
            if let Some(node) = node {
                return Ok(Some(TypeSpecSyntax::Structure(node)));
            }
        }
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
            return Ok(None);
        }

        let generics = self.parse_genericarguments()?;

        let optional = self.parse_optional()?;

        let end: Position = self.lexer.get_position();

        let ret = TypeSyntax {
            start: start,
            end: end,
            name: name.unwrap(),
            generics: generics,
            optional: optional,
        };

        Ok(Some(ret))
    }

    pub fn parse_genericarguments(&mut self) -> Result<Option<GenericArgumentsSyntax>, ParserError> {
        let start: Position = self.lexer.get_previous_position();

        let success_left_bracket_1 = self.lexer.parse_punctuation("[".to_string());
        if !success_left_bracket_1 {

                return Ok(None)
        }

        let generics = self.parse_genericargument_list()?;

        let success_right_bracket_3 = self.lexer.parse_punctuation("]".to_string());
        if !success_right_bracket_3 {

            return Result::Err(
                ParserError {
                    file_name: "".to_string(),
                    line: self.lexer.line,
                    column: self.lexer.column,
                },
            )        }

        let end: Position = self.lexer.get_position();

        let ret = GenericArgumentsSyntax {
            start: start,
            end: end,
            generics: generics,
        };

        Ok(Some(ret))
    }

    pub fn parse_genericargument_list(&mut self) -> Result<Option<Vec<GenericArgumentSyntax>>, ParserError> {
        let mut array: Option<Vec<GenericArgumentSyntax>> = Option::None;
        loop {
            let node = self.parse_genericargument()?;
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

    pub fn parse_genericargument(&mut self) -> Result<Option<GenericArgumentSyntax>, ParserError> {
        let start: Position = self.lexer.get_previous_position();

        let spec = self.parse_type()?;
        if let None = spec {
            return Ok(None);
        }

        let success_comma_2 = self.lexer.parse_punctuation(",".to_string());
        if !success_comma_2 {
            ()
        }

        let end: Position = self.lexer.get_position();

        let ret = GenericArgumentSyntax {
            start: start,
            end: end,
            spec: spec.unwrap(),
        };

        Ok(Some(ret))
    }

    pub fn parse_optional(&mut self) -> Result<Option<OptionalSyntax>, ParserError> {
        let start: Position = self.lexer.get_previous_position();

        let success_question_1 = self.lexer.parse_punctuation("?".to_string());
        if !success_question_1 {

                return Ok(None)
        }

        let end: Position = self.lexer.get_position();

        let ret = OptionalSyntax {
            start: start,
            end: end,
        };

        Ok(Some(ret))
    }

    pub fn parse_function_list(&mut self) -> Result<Option<Vec<FunctionSyntax>>, ParserError> {
        let mut array: Option<Vec<FunctionSyntax>> = Option::None;
        loop {
            let node = self.parse_function()?;
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

        let attributes = self.parse_attribute_list()?;

        let result = self.parse_returns()?;

        let error = self.parse_throws()?;

        let exception = self.parse_attribute_list()?;

        let implementation = self.parse_implementation()?;
        if let None = implementation {
            return Err(ParserError {
                file_name: "".to_string(),
                line: self.lexer.line,
                column: self.lexer.column,
            });
        }

        let end: Position = self.lexer.get_position();

        let ret = FunctionSyntax {
            start: start,
            end: end,
            operand: operand.unwrap(),
            operator: operator,
            attributes: attributes,
            result: result,
            error: error,
            exception: exception,
            implementation: implementation.unwrap(),
        };

        Ok(Some(ret))
    }

    pub fn parse_returns(&mut self) -> Result<Option<ReturnsSyntax>, ParserError> {
        let start: Position = self.lexer.get_previous_position();

        let success_returns_1 = self.lexer.parse_keyword("returns".to_string());
        if !success_returns_1 {

                return Ok(None)
        }

        let spec = self.parse_typespec()?;
        if let None = spec {
            return Err(ParserError {
                file_name: "".to_string(),
                line: self.lexer.line,
                column: self.lexer.column,
            });
        }

        let attributes = self.parse_attribute_list()?;

        let end: Position = self.lexer.get_position();

        let ret = ReturnsSyntax {
            start: start,
            end: end,
            spec: spec.unwrap(),
            attributes: attributes,
        };

        Ok(Some(ret))
    }

    pub fn parse_throws(&mut self) -> Result<Option<ThrowsSyntax>, ParserError> {
        let start: Position = self.lexer.get_previous_position();

        let success_throws_1 = self.lexer.parse_keyword("throws".to_string());
        if !success_throws_1 {

                return Ok(None)
        }

        let spec = self.parse_typespec()?;
        if let None = spec {
            return Err(ParserError {
                file_name: "".to_string(),
                line: self.lexer.line,
                column: self.lexer.column,
            });
        }

        let attributes = self.parse_attribute_list()?;

        let end: Position = self.lexer.get_position();

        let ret = ThrowsSyntax {
            start: start,
            end: end,
            spec: spec.unwrap(),
            attributes: attributes,
        };

        Ok(Some(ret))
    }

    pub fn parse_implementation(&mut self) -> Result<Option<ImplementationSyntax>, ParserError> {
        {
            let node = self.parse_operation()?;
            if let Some(node) = node {
                return Ok(Some(ImplementationSyntax::Operation(node)));
            }
        }
        {
            let node = self.parse_extern()?;
            if let Some(node) = node {
                return Ok(Some(ImplementationSyntax::Extern(node)));
            }
        }
        {
            let node = self.parse_instruction()?;
            if let Some(node) = node {
                return Ok(Some(ImplementationSyntax::Instruction(node)));
            }
        }
        {
            let node = self.parse_intrinsic()?;
            if let Some(node) = node {
                return Ok(Some(ImplementationSyntax::Intrinsic(node)));
            }
        }
        return Ok(None)
    }

    pub fn parse_extern(&mut self) -> Result<Option<ExternSyntax>, ParserError> {
        let start: Position = self.lexer.get_previous_position();

        let success_extern_1 = self.lexer.parse_keyword("extern".to_string());
        if !success_extern_1 {

                return Ok(None)
        }

        let end: Position = self.lexer.get_position();

        let ret = ExternSyntax {
            start: start,
            end: end,
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

    pub fn parse_intrinsic(&mut self) -> Result<Option<IntrinsicSyntax>, ParserError> {
        let start: Position = self.lexer.get_previous_position();

        let success_intrinsic_1 = self.lexer.parse_keyword("intrinsic".to_string());
        if !success_intrinsic_1 {

                return Ok(None)
        }

        let end: Position = self.lexer.get_position();

        let ret = IntrinsicSyntax {
            start: start,
            end: end,
        };

        Ok(Some(ret))
    }

    pub fn parse_use(&mut self) -> Result<Option<UseSyntax>, ParserError> {
        let start: Position = self.lexer.get_previous_position();

        let success_use_1 = self.lexer.parse_keyword("use".to_string());
        if !success_use_1 {

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

        let end: Position = self.lexer.get_position();

        let ret = UseSyntax {
            start: start,
            end: end,
            name: name.unwrap(),
        };

        Ok(Some(ret))
    }

    pub fn parse_implement(&mut self) -> Result<Option<ImplementSyntax>, ParserError> {
        let start: Position = self.lexer.get_previous_position();

        let success_implement_1 = self.lexer.parse_keyword("implement".to_string());
        if !success_implement_1 {

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

        let attributes = self.parse_attribute_list()?;

        let success_left_curly_4 = self.lexer.parse_punctuation("{".to_string());
        if !success_left_curly_4 {

            return Result::Err(
                ParserError {
                    file_name: "".to_string(),
                    line: self.lexer.line,
                    column: self.lexer.column,
                },
            )        }

        let functions = self.parse_function_list()?;

        let success_right_curly_6 = self.lexer.parse_punctuation("}".to_string());
        if !success_right_curly_6 {

            return Result::Err(
                ParserError {
                    file_name: "".to_string(),
                    line: self.lexer.line,
                    column: self.lexer.column,
                },
            )        }

        let end: Position = self.lexer.get_position();

        let ret = ImplementSyntax {
            start: start,
            end: end,
            name: name.unwrap(),
            attributes: attributes,
            functions: functions,
        };

        Ok(Some(ret))
    }

    pub fn parse_trait(&mut self) -> Result<Option<TraitSyntax>, ParserError> {
        let start: Position = self.lexer.get_previous_position();

        let success_trait_1 = self.lexer.parse_keyword("trait".to_string());
        if !success_trait_1 {

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

        let extension = self.parse_extends()?;
        if let None = extension {
            return Err(ParserError {
                file_name: "".to_string(),
                line: self.lexer.line,
                column: self.lexer.column,
            });
        }

        let attributes = self.parse_attribute_list()?;

        let success_left_curly_5 = self.lexer.parse_punctuation("{".to_string());
        if !success_left_curly_5 {

            return Result::Err(
                ParserError {
                    file_name: "".to_string(),
                    line: self.lexer.line,
                    column: self.lexer.column,
                },
            )        }

        let functions = self.parse_function_list()?;

        let success_right_curly_7 = self.lexer.parse_punctuation("}".to_string());
        if !success_right_curly_7 {

            return Result::Err(
                ParserError {
                    file_name: "".to_string(),
                    line: self.lexer.line,
                    column: self.lexer.column,
                },
            )        }

        let end: Position = self.lexer.get_position();

        let ret = TraitSyntax {
            start: start,
            end: end,
            name: name.unwrap(),
            extension: extension.unwrap(),
            attributes: attributes,
            functions: functions,
        };

        Ok(Some(ret))
    }

    pub fn parse_extends(&mut self) -> Result<Option<ExtendsSyntax>, ParserError> {
        let start: Position = self.lexer.get_previous_position();

        let success_extends_1 = self.lexer.parse_keyword("extends".to_string());
        if !success_extends_1 {

                return Ok(None)
        }

        let extensions = self.parse_extend_list()?;

        let end: Position = self.lexer.get_position();

        let ret = ExtendsSyntax {
            start: start,
            end: end,
            extensions: extensions,
        };

        Ok(Some(ret))
    }

    pub fn parse_extend_list(&mut self) -> Result<Option<Vec<ExtendSyntax>>, ParserError> {
        let mut array: Option<Vec<ExtendSyntax>> = Option::None;
        loop {
            let node = self.parse_extend()?;
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

    pub fn parse_extend(&mut self) -> Result<Option<ExtendSyntax>, ParserError> {
        let start: Position = self.lexer.get_previous_position();

        let spec = self.parse_typespec()?;
        if let None = spec {
            return Ok(None);
        }

        let success_comma_2 = self.lexer.parse_punctuation(",".to_string());
        if !success_comma_2 {
            ()
        }

        let end: Position = self.lexer.get_position();

        let ret = ExtendSyntax {
            start: start,
            end: end,
            spec: spec.unwrap(),
        };

        Ok(Some(ret))
    }

    pub fn parse_macro(&mut self) -> Result<Option<MacroSyntax>, ParserError> {
        let start: Position = self.lexer.get_previous_position();

        let success_macro_1 = self.lexer.parse_keyword("macro".to_string());
        if !success_macro_1 {

                return Ok(None)
        }

        let name = self.lexer.parse_identifier(&self.keywords);
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

        let model = self.parse_model()?;
        if let None = model {
            return Err(ParserError {
                file_name: "".to_string(),
                line: self.lexer.line,
                column: self.lexer.column,
            });
        }

        let rule = self.parse_operation()?;
        if let None = rule {
            return Err(ParserError {
                file_name: "".to_string(),
                line: self.lexer.line,
                column: self.lexer.column,
            });
        }

        let end: Position = self.lexer.get_position();

        let ret = MacroSyntax {
            start: start,
            end: end,
            name: name.unwrap(),
            model: model.unwrap(),
            rule: rule.unwrap(),
        };

        Ok(Some(ret))
    }

    pub fn parse_model(&mut self) -> Result<Option<ModelSyntax>, ParserError> {
        {
            let node = self.parse_literal()?;
            if let Some(node) = node {
                return Ok(Some(ModelSyntax::Literal(node)));
            }
        }
        {
            let node = self.parse_name()?;
            if let Some(node) = node {
                return Ok(Some(ModelSyntax::Name(node)));
            }
        }
        {
            let node = self.parse_object()?;
            if let Some(node) = node {
                return Ok(Some(ModelSyntax::Object(node)));
            }
        }
        {
            let node = self.parse_array()?;
            if let Some(node) = node {
                return Ok(Some(ModelSyntax::Array(node)));
            }
        }
        return Ok(None)
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
            let node = self.parse_operation()?;
            if let Some(node) = node {
                return Ok(Some(StatementSyntax::Operation(node)));
            }
        }
        {
            let node = self.parse_let()?;
            if let Some(node) = node {
                return Ok(Some(StatementSyntax::Let(node)));
            }
        }
        {
            let node = self.parse_var()?;
            if let Some(node) = node {
                return Ok(Some(StatementSyntax::Var(node)));
            }
        }
        {
            let node = self.parse_mutable()?;
            if let Some(node) = node {
                return Ok(Some(StatementSyntax::Mutable(node)));
            }
        }
        {
            let node = self.parse_set()?;
            if let Some(node) = node {
                return Ok(Some(StatementSyntax::Set(node)));
            }
        }
        {
            let node = self.parse_continue()?;
            if let Some(node) = node {
                return Ok(Some(StatementSyntax::Continue(node)));
            }
        }
        {
            let node = self.parse_break()?;
            if let Some(node) = node {
                return Ok(Some(StatementSyntax::Break(node)));
            }
        }
        {
            let node = self.parse_return()?;
            if let Some(node) = node {
                return Ok(Some(StatementSyntax::Return(node)));
            }
        }
        {
            let node = self.parse_throw()?;
            if let Some(node) = node {
                return Ok(Some(StatementSyntax::Throw(node)));
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

    pub fn parse_var(&mut self) -> Result<Option<VarSyntax>, ParserError> {
        let start: Position = self.lexer.get_previous_position();

        let success_var_1 = self.lexer.parse_keyword("var".to_string());
        if !success_var_1 {

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

        let ret = VarSyntax {
            start: start,
            end: end,
            binding: binding.unwrap(),
        };

        Ok(Some(ret))
    }

    pub fn parse_mutable(&mut self) -> Result<Option<MutableSyntax>, ParserError> {
        let start: Position = self.lexer.get_previous_position();

        let success_mutable_1 = self.lexer.parse_keyword("mutable".to_string());
        if !success_mutable_1 {

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

        let ret = MutableSyntax {
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

        let annotation = self.parse_typeannotation()?;

        let calculation = self.parse_operation()?;
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
            annotation: annotation,
            calculation: calculation.unwrap(),
        };

        Ok(Some(ret))
    }

    pub fn parse_set(&mut self) -> Result<Option<SetSyntax>, ParserError> {
        let start: Position = self.lexer.get_previous_position();

        let success_set_1 = self.lexer.parse_keyword("set".to_string());
        if !success_set_1 {

                return Ok(None)
        }

        let target = self.parse_operation()?;
        if let None = target {
            return Err(ParserError {
                file_name: "".to_string(),
                line: self.lexer.line,
                column: self.lexer.column,
            });
        }

        let source = self.parse_operation()?;
        if let None = source {
            return Err(ParserError {
                file_name: "".to_string(),
                line: self.lexer.line,
                column: self.lexer.column,
            });
        }

        let end: Position = self.lexer.get_position();

        let ret = SetSyntax {
            start: start,
            end: end,
            target: target.unwrap(),
            source: source.unwrap(),
        };

        Ok(Some(ret))
    }

    pub fn parse_operation(&mut self) -> Result<Option<OperationSyntax>, ParserError> {
        let start: Position = self.lexer.get_previous_position();

        let op = self.parse_operand_list()?;
        if let None = op {
            return Ok(None);
        }

        let success_colon_2 = self.lexer.parse_colon();
        if !success_colon_2 {
            ()
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

        let postfixes = self.parse_postfix_list()?;

        let end: Position = self.lexer.get_position();

        let ret = OperandSyntax {
            start: start,
            end: end,
            primary: primary.unwrap(),
            postfixes: postfixes,
        };

        Ok(Some(ret))
    }

    pub fn parse_postfix_list(&mut self) -> Result<Option<Vec<PostfixSyntax>>, ParserError> {
        let mut array: Option<Vec<PostfixSyntax>> = Option::None;
        loop {
            let node = self.parse_postfix()?;
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

    pub fn parse_postfix(&mut self) -> Result<Option<PostfixSyntax>, ParserError> {
        {
            let node = self.parse_memberaccess()?;
            if let Some(node) = node {
                return Ok(Some(PostfixSyntax::MemberAccess(node)));
            }
        }
        {
            let node = self.parse_catcher()?;
            if let Some(node) = node {
                return Ok(Some(PostfixSyntax::Catcher(node)));
            }
        }
        return Ok(None)
    }

    pub fn parse_memberaccess(&mut self) -> Result<Option<MemberAccessSyntax>, ParserError> {
        let start: Position = self.lexer.get_previous_position();

        let success_dot_1 = self.lexer.parse_punctuation(".".to_string());
        if !success_dot_1 {

                return Ok(None)
        }

        let member = self.parse_name()?;
        if let None = member {
            return Err(ParserError {
                file_name: "".to_string(),
                line: self.lexer.line,
                column: self.lexer.column,
            });
        }

        let end: Position = self.lexer.get_position();

        let ret = MemberAccessSyntax {
            start: start,
            end: end,
            member: member.unwrap(),
        };

        Ok(Some(ret))
    }

    pub fn parse_catcher(&mut self) -> Result<Option<CatcherSyntax>, ParserError> {
        let start: Position = self.lexer.get_previous_position();

        let catchers = self.parse_catch_list()?;
        if let None = catchers {
            return Ok(None);
        }

        let dropper = self.parse_drop()?;

        let end: Position = self.lexer.get_position();

        let ret = CatcherSyntax {
            start: start,
            end: end,
            catchers: catchers.unwrap(),
            dropper: dropper,
        };

        Ok(Some(ret))
    }

    pub fn parse_catch_list(&mut self) -> Result<Option<Vec<CatchSyntax>>, ParserError> {
        let mut array: Option<Vec<CatchSyntax>> = Option::None;
        loop {
            let node = self.parse_catch()?;
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

    pub fn parse_catch(&mut self) -> Result<Option<CatchSyntax>, ParserError> {
        let start: Position = self.lexer.get_previous_position();

        let success_catch_1 = self.lexer.parse_keyword("catch".to_string());
        if !success_catch_1 {

                return Ok(None)
        }

        let condition = self.parse_operation()?;
        if let None = condition {
            return Err(ParserError {
                file_name: "".to_string(),
                line: self.lexer.line,
                column: self.lexer.column,
            });
        }

        let handler = self.parse_operation()?;
        if let None = handler {
            return Err(ParserError {
                file_name: "".to_string(),
                line: self.lexer.line,
                column: self.lexer.column,
            });
        }

        let end: Position = self.lexer.get_position();

        let ret = CatchSyntax {
            start: start,
            end: end,
            condition: condition.unwrap(),
            handler: handler.unwrap(),
        };

        Ok(Some(ret))
    }

    pub fn parse_drop(&mut self) -> Result<Option<DropSyntax>, ParserError> {
        let start: Position = self.lexer.get_previous_position();

        let success_drop_1 = self.lexer.parse_keyword("drop".to_string());
        if !success_drop_1 {

                return Ok(None)
        }

        let handler = self.parse_operation()?;
        if let None = handler {
            return Err(ParserError {
                file_name: "".to_string(),
                line: self.lexer.line,
                column: self.lexer.column,
            });
        }

        let end: Position = self.lexer.get_position();

        let ret = DropSyntax {
            start: start,
            end: end,
            handler: handler.unwrap(),
        };

        Ok(Some(ret))
    }

    pub fn parse_continue(&mut self) -> Result<Option<ContinueSyntax>, ParserError> {
        let start: Position = self.lexer.get_previous_position();

        let success_continue_1 = self.lexer.parse_keyword("continue".to_string());
        if !success_continue_1 {

                return Ok(None)
        }

        let name = self.parse_loop()?;

        let success_colon_3 = self.lexer.parse_colon();
        if !success_colon_3 {
            ()
        }

        let end: Position = self.lexer.get_position();

        let ret = ContinueSyntax {
            start: start,
            end: end,
            name: name,
        };

        Ok(Some(ret))
    }

    pub fn parse_loop(&mut self) -> Result<Option<LoopSyntax>, ParserError> {
        let start: Position = self.lexer.get_previous_position();

        let success_loop_1 = self.lexer.parse_keyword("loop".to_string());
        if !success_loop_1 {

                return Ok(None)
        }

        let name = self.lexer.parse_identifier(&self.keywords);
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

        let ret = LoopSyntax {
            start: start,
            end: end,
            name: name.unwrap(),
        };

        Ok(Some(ret))
    }

    pub fn parse_break(&mut self) -> Result<Option<BreakSyntax>, ParserError> {
        let start: Position = self.lexer.get_previous_position();

        let success_break_1 = self.lexer.parse_keyword("break".to_string());
        if !success_break_1 {

                return Ok(None)
        }

        let name = self.parse_loop()?;

        let result = self.parse_operation()?;
        if let None = result {
            return Err(ParserError {
                file_name: "".to_string(),
                line: self.lexer.line,
                column: self.lexer.column,
            });
        }

        let end: Position = self.lexer.get_position();

        let ret = BreakSyntax {
            start: start,
            end: end,
            name: name,
            result: result.unwrap(),
        };

        Ok(Some(ret))
    }

    pub fn parse_return(&mut self) -> Result<Option<ReturnSyntax>, ParserError> {
        let start: Position = self.lexer.get_previous_position();

        let success_return_1 = self.lexer.parse_keyword("return".to_string());
        if !success_return_1 {

                return Ok(None)
        }

        let result = self.parse_operation()?;

        let end: Position = self.lexer.get_position();

        let ret = ReturnSyntax {
            start: start,
            end: end,
            result: result,
        };

        Ok(Some(ret))
    }

    pub fn parse_throw(&mut self) -> Result<Option<ThrowSyntax>, ParserError> {
        let start: Position = self.lexer.get_previous_position();

        let success_throw_1 = self.lexer.parse_keyword("throw".to_string());
        if !success_throw_1 {

                return Ok(None)
        }

        let result = self.parse_operation()?;

        let end: Position = self.lexer.get_position();

        let ret = ThrowSyntax {
            start: start,
            end: end,
            result: result,
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
            let node = self.parse_object()?;
            if let Some(node) = node {
                return Ok(Some(ExpressionSyntax::Object(node)));
            }
        }
        {
            let node = self.parse_array()?;
            if let Some(node) = node {
                return Ok(Some(ExpressionSyntax::Array(node)));
            }
        }
        {
            let node = self.parse_block()?;
            if let Some(node) = node {
                return Ok(Some(ExpressionSyntax::Block(node)));
            }
        }
        {
            let node = self.parse_if()?;
            if let Some(node) = node {
                return Ok(Some(ExpressionSyntax::If(node)));
            }
        }
        {
            let node = self.parse_match()?;
            if let Some(node) = node {
                return Ok(Some(ExpressionSyntax::Match(node)));
            }
        }
        {
            let node = self.parse_lambda()?;
            if let Some(node) = node {
                return Ok(Some(ExpressionSyntax::Lambda(node)));
            }
        }
        {
            let node = self.parse_for()?;
            if let Some(node) = node {
                return Ok(Some(ExpressionSyntax::For(node)));
            }
        }
        {
            let node = self.parse_while()?;
            if let Some(node) = node {
                return Ok(Some(ExpressionSyntax::While(node)));
            }
        }
        {
            let node = self.parse_repeat()?;
            if let Some(node) = node {
                return Ok(Some(ExpressionSyntax::Repeat(node)));
            }
        }
        {
            let node = self.parse_sizeof()?;
            if let Some(node) = node {
                return Ok(Some(ExpressionSyntax::SizeOf(node)));
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

    pub fn parse_object(&mut self) -> Result<Option<ObjectSyntax>, ParserError> {
        let start: Position = self.lexer.get_previous_position();

        let success_left_paren_1 = self.lexer.parse_punctuation("(".to_string());
        if !success_left_paren_1 {

                return Ok(None)
        }

        let fields = self.parse_field_list()?;

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

        let ret = ObjectSyntax {
            start: start,
            end: end,
            fields: fields,
        };

        Ok(Some(ret))
    }

    pub fn parse_field_list(&mut self) -> Result<Option<Vec<FieldSyntax>>, ParserError> {
        let mut array: Option<Vec<FieldSyntax>> = Option::None;
        loop {
            let node = self.parse_field()?;
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

    pub fn parse_field(&mut self) -> Result<Option<FieldSyntax>, ParserError> {
        let start: Position = self.lexer.get_previous_position();

        let operation = self.parse_operation()?;
        if let None = operation {
            return Ok(None);
        }

        let attributes = self.parse_attribute_list()?;

        let value = self.parse_value()?;

        let success_comma_4 = self.lexer.parse_punctuation(",".to_string());
        if !success_comma_4 {
            ()
        }

        let end: Position = self.lexer.get_position();

        let ret = FieldSyntax {
            start: start,
            end: end,
            operation: operation.unwrap(),
            attributes: attributes,
            value: value,
        };

        Ok(Some(ret))
    }

    pub fn parse_value(&mut self) -> Result<Option<ValueSyntax>, ParserError> {
        let start: Position = self.lexer.get_previous_position();

        let success_colon_1 = self.lexer.parse_colon();
        if !success_colon_1 {

                return Ok(None)
        }

        let value = self.parse_operation()?;
        if let None = value {
            return Err(ParserError {
                file_name: "".to_string(),
                line: self.lexer.line,
                column: self.lexer.column,
            });
        }

        let attributes = self.parse_attribute_list()?;

        let end: Position = self.lexer.get_position();

        let ret = ValueSyntax {
            start: start,
            end: end,
            value: value.unwrap(),
            attributes: attributes,
        };

        Ok(Some(ret))
    }

    pub fn parse_array(&mut self) -> Result<Option<ArraySyntax>, ParserError> {
        let start: Position = self.lexer.get_previous_position();

        let success_left_bracket_1 = self.lexer.parse_punctuation("[".to_string());
        if !success_left_bracket_1 {

                return Ok(None)
        }

        let elements = self.parse_element_list()?;
        if let None = elements {
            return Err(ParserError {
                file_name: "".to_string(),
                line: self.lexer.line,
                column: self.lexer.column,
            });
        }

        let success_right_bracket_3 = self.lexer.parse_punctuation("]".to_string());
        if !success_right_bracket_3 {

            return Result::Err(
                ParserError {
                    file_name: "".to_string(),
                    line: self.lexer.line,
                    column: self.lexer.column,
                },
            )        }

        let end: Position = self.lexer.get_position();

        let ret = ArraySyntax {
            start: start,
            end: end,
            elements: elements.unwrap(),
        };

        Ok(Some(ret))
    }

    pub fn parse_element_list(&mut self) -> Result<Option<Vec<ElementSyntax>>, ParserError> {
        let mut array: Option<Vec<ElementSyntax>> = Option::None;
        loop {
            let node = self.parse_element()?;
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

    pub fn parse_element(&mut self) -> Result<Option<ElementSyntax>, ParserError> {
        let start: Position = self.lexer.get_previous_position();

        let operation = self.parse_operation()?;
        if let None = operation {
            return Ok(None);
        }

        let attributes = self.parse_attribute_list()?;

        let success_comma_3 = self.lexer.parse_punctuation(",".to_string());
        if !success_comma_3 {
            ()
        }

        let end: Position = self.lexer.get_position();

        let ret = ElementSyntax {
            start: start,
            end: end,
            operation: operation.unwrap(),
            attributes: attributes,
        };

        Ok(Some(ret))
    }

    pub fn parse_block(&mut self) -> Result<Option<BlockSyntax>, ParserError> {
        let start: Position = self.lexer.get_previous_position();

        let success_left_curly_1 = self.lexer.parse_punctuation("{".to_string());
        if !success_left_curly_1 {

                return Ok(None)
        }

        let statements = self.parse_statement_list()?;

        let success_right_curly_3 = self.lexer.parse_punctuation("}".to_string());
        if !success_right_curly_3 {

            return Result::Err(
                ParserError {
                    file_name: "".to_string(),
                    line: self.lexer.line,
                    column: self.lexer.column,
                },
            )        }

        let end: Position = self.lexer.get_position();

        let ret = BlockSyntax {
            start: start,
            end: end,
            statements: statements,
        };

        Ok(Some(ret))
    }

    pub fn parse_if(&mut self) -> Result<Option<IfSyntax>, ParserError> {
        let start: Position = self.lexer.get_previous_position();

        let success_if_1 = self.lexer.parse_keyword("if".to_string());
        if !success_if_1 {

                return Ok(None)
        }

        let condition = self.parse_operation()?;
        if let None = condition {
            return Err(ParserError {
                file_name: "".to_string(),
                line: self.lexer.line,
                column: self.lexer.column,
            });
        }

        let consequent = self.parse_operation()?;
        if let None = consequent {
            return Err(ParserError {
                file_name: "".to_string(),
                line: self.lexer.line,
                column: self.lexer.column,
            });
        }

        let alternative = self.parse_else()?;

        let end: Position = self.lexer.get_position();

        let ret = IfSyntax {
            start: start,
            end: end,
            condition: condition.unwrap(),
            consequent: consequent.unwrap(),
            alternative: alternative,
        };

        Ok(Some(ret))
    }

    pub fn parse_else(&mut self) -> Result<Option<ElseSyntax>, ParserError> {
        let start: Position = self.lexer.get_previous_position();

        let success_else_1 = self.lexer.parse_keyword("else".to_string());
        if !success_else_1 {

                return Ok(None)
        }

        let alternative = self.parse_operation()?;
        if let None = alternative {
            return Err(ParserError {
                file_name: "".to_string(),
                line: self.lexer.line,
                column: self.lexer.column,
            });
        }

        let end: Position = self.lexer.get_position();

        let ret = ElseSyntax {
            start: start,
            end: end,
            alternative: alternative.unwrap(),
        };

        Ok(Some(ret))
    }

    pub fn parse_match(&mut self) -> Result<Option<MatchSyntax>, ParserError> {
        let start: Position = self.lexer.get_previous_position();

        let success_match_1 = self.lexer.parse_keyword("match".to_string());
        if !success_match_1 {

                return Ok(None)
        }

        let scrutinee = self.parse_operation()?;
        if let None = scrutinee {
            return Err(ParserError {
                file_name: "".to_string(),
                line: self.lexer.line,
                column: self.lexer.column,
            });
        }

        let cases = self.parse_case_list()?;
        if let None = cases {
            return Err(ParserError {
                file_name: "".to_string(),
                line: self.lexer.line,
                column: self.lexer.column,
            });
        }

        let alternative = self.parse_default()?;

        let end: Position = self.lexer.get_position();

        let ret = MatchSyntax {
            start: start,
            end: end,
            scrutinee: scrutinee.unwrap(),
            cases: cases.unwrap(),
            alternative: alternative,
        };

        Ok(Some(ret))
    }

    pub fn parse_case_list(&mut self) -> Result<Option<Vec<CaseSyntax>>, ParserError> {
        let mut array: Option<Vec<CaseSyntax>> = Option::None;
        loop {
            let node = self.parse_case()?;
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

    pub fn parse_case(&mut self) -> Result<Option<CaseSyntax>, ParserError> {
        let start: Position = self.lexer.get_previous_position();

        let success_case_1 = self.lexer.parse_keyword("case".to_string());
        if !success_case_1 {

                return Ok(None)
        }

        let condition = self.parse_operation()?;
        if let None = condition {
            return Err(ParserError {
                file_name: "".to_string(),
                line: self.lexer.line,
                column: self.lexer.column,
            });
        }

        let consequent = self.parse_operation()?;
        if let None = consequent {
            return Err(ParserError {
                file_name: "".to_string(),
                line: self.lexer.line,
                column: self.lexer.column,
            });
        }

        let end: Position = self.lexer.get_position();

        let ret = CaseSyntax {
            start: start,
            end: end,
            condition: condition.unwrap(),
            consequent: consequent.unwrap(),
        };

        Ok(Some(ret))
    }

    pub fn parse_default(&mut self) -> Result<Option<DefaultSyntax>, ParserError> {
        let start: Position = self.lexer.get_previous_position();

        let success_default_1 = self.lexer.parse_keyword("default".to_string());
        if !success_default_1 {

                return Ok(None)
        }

        let alternative = self.parse_operation()?;

        let end: Position = self.lexer.get_position();

        let ret = DefaultSyntax {
            start: start,
            end: end,
            alternative: alternative,
        };

        Ok(Some(ret))
    }

    pub fn parse_lambda(&mut self) -> Result<Option<LambdaSyntax>, ParserError> {
        let start: Position = self.lexer.get_previous_position();

        let success_lambda_1 = self.lexer.parse_keyword("lambda".to_string());
        if !success_lambda_1 {

                return Ok(None)
        }

        let input = self.parse_operation()?;
        if let None = input {
            return Err(ParserError {
                file_name: "".to_string(),
                line: self.lexer.line,
                column: self.lexer.column,
            });
        }

        let block = self.parse_operation()?;
        if let None = block {
            return Err(ParserError {
                file_name: "".to_string(),
                line: self.lexer.line,
                column: self.lexer.column,
            });
        }

        let end: Position = self.lexer.get_position();

        let ret = LambdaSyntax {
            start: start,
            end: end,
            input: input.unwrap(),
            block: block.unwrap(),
        };

        Ok(Some(ret))
    }

    pub fn parse_for(&mut self) -> Result<Option<ForSyntax>, ParserError> {
        let start: Position = self.lexer.get_previous_position();

        let success_for_1 = self.lexer.parse_keyword("for".to_string());
        if !success_for_1 {

                return Ok(None)
        }

        let condition = self.parse_operation()?;
        if let None = condition {
            return Err(ParserError {
                file_name: "".to_string(),
                line: self.lexer.line,
                column: self.lexer.column,
            });
        }

        let success_in_3 = self.lexer.parse_keyword("in".to_string());
        if !success_in_3 {

            return Result::Err(
                ParserError {
                    file_name: "".to_string(),
                    line: self.lexer.line,
                    column: self.lexer.column,
                },
            )        }

        let expression = self.parse_operation()?;
        if let None = expression {
            return Err(ParserError {
                file_name: "".to_string(),
                line: self.lexer.line,
                column: self.lexer.column,
            });
        }

        let name = self.parse_label()?;

        let action = self.parse_operation()?;
        if let None = action {
            return Err(ParserError {
                file_name: "".to_string(),
                line: self.lexer.line,
                column: self.lexer.column,
            });
        }

        let end: Position = self.lexer.get_position();

        let ret = ForSyntax {
            start: start,
            end: end,
            condition: condition.unwrap(),
            expression: expression.unwrap(),
            name: name,
            action: action.unwrap(),
        };

        Ok(Some(ret))
    }

    pub fn parse_label(&mut self) -> Result<Option<LabelSyntax>, ParserError> {
        let start: Position = self.lexer.get_previous_position();

        let success_label_1 = self.lexer.parse_keyword("label".to_string());
        if !success_label_1 {

                return Ok(None)
        }

        let name = self.lexer.parse_identifier(&self.keywords);
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

        let ret = LabelSyntax {
            start: start,
            end: end,
            name: name.unwrap(),
        };

        Ok(Some(ret))
    }

    pub fn parse_while(&mut self) -> Result<Option<WhileSyntax>, ParserError> {
        let start: Position = self.lexer.get_previous_position();

        let success_while_1 = self.lexer.parse_keyword("while".to_string());
        if !success_while_1 {

                return Ok(None)
        }

        let condition = self.parse_operation()?;
        if let None = condition {
            return Err(ParserError {
                file_name: "".to_string(),
                line: self.lexer.line,
                column: self.lexer.column,
            });
        }

        let name = self.parse_label()?;

        let action = self.parse_operation()?;
        if let None = action {
            return Err(ParserError {
                file_name: "".to_string(),
                line: self.lexer.line,
                column: self.lexer.column,
            });
        }

        let end: Position = self.lexer.get_position();

        let ret = WhileSyntax {
            start: start,
            end: end,
            condition: condition.unwrap(),
            name: name,
            action: action.unwrap(),
        };

        Ok(Some(ret))
    }

    pub fn parse_repeat(&mut self) -> Result<Option<RepeatSyntax>, ParserError> {
        let start: Position = self.lexer.get_previous_position();

        let success_repeat_1 = self.lexer.parse_keyword("repeat".to_string());
        if !success_repeat_1 {

                return Ok(None)
        }

        let name = self.parse_label()?;

        let action = self.parse_operation()?;
        if let None = action {
            return Err(ParserError {
                file_name: "".to_string(),
                line: self.lexer.line,
                column: self.lexer.column,
            });
        }

        let end: Position = self.lexer.get_position();

        let ret = RepeatSyntax {
            start: start,
            end: end,
            name: name,
            action: action.unwrap(),
        };

        Ok(Some(ret))
    }

    pub fn parse_sizeof(&mut self) -> Result<Option<SizeOfSyntax>, ParserError> {
        let start: Position = self.lexer.get_previous_position();

        let success_sizeof_1 = self.lexer.parse_keyword("sizeof".to_string());
        if !success_sizeof_1 {

                return Ok(None)
        }

        let spec = self.parse_type()?;
        if let None = spec {
            return Err(ParserError {
                file_name: "".to_string(),
                line: self.lexer.line,
                column: self.lexer.column,
            });
        }

        let end: Position = self.lexer.get_position();

        let ret = SizeOfSyntax {
            start: start,
            end: end,
            spec: spec.unwrap(),
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
