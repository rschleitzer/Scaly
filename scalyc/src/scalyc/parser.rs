use scaly::containers::{Array, HashSet, Ref, String, Vector};
use scaly::io::Stream;
use scaly::memory::Region;
use scaly::Page;
use scalyc::errors::ParserError;
use scalyc::lexer::Lexer;
use scalyc::lexer::Position;
use std::ptr::null_mut;

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
                        String::from_string_slice(_rp, "using"),
                        String::from_string_slice(_rp, "namespace"),
                        String::from_string_slice(_rp, "typedef"),
                        String::from_string_slice(_rp, "let"),
                        String::from_string_slice(_rp, "mutable"),
                        String::from_string_slice(_rp, "threadlocal"),
                        String::from_string_slice(_rp, "var"),
                        String::from_string_slice(_rp, "set"),
                        String::from_string_slice(_rp, "class"),
                        String::from_string_slice(_rp, "extends"),
                        String::from_string_slice(_rp, "initializer"),
                        String::from_string_slice(_rp, "allocator"),
                        String::from_string_slice(_rp, "method"),
                        String::from_string_slice(_rp, "function"),
                        String::from_string_slice(_rp, "operator"),
                        String::from_string_slice(_rp, "this"),
                        String::from_string_slice(_rp, "new"),
                        String::from_string_slice(_rp, "sizeof"),
                        String::from_string_slice(_rp, "catch"),
                        String::from_string_slice(_rp, "throws"),
                        String::from_string_slice(_rp, "as"),
                        String::from_string_slice(_rp, "is"),
                        String::from_string_slice(_rp, "if"),
                        String::from_string_slice(_rp, "else"),
                        String::from_string_slice(_rp, "switch"),
                        String::from_string_slice(_rp, "case"),
                        String::from_string_slice(_rp, "default"),
                        String::from_string_slice(_rp, "for"),
                        String::from_string_slice(_rp, "in"),
                        String::from_string_slice(_rp, "while"),
                        String::from_string_slice(_rp, "do"),
                        String::from_string_slice(_rp, "loop"),
                        String::from_string_slice(_rp, "break"),
                        String::from_string_slice(_rp, "continue"),
                        String::from_string_slice(_rp, "return"),
                        String::from_string_slice(_rp, "throw"),
                        String::from_string_slice(_rp, "intrinsic"),
                        String::from_string_slice(_rp, "define"),
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

        let start: Position = self.lexer.get_previous_position();

        let intrinsics = self.parse_intrinsic_list(&_r, _rp, _ep)?;

        let usings = self.parse_using_list(&_r, _rp, _ep)?;

        let defines = self.parse_define_list(&_r, _rp, _ep)?;

        let declarations = self.parse_declaration_list(&_r, _rp, _ep)?;

        let statements = self.parse_statement_list(&_r, _rp, _ep)?;
        match statements {
            Some(_) =>
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
            None => (),
        }

        let end: Position = self.lexer.get_position();

        let ret = Ref::new(_rp, FileSyntax { file_name: self.file_name, intrinsics: intrinsics, usings: usings, defines: defines, declarations: declarations, statements: statements });

        match intrinsics {
            Some(x) => for item in x.iter() {
                item.parent = ret.data as *mut SyntaxNode;
            }
            None => ()
        }

        match usings {
            Some(x) => for item in x.iter() {
                item.parent = ret.data as *mut SyntaxNode;
            }
            None => ()
        }

        match defines {
            Some(x) => for item in x.iter() {
                item.parent = ret.data as *mut SyntaxNode;
            }
            None => ()
        }

        match declarations {
            Some(x) => for item in x.iter() {
                match **item {
                    DeclarationSyntax::Namespace(y) => y.parent = ret.data as *mut SyntaxNode,
                    DeclarationSyntax::Function(y) => y.parent = ret.data as *mut SyntaxNode,
                    DeclarationSyntax::Class(y) => y.parent = ret.data as *mut SyntaxNode,
                    DeclarationSyntax::LetDeclaration(y) => y.parent = ret.data as *mut SyntaxNode,
                    DeclarationSyntax::VarDeclaration(y) => y.parent = ret.data as *mut SyntaxNode,
                    DeclarationSyntax::MutableDeclaration(y) => y.parent = ret.data as *mut SyntaxNode,
                    DeclarationSyntax::ThreadLocalDeclaration(y) => y.parent = ret.data as *mut SyntaxNode,
                }
            }
            None => ()
        }

        match statements {
            Some(x) => for item in x.iter() {
                match **item {
                    StatementSyntax::Let(y) => y.parent = ret.data as *mut SyntaxNode,
                    StatementSyntax::Var(y) => y.parent = ret.data as *mut SyntaxNode,
                    StatementSyntax::Mutable(y) => y.parent = ret.data as *mut SyntaxNode,
                    StatementSyntax::ThreadLocal(y) => y.parent = ret.data as *mut SyntaxNode,
                    StatementSyntax::Set(y) => y.parent = ret.data as *mut SyntaxNode,
                    StatementSyntax::Calculation(y) => y.parent = ret.data as *mut SyntaxNode,
                    StatementSyntax::Break(y) => y.parent = ret.data as *mut SyntaxNode,
                    StatementSyntax::Continue(y) => y.parent = ret.data as *mut SyntaxNode,
                    StatementSyntax::Return(y) => y.parent = ret.data as *mut SyntaxNode,
                    StatementSyntax::Throw(y) => y.parent = ret.data as *mut SyntaxNode,
                }
            }
            None => ()
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
            match node {
                None => break,
                Some(node) => {
                    match ret {
                        None => ret = Some(Ref::new(_rp, Array::new())),
                        Some(_) => (),
                    };
                    ret.unwrap().add(node);
                }
            }
        }

        match ret {
            Some(ret) => Ok(Some(Ref::new(_rp, Vector::from_array(_rp, ret)))),
            None => Ok(None),
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

        let successIntrinsic1 = self.lexer.parse_keyword(String::from_string_slice(_r.page, "intrinsic"));
        if successIntrinsic1 {
            self.lexer.advance(&_r)
        } else {
            return Ok(None)
        }

        let name = self.lexer.parse_identifier(_rp);
        match name {
            Some(name) => if self.is_identifier(name) {
                self.lexer.advance(&_r)
            } else {

            return Result::Err(Ref::new(
                _ep,
                ParserError {
                    file_name: self.file_name,
                    line: self.lexer.line,
                    column: self.lexer.column,
                },
            ))
           },
            None => 
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

        let ret = Ref::new(_rp, IntrinsicSyntax { parent: null_mut(), start: start, end: end, name: name.unwrap() });

        Ok(Some(ret))
    }

    pub fn parse_using_list(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Option<Ref<Vector<Ref<UsingSyntax>>>>, Ref<ParserError>> {
        let _r = Region::create(_pr);
        let mut ret: Option<Ref<Array<Ref<UsingSyntax>>>> = Option::None;
        loop {
            let node = self.parse_using(&_r, _rp, _ep)?;
            match node {
                None => break,
                Some(node) => {
                    match ret {
                        None => ret = Some(Ref::new(_rp, Array::new())),
                        Some(_) => (),
                    };
                    ret.unwrap().add(node);
                }
            }
        }

        match ret {
            Some(ret) => Ok(Some(Ref::new(_rp, Vector::from_array(_rp, ret)))),
            None => Ok(None),
        }
    }

    pub fn parse_using(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Option<Ref<UsingSyntax>>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        let start: Position = self.lexer.get_previous_position();

        let successUsing1 = self.lexer.parse_keyword(String::from_string_slice(_r.page, "using"));
        if successUsing1 {
            self.lexer.advance(&_r)
        } else {
            return Ok(None)
        }

        let name = self.parse_name(&_r, _rp, _ep)?;
        match name {
            None =>
                return Err(Ref::new(_ep, ParserError { file_name: String::copy(_ep, self.file_name), line: self.lexer.line, column: self.lexer.column })),
            Some(_) => ()
        }

        let end: Position = self.lexer.get_position();

        let ret = Ref::new(_rp, UsingSyntax { parent: null_mut(), start: start, end: end, name: name.unwrap() });

        name.unwrap().parent = ret.data as *mut SyntaxNode;

        Ok(Some(ret))
    }

    pub fn parse_define_list(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Option<Ref<Vector<Ref<DefineSyntax>>>>, Ref<ParserError>> {
        let _r = Region::create(_pr);
        let mut ret: Option<Ref<Array<Ref<DefineSyntax>>>> = Option::None;
        loop {
            let node = self.parse_define(&_r, _rp, _ep)?;
            match node {
                None => break,
                Some(node) => {
                    match ret {
                        None => ret = Some(Ref::new(_rp, Array::new())),
                        Some(_) => (),
                    };
                    ret.unwrap().add(node);
                }
            }
        }

        match ret {
            Some(ret) => Ok(Some(Ref::new(_rp, Vector::from_array(_rp, ret)))),
            None => Ok(None),
        }
    }

    pub fn parse_define(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Option<Ref<DefineSyntax>>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        let start: Position = self.lexer.get_previous_position();

        let successDefine1 = self.lexer.parse_keyword(String::from_string_slice(_r.page, "define"));
        if successDefine1 {
            self.lexer.advance(&_r)
        } else {
            return Ok(None)
        }

        let name = self.parse_name(&_r, _rp, _ep)?;
        match name {
            None =>
                return Err(Ref::new(_ep, ParserError { file_name: String::copy(_ep, self.file_name), line: self.lexer.line, column: self.lexer.column })),
            Some(_) => ()
        }

        let typeSpec = self.parse_type(&_r, _rp, _ep)?;
        match typeSpec {
            None =>
                return Err(Ref::new(_ep, ParserError { file_name: String::copy(_ep, self.file_name), line: self.lexer.line, column: self.lexer.column })),
            Some(_) => ()
        }

        let end: Position = self.lexer.get_position();

        let ret = Ref::new(_rp, DefineSyntax { parent: null_mut(), start: start, end: end, name: name.unwrap(), typeSpec: typeSpec.unwrap() });

        name.unwrap().parent = ret.data as *mut SyntaxNode;

        typeSpec.unwrap().parent = ret.data as *mut SyntaxNode;

        Ok(Some(ret))
    }

    pub fn parse_name(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Option<Ref<NameSyntax>>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        let start: Position = self.lexer.get_previous_position();

        let name = self.lexer.parse_identifier(_rp);
        match name {
            Some(name) => if self.is_identifier(name) {
                self.lexer.advance(&_r)
            } else {
            return Ok(None)
           },
            None =>             return Ok(None)
        }

        let extensions = self.parse_extension_list(&_r, _rp, _ep)?;

        let end: Position = self.lexer.get_position();

        let ret = Ref::new(_rp, NameSyntax { parent: null_mut(), start: start, end: end, name: name.unwrap(), extensions: extensions });

        match extensions {
            Some(x) => for item in x.iter() {
                item.parent = ret.data as *mut SyntaxNode;
            }
            None => ()
        }

        Ok(Some(ret))
    }

    pub fn parse_extension_list(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Option<Ref<Vector<Ref<ExtensionSyntax>>>>, Ref<ParserError>> {
        let _r = Region::create(_pr);
        let mut ret: Option<Ref<Array<Ref<ExtensionSyntax>>>> = Option::None;
        loop {
            let node = self.parse_extension(&_r, _rp, _ep)?;
            match node {
                None => break,
                Some(node) => {
                    match ret {
                        None => ret = Some(Ref::new(_rp, Array::new())),
                        Some(_) => (),
                    };
                    ret.unwrap().add(node);
                }
            }
        }

        match ret {
            Some(ret) => Ok(Some(Ref::new(_rp, Vector::from_array(_rp, ret)))),
            None => Ok(None),
        }
    }

    pub fn parse_extension(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Option<Ref<ExtensionSyntax>>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        let start: Position = self.lexer.get_previous_position();

        let successDot1 = self.lexer.parse_punctuation(String::from_string_slice(_r.page, "."));
        if successDot1 {
            self.lexer.advance(&_r)
        } else {
            return Ok(None)
        }

        let name = self.lexer.parse_identifier(_rp);
        match name {
            Some(name) => if self.is_identifier(name) {
                self.lexer.advance(&_r)
            } else {

            return Result::Err(Ref::new(
                _ep,
                ParserError {
                    file_name: self.file_name,
                    line: self.lexer.line,
                    column: self.lexer.column,
                },
            ))
           },
            None => 
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

        let ret = Ref::new(_rp, ExtensionSyntax { parent: null_mut(), start: start, end: end, name: name.unwrap() });

        Ok(Some(ret))
    }

    pub fn parse_declaration_list(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Option<Ref<Vector<Ref<DeclarationSyntax>>>>, Ref<ParserError>> {
        let _r = Region::create(_pr);
        let mut ret: Option<Ref<Array<Ref<DeclarationSyntax>>>> = Option::None;
        loop {
            let node = self.parse_declaration(&_r, _rp, _ep)?;
            match node {
                None => break,
                Some(node) => {
                    match ret {
                        None => ret = Some(Ref::new(_rp, Array::new())),
                        Some(_) => (),
                    };
                    ret.unwrap().add(node);
                }
            }
        }

        match ret {
            Some(ret) => Ok(Some(Ref::new(_rp, Vector::from_array(_rp, ret)))),
            None => Ok(None),
        }
    }

    pub fn parse_declaration(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Option<Ref<DeclarationSyntax>>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        {
            let node = self.parse_namespace(&_r, _rp, _ep)?;
            match node {
                Some(it) => return Ok(Some(Ref::new(_rp, DeclarationSyntax::Namespace(*it)))),
                None => ()
            }
        }

        {
            let node = self.parse_function(&_r, _rp, _ep)?;
            match node {
                Some(it) => return Ok(Some(Ref::new(_rp, DeclarationSyntax::Function(*it)))),
                None => ()
            }
        }

        {
            let node = self.parse_class(&_r, _rp, _ep)?;
            match node {
                Some(it) => return Ok(Some(Ref::new(_rp, DeclarationSyntax::Class(*it)))),
                None => ()
            }
        }

        {
            let node = self.parse_letdeclaration(&_r, _rp, _ep)?;
            match node {
                Some(it) => return Ok(Some(Ref::new(_rp, DeclarationSyntax::LetDeclaration(*it)))),
                None => ()
            }
        }

        {
            let node = self.parse_vardeclaration(&_r, _rp, _ep)?;
            match node {
                Some(it) => return Ok(Some(Ref::new(_rp, DeclarationSyntax::VarDeclaration(*it)))),
                None => ()
            }
        }

        {
            let node = self.parse_mutabledeclaration(&_r, _rp, _ep)?;
            match node {
                Some(it) => return Ok(Some(Ref::new(_rp, DeclarationSyntax::MutableDeclaration(*it)))),
                None => ()
            }
        }

        {
            let node = self.parse_threadlocaldeclaration(&_r, _rp, _ep)?;
            match node {
                Some(it) => return Ok(Some(Ref::new(_rp, DeclarationSyntax::ThreadLocalDeclaration(*it)))),
                None => ()
            }
        }

        return Ok(None)
    }

    pub fn parse_namespace(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Option<Ref<NamespaceSyntax>>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        let start: Position = self.lexer.get_previous_position();

        let successNamespace1 = self.lexer.parse_keyword(String::from_string_slice(_r.page, "namespace"));
        if successNamespace1 {
            self.lexer.advance(&_r)
        } else {
            return Ok(None)
        }

        let name = self.parse_name(&_r, _rp, _ep)?;
        match name {
            None =>
                return Err(Ref::new(_ep, ParserError { file_name: String::copy(_ep, self.file_name), line: self.lexer.line, column: self.lexer.column })),
            Some(_) => ()
        }

        let successLeftCurly3 = self.lexer.parse_punctuation(String::from_string_slice(_r.page, "{"));
        if successLeftCurly3 {
            self.lexer.advance(&_r)
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

        let usings = self.parse_using_list(&_r, _rp, _ep)?;

        let defines = self.parse_define_list(&_r, _rp, _ep)?;

        let declarations = self.parse_declaration_list(&_r, _rp, _ep)?;

        let successRightCurly7 = self.lexer.parse_punctuation(String::from_string_slice(_r.page, "}"));
        if successRightCurly7 {
            self.lexer.advance(&_r)
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

        let ret = Ref::new(_rp, NamespaceSyntax { parent: null_mut(), start: start, end: end, name: name.unwrap(), usings: usings, defines: defines, declarations: declarations });

        name.unwrap().parent = ret.data as *mut SyntaxNode;

        match usings {
            Some(x) => for item in x.iter() {
                item.parent = ret.data as *mut SyntaxNode;
            }
            None => ()
        }

        match defines {
            Some(x) => for item in x.iter() {
                item.parent = ret.data as *mut SyntaxNode;
            }
            None => ()
        }

        match declarations {
            Some(x) => for item in x.iter() {
                match **item {
                    DeclarationSyntax::Namespace(y) => y.parent = ret.data as *mut SyntaxNode,
                    DeclarationSyntax::Function(y) => y.parent = ret.data as *mut SyntaxNode,
                    DeclarationSyntax::Class(y) => y.parent = ret.data as *mut SyntaxNode,
                    DeclarationSyntax::LetDeclaration(y) => y.parent = ret.data as *mut SyntaxNode,
                    DeclarationSyntax::VarDeclaration(y) => y.parent = ret.data as *mut SyntaxNode,
                    DeclarationSyntax::MutableDeclaration(y) => y.parent = ret.data as *mut SyntaxNode,
                    DeclarationSyntax::ThreadLocalDeclaration(y) => y.parent = ret.data as *mut SyntaxNode,
                }
            }
            None => ()
        }

        Ok(Some(ret))
    }

    pub fn parse_function(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Option<Ref<FunctionSyntax>>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        let start: Position = self.lexer.get_previous_position();

        let successFunction1 = self.lexer.parse_keyword(String::from_string_slice(_r.page, "function"));
        if successFunction1 {
            self.lexer.advance(&_r)
        } else {
            return Ok(None)
        }

        let procedure = self.parse_procedure(&_r, _rp, _ep)?;
        match procedure {
            None =>
                return Err(Ref::new(_ep, ParserError { file_name: String::copy(_ep, self.file_name), line: self.lexer.line, column: self.lexer.column })),
            Some(_) => ()
        }

        let end: Position = self.lexer.get_position();

        let ret = Ref::new(_rp, FunctionSyntax { parent: null_mut(), start: start, end: end, procedure: procedure.unwrap() });

        procedure.unwrap().parent = ret.data as *mut SyntaxNode;

        Ok(Some(ret))
    }

    pub fn parse_procedure(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Option<Ref<ProcedureSyntax>>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        let start: Position = self.lexer.get_previous_position();

        let name = self.lexer.parse_identifier(_rp);
        match name {
            Some(name) => if self.is_identifier(name) {
                self.lexer.advance(&_r)
            } else {
            return Ok(None)
           },
            None =>             return Ok(None)
        }

        let routine = self.parse_routine(&_r, _rp, _ep)?;
        match routine {
            None =>
                return Err(Ref::new(_ep, ParserError { file_name: String::copy(_ep, self.file_name), line: self.lexer.line, column: self.lexer.column })),
            Some(_) => ()
        }

        let end: Position = self.lexer.get_position();

        let ret = Ref::new(_rp, ProcedureSyntax { parent: null_mut(), start: start, end: end, name: name.unwrap(), routine: routine.unwrap() });

        routine.unwrap().parent = ret.data as *mut SyntaxNode;

        Ok(Some(ret))
    }

    pub fn parse_routine(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Option<Ref<RoutineSyntax>>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        let start: Position = self.lexer.get_previous_position();

        let input = self.parse_structure(&_r, _rp, _ep)?;

        let output = self.parse_typeannotation(&_r, _rp, _ep)?;

        let throwsClause = self.parse_throws(&_r, _rp, _ep)?;

        let body = self.parse_block(&_r, _rp, _ep)?;
        match body {
            None =>
                return Err(Ref::new(_ep, ParserError { file_name: String::copy(_ep, self.file_name), line: self.lexer.line, column: self.lexer.column })),
            Some(_) => ()
        }

        let end: Position = self.lexer.get_position();

        let ret = Ref::new(_rp, RoutineSyntax { parent: null_mut(), start: start, end: end, input: input, output: output, throwsClause: throwsClause, body: body.unwrap() });

        match input {
            Some(x) => 
                x.parent = ret.data as *mut SyntaxNode,
            None => ()
        }

        match output {
            Some(x) => 
                x.parent = ret.data as *mut SyntaxNode,
            None => ()
        }

        match throwsClause {
            Some(x) => 
                x.parent = ret.data as *mut SyntaxNode,
            None => ()
        }

        body.unwrap().parent = ret.data as *mut SyntaxNode;

        Ok(Some(ret))
    }

    pub fn parse_letdeclaration(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Option<Ref<LetDeclarationSyntax>>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        let start: Position = self.lexer.get_previous_position();

        let declaration = self.parse_let(&_r, _rp, _ep)?;
        match declaration {
            None =>
                return Ok(None),
            Some(_) => ()
        }

        let end: Position = self.lexer.get_position();

        let ret = Ref::new(_rp, LetDeclarationSyntax { parent: null_mut(), start: start, end: end, declaration: declaration.unwrap() });

        declaration.unwrap().parent = ret.data as *mut SyntaxNode;

        Ok(Some(ret))
    }

    pub fn parse_vardeclaration(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Option<Ref<VarDeclarationSyntax>>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        let start: Position = self.lexer.get_previous_position();

        let declaration = self.parse_var(&_r, _rp, _ep)?;
        match declaration {
            None =>
                return Ok(None),
            Some(_) => ()
        }

        let end: Position = self.lexer.get_position();

        let ret = Ref::new(_rp, VarDeclarationSyntax { parent: null_mut(), start: start, end: end, declaration: declaration.unwrap() });

        declaration.unwrap().parent = ret.data as *mut SyntaxNode;

        Ok(Some(ret))
    }

    pub fn parse_mutabledeclaration(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Option<Ref<MutableDeclarationSyntax>>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        let start: Position = self.lexer.get_previous_position();

        let declaration = self.parse_mutable(&_r, _rp, _ep)?;
        match declaration {
            None =>
                return Ok(None),
            Some(_) => ()
        }

        let end: Position = self.lexer.get_position();

        let ret = Ref::new(_rp, MutableDeclarationSyntax { parent: null_mut(), start: start, end: end, declaration: declaration.unwrap() });

        declaration.unwrap().parent = ret.data as *mut SyntaxNode;

        Ok(Some(ret))
    }

    pub fn parse_threadlocaldeclaration(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Option<Ref<ThreadLocalDeclarationSyntax>>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        let start: Position = self.lexer.get_previous_position();

        let declaration = self.parse_threadlocal(&_r, _rp, _ep)?;
        match declaration {
            None =>
                return Ok(None),
            Some(_) => ()
        }

        let end: Position = self.lexer.get_position();

        let ret = Ref::new(_rp, ThreadLocalDeclarationSyntax { parent: null_mut(), start: start, end: end, declaration: declaration.unwrap() });

        declaration.unwrap().parent = ret.data as *mut SyntaxNode;

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
            match node {
                None => break,
                Some(node) => {
                    match ret {
                        None => ret = Some(Ref::new(_rp, Array::new())),
                        Some(_) => (),
                    };
                    ret.unwrap().add(node);
                }
            }
        }

        match ret {
            Some(ret) => Ok(Some(Ref::new(_rp, Vector::from_array(_rp, ret)))),
            None => Ok(None),
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
            let node = self.parse_let(&_r, _rp, _ep)?;
            match node {
                Some(it) => return Ok(Some(Ref::new(_rp, StatementSyntax::Let(*it)))),
                None => ()
            }
        }

        {
            let node = self.parse_var(&_r, _rp, _ep)?;
            match node {
                Some(it) => return Ok(Some(Ref::new(_rp, StatementSyntax::Var(*it)))),
                None => ()
            }
        }

        {
            let node = self.parse_mutable(&_r, _rp, _ep)?;
            match node {
                Some(it) => return Ok(Some(Ref::new(_rp, StatementSyntax::Mutable(*it)))),
                None => ()
            }
        }

        {
            let node = self.parse_threadlocal(&_r, _rp, _ep)?;
            match node {
                Some(it) => return Ok(Some(Ref::new(_rp, StatementSyntax::ThreadLocal(*it)))),
                None => ()
            }
        }

        {
            let node = self.parse_set(&_r, _rp, _ep)?;
            match node {
                Some(it) => return Ok(Some(Ref::new(_rp, StatementSyntax::Set(*it)))),
                None => ()
            }
        }

        {
            let node = self.parse_calculation(&_r, _rp, _ep)?;
            match node {
                Some(it) => return Ok(Some(Ref::new(_rp, StatementSyntax::Calculation(*it)))),
                None => ()
            }
        }

        {
            let node = self.parse_break(&_r, _rp, _ep)?;
            match node {
                Some(it) => return Ok(Some(Ref::new(_rp, StatementSyntax::Break(*it)))),
                None => ()
            }
        }

        {
            let node = self.parse_continue(&_r, _rp, _ep)?;
            match node {
                Some(it) => return Ok(Some(Ref::new(_rp, StatementSyntax::Continue(*it)))),
                None => ()
            }
        }

        {
            let node = self.parse_return(&_r, _rp, _ep)?;
            match node {
                Some(it) => return Ok(Some(Ref::new(_rp, StatementSyntax::Return(*it)))),
                None => ()
            }
        }

        {
            let node = self.parse_throw(&_r, _rp, _ep)?;
            match node {
                Some(it) => return Ok(Some(Ref::new(_rp, StatementSyntax::Throw(*it)))),
                None => ()
            }
        }

        return Ok(None)
    }

    pub fn parse_let(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Option<Ref<LetSyntax>>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        let start: Position = self.lexer.get_previous_position();

        let successLet1 = self.lexer.parse_keyword(String::from_string_slice(_r.page, "let"));
        if successLet1 {
            self.lexer.advance(&_r)
        } else {
            return Ok(None)
        }

        let binding = self.parse_binding(&_r, _rp, _ep)?;
        match binding {
            None =>
                return Err(Ref::new(_ep, ParserError { file_name: String::copy(_ep, self.file_name), line: self.lexer.line, column: self.lexer.column })),
            Some(_) => ()
        }

        let end: Position = self.lexer.get_position();

        let ret = Ref::new(_rp, LetSyntax { parent: null_mut(), start: start, end: end, binding: binding.unwrap() });

        binding.unwrap().parent = ret.data as *mut SyntaxNode;

        Ok(Some(ret))
    }

    pub fn parse_var(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Option<Ref<VarSyntax>>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        let start: Position = self.lexer.get_previous_position();

        let successVar1 = self.lexer.parse_keyword(String::from_string_slice(_r.page, "var"));
        if successVar1 {
            self.lexer.advance(&_r)
        } else {
            return Ok(None)
        }

        let binding = self.parse_binding(&_r, _rp, _ep)?;
        match binding {
            None =>
                return Err(Ref::new(_ep, ParserError { file_name: String::copy(_ep, self.file_name), line: self.lexer.line, column: self.lexer.column })),
            Some(_) => ()
        }

        let end: Position = self.lexer.get_position();

        let ret = Ref::new(_rp, VarSyntax { parent: null_mut(), start: start, end: end, binding: binding.unwrap() });

        binding.unwrap().parent = ret.data as *mut SyntaxNode;

        Ok(Some(ret))
    }

    pub fn parse_mutable(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Option<Ref<MutableSyntax>>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        let start: Position = self.lexer.get_previous_position();

        let successMutable1 = self.lexer.parse_keyword(String::from_string_slice(_r.page, "mutable"));
        if successMutable1 {
            self.lexer.advance(&_r)
        } else {
            return Ok(None)
        }

        let binding = self.parse_binding(&_r, _rp, _ep)?;
        match binding {
            None =>
                return Err(Ref::new(_ep, ParserError { file_name: String::copy(_ep, self.file_name), line: self.lexer.line, column: self.lexer.column })),
            Some(_) => ()
        }

        let end: Position = self.lexer.get_position();

        let ret = Ref::new(_rp, MutableSyntax { parent: null_mut(), start: start, end: end, binding: binding.unwrap() });

        binding.unwrap().parent = ret.data as *mut SyntaxNode;

        Ok(Some(ret))
    }

    pub fn parse_threadlocal(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Option<Ref<ThreadLocalSyntax>>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        let start: Position = self.lexer.get_previous_position();

        let successThreadlocal1 = self.lexer.parse_keyword(String::from_string_slice(_r.page, "threadlocal"));
        if successThreadlocal1 {
            self.lexer.advance(&_r)
        } else {
            return Ok(None)
        }

        let binding = self.parse_binding(&_r, _rp, _ep)?;
        match binding {
            None =>
                return Err(Ref::new(_ep, ParserError { file_name: String::copy(_ep, self.file_name), line: self.lexer.line, column: self.lexer.column })),
            Some(_) => ()
        }

        let end: Position = self.lexer.get_position();

        let ret = Ref::new(_rp, ThreadLocalSyntax { parent: null_mut(), start: start, end: end, binding: binding.unwrap() });

        binding.unwrap().parent = ret.data as *mut SyntaxNode;

        Ok(Some(ret))
    }

    pub fn parse_binding(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Option<Ref<BindingSyntax>>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        let start: Position = self.lexer.get_previous_position();

        let name = self.lexer.parse_identifier(_rp);
        match name {
            Some(name) => if self.is_identifier(name) {
                self.lexer.advance(&_r)
            } else {
            return Ok(None)
           },
            None =>             return Ok(None)
        }

        let typeAnnotation = self.parse_typeannotation(&_r, _rp, _ep)?;

        let calculation = self.parse_calculation(&_r, _rp, _ep)?;
        match calculation {
            None =>
                return Err(Ref::new(_ep, ParserError { file_name: String::copy(_ep, self.file_name), line: self.lexer.line, column: self.lexer.column })),
            Some(_) => ()
        }

        let end: Position = self.lexer.get_position();

        let ret = Ref::new(_rp, BindingSyntax { parent: null_mut(), start: start, end: end, name: name.unwrap(), typeAnnotation: typeAnnotation, calculation: calculation.unwrap() });

        match typeAnnotation {
            Some(x) => 
                x.parent = ret.data as *mut SyntaxNode,
            None => ()
        }

        calculation.unwrap().parent = ret.data as *mut SyntaxNode;

        Ok(Some(ret))
    }

    pub fn parse_set(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Option<Ref<SetSyntax>>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        let start: Position = self.lexer.get_previous_position();

        let successSet1 = self.lexer.parse_keyword(String::from_string_slice(_r.page, "set"));
        if successSet1 {
            self.lexer.advance(&_r)
        } else {
            return Ok(None)
        }

        let lValue = self.parse_operation(&_r, _rp, _ep)?;
        match lValue {
            None =>
                return Err(Ref::new(_ep, ParserError { file_name: String::copy(_ep, self.file_name), line: self.lexer.line, column: self.lexer.column })),
            Some(_) => ()
        }

        let successColon3 = self.lexer.parse_punctuation(String::from_string_slice(_r.page, ":"));
        if successColon3 {
            self.lexer.advance(&_r)
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

        let rValue = self.parse_calculation(&_r, _rp, _ep)?;
        match rValue {
            None =>
                return Err(Ref::new(_ep, ParserError { file_name: String::copy(_ep, self.file_name), line: self.lexer.line, column: self.lexer.column })),
            Some(_) => ()
        }

        let end: Position = self.lexer.get_position();

        let ret = Ref::new(_rp, SetSyntax { parent: null_mut(), start: start, end: end, lValue: lValue.unwrap(), rValue: rValue.unwrap() });

        lValue.unwrap().parent = ret.data as *mut SyntaxNode;

        rValue.unwrap().parent = ret.data as *mut SyntaxNode;

        Ok(Some(ret))
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
        match operation {
            None =>
                return Ok(None),
            Some(_) => ()
        }

        let successSemicolon2 = self.lexer.parse_punctuation(String::from_string_slice(_r.page, ";"));
        if successSemicolon2 {
            self.lexer.advance(&_r)
        } else {
()
        }

        let end: Position = self.lexer.get_position();

        let ret = Ref::new(_rp, CalculationSyntax { parent: null_mut(), start: start, end: end, operation: operation.unwrap() });

        operation.unwrap().parent = ret.data as *mut SyntaxNode;

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
        match op {
            None =>
                return Ok(None),
            Some(_) => ()
        }

        let end: Position = self.lexer.get_position();

        let ret = Ref::new(_rp, OperationSyntax { parent: null_mut(), start: start, end: end, op: op.unwrap() });

        match op {
            Some(x) => for item in x.iter() {
                item.parent = ret.data as *mut SyntaxNode;
            }
            None => ()
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
            match node {
                None => break,
                Some(node) => {
                    match ret {
                        None => ret = Some(Ref::new(_rp, Array::new())),
                        Some(_) => (),
                    };
                    ret.unwrap().add(node);
                }
            }
        }

        match ret {
            Some(ret) => Ok(Some(Ref::new(_rp, Vector::from_array(_rp, ret)))),
            None => Ok(None),
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
        match primary {
            None =>
                return Ok(None),
            Some(_) => ()
        }

        let postfixes = self.parse_postfix_list(&_r, _rp, _ep)?;

        let end: Position = self.lexer.get_position();

        let ret = Ref::new(_rp, OperandSyntax { parent: null_mut(), start: start, end: end, primary: primary.unwrap(), postfixes: postfixes });

        match *primary.unwrap() {
            ExpressionSyntax::Block(x) => x.parent = ret.data as *mut SyntaxNode,
            ExpressionSyntax::Name(x) => x.parent = ret.data as *mut SyntaxNode,
            ExpressionSyntax::Constant(x) => x.parent = ret.data as *mut SyntaxNode,
            ExpressionSyntax::If(x) => x.parent = ret.data as *mut SyntaxNode,
            ExpressionSyntax::Switch(x) => x.parent = ret.data as *mut SyntaxNode,
            ExpressionSyntax::For(x) => x.parent = ret.data as *mut SyntaxNode,
            ExpressionSyntax::While(x) => x.parent = ret.data as *mut SyntaxNode,
            ExpressionSyntax::Do(x) => x.parent = ret.data as *mut SyntaxNode,
            ExpressionSyntax::This(x) => x.parent = ret.data as *mut SyntaxNode,
            ExpressionSyntax::New(x) => x.parent = ret.data as *mut SyntaxNode,
            ExpressionSyntax::Object(x) => x.parent = ret.data as *mut SyntaxNode,
            ExpressionSyntax::Array(x) => x.parent = ret.data as *mut SyntaxNode,
            ExpressionSyntax::SizeOf(x) => x.parent = ret.data as *mut SyntaxNode,
        }

        match postfixes {
            Some(x) => for item in x.iter() {
                match **item {
                    PostfixSyntax::MemberAccess(y) => y.parent = ret.data as *mut SyntaxNode,
                    PostfixSyntax::As(y) => y.parent = ret.data as *mut SyntaxNode,
                    PostfixSyntax::Is(y) => y.parent = ret.data as *mut SyntaxNode,
                    PostfixSyntax::Unwrap(y) => y.parent = ret.data as *mut SyntaxNode,
                    PostfixSyntax::Catch(y) => y.parent = ret.data as *mut SyntaxNode,
                }
            }
            None => ()
        }

        Ok(Some(ret))
    }

    pub fn parse_postfix_list(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Option<Ref<Vector<Ref<PostfixSyntax>>>>, Ref<ParserError>> {
        let _r = Region::create(_pr);
        let mut ret: Option<Ref<Array<Ref<PostfixSyntax>>>> = Option::None;
        loop {
            let node = self.parse_postfix(&_r, _rp, _ep)?;
            match node {
                None => break,
                Some(node) => {
                    match ret {
                        None => ret = Some(Ref::new(_rp, Array::new())),
                        Some(_) => (),
                    };
                    ret.unwrap().add(node);
                }
            }
        }

        match ret {
            Some(ret) => Ok(Some(Ref::new(_rp, Vector::from_array(_rp, ret)))),
            None => Ok(None),
        }
    }

    pub fn parse_postfix(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Option<Ref<PostfixSyntax>>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        {
            let node = self.parse_memberaccess(&_r, _rp, _ep)?;
            match node {
                Some(it) => return Ok(Some(Ref::new(_rp, PostfixSyntax::MemberAccess(*it)))),
                None => ()
            }
        }

        {
            let node = self.parse_as(&_r, _rp, _ep)?;
            match node {
                Some(it) => return Ok(Some(Ref::new(_rp, PostfixSyntax::As(*it)))),
                None => ()
            }
        }

        {
            let node = self.parse_is(&_r, _rp, _ep)?;
            match node {
                Some(it) => return Ok(Some(Ref::new(_rp, PostfixSyntax::Is(*it)))),
                None => ()
            }
        }

        {
            let node = self.parse_unwrap(&_r, _rp, _ep)?;
            match node {
                Some(it) => return Ok(Some(Ref::new(_rp, PostfixSyntax::Unwrap(*it)))),
                None => ()
            }
        }

        {
            let node = self.parse_catch(&_r, _rp, _ep)?;
            match node {
                Some(it) => return Ok(Some(Ref::new(_rp, PostfixSyntax::Catch(*it)))),
                None => ()
            }
        }

        return Ok(None)
    }

    pub fn parse_memberaccess(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Option<Ref<MemberAccessSyntax>>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        let start: Position = self.lexer.get_previous_position();

        let successDot1 = self.lexer.parse_punctuation(String::from_string_slice(_r.page, "."));
        if successDot1 {
            self.lexer.advance(&_r)
        } else {
            return Ok(None)
        }

        let member = self.lexer.parse_identifier(_rp);
        match member {
            Some(member) => if self.is_identifier(member) {
                self.lexer.advance(&_r)
            } else {

            return Result::Err(Ref::new(
                _ep,
                ParserError {
                    file_name: self.file_name,
                    line: self.lexer.line,
                    column: self.lexer.column,
                },
            ))
           },
            None => 
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

        let ret = Ref::new(_rp, MemberAccessSyntax { parent: null_mut(), start: start, end: end, member: member.unwrap() });

        Ok(Some(ret))
    }

    pub fn parse_as(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Option<Ref<AsSyntax>>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        let start: Position = self.lexer.get_previous_position();

        let successAs1 = self.lexer.parse_keyword(String::from_string_slice(_r.page, "as"));
        if successAs1 {
            self.lexer.advance(&_r)
        } else {
            return Ok(None)
        }

        let typeSpec = self.parse_typespec(&_r, _rp, _ep)?;
        match typeSpec {
            None =>
                return Err(Ref::new(_ep, ParserError { file_name: String::copy(_ep, self.file_name), line: self.lexer.line, column: self.lexer.column })),
            Some(_) => ()
        }

        let end: Position = self.lexer.get_position();

        let ret = Ref::new(_rp, AsSyntax { parent: null_mut(), start: start, end: end, typeSpec: typeSpec.unwrap() });

        match *typeSpec.unwrap() {
            TypeSpecSyntax::Type(x) => x.parent = ret.data as *mut SyntaxNode,
            TypeSpecSyntax::Variant(x) => x.parent = ret.data as *mut SyntaxNode,
        }

        Ok(Some(ret))
    }

    pub fn parse_is(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Option<Ref<IsSyntax>>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        let start: Position = self.lexer.get_previous_position();

        let successIs1 = self.lexer.parse_keyword(String::from_string_slice(_r.page, "is"));
        if successIs1 {
            self.lexer.advance(&_r)
        } else {
            return Ok(None)
        }

        let typeSpec = self.parse_typespec(&_r, _rp, _ep)?;
        match typeSpec {
            None =>
                return Err(Ref::new(_ep, ParserError { file_name: String::copy(_ep, self.file_name), line: self.lexer.line, column: self.lexer.column })),
            Some(_) => ()
        }

        let end: Position = self.lexer.get_position();

        let ret = Ref::new(_rp, IsSyntax { parent: null_mut(), start: start, end: end, typeSpec: typeSpec.unwrap() });

        match *typeSpec.unwrap() {
            TypeSpecSyntax::Type(x) => x.parent = ret.data as *mut SyntaxNode,
            TypeSpecSyntax::Variant(x) => x.parent = ret.data as *mut SyntaxNode,
        }

        Ok(Some(ret))
    }

    pub fn parse_unwrap(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Option<Ref<UnwrapSyntax>>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        let start: Position = self.lexer.get_previous_position();

        let successExclamation1 = self.lexer.parse_punctuation(String::from_string_slice(_r.page, "!"));
        if successExclamation1 {
            self.lexer.advance(&_r)
        } else {
            return Ok(None)
        }

        let end: Position = self.lexer.get_position();

        let ret = Ref::new(_rp, UnwrapSyntax { parent: null_mut(), start: start, end: end });

        Ok(Some(ret))
    }

    pub fn parse_catch(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Option<Ref<CatchSyntax>>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        let start: Position = self.lexer.get_previous_position();

        let successCatch1 = self.lexer.parse_keyword(String::from_string_slice(_r.page, "catch"));
        if successCatch1 {
            self.lexer.advance(&_r)
        } else {
            return Ok(None)
        }

        let typeSpec = self.parse_catchpattern(&_r, _rp, _ep)?;
        match typeSpec {
            None =>
                return Err(Ref::new(_ep, ParserError { file_name: String::copy(_ep, self.file_name), line: self.lexer.line, column: self.lexer.column })),
            Some(_) => ()
        }

        let handler = self.parse_block(&_r, _rp, _ep)?;
        match handler {
            None =>
                return Err(Ref::new(_ep, ParserError { file_name: String::copy(_ep, self.file_name), line: self.lexer.line, column: self.lexer.column })),
            Some(_) => ()
        }

        let end: Position = self.lexer.get_position();

        let ret = Ref::new(_rp, CatchSyntax { parent: null_mut(), start: start, end: end, typeSpec: typeSpec.unwrap(), handler: handler.unwrap() });

        match *typeSpec.unwrap() {
            CatchPatternSyntax::WildCardCatchPattern(x) => x.parent = ret.data as *mut SyntaxNode,
            CatchPatternSyntax::TypeCatchPattern(x) => x.parent = ret.data as *mut SyntaxNode,
        }

        handler.unwrap().parent = ret.data as *mut SyntaxNode;

        Ok(Some(ret))
    }

    pub fn parse_catchpattern(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Option<Ref<CatchPatternSyntax>>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        {
            let node = self.parse_wildcardcatchpattern(&_r, _rp, _ep)?;
            match node {
                Some(it) => return Ok(Some(Ref::new(_rp, CatchPatternSyntax::WildCardCatchPattern(*it)))),
                None => ()
            }
        }

        {
            let node = self.parse_typecatchpattern(&_r, _rp, _ep)?;
            match node {
                Some(it) => return Ok(Some(Ref::new(_rp, CatchPatternSyntax::TypeCatchPattern(*it)))),
                None => ()
            }
        }

        return Ok(None)
    }

    pub fn parse_wildcardcatchpattern(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Option<Ref<WildCardCatchPatternSyntax>>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        let start: Position = self.lexer.get_previous_position();

        let pattern = self.parse_wildcardpattern(&_r, _rp, _ep)?;
        match pattern {
            None =>
                return Ok(None),
            Some(_) => ()
        }

        let end: Position = self.lexer.get_position();

        let ret = Ref::new(_rp, WildCardCatchPatternSyntax { parent: null_mut(), start: start, end: end, pattern: pattern.unwrap() });

        pattern.unwrap().parent = ret.data as *mut SyntaxNode;

        Ok(Some(ret))
    }

    pub fn parse_typecatchpattern(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Option<Ref<TypeCatchPatternSyntax>>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        let start: Position = self.lexer.get_previous_position();

        let typeSpec = self.parse_typespec(&_r, _rp, _ep)?;
        match typeSpec {
            None =>
                return Ok(None),
            Some(_) => ()
        }

        let errorName = self.lexer.parse_identifier(_rp);
        match errorName {
            Some(errorName) => if self.is_identifier(errorName) {
                self.lexer.advance(&_r)
            } else {
()
           },
            None => ()
        }

        let end: Position = self.lexer.get_position();

        let ret = Ref::new(_rp, TypeCatchPatternSyntax { parent: null_mut(), start: start, end: end, typeSpec: typeSpec.unwrap(), errorName: errorName });

        match *typeSpec.unwrap() {
            TypeSpecSyntax::Type(x) => x.parent = ret.data as *mut SyntaxNode,
            TypeSpecSyntax::Variant(x) => x.parent = ret.data as *mut SyntaxNode,
        }

        Ok(Some(ret))
    }

    pub fn parse_expression_list(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Option<Ref<Vector<Ref<ExpressionSyntax>>>>, Ref<ParserError>> {
        let _r = Region::create(_pr);
        let mut ret: Option<Ref<Array<Ref<ExpressionSyntax>>>> = Option::None;
        loop {
            let node = self.parse_expression(&_r, _rp, _ep)?;
            match node {
                None => break,
                Some(node) => {
                    match ret {
                        None => ret = Some(Ref::new(_rp, Array::new())),
                        Some(_) => (),
                    };
                    ret.unwrap().add(node);
                }
            }
        }

        match ret {
            Some(ret) => Ok(Some(Ref::new(_rp, Vector::from_array(_rp, ret)))),
            None => Ok(None),
        }
    }

    pub fn parse_expression(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Option<Ref<ExpressionSyntax>>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        {
            let node = self.parse_block(&_r, _rp, _ep)?;
            match node {
                Some(it) => return Ok(Some(Ref::new(_rp, ExpressionSyntax::Block(*it)))),
                None => ()
            }
        }

        {
            let node = self.parse_name(&_r, _rp, _ep)?;
            match node {
                Some(it) => return Ok(Some(Ref::new(_rp, ExpressionSyntax::Name(*it)))),
                None => ()
            }
        }

        {
            let node = self.parse_constant(&_r, _rp, _ep)?;
            match node {
                Some(it) => return Ok(Some(Ref::new(_rp, ExpressionSyntax::Constant(*it)))),
                None => ()
            }
        }

        {
            let node = self.parse_if(&_r, _rp, _ep)?;
            match node {
                Some(it) => return Ok(Some(Ref::new(_rp, ExpressionSyntax::If(*it)))),
                None => ()
            }
        }

        {
            let node = self.parse_switch(&_r, _rp, _ep)?;
            match node {
                Some(it) => return Ok(Some(Ref::new(_rp, ExpressionSyntax::Switch(*it)))),
                None => ()
            }
        }

        {
            let node = self.parse_for(&_r, _rp, _ep)?;
            match node {
                Some(it) => return Ok(Some(Ref::new(_rp, ExpressionSyntax::For(*it)))),
                None => ()
            }
        }

        {
            let node = self.parse_while(&_r, _rp, _ep)?;
            match node {
                Some(it) => return Ok(Some(Ref::new(_rp, ExpressionSyntax::While(*it)))),
                None => ()
            }
        }

        {
            let node = self.parse_do(&_r, _rp, _ep)?;
            match node {
                Some(it) => return Ok(Some(Ref::new(_rp, ExpressionSyntax::Do(*it)))),
                None => ()
            }
        }

        {
            let node = self.parse_this(&_r, _rp, _ep)?;
            match node {
                Some(it) => return Ok(Some(Ref::new(_rp, ExpressionSyntax::This(*it)))),
                None => ()
            }
        }

        {
            let node = self.parse_new(&_r, _rp, _ep)?;
            match node {
                Some(it) => return Ok(Some(Ref::new(_rp, ExpressionSyntax::New(*it)))),
                None => ()
            }
        }

        {
            let node = self.parse_object(&_r, _rp, _ep)?;
            match node {
                Some(it) => return Ok(Some(Ref::new(_rp, ExpressionSyntax::Object(*it)))),
                None => ()
            }
        }

        {
            let node = self.parse_array(&_r, _rp, _ep)?;
            match node {
                Some(it) => return Ok(Some(Ref::new(_rp, ExpressionSyntax::Array(*it)))),
                None => ()
            }
        }

        {
            let node = self.parse_sizeof(&_r, _rp, _ep)?;
            match node {
                Some(it) => return Ok(Some(Ref::new(_rp, ExpressionSyntax::SizeOf(*it)))),
                None => ()
            }
        }

        return Ok(None)
    }

    pub fn parse_block(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Option<Ref<BlockSyntax>>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        let start: Position = self.lexer.get_previous_position();

        let successLeftCurly1 = self.lexer.parse_punctuation(String::from_string_slice(_r.page, "{"));
        if successLeftCurly1 {
            self.lexer.advance(&_r)
        } else {
            return Ok(None)
        }

        let statements = self.parse_statement_list(&_r, _rp, _ep)?;

        let successRightCurly3 = self.lexer.parse_punctuation(String::from_string_slice(_r.page, "}"));
        if successRightCurly3 {
            self.lexer.advance(&_r)
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

        let ret = Ref::new(_rp, BlockSyntax { parent: null_mut(), start: start, end: end, statements: statements });

        match statements {
            Some(x) => for item in x.iter() {
                match **item {
                    StatementSyntax::Let(y) => y.parent = ret.data as *mut SyntaxNode,
                    StatementSyntax::Var(y) => y.parent = ret.data as *mut SyntaxNode,
                    StatementSyntax::Mutable(y) => y.parent = ret.data as *mut SyntaxNode,
                    StatementSyntax::ThreadLocal(y) => y.parent = ret.data as *mut SyntaxNode,
                    StatementSyntax::Set(y) => y.parent = ret.data as *mut SyntaxNode,
                    StatementSyntax::Calculation(y) => y.parent = ret.data as *mut SyntaxNode,
                    StatementSyntax::Break(y) => y.parent = ret.data as *mut SyntaxNode,
                    StatementSyntax::Continue(y) => y.parent = ret.data as *mut SyntaxNode,
                    StatementSyntax::Return(y) => y.parent = ret.data as *mut SyntaxNode,
                    StatementSyntax::Throw(y) => y.parent = ret.data as *mut SyntaxNode,
                }
            }
            None => ()
        }

        Ok(Some(ret))
    }

    pub fn parse_constant(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Option<Ref<ConstantSyntax>>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        let start: Position = self.lexer.get_previous_position();

        let literal = self.lexer.parse_literal(_rp);
        match literal {
            Some(x) =>
                self.lexer.advance(&_r),
            None =>             return Ok(None)
        }

        let end: Position = self.lexer.get_position();

        let ret = Ref::new(_rp, ConstantSyntax { parent: null_mut(), start: start, end: end, literal: literal.unwrap() });

        Ok(Some(ret))
    }

    pub fn parse_if(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Option<Ref<IfSyntax>>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        let start: Position = self.lexer.get_previous_position();

        let successIf1 = self.lexer.parse_keyword(String::from_string_slice(_r.page, "if"));
        if successIf1 {
            self.lexer.advance(&_r)
        } else {
            return Ok(None)
        }

        let successLeftParen2 = self.lexer.parse_punctuation(String::from_string_slice(_r.page, "("));
        if successLeftParen2 {
            self.lexer.advance(&_r)
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

        let condition = self.parse_operation(&_r, _rp, _ep)?;
        match condition {
            None =>
                return Err(Ref::new(_ep, ParserError { file_name: String::copy(_ep, self.file_name), line: self.lexer.line, column: self.lexer.column })),
            Some(_) => ()
        }

        let successRightParen4 = self.lexer.parse_punctuation(String::from_string_slice(_r.page, ")"));
        if successRightParen4 {
            self.lexer.advance(&_r)
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

        let consequent = self.parse_block(&_r, _rp, _ep)?;
        match consequent {
            None =>
                return Err(Ref::new(_ep, ParserError { file_name: String::copy(_ep, self.file_name), line: self.lexer.line, column: self.lexer.column })),
            Some(_) => ()
        }

        let elseClause = self.parse_else(&_r, _rp, _ep)?;

        let end: Position = self.lexer.get_position();

        let ret = Ref::new(_rp, IfSyntax { parent: null_mut(), start: start, end: end, condition: condition.unwrap(), consequent: consequent.unwrap(), elseClause: elseClause });

        condition.unwrap().parent = ret.data as *mut SyntaxNode;

        consequent.unwrap().parent = ret.data as *mut SyntaxNode;

        match elseClause {
            Some(x) => 
                x.parent = ret.data as *mut SyntaxNode,
            None => ()
        }

        Ok(Some(ret))
    }

    pub fn parse_else(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Option<Ref<ElseSyntax>>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        let start: Position = self.lexer.get_previous_position();

        let successElse1 = self.lexer.parse_keyword(String::from_string_slice(_r.page, "else"));
        if successElse1 {
            self.lexer.advance(&_r)
        } else {
            return Ok(None)
        }

        let alternative = self.parse_block(&_r, _rp, _ep)?;
        match alternative {
            None =>
                return Err(Ref::new(_ep, ParserError { file_name: String::copy(_ep, self.file_name), line: self.lexer.line, column: self.lexer.column })),
            Some(_) => ()
        }

        let end: Position = self.lexer.get_position();

        let ret = Ref::new(_rp, ElseSyntax { parent: null_mut(), start: start, end: end, alternative: alternative.unwrap() });

        alternative.unwrap().parent = ret.data as *mut SyntaxNode;

        Ok(Some(ret))
    }

    pub fn parse_switch(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Option<Ref<SwitchSyntax>>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        let start: Position = self.lexer.get_previous_position();

        let successSwitch1 = self.lexer.parse_keyword(String::from_string_slice(_r.page, "switch"));
        if successSwitch1 {
            self.lexer.advance(&_r)
        } else {
            return Ok(None)
        }

        let successLeftParen2 = self.lexer.parse_punctuation(String::from_string_slice(_r.page, "("));
        if successLeftParen2 {
            self.lexer.advance(&_r)
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

        let condition = self.parse_operation(&_r, _rp, _ep)?;
        match condition {
            None =>
                return Err(Ref::new(_ep, ParserError { file_name: String::copy(_ep, self.file_name), line: self.lexer.line, column: self.lexer.column })),
            Some(_) => ()
        }

        let successRightParen4 = self.lexer.parse_punctuation(String::from_string_slice(_r.page, ")"));
        if successRightParen4 {
            self.lexer.advance(&_r)
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

        let successLeftCurly5 = self.lexer.parse_punctuation(String::from_string_slice(_r.page, "{"));
        if successLeftCurly5 {
            self.lexer.advance(&_r)
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

        let cases = self.parse_switchcase_list(&_r, _rp, _ep)?;
        match cases {
            None =>
                return Err(Ref::new(_ep, ParserError { file_name: String::copy(_ep, self.file_name), line: self.lexer.line, column: self.lexer.column })),
            Some(_) => ()
        }

        let successRightCurly7 = self.lexer.parse_punctuation(String::from_string_slice(_r.page, "}"));
        if successRightCurly7 {
            self.lexer.advance(&_r)
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

        let ret = Ref::new(_rp, SwitchSyntax { parent: null_mut(), start: start, end: end, condition: condition.unwrap(), cases: cases.unwrap() });

        condition.unwrap().parent = ret.data as *mut SyntaxNode;

        match cases {
            Some(x) => for item in x.iter() {
                item.parent = ret.data as *mut SyntaxNode;
            }
            None => ()
        }

        Ok(Some(ret))
    }

    pub fn parse_switchcase_list(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Option<Ref<Vector<Ref<SwitchCaseSyntax>>>>, Ref<ParserError>> {
        let _r = Region::create(_pr);
        let mut ret: Option<Ref<Array<Ref<SwitchCaseSyntax>>>> = Option::None;
        loop {
            let node = self.parse_switchcase(&_r, _rp, _ep)?;
            match node {
                None => break,
                Some(node) => {
                    match ret {
                        None => ret = Some(Ref::new(_rp, Array::new())),
                        Some(_) => (),
                    };
                    ret.unwrap().add(node);
                }
            }
        }

        match ret {
            Some(ret) => Ok(Some(Ref::new(_rp, Vector::from_array(_rp, ret)))),
            None => Ok(None),
        }
    }

    pub fn parse_switchcase(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Option<Ref<SwitchCaseSyntax>>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        let start: Position = self.lexer.get_previous_position();

        let label = self.parse_caselabel(&_r, _rp, _ep)?;
        match label {
            None =>
                return Ok(None),
            Some(_) => ()
        }

        let content = self.parse_block(&_r, _rp, _ep)?;
        match content {
            None =>
                return Err(Ref::new(_ep, ParserError { file_name: String::copy(_ep, self.file_name), line: self.lexer.line, column: self.lexer.column })),
            Some(_) => ()
        }

        let end: Position = self.lexer.get_position();

        let ret = Ref::new(_rp, SwitchCaseSyntax { parent: null_mut(), start: start, end: end, label: label.unwrap(), content: content.unwrap() });

        match *label.unwrap() {
            CaseLabelSyntax::ItemCaseLabel(x) => x.parent = ret.data as *mut SyntaxNode,
            CaseLabelSyntax::DefaultCaseLabel(x) => x.parent = ret.data as *mut SyntaxNode,
        }

        content.unwrap().parent = ret.data as *mut SyntaxNode;

        Ok(Some(ret))
    }

    pub fn parse_caselabel(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Option<Ref<CaseLabelSyntax>>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        {
            let node = self.parse_itemcaselabel(&_r, _rp, _ep)?;
            match node {
                Some(it) => return Ok(Some(Ref::new(_rp, CaseLabelSyntax::ItemCaseLabel(*it)))),
                None => ()
            }
        }

        {
            let node = self.parse_defaultcaselabel(&_r, _rp, _ep)?;
            match node {
                Some(it) => return Ok(Some(Ref::new(_rp, CaseLabelSyntax::DefaultCaseLabel(*it)))),
                None => ()
            }
        }

        return Ok(None)
    }

    pub fn parse_itemcaselabel(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Option<Ref<ItemCaseLabelSyntax>>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        let start: Position = self.lexer.get_previous_position();

        let successCase1 = self.lexer.parse_keyword(String::from_string_slice(_r.page, "case"));
        if successCase1 {
            self.lexer.advance(&_r)
        } else {
            return Ok(None)
        }

        let items = self.parse_caseitem_list(&_r, _rp, _ep)?;
        match items {
            None =>
                return Err(Ref::new(_ep, ParserError { file_name: String::copy(_ep, self.file_name), line: self.lexer.line, column: self.lexer.column })),
            Some(_) => ()
        }

        let end: Position = self.lexer.get_position();

        let ret = Ref::new(_rp, ItemCaseLabelSyntax { parent: null_mut(), start: start, end: end, items: items.unwrap() });

        match items {
            Some(x) => for item in x.iter() {
                item.parent = ret.data as *mut SyntaxNode;
            }
            None => ()
        }

        Ok(Some(ret))
    }

    pub fn parse_caseitem_list(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Option<Ref<Vector<Ref<CaseItemSyntax>>>>, Ref<ParserError>> {
        let _r = Region::create(_pr);
        let mut ret: Option<Ref<Array<Ref<CaseItemSyntax>>>> = Option::None;
        loop {
            let node = self.parse_caseitem(&_r, _rp, _ep)?;
            match node {
                None => break,
                Some(node) => {
                    match ret {
                        None => ret = Some(Ref::new(_rp, Array::new())),
                        Some(_) => (),
                    };
                    ret.unwrap().add(node);
                }
            }
        }

        match ret {
            Some(ret) => Ok(Some(Ref::new(_rp, Vector::from_array(_rp, ret)))),
            None => Ok(None),
        }
    }

    pub fn parse_caseitem(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Option<Ref<CaseItemSyntax>>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        let start: Position = self.lexer.get_previous_position();

        let pattern = self.parse_casepattern(&_r, _rp, _ep)?;
        match pattern {
            None =>
                return Ok(None),
            Some(_) => ()
        }

        let successComma2 = self.lexer.parse_punctuation(String::from_string_slice(_r.page, ","));
        if successComma2 {
            self.lexer.advance(&_r)
        } else {
()
        }

        let end: Position = self.lexer.get_position();

        let ret = Ref::new(_rp, CaseItemSyntax { parent: null_mut(), start: start, end: end, pattern: pattern.unwrap() });

        match *pattern.unwrap() {
            CasePatternSyntax::ConstantPattern(x) => x.parent = ret.data as *mut SyntaxNode,
            CasePatternSyntax::WildcardPattern(x) => x.parent = ret.data as *mut SyntaxNode,
            CasePatternSyntax::NamePattern(x) => x.parent = ret.data as *mut SyntaxNode,
        }

        Ok(Some(ret))
    }

    pub fn parse_casepattern_list(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Option<Ref<Vector<Ref<CasePatternSyntax>>>>, Ref<ParserError>> {
        let _r = Region::create(_pr);
        let mut ret: Option<Ref<Array<Ref<CasePatternSyntax>>>> = Option::None;
        loop {
            let node = self.parse_casepattern(&_r, _rp, _ep)?;
            match node {
                None => break,
                Some(node) => {
                    match ret {
                        None => ret = Some(Ref::new(_rp, Array::new())),
                        Some(_) => (),
                    };
                    ret.unwrap().add(node);
                }
            }
        }

        match ret {
            Some(ret) => Ok(Some(Ref::new(_rp, Vector::from_array(_rp, ret)))),
            None => Ok(None),
        }
    }

    pub fn parse_casepattern(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Option<Ref<CasePatternSyntax>>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        {
            let node = self.parse_constantpattern(&_r, _rp, _ep)?;
            match node {
                Some(it) => return Ok(Some(Ref::new(_rp, CasePatternSyntax::ConstantPattern(*it)))),
                None => ()
            }
        }

        {
            let node = self.parse_wildcardpattern(&_r, _rp, _ep)?;
            match node {
                Some(it) => return Ok(Some(Ref::new(_rp, CasePatternSyntax::WildcardPattern(*it)))),
                None => ()
            }
        }

        {
            let node = self.parse_namepattern(&_r, _rp, _ep)?;
            match node {
                Some(it) => return Ok(Some(Ref::new(_rp, CasePatternSyntax::NamePattern(*it)))),
                None => ()
            }
        }

        return Ok(None)
    }

    pub fn parse_constantpattern(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Option<Ref<ConstantPatternSyntax>>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        let start: Position = self.lexer.get_previous_position();

        let constant = self.parse_constant(&_r, _rp, _ep)?;
        match constant {
            None =>
                return Ok(None),
            Some(_) => ()
        }

        let end: Position = self.lexer.get_position();

        let ret = Ref::new(_rp, ConstantPatternSyntax { parent: null_mut(), start: start, end: end, constant: constant.unwrap() });

        constant.unwrap().parent = ret.data as *mut SyntaxNode;

        Ok(Some(ret))
    }

    pub fn parse_wildcardpattern(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Option<Ref<WildcardPatternSyntax>>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        let start: Position = self.lexer.get_previous_position();

        let successUnderscore1 = self.lexer.parse_punctuation(String::from_string_slice(_r.page, "_"));
        if successUnderscore1 {
            self.lexer.advance(&_r)
        } else {
            return Ok(None)
        }

        let end: Position = self.lexer.get_position();

        let ret = Ref::new(_rp, WildcardPatternSyntax { parent: null_mut(), start: start, end: end });

        Ok(Some(ret))
    }

    pub fn parse_namepattern(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Option<Ref<NamePatternSyntax>>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        let start: Position = self.lexer.get_previous_position();

        let name = self.parse_name(&_r, _rp, _ep)?;
        match name {
            None =>
                return Ok(None),
            Some(_) => ()
        }

        let end: Position = self.lexer.get_position();

        let ret = Ref::new(_rp, NamePatternSyntax { parent: null_mut(), start: start, end: end, name: name.unwrap() });

        name.unwrap().parent = ret.data as *mut SyntaxNode;

        Ok(Some(ret))
    }

    pub fn parse_defaultcaselabel(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Option<Ref<DefaultCaseLabelSyntax>>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        let start: Position = self.lexer.get_previous_position();

        let successDefault1 = self.lexer.parse_keyword(String::from_string_slice(_r.page, "default"));
        if successDefault1 {
            self.lexer.advance(&_r)
        } else {
            return Ok(None)
        }

        let end: Position = self.lexer.get_position();

        let ret = Ref::new(_rp, DefaultCaseLabelSyntax { parent: null_mut(), start: start, end: end });

        Ok(Some(ret))
    }

    pub fn parse_for(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Option<Ref<ForSyntax>>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        let start: Position = self.lexer.get_previous_position();

        let successFor1 = self.lexer.parse_keyword(String::from_string_slice(_r.page, "for"));
        if successFor1 {
            self.lexer.advance(&_r)
        } else {
            return Ok(None)
        }

        let successLeftParen2 = self.lexer.parse_punctuation(String::from_string_slice(_r.page, "("));
        if successLeftParen2 {
            self.lexer.advance(&_r)
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

        let index = self.lexer.parse_identifier(_rp);
        match index {
            Some(index) => if self.is_identifier(index) {
                self.lexer.advance(&_r)
            } else {

            return Result::Err(Ref::new(
                _ep,
                ParserError {
                    file_name: self.file_name,
                    line: self.lexer.line,
                    column: self.lexer.column,
                },
            ))
           },
            None => 
            return Result::Err(Ref::new(
                _ep,
                ParserError {
                    file_name: self.file_name,
                    line: self.lexer.line,
                    column: self.lexer.column,
                },
            ))
        }

        let typeAnnotation = self.parse_typeannotation(&_r, _rp, _ep)?;

        let successIn5 = self.lexer.parse_keyword(String::from_string_slice(_r.page, "in"));
        if successIn5 {
            self.lexer.advance(&_r)
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

        let operation = self.parse_operation(&_r, _rp, _ep)?;
        match operation {
            None =>
                return Err(Ref::new(_ep, ParserError { file_name: String::copy(_ep, self.file_name), line: self.lexer.line, column: self.lexer.column })),
            Some(_) => ()
        }

        let successRightParen7 = self.lexer.parse_punctuation(String::from_string_slice(_r.page, ")"));
        if successRightParen7 {
            self.lexer.advance(&_r)
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

        let iteration = self.parse_loop(&_r, _rp, _ep)?;
        match iteration {
            None =>
                return Err(Ref::new(_ep, ParserError { file_name: String::copy(_ep, self.file_name), line: self.lexer.line, column: self.lexer.column })),
            Some(_) => ()
        }

        let end: Position = self.lexer.get_position();

        let ret = Ref::new(_rp, ForSyntax { parent: null_mut(), start: start, end: end, index: index.unwrap(), typeAnnotation: typeAnnotation, operation: operation.unwrap(), iteration: iteration.unwrap() });

        match typeAnnotation {
            Some(x) => 
                x.parent = ret.data as *mut SyntaxNode,
            None => ()
        }

        operation.unwrap().parent = ret.data as *mut SyntaxNode;

        match *iteration.unwrap() {
            LoopSyntax::SimpleLoop(x) => x.parent = ret.data as *mut SyntaxNode,
            LoopSyntax::NamedLoop(x) => x.parent = ret.data as *mut SyntaxNode,
        }

        Ok(Some(ret))
    }

    pub fn parse_while(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Option<Ref<WhileSyntax>>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        let start: Position = self.lexer.get_previous_position();

        let successWhile1 = self.lexer.parse_keyword(String::from_string_slice(_r.page, "while"));
        if successWhile1 {
            self.lexer.advance(&_r)
        } else {
            return Ok(None)
        }

        let successLeftParen2 = self.lexer.parse_punctuation(String::from_string_slice(_r.page, "("));
        if successLeftParen2 {
            self.lexer.advance(&_r)
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

        let condition = self.parse_operation(&_r, _rp, _ep)?;
        match condition {
            None =>
                return Err(Ref::new(_ep, ParserError { file_name: String::copy(_ep, self.file_name), line: self.lexer.line, column: self.lexer.column })),
            Some(_) => ()
        }

        let successRightParen4 = self.lexer.parse_punctuation(String::from_string_slice(_r.page, ")"));
        if successRightParen4 {
            self.lexer.advance(&_r)
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

        let iteration = self.parse_loop(&_r, _rp, _ep)?;
        match iteration {
            None =>
                return Err(Ref::new(_ep, ParserError { file_name: String::copy(_ep, self.file_name), line: self.lexer.line, column: self.lexer.column })),
            Some(_) => ()
        }

        let end: Position = self.lexer.get_position();

        let ret = Ref::new(_rp, WhileSyntax { parent: null_mut(), start: start, end: end, condition: condition.unwrap(), iteration: iteration.unwrap() });

        condition.unwrap().parent = ret.data as *mut SyntaxNode;

        match *iteration.unwrap() {
            LoopSyntax::SimpleLoop(x) => x.parent = ret.data as *mut SyntaxNode,
            LoopSyntax::NamedLoop(x) => x.parent = ret.data as *mut SyntaxNode,
        }

        Ok(Some(ret))
    }

    pub fn parse_do(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Option<Ref<DoSyntax>>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        let start: Position = self.lexer.get_previous_position();

        let successDo1 = self.lexer.parse_keyword(String::from_string_slice(_r.page, "do"));
        if successDo1 {
            self.lexer.advance(&_r)
        } else {
            return Ok(None)
        }

        let iteration = self.parse_loop(&_r, _rp, _ep)?;
        match iteration {
            None =>
                return Err(Ref::new(_ep, ParserError { file_name: String::copy(_ep, self.file_name), line: self.lexer.line, column: self.lexer.column })),
            Some(_) => ()
        }

        let successWhile3 = self.lexer.parse_keyword(String::from_string_slice(_r.page, "while"));
        if successWhile3 {
            self.lexer.advance(&_r)
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

        let successLeftParen4 = self.lexer.parse_punctuation(String::from_string_slice(_r.page, "("));
        if successLeftParen4 {
            self.lexer.advance(&_r)
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

        let condition = self.parse_operation(&_r, _rp, _ep)?;
        match condition {
            None =>
                return Err(Ref::new(_ep, ParserError { file_name: String::copy(_ep, self.file_name), line: self.lexer.line, column: self.lexer.column })),
            Some(_) => ()
        }

        let successRightParen6 = self.lexer.parse_punctuation(String::from_string_slice(_r.page, ")"));
        if successRightParen6 {
            self.lexer.advance(&_r)
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

        let ret = Ref::new(_rp, DoSyntax { parent: null_mut(), start: start, end: end, iteration: iteration.unwrap(), condition: condition.unwrap() });

        match *iteration.unwrap() {
            LoopSyntax::SimpleLoop(x) => x.parent = ret.data as *mut SyntaxNode,
            LoopSyntax::NamedLoop(x) => x.parent = ret.data as *mut SyntaxNode,
        }

        condition.unwrap().parent = ret.data as *mut SyntaxNode;

        Ok(Some(ret))
    }

    pub fn parse_loop(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Option<Ref<LoopSyntax>>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        {
            let node = self.parse_simpleloop(&_r, _rp, _ep)?;
            match node {
                Some(it) => return Ok(Some(Ref::new(_rp, LoopSyntax::SimpleLoop(*it)))),
                None => ()
            }
        }

        {
            let node = self.parse_namedloop(&_r, _rp, _ep)?;
            match node {
                Some(it) => return Ok(Some(Ref::new(_rp, LoopSyntax::NamedLoop(*it)))),
                None => ()
            }
        }

        return Ok(None)
    }

    pub fn parse_simpleloop(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Option<Ref<SimpleLoopSyntax>>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        let start: Position = self.lexer.get_previous_position();

        let code = self.parse_block(&_r, _rp, _ep)?;
        match code {
            None =>
                return Ok(None),
            Some(_) => ()
        }

        let end: Position = self.lexer.get_position();

        let ret = Ref::new(_rp, SimpleLoopSyntax { parent: null_mut(), start: start, end: end, code: code.unwrap() });

        code.unwrap().parent = ret.data as *mut SyntaxNode;

        Ok(Some(ret))
    }

    pub fn parse_namedloop(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Option<Ref<NamedLoopSyntax>>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        let start: Position = self.lexer.get_previous_position();

        let successLoop1 = self.lexer.parse_keyword(String::from_string_slice(_r.page, "loop"));
        if successLoop1 {
            self.lexer.advance(&_r)
        } else {
            return Ok(None)
        }

        let name = self.lexer.parse_identifier(_rp);
        match name {
            Some(name) => if self.is_identifier(name) {
                self.lexer.advance(&_r)
            } else {

            return Result::Err(Ref::new(
                _ep,
                ParserError {
                    file_name: self.file_name,
                    line: self.lexer.line,
                    column: self.lexer.column,
                },
            ))
           },
            None => 
            return Result::Err(Ref::new(
                _ep,
                ParserError {
                    file_name: self.file_name,
                    line: self.lexer.line,
                    column: self.lexer.column,
                },
            ))
        }

        let code = self.parse_block(&_r, _rp, _ep)?;
        match code {
            None =>
                return Err(Ref::new(_ep, ParserError { file_name: String::copy(_ep, self.file_name), line: self.lexer.line, column: self.lexer.column })),
            Some(_) => ()
        }

        let end: Position = self.lexer.get_position();

        let ret = Ref::new(_rp, NamedLoopSyntax { parent: null_mut(), start: start, end: end, name: name.unwrap(), code: code.unwrap() });

        code.unwrap().parent = ret.data as *mut SyntaxNode;

        Ok(Some(ret))
    }

    pub fn parse_this(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Option<Ref<ThisSyntax>>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        let start: Position = self.lexer.get_previous_position();

        let successThis1 = self.lexer.parse_keyword(String::from_string_slice(_r.page, "this"));
        if successThis1 {
            self.lexer.advance(&_r)
        } else {
            return Ok(None)
        }

        let end: Position = self.lexer.get_position();

        let ret = Ref::new(_rp, ThisSyntax { parent: null_mut(), start: start, end: end });

        Ok(Some(ret))
    }

    pub fn parse_new(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Option<Ref<NewSyntax>>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        let start: Position = self.lexer.get_previous_position();

        let successNew1 = self.lexer.parse_keyword(String::from_string_slice(_r.page, "new"));
        if successNew1 {
            self.lexer.advance(&_r)
        } else {
            return Ok(None)
        }

        let typeSpec = self.parse_type(&_r, _rp, _ep)?;
        match typeSpec {
            None =>
                return Err(Ref::new(_ep, ParserError { file_name: String::copy(_ep, self.file_name), line: self.lexer.line, column: self.lexer.column })),
            Some(_) => ()
        }

        let end: Position = self.lexer.get_position();

        let ret = Ref::new(_rp, NewSyntax { parent: null_mut(), start: start, end: end, typeSpec: typeSpec.unwrap() });

        typeSpec.unwrap().parent = ret.data as *mut SyntaxNode;

        Ok(Some(ret))
    }

    pub fn parse_object(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Option<Ref<ObjectSyntax>>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        let start: Position = self.lexer.get_previous_position();

        let successLeftParen1 = self.lexer.parse_punctuation(String::from_string_slice(_r.page, "("));
        if successLeftParen1 {
            self.lexer.advance(&_r)
        } else {
            return Ok(None)
        }

        let firstOp = self.parse_operation(&_r, _rp, _ep)?;

        let additionalOps = self.parse_item_list(&_r, _rp, _ep)?;

        let successRightParen4 = self.lexer.parse_punctuation(String::from_string_slice(_r.page, ")"));
        if successRightParen4 {
            self.lexer.advance(&_r)
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

        let ret = Ref::new(_rp, ObjectSyntax { parent: null_mut(), start: start, end: end, firstOp: firstOp, additionalOps: additionalOps });

        match firstOp {
            Some(x) => 
                x.parent = ret.data as *mut SyntaxNode,
            None => ()
        }

        match additionalOps {
            Some(x) => for item in x.iter() {
                item.parent = ret.data as *mut SyntaxNode;
            }
            None => ()
        }

        Ok(Some(ret))
    }

    pub fn parse_array(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Option<Ref<ArraySyntax>>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        let start: Position = self.lexer.get_previous_position();

        let successLeftBracket1 = self.lexer.parse_punctuation(String::from_string_slice(_r.page, "["));
        if successLeftBracket1 {
            self.lexer.advance(&_r)
        } else {
            return Ok(None)
        }

        let firstOp = self.parse_operation(&_r, _rp, _ep)?;

        let additionalOps = self.parse_item_list(&_r, _rp, _ep)?;

        let successRightBracket4 = self.lexer.parse_punctuation(String::from_string_slice(_r.page, "]"));
        if successRightBracket4 {
            self.lexer.advance(&_r)
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

        let ret = Ref::new(_rp, ArraySyntax { parent: null_mut(), start: start, end: end, firstOp: firstOp, additionalOps: additionalOps });

        match firstOp {
            Some(x) => 
                x.parent = ret.data as *mut SyntaxNode,
            None => ()
        }

        match additionalOps {
            Some(x) => for item in x.iter() {
                item.parent = ret.data as *mut SyntaxNode;
            }
            None => ()
        }

        Ok(Some(ret))
    }

    pub fn parse_item_list(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Option<Ref<Vector<Ref<ItemSyntax>>>>, Ref<ParserError>> {
        let _r = Region::create(_pr);
        let mut ret: Option<Ref<Array<Ref<ItemSyntax>>>> = Option::None;
        loop {
            let node = self.parse_item(&_r, _rp, _ep)?;
            match node {
                None => break,
                Some(node) => {
                    match ret {
                        None => ret = Some(Ref::new(_rp, Array::new())),
                        Some(_) => (),
                    };
                    ret.unwrap().add(node);
                }
            }
        }

        match ret {
            Some(ret) => Ok(Some(Ref::new(_rp, Vector::from_array(_rp, ret)))),
            None => Ok(None),
        }
    }

    pub fn parse_item(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Option<Ref<ItemSyntax>>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        let start: Position = self.lexer.get_previous_position();

        let successComma1 = self.lexer.parse_punctuation(String::from_string_slice(_r.page, ","));
        if successComma1 {
            self.lexer.advance(&_r)
        } else {
            return Ok(None)
        }

        let operation = self.parse_operation(&_r, _rp, _ep)?;
        match operation {
            None =>
                return Err(Ref::new(_ep, ParserError { file_name: String::copy(_ep, self.file_name), line: self.lexer.line, column: self.lexer.column })),
            Some(_) => ()
        }

        let end: Position = self.lexer.get_position();

        let ret = Ref::new(_rp, ItemSyntax { parent: null_mut(), start: start, end: end, operation: operation.unwrap() });

        operation.unwrap().parent = ret.data as *mut SyntaxNode;

        Ok(Some(ret))
    }

    pub fn parse_sizeof(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Option<Ref<SizeOfSyntax>>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        let start: Position = self.lexer.get_previous_position();

        let successSizeof1 = self.lexer.parse_keyword(String::from_string_slice(_r.page, "sizeof"));
        if successSizeof1 {
            self.lexer.advance(&_r)
        } else {
            return Ok(None)
        }

        let typeSpec = self.parse_type(&_r, _rp, _ep)?;
        match typeSpec {
            None =>
                return Err(Ref::new(_ep, ParserError { file_name: String::copy(_ep, self.file_name), line: self.lexer.line, column: self.lexer.column })),
            Some(_) => ()
        }

        let end: Position = self.lexer.get_position();

        let ret = Ref::new(_rp, SizeOfSyntax { parent: null_mut(), start: start, end: end, typeSpec: typeSpec.unwrap() });

        typeSpec.unwrap().parent = ret.data as *mut SyntaxNode;

        Ok(Some(ret))
    }

    pub fn parse_break(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Option<Ref<BreakSyntax>>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        let start: Position = self.lexer.get_previous_position();

        let successBreak1 = self.lexer.parse_keyword(String::from_string_slice(_r.page, "break"));
        if successBreak1 {
            self.lexer.advance(&_r)
        } else {
            return Ok(None)
        }

        let iteration = self.lexer.parse_identifier(_rp);
        match iteration {
            Some(iteration) => if self.is_identifier(iteration) {
                self.lexer.advance(&_r)
            } else {
()
           },
            None => ()
        }

        let successSemicolon3 = self.lexer.parse_punctuation(String::from_string_slice(_r.page, ";"));
        if successSemicolon3 {
            self.lexer.advance(&_r)
        } else {
()
        }

        let end: Position = self.lexer.get_position();

        let ret = Ref::new(_rp, BreakSyntax { parent: null_mut(), start: start, end: end, iteration: iteration });

        Ok(Some(ret))
    }

    pub fn parse_continue(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Option<Ref<ContinueSyntax>>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        let start: Position = self.lexer.get_previous_position();

        let successContinue1 = self.lexer.parse_keyword(String::from_string_slice(_r.page, "continue"));
        if successContinue1 {
            self.lexer.advance(&_r)
        } else {
            return Ok(None)
        }

        let iteration = self.lexer.parse_identifier(_rp);
        match iteration {
            Some(iteration) => if self.is_identifier(iteration) {
                self.lexer.advance(&_r)
            } else {
()
           },
            None => ()
        }

        let successSemicolon3 = self.lexer.parse_punctuation(String::from_string_slice(_r.page, ";"));
        if successSemicolon3 {
            self.lexer.advance(&_r)
        } else {
()
        }

        let end: Position = self.lexer.get_position();

        let ret = Ref::new(_rp, ContinueSyntax { parent: null_mut(), start: start, end: end, iteration: iteration });

        Ok(Some(ret))
    }

    pub fn parse_return(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Option<Ref<ReturnSyntax>>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        let start: Position = self.lexer.get_previous_position();

        let successReturn1 = self.lexer.parse_keyword(String::from_string_slice(_r.page, "return"));
        if successReturn1 {
            self.lexer.advance(&_r)
        } else {
            return Ok(None)
        }

        let result = self.parse_calculation(&_r, _rp, _ep)?;

        let end: Position = self.lexer.get_position();

        let ret = Ref::new(_rp, ReturnSyntax { parent: null_mut(), start: start, end: end, result: result });

        match result {
            Some(x) => 
                x.parent = ret.data as *mut SyntaxNode,
            None => ()
        }

        Ok(Some(ret))
    }

    pub fn parse_throw(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Option<Ref<ThrowSyntax>>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        let start: Position = self.lexer.get_previous_position();

        let successThrow1 = self.lexer.parse_keyword(String::from_string_slice(_r.page, "throw"));
        if successThrow1 {
            self.lexer.advance(&_r)
        } else {
            return Ok(None)
        }

        let exception = self.parse_calculation(&_r, _rp, _ep)?;
        match exception {
            None =>
                return Err(Ref::new(_ep, ParserError { file_name: String::copy(_ep, self.file_name), line: self.lexer.line, column: self.lexer.column })),
            Some(_) => ()
        }

        let end: Position = self.lexer.get_position();

        let ret = Ref::new(_rp, ThrowSyntax { parent: null_mut(), start: start, end: end, exception: exception.unwrap() });

        exception.unwrap().parent = ret.data as *mut SyntaxNode;

        Ok(Some(ret))
    }

    pub fn parse_class(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Option<Ref<ClassSyntax>>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        let start: Position = self.lexer.get_previous_position();

        let successClass1 = self.lexer.parse_keyword(String::from_string_slice(_r.page, "class"));
        if successClass1 {
            self.lexer.advance(&_r)
        } else {
            return Ok(None)
        }

        let name = self.parse_name(&_r, _rp, _ep)?;
        match name {
            None =>
                return Err(Ref::new(_ep, ParserError { file_name: String::copy(_ep, self.file_name), line: self.lexer.line, column: self.lexer.column })),
            Some(_) => ()
        }

        let generics = self.parse_genericparameters(&_r, _rp, _ep)?;

        let baseClass = self.parse_extends(&_r, _rp, _ep)?;

        let contents = self.parse_structure(&_r, _rp, _ep)?;

        let body = self.parse_classbody(&_r, _rp, _ep)?;

        let end: Position = self.lexer.get_position();

        let ret = Ref::new(_rp, ClassSyntax { parent: null_mut(), start: start, end: end, name: name.unwrap(), generics: generics, baseClass: baseClass, contents: contents, body: body });

        name.unwrap().parent = ret.data as *mut SyntaxNode;

        match generics {
            Some(x) => 
                x.parent = ret.data as *mut SyntaxNode,
            None => ()
        }

        match baseClass {
            Some(x) => 
                x.parent = ret.data as *mut SyntaxNode,
            None => ()
        }

        match contents {
            Some(x) => 
                x.parent = ret.data as *mut SyntaxNode,
            None => ()
        }

        match body {
            Some(x) => 
                x.parent = ret.data as *mut SyntaxNode,
            None => ()
        }

        Ok(Some(ret))
    }

    pub fn parse_genericparameters(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Option<Ref<GenericParametersSyntax>>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        let start: Position = self.lexer.get_previous_position();

        let successLeftBracket1 = self.lexer.parse_punctuation(String::from_string_slice(_r.page, "["));
        if successLeftBracket1 {
            self.lexer.advance(&_r)
        } else {
            return Ok(None)
        }

        let name = self.lexer.parse_identifier(_rp);
        match name {
            Some(name) => if self.is_identifier(name) {
                self.lexer.advance(&_r)
            } else {

            return Result::Err(Ref::new(
                _ep,
                ParserError {
                    file_name: self.file_name,
                    line: self.lexer.line,
                    column: self.lexer.column,
                },
            ))
           },
            None => 
            return Result::Err(Ref::new(
                _ep,
                ParserError {
                    file_name: self.file_name,
                    line: self.lexer.line,
                    column: self.lexer.column,
                },
            ))
        }

        let additionalGenerics = self.parse_genericparameter_list(&_r, _rp, _ep)?;

        let successRightBracket4 = self.lexer.parse_punctuation(String::from_string_slice(_r.page, "]"));
        if successRightBracket4 {
            self.lexer.advance(&_r)
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

        let ret = Ref::new(_rp, GenericParametersSyntax { parent: null_mut(), start: start, end: end, name: name.unwrap(), additionalGenerics: additionalGenerics });

        match additionalGenerics {
            Some(x) => for item in x.iter() {
                item.parent = ret.data as *mut SyntaxNode;
            }
            None => ()
        }

        Ok(Some(ret))
    }

    pub fn parse_genericparameter_list(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Option<Ref<Vector<Ref<GenericParameterSyntax>>>>, Ref<ParserError>> {
        let _r = Region::create(_pr);
        let mut ret: Option<Ref<Array<Ref<GenericParameterSyntax>>>> = Option::None;
        loop {
            let node = self.parse_genericparameter(&_r, _rp, _ep)?;
            match node {
                None => break,
                Some(node) => {
                    match ret {
                        None => ret = Some(Ref::new(_rp, Array::new())),
                        Some(_) => (),
                    };
                    ret.unwrap().add(node);
                }
            }
        }

        match ret {
            Some(ret) => Ok(Some(Ref::new(_rp, Vector::from_array(_rp, ret)))),
            None => Ok(None),
        }
    }

    pub fn parse_genericparameter(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Option<Ref<GenericParameterSyntax>>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        let start: Position = self.lexer.get_previous_position();

        let successComma1 = self.lexer.parse_punctuation(String::from_string_slice(_r.page, ","));
        if successComma1 {
            self.lexer.advance(&_r)
        } else {
            return Ok(None)
        }

        let name = self.lexer.parse_identifier(_rp);
        match name {
            Some(name) => if self.is_identifier(name) {
                self.lexer.advance(&_r)
            } else {

            return Result::Err(Ref::new(
                _ep,
                ParserError {
                    file_name: self.file_name,
                    line: self.lexer.line,
                    column: self.lexer.column,
                },
            ))
           },
            None => 
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

        let ret = Ref::new(_rp, GenericParameterSyntax { parent: null_mut(), start: start, end: end, name: name.unwrap() });

        Ok(Some(ret))
    }

    pub fn parse_extends(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Option<Ref<ExtendsSyntax>>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        let start: Position = self.lexer.get_previous_position();

        let successExtends1 = self.lexer.parse_keyword(String::from_string_slice(_r.page, "extends"));
        if successExtends1 {
            self.lexer.advance(&_r)
        } else {
            return Ok(None)
        }

        let name = self.parse_name(&_r, _rp, _ep)?;
        match name {
            None =>
                return Err(Ref::new(_ep, ParserError { file_name: String::copy(_ep, self.file_name), line: self.lexer.line, column: self.lexer.column })),
            Some(_) => ()
        }

        let end: Position = self.lexer.get_position();

        let ret = Ref::new(_rp, ExtendsSyntax { parent: null_mut(), start: start, end: end, name: name.unwrap() });

        name.unwrap().parent = ret.data as *mut SyntaxNode;

        Ok(Some(ret))
    }

    pub fn parse_structure(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Option<Ref<StructureSyntax>>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        let start: Position = self.lexer.get_previous_position();

        let successLeftParen1 = self.lexer.parse_punctuation(String::from_string_slice(_r.page, "("));
        if successLeftParen1 {
            self.lexer.advance(&_r)
        } else {
            return Ok(None)
        }

        let components = self.parse_component_list(&_r, _rp, _ep)?;

        let successRightParen3 = self.lexer.parse_punctuation(String::from_string_slice(_r.page, ")"));
        if successRightParen3 {
            self.lexer.advance(&_r)
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

        let ret = Ref::new(_rp, StructureSyntax { parent: null_mut(), start: start, end: end, components: components });

        match components {
            Some(x) => for item in x.iter() {
                item.parent = ret.data as *mut SyntaxNode;
            }
            None => ()
        }

        Ok(Some(ret))
    }

    pub fn parse_component_list(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Option<Ref<Vector<Ref<ComponentSyntax>>>>, Ref<ParserError>> {
        let _r = Region::create(_pr);
        let mut ret: Option<Ref<Array<Ref<ComponentSyntax>>>> = Option::None;
        loop {
            let node = self.parse_component(&_r, _rp, _ep)?;
            match node {
                None => break,
                Some(node) => {
                    match ret {
                        None => ret = Some(Ref::new(_rp, Array::new())),
                        Some(_) => (),
                    };
                    ret.unwrap().add(node);
                }
            }
        }

        match ret {
            Some(ret) => Ok(Some(Ref::new(_rp, Vector::from_array(_rp, ret)))),
            None => Ok(None),
        }
    }

    pub fn parse_component(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Option<Ref<ComponentSyntax>>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        let start: Position = self.lexer.get_previous_position();

        let name = self.lexer.parse_identifier(_rp);
        match name {
            Some(name) => if self.is_identifier(name) {
                self.lexer.advance(&_r)
            } else {
            return Ok(None)
           },
            None =>             return Ok(None)
        }

        let typeAnnotation = self.parse_typeannotation(&_r, _rp, _ep)?;

        let successComma3 = self.lexer.parse_punctuation(String::from_string_slice(_r.page, ","));
        if successComma3 {
            self.lexer.advance(&_r)
        } else {
()
        }

        let end: Position = self.lexer.get_position();

        let ret = Ref::new(_rp, ComponentSyntax { parent: null_mut(), start: start, end: end, name: name.unwrap(), typeAnnotation: typeAnnotation });

        match typeAnnotation {
            Some(x) => 
                x.parent = ret.data as *mut SyntaxNode,
            None => ()
        }

        Ok(Some(ret))
    }

    pub fn parse_classbody(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Option<Ref<ClassBodySyntax>>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        let start: Position = self.lexer.get_previous_position();

        let successLeftCurly1 = self.lexer.parse_punctuation(String::from_string_slice(_r.page, "{"));
        if successLeftCurly1 {
            self.lexer.advance(&_r)
        } else {
            return Ok(None)
        }

        let members = self.parse_classmember_list(&_r, _rp, _ep)?;

        let successRightCurly3 = self.lexer.parse_punctuation(String::from_string_slice(_r.page, "}"));
        if successRightCurly3 {
            self.lexer.advance(&_r)
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

        let ret = Ref::new(_rp, ClassBodySyntax { parent: null_mut(), start: start, end: end, members: members });

        match members {
            Some(x) => for item in x.iter() {
                match **item {
                    ClassMemberSyntax::LetMember(y) => y.parent = ret.data as *mut SyntaxNode,
                    ClassMemberSyntax::VarMember(y) => y.parent = ret.data as *mut SyntaxNode,
                    ClassMemberSyntax::MutableMember(y) => y.parent = ret.data as *mut SyntaxNode,
                    ClassMemberSyntax::SetInitialization(y) => y.parent = ret.data as *mut SyntaxNode,
                    ClassMemberSyntax::Method(y) => y.parent = ret.data as *mut SyntaxNode,
                    ClassMemberSyntax::StaticFunction(y) => y.parent = ret.data as *mut SyntaxNode,
                    ClassMemberSyntax::Operator(y) => y.parent = ret.data as *mut SyntaxNode,
                    ClassMemberSyntax::Initializer(y) => y.parent = ret.data as *mut SyntaxNode,
                    ClassMemberSyntax::Allocator(y) => y.parent = ret.data as *mut SyntaxNode,
                }
            }
            None => ()
        }

        Ok(Some(ret))
    }

    pub fn parse_classmember_list(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Option<Ref<Vector<Ref<ClassMemberSyntax>>>>, Ref<ParserError>> {
        let _r = Region::create(_pr);
        let mut ret: Option<Ref<Array<Ref<ClassMemberSyntax>>>> = Option::None;
        loop {
            let node = self.parse_classmember(&_r, _rp, _ep)?;
            match node {
                None => break,
                Some(node) => {
                    match ret {
                        None => ret = Some(Ref::new(_rp, Array::new())),
                        Some(_) => (),
                    };
                    ret.unwrap().add(node);
                }
            }
        }

        match ret {
            Some(ret) => Ok(Some(Ref::new(_rp, Vector::from_array(_rp, ret)))),
            None => Ok(None),
        }
    }

    pub fn parse_classmember(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Option<Ref<ClassMemberSyntax>>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        {
            let node = self.parse_letmember(&_r, _rp, _ep)?;
            match node {
                Some(it) => return Ok(Some(Ref::new(_rp, ClassMemberSyntax::LetMember(*it)))),
                None => ()
            }
        }

        {
            let node = self.parse_varmember(&_r, _rp, _ep)?;
            match node {
                Some(it) => return Ok(Some(Ref::new(_rp, ClassMemberSyntax::VarMember(*it)))),
                None => ()
            }
        }

        {
            let node = self.parse_mutablemember(&_r, _rp, _ep)?;
            match node {
                Some(it) => return Ok(Some(Ref::new(_rp, ClassMemberSyntax::MutableMember(*it)))),
                None => ()
            }
        }

        {
            let node = self.parse_setinitialization(&_r, _rp, _ep)?;
            match node {
                Some(it) => return Ok(Some(Ref::new(_rp, ClassMemberSyntax::SetInitialization(*it)))),
                None => ()
            }
        }

        {
            let node = self.parse_method(&_r, _rp, _ep)?;
            match node {
                Some(it) => return Ok(Some(Ref::new(_rp, ClassMemberSyntax::Method(*it)))),
                None => ()
            }
        }

        {
            let node = self.parse_staticfunction(&_r, _rp, _ep)?;
            match node {
                Some(it) => return Ok(Some(Ref::new(_rp, ClassMemberSyntax::StaticFunction(*it)))),
                None => ()
            }
        }

        {
            let node = self.parse_operator(&_r, _rp, _ep)?;
            match node {
                Some(it) => return Ok(Some(Ref::new(_rp, ClassMemberSyntax::Operator(*it)))),
                None => ()
            }
        }

        {
            let node = self.parse_initializer(&_r, _rp, _ep)?;
            match node {
                Some(it) => return Ok(Some(Ref::new(_rp, ClassMemberSyntax::Initializer(*it)))),
                None => ()
            }
        }

        {
            let node = self.parse_allocator(&_r, _rp, _ep)?;
            match node {
                Some(it) => return Ok(Some(Ref::new(_rp, ClassMemberSyntax::Allocator(*it)))),
                None => ()
            }
        }

        return Ok(None)
    }

    pub fn parse_letmember(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Option<Ref<LetMemberSyntax>>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        let start: Position = self.lexer.get_previous_position();

        let declaration = self.parse_let(&_r, _rp, _ep)?;
        match declaration {
            None =>
                return Ok(None),
            Some(_) => ()
        }

        let end: Position = self.lexer.get_position();

        let ret = Ref::new(_rp, LetMemberSyntax { parent: null_mut(), start: start, end: end, declaration: declaration.unwrap() });

        declaration.unwrap().parent = ret.data as *mut SyntaxNode;

        Ok(Some(ret))
    }

    pub fn parse_varmember(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Option<Ref<VarMemberSyntax>>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        let start: Position = self.lexer.get_previous_position();

        let declaration = self.parse_var(&_r, _rp, _ep)?;
        match declaration {
            None =>
                return Ok(None),
            Some(_) => ()
        }

        let end: Position = self.lexer.get_position();

        let ret = Ref::new(_rp, VarMemberSyntax { parent: null_mut(), start: start, end: end, declaration: declaration.unwrap() });

        declaration.unwrap().parent = ret.data as *mut SyntaxNode;

        Ok(Some(ret))
    }

    pub fn parse_mutablemember(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Option<Ref<MutableMemberSyntax>>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        let start: Position = self.lexer.get_previous_position();

        let declaration = self.parse_mutable(&_r, _rp, _ep)?;
        match declaration {
            None =>
                return Ok(None),
            Some(_) => ()
        }

        let end: Position = self.lexer.get_position();

        let ret = Ref::new(_rp, MutableMemberSyntax { parent: null_mut(), start: start, end: end, declaration: declaration.unwrap() });

        declaration.unwrap().parent = ret.data as *mut SyntaxNode;

        Ok(Some(ret))
    }

    pub fn parse_setinitialization(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Option<Ref<SetInitializationSyntax>>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        let start: Position = self.lexer.get_previous_position();

        let definition = self.parse_set(&_r, _rp, _ep)?;
        match definition {
            None =>
                return Ok(None),
            Some(_) => ()
        }

        let end: Position = self.lexer.get_position();

        let ret = Ref::new(_rp, SetInitializationSyntax { parent: null_mut(), start: start, end: end, definition: definition.unwrap() });

        definition.unwrap().parent = ret.data as *mut SyntaxNode;

        Ok(Some(ret))
    }

    pub fn parse_method(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Option<Ref<MethodSyntax>>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        let start: Position = self.lexer.get_previous_position();

        let successMethod1 = self.lexer.parse_keyword(String::from_string_slice(_r.page, "method"));
        if successMethod1 {
            self.lexer.advance(&_r)
        } else {
            return Ok(None)
        }

        let procedure = self.parse_procedure(&_r, _rp, _ep)?;
        match procedure {
            None =>
                return Err(Ref::new(_ep, ParserError { file_name: String::copy(_ep, self.file_name), line: self.lexer.line, column: self.lexer.column })),
            Some(_) => ()
        }

        let end: Position = self.lexer.get_position();

        let ret = Ref::new(_rp, MethodSyntax { parent: null_mut(), start: start, end: end, procedure: procedure.unwrap() });

        procedure.unwrap().parent = ret.data as *mut SyntaxNode;

        Ok(Some(ret))
    }

    pub fn parse_staticfunction(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Option<Ref<StaticFunctionSyntax>>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        let start: Position = self.lexer.get_previous_position();

        let procedure = self.parse_function(&_r, _rp, _ep)?;
        match procedure {
            None =>
                return Ok(None),
            Some(_) => ()
        }

        let end: Position = self.lexer.get_position();

        let ret = Ref::new(_rp, StaticFunctionSyntax { parent: null_mut(), start: start, end: end, procedure: procedure.unwrap() });

        procedure.unwrap().parent = ret.data as *mut SyntaxNode;

        Ok(Some(ret))
    }

    pub fn parse_operator(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Option<Ref<OperatorSyntax>>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        let start: Position = self.lexer.get_previous_position();

        let successOperator1 = self.lexer.parse_keyword(String::from_string_slice(_r.page, "operator"));
        if successOperator1 {
            self.lexer.advance(&_r)
        } else {
            return Ok(None)
        }

        let routine = self.parse_routine(&_r, _rp, _ep)?;
        match routine {
            None =>
                return Err(Ref::new(_ep, ParserError { file_name: String::copy(_ep, self.file_name), line: self.lexer.line, column: self.lexer.column })),
            Some(_) => ()
        }

        let end: Position = self.lexer.get_position();

        let ret = Ref::new(_rp, OperatorSyntax { parent: null_mut(), start: start, end: end, routine: routine.unwrap() });

        routine.unwrap().parent = ret.data as *mut SyntaxNode;

        Ok(Some(ret))
    }

    pub fn parse_initializer(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Option<Ref<InitializerSyntax>>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        let start: Position = self.lexer.get_previous_position();

        let successInitializer1 = self.lexer.parse_keyword(String::from_string_slice(_r.page, "initializer"));
        if successInitializer1 {
            self.lexer.advance(&_r)
        } else {
            return Ok(None)
        }

        let input = self.parse_structure(&_r, _rp, _ep)?;

        let body = self.parse_block(&_r, _rp, _ep)?;
        match body {
            None =>
                return Err(Ref::new(_ep, ParserError { file_name: String::copy(_ep, self.file_name), line: self.lexer.line, column: self.lexer.column })),
            Some(_) => ()
        }

        let end: Position = self.lexer.get_position();

        let ret = Ref::new(_rp, InitializerSyntax { parent: null_mut(), start: start, end: end, input: input, body: body.unwrap() });

        match input {
            Some(x) => 
                x.parent = ret.data as *mut SyntaxNode,
            None => ()
        }

        body.unwrap().parent = ret.data as *mut SyntaxNode;

        Ok(Some(ret))
    }

    pub fn parse_allocator(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Option<Ref<AllocatorSyntax>>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        let start: Position = self.lexer.get_previous_position();

        let successAllocator1 = self.lexer.parse_keyword(String::from_string_slice(_r.page, "allocator"));
        if successAllocator1 {
            self.lexer.advance(&_r)
        } else {
            return Ok(None)
        }

        let input = self.parse_structure(&_r, _rp, _ep)?;

        let body = self.parse_block(&_r, _rp, _ep)?;
        match body {
            None =>
                return Err(Ref::new(_ep, ParserError { file_name: String::copy(_ep, self.file_name), line: self.lexer.line, column: self.lexer.column })),
            Some(_) => ()
        }

        let end: Position = self.lexer.get_position();

        let ret = Ref::new(_rp, AllocatorSyntax { parent: null_mut(), start: start, end: end, input: input, body: body.unwrap() });

        match input {
            Some(x) => 
                x.parent = ret.data as *mut SyntaxNode,
            None => ()
        }

        body.unwrap().parent = ret.data as *mut SyntaxNode;

        Ok(Some(ret))
    }

    pub fn parse_typeannotation(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Option<Ref<TypeAnnotationSyntax>>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        let start: Position = self.lexer.get_previous_position();

        let successColon1 = self.lexer.parse_punctuation(String::from_string_slice(_r.page, ":"));
        if successColon1 {
            self.lexer.advance(&_r)
        } else {
            return Ok(None)
        }

        let typeSpec = self.parse_typespec(&_r, _rp, _ep)?;
        match typeSpec {
            None =>
                return Err(Ref::new(_ep, ParserError { file_name: String::copy(_ep, self.file_name), line: self.lexer.line, column: self.lexer.column })),
            Some(_) => ()
        }

        let end: Position = self.lexer.get_position();

        let ret = Ref::new(_rp, TypeAnnotationSyntax { parent: null_mut(), start: start, end: end, typeSpec: typeSpec.unwrap() });

        match *typeSpec.unwrap() {
            TypeSpecSyntax::Type(x) => x.parent = ret.data as *mut SyntaxNode,
            TypeSpecSyntax::Variant(x) => x.parent = ret.data as *mut SyntaxNode,
        }

        Ok(Some(ret))
    }

    pub fn parse_typespec_list(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Option<Ref<Vector<Ref<TypeSpecSyntax>>>>, Ref<ParserError>> {
        let _r = Region::create(_pr);
        let mut ret: Option<Ref<Array<Ref<TypeSpecSyntax>>>> = Option::None;
        loop {
            let node = self.parse_typespec(&_r, _rp, _ep)?;
            match node {
                None => break,
                Some(node) => {
                    match ret {
                        None => ret = Some(Ref::new(_rp, Array::new())),
                        Some(_) => (),
                    };
                    ret.unwrap().add(node);
                }
            }
        }

        match ret {
            Some(ret) => Ok(Some(Ref::new(_rp, Vector::from_array(_rp, ret)))),
            None => Ok(None),
        }
    }

    pub fn parse_typespec(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Option<Ref<TypeSpecSyntax>>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        {
            let node = self.parse_type(&_r, _rp, _ep)?;
            match node {
                Some(it) => return Ok(Some(Ref::new(_rp, TypeSpecSyntax::Type(*it)))),
                None => ()
            }
        }

        {
            let node = self.parse_variant(&_r, _rp, _ep)?;
            match node {
                Some(it) => return Ok(Some(Ref::new(_rp, TypeSpecSyntax::Variant(*it)))),
                None => ()
            }
        }

        return Ok(None)
    }

    pub fn parse_type_list(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Option<Ref<Vector<Ref<TypeSyntax>>>>, Ref<ParserError>> {
        let _r = Region::create(_pr);
        let mut ret: Option<Ref<Array<Ref<TypeSyntax>>>> = Option::None;
        loop {
            let node = self.parse_type(&_r, _rp, _ep)?;
            match node {
                None => break,
                Some(node) => {
                    match ret {
                        None => ret = Some(Ref::new(_rp, Array::new())),
                        Some(_) => (),
                    };
                    ret.unwrap().add(node);
                }
            }
        }

        match ret {
            Some(ret) => Ok(Some(Ref::new(_rp, Vector::from_array(_rp, ret)))),
            None => Ok(None),
        }
    }

    pub fn parse_type(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Option<Ref<TypeSyntax>>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        let start: Position = self.lexer.get_previous_position();

        let name = self.parse_name(&_r, _rp, _ep)?;
        match name {
            None =>
                return Ok(None),
            Some(_) => ()
        }

        let generics = self.parse_genericarguments(&_r, _rp, _ep)?;

        let optional = self.parse_optional(&_r, _rp, _ep)?;

        let lifeTime = self.parse_lifetime(&_r, _rp, _ep)?;

        let end: Position = self.lexer.get_position();

        let ret = Ref::new(_rp, TypeSyntax { parent: null_mut(), start: start, end: end, name: name.unwrap(), generics: generics, optional: optional, lifeTime: lifeTime });

        name.unwrap().parent = ret.data as *mut SyntaxNode;

        match generics {
            Some(x) => 
                x.parent = ret.data as *mut SyntaxNode,
            None => ()
        }

        match optional {
            Some(x) => 
                x.parent = ret.data as *mut SyntaxNode,
            None => ()
        }

        match lifeTime {
            Some(x) => 
                match *x {
                    LifeTimeSyntax::Root(y) => y.parent = ret.data as *mut SyntaxNode,
                    LifeTimeSyntax::Local(y) => y.parent = ret.data as *mut SyntaxNode,
                    LifeTimeSyntax::Reference(y) => y.parent = ret.data as *mut SyntaxNode,
                    LifeTimeSyntax::Thrown(y) => y.parent = ret.data as *mut SyntaxNode,
                },
            None => ()
        }

        Ok(Some(ret))
    }

    pub fn parse_variant(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Option<Ref<VariantSyntax>>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        let start: Position = self.lexer.get_previous_position();

        let successLeftParen1 = self.lexer.parse_punctuation(String::from_string_slice(_r.page, "("));
        if successLeftParen1 {
            self.lexer.advance(&_r)
        } else {
            return Ok(None)
        }

        let types = self.parse_type_list(&_r, _rp, _ep)?;

        let successRightParen3 = self.lexer.parse_punctuation(String::from_string_slice(_r.page, ")"));
        if successRightParen3 {
            self.lexer.advance(&_r)
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

        let ret = Ref::new(_rp, VariantSyntax { parent: null_mut(), start: start, end: end, types: types });

        match types {
            Some(x) => for item in x.iter() {
                item.parent = ret.data as *mut SyntaxNode;
            }
            None => ()
        }

        Ok(Some(ret))
    }

    pub fn parse_throws(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Option<Ref<ThrowsSyntax>>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        let start: Position = self.lexer.get_previous_position();

        let successThrows1 = self.lexer.parse_keyword(String::from_string_slice(_r.page, "throws"));
        if successThrows1 {
            self.lexer.advance(&_r)
        } else {
            return Ok(None)
        }

        let throwsType = self.parse_typespec(&_r, _rp, _ep)?;
        match throwsType {
            None =>
                return Err(Ref::new(_ep, ParserError { file_name: String::copy(_ep, self.file_name), line: self.lexer.line, column: self.lexer.column })),
            Some(_) => ()
        }

        let end: Position = self.lexer.get_position();

        let ret = Ref::new(_rp, ThrowsSyntax { parent: null_mut(), start: start, end: end, throwsType: throwsType.unwrap() });

        match *throwsType.unwrap() {
            TypeSpecSyntax::Type(x) => x.parent = ret.data as *mut SyntaxNode,
            TypeSpecSyntax::Variant(x) => x.parent = ret.data as *mut SyntaxNode,
        }

        Ok(Some(ret))
    }

    pub fn parse_genericarguments(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Option<Ref<GenericArgumentsSyntax>>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        let start: Position = self.lexer.get_previous_position();

        let successLeftBracket1 = self.lexer.parse_punctuation(String::from_string_slice(_r.page, "["));
        if successLeftBracket1 {
            self.lexer.advance(&_r)
        } else {
            return Ok(None)
        }

        let generic = self.parse_type(&_r, _rp, _ep)?;
        match generic {
            None =>
                return Err(Ref::new(_ep, ParserError { file_name: String::copy(_ep, self.file_name), line: self.lexer.line, column: self.lexer.column })),
            Some(_) => ()
        }

        let additionalGenerics = self.parse_genericargument_list(&_r, _rp, _ep)?;

        let successRightBracket4 = self.lexer.parse_punctuation(String::from_string_slice(_r.page, "]"));
        if successRightBracket4 {
            self.lexer.advance(&_r)
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

        let ret = Ref::new(_rp, GenericArgumentsSyntax { parent: null_mut(), start: start, end: end, generic: generic.unwrap(), additionalGenerics: additionalGenerics });

        generic.unwrap().parent = ret.data as *mut SyntaxNode;

        match additionalGenerics {
            Some(x) => for item in x.iter() {
                item.parent = ret.data as *mut SyntaxNode;
            }
            None => ()
        }

        Ok(Some(ret))
    }

    pub fn parse_genericargument_list(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Option<Ref<Vector<Ref<GenericArgumentSyntax>>>>, Ref<ParserError>> {
        let _r = Region::create(_pr);
        let mut ret: Option<Ref<Array<Ref<GenericArgumentSyntax>>>> = Option::None;
        loop {
            let node = self.parse_genericargument(&_r, _rp, _ep)?;
            match node {
                None => break,
                Some(node) => {
                    match ret {
                        None => ret = Some(Ref::new(_rp, Array::new())),
                        Some(_) => (),
                    };
                    ret.unwrap().add(node);
                }
            }
        }

        match ret {
            Some(ret) => Ok(Some(Ref::new(_rp, Vector::from_array(_rp, ret)))),
            None => Ok(None),
        }
    }

    pub fn parse_genericargument(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Option<Ref<GenericArgumentSyntax>>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        let start: Position = self.lexer.get_previous_position();

        let successComma1 = self.lexer.parse_punctuation(String::from_string_slice(_r.page, ","));
        if successComma1 {
            self.lexer.advance(&_r)
        } else {
            return Ok(None)
        }

        let typeSpec = self.parse_type(&_r, _rp, _ep)?;
        match typeSpec {
            None =>
                return Err(Ref::new(_ep, ParserError { file_name: String::copy(_ep, self.file_name), line: self.lexer.line, column: self.lexer.column })),
            Some(_) => ()
        }

        let end: Position = self.lexer.get_position();

        let ret = Ref::new(_rp, GenericArgumentSyntax { parent: null_mut(), start: start, end: end, typeSpec: typeSpec.unwrap() });

        typeSpec.unwrap().parent = ret.data as *mut SyntaxNode;

        Ok(Some(ret))
    }

    pub fn parse_optional(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Option<Ref<OptionalSyntax>>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        let start: Position = self.lexer.get_previous_position();

        let successQuestion1 = self.lexer.parse_punctuation(String::from_string_slice(_r.page, "?"));
        if successQuestion1 {
            self.lexer.advance(&_r)
        } else {
            return Ok(None)
        }

        let end: Position = self.lexer.get_position();

        let ret = Ref::new(_rp, OptionalSyntax { parent: null_mut(), start: start, end: end });

        Ok(Some(ret))
    }

    pub fn parse_lifetime(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Option<Ref<LifeTimeSyntax>>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        {
            let node = self.parse_root(&_r, _rp, _ep)?;
            match node {
                Some(it) => return Ok(Some(Ref::new(_rp, LifeTimeSyntax::Root(*it)))),
                None => ()
            }
        }

        {
            let node = self.parse_local(&_r, _rp, _ep)?;
            match node {
                Some(it) => return Ok(Some(Ref::new(_rp, LifeTimeSyntax::Local(*it)))),
                None => ()
            }
        }

        {
            let node = self.parse_reference(&_r, _rp, _ep)?;
            match node {
                Some(it) => return Ok(Some(Ref::new(_rp, LifeTimeSyntax::Reference(*it)))),
                None => ()
            }
        }

        {
            let node = self.parse_thrown(&_r, _rp, _ep)?;
            match node {
                Some(it) => return Ok(Some(Ref::new(_rp, LifeTimeSyntax::Thrown(*it)))),
                None => ()
            }
        }

        return Ok(None)
    }

    pub fn parse_root(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Option<Ref<RootSyntax>>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        let start: Position = self.lexer.get_previous_position();

        let successDollar1 = self.lexer.parse_punctuation(String::from_string_slice(_r.page, "$"));
        if successDollar1 {
            self.lexer.advance(&_r)
        } else {
            return Ok(None)
        }

        let end: Position = self.lexer.get_position();

        let ret = Ref::new(_rp, RootSyntax { parent: null_mut(), start: start, end: end });

        Ok(Some(ret))
    }

    pub fn parse_local(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Option<Ref<LocalSyntax>>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        let start: Position = self.lexer.get_previous_position();

        let successAt1 = self.lexer.parse_punctuation(String::from_string_slice(_r.page, "@"));
        if successAt1 {
            self.lexer.advance(&_r)
        } else {
            return Ok(None)
        }

        let location = self.lexer.parse_identifier(_rp);
        match location {
            Some(location) => if self.is_identifier(location) {
                self.lexer.advance(&_r)
            } else {

            return Result::Err(Ref::new(
                _ep,
                ParserError {
                    file_name: self.file_name,
                    line: self.lexer.line,
                    column: self.lexer.column,
                },
            ))
           },
            None => 
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

        let ret = Ref::new(_rp, LocalSyntax { parent: null_mut(), start: start, end: end, location: location.unwrap() });

        Ok(Some(ret))
    }

    pub fn parse_reference(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Option<Ref<ReferenceSyntax>>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        let start: Position = self.lexer.get_previous_position();

        let successBacktick1 = self.lexer.parse_punctuation(String::from_string_slice(_r.page, "`"));
        if successBacktick1 {
            self.lexer.advance(&_r)
        } else {
            return Ok(None)
        }

        let age = self.lexer.parse_literal(_rp);
        match age {
            Some(x) =>
                self.lexer.advance(&_r),
            None => ()
        }

        let end: Position = self.lexer.get_position();

        let ret = Ref::new(_rp, ReferenceSyntax { parent: null_mut(), start: start, end: end, age: age });

        Ok(Some(ret))
    }

    pub fn parse_thrown(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Option<Ref<ThrownSyntax>>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        let start: Position = self.lexer.get_previous_position();

        let successHash1 = self.lexer.parse_punctuation(String::from_string_slice(_r.page, "#"));
        if successHash1 {
            self.lexer.advance(&_r)
        } else {
            return Ok(None)
        }

        let end: Position = self.lexer.get_position();

        let ret = Ref::new(_rp, ThrownSyntax { parent: null_mut(), start: start, end: end });

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
    fn openProgram(&mut self, programSyntax: Ref<ProgramSyntax>) -> bool;
    fn closeProgram(&mut self, programSyntax: Ref<ProgramSyntax>);

    fn openFile(&mut self, fileSyntax: Ref<FileSyntax>) -> bool;
    fn closeFile(&mut self, fileSyntax: Ref<FileSyntax>);

    fn visitIntrinsic(&mut self, intrinsicSyntax: Ref<IntrinsicSyntax>);

    fn openUsing(&mut self, usingSyntax: Ref<UsingSyntax>) -> bool;
    fn closeUsing(&mut self, usingSyntax: Ref<UsingSyntax>);

    fn openDefine(&mut self, defineSyntax: Ref<DefineSyntax>) -> bool;
    fn closeDefine(&mut self, defineSyntax: Ref<DefineSyntax>);

    fn openName(&mut self, nameSyntax: Ref<NameSyntax>) -> bool;
    fn closeName(&mut self, nameSyntax: Ref<NameSyntax>);

    fn visitExtension(&mut self, extensionSyntax: Ref<ExtensionSyntax>);

    fn openNamespace(&mut self, namespaceSyntax: Ref<NamespaceSyntax>) -> bool;
    fn closeNamespace(&mut self, namespaceSyntax: Ref<NamespaceSyntax>);

    fn openFunction(&mut self, functionSyntax: Ref<FunctionSyntax>) -> bool;
    fn closeFunction(&mut self, functionSyntax: Ref<FunctionSyntax>);

    fn openProcedure(&mut self, procedureSyntax: Ref<ProcedureSyntax>) -> bool;
    fn closeProcedure(&mut self, procedureSyntax: Ref<ProcedureSyntax>);

    fn openRoutine(&mut self, routineSyntax: Ref<RoutineSyntax>) -> bool;
    fn closeRoutine(&mut self, routineSyntax: Ref<RoutineSyntax>);

    fn openLetDeclaration(&mut self, letDeclarationSyntax: Ref<LetDeclarationSyntax>) -> bool;
    fn closeLetDeclaration(&mut self, letDeclarationSyntax: Ref<LetDeclarationSyntax>);

    fn openVarDeclaration(&mut self, varDeclarationSyntax: Ref<VarDeclarationSyntax>) -> bool;
    fn closeVarDeclaration(&mut self, varDeclarationSyntax: Ref<VarDeclarationSyntax>);

    fn openMutableDeclaration(&mut self, mutableDeclarationSyntax: Ref<MutableDeclarationSyntax>) -> bool;
    fn closeMutableDeclaration(&mut self, mutableDeclarationSyntax: Ref<MutableDeclarationSyntax>);

    fn openThreadLocalDeclaration(&mut self, threadLocalDeclarationSyntax: Ref<ThreadLocalDeclarationSyntax>) -> bool;
    fn closeThreadLocalDeclaration(&mut self, threadLocalDeclarationSyntax: Ref<ThreadLocalDeclarationSyntax>);

    fn openLet(&mut self, letSyntax: Ref<LetSyntax>) -> bool;
    fn closeLet(&mut self, letSyntax: Ref<LetSyntax>);

    fn openVar(&mut self, varSyntax: Ref<VarSyntax>) -> bool;
    fn closeVar(&mut self, varSyntax: Ref<VarSyntax>);

    fn openMutable(&mut self, mutableSyntax: Ref<MutableSyntax>) -> bool;
    fn closeMutable(&mut self, mutableSyntax: Ref<MutableSyntax>);

    fn openThreadLocal(&mut self, threadLocalSyntax: Ref<ThreadLocalSyntax>) -> bool;
    fn closeThreadLocal(&mut self, threadLocalSyntax: Ref<ThreadLocalSyntax>);

    fn openBinding(&mut self, bindingSyntax: Ref<BindingSyntax>) -> bool;
    fn closeBinding(&mut self, bindingSyntax: Ref<BindingSyntax>);

    fn openSet(&mut self, setSyntax: Ref<SetSyntax>) -> bool;
    fn closeSet(&mut self, setSyntax: Ref<SetSyntax>);

    fn openCalculation(&mut self, calculationSyntax: Ref<CalculationSyntax>) -> bool;
    fn closeCalculation(&mut self, calculationSyntax: Ref<CalculationSyntax>);

    fn openOperation(&mut self, operationSyntax: Ref<OperationSyntax>) -> bool;
    fn closeOperation(&mut self, operationSyntax: Ref<OperationSyntax>);

    fn openOperand(&mut self, operandSyntax: Ref<OperandSyntax>) -> bool;
    fn closeOperand(&mut self, operandSyntax: Ref<OperandSyntax>);

    fn visitMemberAccess(&mut self, memberAccessSyntax: Ref<MemberAccessSyntax>);

    fn openAs(&mut self, asSyntax: Ref<AsSyntax>) -> bool;
    fn closeAs(&mut self, asSyntax: Ref<AsSyntax>);

    fn openIs(&mut self, isSyntax: Ref<IsSyntax>) -> bool;
    fn closeIs(&mut self, isSyntax: Ref<IsSyntax>);

    fn visitUnwrap(&mut self, unwrapSyntax: Ref<UnwrapSyntax>);

    fn openCatch(&mut self, catchSyntax: Ref<CatchSyntax>) -> bool;
    fn closeCatch(&mut self, catchSyntax: Ref<CatchSyntax>);

    fn openWildCardCatchPattern(&mut self, wildCardCatchPatternSyntax: Ref<WildCardCatchPatternSyntax>) -> bool;
    fn closeWildCardCatchPattern(&mut self, wildCardCatchPatternSyntax: Ref<WildCardCatchPatternSyntax>);

    fn openTypeCatchPattern(&mut self, typeCatchPatternSyntax: Ref<TypeCatchPatternSyntax>) -> bool;
    fn closeTypeCatchPattern(&mut self, typeCatchPatternSyntax: Ref<TypeCatchPatternSyntax>);

    fn openBlock(&mut self, blockSyntax: Ref<BlockSyntax>) -> bool;
    fn closeBlock(&mut self, blockSyntax: Ref<BlockSyntax>);

    fn visitConstant(&mut self, constantSyntax: Ref<ConstantSyntax>);

    fn openIf(&mut self, ifSyntax: Ref<IfSyntax>) -> bool;
    fn closeIf(&mut self, ifSyntax: Ref<IfSyntax>);

    fn openElse(&mut self, elseSyntax: Ref<ElseSyntax>) -> bool;
    fn closeElse(&mut self, elseSyntax: Ref<ElseSyntax>);

    fn openSwitch(&mut self, switchSyntax: Ref<SwitchSyntax>) -> bool;
    fn closeSwitch(&mut self, switchSyntax: Ref<SwitchSyntax>);

    fn openSwitchCase(&mut self, switchCaseSyntax: Ref<SwitchCaseSyntax>) -> bool;
    fn closeSwitchCase(&mut self, switchCaseSyntax: Ref<SwitchCaseSyntax>);

    fn openItemCaseLabel(&mut self, itemCaseLabelSyntax: Ref<ItemCaseLabelSyntax>) -> bool;
    fn closeItemCaseLabel(&mut self, itemCaseLabelSyntax: Ref<ItemCaseLabelSyntax>);

    fn openCaseItem(&mut self, caseItemSyntax: Ref<CaseItemSyntax>) -> bool;
    fn closeCaseItem(&mut self, caseItemSyntax: Ref<CaseItemSyntax>);

    fn openConstantPattern(&mut self, constantPatternSyntax: Ref<ConstantPatternSyntax>) -> bool;
    fn closeConstantPattern(&mut self, constantPatternSyntax: Ref<ConstantPatternSyntax>);

    fn visitWildcardPattern(&mut self, wildcardPatternSyntax: Ref<WildcardPatternSyntax>);

    fn openNamePattern(&mut self, namePatternSyntax: Ref<NamePatternSyntax>) -> bool;
    fn closeNamePattern(&mut self, namePatternSyntax: Ref<NamePatternSyntax>);

    fn visitDefaultCaseLabel(&mut self, defaultCaseLabelSyntax: Ref<DefaultCaseLabelSyntax>);

    fn openFor(&mut self, forSyntax: Ref<ForSyntax>) -> bool;
    fn closeFor(&mut self, forSyntax: Ref<ForSyntax>);

    fn openWhile(&mut self, whileSyntax: Ref<WhileSyntax>) -> bool;
    fn closeWhile(&mut self, whileSyntax: Ref<WhileSyntax>);

    fn openDo(&mut self, doSyntax: Ref<DoSyntax>) -> bool;
    fn closeDo(&mut self, doSyntax: Ref<DoSyntax>);

    fn openSimpleLoop(&mut self, simpleLoopSyntax: Ref<SimpleLoopSyntax>) -> bool;
    fn closeSimpleLoop(&mut self, simpleLoopSyntax: Ref<SimpleLoopSyntax>);

    fn openNamedLoop(&mut self, namedLoopSyntax: Ref<NamedLoopSyntax>) -> bool;
    fn closeNamedLoop(&mut self, namedLoopSyntax: Ref<NamedLoopSyntax>);

    fn visitThis(&mut self, thisSyntax: Ref<ThisSyntax>);

    fn openNew(&mut self, newSyntax: Ref<NewSyntax>) -> bool;
    fn closeNew(&mut self, newSyntax: Ref<NewSyntax>);

    fn openObject(&mut self, objectSyntax: Ref<ObjectSyntax>) -> bool;
    fn closeObject(&mut self, objectSyntax: Ref<ObjectSyntax>);

    fn openArray(&mut self, arraySyntax: Ref<ArraySyntax>) -> bool;
    fn closeArray(&mut self, arraySyntax: Ref<ArraySyntax>);

    fn openItem(&mut self, itemSyntax: Ref<ItemSyntax>) -> bool;
    fn closeItem(&mut self, itemSyntax: Ref<ItemSyntax>);

    fn openSizeOf(&mut self, sizeOfSyntax: Ref<SizeOfSyntax>) -> bool;
    fn closeSizeOf(&mut self, sizeOfSyntax: Ref<SizeOfSyntax>);

    fn visitBreak(&mut self, breakSyntax: Ref<BreakSyntax>);

    fn visitContinue(&mut self, continueSyntax: Ref<ContinueSyntax>);

    fn openReturn(&mut self, returnSyntax: Ref<ReturnSyntax>) -> bool;
    fn closeReturn(&mut self, returnSyntax: Ref<ReturnSyntax>);

    fn openThrow(&mut self, throwSyntax: Ref<ThrowSyntax>) -> bool;
    fn closeThrow(&mut self, throwSyntax: Ref<ThrowSyntax>);

    fn openClass(&mut self, classSyntax: Ref<ClassSyntax>) -> bool;
    fn closeClass(&mut self, classSyntax: Ref<ClassSyntax>);

    fn openGenericParameters(&mut self, genericParametersSyntax: Ref<GenericParametersSyntax>) -> bool;
    fn closeGenericParameters(&mut self, genericParametersSyntax: Ref<GenericParametersSyntax>);

    fn visitGenericParameter(&mut self, genericParameterSyntax: Ref<GenericParameterSyntax>);

    fn openExtends(&mut self, extendsSyntax: Ref<ExtendsSyntax>) -> bool;
    fn closeExtends(&mut self, extendsSyntax: Ref<ExtendsSyntax>);

    fn openStructure(&mut self, structureSyntax: Ref<StructureSyntax>) -> bool;
    fn closeStructure(&mut self, structureSyntax: Ref<StructureSyntax>);

    fn openComponent(&mut self, componentSyntax: Ref<ComponentSyntax>) -> bool;
    fn closeComponent(&mut self, componentSyntax: Ref<ComponentSyntax>);

    fn openClassBody(&mut self, classBodySyntax: Ref<ClassBodySyntax>) -> bool;
    fn closeClassBody(&mut self, classBodySyntax: Ref<ClassBodySyntax>);

    fn openLetMember(&mut self, letMemberSyntax: Ref<LetMemberSyntax>) -> bool;
    fn closeLetMember(&mut self, letMemberSyntax: Ref<LetMemberSyntax>);

    fn openVarMember(&mut self, varMemberSyntax: Ref<VarMemberSyntax>) -> bool;
    fn closeVarMember(&mut self, varMemberSyntax: Ref<VarMemberSyntax>);

    fn openMutableMember(&mut self, mutableMemberSyntax: Ref<MutableMemberSyntax>) -> bool;
    fn closeMutableMember(&mut self, mutableMemberSyntax: Ref<MutableMemberSyntax>);

    fn openSetInitialization(&mut self, setInitializationSyntax: Ref<SetInitializationSyntax>) -> bool;
    fn closeSetInitialization(&mut self, setInitializationSyntax: Ref<SetInitializationSyntax>);

    fn openMethod(&mut self, methodSyntax: Ref<MethodSyntax>) -> bool;
    fn closeMethod(&mut self, methodSyntax: Ref<MethodSyntax>);

    fn openStaticFunction(&mut self, staticFunctionSyntax: Ref<StaticFunctionSyntax>) -> bool;
    fn closeStaticFunction(&mut self, staticFunctionSyntax: Ref<StaticFunctionSyntax>);

    fn openOperator(&mut self, operatorSyntax: Ref<OperatorSyntax>) -> bool;
    fn closeOperator(&mut self, operatorSyntax: Ref<OperatorSyntax>);

    fn openInitializer(&mut self, initializerSyntax: Ref<InitializerSyntax>) -> bool;
    fn closeInitializer(&mut self, initializerSyntax: Ref<InitializerSyntax>);

    fn openAllocator(&mut self, allocatorSyntax: Ref<AllocatorSyntax>) -> bool;
    fn closeAllocator(&mut self, allocatorSyntax: Ref<AllocatorSyntax>);

    fn openTypeAnnotation(&mut self, typeAnnotationSyntax: Ref<TypeAnnotationSyntax>) -> bool;
    fn closeTypeAnnotation(&mut self, typeAnnotationSyntax: Ref<TypeAnnotationSyntax>);

    fn openType(&mut self, typeSyntax: Ref<TypeSyntax>) -> bool;
    fn closeType(&mut self, typeSyntax: Ref<TypeSyntax>);

    fn openVariant(&mut self, variantSyntax: Ref<VariantSyntax>) -> bool;
    fn closeVariant(&mut self, variantSyntax: Ref<VariantSyntax>);

    fn openThrows(&mut self, throwsSyntax: Ref<ThrowsSyntax>) -> bool;
    fn closeThrows(&mut self, throwsSyntax: Ref<ThrowsSyntax>);

    fn openGenericArguments(&mut self, genericArgumentsSyntax: Ref<GenericArgumentsSyntax>) -> bool;
    fn closeGenericArguments(&mut self, genericArgumentsSyntax: Ref<GenericArgumentsSyntax>);

    fn openGenericArgument(&mut self, genericArgumentSyntax: Ref<GenericArgumentSyntax>) -> bool;
    fn closeGenericArgument(&mut self, genericArgumentSyntax: Ref<GenericArgumentSyntax>);

    fn visitOptional(&mut self, optionalSyntax: Ref<OptionalSyntax>);

    fn visitRoot(&mut self, rootSyntax: Ref<RootSyntax>);

    fn visitLocal(&mut self, localSyntax: Ref<LocalSyntax>);

    fn visitReference(&mut self, referenceSyntax: Ref<ReferenceSyntax>);

    fn visitThrown(&mut self, thrownSyntax: Ref<ThrownSyntax>);
}

pub trait SyntaxNode {
    fn accept(&mut self, visitor: *mut SyntaxVisitor);
}

#[derive(Copy, Clone)]
pub struct ProgramSyntax {
    pub start: Position,
    pub end: Position,
    pub parent: *mut SyntaxNode,
    pub name: String,
    pub files: Ref<Vector<Ref<FileSyntax>>>,
}

impl SyntaxNode for ProgramSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        if !(*visitor).openProgram(self) {
            return
        }
        match self.files {
            Some(x) => for node in files {
                node.accept(visitor)
            },
            None => ()
        };
        visitor.closeProgram(self)
    }
}

#[derive(Copy, Clone)]
pub struct FileSyntax {
    pub file_name: String,
    pub intrinsics: Option<Ref<Vector<Ref<IntrinsicSyntax>>>>,
    pub usings: Option<Ref<Vector<Ref<UsingSyntax>>>>,
    pub defines: Option<Ref<Vector<Ref<DefineSyntax>>>>,
    pub declarations: Option<Ref<Vector<Ref<DeclarationSyntax>>>>,
    pub statements: Option<Ref<Vector<Ref<StatementSyntax>>>>,
}

impl SyntaxNode for FileSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        if !(*visitor).openFile(self) {
            return
        }
        match self.intrinsics {
            Some(x) => for node in intrinsics {
                node.accept(visitor)
            },
            None => ()
        };

        match self.usings {
            Some(x) => for node in usings {
                node.accept(visitor)
            },
            None => ()
        };

        match self.defines {
            Some(x) => for node in defines {
                node.accept(visitor)
            },
            None => ()
        };

        match self.declarations {
            Some(x) => for node in declarations {
                node.accept(visitor)
            },
            None => ()
        };

        match self.statements {
            Some(x) => for node in statements {
                node.accept(visitor)
            },
            None => ()
        };
        visitor.closeFile(self)
    }
}

#[derive(Copy, Clone)]
pub struct IntrinsicSyntax {
    pub start: Position,
    pub end: Position,
    pub parent: *mut SyntaxNode,
    pub name: String,
}

impl SyntaxNode for IntrinsicSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        visitor.visitIntrinsic(self)
    }
}

#[derive(Copy, Clone)]
pub struct UsingSyntax {
    pub start: Position,
    pub end: Position,
    pub parent: *mut SyntaxNode,
    pub name: Ref<NameSyntax>,
}

impl SyntaxNode for UsingSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        if !(*visitor).openUsing(self) {
            return
        }
        name.accept(visitor);
        visitor.closeUsing(self)
    }
}

#[derive(Copy, Clone)]
pub struct DefineSyntax {
    pub start: Position,
    pub end: Position,
    pub parent: *mut SyntaxNode,
    pub name: Ref<NameSyntax>,
    pub typeSpec: Ref<TypeSyntax>,
}

impl SyntaxNode for DefineSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        if !(*visitor).openDefine(self) {
            return
        }
        name.accept(visitor);

        typeSpec.accept(visitor);
        visitor.closeDefine(self)
    }
}

#[derive(Copy, Clone)]
pub struct NameSyntax {
    pub start: Position,
    pub end: Position,
    pub parent: *mut SyntaxNode,
    pub name: String,
    pub extensions: Option<Ref<Vector<Ref<ExtensionSyntax>>>>,
}

impl SyntaxNode for NameSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        if !(*visitor).openName(self) {
            return
        }
        match self.extensions {
            Some(x) => for node in extensions {
                node.accept(visitor)
            },
            None => ()
        };
        visitor.closeName(self)
    }
}

#[derive(Copy, Clone)]
pub struct ExtensionSyntax {
    pub start: Position,
    pub end: Position,
    pub parent: *mut SyntaxNode,
    pub name: String,
}

impl SyntaxNode for ExtensionSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        visitor.visitExtension(self)
    }
}

#[derive(Copy, Clone)]
pub enum DeclarationSyntax {
   Namespace(NamespaceSyntax),
   Function(FunctionSyntax),
   Class(ClassSyntax),
   LetDeclaration(LetDeclarationSyntax),
   VarDeclaration(VarDeclarationSyntax),
   MutableDeclaration(MutableDeclarationSyntax),
   ThreadLocalDeclaration(ThreadLocalDeclarationSyntax),
}

impl SyntaxNode for DeclarationSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
    }
}

#[derive(Copy, Clone)]
pub struct NamespaceSyntax {
    pub start: Position,
    pub end: Position,
    pub parent: *mut SyntaxNode,
    pub name: Ref<NameSyntax>,
    pub usings: Option<Ref<Vector<Ref<UsingSyntax>>>>,
    pub defines: Option<Ref<Vector<Ref<DefineSyntax>>>>,
    pub declarations: Option<Ref<Vector<Ref<DeclarationSyntax>>>>,
}

impl SyntaxNode for NamespaceSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        if !(*visitor).openNamespace(self) {
            return
        }
        name.accept(visitor);

        match self.usings {
            Some(x) => for node in usings {
                node.accept(visitor)
            },
            None => ()
        };

        match self.defines {
            Some(x) => for node in defines {
                node.accept(visitor)
            },
            None => ()
        };

        match self.declarations {
            Some(x) => for node in declarations {
                node.accept(visitor)
            },
            None => ()
        };
        visitor.closeNamespace(self)
    }
}

#[derive(Copy, Clone)]
pub struct FunctionSyntax {
    pub start: Position,
    pub end: Position,
    pub parent: *mut SyntaxNode,
    pub procedure: Ref<ProcedureSyntax>,
}

impl SyntaxNode for FunctionSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        if !(*visitor).openFunction(self) {
            return
        }
        procedure.accept(visitor);
        visitor.closeFunction(self)
    }
}

#[derive(Copy, Clone)]
pub struct ProcedureSyntax {
    pub start: Position,
    pub end: Position,
    pub parent: *mut SyntaxNode,
    pub name: String,
    pub routine: Ref<RoutineSyntax>,
}

impl SyntaxNode for ProcedureSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        if !(*visitor).openProcedure(self) {
            return
        }
        routine.accept(visitor);
        visitor.closeProcedure(self)
    }
}

#[derive(Copy, Clone)]
pub struct RoutineSyntax {
    pub start: Position,
    pub end: Position,
    pub parent: *mut SyntaxNode,
    pub input: Option<Ref<StructureSyntax>>,
    pub output: Option<Ref<TypeAnnotationSyntax>>,
    pub throwsClause: Option<Ref<ThrowsSyntax>>,
    pub body: Ref<BlockSyntax>,
}

impl SyntaxNode for RoutineSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        if !(*visitor).openRoutine(self) {
            return
        }
        match self.input {
            Some(x) => input.accept(visitor),
            None => ()
        };

        match self.output {
            Some(x) => output.accept(visitor),
            None => ()
        };

        match self.throwsClause {
            Some(x) => throwsClause.accept(visitor),
            None => ()
        };

        body.accept(visitor);
        visitor.closeRoutine(self)
    }
}

#[derive(Copy, Clone)]
pub struct LetDeclarationSyntax {
    pub start: Position,
    pub end: Position,
    pub parent: *mut SyntaxNode,
    pub declaration: Ref<LetSyntax>,
}

impl SyntaxNode for LetDeclarationSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        if !(*visitor).openLetDeclaration(self) {
            return
        }
        declaration.accept(visitor);
        visitor.closeLetDeclaration(self)
    }
}

#[derive(Copy, Clone)]
pub struct VarDeclarationSyntax {
    pub start: Position,
    pub end: Position,
    pub parent: *mut SyntaxNode,
    pub declaration: Ref<VarSyntax>,
}

impl SyntaxNode for VarDeclarationSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        if !(*visitor).openVarDeclaration(self) {
            return
        }
        declaration.accept(visitor);
        visitor.closeVarDeclaration(self)
    }
}

#[derive(Copy, Clone)]
pub struct MutableDeclarationSyntax {
    pub start: Position,
    pub end: Position,
    pub parent: *mut SyntaxNode,
    pub declaration: Ref<MutableSyntax>,
}

impl SyntaxNode for MutableDeclarationSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        if !(*visitor).openMutableDeclaration(self) {
            return
        }
        declaration.accept(visitor);
        visitor.closeMutableDeclaration(self)
    }
}

#[derive(Copy, Clone)]
pub struct ThreadLocalDeclarationSyntax {
    pub start: Position,
    pub end: Position,
    pub parent: *mut SyntaxNode,
    pub declaration: Ref<ThreadLocalSyntax>,
}

impl SyntaxNode for ThreadLocalDeclarationSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        if !(*visitor).openThreadLocalDeclaration(self) {
            return
        }
        declaration.accept(visitor);
        visitor.closeThreadLocalDeclaration(self)
    }
}

#[derive(Copy, Clone)]
pub enum StatementSyntax {
   Let(LetSyntax),
   Var(VarSyntax),
   Mutable(MutableSyntax),
   ThreadLocal(ThreadLocalSyntax),
   Set(SetSyntax),
   Calculation(CalculationSyntax),
   Break(BreakSyntax),
   Continue(ContinueSyntax),
   Return(ReturnSyntax),
   Throw(ThrowSyntax),
}

impl SyntaxNode for StatementSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
    }
}

#[derive(Copy, Clone)]
pub struct LetSyntax {
    pub start: Position,
    pub end: Position,
    pub parent: *mut SyntaxNode,
    pub binding: Ref<BindingSyntax>,
}

impl SyntaxNode for LetSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        if !(*visitor).openLet(self) {
            return
        }
        binding.accept(visitor);
        visitor.closeLet(self)
    }
}

#[derive(Copy, Clone)]
pub struct VarSyntax {
    pub start: Position,
    pub end: Position,
    pub parent: *mut SyntaxNode,
    pub binding: Ref<BindingSyntax>,
}

impl SyntaxNode for VarSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        if !(*visitor).openVar(self) {
            return
        }
        binding.accept(visitor);
        visitor.closeVar(self)
    }
}

#[derive(Copy, Clone)]
pub struct MutableSyntax {
    pub start: Position,
    pub end: Position,
    pub parent: *mut SyntaxNode,
    pub binding: Ref<BindingSyntax>,
}

impl SyntaxNode for MutableSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        if !(*visitor).openMutable(self) {
            return
        }
        binding.accept(visitor);
        visitor.closeMutable(self)
    }
}

#[derive(Copy, Clone)]
pub struct ThreadLocalSyntax {
    pub start: Position,
    pub end: Position,
    pub parent: *mut SyntaxNode,
    pub binding: Ref<BindingSyntax>,
}

impl SyntaxNode for ThreadLocalSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        if !(*visitor).openThreadLocal(self) {
            return
        }
        binding.accept(visitor);
        visitor.closeThreadLocal(self)
    }
}

#[derive(Copy, Clone)]
pub struct BindingSyntax {
    pub start: Position,
    pub end: Position,
    pub parent: *mut SyntaxNode,
    pub name: String,
    pub typeAnnotation: Option<Ref<TypeAnnotationSyntax>>,
    pub calculation: Ref<CalculationSyntax>,
}

impl SyntaxNode for BindingSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        if !(*visitor).openBinding(self) {
            return
        }
        match self.typeAnnotation {
            Some(x) => typeAnnotation.accept(visitor),
            None => ()
        };

        calculation.accept(visitor);
        visitor.closeBinding(self)
    }
}

#[derive(Copy, Clone)]
pub struct SetSyntax {
    pub start: Position,
    pub end: Position,
    pub parent: *mut SyntaxNode,
    pub lValue: Ref<OperationSyntax>,
    pub rValue: Ref<CalculationSyntax>,
}

impl SyntaxNode for SetSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        if !(*visitor).openSet(self) {
            return
        }
        lValue.accept(visitor);

        rValue.accept(visitor);
        visitor.closeSet(self)
    }
}

