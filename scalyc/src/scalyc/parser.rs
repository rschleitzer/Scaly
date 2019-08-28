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

        let ret = Ref::new(_rp, FileSyntax { file_name: self.file_name, intrinsics: intrinsics, usings: usings, defines: defines, declarations: declarations, statements: statements });

        match intrinsics {
            Some(mut x) => for item in x.iter_mut() {
                item.parent = Some(ParentNode::File(ret));
            }
            None => ()
        }

        match usings {
            Some(mut x) => for item in x.iter_mut() {
                item.parent = Some(ParentNode::File(ret));
            }
            None => ()
        }

        match defines {
            Some(mut x) => for item in x.iter_mut() {
                item.parent = Some(ParentNode::File(ret));
            }
            None => ()
        }

        match declarations {
            Some(mut x) => for item in x.iter_mut() {
                match **item {
                    DeclarationSyntax::Namespace(mut y) => y.parent = Some(ParentNode::File(ret)),
                    DeclarationSyntax::Function(mut y) => y.parent = Some(ParentNode::File(ret)),
                    DeclarationSyntax::Class(mut y) => y.parent = Some(ParentNode::File(ret)),
                    DeclarationSyntax::LetDeclaration(mut y) => y.parent = Some(ParentNode::File(ret)),
                    DeclarationSyntax::VarDeclaration(mut y) => y.parent = Some(ParentNode::File(ret)),
                    DeclarationSyntax::MutableDeclaration(mut y) => y.parent = Some(ParentNode::File(ret)),
                    DeclarationSyntax::ThreadLocalDeclaration(mut y) => y.parent = Some(ParentNode::File(ret)),
                }
            }
            None => ()
        }

        match statements {
            Some(mut x) => for item in x.iter_mut() {
                match **item {
                    StatementSyntax::Let(mut y) => y.parent = Some(ParentNode::File(ret)),
                    StatementSyntax::Var(mut y) => y.parent = Some(ParentNode::File(ret)),
                    StatementSyntax::Mutable(mut y) => y.parent = Some(ParentNode::File(ret)),
                    StatementSyntax::ThreadLocal(mut y) => y.parent = Some(ParentNode::File(ret)),
                    StatementSyntax::Set(mut y) => y.parent = Some(ParentNode::File(ret)),
                    StatementSyntax::Calculation(mut y) => y.parent = Some(ParentNode::File(ret)),
                    StatementSyntax::Break(mut y) => y.parent = Some(ParentNode::File(ret)),
                    StatementSyntax::Continue(mut y) => y.parent = Some(ParentNode::File(ret)),
                    StatementSyntax::Return(mut y) => y.parent = Some(ParentNode::File(ret)),
                    StatementSyntax::Throw(mut y) => y.parent = Some(ParentNode::File(ret)),
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

        let success_intrinsic_1 = self.lexer.parse_keyword(String::from_string_slice(_r.page, "intrinsic"));
        if success_intrinsic_1 {
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

        let ret = Ref::new(_rp, IntrinsicSyntax { parent: None, start: start, end: end, name: name.unwrap() });

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

        let success_using_1 = self.lexer.parse_keyword(String::from_string_slice(_r.page, "using"));
        if success_using_1 {
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

        let ret = Ref::new(_rp, UsingSyntax { parent: None, start: start, end: end, name: name.unwrap() });

        name.unwrap().parent = Some(ParentNode::Using(ret));

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

        let success_define_1 = self.lexer.parse_keyword(String::from_string_slice(_r.page, "define"));
        if success_define_1 {
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

        let type_spec = self.parse_type(&_r, _rp, _ep)?;
        match type_spec {
            None =>
                return Err(Ref::new(_ep, ParserError { file_name: String::copy(_ep, self.file_name), line: self.lexer.line, column: self.lexer.column })),
            Some(_) => ()
        }

        let end: Position = self.lexer.get_position();

        let ret = Ref::new(_rp, DefineSyntax { parent: None, start: start, end: end, name: name.unwrap(), type_spec: type_spec.unwrap() });

        name.unwrap().parent = Some(ParentNode::Define(ret));

        type_spec.unwrap().parent = Some(ParentNode::Define(ret));

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

        let ret = Ref::new(_rp, NameSyntax { parent: None, start: start, end: end, name: name.unwrap(), extensions: extensions });

        match extensions {
            Some(mut x) => for item in x.iter_mut() {
                item.parent = Some(ParentNode::Name(ret));
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

        let success_dot_1 = self.lexer.parse_punctuation(String::from_string_slice(_r.page, "."));
        if success_dot_1 {
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

        let ret = Ref::new(_rp, ExtensionSyntax { parent: None, start: start, end: end, name: name.unwrap() });

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

        let success_namespace_1 = self.lexer.parse_keyword(String::from_string_slice(_r.page, "namespace"));
        if success_namespace_1 {
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

        let success_left_curly_3 = self.lexer.parse_punctuation(String::from_string_slice(_r.page, "{"));
        if success_left_curly_3 {
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

        let success_right_curly_7 = self.lexer.parse_punctuation(String::from_string_slice(_r.page, "}"));
        if success_right_curly_7 {
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

        let ret = Ref::new(_rp, NamespaceSyntax { parent: None, start: start, end: end, name: name.unwrap(), usings: usings, defines: defines, declarations: declarations });

        name.unwrap().parent = Some(ParentNode::Namespace(ret));

        match usings {
            Some(mut x) => for item in x.iter_mut() {
                item.parent = Some(ParentNode::Namespace(ret));
            }
            None => ()
        }

        match defines {
            Some(mut x) => for item in x.iter_mut() {
                item.parent = Some(ParentNode::Namespace(ret));
            }
            None => ()
        }

        match declarations {
            Some(mut x) => for item in x.iter_mut() {
                match **item {
                    DeclarationSyntax::Namespace(mut y) => y.parent = Some(ParentNode::Namespace(ret)),
                    DeclarationSyntax::Function(mut y) => y.parent = Some(ParentNode::Namespace(ret)),
                    DeclarationSyntax::Class(mut y) => y.parent = Some(ParentNode::Namespace(ret)),
                    DeclarationSyntax::LetDeclaration(mut y) => y.parent = Some(ParentNode::Namespace(ret)),
                    DeclarationSyntax::VarDeclaration(mut y) => y.parent = Some(ParentNode::Namespace(ret)),
                    DeclarationSyntax::MutableDeclaration(mut y) => y.parent = Some(ParentNode::Namespace(ret)),
                    DeclarationSyntax::ThreadLocalDeclaration(mut y) => y.parent = Some(ParentNode::Namespace(ret)),
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

        let success_function_1 = self.lexer.parse_keyword(String::from_string_slice(_r.page, "function"));
        if success_function_1 {
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

        let ret = Ref::new(_rp, FunctionSyntax { parent: None, start: start, end: end, procedure: procedure.unwrap() });

        procedure.unwrap().parent = Some(ParentNode::Function(ret));

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

        let ret = Ref::new(_rp, ProcedureSyntax { parent: None, start: start, end: end, name: name.unwrap(), routine: routine.unwrap() });

        routine.unwrap().parent = Some(ParentNode::Procedure(ret));

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

        let throws_clause = self.parse_throws(&_r, _rp, _ep)?;

        let body = self.parse_block(&_r, _rp, _ep)?;
        match body {
            None =>
                return Err(Ref::new(_ep, ParserError { file_name: String::copy(_ep, self.file_name), line: self.lexer.line, column: self.lexer.column })),
            Some(_) => ()
        }

        let end: Position = self.lexer.get_position();

        let ret = Ref::new(_rp, RoutineSyntax { parent: None, start: start, end: end, input: input, output: output, throws_clause: throws_clause, body: body.unwrap() });

        match input {
            Some(mut x) => 
                x.parent = Some(ParentNode::Routine(ret)),
            None => ()
        }

        match output {
            Some(mut x) => 
                x.parent = Some(ParentNode::Routine(ret)),
            None => ()
        }

        match throws_clause {
            Some(mut x) => 
                x.parent = Some(ParentNode::Routine(ret)),
            None => ()
        }

        body.unwrap().parent = Some(ParentNode::Routine(ret));

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

        let ret = Ref::new(_rp, LetDeclarationSyntax { parent: None, start: start, end: end, declaration: declaration.unwrap() });

        declaration.unwrap().parent = Some(ParentNode::LetDeclaration(ret));

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

        let ret = Ref::new(_rp, VarDeclarationSyntax { parent: None, start: start, end: end, declaration: declaration.unwrap() });

        declaration.unwrap().parent = Some(ParentNode::VarDeclaration(ret));

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

        let ret = Ref::new(_rp, MutableDeclarationSyntax { parent: None, start: start, end: end, declaration: declaration.unwrap() });

        declaration.unwrap().parent = Some(ParentNode::MutableDeclaration(ret));

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

        let ret = Ref::new(_rp, ThreadLocalDeclarationSyntax { parent: None, start: start, end: end, declaration: declaration.unwrap() });

        declaration.unwrap().parent = Some(ParentNode::ThreadLocalDeclaration(ret));

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

        let success_let_1 = self.lexer.parse_keyword(String::from_string_slice(_r.page, "let"));
        if success_let_1 {
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

        let ret = Ref::new(_rp, LetSyntax { parent: None, start: start, end: end, binding: binding.unwrap() });

        binding.unwrap().parent = Some(ParentNode::Let(ret));

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

        let success_var_1 = self.lexer.parse_keyword(String::from_string_slice(_r.page, "var"));
        if success_var_1 {
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

        let ret = Ref::new(_rp, VarSyntax { parent: None, start: start, end: end, binding: binding.unwrap() });

        binding.unwrap().parent = Some(ParentNode::Var(ret));

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

        let success_mutable_1 = self.lexer.parse_keyword(String::from_string_slice(_r.page, "mutable"));
        if success_mutable_1 {
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

        let ret = Ref::new(_rp, MutableSyntax { parent: None, start: start, end: end, binding: binding.unwrap() });

        binding.unwrap().parent = Some(ParentNode::Mutable(ret));

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

        let success_threadlocal_1 = self.lexer.parse_keyword(String::from_string_slice(_r.page, "threadlocal"));
        if success_threadlocal_1 {
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

        let ret = Ref::new(_rp, ThreadLocalSyntax { parent: None, start: start, end: end, binding: binding.unwrap() });

        binding.unwrap().parent = Some(ParentNode::ThreadLocal(ret));

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

        let type_annotation = self.parse_typeannotation(&_r, _rp, _ep)?;

        let calculation = self.parse_calculation(&_r, _rp, _ep)?;
        match calculation {
            None =>
                return Err(Ref::new(_ep, ParserError { file_name: String::copy(_ep, self.file_name), line: self.lexer.line, column: self.lexer.column })),
            Some(_) => ()
        }

        let end: Position = self.lexer.get_position();

        let ret = Ref::new(_rp, BindingSyntax { parent: None, start: start, end: end, name: name.unwrap(), type_annotation: type_annotation, calculation: calculation.unwrap() });

        match type_annotation {
            Some(mut x) => 
                x.parent = Some(ParentNode::Binding(ret)),
            None => ()
        }

        calculation.unwrap().parent = Some(ParentNode::Binding(ret));

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

        let success_set_1 = self.lexer.parse_keyword(String::from_string_slice(_r.page, "set"));
        if success_set_1 {
            self.lexer.advance(&_r)
        } else {
            return Ok(None)
        }

        let l_value = self.parse_operation(&_r, _rp, _ep)?;
        match l_value {
            None =>
                return Err(Ref::new(_ep, ParserError { file_name: String::copy(_ep, self.file_name), line: self.lexer.line, column: self.lexer.column })),
            Some(_) => ()
        }

        let success_colon_3 = self.lexer.parse_punctuation(String::from_string_slice(_r.page, ":"));
        if success_colon_3 {
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

        let r_value = self.parse_calculation(&_r, _rp, _ep)?;
        match r_value {
            None =>
                return Err(Ref::new(_ep, ParserError { file_name: String::copy(_ep, self.file_name), line: self.lexer.line, column: self.lexer.column })),
            Some(_) => ()
        }

        let end: Position = self.lexer.get_position();

        let ret = Ref::new(_rp, SetSyntax { parent: None, start: start, end: end, l_value: l_value.unwrap(), r_value: r_value.unwrap() });

        l_value.unwrap().parent = Some(ParentNode::Set(ret));

        r_value.unwrap().parent = Some(ParentNode::Set(ret));

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

        let success_semicolon_2 = self.lexer.parse_punctuation(String::from_string_slice(_r.page, ";"));
        if success_semicolon_2 {
            self.lexer.advance(&_r)
        } else {
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
        match op {
            None =>
                return Ok(None),
            Some(_) => ()
        }

        let end: Position = self.lexer.get_position();

        let ret = Ref::new(_rp, OperationSyntax { parent: None, start: start, end: end, op: op.unwrap() });

        match op {
            Some(mut x) => for item in x.iter_mut() {
                item.parent = Some(ParentNode::Operation(ret));
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

        let ret = Ref::new(_rp, OperandSyntax { parent: None, start: start, end: end, primary: primary.unwrap(), postfixes: postfixes });

        match *primary.unwrap() {
            ExpressionSyntax::Block(mut x) => x.parent = Some(ParentNode::Operand(ret)),
            ExpressionSyntax::Name(mut x) => x.parent = Some(ParentNode::Operand(ret)),
            ExpressionSyntax::Constant(mut x) => x.parent = Some(ParentNode::Operand(ret)),
            ExpressionSyntax::If(mut x) => x.parent = Some(ParentNode::Operand(ret)),
            ExpressionSyntax::Switch(mut x) => x.parent = Some(ParentNode::Operand(ret)),
            ExpressionSyntax::For(mut x) => x.parent = Some(ParentNode::Operand(ret)),
            ExpressionSyntax::While(mut x) => x.parent = Some(ParentNode::Operand(ret)),
            ExpressionSyntax::Do(mut x) => x.parent = Some(ParentNode::Operand(ret)),
            ExpressionSyntax::This(mut x) => x.parent = Some(ParentNode::Operand(ret)),
            ExpressionSyntax::New(mut x) => x.parent = Some(ParentNode::Operand(ret)),
            ExpressionSyntax::Object(mut x) => x.parent = Some(ParentNode::Operand(ret)),
            ExpressionSyntax::Array(mut x) => x.parent = Some(ParentNode::Operand(ret)),
            ExpressionSyntax::SizeOf(mut x) => x.parent = Some(ParentNode::Operand(ret)),
        }

        match postfixes {
            Some(mut x) => for item in x.iter_mut() {
                match **item {
                    PostfixSyntax::MemberAccess(mut y) => y.parent = Some(ParentNode::Operand(ret)),
                    PostfixSyntax::As(mut y) => y.parent = Some(ParentNode::Operand(ret)),
                    PostfixSyntax::Is(mut y) => y.parent = Some(ParentNode::Operand(ret)),
                    PostfixSyntax::Unwrap(mut y) => y.parent = Some(ParentNode::Operand(ret)),
                    PostfixSyntax::Catch(mut y) => y.parent = Some(ParentNode::Operand(ret)),
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

        let success_dot_1 = self.lexer.parse_punctuation(String::from_string_slice(_r.page, "."));
        if success_dot_1 {
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

        let ret = Ref::new(_rp, MemberAccessSyntax { parent: None, start: start, end: end, member: member.unwrap() });

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

        let success_as_1 = self.lexer.parse_keyword(String::from_string_slice(_r.page, "as"));
        if success_as_1 {
            self.lexer.advance(&_r)
        } else {
            return Ok(None)
        }

        let type_spec = self.parse_typespec(&_r, _rp, _ep)?;
        match type_spec {
            None =>
                return Err(Ref::new(_ep, ParserError { file_name: String::copy(_ep, self.file_name), line: self.lexer.line, column: self.lexer.column })),
            Some(_) => ()
        }

        let end: Position = self.lexer.get_position();

        let ret = Ref::new(_rp, AsSyntax { parent: None, start: start, end: end, type_spec: type_spec.unwrap() });

        match *type_spec.unwrap() {
            TypeSpecSyntax::Type(mut x) => x.parent = Some(ParentNode::As(ret)),
            TypeSpecSyntax::Variant(mut x) => x.parent = Some(ParentNode::As(ret)),
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

        let success_is_1 = self.lexer.parse_keyword(String::from_string_slice(_r.page, "is"));
        if success_is_1 {
            self.lexer.advance(&_r)
        } else {
            return Ok(None)
        }

        let type_spec = self.parse_typespec(&_r, _rp, _ep)?;
        match type_spec {
            None =>
                return Err(Ref::new(_ep, ParserError { file_name: String::copy(_ep, self.file_name), line: self.lexer.line, column: self.lexer.column })),
            Some(_) => ()
        }

        let end: Position = self.lexer.get_position();

        let ret = Ref::new(_rp, IsSyntax { parent: None, start: start, end: end, type_spec: type_spec.unwrap() });

        match *type_spec.unwrap() {
            TypeSpecSyntax::Type(mut x) => x.parent = Some(ParentNode::Is(ret)),
            TypeSpecSyntax::Variant(mut x) => x.parent = Some(ParentNode::Is(ret)),
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

        let success_exclamation_1 = self.lexer.parse_punctuation(String::from_string_slice(_r.page, "!"));
        if success_exclamation_1 {
            self.lexer.advance(&_r)
        } else {
            return Ok(None)
        }

        let end: Position = self.lexer.get_position();

        let ret = Ref::new(_rp, UnwrapSyntax { parent: None, start: start, end: end });

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

        let success_catch_1 = self.lexer.parse_keyword(String::from_string_slice(_r.page, "catch"));
        if success_catch_1 {
            self.lexer.advance(&_r)
        } else {
            return Ok(None)
        }

        let type_spec = self.parse_catchpattern(&_r, _rp, _ep)?;
        match type_spec {
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

        let ret = Ref::new(_rp, CatchSyntax { parent: None, start: start, end: end, type_spec: type_spec.unwrap(), handler: handler.unwrap() });

        match *type_spec.unwrap() {
            CatchPatternSyntax::WildCardCatchPattern(mut x) => x.parent = Some(ParentNode::Catch(ret)),
            CatchPatternSyntax::TypeCatchPattern(mut x) => x.parent = Some(ParentNode::Catch(ret)),
        }

        handler.unwrap().parent = Some(ParentNode::Catch(ret));

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

        let ret = Ref::new(_rp, WildCardCatchPatternSyntax { parent: None, start: start, end: end, pattern: pattern.unwrap() });

        pattern.unwrap().parent = Some(ParentNode::WildCardCatchPattern(ret));

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

        let type_spec = self.parse_typespec(&_r, _rp, _ep)?;
        match type_spec {
            None =>
                return Ok(None),
            Some(_) => ()
        }

        let error_name = self.lexer.parse_identifier(_rp);
        match error_name {
            Some(error_name) => if self.is_identifier(error_name) {
                self.lexer.advance(&_r)
            } else {
()
           },
            None => ()
        }

        let end: Position = self.lexer.get_position();

        let ret = Ref::new(_rp, TypeCatchPatternSyntax { parent: None, start: start, end: end, type_spec: type_spec.unwrap(), error_name: error_name });

        match *type_spec.unwrap() {
            TypeSpecSyntax::Type(mut x) => x.parent = Some(ParentNode::TypeCatchPattern(ret)),
            TypeSpecSyntax::Variant(mut x) => x.parent = Some(ParentNode::TypeCatchPattern(ret)),
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

        let success_left_curly_1 = self.lexer.parse_punctuation(String::from_string_slice(_r.page, "{"));
        if success_left_curly_1 {
            self.lexer.advance(&_r)
        } else {
            return Ok(None)
        }

        let statements = self.parse_statement_list(&_r, _rp, _ep)?;

        let success_right_curly_3 = self.lexer.parse_punctuation(String::from_string_slice(_r.page, "}"));
        if success_right_curly_3 {
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

        let ret = Ref::new(_rp, BlockSyntax { parent: None, start: start, end: end, statements: statements });

        match statements {
            Some(mut x) => for item in x.iter_mut() {
                match **item {
                    StatementSyntax::Let(mut y) => y.parent = Some(ParentNode::Block(ret)),
                    StatementSyntax::Var(mut y) => y.parent = Some(ParentNode::Block(ret)),
                    StatementSyntax::Mutable(mut y) => y.parent = Some(ParentNode::Block(ret)),
                    StatementSyntax::ThreadLocal(mut y) => y.parent = Some(ParentNode::Block(ret)),
                    StatementSyntax::Set(mut y) => y.parent = Some(ParentNode::Block(ret)),
                    StatementSyntax::Calculation(mut y) => y.parent = Some(ParentNode::Block(ret)),
                    StatementSyntax::Break(mut y) => y.parent = Some(ParentNode::Block(ret)),
                    StatementSyntax::Continue(mut y) => y.parent = Some(ParentNode::Block(ret)),
                    StatementSyntax::Return(mut y) => y.parent = Some(ParentNode::Block(ret)),
                    StatementSyntax::Throw(mut y) => y.parent = Some(ParentNode::Block(ret)),
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
            Some(_) =>
                self.lexer.advance(&_r),
            None =>             return Ok(None)
        }

        let end: Position = self.lexer.get_position();

        let ret = Ref::new(_rp, ConstantSyntax { parent: None, start: start, end: end, literal: literal.unwrap() });

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

        let success_if_1 = self.lexer.parse_keyword(String::from_string_slice(_r.page, "if"));
        if success_if_1 {
            self.lexer.advance(&_r)
        } else {
            return Ok(None)
        }

        let success_left_paren_2 = self.lexer.parse_punctuation(String::from_string_slice(_r.page, "("));
        if success_left_paren_2 {
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

        let success_right_paren_4 = self.lexer.parse_punctuation(String::from_string_slice(_r.page, ")"));
        if success_right_paren_4 {
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

        let else_clause = self.parse_else(&_r, _rp, _ep)?;

        let end: Position = self.lexer.get_position();

        let ret = Ref::new(_rp, IfSyntax { parent: None, start: start, end: end, condition: condition.unwrap(), consequent: consequent.unwrap(), else_clause: else_clause });

        condition.unwrap().parent = Some(ParentNode::If(ret));

        consequent.unwrap().parent = Some(ParentNode::If(ret));

        match else_clause {
            Some(mut x) => 
                x.parent = Some(ParentNode::If(ret)),
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

        let success_else_1 = self.lexer.parse_keyword(String::from_string_slice(_r.page, "else"));
        if success_else_1 {
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

        let ret = Ref::new(_rp, ElseSyntax { parent: None, start: start, end: end, alternative: alternative.unwrap() });

        alternative.unwrap().parent = Some(ParentNode::Else(ret));

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

        let success_switch_1 = self.lexer.parse_keyword(String::from_string_slice(_r.page, "switch"));
        if success_switch_1 {
            self.lexer.advance(&_r)
        } else {
            return Ok(None)
        }

        let success_left_paren_2 = self.lexer.parse_punctuation(String::from_string_slice(_r.page, "("));
        if success_left_paren_2 {
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

        let success_right_paren_4 = self.lexer.parse_punctuation(String::from_string_slice(_r.page, ")"));
        if success_right_paren_4 {
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

        let success_left_curly_5 = self.lexer.parse_punctuation(String::from_string_slice(_r.page, "{"));
        if success_left_curly_5 {
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

        let success_right_curly_7 = self.lexer.parse_punctuation(String::from_string_slice(_r.page, "}"));
        if success_right_curly_7 {
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

        let ret = Ref::new(_rp, SwitchSyntax { parent: None, start: start, end: end, condition: condition.unwrap(), cases: cases.unwrap() });

        condition.unwrap().parent = Some(ParentNode::Switch(ret));

        match cases {
            Some(mut x) => for item in x.iter_mut() {
                item.parent = Some(ParentNode::Switch(ret));
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

        let ret = Ref::new(_rp, SwitchCaseSyntax { parent: None, start: start, end: end, label: label.unwrap(), content: content.unwrap() });

        match *label.unwrap() {
            CaseLabelSyntax::ItemCaseLabel(mut x) => x.parent = Some(ParentNode::SwitchCase(ret)),
            CaseLabelSyntax::DefaultCaseLabel(mut x) => x.parent = Some(ParentNode::SwitchCase(ret)),
        }

        content.unwrap().parent = Some(ParentNode::SwitchCase(ret));

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

        let success_case_1 = self.lexer.parse_keyword(String::from_string_slice(_r.page, "case"));
        if success_case_1 {
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

        let ret = Ref::new(_rp, ItemCaseLabelSyntax { parent: None, start: start, end: end, items: items.unwrap() });

        match items {
            Some(mut x) => for item in x.iter_mut() {
                item.parent = Some(ParentNode::ItemCaseLabel(ret));
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

        let success_comma_2 = self.lexer.parse_punctuation(String::from_string_slice(_r.page, ","));
        if success_comma_2 {
            self.lexer.advance(&_r)
        } else {
()
        }

        let end: Position = self.lexer.get_position();

        let ret = Ref::new(_rp, CaseItemSyntax { parent: None, start: start, end: end, pattern: pattern.unwrap() });

        match *pattern.unwrap() {
            CasePatternSyntax::ConstantPattern(mut x) => x.parent = Some(ParentNode::CaseItem(ret)),
            CasePatternSyntax::WildcardPattern(mut x) => x.parent = Some(ParentNode::CaseItem(ret)),
            CasePatternSyntax::NamePattern(mut x) => x.parent = Some(ParentNode::CaseItem(ret)),
        }

        Ok(Some(ret))
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

        let ret = Ref::new(_rp, ConstantPatternSyntax { parent: None, start: start, end: end, constant: constant.unwrap() });

        constant.unwrap().parent = Some(ParentNode::ConstantPattern(ret));

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

        let success_underscore_1 = self.lexer.parse_punctuation(String::from_string_slice(_r.page, "_"));
        if success_underscore_1 {
            self.lexer.advance(&_r)
        } else {
            return Ok(None)
        }

        let end: Position = self.lexer.get_position();

        let ret = Ref::new(_rp, WildcardPatternSyntax { parent: None, start: start, end: end });

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

        let ret = Ref::new(_rp, NamePatternSyntax { parent: None, start: start, end: end, name: name.unwrap() });

        name.unwrap().parent = Some(ParentNode::NamePattern(ret));

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

        let success_default_1 = self.lexer.parse_keyword(String::from_string_slice(_r.page, "default"));
        if success_default_1 {
            self.lexer.advance(&_r)
        } else {
            return Ok(None)
        }

        let end: Position = self.lexer.get_position();

        let ret = Ref::new(_rp, DefaultCaseLabelSyntax { parent: None, start: start, end: end });

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

        let success_for_1 = self.lexer.parse_keyword(String::from_string_slice(_r.page, "for"));
        if success_for_1 {
            self.lexer.advance(&_r)
        } else {
            return Ok(None)
        }

        let success_left_paren_2 = self.lexer.parse_punctuation(String::from_string_slice(_r.page, "("));
        if success_left_paren_2 {
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

        let type_annotation = self.parse_typeannotation(&_r, _rp, _ep)?;

        let success_in_5 = self.lexer.parse_keyword(String::from_string_slice(_r.page, "in"));
        if success_in_5 {
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

        let success_right_paren_7 = self.lexer.parse_punctuation(String::from_string_slice(_r.page, ")"));
        if success_right_paren_7 {
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

        let ret = Ref::new(_rp, ForSyntax { parent: None, start: start, end: end, index: index.unwrap(), type_annotation: type_annotation, operation: operation.unwrap(), iteration: iteration.unwrap() });

        match type_annotation {
            Some(mut x) => 
                x.parent = Some(ParentNode::For(ret)),
            None => ()
        }

        operation.unwrap().parent = Some(ParentNode::For(ret));

        match *iteration.unwrap() {
            LoopSyntax::SimpleLoop(mut x) => x.parent = Some(ParentNode::For(ret)),
            LoopSyntax::NamedLoop(mut x) => x.parent = Some(ParentNode::For(ret)),
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

        let success_while_1 = self.lexer.parse_keyword(String::from_string_slice(_r.page, "while"));
        if success_while_1 {
            self.lexer.advance(&_r)
        } else {
            return Ok(None)
        }

        let success_left_paren_2 = self.lexer.parse_punctuation(String::from_string_slice(_r.page, "("));
        if success_left_paren_2 {
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

        let success_right_paren_4 = self.lexer.parse_punctuation(String::from_string_slice(_r.page, ")"));
        if success_right_paren_4 {
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

        let ret = Ref::new(_rp, WhileSyntax { parent: None, start: start, end: end, condition: condition.unwrap(), iteration: iteration.unwrap() });

        condition.unwrap().parent = Some(ParentNode::While(ret));

        match *iteration.unwrap() {
            LoopSyntax::SimpleLoop(mut x) => x.parent = Some(ParentNode::While(ret)),
            LoopSyntax::NamedLoop(mut x) => x.parent = Some(ParentNode::While(ret)),
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

        let success_do_1 = self.lexer.parse_keyword(String::from_string_slice(_r.page, "do"));
        if success_do_1 {
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

        let success_while_3 = self.lexer.parse_keyword(String::from_string_slice(_r.page, "while"));
        if success_while_3 {
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

        let success_left_paren_4 = self.lexer.parse_punctuation(String::from_string_slice(_r.page, "("));
        if success_left_paren_4 {
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

        let success_right_paren_6 = self.lexer.parse_punctuation(String::from_string_slice(_r.page, ")"));
        if success_right_paren_6 {
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

        let ret = Ref::new(_rp, DoSyntax { parent: None, start: start, end: end, iteration: iteration.unwrap(), condition: condition.unwrap() });

        match *iteration.unwrap() {
            LoopSyntax::SimpleLoop(mut x) => x.parent = Some(ParentNode::Do(ret)),
            LoopSyntax::NamedLoop(mut x) => x.parent = Some(ParentNode::Do(ret)),
        }

        condition.unwrap().parent = Some(ParentNode::Do(ret));

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

        let ret = Ref::new(_rp, SimpleLoopSyntax { parent: None, start: start, end: end, code: code.unwrap() });

        code.unwrap().parent = Some(ParentNode::SimpleLoop(ret));

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

        let success_loop_1 = self.lexer.parse_keyword(String::from_string_slice(_r.page, "loop"));
        if success_loop_1 {
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

        let ret = Ref::new(_rp, NamedLoopSyntax { parent: None, start: start, end: end, name: name.unwrap(), code: code.unwrap() });

        code.unwrap().parent = Some(ParentNode::NamedLoop(ret));

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

        let success_this_1 = self.lexer.parse_keyword(String::from_string_slice(_r.page, "this"));
        if success_this_1 {
            self.lexer.advance(&_r)
        } else {
            return Ok(None)
        }

        let end: Position = self.lexer.get_position();

        let ret = Ref::new(_rp, ThisSyntax { parent: None, start: start, end: end });

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

        let success_new_1 = self.lexer.parse_keyword(String::from_string_slice(_r.page, "new"));
        if success_new_1 {
            self.lexer.advance(&_r)
        } else {
            return Ok(None)
        }

        let type_spec = self.parse_type(&_r, _rp, _ep)?;
        match type_spec {
            None =>
                return Err(Ref::new(_ep, ParserError { file_name: String::copy(_ep, self.file_name), line: self.lexer.line, column: self.lexer.column })),
            Some(_) => ()
        }

        let end: Position = self.lexer.get_position();

        let ret = Ref::new(_rp, NewSyntax { parent: None, start: start, end: end, type_spec: type_spec.unwrap() });

        type_spec.unwrap().parent = Some(ParentNode::New(ret));

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

        let success_left_paren_1 = self.lexer.parse_punctuation(String::from_string_slice(_r.page, "("));
        if success_left_paren_1 {
            self.lexer.advance(&_r)
        } else {
            return Ok(None)
        }

        let first_op = self.parse_operation(&_r, _rp, _ep)?;

        let additional_ops = self.parse_item_list(&_r, _rp, _ep)?;

        let success_right_paren_4 = self.lexer.parse_punctuation(String::from_string_slice(_r.page, ")"));
        if success_right_paren_4 {
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

        let ret = Ref::new(_rp, ObjectSyntax { parent: None, start: start, end: end, first_op: first_op, additional_ops: additional_ops });

        match first_op {
            Some(mut x) => 
                x.parent = Some(ParentNode::Object(ret)),
            None => ()
        }

        match additional_ops {
            Some(mut x) => for item in x.iter_mut() {
                item.parent = Some(ParentNode::Object(ret));
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

        let success_left_bracket_1 = self.lexer.parse_punctuation(String::from_string_slice(_r.page, "["));
        if success_left_bracket_1 {
            self.lexer.advance(&_r)
        } else {
            return Ok(None)
        }

        let first_op = self.parse_operation(&_r, _rp, _ep)?;

        let additional_ops = self.parse_item_list(&_r, _rp, _ep)?;

        let success_right_bracket_4 = self.lexer.parse_punctuation(String::from_string_slice(_r.page, "]"));
        if success_right_bracket_4 {
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

        let ret = Ref::new(_rp, ArraySyntax { parent: None, start: start, end: end, first_op: first_op, additional_ops: additional_ops });

        match first_op {
            Some(mut x) => 
                x.parent = Some(ParentNode::Array(ret)),
            None => ()
        }

        match additional_ops {
            Some(mut x) => for item in x.iter_mut() {
                item.parent = Some(ParentNode::Array(ret));
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

        let success_comma_1 = self.lexer.parse_punctuation(String::from_string_slice(_r.page, ","));
        if success_comma_1 {
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

        let ret = Ref::new(_rp, ItemSyntax { parent: None, start: start, end: end, operation: operation.unwrap() });

        operation.unwrap().parent = Some(ParentNode::Item(ret));

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

        let success_sizeof_1 = self.lexer.parse_keyword(String::from_string_slice(_r.page, "sizeof"));
        if success_sizeof_1 {
            self.lexer.advance(&_r)
        } else {
            return Ok(None)
        }

        let type_spec = self.parse_type(&_r, _rp, _ep)?;
        match type_spec {
            None =>
                return Err(Ref::new(_ep, ParserError { file_name: String::copy(_ep, self.file_name), line: self.lexer.line, column: self.lexer.column })),
            Some(_) => ()
        }

        let end: Position = self.lexer.get_position();

        let ret = Ref::new(_rp, SizeOfSyntax { parent: None, start: start, end: end, type_spec: type_spec.unwrap() });

        type_spec.unwrap().parent = Some(ParentNode::SizeOf(ret));

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

        let success_break_1 = self.lexer.parse_keyword(String::from_string_slice(_r.page, "break"));
        if success_break_1 {
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

        let success_semicolon_3 = self.lexer.parse_punctuation(String::from_string_slice(_r.page, ";"));
        if success_semicolon_3 {
            self.lexer.advance(&_r)
        } else {
()
        }

        let end: Position = self.lexer.get_position();

        let ret = Ref::new(_rp, BreakSyntax { parent: None, start: start, end: end, iteration: iteration });

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

        let success_continue_1 = self.lexer.parse_keyword(String::from_string_slice(_r.page, "continue"));
        if success_continue_1 {
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

        let success_semicolon_3 = self.lexer.parse_punctuation(String::from_string_slice(_r.page, ";"));
        if success_semicolon_3 {
            self.lexer.advance(&_r)
        } else {
()
        }

        let end: Position = self.lexer.get_position();

        let ret = Ref::new(_rp, ContinueSyntax { parent: None, start: start, end: end, iteration: iteration });

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

        let success_return_1 = self.lexer.parse_keyword(String::from_string_slice(_r.page, "return"));
        if success_return_1 {
            self.lexer.advance(&_r)
        } else {
            return Ok(None)
        }

        let result = self.parse_calculation(&_r, _rp, _ep)?;

        let end: Position = self.lexer.get_position();

        let ret = Ref::new(_rp, ReturnSyntax { parent: None, start: start, end: end, result: result });

        match result {
            Some(mut x) => 
                x.parent = Some(ParentNode::Return(ret)),
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

        let success_throw_1 = self.lexer.parse_keyword(String::from_string_slice(_r.page, "throw"));
        if success_throw_1 {
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

        let ret = Ref::new(_rp, ThrowSyntax { parent: None, start: start, end: end, exception: exception.unwrap() });

        exception.unwrap().parent = Some(ParentNode::Throw(ret));

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

        let success_class_1 = self.lexer.parse_keyword(String::from_string_slice(_r.page, "class"));
        if success_class_1 {
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

        let base_class = self.parse_extends(&_r, _rp, _ep)?;

        let contents = self.parse_structure(&_r, _rp, _ep)?;

        let body = self.parse_classbody(&_r, _rp, _ep)?;

        let end: Position = self.lexer.get_position();

        let ret = Ref::new(_rp, ClassSyntax { parent: None, start: start, end: end, name: name.unwrap(), generics: generics, base_class: base_class, contents: contents, body: body });

        name.unwrap().parent = Some(ParentNode::Class(ret));

        match generics {
            Some(mut x) => 
                x.parent = Some(ParentNode::Class(ret)),
            None => ()
        }

        match base_class {
            Some(mut x) => 
                x.parent = Some(ParentNode::Class(ret)),
            None => ()
        }

        match contents {
            Some(mut x) => 
                x.parent = Some(ParentNode::Class(ret)),
            None => ()
        }

        match body {
            Some(mut x) => 
                x.parent = Some(ParentNode::Class(ret)),
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

        let success_left_bracket_1 = self.lexer.parse_punctuation(String::from_string_slice(_r.page, "["));
        if success_left_bracket_1 {
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

        let additional_generics = self.parse_genericparameter_list(&_r, _rp, _ep)?;

        let success_right_bracket_4 = self.lexer.parse_punctuation(String::from_string_slice(_r.page, "]"));
        if success_right_bracket_4 {
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

        let ret = Ref::new(_rp, GenericParametersSyntax { parent: None, start: start, end: end, name: name.unwrap(), additional_generics: additional_generics });

        match additional_generics {
            Some(mut x) => for item in x.iter_mut() {
                item.parent = Some(ParentNode::GenericParameters(ret));
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

        let success_comma_1 = self.lexer.parse_punctuation(String::from_string_slice(_r.page, ","));
        if success_comma_1 {
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

        let ret = Ref::new(_rp, GenericParameterSyntax { parent: None, start: start, end: end, name: name.unwrap() });

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

        let success_extends_1 = self.lexer.parse_keyword(String::from_string_slice(_r.page, "extends"));
        if success_extends_1 {
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

        let ret = Ref::new(_rp, ExtendsSyntax { parent: None, start: start, end: end, name: name.unwrap() });

        name.unwrap().parent = Some(ParentNode::Extends(ret));

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

        let success_left_paren_1 = self.lexer.parse_punctuation(String::from_string_slice(_r.page, "("));
        if success_left_paren_1 {
            self.lexer.advance(&_r)
        } else {
            return Ok(None)
        }

        let components = self.parse_component_list(&_r, _rp, _ep)?;

        let success_right_paren_3 = self.lexer.parse_punctuation(String::from_string_slice(_r.page, ")"));
        if success_right_paren_3 {
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

        let ret = Ref::new(_rp, StructureSyntax { parent: None, start: start, end: end, components: components });

        match components {
            Some(mut x) => for item in x.iter_mut() {
                item.parent = Some(ParentNode::Structure(ret));
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

        let type_annotation = self.parse_typeannotation(&_r, _rp, _ep)?;

        let success_comma_3 = self.lexer.parse_punctuation(String::from_string_slice(_r.page, ","));
        if success_comma_3 {
            self.lexer.advance(&_r)
        } else {
()
        }

        let end: Position = self.lexer.get_position();

        let ret = Ref::new(_rp, ComponentSyntax { parent: None, start: start, end: end, name: name.unwrap(), type_annotation: type_annotation });

        match type_annotation {
            Some(mut x) => 
                x.parent = Some(ParentNode::Component(ret)),
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

        let success_left_curly_1 = self.lexer.parse_punctuation(String::from_string_slice(_r.page, "{"));
        if success_left_curly_1 {
            self.lexer.advance(&_r)
        } else {
            return Ok(None)
        }

        let members = self.parse_classmember_list(&_r, _rp, _ep)?;

        let success_right_curly_3 = self.lexer.parse_punctuation(String::from_string_slice(_r.page, "}"));
        if success_right_curly_3 {
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

        let ret = Ref::new(_rp, ClassBodySyntax { parent: None, start: start, end: end, members: members });

        match members {
            Some(mut x) => for item in x.iter_mut() {
                match **item {
                    ClassMemberSyntax::LetMember(mut y) => y.parent = Some(ParentNode::ClassBody(ret)),
                    ClassMemberSyntax::VarMember(mut y) => y.parent = Some(ParentNode::ClassBody(ret)),
                    ClassMemberSyntax::MutableMember(mut y) => y.parent = Some(ParentNode::ClassBody(ret)),
                    ClassMemberSyntax::SetInitialization(mut y) => y.parent = Some(ParentNode::ClassBody(ret)),
                    ClassMemberSyntax::Method(mut y) => y.parent = Some(ParentNode::ClassBody(ret)),
                    ClassMemberSyntax::StaticFunction(mut y) => y.parent = Some(ParentNode::ClassBody(ret)),
                    ClassMemberSyntax::Operator(mut y) => y.parent = Some(ParentNode::ClassBody(ret)),
                    ClassMemberSyntax::Initializer(mut y) => y.parent = Some(ParentNode::ClassBody(ret)),
                    ClassMemberSyntax::Allocator(mut y) => y.parent = Some(ParentNode::ClassBody(ret)),
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

        let ret = Ref::new(_rp, LetMemberSyntax { parent: None, start: start, end: end, declaration: declaration.unwrap() });

        declaration.unwrap().parent = Some(ParentNode::LetMember(ret));

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

        let ret = Ref::new(_rp, VarMemberSyntax { parent: None, start: start, end: end, declaration: declaration.unwrap() });

        declaration.unwrap().parent = Some(ParentNode::VarMember(ret));

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

        let ret = Ref::new(_rp, MutableMemberSyntax { parent: None, start: start, end: end, declaration: declaration.unwrap() });

        declaration.unwrap().parent = Some(ParentNode::MutableMember(ret));

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

        let ret = Ref::new(_rp, SetInitializationSyntax { parent: None, start: start, end: end, definition: definition.unwrap() });

        definition.unwrap().parent = Some(ParentNode::SetInitialization(ret));

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

        let success_method_1 = self.lexer.parse_keyword(String::from_string_slice(_r.page, "method"));
        if success_method_1 {
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

        let ret = Ref::new(_rp, MethodSyntax { parent: None, start: start, end: end, procedure: procedure.unwrap() });

        procedure.unwrap().parent = Some(ParentNode::Method(ret));

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

        let ret = Ref::new(_rp, StaticFunctionSyntax { parent: None, start: start, end: end, procedure: procedure.unwrap() });

        procedure.unwrap().parent = Some(ParentNode::StaticFunction(ret));

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

        let success_operator_1 = self.lexer.parse_keyword(String::from_string_slice(_r.page, "operator"));
        if success_operator_1 {
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

        let ret = Ref::new(_rp, OperatorSyntax { parent: None, start: start, end: end, routine: routine.unwrap() });

        routine.unwrap().parent = Some(ParentNode::Operator(ret));

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

        let success_initializer_1 = self.lexer.parse_keyword(String::from_string_slice(_r.page, "initializer"));
        if success_initializer_1 {
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

        let ret = Ref::new(_rp, InitializerSyntax { parent: None, start: start, end: end, input: input, body: body.unwrap() });

        match input {
            Some(mut x) => 
                x.parent = Some(ParentNode::Initializer(ret)),
            None => ()
        }

        body.unwrap().parent = Some(ParentNode::Initializer(ret));

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

        let success_allocator_1 = self.lexer.parse_keyword(String::from_string_slice(_r.page, "allocator"));
        if success_allocator_1 {
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

        let ret = Ref::new(_rp, AllocatorSyntax { parent: None, start: start, end: end, input: input, body: body.unwrap() });

        match input {
            Some(mut x) => 
                x.parent = Some(ParentNode::Allocator(ret)),
            None => ()
        }

        body.unwrap().parent = Some(ParentNode::Allocator(ret));

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

        let success_colon_1 = self.lexer.parse_punctuation(String::from_string_slice(_r.page, ":"));
        if success_colon_1 {
            self.lexer.advance(&_r)
        } else {
            return Ok(None)
        }

        let type_spec = self.parse_typespec(&_r, _rp, _ep)?;
        match type_spec {
            None =>
                return Err(Ref::new(_ep, ParserError { file_name: String::copy(_ep, self.file_name), line: self.lexer.line, column: self.lexer.column })),
            Some(_) => ()
        }

        let end: Position = self.lexer.get_position();

        let ret = Ref::new(_rp, TypeAnnotationSyntax { parent: None, start: start, end: end, type_spec: type_spec.unwrap() });

        match *type_spec.unwrap() {
            TypeSpecSyntax::Type(mut x) => x.parent = Some(ParentNode::TypeAnnotation(ret)),
            TypeSpecSyntax::Variant(mut x) => x.parent = Some(ParentNode::TypeAnnotation(ret)),
        }

        Ok(Some(ret))
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

        let life_time = self.parse_lifetime(&_r, _rp, _ep)?;

        let end: Position = self.lexer.get_position();

        let ret = Ref::new(_rp, TypeSyntax { parent: None, start: start, end: end, name: name.unwrap(), generics: generics, optional: optional, life_time: life_time });

        name.unwrap().parent = Some(ParentNode::Type(ret));

        match generics {
            Some(mut x) => 
                x.parent = Some(ParentNode::Type(ret)),
            None => ()
        }

        match optional {
            Some(mut x) => 
                x.parent = Some(ParentNode::Type(ret)),
            None => ()
        }

        match life_time {
            Some(x) => 
                match *x {
                    LifeTimeSyntax::Root(mut y) => y.parent = Some(ParentNode::Type(ret)),
                    LifeTimeSyntax::Local(mut y) => y.parent = Some(ParentNode::Type(ret)),
                    LifeTimeSyntax::Reference(mut y) => y.parent = Some(ParentNode::Type(ret)),
                    LifeTimeSyntax::Thrown(mut y) => y.parent = Some(ParentNode::Type(ret)),
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

        let success_left_paren_1 = self.lexer.parse_punctuation(String::from_string_slice(_r.page, "("));
        if success_left_paren_1 {
            self.lexer.advance(&_r)
        } else {
            return Ok(None)
        }

        let types = self.parse_type_list(&_r, _rp, _ep)?;

        let success_right_paren_3 = self.lexer.parse_punctuation(String::from_string_slice(_r.page, ")"));
        if success_right_paren_3 {
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

        let ret = Ref::new(_rp, VariantSyntax { parent: None, start: start, end: end, types: types });

        match types {
            Some(mut x) => for item in x.iter_mut() {
                item.parent = Some(ParentNode::Variant(ret));
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

        let success_throws_1 = self.lexer.parse_keyword(String::from_string_slice(_r.page, "throws"));
        if success_throws_1 {
            self.lexer.advance(&_r)
        } else {
            return Ok(None)
        }

        let throws_type = self.parse_typespec(&_r, _rp, _ep)?;
        match throws_type {
            None =>
                return Err(Ref::new(_ep, ParserError { file_name: String::copy(_ep, self.file_name), line: self.lexer.line, column: self.lexer.column })),
            Some(_) => ()
        }

        let end: Position = self.lexer.get_position();

        let ret = Ref::new(_rp, ThrowsSyntax { parent: None, start: start, end: end, throws_type: throws_type.unwrap() });

        match *throws_type.unwrap() {
            TypeSpecSyntax::Type(mut x) => x.parent = Some(ParentNode::Throws(ret)),
            TypeSpecSyntax::Variant(mut x) => x.parent = Some(ParentNode::Throws(ret)),
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

        let success_left_bracket_1 = self.lexer.parse_punctuation(String::from_string_slice(_r.page, "["));
        if success_left_bracket_1 {
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

        let additional_generics = self.parse_genericargument_list(&_r, _rp, _ep)?;

        let success_right_bracket_4 = self.lexer.parse_punctuation(String::from_string_slice(_r.page, "]"));
        if success_right_bracket_4 {
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

        let ret = Ref::new(_rp, GenericArgumentsSyntax { parent: None, start: start, end: end, generic: generic.unwrap(), additional_generics: additional_generics });

        generic.unwrap().parent = Some(ParentNode::GenericArguments(ret));

        match additional_generics {
            Some(mut x) => for item in x.iter_mut() {
                item.parent = Some(ParentNode::GenericArguments(ret));
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

        let success_comma_1 = self.lexer.parse_punctuation(String::from_string_slice(_r.page, ","));
        if success_comma_1 {
            self.lexer.advance(&_r)
        } else {
            return Ok(None)
        }

        let type_spec = self.parse_type(&_r, _rp, _ep)?;
        match type_spec {
            None =>
                return Err(Ref::new(_ep, ParserError { file_name: String::copy(_ep, self.file_name), line: self.lexer.line, column: self.lexer.column })),
            Some(_) => ()
        }

        let end: Position = self.lexer.get_position();

        let ret = Ref::new(_rp, GenericArgumentSyntax { parent: None, start: start, end: end, type_spec: type_spec.unwrap() });

        type_spec.unwrap().parent = Some(ParentNode::GenericArgument(ret));

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

        let success_question_1 = self.lexer.parse_punctuation(String::from_string_slice(_r.page, "?"));
        if success_question_1 {
            self.lexer.advance(&_r)
        } else {
            return Ok(None)
        }

        let end: Position = self.lexer.get_position();

        let ret = Ref::new(_rp, OptionalSyntax { parent: None, start: start, end: end });

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

        let success_dollar_1 = self.lexer.parse_punctuation(String::from_string_slice(_r.page, "$"));
        if success_dollar_1 {
            self.lexer.advance(&_r)
        } else {
            return Ok(None)
        }

        let end: Position = self.lexer.get_position();

        let ret = Ref::new(_rp, RootSyntax { parent: None, start: start, end: end });

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

        let success_at_1 = self.lexer.parse_punctuation(String::from_string_slice(_r.page, "@"));
        if success_at_1 {
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

        let ret = Ref::new(_rp, LocalSyntax { parent: None, start: start, end: end, location: location.unwrap() });

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

        let success_backtick_1 = self.lexer.parse_punctuation(String::from_string_slice(_r.page, "`"));
        if success_backtick_1 {
            self.lexer.advance(&_r)
        } else {
            return Ok(None)
        }

        let age = self.lexer.parse_literal(_rp);
        match age {
            Some(_) =>
                self.lexer.advance(&_r),
            None => ()
        }

        let end: Position = self.lexer.get_position();

        let ret = Ref::new(_rp, ReferenceSyntax { parent: None, start: start, end: end, age: age });

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

        let success_hash_1 = self.lexer.parse_punctuation(String::from_string_slice(_r.page, "#"));
        if success_hash_1 {
            self.lexer.advance(&_r)
        } else {
            return Ok(None)
        }

        let end: Position = self.lexer.get_position();

        let ret = Ref::new(_rp, ThrownSyntax { parent: None, start: start, end: end });

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

    fn open_using(&mut self, using_syntax: Ref<UsingSyntax>) -> bool;
    fn close_using(&mut self, using_syntax: Ref<UsingSyntax>);

    fn open_define(&mut self, define_syntax: Ref<DefineSyntax>) -> bool;
    fn close_define(&mut self, define_syntax: Ref<DefineSyntax>);

    fn open_name(&mut self, name_syntax: Ref<NameSyntax>) -> bool;
    fn close_name(&mut self, name_syntax: Ref<NameSyntax>);

    fn visit_extension(&mut self, extension_syntax: Ref<ExtensionSyntax>);

    fn open_namespace(&mut self, namespace_syntax: Ref<NamespaceSyntax>) -> bool;
    fn close_namespace(&mut self, namespace_syntax: Ref<NamespaceSyntax>);

    fn open_function(&mut self, function_syntax: Ref<FunctionSyntax>) -> bool;
    fn close_function(&mut self, function_syntax: Ref<FunctionSyntax>);

    fn open_procedure(&mut self, procedure_syntax: Ref<ProcedureSyntax>) -> bool;
    fn close_procedure(&mut self, procedure_syntax: Ref<ProcedureSyntax>);

    fn open_routine(&mut self, routine_syntax: Ref<RoutineSyntax>) -> bool;
    fn close_routine(&mut self, routine_syntax: Ref<RoutineSyntax>);

    fn open_letdeclaration(&mut self, letdeclaration_syntax: Ref<LetDeclarationSyntax>) -> bool;
    fn close_letdeclaration(&mut self, letdeclaration_syntax: Ref<LetDeclarationSyntax>);

    fn open_vardeclaration(&mut self, vardeclaration_syntax: Ref<VarDeclarationSyntax>) -> bool;
    fn close_vardeclaration(&mut self, vardeclaration_syntax: Ref<VarDeclarationSyntax>);

    fn open_mutabledeclaration(&mut self, mutabledeclaration_syntax: Ref<MutableDeclarationSyntax>) -> bool;
    fn close_mutabledeclaration(&mut self, mutabledeclaration_syntax: Ref<MutableDeclarationSyntax>);

    fn open_threadlocaldeclaration(&mut self, threadlocaldeclaration_syntax: Ref<ThreadLocalDeclarationSyntax>) -> bool;
    fn close_threadlocaldeclaration(&mut self, threadlocaldeclaration_syntax: Ref<ThreadLocalDeclarationSyntax>);

    fn open_let(&mut self, let_syntax: Ref<LetSyntax>) -> bool;
    fn close_let(&mut self, let_syntax: Ref<LetSyntax>);

    fn open_var(&mut self, var_syntax: Ref<VarSyntax>) -> bool;
    fn close_var(&mut self, var_syntax: Ref<VarSyntax>);

    fn open_mutable(&mut self, mutable_syntax: Ref<MutableSyntax>) -> bool;
    fn close_mutable(&mut self, mutable_syntax: Ref<MutableSyntax>);

    fn open_threadlocal(&mut self, threadlocal_syntax: Ref<ThreadLocalSyntax>) -> bool;
    fn close_threadlocal(&mut self, threadlocal_syntax: Ref<ThreadLocalSyntax>);

    fn open_binding(&mut self, binding_syntax: Ref<BindingSyntax>) -> bool;
    fn close_binding(&mut self, binding_syntax: Ref<BindingSyntax>);

    fn open_set(&mut self, set_syntax: Ref<SetSyntax>) -> bool;
    fn close_set(&mut self, set_syntax: Ref<SetSyntax>);

    fn open_calculation(&mut self, calculation_syntax: Ref<CalculationSyntax>) -> bool;
    fn close_calculation(&mut self, calculation_syntax: Ref<CalculationSyntax>);

    fn open_operation(&mut self, operation_syntax: Ref<OperationSyntax>) -> bool;
    fn close_operation(&mut self, operation_syntax: Ref<OperationSyntax>);

    fn open_operand(&mut self, operand_syntax: Ref<OperandSyntax>) -> bool;
    fn close_operand(&mut self, operand_syntax: Ref<OperandSyntax>);

    fn visit_memberaccess(&mut self, memberaccess_syntax: Ref<MemberAccessSyntax>);

    fn open_as(&mut self, as_syntax: Ref<AsSyntax>) -> bool;
    fn close_as(&mut self, as_syntax: Ref<AsSyntax>);

    fn open_is(&mut self, is_syntax: Ref<IsSyntax>) -> bool;
    fn close_is(&mut self, is_syntax: Ref<IsSyntax>);

    fn visit_unwrap(&mut self, unwrap_syntax: Ref<UnwrapSyntax>);

    fn open_catch(&mut self, catch_syntax: Ref<CatchSyntax>) -> bool;
    fn close_catch(&mut self, catch_syntax: Ref<CatchSyntax>);

    fn open_wildcardcatchpattern(&mut self, wildcardcatchpattern_syntax: Ref<WildCardCatchPatternSyntax>) -> bool;
    fn close_wildcardcatchpattern(&mut self, wildcardcatchpattern_syntax: Ref<WildCardCatchPatternSyntax>);

    fn open_typecatchpattern(&mut self, typecatchpattern_syntax: Ref<TypeCatchPatternSyntax>) -> bool;
    fn close_typecatchpattern(&mut self, typecatchpattern_syntax: Ref<TypeCatchPatternSyntax>);

    fn open_block(&mut self, block_syntax: Ref<BlockSyntax>) -> bool;
    fn close_block(&mut self, block_syntax: Ref<BlockSyntax>);

    fn visit_constant(&mut self, constant_syntax: Ref<ConstantSyntax>);

    fn open_if(&mut self, if_syntax: Ref<IfSyntax>) -> bool;
    fn close_if(&mut self, if_syntax: Ref<IfSyntax>);

    fn open_else(&mut self, else_syntax: Ref<ElseSyntax>) -> bool;
    fn close_else(&mut self, else_syntax: Ref<ElseSyntax>);

    fn open_switch(&mut self, switch_syntax: Ref<SwitchSyntax>) -> bool;
    fn close_switch(&mut self, switch_syntax: Ref<SwitchSyntax>);

    fn open_switchcase(&mut self, switchcase_syntax: Ref<SwitchCaseSyntax>) -> bool;
    fn close_switchcase(&mut self, switchcase_syntax: Ref<SwitchCaseSyntax>);

    fn open_itemcaselabel(&mut self, itemcaselabel_syntax: Ref<ItemCaseLabelSyntax>) -> bool;
    fn close_itemcaselabel(&mut self, itemcaselabel_syntax: Ref<ItemCaseLabelSyntax>);

    fn open_caseitem(&mut self, caseitem_syntax: Ref<CaseItemSyntax>) -> bool;
    fn close_caseitem(&mut self, caseitem_syntax: Ref<CaseItemSyntax>);

    fn open_constantpattern(&mut self, constantpattern_syntax: Ref<ConstantPatternSyntax>) -> bool;
    fn close_constantpattern(&mut self, constantpattern_syntax: Ref<ConstantPatternSyntax>);

    fn visit_wildcardpattern(&mut self, wildcardpattern_syntax: Ref<WildcardPatternSyntax>);

    fn open_namepattern(&mut self, namepattern_syntax: Ref<NamePatternSyntax>) -> bool;
    fn close_namepattern(&mut self, namepattern_syntax: Ref<NamePatternSyntax>);

    fn visit_defaultcaselabel(&mut self, defaultcaselabel_syntax: Ref<DefaultCaseLabelSyntax>);

    fn open_for(&mut self, for_syntax: Ref<ForSyntax>) -> bool;
    fn close_for(&mut self, for_syntax: Ref<ForSyntax>);

    fn open_while(&mut self, while_syntax: Ref<WhileSyntax>) -> bool;
    fn close_while(&mut self, while_syntax: Ref<WhileSyntax>);

    fn open_do(&mut self, do_syntax: Ref<DoSyntax>) -> bool;
    fn close_do(&mut self, do_syntax: Ref<DoSyntax>);

    fn open_simpleloop(&mut self, simpleloop_syntax: Ref<SimpleLoopSyntax>) -> bool;
    fn close_simpleloop(&mut self, simpleloop_syntax: Ref<SimpleLoopSyntax>);

    fn open_namedloop(&mut self, namedloop_syntax: Ref<NamedLoopSyntax>) -> bool;
    fn close_namedloop(&mut self, namedloop_syntax: Ref<NamedLoopSyntax>);

    fn visit_this(&mut self, this_syntax: Ref<ThisSyntax>);

    fn open_new(&mut self, new_syntax: Ref<NewSyntax>) -> bool;
    fn close_new(&mut self, new_syntax: Ref<NewSyntax>);

    fn open_object(&mut self, object_syntax: Ref<ObjectSyntax>) -> bool;
    fn close_object(&mut self, object_syntax: Ref<ObjectSyntax>);

    fn open_array(&mut self, array_syntax: Ref<ArraySyntax>) -> bool;
    fn close_array(&mut self, array_syntax: Ref<ArraySyntax>);

    fn open_item(&mut self, item_syntax: Ref<ItemSyntax>) -> bool;
    fn close_item(&mut self, item_syntax: Ref<ItemSyntax>);

    fn open_sizeof(&mut self, sizeof_syntax: Ref<SizeOfSyntax>) -> bool;
    fn close_sizeof(&mut self, sizeof_syntax: Ref<SizeOfSyntax>);

    fn visit_break(&mut self, break_syntax: Ref<BreakSyntax>);

    fn visit_continue(&mut self, continue_syntax: Ref<ContinueSyntax>);

    fn open_return(&mut self, return_syntax: Ref<ReturnSyntax>) -> bool;
    fn close_return(&mut self, return_syntax: Ref<ReturnSyntax>);

    fn open_throw(&mut self, throw_syntax: Ref<ThrowSyntax>) -> bool;
    fn close_throw(&mut self, throw_syntax: Ref<ThrowSyntax>);

    fn open_class(&mut self, class_syntax: Ref<ClassSyntax>) -> bool;
    fn close_class(&mut self, class_syntax: Ref<ClassSyntax>);

    fn open_genericparameters(&mut self, genericparameters_syntax: Ref<GenericParametersSyntax>) -> bool;
    fn close_genericparameters(&mut self, genericparameters_syntax: Ref<GenericParametersSyntax>);

    fn visit_genericparameter(&mut self, genericparameter_syntax: Ref<GenericParameterSyntax>);

    fn open_extends(&mut self, extends_syntax: Ref<ExtendsSyntax>) -> bool;
    fn close_extends(&mut self, extends_syntax: Ref<ExtendsSyntax>);

    fn open_structure(&mut self, structure_syntax: Ref<StructureSyntax>) -> bool;
    fn close_structure(&mut self, structure_syntax: Ref<StructureSyntax>);

    fn open_component(&mut self, component_syntax: Ref<ComponentSyntax>) -> bool;
    fn close_component(&mut self, component_syntax: Ref<ComponentSyntax>);

    fn open_classbody(&mut self, classbody_syntax: Ref<ClassBodySyntax>) -> bool;
    fn close_classbody(&mut self, classbody_syntax: Ref<ClassBodySyntax>);

    fn open_letmember(&mut self, letmember_syntax: Ref<LetMemberSyntax>) -> bool;
    fn close_letmember(&mut self, letmember_syntax: Ref<LetMemberSyntax>);

    fn open_varmember(&mut self, varmember_syntax: Ref<VarMemberSyntax>) -> bool;
    fn close_varmember(&mut self, varmember_syntax: Ref<VarMemberSyntax>);

    fn open_mutablemember(&mut self, mutablemember_syntax: Ref<MutableMemberSyntax>) -> bool;
    fn close_mutablemember(&mut self, mutablemember_syntax: Ref<MutableMemberSyntax>);

    fn open_setinitialization(&mut self, setinitialization_syntax: Ref<SetInitializationSyntax>) -> bool;
    fn close_setinitialization(&mut self, setinitialization_syntax: Ref<SetInitializationSyntax>);

    fn open_method(&mut self, method_syntax: Ref<MethodSyntax>) -> bool;
    fn close_method(&mut self, method_syntax: Ref<MethodSyntax>);

    fn open_staticfunction(&mut self, staticfunction_syntax: Ref<StaticFunctionSyntax>) -> bool;
    fn close_staticfunction(&mut self, staticfunction_syntax: Ref<StaticFunctionSyntax>);

    fn open_operator(&mut self, operator_syntax: Ref<OperatorSyntax>) -> bool;
    fn close_operator(&mut self, operator_syntax: Ref<OperatorSyntax>);

    fn open_initializer(&mut self, initializer_syntax: Ref<InitializerSyntax>) -> bool;
    fn close_initializer(&mut self, initializer_syntax: Ref<InitializerSyntax>);

    fn open_allocator(&mut self, allocator_syntax: Ref<AllocatorSyntax>) -> bool;
    fn close_allocator(&mut self, allocator_syntax: Ref<AllocatorSyntax>);

    fn open_typeannotation(&mut self, typeannotation_syntax: Ref<TypeAnnotationSyntax>) -> bool;
    fn close_typeannotation(&mut self, typeannotation_syntax: Ref<TypeAnnotationSyntax>);

    fn open_type(&mut self, type_syntax: Ref<TypeSyntax>) -> bool;
    fn close_type(&mut self, type_syntax: Ref<TypeSyntax>);

    fn open_variant(&mut self, variant_syntax: Ref<VariantSyntax>) -> bool;
    fn close_variant(&mut self, variant_syntax: Ref<VariantSyntax>);

    fn open_throws(&mut self, throws_syntax: Ref<ThrowsSyntax>) -> bool;
    fn close_throws(&mut self, throws_syntax: Ref<ThrowsSyntax>);

    fn open_genericarguments(&mut self, genericarguments_syntax: Ref<GenericArgumentsSyntax>) -> bool;
    fn close_genericarguments(&mut self, genericarguments_syntax: Ref<GenericArgumentsSyntax>);

    fn open_genericargument(&mut self, genericargument_syntax: Ref<GenericArgumentSyntax>) -> bool;
    fn close_genericargument(&mut self, genericargument_syntax: Ref<GenericArgumentSyntax>);

    fn visit_optional(&mut self, optional_syntax: Ref<OptionalSyntax>);

    fn visit_root(&mut self, root_syntax: Ref<RootSyntax>);

    fn visit_local(&mut self, local_syntax: Ref<LocalSyntax>);

    fn visit_reference(&mut self, reference_syntax: Ref<ReferenceSyntax>);

    fn visit_thrown(&mut self, thrown_syntax: Ref<ThrownSyntax>);
}

pub trait SyntaxNode {
    fn accept(&mut self, visitor: *mut SyntaxVisitor);
}

#[derive(Copy, Clone)]
pub enum ParentNode {
    File(Ref<FileSyntax>),
    Using(Ref<UsingSyntax>),
    Define(Ref<DefineSyntax>),
    Name(Ref<NameSyntax>),
    Namespace(Ref<NamespaceSyntax>),
    Function(Ref<FunctionSyntax>),
    Procedure(Ref<ProcedureSyntax>),
    Routine(Ref<RoutineSyntax>),
    LetDeclaration(Ref<LetDeclarationSyntax>),
    VarDeclaration(Ref<VarDeclarationSyntax>),
    MutableDeclaration(Ref<MutableDeclarationSyntax>),
    ThreadLocalDeclaration(Ref<ThreadLocalDeclarationSyntax>),
    Let(Ref<LetSyntax>),
    Var(Ref<VarSyntax>),
    Mutable(Ref<MutableSyntax>),
    ThreadLocal(Ref<ThreadLocalSyntax>),
    Binding(Ref<BindingSyntax>),
    Set(Ref<SetSyntax>),
    Calculation(Ref<CalculationSyntax>),
    Operation(Ref<OperationSyntax>),
    Operand(Ref<OperandSyntax>),
    As(Ref<AsSyntax>),
    Is(Ref<IsSyntax>),
    Catch(Ref<CatchSyntax>),
    WildCardCatchPattern(Ref<WildCardCatchPatternSyntax>),
    TypeCatchPattern(Ref<TypeCatchPatternSyntax>),
    Block(Ref<BlockSyntax>),
    If(Ref<IfSyntax>),
    Else(Ref<ElseSyntax>),
    Switch(Ref<SwitchSyntax>),
    SwitchCase(Ref<SwitchCaseSyntax>),
    ItemCaseLabel(Ref<ItemCaseLabelSyntax>),
    CaseItem(Ref<CaseItemSyntax>),
    ConstantPattern(Ref<ConstantPatternSyntax>),
    NamePattern(Ref<NamePatternSyntax>),
    For(Ref<ForSyntax>),
    While(Ref<WhileSyntax>),
    Do(Ref<DoSyntax>),
    SimpleLoop(Ref<SimpleLoopSyntax>),
    NamedLoop(Ref<NamedLoopSyntax>),
    New(Ref<NewSyntax>),
    Object(Ref<ObjectSyntax>),
    Array(Ref<ArraySyntax>),
    Item(Ref<ItemSyntax>),
    SizeOf(Ref<SizeOfSyntax>),
    Return(Ref<ReturnSyntax>),
    Throw(Ref<ThrowSyntax>),
    Class(Ref<ClassSyntax>),
    GenericParameters(Ref<GenericParametersSyntax>),
    Extends(Ref<ExtendsSyntax>),
    Structure(Ref<StructureSyntax>),
    Component(Ref<ComponentSyntax>),
    ClassBody(Ref<ClassBodySyntax>),
    LetMember(Ref<LetMemberSyntax>),
    VarMember(Ref<VarMemberSyntax>),
    MutableMember(Ref<MutableMemberSyntax>),
    SetInitialization(Ref<SetInitializationSyntax>),
    Method(Ref<MethodSyntax>),
    StaticFunction(Ref<StaticFunctionSyntax>),
    Operator(Ref<OperatorSyntax>),
    Initializer(Ref<InitializerSyntax>),
    Allocator(Ref<AllocatorSyntax>),
    TypeAnnotation(Ref<TypeAnnotationSyntax>),
    Type(Ref<TypeSyntax>),
    Variant(Ref<VariantSyntax>),
    Throws(Ref<ThrowsSyntax>),
    GenericArguments(Ref<GenericArgumentsSyntax>),
    GenericArgument(Ref<GenericArgumentSyntax>),
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
    pub usings: Option<Ref<Vector<Ref<UsingSyntax>>>>,
    pub defines: Option<Ref<Vector<Ref<DefineSyntax>>>>,
    pub declarations: Option<Ref<Vector<Ref<DeclarationSyntax>>>>,
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

        match self.usings {
            Some(mut x) => for node in x.iter_mut() {
                node.accept(visitor)
            },
            None => ()
        };

        match self.defines {
            Some(mut x) => for node in x.iter_mut() {
                node.accept(visitor)
            },
            None => ()
        };

        match self.declarations {
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
pub struct UsingSyntax {
    pub start: Position,
    pub end: Position,
    pub parent: Option<ParentNode>,
    pub name: Ref<NameSyntax>,
}

impl SyntaxNode for UsingSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        unsafe {
            if !(*visitor).open_using(Ref::from(self as *mut UsingSyntax)) {
                return
            }
        }
        self.name.accept(visitor);
            unsafe {
            (*visitor).close_using(Ref::from(self as *mut UsingSyntax))
        }
    }
}

#[derive(Copy, Clone)]
pub struct DefineSyntax {
    pub start: Position,
    pub end: Position,
    pub parent: Option<ParentNode>,
    pub name: Ref<NameSyntax>,
    pub type_spec: Ref<TypeSyntax>,
}

impl SyntaxNode for DefineSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        unsafe {
            if !(*visitor).open_define(Ref::from(self as *mut DefineSyntax)) {
                return
            }
        }
        self.name.accept(visitor);
    
        self.type_spec.accept(visitor);
            unsafe {
            (*visitor).close_define(Ref::from(self as *mut DefineSyntax))
        }
    }
}

#[derive(Copy, Clone)]
pub struct NameSyntax {
    pub start: Position,
    pub end: Position,
    pub parent: Option<ParentNode>,
    pub name: String,
    pub extensions: Option<Ref<Vector<Ref<ExtensionSyntax>>>>,
}

impl SyntaxNode for NameSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        unsafe {
            if !(*visitor).open_name(Ref::from(self as *mut NameSyntax)) {
                return
            }
        }
        match self.extensions {
            Some(mut x) => for node in x.iter_mut() {
                node.accept(visitor)
            },
            None => ()
        };
        unsafe {
            (*visitor).close_name(Ref::from(self as *mut NameSyntax))
        }
    }
}

#[derive(Copy, Clone)]
pub struct ExtensionSyntax {
    pub start: Position,
    pub end: Position,
    pub parent: Option<ParentNode>,
    pub name: String,
}

impl SyntaxNode for ExtensionSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        unsafe {
            (*visitor).visit_extension(Ref::from(self as *mut ExtensionSyntax))
        }
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
        match self {
            DeclarationSyntax::Namespace(syntax) => syntax.accept(visitor),
            DeclarationSyntax::Function(syntax) => syntax.accept(visitor),
            DeclarationSyntax::Class(syntax) => syntax.accept(visitor),
            DeclarationSyntax::LetDeclaration(syntax) => syntax.accept(visitor),
            DeclarationSyntax::VarDeclaration(syntax) => syntax.accept(visitor),
            DeclarationSyntax::MutableDeclaration(syntax) => syntax.accept(visitor),
            DeclarationSyntax::ThreadLocalDeclaration(syntax) => syntax.accept(visitor),
        }
    }
}

#[derive(Copy, Clone)]
pub struct NamespaceSyntax {
    pub start: Position,
    pub end: Position,
    pub parent: Option<ParentNode>,
    pub name: Ref<NameSyntax>,
    pub usings: Option<Ref<Vector<Ref<UsingSyntax>>>>,
    pub defines: Option<Ref<Vector<Ref<DefineSyntax>>>>,
    pub declarations: Option<Ref<Vector<Ref<DeclarationSyntax>>>>,
}

impl SyntaxNode for NamespaceSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        unsafe {
            if !(*visitor).open_namespace(Ref::from(self as *mut NamespaceSyntax)) {
                return
            }
        }
        self.name.accept(visitor);
    
        match self.usings {
            Some(mut x) => for node in x.iter_mut() {
                node.accept(visitor)
            },
            None => ()
        };

        match self.defines {
            Some(mut x) => for node in x.iter_mut() {
                node.accept(visitor)
            },
            None => ()
        };

        match self.declarations {
            Some(mut x) => for node in x.iter_mut() {
                node.accept(visitor)
            },
            None => ()
        };
        unsafe {
            (*visitor).close_namespace(Ref::from(self as *mut NamespaceSyntax))
        }
    }
}

#[derive(Copy, Clone)]
pub struct FunctionSyntax {
    pub start: Position,
    pub end: Position,
    pub parent: Option<ParentNode>,
    pub procedure: Ref<ProcedureSyntax>,
}

impl SyntaxNode for FunctionSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        unsafe {
            if !(*visitor).open_function(Ref::from(self as *mut FunctionSyntax)) {
                return
            }
        }
        self.procedure.accept(visitor);
            unsafe {
            (*visitor).close_function(Ref::from(self as *mut FunctionSyntax))
        }
    }
}

#[derive(Copy, Clone)]
pub struct ProcedureSyntax {
    pub start: Position,
    pub end: Position,
    pub parent: Option<ParentNode>,
    pub name: String,
    pub routine: Ref<RoutineSyntax>,
}

impl SyntaxNode for ProcedureSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        unsafe {
            if !(*visitor).open_procedure(Ref::from(self as *mut ProcedureSyntax)) {
                return
            }
        }
        self.routine.accept(visitor);
            unsafe {
            (*visitor).close_procedure(Ref::from(self as *mut ProcedureSyntax))
        }
    }
}

#[derive(Copy, Clone)]
pub struct RoutineSyntax {
    pub start: Position,
    pub end: Position,
    pub parent: Option<ParentNode>,
    pub input: Option<Ref<StructureSyntax>>,
    pub output: Option<Ref<TypeAnnotationSyntax>>,
    pub throws_clause: Option<Ref<ThrowsSyntax>>,
    pub body: Ref<BlockSyntax>,
}

impl SyntaxNode for RoutineSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        unsafe {
            if !(*visitor).open_routine(Ref::from(self as *mut RoutineSyntax)) {
                return
            }
        }
        match self.input {
            Some(mut x) => x.accept(visitor),
            None => ()
        };

        match self.output {
            Some(mut x) => x.accept(visitor),
            None => ()
        };

        match self.throws_clause {
            Some(mut x) => x.accept(visitor),
            None => ()
        };

        self.body.accept(visitor);
            unsafe {
            (*visitor).close_routine(Ref::from(self as *mut RoutineSyntax))
        }
    }
}

#[derive(Copy, Clone)]
pub struct LetDeclarationSyntax {
    pub start: Position,
    pub end: Position,
    pub parent: Option<ParentNode>,
    pub declaration: Ref<LetSyntax>,
}

impl SyntaxNode for LetDeclarationSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        unsafe {
            if !(*visitor).open_letdeclaration(Ref::from(self as *mut LetDeclarationSyntax)) {
                return
            }
        }
        self.declaration.accept(visitor);
            unsafe {
            (*visitor).close_letdeclaration(Ref::from(self as *mut LetDeclarationSyntax))
        }
    }
}

#[derive(Copy, Clone)]
pub struct VarDeclarationSyntax {
    pub start: Position,
    pub end: Position,
    pub parent: Option<ParentNode>,
    pub declaration: Ref<VarSyntax>,
}

impl SyntaxNode for VarDeclarationSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        unsafe {
            if !(*visitor).open_vardeclaration(Ref::from(self as *mut VarDeclarationSyntax)) {
                return
            }
        }
        self.declaration.accept(visitor);
            unsafe {
            (*visitor).close_vardeclaration(Ref::from(self as *mut VarDeclarationSyntax))
        }
    }
}

#[derive(Copy, Clone)]
pub struct MutableDeclarationSyntax {
    pub start: Position,
    pub end: Position,
    pub parent: Option<ParentNode>,
    pub declaration: Ref<MutableSyntax>,
}

impl SyntaxNode for MutableDeclarationSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        unsafe {
            if !(*visitor).open_mutabledeclaration(Ref::from(self as *mut MutableDeclarationSyntax)) {
                return
            }
        }
        self.declaration.accept(visitor);
            unsafe {
            (*visitor).close_mutabledeclaration(Ref::from(self as *mut MutableDeclarationSyntax))
        }
    }
}

#[derive(Copy, Clone)]
pub struct ThreadLocalDeclarationSyntax {
    pub start: Position,
    pub end: Position,
    pub parent: Option<ParentNode>,
    pub declaration: Ref<ThreadLocalSyntax>,
}

impl SyntaxNode for ThreadLocalDeclarationSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        unsafe {
            if !(*visitor).open_threadlocaldeclaration(Ref::from(self as *mut ThreadLocalDeclarationSyntax)) {
                return
            }
        }
        self.declaration.accept(visitor);
            unsafe {
            (*visitor).close_threadlocaldeclaration(Ref::from(self as *mut ThreadLocalDeclarationSyntax))
        }
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
        match self {
            StatementSyntax::Let(syntax) => syntax.accept(visitor),
            StatementSyntax::Var(syntax) => syntax.accept(visitor),
            StatementSyntax::Mutable(syntax) => syntax.accept(visitor),
            StatementSyntax::ThreadLocal(syntax) => syntax.accept(visitor),
            StatementSyntax::Set(syntax) => syntax.accept(visitor),
            StatementSyntax::Calculation(syntax) => syntax.accept(visitor),
            StatementSyntax::Break(syntax) => syntax.accept(visitor),
            StatementSyntax::Continue(syntax) => syntax.accept(visitor),
            StatementSyntax::Return(syntax) => syntax.accept(visitor),
            StatementSyntax::Throw(syntax) => syntax.accept(visitor),
        }
    }
}

#[derive(Copy, Clone)]
pub struct LetSyntax {
    pub start: Position,
    pub end: Position,
    pub parent: Option<ParentNode>,
    pub binding: Ref<BindingSyntax>,
}

impl SyntaxNode for LetSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        unsafe {
            if !(*visitor).open_let(Ref::from(self as *mut LetSyntax)) {
                return
            }
        }
        self.binding.accept(visitor);
            unsafe {
            (*visitor).close_let(Ref::from(self as *mut LetSyntax))
        }
    }
}

#[derive(Copy, Clone)]
pub struct VarSyntax {
    pub start: Position,
    pub end: Position,
    pub parent: Option<ParentNode>,
    pub binding: Ref<BindingSyntax>,
}

impl SyntaxNode for VarSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        unsafe {
            if !(*visitor).open_var(Ref::from(self as *mut VarSyntax)) {
                return
            }
        }
        self.binding.accept(visitor);
            unsafe {
            (*visitor).close_var(Ref::from(self as *mut VarSyntax))
        }
    }
}

#[derive(Copy, Clone)]
pub struct MutableSyntax {
    pub start: Position,
    pub end: Position,
    pub parent: Option<ParentNode>,
    pub binding: Ref<BindingSyntax>,
}

impl SyntaxNode for MutableSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        unsafe {
            if !(*visitor).open_mutable(Ref::from(self as *mut MutableSyntax)) {
                return
            }
        }
        self.binding.accept(visitor);
            unsafe {
            (*visitor).close_mutable(Ref::from(self as *mut MutableSyntax))
        }
    }
}

#[derive(Copy, Clone)]
pub struct ThreadLocalSyntax {
    pub start: Position,
    pub end: Position,
    pub parent: Option<ParentNode>,
    pub binding: Ref<BindingSyntax>,
}

impl SyntaxNode for ThreadLocalSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        unsafe {
            if !(*visitor).open_threadlocal(Ref::from(self as *mut ThreadLocalSyntax)) {
                return
            }
        }
        self.binding.accept(visitor);
            unsafe {
            (*visitor).close_threadlocal(Ref::from(self as *mut ThreadLocalSyntax))
        }
    }
}

#[derive(Copy, Clone)]
pub struct BindingSyntax {
    pub start: Position,
    pub end: Position,
    pub parent: Option<ParentNode>,
    pub name: String,
    pub type_annotation: Option<Ref<TypeAnnotationSyntax>>,
    pub calculation: Ref<CalculationSyntax>,
}

impl SyntaxNode for BindingSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        unsafe {
            if !(*visitor).open_binding(Ref::from(self as *mut BindingSyntax)) {
                return
            }
        }
        match self.type_annotation {
            Some(mut x) => x.accept(visitor),
            None => ()
        };

        self.calculation.accept(visitor);
            unsafe {
            (*visitor).close_binding(Ref::from(self as *mut BindingSyntax))
        }
    }
}

#[derive(Copy, Clone)]
pub struct SetSyntax {
    pub start: Position,
    pub end: Position,
    pub parent: Option<ParentNode>,
    pub l_value: Ref<OperationSyntax>,
    pub r_value: Ref<CalculationSyntax>,
}

impl SyntaxNode for SetSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        unsafe {
            if !(*visitor).open_set(Ref::from(self as *mut SetSyntax)) {
                return
            }
        }
        self.l_value.accept(visitor);
    
        self.r_value.accept(visitor);
            unsafe {
            (*visitor).close_set(Ref::from(self as *mut SetSyntax))
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
    pub postfixes: Option<Ref<Vector<Ref<PostfixSyntax>>>>,
}

impl SyntaxNode for OperandSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        unsafe {
            if !(*visitor).open_operand(Ref::from(self as *mut OperandSyntax)) {
                return
            }
        }
        self.primary.accept(visitor);
    
        match self.postfixes {
            Some(mut x) => for node in x.iter_mut() {
                node.accept(visitor)
            },
            None => ()
        };
        unsafe {
            (*visitor).close_operand(Ref::from(self as *mut OperandSyntax))
        }
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
        match self {
            PostfixSyntax::MemberAccess(syntax) => syntax.accept(visitor),
            PostfixSyntax::As(syntax) => syntax.accept(visitor),
            PostfixSyntax::Is(syntax) => syntax.accept(visitor),
            PostfixSyntax::Unwrap(syntax) => syntax.accept(visitor),
            PostfixSyntax::Catch(syntax) => syntax.accept(visitor),
        }
    }
}

#[derive(Copy, Clone)]
pub struct MemberAccessSyntax {
    pub start: Position,
    pub end: Position,
    pub parent: Option<ParentNode>,
    pub member: String,
}

impl SyntaxNode for MemberAccessSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        unsafe {
            (*visitor).visit_memberaccess(Ref::from(self as *mut MemberAccessSyntax))
        }
    }
}

#[derive(Copy, Clone)]
pub struct AsSyntax {
    pub start: Position,
    pub end: Position,
    pub parent: Option<ParentNode>,
    pub type_spec: Ref<TypeSpecSyntax>,
}

impl SyntaxNode for AsSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        unsafe {
            if !(*visitor).open_as(Ref::from(self as *mut AsSyntax)) {
                return
            }
        }
        self.type_spec.accept(visitor);
            unsafe {
            (*visitor).close_as(Ref::from(self as *mut AsSyntax))
        }
    }
}

#[derive(Copy, Clone)]
pub struct IsSyntax {
    pub start: Position,
    pub end: Position,
    pub parent: Option<ParentNode>,
    pub type_spec: Ref<TypeSpecSyntax>,
}

impl SyntaxNode for IsSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        unsafe {
            if !(*visitor).open_is(Ref::from(self as *mut IsSyntax)) {
                return
            }
        }
        self.type_spec.accept(visitor);
            unsafe {
            (*visitor).close_is(Ref::from(self as *mut IsSyntax))
        }
    }
}

#[derive(Copy, Clone)]
pub struct UnwrapSyntax {
    pub start: Position,
    pub end: Position,
    pub parent: Option<ParentNode>,
}

impl SyntaxNode for UnwrapSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        unsafe {
            (*visitor).visit_unwrap(Ref::from(self as *mut UnwrapSyntax))
        }
    }
}

#[derive(Copy, Clone)]
pub struct CatchSyntax {
    pub start: Position,
    pub end: Position,
    pub parent: Option<ParentNode>,
    pub type_spec: Ref<CatchPatternSyntax>,
    pub handler: Ref<BlockSyntax>,
}

impl SyntaxNode for CatchSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        unsafe {
            if !(*visitor).open_catch(Ref::from(self as *mut CatchSyntax)) {
                return
            }
        }
        self.type_spec.accept(visitor);
    
        self.handler.accept(visitor);
            unsafe {
            (*visitor).close_catch(Ref::from(self as *mut CatchSyntax))
        }
    }
}

#[derive(Copy, Clone)]
pub enum CatchPatternSyntax {
   WildCardCatchPattern(WildCardCatchPatternSyntax),
   TypeCatchPattern(TypeCatchPatternSyntax),
}

impl SyntaxNode for CatchPatternSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        match self {
            CatchPatternSyntax::WildCardCatchPattern(syntax) => syntax.accept(visitor),
            CatchPatternSyntax::TypeCatchPattern(syntax) => syntax.accept(visitor),
        }
    }
}

#[derive(Copy, Clone)]
pub struct WildCardCatchPatternSyntax {
    pub start: Position,
    pub end: Position,
    pub parent: Option<ParentNode>,
    pub pattern: Ref<WildcardPatternSyntax>,
}

impl SyntaxNode for WildCardCatchPatternSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        unsafe {
            if !(*visitor).open_wildcardcatchpattern(Ref::from(self as *mut WildCardCatchPatternSyntax)) {
                return
            }
        }
        self.pattern.accept(visitor);
            unsafe {
            (*visitor).close_wildcardcatchpattern(Ref::from(self as *mut WildCardCatchPatternSyntax))
        }
    }
}

#[derive(Copy, Clone)]
pub struct TypeCatchPatternSyntax {
    pub start: Position,
    pub end: Position,
    pub parent: Option<ParentNode>,
    pub type_spec: Ref<TypeSpecSyntax>,
    pub error_name: Option<String>,
}

impl SyntaxNode for TypeCatchPatternSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        unsafe {
            if !(*visitor).open_typecatchpattern(Ref::from(self as *mut TypeCatchPatternSyntax)) {
                return
            }
        }
        self.type_spec.accept(visitor);
            unsafe {
            (*visitor).close_typecatchpattern(Ref::from(self as *mut TypeCatchPatternSyntax))
        }
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
        match self {
            ExpressionSyntax::Block(syntax) => syntax.accept(visitor),
            ExpressionSyntax::Name(syntax) => syntax.accept(visitor),
            ExpressionSyntax::Constant(syntax) => syntax.accept(visitor),
            ExpressionSyntax::If(syntax) => syntax.accept(visitor),
            ExpressionSyntax::Switch(syntax) => syntax.accept(visitor),
            ExpressionSyntax::For(syntax) => syntax.accept(visitor),
            ExpressionSyntax::While(syntax) => syntax.accept(visitor),
            ExpressionSyntax::Do(syntax) => syntax.accept(visitor),
            ExpressionSyntax::This(syntax) => syntax.accept(visitor),
            ExpressionSyntax::New(syntax) => syntax.accept(visitor),
            ExpressionSyntax::Object(syntax) => syntax.accept(visitor),
            ExpressionSyntax::Array(syntax) => syntax.accept(visitor),
            ExpressionSyntax::SizeOf(syntax) => syntax.accept(visitor),
        }
    }
}

#[derive(Copy, Clone)]
pub struct BlockSyntax {
    pub start: Position,
    pub end: Position,
    pub parent: Option<ParentNode>,
    pub statements: Option<Ref<Vector<Ref<StatementSyntax>>>>,
}

impl SyntaxNode for BlockSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        unsafe {
            if !(*visitor).open_block(Ref::from(self as *mut BlockSyntax)) {
                return
            }
        }
        match self.statements {
            Some(mut x) => for node in x.iter_mut() {
                node.accept(visitor)
            },
            None => ()
        };
        unsafe {
            (*visitor).close_block(Ref::from(self as *mut BlockSyntax))
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

#[derive(Copy, Clone)]
pub struct IfSyntax {
    pub start: Position,
    pub end: Position,
    pub parent: Option<ParentNode>,
    pub condition: Ref<OperationSyntax>,
    pub consequent: Ref<BlockSyntax>,
    pub else_clause: Option<Ref<ElseSyntax>>,
}

impl SyntaxNode for IfSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        unsafe {
            if !(*visitor).open_if(Ref::from(self as *mut IfSyntax)) {
                return
            }
        }
        self.condition.accept(visitor);
    
        self.consequent.accept(visitor);
    
        match self.else_clause {
            Some(mut x) => x.accept(visitor),
            None => ()
        };
        unsafe {
            (*visitor).close_if(Ref::from(self as *mut IfSyntax))
        }
    }
}

#[derive(Copy, Clone)]
pub struct ElseSyntax {
    pub start: Position,
    pub end: Position,
    pub parent: Option<ParentNode>,
    pub alternative: Ref<BlockSyntax>,
}

impl SyntaxNode for ElseSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        unsafe {
            if !(*visitor).open_else(Ref::from(self as *mut ElseSyntax)) {
                return
            }
        }
        self.alternative.accept(visitor);
            unsafe {
            (*visitor).close_else(Ref::from(self as *mut ElseSyntax))
        }
    }
}

#[derive(Copy, Clone)]
pub struct SwitchSyntax {
    pub start: Position,
    pub end: Position,
    pub parent: Option<ParentNode>,
    pub condition: Ref<OperationSyntax>,
    pub cases: Ref<Vector<Ref<SwitchCaseSyntax>>>,
}

impl SyntaxNode for SwitchSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        unsafe {
            if !(*visitor).open_switch(Ref::from(self as *mut SwitchSyntax)) {
                return
            }
        }
        self.condition.accept(visitor);
    
        for node in self.cases.iter_mut() {
            node.accept(visitor)
        };
        unsafe {
            (*visitor).close_switch(Ref::from(self as *mut SwitchSyntax))
        }
    }
}

#[derive(Copy, Clone)]
pub struct SwitchCaseSyntax {
    pub start: Position,
    pub end: Position,
    pub parent: Option<ParentNode>,
    pub label: Ref<CaseLabelSyntax>,
    pub content: Ref<BlockSyntax>,
}

impl SyntaxNode for SwitchCaseSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        unsafe {
            if !(*visitor).open_switchcase(Ref::from(self as *mut SwitchCaseSyntax)) {
                return
            }
        }
        self.label.accept(visitor);
    
        self.content.accept(visitor);
            unsafe {
            (*visitor).close_switchcase(Ref::from(self as *mut SwitchCaseSyntax))
        }
    }
}

#[derive(Copy, Clone)]
pub enum CaseLabelSyntax {
   ItemCaseLabel(ItemCaseLabelSyntax),
   DefaultCaseLabel(DefaultCaseLabelSyntax),
}

impl SyntaxNode for CaseLabelSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        match self {
            CaseLabelSyntax::ItemCaseLabel(syntax) => syntax.accept(visitor),
            CaseLabelSyntax::DefaultCaseLabel(syntax) => syntax.accept(visitor),
        }
    }
}

#[derive(Copy, Clone)]
pub struct ItemCaseLabelSyntax {
    pub start: Position,
    pub end: Position,
    pub parent: Option<ParentNode>,
    pub items: Ref<Vector<Ref<CaseItemSyntax>>>,
}

impl SyntaxNode for ItemCaseLabelSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        unsafe {
            if !(*visitor).open_itemcaselabel(Ref::from(self as *mut ItemCaseLabelSyntax)) {
                return
            }
        }
        for node in self.items.iter_mut() {
            node.accept(visitor)
        };
        unsafe {
            (*visitor).close_itemcaselabel(Ref::from(self as *mut ItemCaseLabelSyntax))
        }
    }
}

#[derive(Copy, Clone)]
pub struct CaseItemSyntax {
    pub start: Position,
    pub end: Position,
    pub parent: Option<ParentNode>,
    pub pattern: Ref<CasePatternSyntax>,
}

impl SyntaxNode for CaseItemSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        unsafe {
            if !(*visitor).open_caseitem(Ref::from(self as *mut CaseItemSyntax)) {
                return
            }
        }
        self.pattern.accept(visitor);
            unsafe {
            (*visitor).close_caseitem(Ref::from(self as *mut CaseItemSyntax))
        }
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
        match self {
            CasePatternSyntax::ConstantPattern(syntax) => syntax.accept(visitor),
            CasePatternSyntax::WildcardPattern(syntax) => syntax.accept(visitor),
            CasePatternSyntax::NamePattern(syntax) => syntax.accept(visitor),
        }
    }
}

#[derive(Copy, Clone)]
pub struct ConstantPatternSyntax {
    pub start: Position,
    pub end: Position,
    pub parent: Option<ParentNode>,
    pub constant: Ref<ConstantSyntax>,
}

impl SyntaxNode for ConstantPatternSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        unsafe {
            if !(*visitor).open_constantpattern(Ref::from(self as *mut ConstantPatternSyntax)) {
                return
            }
        }
        self.constant.accept(visitor);
            unsafe {
            (*visitor).close_constantpattern(Ref::from(self as *mut ConstantPatternSyntax))
        }
    }
}

#[derive(Copy, Clone)]
pub struct WildcardPatternSyntax {
    pub start: Position,
    pub end: Position,
    pub parent: Option<ParentNode>,
}

impl SyntaxNode for WildcardPatternSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        unsafe {
            (*visitor).visit_wildcardpattern(Ref::from(self as *mut WildcardPatternSyntax))
        }
    }
}

#[derive(Copy, Clone)]
pub struct NamePatternSyntax {
    pub start: Position,
    pub end: Position,
    pub parent: Option<ParentNode>,
    pub name: Ref<NameSyntax>,
}

impl SyntaxNode for NamePatternSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        unsafe {
            if !(*visitor).open_namepattern(Ref::from(self as *mut NamePatternSyntax)) {
                return
            }
        }
        self.name.accept(visitor);
            unsafe {
            (*visitor).close_namepattern(Ref::from(self as *mut NamePatternSyntax))
        }
    }
}

#[derive(Copy, Clone)]
pub struct DefaultCaseLabelSyntax {
    pub start: Position,
    pub end: Position,
    pub parent: Option<ParentNode>,
}

impl SyntaxNode for DefaultCaseLabelSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        unsafe {
            (*visitor).visit_defaultcaselabel(Ref::from(self as *mut DefaultCaseLabelSyntax))
        }
    }
}

#[derive(Copy, Clone)]
pub struct ForSyntax {
    pub start: Position,
    pub end: Position,
    pub parent: Option<ParentNode>,
    pub index: String,
    pub type_annotation: Option<Ref<TypeAnnotationSyntax>>,
    pub operation: Ref<OperationSyntax>,
    pub iteration: Ref<LoopSyntax>,
}

impl SyntaxNode for ForSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        unsafe {
            if !(*visitor).open_for(Ref::from(self as *mut ForSyntax)) {
                return
            }
        }
        match self.type_annotation {
            Some(mut x) => x.accept(visitor),
            None => ()
        };

        self.operation.accept(visitor);
    
        self.iteration.accept(visitor);
            unsafe {
            (*visitor).close_for(Ref::from(self as *mut ForSyntax))
        }
    }
}

#[derive(Copy, Clone)]
pub struct WhileSyntax {
    pub start: Position,
    pub end: Position,
    pub parent: Option<ParentNode>,
    pub condition: Ref<OperationSyntax>,
    pub iteration: Ref<LoopSyntax>,
}

impl SyntaxNode for WhileSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        unsafe {
            if !(*visitor).open_while(Ref::from(self as *mut WhileSyntax)) {
                return
            }
        }
        self.condition.accept(visitor);
    
        self.iteration.accept(visitor);
            unsafe {
            (*visitor).close_while(Ref::from(self as *mut WhileSyntax))
        }
    }
}

#[derive(Copy, Clone)]
pub struct DoSyntax {
    pub start: Position,
    pub end: Position,
    pub parent: Option<ParentNode>,
    pub iteration: Ref<LoopSyntax>,
    pub condition: Ref<OperationSyntax>,
}

impl SyntaxNode for DoSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        unsafe {
            if !(*visitor).open_do(Ref::from(self as *mut DoSyntax)) {
                return
            }
        }
        self.iteration.accept(visitor);
    
        self.condition.accept(visitor);
            unsafe {
            (*visitor).close_do(Ref::from(self as *mut DoSyntax))
        }
    }
}

#[derive(Copy, Clone)]
pub enum LoopSyntax {
   SimpleLoop(SimpleLoopSyntax),
   NamedLoop(NamedLoopSyntax),
}

impl SyntaxNode for LoopSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        match self {
            LoopSyntax::SimpleLoop(syntax) => syntax.accept(visitor),
            LoopSyntax::NamedLoop(syntax) => syntax.accept(visitor),
        }
    }
}

#[derive(Copy, Clone)]
pub struct SimpleLoopSyntax {
    pub start: Position,
    pub end: Position,
    pub parent: Option<ParentNode>,
    pub code: Ref<BlockSyntax>,
}

impl SyntaxNode for SimpleLoopSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        unsafe {
            if !(*visitor).open_simpleloop(Ref::from(self as *mut SimpleLoopSyntax)) {
                return
            }
        }
        self.code.accept(visitor);
            unsafe {
            (*visitor).close_simpleloop(Ref::from(self as *mut SimpleLoopSyntax))
        }
    }
}

#[derive(Copy, Clone)]
pub struct NamedLoopSyntax {
    pub start: Position,
    pub end: Position,
    pub parent: Option<ParentNode>,
    pub name: String,
    pub code: Ref<BlockSyntax>,
}

impl SyntaxNode for NamedLoopSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        unsafe {
            if !(*visitor).open_namedloop(Ref::from(self as *mut NamedLoopSyntax)) {
                return
            }
        }
        self.code.accept(visitor);
            unsafe {
            (*visitor).close_namedloop(Ref::from(self as *mut NamedLoopSyntax))
        }
    }
}

#[derive(Copy, Clone)]
pub struct ThisSyntax {
    pub start: Position,
    pub end: Position,
    pub parent: Option<ParentNode>,
}

impl SyntaxNode for ThisSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        unsafe {
            (*visitor).visit_this(Ref::from(self as *mut ThisSyntax))
        }
    }
}

#[derive(Copy, Clone)]
pub struct NewSyntax {
    pub start: Position,
    pub end: Position,
    pub parent: Option<ParentNode>,
    pub type_spec: Ref<TypeSyntax>,
}

impl SyntaxNode for NewSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        unsafe {
            if !(*visitor).open_new(Ref::from(self as *mut NewSyntax)) {
                return
            }
        }
        self.type_spec.accept(visitor);
            unsafe {
            (*visitor).close_new(Ref::from(self as *mut NewSyntax))
        }
    }
}

#[derive(Copy, Clone)]
pub struct ObjectSyntax {
    pub start: Position,
    pub end: Position,
    pub parent: Option<ParentNode>,
    pub first_op: Option<Ref<OperationSyntax>>,
    pub additional_ops: Option<Ref<Vector<Ref<ItemSyntax>>>>,
}

impl SyntaxNode for ObjectSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        unsafe {
            if !(*visitor).open_object(Ref::from(self as *mut ObjectSyntax)) {
                return
            }
        }
        match self.first_op {
            Some(mut x) => x.accept(visitor),
            None => ()
        };

        match self.additional_ops {
            Some(mut x) => for node in x.iter_mut() {
                node.accept(visitor)
            },
            None => ()
        };
        unsafe {
            (*visitor).close_object(Ref::from(self as *mut ObjectSyntax))
        }
    }
}

#[derive(Copy, Clone)]
pub struct ArraySyntax {
    pub start: Position,
    pub end: Position,
    pub parent: Option<ParentNode>,
    pub first_op: Option<Ref<OperationSyntax>>,
    pub additional_ops: Option<Ref<Vector<Ref<ItemSyntax>>>>,
}

impl SyntaxNode for ArraySyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        unsafe {
            if !(*visitor).open_array(Ref::from(self as *mut ArraySyntax)) {
                return
            }
        }
        match self.first_op {
            Some(mut x) => x.accept(visitor),
            None => ()
        };

        match self.additional_ops {
            Some(mut x) => for node in x.iter_mut() {
                node.accept(visitor)
            },
            None => ()
        };
        unsafe {
            (*visitor).close_array(Ref::from(self as *mut ArraySyntax))
        }
    }
}

#[derive(Copy, Clone)]
pub struct ItemSyntax {
    pub start: Position,
    pub end: Position,
    pub parent: Option<ParentNode>,
    pub operation: Ref<OperationSyntax>,
}

impl SyntaxNode for ItemSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        unsafe {
            if !(*visitor).open_item(Ref::from(self as *mut ItemSyntax)) {
                return
            }
        }
        self.operation.accept(visitor);
            unsafe {
            (*visitor).close_item(Ref::from(self as *mut ItemSyntax))
        }
    }
}

#[derive(Copy, Clone)]
pub struct SizeOfSyntax {
    pub start: Position,
    pub end: Position,
    pub parent: Option<ParentNode>,
    pub type_spec: Ref<TypeSyntax>,
}

impl SyntaxNode for SizeOfSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        unsafe {
            if !(*visitor).open_sizeof(Ref::from(self as *mut SizeOfSyntax)) {
                return
            }
        }
        self.type_spec.accept(visitor);
            unsafe {
            (*visitor).close_sizeof(Ref::from(self as *mut SizeOfSyntax))
        }
    }
}

#[derive(Copy, Clone)]
pub struct BreakSyntax {
    pub start: Position,
    pub end: Position,
    pub parent: Option<ParentNode>,
    pub iteration: Option<String>,
}

impl SyntaxNode for BreakSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        unsafe {
            (*visitor).visit_break(Ref::from(self as *mut BreakSyntax))
        }
    }
}

#[derive(Copy, Clone)]
pub struct ContinueSyntax {
    pub start: Position,
    pub end: Position,
    pub parent: Option<ParentNode>,
    pub iteration: Option<String>,
}

impl SyntaxNode for ContinueSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        unsafe {
            (*visitor).visit_continue(Ref::from(self as *mut ContinueSyntax))
        }
    }
}

#[derive(Copy, Clone)]
pub struct ReturnSyntax {
    pub start: Position,
    pub end: Position,
    pub parent: Option<ParentNode>,
    pub result: Option<Ref<CalculationSyntax>>,
}

impl SyntaxNode for ReturnSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        unsafe {
            if !(*visitor).open_return(Ref::from(self as *mut ReturnSyntax)) {
                return
            }
        }
        match self.result {
            Some(mut x) => x.accept(visitor),
            None => ()
        };
        unsafe {
            (*visitor).close_return(Ref::from(self as *mut ReturnSyntax))
        }
    }
}

