use scaly::containers::{Array, HashSet, Ref, String, Vector};
use scaly::io::Stream;
use scaly::memory::Region;
use scaly::Page;
use scalyc::errors::ParserError;
use scalyc::lexer::{Lexer, Literal, Position};

pub struct Parser {
    lexer: Ref<Lexer>,
    file_name: String,
    keywords: Ref<HashSet<String>>,
}

impl Parser {
    pub fn new(_pr: &Region, _rp: *mut Page, file_name: String, stream: *mut Stream) -> Parser {
        let _r = Region::create(_pr);
        let keywords = HashSet::from_vector(
            &_r,
            _rp,
            Ref::new(
                _rp,
                Vector::from_raw_array(
                    _rp,
                    &[
                        String::from_string_slice(_rp, "intrinsic"),
                    ],
                ),
            ),
        );
        Parser {
            lexer: Lexer::new(&_r,_rp, stream),
            file_name: file_name,
            keywords: keywords,
        }
    }

    pub fn parse_file(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Option<Ref<FileSyntax>>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        let intrinsics = self.parse_intrinsic_list(&_r, _rp, _ep)?;

        let statements = self.parse_statement_list(&_r, _rp, _ep)?;
        if let Some(_) = statements {
            if !self.is_at_end() {
                let error_pos = self.lexer.get_previous_position();
                return Result::Err(Ref::new(
                    _ep,
                    ParserError {
                        file_name: String::copy(_ep, self.file_name),
                        line: error_pos.line,
                        column: error_pos.column,
                    },
                ))
            }
        }

        let ret = Ref::new(_rp, FileSyntax { file_name: self.file_name, intrinsics: intrinsics, statements: statements });

        if let Some(mut intrinsics) = intrinsics {
            for item in intrinsics.iter_mut() {
                item.parent = Some(ParentNode::File(ret));
            }
        }

        if let Some(mut statements) = statements {
            for item in statements.iter_mut() {
                match **item {
                    StatementSyntax::Calculation(mut y) => y.parent = Some(ParentNode::File(ret)),
                }
            }
        }

        Ok(Some(ret))
    }