#[derive(Copy, Clone)]
pub struct CalculationSyntax {
    pub start: Position,
    pub end: Position,
    pub parent: *mut SyntaxNode,
    pub operation: Ref<OperationSyntax>,
}

impl SyntaxNode for CalculationSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        if !(*visitor).openCalculation(self) {
            return
        }
        operation.accept(visitor);
        visitor.closeCalculation(self)
    }
}

#[derive(Copy, Clone)]
pub struct OperationSyntax {
    pub start: Position,
    pub end: Position,
    pub parent: *mut SyntaxNode,
    pub op: Ref<Vector<Ref<OperandSyntax>>>,
}

impl SyntaxNode for OperationSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        if !(*visitor).openOperation(self) {
            return
        }
        match self.op {
            Some(x) => for node in op {
                node.accept(visitor)
            },
            None => ()
        };
        visitor.closeOperation(self)
    }
}

#[derive(Copy, Clone)]
pub struct OperandSyntax {
    pub start: Position,
    pub end: Position,
    pub parent: *mut SyntaxNode,
    pub primary: Ref<ExpressionSyntax>,
    pub postfixes: Option<Ref<Vector<Ref<PostfixSyntax>>>>,
}

impl SyntaxNode for OperandSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        if !(*visitor).openOperand(self) {
            return
        }
        primary.accept(visitor);

        match self.postfixes {
            Some(x) => for node in postfixes {
                node.accept(visitor)
            },
            None => ()
        };
        visitor.closeOperand(self)
    }
}