#[derive(Copy, Clone)]
pub struct ThrowSyntax {
    pub start: Position,
    pub end: Position,
    pub parent: Option<ParentNode>,
    pub exception: Ref<CalculationSyntax>,
}

impl SyntaxNode for ThrowSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        unsafe {
            if !(*visitor).open_throw(Ref::from(self as *mut ThrowSyntax)) {
                return
            }
        }
        self.exception.accept(visitor);
            unsafe {
            (*visitor).close_throw(Ref::from(self as *mut ThrowSyntax))
        }
    }
}

#[derive(Copy, Clone)]
pub struct ClassSyntax {
    pub start: Position,
    pub end: Position,
    pub parent: Option<ParentNode>,
    pub name: Ref<NameSyntax>,
    pub generics: Option<Ref<GenericParametersSyntax>>,
    pub base_class: Option<Ref<ExtendsSyntax>>,
    pub contents: Option<Ref<StructureSyntax>>,
    pub body: Option<Ref<ClassBodySyntax>>,
}

impl SyntaxNode for ClassSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        unsafe {
            if !(*visitor).open_class(Ref::from(self as *mut ClassSyntax)) {
                return
            }
        }
        self.name.accept(visitor);
    
        match self.generics {
            Some(mut x) => x.accept(visitor),
            None => ()
        };

        match self.base_class {
            Some(mut x) => x.accept(visitor),
            None => ()
        };

        match self.contents {
            Some(mut x) => x.accept(visitor),
            None => ()
        };

        match self.body {
            Some(mut x) => x.accept(visitor),
            None => ()
        };
        unsafe {
            (*visitor).close_class(Ref::from(self as *mut ClassSyntax))
        }
    }
}