    pub fn parse_intrinsic_list(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Option<Ref<Vector<Ref<IntrinsicSyntax>>>>, Ref<ParserError>> {
        let _r = Region::create(_pr);
        let mut ret: Option<Ref<Array<Ref<IntrinsicSyntax>>>> = Option::None;
        loop {
            let node = self.parse_intrinsic(&_r, _rp, _ep)?;
            if let Some(node) = node {
                if let None = ret {
                    ret = Some(Ref::new(_rp, Array::new()))
                };
                ret.unwrap().add(node);
            } else {
                break;
            }
        }

        if let Some(ret) = ret {
            Ok(Some(Ref::new(_rp, Vector::from_array(_rp, ret))))
        } else {
            Ok(None)
        }
    }

    pub fn parse_intrinsic(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Option<Ref<IntrinsicSyntax>>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        let start: Position = self.lexer.get_previous_position();

        let success_intrinsic_1 = self.lexer.parse_keyword(&_r, String::from_string_slice(_r.page, "intrinsic"));
        if !success_intrinsic_1 {
            return Ok(None)
        }

        let name = self.lexer.parse_identifier(&_r, _rp);
        if let Some(name) = name {
            if !self.is_identifier(name) {

            return Result::Err(Ref::new(
                _ep,
                ParserError {
                    file_name: self.file_name,
                    line: self.lexer.line,
                    column: self.lexer.column,
                },
            ))
           }
        } else {
            return Result::Err(Ref::new(
                _ep,
                ParserError {
                    file_name: self.file_name,
                    line: self.lexer.line,
                    column: self.lexer.column,
                },
            ))
        }

        let end: Position = self.lexer.get_position();

        let ret = Ref::new(_rp, IntrinsicSyntax { parent: None, start: start, end: end, name: name.unwrap() });

        Ok(Some(ret))
    }

    pub fn parse_statement_list(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Option<Ref<Vector<Ref<StatementSyntax>>>>, Ref<ParserError>> {
        let _r = Region::create(_pr);
        let mut ret: Option<Ref<Array<Ref<StatementSyntax>>>> = Option::None;
        loop {
            let node = self.parse_statement(&_r, _rp, _ep)?;
            if let Some(node) = node {
                if let None = ret {
                    ret = Some(Ref::new(_rp, Array::new()))
                };
                ret.unwrap().add(node);
            } else {
                break;
            }
        }

        if let Some(ret) = ret {
            Ok(Some(Ref::new(_rp, Vector::from_array(_rp, ret))))
        } else {
            Ok(None)
        }
    }

    pub fn parse_statement(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Option<Ref<StatementSyntax>>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        {
            let node = self.parse_calculation(&_r, _rp, _ep)?;
            if let Some(node) = node {
                return Ok(Some(Ref::new(_rp, StatementSyntax::Calculation(*node))))
            }
        }
        return Ok(None)
    }

    pub fn parse_calculation(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Option<Ref<CalculationSyntax>>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        let start: Position = self.lexer.get_previous_position();

        let operation = self.parse_operation(&_r, _rp, _ep)?;
        if let None = operation {
            return Ok(None)
        }

        let success_semicolon_2 = self.lexer.parse_punctuation(&_r, String::from_string_slice(_r.page, ";"));
        if !success_semicolon_2 {

            ()
        }

        let end: Position = self.lexer.get_position();

        let ret = Ref::new(_rp, CalculationSyntax { parent: None, start: start, end: end, operation: operation.unwrap() });

        operation.unwrap().parent = Some(ParentNode::Calculation(ret));

        Ok(Some(ret))
    }

    pub fn parse_operation(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Option<Ref<OperationSyntax>>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        let start: Position = self.lexer.get_previous_position();

        let op = self.parse_operand_list(&_r, _rp, _ep)?;
        if let None = op {
            return Ok(None)
        }

        let end: Position = self.lexer.get_position();

        let ret = Ref::new(_rp, OperationSyntax { parent: None, start: start, end: end, op: op.unwrap() });

        if let Some(mut op) = op {
            for item in op.iter_mut() {
                item.parent = Some(ParentNode::Operation(ret));
            }
        }

        Ok(Some(ret))
    }

    pub fn parse_operand_list(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Option<Ref<Vector<Ref<OperandSyntax>>>>, Ref<ParserError>> {
        let _r = Region::create(_pr);
        let mut ret: Option<Ref<Array<Ref<OperandSyntax>>>> = Option::None;
        loop {
            let node = self.parse_operand(&_r, _rp, _ep)?;
            if let Some(node) = node {
                if let None = ret {
                    ret = Some(Ref::new(_rp, Array::new()))
                };
                ret.unwrap().add(node);
            } else {
                break;
            }
        }

        if let Some(ret) = ret {
            Ok(Some(Ref::new(_rp, Vector::from_array(_rp, ret))))
        } else {
            Ok(None)
        }
    }

    pub fn parse_operand(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Option<Ref<OperandSyntax>>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        let start: Position = self.lexer.get_previous_position();

        let primary = self.parse_expression(&_r, _rp, _ep)?;
        if let None = primary {
            return Ok(None)
        }

        let end: Position = self.lexer.get_position();

        let ret = Ref::new(_rp, OperandSyntax { parent: None, start: start, end: end, primary: primary.unwrap() });

        match *primary.unwrap() {
            ExpressionSyntax::Constant(mut x) => x.parent = Some(ParentNode::Operand(ret)),
        }

        Ok(Some(ret))
    }

    pub fn parse_expression(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Option<Ref<ExpressionSyntax>>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        {
            let node = self.parse_constant(&_r, _rp, _ep)?;
            if let Some(node) = node {
                return Ok(Some(Ref::new(_rp, ExpressionSyntax::Constant(*node))))
            }
        }
        return Ok(None)
    }

    pub fn parse_constant(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Option<Ref<ConstantSyntax>>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        let start: Position = self.lexer.get_previous_position();

        let literal = self.lexer.parse_literal(&_r, _rp);
        if let None = literal {            return Ok(None)
        }

        let end: Position = self.lexer.get_position();

        let ret = Ref::new(_rp, ConstantSyntax { parent: None, start: start, end: end, literal: literal.unwrap() });

        Ok(Some(ret))
    }

    fn is_at_end(&self) -> bool {
        self.lexer.is_at_end()
    }

    fn is_identifier(&self, id: String) -> bool {
        if self.keywords.contains(id) {
            false
        } else {
            true
        }
    }
}

pub trait SyntaxVisitor {
    fn open_program(&mut self, program_syntax: Ref<ProgramSyntax>) -> bool;
    fn close_program(&mut self, program_syntax: Ref<ProgramSyntax>);

    fn open_file(&mut self, file_syntax: Ref<FileSyntax>) -> bool;
    fn close_file(&mut self, file_syntax: Ref<FileSyntax>);

    fn visit_intrinsic(&mut self, intrinsic_syntax: Ref<IntrinsicSyntax>);

    fn open_calculation(&mut self, calculation_syntax: Ref<CalculationSyntax>) -> bool;
    fn close_calculation(&mut self, calculation_syntax: Ref<CalculationSyntax>);

    fn open_operation(&mut self, operation_syntax: Ref<OperationSyntax>) -> bool;
    fn close_operation(&mut self, operation_syntax: Ref<OperationSyntax>);

    fn open_operand(&mut self, operand_syntax: Ref<OperandSyntax>) -> bool;
    fn close_operand(&mut self, operand_syntax: Ref<OperandSyntax>);

    fn visit_constant(&mut self, constant_syntax: Ref<ConstantSyntax>);
}

pub trait SyntaxNode {
    fn accept(&mut self, visitor: *mut SyntaxVisitor);
}

#[derive(Copy, Clone)]
pub enum ParentNode {
    File(Ref<FileSyntax>),
    Calculation(Ref<CalculationSyntax>),
    Operation(Ref<OperationSyntax>),
    Operand(Ref<OperandSyntax>),
}

#[derive(Copy, Clone)]
pub struct ProgramSyntax {
    pub start: Position,
    pub end: Position,
    pub parent: Option<ParentNode>,
    pub name: String,
    pub files: Ref<Vector<Ref<FileSyntax>>>,
}

impl SyntaxNode for ProgramSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        unsafe {
            if !(*visitor).open_program(Ref::from(self as *mut ProgramSyntax)) {
                return
            }
        }
        for node in self.files.iter_mut() {
            node.accept(visitor)
        };
        unsafe {
            (*visitor).close_program(Ref::from(self as *mut ProgramSyntax))
        }
    }
}

#[derive(Copy, Clone)]
pub struct FileSyntax {
    pub file_name: String,
    pub intrinsics: Option<Ref<Vector<Ref<IntrinsicSyntax>>>>,
    pub statements: Option<Ref<Vector<Ref<StatementSyntax>>>>,
}

impl SyntaxNode for FileSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        unsafe {
            if !(*visitor).open_file(Ref::from(self as *mut FileSyntax)) {
                return
            }
        }
        match self.intrinsics {
            Some(mut x) => for node in x.iter_mut() {
                node.accept(visitor)
            },
            None => ()
        };