#[derive(Copy, Clone)]
pub enum PostfixSyntax {
   MemberAccess(MemberAccessSyntax),
   As(AsSyntax),
   Is(IsSyntax),
   Unwrap(UnwrapSyntax),
   Catch(CatchSyntax),
}

impl SyntaxNode for PostfixSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
    }
}

#[derive(Copy, Clone)]
pub struct MemberAccessSyntax {
    pub start: Position,
    pub end: Position,
    pub parent: *mut SyntaxNode,
    pub member: String,
}

impl SyntaxNode for MemberAccessSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        visitor.visitMemberAccess(self)
    }
}

#[derive(Copy, Clone)]
pub struct AsSyntax {
    pub start: Position,
    pub end: Position,
    pub parent: *mut SyntaxNode,
    pub typeSpec: Ref<TypeSpecSyntax>,
}

impl SyntaxNode for AsSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        if !(*visitor).openAs(self) {
            return
        }
        typeSpec.accept(visitor);
        visitor.closeAs(self)
    }
}

#[derive(Copy, Clone)]
pub struct IsSyntax {
    pub start: Position,
    pub end: Position,
    pub parent: *mut SyntaxNode,
    pub typeSpec: Ref<TypeSpecSyntax>,
}

impl SyntaxNode for IsSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        if !(*visitor).openIs(self) {
            return
        }
        typeSpec.accept(visitor);
        visitor.closeIs(self)
    }
}