#[derive(Copy, Clone)]
pub struct GenericParametersSyntax {
    pub start: Position,
    pub end: Position,
    pub parent: Option<ParentNode>,
    pub name: String,
    pub additional_generics: Option<Ref<Vector<Ref<GenericParameterSyntax>>>>,
}

impl SyntaxNode for GenericParametersSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        unsafe {
            if !(*visitor).open_genericparameters(Ref::from(self as *mut GenericParametersSyntax)) {
                return
            }
        }
        match self.additional_generics {
            Some(mut x) => for node in x.iter_mut() {
                node.accept(visitor)
            },
            None => ()
        };
        unsafe {
            (*visitor).close_genericparameters(Ref::from(self as *mut GenericParametersSyntax))
        }
    }
}

#[derive(Copy, Clone)]
pub struct GenericParameterSyntax {
    pub start: Position,
    pub end: Position,
    pub parent: Option<ParentNode>,
    pub name: String,
}

impl SyntaxNode for GenericParameterSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        unsafe {
            (*visitor).visit_genericparameter(Ref::from(self as *mut GenericParameterSyntax))
        }
    }
}

#[derive(Copy, Clone)]
pub struct ExtendsSyntax {
    pub start: Position,
    pub end: Position,
    pub parent: Option<ParentNode>,
    pub name: Ref<NameSyntax>,
}