        match self.statements {
            Some(mut x) => for node in x.iter_mut() {
                node.accept(visitor)
            },
            None => ()
        };
        unsafe {
            (*visitor).close_file(Ref::from(self as *mut FileSyntax))
        }
    }
}

#[derive(Copy, Clone)]
pub struct IntrinsicSyntax {
    pub start: Position,
    pub end: Position,
    pub parent: Option<ParentNode>,
    pub name: String,
}

impl SyntaxNode for IntrinsicSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        unsafe {
            (*visitor).visit_intrinsic(Ref::from(self as *mut IntrinsicSyntax))
        }
    }
}

#[derive(Copy, Clone)]
pub enum StatementSyntax {
   Calculation(CalculationSyntax),
}

impl SyntaxNode for StatementSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        match self {
            StatementSyntax::Calculation(syntax) => syntax.accept(visitor),
        }
    }
}

#[derive(Copy, Clone)]
pub struct CalculationSyntax {
    pub start: Position,
    pub end: Position,
    pub parent: Option<ParentNode>,
    pub operation: Ref<OperationSyntax>,
}

impl SyntaxNode for CalculationSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        unsafe {
            if !(*visitor).open_calculation(Ref::from(self as *mut CalculationSyntax)) {
                return
            }
        }
        self.operation.accept(visitor);
            unsafe {
            (*visitor).close_calculation(Ref::from(self as *mut CalculationSyntax))
        }
    }
}

#[derive(Copy, Clone)]
pub struct OperationSyntax {
    pub start: Position,
    pub end: Position,
    pub parent: Option<ParentNode>,
    pub op: Ref<Vector<Ref<OperandSyntax>>>,
}

impl SyntaxNode for OperationSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        unsafe {
            if !(*visitor).open_operation(Ref::from(self as *mut OperationSyntax)) {
                return
            }
        }
        for node in self.op.iter_mut() {
            node.accept(visitor)
        };
        unsafe {
            (*visitor).close_operation(Ref::from(self as *mut OperationSyntax))
        }
    }
}

#[derive(Copy, Clone)]
pub struct OperandSyntax {
    pub start: Position,
    pub end: Position,
    pub parent: Option<ParentNode>,
    pub primary: Ref<ExpressionSyntax>,
}

impl SyntaxNode for OperandSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        unsafe {
            if !(*visitor).open_operand(Ref::from(self as *mut OperandSyntax)) {
                return
            }
        }
        self.primary.accept(visitor);
            unsafe {
            (*visitor).close_operand(Ref::from(self as *mut OperandSyntax))
        }
    }
}

#[derive(Copy, Clone)]
pub enum ExpressionSyntax {
   Constant(ConstantSyntax),
}

impl SyntaxNode for ExpressionSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        match self {
            ExpressionSyntax::Constant(syntax) => syntax.accept(visitor),
        }
    }
}

#[derive(Copy, Clone)]
pub struct ConstantSyntax {
    pub start: Position,
    pub end: Position,
    pub parent: Option<ParentNode>,
    pub literal: Literal,
}

impl SyntaxNode for ConstantSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        unsafe {
            (*visitor).visit_constant(Ref::from(self as *mut ConstantSyntax))
        }
    }
}