#[derive(Copy, Clone)]
pub struct UnwrapSyntax {
    pub start: Position,
    pub end: Position,
    pub parent: *mut SyntaxNode,
}

impl SyntaxNode for UnwrapSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        visitor.visitUnwrap(self)
    }
}

#[derive(Copy, Clone)]
pub struct CatchSyntax {
    pub start: Position,
    pub end: Position,
    pub parent: *mut SyntaxNode,
    pub typeSpec: Ref<CatchPatternSyntax>,
    pub handler: Ref<BlockSyntax>,
}

impl SyntaxNode for CatchSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        if !(*visitor).openCatch(self) {
            return
        }
        typeSpec.accept(visitor);

        handler.accept(visitor);
        visitor.closeCatch(self)
    }
}

#[derive(Copy, Clone)]
pub enum CatchPatternSyntax {
   WildCardCatchPattern(WildCardCatchPatternSyntax),
   TypeCatchPattern(TypeCatchPatternSyntax),
}

impl SyntaxNode for CatchPatternSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
    }
}

#[derive(Copy, Clone)]
pub struct WildCardCatchPatternSyntax {
    pub start: Position,
    pub end: Position,
    pub parent: *mut SyntaxNode,
    pub pattern: Ref<WildcardPatternSyntax>,
}