impl SyntaxNode for ExtendsSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        unsafe {
            if !(*visitor).open_extends(Ref::from(self as *mut ExtendsSyntax)) {
                return
            }
        }
        self.name.accept(visitor);
            unsafe {
            (*visitor).close_extends(Ref::from(self as *mut ExtendsSyntax))
        }
    }
}

#[derive(Copy, Clone)]
pub struct StructureSyntax {
    pub start: Position,
    pub end: Position,
    pub parent: Option<ParentNode>,
    pub components: Option<Ref<Vector<Ref<ComponentSyntax>>>>,
}

impl SyntaxNode for StructureSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        unsafe {
            if !(*visitor).open_structure(Ref::from(self as *mut StructureSyntax)) {
                return
            }
        }
        match self.components {
            Some(mut x) => for node in x.iter_mut() {
                node.accept(visitor)
            },
            None => ()
        };
        unsafe {
            (*visitor).close_structure(Ref::from(self as *mut StructureSyntax))
        }
    }
}

#[derive(Copy, Clone)]
pub struct ComponentSyntax {
    pub start: Position,
    pub end: Position,
    pub parent: Option<ParentNode>,
    pub name: String,
    pub type_annotation: Option<Ref<TypeAnnotationSyntax>>,
}

impl SyntaxNode for ComponentSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        unsafe {
            if !(*visitor).open_component(Ref::from(self as *mut ComponentSyntax)) {
                return
            }
        }
        match self.type_annotation {
            Some(mut x) => x.accept(visitor),
            None => ()
        };
        unsafe {
            (*visitor).close_component(Ref::from(self as *mut ComponentSyntax))
        }
    }
}