impl SyntaxNode for WildCardCatchPatternSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        if !(*visitor).openWildCardCatchPattern(self) {
            return
        }
        pattern.accept(visitor);
        visitor.closeWildCardCatchPattern(self)
    }
}

#[derive(Copy, Clone)]
pub struct TypeCatchPatternSyntax {
    pub start: Position,
    pub end: Position,
    pub parent: *mut SyntaxNode,
    pub typeSpec: Ref<TypeSpecSyntax>,
    pub errorName: Option<String>,
}

impl SyntaxNode for TypeCatchPatternSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        if !(*visitor).openTypeCatchPattern(self) {
            return
        }
        typeSpec.accept(visitor);
        visitor.closeTypeCatchPattern(self)
    }
}

#[derive(Copy, Clone)]
pub enum ExpressionSyntax {
   Block(BlockSyntax),
   Name(NameSyntax),
   Constant(ConstantSyntax),
   If(IfSyntax),
   Switch(SwitchSyntax),
   For(ForSyntax),
   While(WhileSyntax),
   Do(DoSyntax),
   This(ThisSyntax),
   New(NewSyntax),
   Object(ObjectSyntax),
   Array(ArraySyntax),
   SizeOf(SizeOfSyntax),
}

impl SyntaxNode for ExpressionSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
    }
}

#[derive(Copy, Clone)]
pub struct BlockSyntax {
    pub start: Position,
    pub end: Position,
    pub parent: *mut SyntaxNode,
    pub statements: Option<Ref<Vector<Ref<StatementSyntax>>>>,
}

impl SyntaxNode for BlockSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        if !(*visitor).openBlock(self) {
            return
        }
        match self.statements {
            Some(x) => for node in statements {
                node.accept(visitor)
            },
            None => ()
        };
        visitor.closeBlock(self)
    }
}

#[derive(Copy, Clone)]
pub struct ConstantSyntax {
    pub start: Position,
    pub end: Position,
    pub parent: *mut SyntaxNode,
    pub literal: Literal,
}

impl SyntaxNode for ConstantSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        visitor.visitConstant(self)
    }
}

#[derive(Copy, Clone)]
pub struct IfSyntax {
    pub start: Position,
    pub end: Position,
    pub parent: *mut SyntaxNode,
    pub condition: Ref<OperationSyntax>,
    pub consequent: Ref<BlockSyntax>,
    pub elseClause: Option<Ref<ElseSyntax>>,
}

impl SyntaxNode for IfSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        if !(*visitor).openIf(self) {
            return
        }
        condition.accept(visitor);

        consequent.accept(visitor);

        match self.elseClause {
            Some(x) => elseClause.accept(visitor),
            None => ()
        };
        visitor.closeIf(self)
    }
}