#[derive(Copy, Clone)]
pub struct ClassBodySyntax {
    pub start: Position,
    pub end: Position,
    pub parent: Option<ParentNode>,
    pub members: Option<Ref<Vector<Ref<ClassMemberSyntax>>>>,
}

impl SyntaxNode for ClassBodySyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        unsafe {
            if !(*visitor).open_classbody(Ref::from(self as *mut ClassBodySyntax)) {
                return
            }
        }
        match self.members {
            Some(mut x) => for node in x.iter_mut() {
                node.accept(visitor)
            },
            None => ()
        };
        unsafe {
            (*visitor).close_classbody(Ref::from(self as *mut ClassBodySyntax))
        }
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
        match self {
            ClassMemberSyntax::LetMember(syntax) => syntax.accept(visitor),
            ClassMemberSyntax::VarMember(syntax) => syntax.accept(visitor),
            ClassMemberSyntax::MutableMember(syntax) => syntax.accept(visitor),
            ClassMemberSyntax::SetInitialization(syntax) => syntax.accept(visitor),
            ClassMemberSyntax::Method(syntax) => syntax.accept(visitor),
            ClassMemberSyntax::StaticFunction(syntax) => syntax.accept(visitor),
            ClassMemberSyntax::Operator(syntax) => syntax.accept(visitor),
            ClassMemberSyntax::Initializer(syntax) => syntax.accept(visitor),
            ClassMemberSyntax::Allocator(syntax) => syntax.accept(visitor),
        }
    }
}