#[derive(Copy, Clone)]
pub struct ElseSyntax {
    pub start: Position,
    pub end: Position,
    pub parent: *mut SyntaxNode,
    pub alternative: Ref<BlockSyntax>,
}

impl SyntaxNode for ElseSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        if !(*visitor).openElse(self) {
            return
        }
        alternative.accept(visitor);
        visitor.closeElse(self)
    }
}

#[derive(Copy, Clone)]
pub struct SwitchSyntax {
    pub start: Position,
    pub end: Position,
    pub parent: *mut SyntaxNode,
    pub condition: Ref<OperationSyntax>,
    pub cases: Ref<Vector<Ref<SwitchCaseSyntax>>>,
}

impl SyntaxNode for SwitchSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        if !(*visitor).openSwitch(self) {
            return
        }
        condition.accept(visitor);

        match self.cases {
            Some(x) => for node in cases {
                node.accept(visitor)
            },
            None => ()
        };
        visitor.closeSwitch(self)
    }
}

#[derive(Copy, Clone)]
pub struct SwitchCaseSyntax {
    pub start: Position,
    pub end: Position,
    pub parent: *mut SyntaxNode,
    pub label: Ref<CaseLabelSyntax>,
    pub content: Ref<BlockSyntax>,
}

impl SyntaxNode for SwitchCaseSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        if !(*visitor).openSwitchCase(self) {
            return
        }
        label.accept(visitor);

        content.accept(visitor);
        visitor.closeSwitchCase(self)
    }
}

#[derive(Copy, Clone)]
pub enum CaseLabelSyntax {
   ItemCaseLabel(ItemCaseLabelSyntax),
   DefaultCaseLabel(DefaultCaseLabelSyntax),
}

impl SyntaxNode for CaseLabelSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
    }
}

#[derive(Copy, Clone)]
pub struct ItemCaseLabelSyntax {
    pub start: Position,
    pub end: Position,
    pub parent: *mut SyntaxNode,
    pub items: Ref<Vector<Ref<CaseItemSyntax>>>,
}

impl SyntaxNode for ItemCaseLabelSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        if !(*visitor).openItemCaseLabel(self) {
            return
        }
        match self.items {
            Some(x) => for node in items {
                node.accept(visitor)
            },
            None => ()
        };
        visitor.closeItemCaseLabel(self)
    }
}

#[derive(Copy, Clone)]
pub struct CaseItemSyntax {
    pub start: Position,
    pub end: Position,
    pub parent: *mut SyntaxNode,
    pub pattern: Ref<CasePatternSyntax>,
}