#[derive(Copy, Clone)]
pub struct LetMemberSyntax {
    pub start: Position,
    pub end: Position,
    pub parent: Option<ParentNode>,
    pub declaration: Ref<LetSyntax>,
}

impl SyntaxNode for LetMemberSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        unsafe {
            if !(*visitor).open_letmember(Ref::from(self as *mut LetMemberSyntax)) {
                return
            }
        }
        self.declaration.accept(visitor);
            unsafe {
            (*visitor).close_letmember(Ref::from(self as *mut LetMemberSyntax))
        }
    }
}

#[derive(Copy, Clone)]
pub struct VarMemberSyntax {
    pub start: Position,
    pub end: Position,
    pub parent: Option<ParentNode>,
    pub declaration: Ref<VarSyntax>,
}

impl SyntaxNode for VarMemberSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        unsafe {
            if !(*visitor).open_varmember(Ref::from(self as *mut VarMemberSyntax)) {
                return
            }
        }
        self.declaration.accept(visitor);
            unsafe {
            (*visitor).close_varmember(Ref::from(self as *mut VarMemberSyntax))
        }
    }
}

#[derive(Copy, Clone)]
pub struct MutableMemberSyntax {
    pub start: Position,
    pub end: Position,
    pub parent: Option<ParentNode>,
    pub declaration: Ref<MutableSyntax>,
}