impl SyntaxNode for CaseItemSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        if !(*visitor).openCaseItem(self) {
            return
        }
        pattern.accept(visitor);
        visitor.closeCaseItem(self)
    }
}

#[derive(Copy, Clone)]
pub enum CasePatternSyntax {
   ConstantPattern(ConstantPatternSyntax),
   WildcardPattern(WildcardPatternSyntax),
   NamePattern(NamePatternSyntax),
}

impl SyntaxNode for CasePatternSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
    }
}

#[derive(Copy, Clone)]
pub struct ConstantPatternSyntax {
    pub start: Position,
    pub end: Position,
    pub parent: *mut SyntaxNode,
    pub constant: Ref<ConstantSyntax>,
}

impl SyntaxNode for ConstantPatternSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        if !(*visitor).openConstantPattern(self) {
            return
        }
        constant.accept(visitor);
        visitor.closeConstantPattern(self)
    }
}

#[derive(Copy, Clone)]
pub struct WildcardPatternSyntax {
    pub start: Position,
    pub end: Position,
    pub parent: *mut SyntaxNode,
}

impl SyntaxNode for WildcardPatternSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        visitor.visitWildcardPattern(self)
    }
}

#[derive(Copy, Clone)]
pub struct NamePatternSyntax {
    pub start: Position,
    pub end: Position,
    pub parent: *mut SyntaxNode,
    pub name: Ref<NameSyntax>,
}

impl SyntaxNode for NamePatternSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        if !(*visitor).openNamePattern(self) {
            return
        }
        name.accept(visitor);
        visitor.closeNamePattern(self)
    }
}

#[derive(Copy, Clone)]
pub struct DefaultCaseLabelSyntax {
    pub start: Position,
    pub end: Position,
    pub parent: *mut SyntaxNode,
}

impl SyntaxNode for DefaultCaseLabelSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        visitor.visitDefaultCaseLabel(self)
    }
}

#[derive(Copy, Clone)]
pub struct ForSyntax {
    pub start: Position,
    pub end: Position,
    pub parent: *mut SyntaxNode,
    pub index: String,
    pub typeAnnotation: Option<Ref<TypeAnnotationSyntax>>,
    pub operation: Ref<OperationSyntax>,
    pub iteration: Ref<LoopSyntax>,
}

impl SyntaxNode for ForSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        if !(*visitor).openFor(self) {
            return
        }
        match self.typeAnnotation {
            Some(x) => typeAnnotation.accept(visitor),
            None => ()
        };

        operation.accept(visitor);

        iteration.accept(visitor);
        visitor.closeFor(self)
    }
}

#[derive(Copy, Clone)]
pub struct WhileSyntax {
    pub start: Position,
    pub end: Position,
    pub parent: *mut SyntaxNode,
    pub condition: Ref<OperationSyntax>,
    pub iteration: Ref<LoopSyntax>,
}

impl SyntaxNode for WhileSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        if !(*visitor).openWhile(self) {
            return
        }
        condition.accept(visitor);

        iteration.accept(visitor);
        visitor.closeWhile(self)
    }
}

#[derive(Copy, Clone)]
pub struct DoSyntax {
    pub start: Position,
    pub end: Position,
    pub parent: *mut SyntaxNode,
    pub iteration: Ref<LoopSyntax>,
    pub condition: Ref<OperationSyntax>,
}

impl SyntaxNode for DoSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        if !(*visitor).openDo(self) {
            return
        }
        iteration.accept(visitor);

        condition.accept(visitor);
        visitor.closeDo(self)
    }
}

#[derive(Copy, Clone)]
pub enum LoopSyntax {
   SimpleLoop(SimpleLoopSyntax),
   NamedLoop(NamedLoopSyntax),
}

impl SyntaxNode for LoopSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
    }
}

#[derive(Copy, Clone)]
pub struct SimpleLoopSyntax {
    pub start: Position,
    pub end: Position,
    pub parent: *mut SyntaxNode,
    pub code: Ref<BlockSyntax>,
}

impl SyntaxNode for SimpleLoopSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        if !(*visitor).openSimpleLoop(self) {
            return
        }
        code.accept(visitor);
        visitor.closeSimpleLoop(self)
    }
}

#[derive(Copy, Clone)]
pub struct NamedLoopSyntax {
    pub start: Position,
    pub end: Position,
    pub parent: *mut SyntaxNode,
    pub name: String,
    pub code: Ref<BlockSyntax>,
}

impl SyntaxNode for NamedLoopSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        if !(*visitor).openNamedLoop(self) {
            return
        }
        code.accept(visitor);
        visitor.closeNamedLoop(self)
    }
}

#[derive(Copy, Clone)]
pub struct ThisSyntax {
    pub start: Position,
    pub end: Position,
    pub parent: *mut SyntaxNode,
}

impl SyntaxNode for ThisSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        visitor.visitThis(self)
    }
}

#[derive(Copy, Clone)]
pub struct NewSyntax {
    pub start: Position,
    pub end: Position,
    pub parent: *mut SyntaxNode,
    pub typeSpec: Ref<TypeSyntax>,
}

impl SyntaxNode for NewSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        if !(*visitor).openNew(self) {
            return
        }
        typeSpec.accept(visitor);
        visitor.closeNew(self)
    }
}

#[derive(Copy, Clone)]
pub struct ObjectSyntax {
    pub start: Position,
    pub end: Position,
    pub parent: *mut SyntaxNode,
    pub firstOp: Option<Ref<OperationSyntax>>,
    pub additionalOps: Option<Ref<Vector<Ref<ItemSyntax>>>>,
}

impl SyntaxNode for ObjectSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        if !(*visitor).openObject(self) {
            return
        }
        match self.firstOp {
            Some(x) => firstOp.accept(visitor),
            None => ()
        };

        match self.additionalOps {
            Some(x) => for node in additionalOps {
                node.accept(visitor)
            },
            None => ()
        };
        visitor.closeObject(self)
    }
}

#[derive(Copy, Clone)]
pub struct ArraySyntax {
    pub start: Position,
    pub end: Position,
    pub parent: *mut SyntaxNode,
    pub firstOp: Option<Ref<OperationSyntax>>,
    pub additionalOps: Option<Ref<Vector<Ref<ItemSyntax>>>>,
}

impl SyntaxNode for ArraySyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        if !(*visitor).openArray(self) {
            return
        }
        match self.firstOp {
            Some(x) => firstOp.accept(visitor),
            None => ()
        };

        match self.additionalOps {
            Some(x) => for node in additionalOps {
                node.accept(visitor)
            },
            None => ()
        };
        visitor.closeArray(self)
    }
}

#[derive(Copy, Clone)]
pub struct ItemSyntax {
    pub start: Position,
    pub end: Position,
    pub parent: *mut SyntaxNode,
    pub operation: Ref<OperationSyntax>,
}

impl SyntaxNode for ItemSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        if !(*visitor).openItem(self) {
            return
        }
        operation.accept(visitor);
        visitor.closeItem(self)
    }
}

#[derive(Copy, Clone)]
pub struct SizeOfSyntax {
    pub start: Position,
    pub end: Position,
    pub parent: *mut SyntaxNode,
    pub typeSpec: Ref<TypeSyntax>,
}

impl SyntaxNode for SizeOfSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        if !(*visitor).openSizeOf(self) {
            return
        }
        typeSpec.accept(visitor);
        visitor.closeSizeOf(self)
    }
}

#[derive(Copy, Clone)]
pub struct BreakSyntax {
    pub start: Position,
    pub end: Position,
    pub parent: *mut SyntaxNode,
    pub iteration: Option<String>,
}

impl SyntaxNode for BreakSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        visitor.visitBreak(self)
    }
}

#[derive(Copy, Clone)]
pub struct ContinueSyntax {
    pub start: Position,
    pub end: Position,
    pub parent: *mut SyntaxNode,
    pub iteration: Option<String>,
}

impl SyntaxNode for ContinueSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        visitor.visitContinue(self)
    }
}

#[derive(Copy, Clone)]
pub struct ReturnSyntax {
    pub start: Position,
    pub end: Position,
    pub parent: *mut SyntaxNode,
    pub result: Option<Ref<CalculationSyntax>>,
}

impl SyntaxNode for ReturnSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        if !(*visitor).openReturn(self) {
            return
        }
        match self.result {
            Some(x) => result.accept(visitor),
            None => ()
        };
        visitor.closeReturn(self)
    }
}

#[derive(Copy, Clone)]
pub struct ThrowSyntax {
    pub start: Position,
    pub end: Position,
    pub parent: *mut SyntaxNode,
    pub exception: Ref<CalculationSyntax>,
}

impl SyntaxNode for ThrowSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        if !(*visitor).openThrow(self) {
            return
        }
        exception.accept(visitor);
        visitor.closeThrow(self)
    }
}

#[derive(Copy, Clone)]
pub struct ClassSyntax {
    pub start: Position,
    pub end: Position,
    pub parent: *mut SyntaxNode,
    pub name: Ref<NameSyntax>,
    pub generics: Option<Ref<GenericParametersSyntax>>,
    pub baseClass: Option<Ref<ExtendsSyntax>>,
    pub contents: Option<Ref<StructureSyntax>>,
    pub body: Option<Ref<ClassBodySyntax>>,
}

impl SyntaxNode for ClassSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        if !(*visitor).openClass(self) {
            return
        }
        name.accept(visitor);

        match self.generics {
            Some(x) => generics.accept(visitor),
            None => ()
        };

        match self.baseClass {
            Some(x) => baseClass.accept(visitor),
            None => ()
        };

        match self.contents {
            Some(x) => contents.accept(visitor),
            None => ()
        };

        match self.body {
            Some(x) => body.accept(visitor),
            None => ()
        };
        visitor.closeClass(self)
    }
}

#[derive(Copy, Clone)]
pub struct GenericParametersSyntax {
    pub start: Position,
    pub end: Position,
    pub parent: *mut SyntaxNode,
    pub name: String,
    pub additionalGenerics: Option<Ref<Vector<Ref<GenericParameterSyntax>>>>,
}

impl SyntaxNode for GenericParametersSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        if !(*visitor).openGenericParameters(self) {
            return
        }
        match self.additionalGenerics {
            Some(x) => for node in additionalGenerics {
                node.accept(visitor)
            },
            None => ()
        };
        visitor.closeGenericParameters(self)
    }
}

#[derive(Copy, Clone)]
pub struct GenericParameterSyntax {
    pub start: Position,
    pub end: Position,
    pub parent: *mut SyntaxNode,
    pub name: String,
}

impl SyntaxNode for GenericParameterSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        visitor.visitGenericParameter(self)
    }
}

#[derive(Copy, Clone)]
pub struct ExtendsSyntax {
    pub start: Position,
    pub end: Position,
    pub parent: *mut SyntaxNode,
    pub name: Ref<NameSyntax>,
}

impl SyntaxNode for ExtendsSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        if !(*visitor).openExtends(self) {
            return
        }
        name.accept(visitor);
        visitor.closeExtends(self)
    }
}

#[derive(Copy, Clone)]
pub struct StructureSyntax {
    pub start: Position,
    pub end: Position,
    pub parent: *mut SyntaxNode,
    pub components: Option<Ref<Vector<Ref<ComponentSyntax>>>>,
}

impl SyntaxNode for StructureSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        if !(*visitor).openStructure(self) {
            return
        }
        match self.components {
            Some(x) => for node in components {
                node.accept(visitor)
            },
            None => ()
        };
        visitor.closeStructure(self)
    }
}

#[derive(Copy, Clone)]
pub struct ComponentSyntax {
    pub start: Position,
    pub end: Position,
    pub parent: *mut SyntaxNode,
    pub name: String,
    pub typeAnnotation: Option<Ref<TypeAnnotationSyntax>>,
}

impl SyntaxNode for ComponentSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        if !(*visitor).openComponent(self) {
            return
        }
        match self.typeAnnotation {
            Some(x) => typeAnnotation.accept(visitor),
            None => ()
        };
        visitor.closeComponent(self)
    }
}

#[derive(Copy, Clone)]
pub struct ClassBodySyntax {
    pub start: Position,
    pub end: Position,
    pub parent: *mut SyntaxNode,
    pub members: Option<Ref<Vector<Ref<ClassMemberSyntax>>>>,
}

impl SyntaxNode for ClassBodySyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        if !(*visitor).openClassBody(self) {
            return
        }
        match self.members {
            Some(x) => for node in members {
                node.accept(visitor)
            },
            None => ()
        };
        visitor.closeClassBody(self)
    }
}

#[derive(Copy, Clone)]
pub enum ClassMemberSyntax {
   LetMember(LetMemberSyntax),
   VarMember(VarMemberSyntax),
   MutableMember(MutableMemberSyntax),
   SetInitialization(SetInitializationSyntax),
   Method(MethodSyntax),
   StaticFunction(StaticFunctionSyntax),
   Operator(OperatorSyntax),
   Initializer(InitializerSyntax),
   Allocator(AllocatorSyntax),
}

impl SyntaxNode for ClassMemberSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
    }
}

#[derive(Copy, Clone)]
pub struct LetMemberSyntax {
    pub start: Position,
    pub end: Position,
    pub parent: *mut SyntaxNode,
    pub declaration: Ref<LetSyntax>,
}

impl SyntaxNode for LetMemberSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        if !(*visitor).openLetMember(self) {
            return
        }
        declaration.accept(visitor);
        visitor.closeLetMember(self)
    }
}

#[derive(Copy, Clone)]
pub struct VarMemberSyntax {
    pub start: Position,
    pub end: Position,
    pub parent: *mut SyntaxNode,
    pub declaration: Ref<VarSyntax>,
}

impl SyntaxNode for VarMemberSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        if !(*visitor).openVarMember(self) {
            return
        }
        declaration.accept(visitor);
        visitor.closeVarMember(self)
    }
}

#[derive(Copy, Clone)]
pub struct MutableMemberSyntax {
    pub start: Position,
    pub end: Position,
    pub parent: *mut SyntaxNode,
    pub declaration: Ref<MutableSyntax>,
}

impl SyntaxNode for MutableMemberSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        if !(*visitor).openMutableMember(self) {
            return
        }
        declaration.accept(visitor);
        visitor.closeMutableMember(self)
    }
}

#[derive(Copy, Clone)]
pub struct SetInitializationSyntax {
    pub start: Position,
    pub end: Position,
    pub parent: *mut SyntaxNode,
    pub definition: Ref<SetSyntax>,
}

impl SyntaxNode for SetInitializationSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        if !(*visitor).openSetInitialization(self) {
            return
        }
        definition.accept(visitor);
        visitor.closeSetInitialization(self)
    }
}

#[derive(Copy, Clone)]
pub struct MethodSyntax {
    pub start: Position,
    pub end: Position,
    pub parent: *mut SyntaxNode,
    pub procedure: Ref<ProcedureSyntax>,
}

impl SyntaxNode for MethodSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        if !(*visitor).openMethod(self) {
            return
        }
        procedure.accept(visitor);
        visitor.closeMethod(self)
    }
}

#[derive(Copy, Clone)]
pub struct StaticFunctionSyntax {
    pub start: Position,
    pub end: Position,
    pub parent: *mut SyntaxNode,
    pub procedure: Ref<FunctionSyntax>,
}

impl SyntaxNode for StaticFunctionSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        if !(*visitor).openStaticFunction(self) {
            return
        }
        procedure.accept(visitor);
        visitor.closeStaticFunction(self)
    }
}

#[derive(Copy, Clone)]
pub struct OperatorSyntax {
    pub start: Position,
    pub end: Position,
    pub parent: *mut SyntaxNode,
    pub routine: Ref<RoutineSyntax>,
}

impl SyntaxNode for OperatorSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        if !(*visitor).openOperator(self) {
            return
        }
        routine.accept(visitor);
        visitor.closeOperator(self)
    }
}

#[derive(Copy, Clone)]
pub struct InitializerSyntax {
    pub start: Position,
    pub end: Position,
    pub parent: *mut SyntaxNode,
    pub input: Option<Ref<StructureSyntax>>,
    pub body: Ref<BlockSyntax>,
}

impl SyntaxNode for InitializerSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        if !(*visitor).openInitializer(self) {
            return
        }
        match self.input {
            Some(x) => input.accept(visitor),
            None => ()
        };

        body.accept(visitor);
        visitor.closeInitializer(self)
    }
}

#[derive(Copy, Clone)]
pub struct AllocatorSyntax {
    pub start: Position,
    pub end: Position,
    pub parent: *mut SyntaxNode,
    pub input: Option<Ref<StructureSyntax>>,
    pub body: Ref<BlockSyntax>,
}

impl SyntaxNode for AllocatorSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        if !(*visitor).openAllocator(self) {
            return
        }
        match self.input {
            Some(x) => input.accept(visitor),
            None => ()
        };

        body.accept(visitor);
        visitor.closeAllocator(self)
    }
}

#[derive(Copy, Clone)]
pub struct TypeAnnotationSyntax {
    pub start: Position,
    pub end: Position,
    pub parent: *mut SyntaxNode,
    pub typeSpec: Ref<TypeSpecSyntax>,
}

impl SyntaxNode for TypeAnnotationSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        if !(*visitor).openTypeAnnotation(self) {
            return
        }
        typeSpec.accept(visitor);
        visitor.closeTypeAnnotation(self)
    }
}

#[derive(Copy, Clone)]
pub enum TypeSpecSyntax {
   Type(TypeSyntax),
   Variant(VariantSyntax),
}

impl SyntaxNode for TypeSpecSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
    }
}

#[derive(Copy, Clone)]
pub struct TypeSyntax {
    pub start: Position,
    pub end: Position,
    pub parent: *mut SyntaxNode,
    pub name: Ref<NameSyntax>,
    pub generics: Option<Ref<GenericArgumentsSyntax>>,
    pub optional: Option<Ref<OptionalSyntax>>,
    pub lifeTime: Option<Ref<LifeTimeSyntax>>,
}

impl SyntaxNode for TypeSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        if !(*visitor).openType(self) {
            return
        }
        name.accept(visitor);

        match self.generics {
            Some(x) => generics.accept(visitor),
            None => ()
        };

        match self.optional {
            Some(x) => optional.accept(visitor),
            None => ()
        };

        match self.lifeTime {
            Some(x) => lifeTime.accept(visitor),
            None => ()
        };
        visitor.closeType(self)
    }
}

#[derive(Copy, Clone)]
pub struct VariantSyntax {
    pub start: Position,
    pub end: Position,
    pub parent: *mut SyntaxNode,
    pub types: Option<Ref<Vector<Ref<TypeSyntax>>>>,
}

impl SyntaxNode for VariantSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        if !(*visitor).openVariant(self) {
            return
        }
        match self.types {
            Some(x) => for node in types {
                node.accept(visitor)
            },
            None => ()
        };
        visitor.closeVariant(self)
    }
}

#[derive(Copy, Clone)]
pub struct ThrowsSyntax {
    pub start: Position,
    pub end: Position,
    pub parent: *mut SyntaxNode,
    pub throwsType: Ref<TypeSpecSyntax>,
}

impl SyntaxNode for ThrowsSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        if !(*visitor).openThrows(self) {
            return
        }
        throwsType.accept(visitor);
        visitor.closeThrows(self)
    }
}

#[derive(Copy, Clone)]
pub struct GenericArgumentsSyntax {
    pub start: Position,
    pub end: Position,
    pub parent: *mut SyntaxNode,
    pub generic: Ref<TypeSyntax>,
    pub additionalGenerics: Option<Ref<Vector<Ref<GenericArgumentSyntax>>>>,
}

impl SyntaxNode for GenericArgumentsSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        if !(*visitor).openGenericArguments(self) {
            return
        }
        generic.accept(visitor);

        match self.additionalGenerics {
            Some(x) => for node in additionalGenerics {
                node.accept(visitor)
            },
            None => ()
        };
        visitor.closeGenericArguments(self)
    }
}

#[derive(Copy, Clone)]
pub struct GenericArgumentSyntax {
    pub start: Position,
    pub end: Position,
    pub parent: *mut SyntaxNode,
    pub typeSpec: Ref<TypeSyntax>,
}

impl SyntaxNode for GenericArgumentSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        if !(*visitor).openGenericArgument(self) {
            return
        }
        typeSpec.accept(visitor);
        visitor.closeGenericArgument(self)
    }
}

#[derive(Copy, Clone)]
pub struct OptionalSyntax {
    pub start: Position,
    pub end: Position,
    pub parent: *mut SyntaxNode,
}

impl SyntaxNode for OptionalSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        visitor.visitOptional(self)
    }
}

#[derive(Copy, Clone)]
pub enum LifeTimeSyntax {
   Root(RootSyntax),
   Local(LocalSyntax),
   Reference(ReferenceSyntax),
   Thrown(ThrownSyntax),
}

impl SyntaxNode for LifeTimeSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
    }
}

#[derive(Copy, Clone)]
pub struct RootSyntax {
    pub start: Position,
    pub end: Position,
    pub parent: *mut SyntaxNode,
}

impl SyntaxNode for RootSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        visitor.visitRoot(self)
    }
}

#[derive(Copy, Clone)]
pub struct LocalSyntax {
    pub start: Position,
    pub end: Position,
    pub parent: *mut SyntaxNode,
    pub location: String,
}

impl SyntaxNode for LocalSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        visitor.visitLocal(self)
    }
}

#[derive(Copy, Clone)]
pub struct ReferenceSyntax {
    pub start: Position,
    pub end: Position,
    pub parent: *mut SyntaxNode,
    pub age: Literal,
}

impl SyntaxNode for ReferenceSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        visitor.visitReference(self)
    }
}

#[derive(Copy, Clone)]
pub struct ThrownSyntax {
    pub start: Position,
    pub end: Position,
    pub parent: *mut SyntaxNode,
}

impl SyntaxNode for ThrownSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        visitor.visitThrown(self)
    }
}