impl SyntaxNode for MutableMemberSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        unsafe {
            if !(*visitor).open_mutablemember(Ref::from(self as *mut MutableMemberSyntax)) {
                return
            }
        }
        self.declaration.accept(visitor);
            unsafe {
            (*visitor).close_mutablemember(Ref::from(self as *mut MutableMemberSyntax))
        }
    }
}

#[derive(Copy, Clone)]
pub struct SetInitializationSyntax {
    pub start: Position,
    pub end: Position,
    pub parent: Option<ParentNode>,
    pub definition: Ref<SetSyntax>,
}

impl SyntaxNode for SetInitializationSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        unsafe {
            if !(*visitor).open_setinitialization(Ref::from(self as *mut SetInitializationSyntax)) {
                return
            }
        }
        self.definition.accept(visitor);
            unsafe {
            (*visitor).close_setinitialization(Ref::from(self as *mut SetInitializationSyntax))
        }
    }
}

#[derive(Copy, Clone)]
pub struct MethodSyntax {
    pub start: Position,
    pub end: Position,
    pub parent: Option<ParentNode>,
    pub procedure: Ref<ProcedureSyntax>,
}

impl SyntaxNode for MethodSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        unsafe {
            if !(*visitor).open_method(Ref::from(self as *mut MethodSyntax)) {
                return
            }
        }
        self.procedure.accept(visitor);
            unsafe {
            (*visitor).close_method(Ref::from(self as *mut MethodSyntax))
        }
    }
}

#[derive(Copy, Clone)]
pub struct StaticFunctionSyntax {
    pub start: Position,
    pub end: Position,
    pub parent: Option<ParentNode>,
    pub procedure: Ref<FunctionSyntax>,
}

impl SyntaxNode for StaticFunctionSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        unsafe {
            if !(*visitor).open_staticfunction(Ref::from(self as *mut StaticFunctionSyntax)) {
                return
            }
        }
        self.procedure.accept(visitor);
            unsafe {
            (*visitor).close_staticfunction(Ref::from(self as *mut StaticFunctionSyntax))
        }
    }
}

#[derive(Copy, Clone)]
pub struct OperatorSyntax {
    pub start: Position,
    pub end: Position,
    pub parent: Option<ParentNode>,
    pub routine: Ref<RoutineSyntax>,
}

impl SyntaxNode for OperatorSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        unsafe {
            if !(*visitor).open_operator(Ref::from(self as *mut OperatorSyntax)) {
                return
            }
        }
        self.routine.accept(visitor);
            unsafe {
            (*visitor).close_operator(Ref::from(self as *mut OperatorSyntax))
        }
    }
}

#[derive(Copy, Clone)]
pub struct InitializerSyntax {
    pub start: Position,
    pub end: Position,
    pub parent: Option<ParentNode>,
    pub input: Option<Ref<StructureSyntax>>,
    pub body: Ref<BlockSyntax>,
}

impl SyntaxNode for InitializerSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        unsafe {
            if !(*visitor).open_initializer(Ref::from(self as *mut InitializerSyntax)) {
                return
            }
        }
        match self.input {
            Some(mut x) => x.accept(visitor),
            None => ()
        };

        self.body.accept(visitor);
            unsafe {
            (*visitor).close_initializer(Ref::from(self as *mut InitializerSyntax))
        }
    }
}

#[derive(Copy, Clone)]
pub struct AllocatorSyntax {
    pub start: Position,
    pub end: Position,
    pub parent: Option<ParentNode>,
    pub input: Option<Ref<StructureSyntax>>,
    pub body: Ref<BlockSyntax>,
}

impl SyntaxNode for AllocatorSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        unsafe {
            if !(*visitor).open_allocator(Ref::from(self as *mut AllocatorSyntax)) {
                return
            }
        }
        match self.input {
            Some(mut x) => x.accept(visitor),
            None => ()
        };

        self.body.accept(visitor);
            unsafe {
            (*visitor).close_allocator(Ref::from(self as *mut AllocatorSyntax))
        }
    }
}

#[derive(Copy, Clone)]
pub struct TypeAnnotationSyntax {
    pub start: Position,
    pub end: Position,
    pub parent: Option<ParentNode>,
    pub type_spec: Ref<TypeSpecSyntax>,
}

impl SyntaxNode for TypeAnnotationSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        unsafe {
            if !(*visitor).open_typeannotation(Ref::from(self as *mut TypeAnnotationSyntax)) {
                return
            }
        }
        self.type_spec.accept(visitor);
            unsafe {
            (*visitor).close_typeannotation(Ref::from(self as *mut TypeAnnotationSyntax))
        }
    }
}

#[derive(Copy, Clone)]
pub enum TypeSpecSyntax {
   Type(TypeSyntax),
   Variant(VariantSyntax),
}

impl SyntaxNode for TypeSpecSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        match self {
            TypeSpecSyntax::Type(syntax) => syntax.accept(visitor),
            TypeSpecSyntax::Variant(syntax) => syntax.accept(visitor),
        }
    }
}

#[derive(Copy, Clone)]
pub struct TypeSyntax {
    pub start: Position,
    pub end: Position,
    pub parent: Option<ParentNode>,
    pub name: Ref<NameSyntax>,
    pub generics: Option<Ref<GenericArgumentsSyntax>>,
    pub optional: Option<Ref<OptionalSyntax>>,
    pub life_time: Option<Ref<LifeTimeSyntax>>,
}

impl SyntaxNode for TypeSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        unsafe {
            if !(*visitor).open_type(Ref::from(self as *mut TypeSyntax)) {
                return
            }
        }
        self.name.accept(visitor);
    
        match self.generics {
            Some(mut x) => x.accept(visitor),
            None => ()
        };

        match self.optional {
            Some(mut x) => x.accept(visitor),
            None => ()
        };

        match self.life_time {
            Some(mut x) => x.accept(visitor),
            None => ()
        };
        unsafe {
            (*visitor).close_type(Ref::from(self as *mut TypeSyntax))
        }
    }
}

#[derive(Copy, Clone)]
pub struct VariantSyntax {
    pub start: Position,
    pub end: Position,
    pub parent: Option<ParentNode>,
    pub types: Option<Ref<Vector<Ref<TypeSyntax>>>>,
}

impl SyntaxNode for VariantSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        unsafe {
            if !(*visitor).open_variant(Ref::from(self as *mut VariantSyntax)) {
                return
            }
        }
        match self.types {
            Some(mut x) => for node in x.iter_mut() {
                node.accept(visitor)
            },
            None => ()
        };
        unsafe {
            (*visitor).close_variant(Ref::from(self as *mut VariantSyntax))
        }
    }
}

#[derive(Copy, Clone)]
pub struct ThrowsSyntax {
    pub start: Position,
    pub end: Position,
    pub parent: Option<ParentNode>,
    pub throws_type: Ref<TypeSpecSyntax>,
}

impl SyntaxNode for ThrowsSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        unsafe {
            if !(*visitor).open_throws(Ref::from(self as *mut ThrowsSyntax)) {
                return
            }
        }
        self.throws_type.accept(visitor);
            unsafe {
            (*visitor).close_throws(Ref::from(self as *mut ThrowsSyntax))
        }
    }
}

#[derive(Copy, Clone)]
pub struct GenericArgumentsSyntax {
    pub start: Position,
    pub end: Position,
    pub parent: Option<ParentNode>,
    pub generic: Ref<TypeSyntax>,
    pub additional_generics: Option<Ref<Vector<Ref<GenericArgumentSyntax>>>>,
}

impl SyntaxNode for GenericArgumentsSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        unsafe {
            if !(*visitor).open_genericarguments(Ref::from(self as *mut GenericArgumentsSyntax)) {
                return
            }
        }
        self.generic.accept(visitor);
    
        match self.additional_generics {
            Some(mut x) => for node in x.iter_mut() {
                node.accept(visitor)
            },
            None => ()
        };
        unsafe {
            (*visitor).close_genericarguments(Ref::from(self as *mut GenericArgumentsSyntax))
        }
    }
}

#[derive(Copy, Clone)]
pub struct GenericArgumentSyntax {
    pub start: Position,
    pub end: Position,
    pub parent: Option<ParentNode>,
    pub type_spec: Ref<TypeSyntax>,
}

impl SyntaxNode for GenericArgumentSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        unsafe {
            if !(*visitor).open_genericargument(Ref::from(self as *mut GenericArgumentSyntax)) {
                return
            }
        }
        self.type_spec.accept(visitor);
            unsafe {
            (*visitor).close_genericargument(Ref::from(self as *mut GenericArgumentSyntax))
        }
    }
}

#[derive(Copy, Clone)]
pub struct OptionalSyntax {
    pub start: Position,
    pub end: Position,
    pub parent: Option<ParentNode>,
}

impl SyntaxNode for OptionalSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        unsafe {
            (*visitor).visit_optional(Ref::from(self as *mut OptionalSyntax))
        }
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
        match self {
            LifeTimeSyntax::Root(syntax) => syntax.accept(visitor),
            LifeTimeSyntax::Local(syntax) => syntax.accept(visitor),
            LifeTimeSyntax::Reference(syntax) => syntax.accept(visitor),
            LifeTimeSyntax::Thrown(syntax) => syntax.accept(visitor),
        }
    }
}

#[derive(Copy, Clone)]
pub struct RootSyntax {
    pub start: Position,
    pub end: Position,
    pub parent: Option<ParentNode>,
}

impl SyntaxNode for RootSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        unsafe {
            (*visitor).visit_root(Ref::from(self as *mut RootSyntax))
        }
    }
}

#[derive(Copy, Clone)]
pub struct LocalSyntax {
    pub start: Position,
    pub end: Position,
    pub parent: Option<ParentNode>,
    pub location: String,
}

impl SyntaxNode for LocalSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        unsafe {
            (*visitor).visit_local(Ref::from(self as *mut LocalSyntax))
        }
    }
}

#[derive(Copy, Clone)]
pub struct ReferenceSyntax {
    pub start: Position,
    pub end: Position,
    pub parent: Option<ParentNode>,
    pub age: Option<Literal>,
}

impl SyntaxNode for ReferenceSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        unsafe {
            (*visitor).visit_reference(Ref::from(self as *mut ReferenceSyntax))
        }
    }
}

#[derive(Copy, Clone)]
pub struct ThrownSyntax {
    pub start: Position,
    pub end: Position,
    pub parent: Option<ParentNode>,
}

impl SyntaxNode for ThrownSyntax {
    fn accept(&mut self, visitor: *mut SyntaxVisitor) {
        unsafe {
            (*visitor).visit_thrown(Ref::from(self as *mut ThrownSyntax))
        }
    }
}
