use scaly::containers::{Array, HashSet, Ref, String, Vector};
use scaly::io::Stream;
use scaly::memory::Region;
use scaly::Page;
use scalyc::errors::ParserError;
use scalyc::lexer::Lexer;
use scalyc::lexer::Position;

pub struct Parser {
    lexer: Ref<Lexer>,
    file_name: String,
    _keywords: Ref<HashSet<String>>,
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
            _keywords: keywords,
        }
    }

    pub fn parse_file(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Ref<File>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        let start: Position = self.lexer.get_previous_position();

        let intrinsics: Option<Ref<Vector<IntrinsicSyntax>>> = parse_intrinsic_list();

        let usings: Option<Ref<Vector<UsingSyntax>>> = parse_using_list();

        let defines: Option<Ref<Vector<DefineSyntax>>> = parse_define_list();

        let declarations: Option<Ref<Vector<DeclarationSyntax>>> = parse_declaration_list();

        let statements: Option<Ref<Vector<StatementSyntax>>> = parse_statement_list();
        match statements {
            Some =>
                if !self.is_at_end() {
                    let error_pos = self.lexer.get_previous_position();
                    return Result::Err(Ref::new(
                        _ep,
                        ParserError {
                            file_name: self.file_name,
                            line: error_pos.line,
                            column: error_pos.column,
                        },
                    ))
                }
            None => (),
        }

        let end: Position = self.lexer.get_position();

        let ret: FileSyntax = Ref::new(_rp, FileSyntax { start: start, end: end, intrinsics: intrinsics, usings: usings, defines: defines, declarations: declarations, statements: statements});

        match intrinsics {
            Some(x) => for item in x {
                item.parent = ret;
            }
            None => ()
        }

        match usings {
            Some(x) => for item in x {
                item.parent = ret;
            }
            None => ()
        }

        match defines {
            Some(x) => for item in x {
                item.parent = ret;
            }
            None => ()
        }

        match declarations {
            Some(x) => for item in x {
                item.parent = ret;
            }
            None => ()
        }

        match statements {
            Some(x) => for item in x {
                item.parent = ret;
            }
            None => ()
        }

        ret
    }

    pub fn parseIntrinsicList(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
    ) -> Option<Ref<Vector<Ref<IntrinsicSyntax>>>> {
        let _r = Region::create(_pr);
        let mut ret: Option<Ref<Array<Ref<IntrinsicSyntax>>>> = Option::None;
        loop {
            let node = self.parse_Intrinsic(&_r, _rp);
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
            Some(ret) => Some(Ref::new(_rp, Vector::from_array(_rp, ret))),
            None => None,
        }
    }

    pub fn parse_intrinsic(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Ref<Intrinsic>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        let start: Position = self.lexer.get_previous_position();

        let successIntrinsic1: bool = self.lexer.parse_keyword("intrinsic");
        match successIntrinsic1 {
            Some (x) =>
                lexer.advance(),
            None =>
                return None
        }

        let name: String = self.lexer.parse_identifier();
        match (name != null) && isIdentifier(name) {
            Some (x) =>
                lexer.advance(),
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

        let ret: IntrinsicSyntax = Ref::new(_rp, IntrinsicSyntax { start: start, end: end, name: name});

        ret
    }

    pub fn parseUsingList(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
    ) -> Option<Ref<Vector<Ref<UsingSyntax>>>> {
        let _r = Region::create(_pr);
        let mut ret: Option<Ref<Array<Ref<UsingSyntax>>>> = Option::None;
        loop {
            let node = self.parse_Using(&_r, _rp);
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
            Some(ret) => Some(Ref::new(_rp, Vector::from_array(_rp, ret))),
            None => None,
        }
    }

    pub fn parse_using(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Ref<Using>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        let start: Position = self.lexer.get_previous_position();

        let successUsing1: bool = self.lexer.parse_keyword("using");
        match successUsing1 {
            Some (x) =>
                lexer.advance(),
            None =>
                return None
        }

        let name: Ref<NameSyntax> = parse_name();
        match name {
            None => {
                return Err(Ref::new(_ep, parser_exception(String::copy(_ep, self.file_name), lexer.line, lexer.column))
            }

        let end: Position = self.lexer.get_position();

        let ret: UsingSyntax = Ref::new(_rp, UsingSyntax { start: start, end: end, name: name});

            name.parent = ret;

        ret
    }

    pub fn parseDefineList(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
    ) -> Option<Ref<Vector<Ref<DefineSyntax>>>> {
        let _r = Region::create(_pr);
        let mut ret: Option<Ref<Array<Ref<DefineSyntax>>>> = Option::None;
        loop {
            let node = self.parse_Define(&_r, _rp);
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
            Some(ret) => Some(Ref::new(_rp, Vector::from_array(_rp, ret))),
            None => None,
        }
    }

    pub fn parse_define(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Ref<Define>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        let start: Position = self.lexer.get_previous_position();

        let successDefine1: bool = self.lexer.parse_keyword("define");
        match successDefine1 {
            Some (x) =>
                lexer.advance(),
            None =>
                return None
        }

        let name: Ref<NameSyntax> = parse_name();
        match name {
            None => {
                return Err(Ref::new(_ep, parser_exception(String::copy(_ep, self.file_name), lexer.line, lexer.column))
            }

        let typeSpec: Ref<TypeSyntax> = parse_type();
        match typeSpec {
            None => {
                return Err(Ref::new(_ep, parser_exception(String::copy(_ep, self.file_name), lexer.line, lexer.column))
            }

        let end: Position = self.lexer.get_position();

        let ret: DefineSyntax = Ref::new(_rp, DefineSyntax { start: start, end: end, name: name, typeSpec: typeSpec});

            name.parent = ret;

            typeSpec.parent = ret;

        ret
    }

    pub fn parse_name(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Ref<Name>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        let start: Position = self.lexer.get_previous_position();

        let name: String = self.lexer.parse_identifier();
        match (name != null) && isIdentifier(name) {
            Some (x) =>
                lexer.advance(),
            None =>
                return None
        }

        let extensions: Option<Ref<Vector<ExtensionSyntax>>> = parse_extension_list();

        let end: Position = self.lexer.get_position();

        let ret: NameSyntax = Ref::new(_rp, NameSyntax { start: start, end: end, name: name, extensions: extensions});

        match extensions {
            Some(x) => for item in x {
                item.parent = ret;
            }
            None => ()
        }

        ret
    }

    pub fn parseExtensionList(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
    ) -> Option<Ref<Vector<Ref<ExtensionSyntax>>>> {
        let _r = Region::create(_pr);
        let mut ret: Option<Ref<Array<Ref<ExtensionSyntax>>>> = Option::None;
        loop {
            let node = self.parse_Extension(&_r, _rp);
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
            Some(ret) => Some(Ref::new(_rp, Vector::from_array(_rp, ret))),
            None => None,
        }
    }

    pub fn parse_extension(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Ref<Extension>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        let start: Position = self.lexer.get_previous_position();

        let successDot1: bool = self.lexer.parse_punctuation(".");
        match successDot1 {
            Some (x) =>
                lexer.advance(),
            None =>
                return None
        }

        let name: String = self.lexer.parse_identifier();
        match (name != null) && isIdentifier(name) {
            Some (x) =>
                lexer.advance(),
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

        let ret: ExtensionSyntax = Ref::new(_rp, ExtensionSyntax { start: start, end: end, name: name});

        ret
    }

    pub fn parseDeclarationList(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
    ) -> Option<Ref<Vector<Ref<DeclarationSyntax>>>> {
        let _r = Region::create(_pr);
        let mut ret: Option<Ref<Array<Ref<DeclarationSyntax>>>> = Option::None;
        loop {
            let node = self.parse_Declaration(&_r, _rp);
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
            Some(ret) => Some(Ref::new(_rp, Vector::from_array(_rp, ret))),
            None => None,
        }
    }

    pub fn parse_declaration(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Ref<Declaration>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        {
            let node = parseNamespace(&_r, _rp);
            match node {
                OK(node_ok) => match node_ok {
                    Some(node_ret) => return node,
                    None => ()
                }
                Error(_) => return node,
            }
        }

        {
            let node = parseFunction(&_r, _rp);
            match node {
                OK(node_ok) => match node_ok {
                    Some(node_ret) => return node,
                    None => ()
                }
                Error(_) => return node,
            }
        }

        {
            let node = parseClass(&_r, _rp);
            match node {
                OK(node_ok) => match node_ok {
                    Some(node_ret) => return node,
                    None => ()
                }
                Error(_) => return node,
            }
        }

        {
            let node = parseLetDeclaration(&_r, _rp);
            match node {
                OK(node_ok) => match node_ok {
                    Some(node_ret) => return node,
                    None => ()
                }
                Error(_) => return node,
            }
        }

        {
            let node = parseVarDeclaration(&_r, _rp);
            match node {
                OK(node_ok) => match node_ok {
                    Some(node_ret) => return node,
                    None => ()
                }
                Error(_) => return node,
            }
        }

        {
            let node = parseMutableDeclaration(&_r, _rp);
            match node {
                OK(node_ok) => match node_ok {
                    Some(node_ret) => return node,
                    None => ()
                }
                Error(_) => return node,
            }
        }

        {
            let node = parseThreadLocalDeclaration(&_r, _rp);
            match node {
                OK(node_ok) => match node_ok {
                    Some(node_ret) => return node,
                    None => ()
                }
                Error(_) => return node,
            }
        }

        return None
    }

    pub fn parse_namespace(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Ref<Namespace>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        let start: Position = self.lexer.get_previous_position();

        let successNamespace1: bool = self.lexer.parse_keyword("namespace");
        match successNamespace1 {
            Some (x) =>
                lexer.advance(),
            None =>
                return None
        }

        let name: Ref<NameSyntax> = parse_name();
        match name {
            None => {
                return Err(Ref::new(_ep, parser_exception(String::copy(_ep, self.file_name), lexer.line, lexer.column))
            }

        let successLeftCurly3: bool = self.lexer.parse_punctuation("{");
        match successLeftCurly3 {
            Some (x) =>
                lexer.advance(),
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

        let usings: Option<Ref<Vector<UsingSyntax>>> = parse_using_list();

        let defines: Option<Ref<Vector<DefineSyntax>>> = parse_define_list();

        let declarations: Option<Ref<Vector<DeclarationSyntax>>> = parse_declaration_list();

        let successRightCurly7: bool = self.lexer.parse_punctuation("}");
        match successRightCurly7 {
            Some (x) =>
                lexer.advance(),
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

        let ret: NamespaceSyntax = Ref::new(_rp, NamespaceSyntax { start: start, end: end, name: name, usings: usings, defines: defines, declarations: declarations});

            name.parent = ret;

        match usings {
            Some(x) => for item in x {
                item.parent = ret;
            }
            None => ()
        }

        match defines {
            Some(x) => for item in x {
                item.parent = ret;
            }
            None => ()
        }

        match declarations {
            Some(x) => for item in x {
                item.parent = ret;
            }
            None => ()
        }

        ret
    }

    pub fn parse_function(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Ref<Function>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        let start: Position = self.lexer.get_previous_position();

        let successFunction1: bool = self.lexer.parse_keyword("function");
        match successFunction1 {
            Some (x) =>
                lexer.advance(),
            None =>
                return None
        }

        let procedure: Ref<ProcedureSyntax> = parse_procedure();
        match procedure {
            None => {
                return Err(Ref::new(_ep, parser_exception(String::copy(_ep, self.file_name), lexer.line, lexer.column))
            }

        let end: Position = self.lexer.get_position();

        let ret: FunctionSyntax = Ref::new(_rp, FunctionSyntax { start: start, end: end, procedure: procedure});

            procedure.parent = ret;

        ret
    }

    pub fn parse_procedure(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Ref<Procedure>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        let start: Position = self.lexer.get_previous_position();

        let name: String = self.lexer.parse_identifier();
        match (name != null) && isIdentifier(name) {
            Some (x) =>
                lexer.advance(),
            None =>
                return None
        }

        let routine: Ref<RoutineSyntax> = parse_routine();
        match routine {
            None => {
                return Err(Ref::new(_ep, parser_exception(String::copy(_ep, self.file_name), lexer.line, lexer.column))
            }

        let end: Position = self.lexer.get_position();

        let ret: ProcedureSyntax = Ref::new(_rp, ProcedureSyntax { start: start, end: end, name: name, routine: routine});

            routine.parent = ret;

        ret
    }

    pub fn parse_routine(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Ref<Routine>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        let start: Position = self.lexer.get_previous_position();

        let input: Option<Ref<StructureSyntax>> = parse_structure();

        let output: Option<Ref<TypeAnnotationSyntax>> = parse_typeAnnotation();

        let throwsClause: Option<Ref<ThrowsSyntax>> = parse_throws();

        let body: Ref<BlockSyntax> = parse_block();
        match body {
            None => {
                return Err(Ref::new(_ep, parser_exception(String::copy(_ep, self.file_name), lexer.line, lexer.column))
            }

        let end: Position = self.lexer.get_position();

        let ret: RoutineSyntax = Ref::new(_rp, RoutineSyntax { start: start, end: end, input: input, output: output, throwsClause: throwsClause, body: body});

        match input {
            Some(x) => 
                x.parent = ret;
            None => ()
        }

        match output {
            Some(x) => 
                x.parent = ret;
            None => ()
        }

        match throwsClause {
            Some(x) => 
                x.parent = ret;
            None => ()
        }

            body.parent = ret;

        ret
    }

    pub fn parse_letdeclaration(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Ref<LetDeclaration>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        let start: Position = self.lexer.get_previous_position();

        let declaration: Ref<LetSyntax> = parse_let();
        match declaration {
            None => {
                return Ok(None)
            }

        let end: Position = self.lexer.get_position();

        let ret: LetDeclarationSyntax = Ref::new(_rp, LetDeclarationSyntax { start: start, end: end, declaration: declaration});

            declaration.parent = ret;

        ret
    }

    pub fn parse_vardeclaration(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Ref<VarDeclaration>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        let start: Position = self.lexer.get_previous_position();

        let declaration: Ref<VarSyntax> = parse_var();
        match declaration {
            None => {
                return Ok(None)
            }

        let end: Position = self.lexer.get_position();

        let ret: VarDeclarationSyntax = Ref::new(_rp, VarDeclarationSyntax { start: start, end: end, declaration: declaration});

            declaration.parent = ret;

        ret
    }

    pub fn parse_mutabledeclaration(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Ref<MutableDeclaration>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        let start: Position = self.lexer.get_previous_position();

        let declaration: Ref<MutableSyntax> = parse_mutable();
        match declaration {
            None => {
                return Ok(None)
            }

        let end: Position = self.lexer.get_position();

        let ret: MutableDeclarationSyntax = Ref::new(_rp, MutableDeclarationSyntax { start: start, end: end, declaration: declaration});

            declaration.parent = ret;

        ret
    }

    pub fn parse_threadlocaldeclaration(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Ref<ThreadLocalDeclaration>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        let start: Position = self.lexer.get_previous_position();

        let declaration: Ref<ThreadLocalSyntax> = parse_threadLocal();
        match declaration {
            None => {
                return Ok(None)
            }

        let end: Position = self.lexer.get_position();

        let ret: ThreadLocalDeclarationSyntax = Ref::new(_rp, ThreadLocalDeclarationSyntax { start: start, end: end, declaration: declaration});

            declaration.parent = ret;

        ret
    }

    pub fn parseStatementList(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
    ) -> Option<Ref<Vector<Ref<StatementSyntax>>>> {
        let _r = Region::create(_pr);
        let mut ret: Option<Ref<Array<Ref<StatementSyntax>>>> = Option::None;
        loop {
            let node = self.parse_Statement(&_r, _rp);
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
            Some(ret) => Some(Ref::new(_rp, Vector::from_array(_rp, ret))),
            None => None,
        }
    }

    pub fn parse_statement(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Ref<Statement>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        {
            let node = parseLet(&_r, _rp);
            match node {
                OK(node_ok) => match node_ok {
                    Some(node_ret) => return node,
                    None => ()
                }
                Error(_) => return node,
            }
        }

        {
            let node = parseVar(&_r, _rp);
            match node {
                OK(node_ok) => match node_ok {
                    Some(node_ret) => return node,
                    None => ()
                }
                Error(_) => return node,
            }
        }

        {
            let node = parseMutable(&_r, _rp);
            match node {
                OK(node_ok) => match node_ok {
                    Some(node_ret) => return node,
                    None => ()
                }
                Error(_) => return node,
            }
        }

        {
            let node = parseThreadLocal(&_r, _rp);
            match node {
                OK(node_ok) => match node_ok {
                    Some(node_ret) => return node,
                    None => ()
                }
                Error(_) => return node,
            }
        }

        {
            let node = parseSet(&_r, _rp);
            match node {
                OK(node_ok) => match node_ok {
                    Some(node_ret) => return node,
                    None => ()
                }
                Error(_) => return node,
            }
        }

        {
            let node = parseCalculation(&_r, _rp);
            match node {
                OK(node_ok) => match node_ok {
                    Some(node_ret) => return node,
                    None => ()
                }
                Error(_) => return node,
            }
        }

        {
            let node = parseBreak(&_r, _rp);
            match node {
                OK(node_ok) => match node_ok {
                    Some(node_ret) => return node,
                    None => ()
                }
                Error(_) => return node,
            }
        }

        {
            let node = parseContinue(&_r, _rp);
            match node {
                OK(node_ok) => match node_ok {
                    Some(node_ret) => return node,
                    None => ()
                }
                Error(_) => return node,
            }
        }

        {
            let node = parseReturn(&_r, _rp);
            match node {
                OK(node_ok) => match node_ok {
                    Some(node_ret) => return node,
                    None => ()
                }
                Error(_) => return node,
            }
        }

        {
            let node = parseThrow(&_r, _rp);
            match node {
                OK(node_ok) => match node_ok {
                    Some(node_ret) => return node,
                    None => ()
                }
                Error(_) => return node,
            }
        }

        return None
    }

    pub fn parse_let(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Ref<Let>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        let start: Position = self.lexer.get_previous_position();

        let successLet1: bool = self.lexer.parse_keyword("let");
        match successLet1 {
            Some (x) =>
                lexer.advance(),
            None =>
                return None
        }

        let binding: Ref<BindingSyntax> = parse_binding();
        match binding {
            None => {
                return Err(Ref::new(_ep, parser_exception(String::copy(_ep, self.file_name), lexer.line, lexer.column))
            }

        let end: Position = self.lexer.get_position();

        let ret: LetSyntax = Ref::new(_rp, LetSyntax { start: start, end: end, binding: binding});

            binding.parent = ret;

        ret
    }

    pub fn parse_var(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Ref<Var>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        let start: Position = self.lexer.get_previous_position();

        let successVar1: bool = self.lexer.parse_keyword("var");
        match successVar1 {
            Some (x) =>
                lexer.advance(),
            None =>
                return None
        }

        let binding: Ref<BindingSyntax> = parse_binding();
        match binding {
            None => {
                return Err(Ref::new(_ep, parser_exception(String::copy(_ep, self.file_name), lexer.line, lexer.column))
            }

        let end: Position = self.lexer.get_position();

        let ret: VarSyntax = Ref::new(_rp, VarSyntax { start: start, end: end, binding: binding});

            binding.parent = ret;

        ret
    }

    pub fn parse_mutable(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Ref<Mutable>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        let start: Position = self.lexer.get_previous_position();

        let successMutable1: bool = self.lexer.parse_keyword("mutable");
        match successMutable1 {
            Some (x) =>
                lexer.advance(),
            None =>
                return None
        }

        let binding: Ref<BindingSyntax> = parse_binding();
        match binding {
            None => {
                return Err(Ref::new(_ep, parser_exception(String::copy(_ep, self.file_name), lexer.line, lexer.column))
            }

        let end: Position = self.lexer.get_position();

        let ret: MutableSyntax = Ref::new(_rp, MutableSyntax { start: start, end: end, binding: binding});

            binding.parent = ret;

        ret
    }

    pub fn parse_threadlocal(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Ref<ThreadLocal>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        let start: Position = self.lexer.get_previous_position();

        let successThreadlocal1: bool = self.lexer.parse_keyword("threadlocal");
        match successThreadlocal1 {
            Some (x) =>
                lexer.advance(),
            None =>
                return None
        }

        let binding: Ref<BindingSyntax> = parse_binding();
        match binding {
            None => {
                return Err(Ref::new(_ep, parser_exception(String::copy(_ep, self.file_name), lexer.line, lexer.column))
            }

        let end: Position = self.lexer.get_position();

        let ret: ThreadLocalSyntax = Ref::new(_rp, ThreadLocalSyntax { start: start, end: end, binding: binding});

            binding.parent = ret;

        ret
    }

    pub fn parse_binding(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Ref<Binding>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        let start: Position = self.lexer.get_previous_position();

        let name: String = self.lexer.parse_identifier();
        match (name != null) && isIdentifier(name) {
            Some (x) =>
                lexer.advance(),
            None =>
                return None
        }

        let typeAnnotation: Option<Ref<TypeAnnotationSyntax>> = parse_typeAnnotation();

        let calculation: Ref<CalculationSyntax> = parse_calculation();
        match calculation {
            None => {
                return Err(Ref::new(_ep, parser_exception(String::copy(_ep, self.file_name), lexer.line, lexer.column))
            }

        let end: Position = self.lexer.get_position();

        let ret: BindingSyntax = Ref::new(_rp, BindingSyntax { start: start, end: end, name: name, typeAnnotation: typeAnnotation, calculation: calculation});

        match typeAnnotation {
            Some(x) => 
                x.parent = ret;
            None => ()
        }

            calculation.parent = ret;

        ret
    }

    pub fn parse_set(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Ref<Set>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        let start: Position = self.lexer.get_previous_position();

        let successSet1: bool = self.lexer.parse_keyword("set");
        match successSet1 {
            Some (x) =>
                lexer.advance(),
            None =>
                return None
        }

        let lValue: Ref<OperationSyntax> = parse_operation();
        match lValue {
            None => {
                return Err(Ref::new(_ep, parser_exception(String::copy(_ep, self.file_name), lexer.line, lexer.column))
            }

        let successColon3: bool = self.lexer.parse_punctuation(":");
        match successColon3 {
            Some (x) =>
                lexer.advance(),
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

        let rValue: Ref<CalculationSyntax> = parse_calculation();
        match rValue {
            None => {
                return Err(Ref::new(_ep, parser_exception(String::copy(_ep, self.file_name), lexer.line, lexer.column))
            }

        let end: Position = self.lexer.get_position();

        let ret: SetSyntax = Ref::new(_rp, SetSyntax { start: start, end: end, lValue: lValue, rValue: rValue});

            lValue.parent = ret;

            rValue.parent = ret;

        ret
    }

    pub fn parse_calculation(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Ref<Calculation>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        let start: Position = self.lexer.get_previous_position();

        let operation: Ref<OperationSyntax> = parse_operation();
        match operation {
            None => {
                return Ok(None)
            }

        let successSemicolon2: bool = self.lexer.parse_punctuation(";");
        match successSemicolon2 {
            Some (x) =>
                lexer.advance(),
            None =>
()        }

        let end: Position = self.lexer.get_position();

        let ret: CalculationSyntax = Ref::new(_rp, CalculationSyntax { start: start, end: end, operation: operation});

            operation.parent = ret;

        ret
    }

    pub fn parse_operation(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Ref<Operation>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        let start: Position = self.lexer.get_previous_position();

        let op: Ref<Vector<OperandSyntax>> = parse_operand_list();
        match op {
            None => {
                return Ok(None)
            }

        let end: Position = self.lexer.get_position();

        let ret: OperationSyntax = Ref::new(_rp, OperationSyntax { start: start, end: end, op: op});

        match op {
            Some(x) => for item in x {
                item.parent = ret;
            }
            None => ()
        }

        ret
    }

    pub fn parseOperandList(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
    ) -> Option<Ref<Vector<Ref<OperandSyntax>>>> {
        let _r = Region::create(_pr);
        let mut ret: Option<Ref<Array<Ref<OperandSyntax>>>> = Option::None;
        loop {
            let node = self.parse_Operand(&_r, _rp);
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
            Some(ret) => Some(Ref::new(_rp, Vector::from_array(_rp, ret))),
            None => None,
        }
    }

    pub fn parse_operand(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Ref<Operand>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        let start: Position = self.lexer.get_previous_position();

        let primary: Ref<ExpressionSyntax> = parse_expression();
        match primary {
            None => {
                return Ok(None)
            }

        let postfixes: Option<Ref<Vector<PostfixSyntax>>> = parse_postfix_list();

        let end: Position = self.lexer.get_position();

        let ret: OperandSyntax = Ref::new(_rp, OperandSyntax { start: start, end: end, primary: primary, postfixes: postfixes});

            primary.parent = ret;

        match postfixes {
            Some(x) => for item in x {
                item.parent = ret;
            }
            None => ()
        }

        ret
    }

    pub fn parsePostfixList(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
    ) -> Option<Ref<Vector<Ref<PostfixSyntax>>>> {
        let _r = Region::create(_pr);
        let mut ret: Option<Ref<Array<Ref<PostfixSyntax>>>> = Option::None;
        loop {
            let node = self.parse_Postfix(&_r, _rp);
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
            Some(ret) => Some(Ref::new(_rp, Vector::from_array(_rp, ret))),
            None => None,
        }
    }

    pub fn parse_postfix(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Ref<Postfix>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        {
            let node = parseMemberAccess(&_r, _rp);
            match node {
                OK(node_ok) => match node_ok {
                    Some(node_ret) => return node,
                    None => ()
                }
                Error(_) => return node,
            }
        }

        {
            let node = parseAs(&_r, _rp);
            match node {
                OK(node_ok) => match node_ok {
                    Some(node_ret) => return node,
                    None => ()
                }
                Error(_) => return node,
            }
        }

        {
            let node = parseIs(&_r, _rp);
            match node {
                OK(node_ok) => match node_ok {
                    Some(node_ret) => return node,
                    None => ()
                }
                Error(_) => return node,
            }
        }

        {
            let node = parseUnwrap(&_r, _rp);
            match node {
                OK(node_ok) => match node_ok {
                    Some(node_ret) => return node,
                    None => ()
                }
                Error(_) => return node,
            }
        }

        {
            let node = parseCatch(&_r, _rp);
            match node {
                OK(node_ok) => match node_ok {
                    Some(node_ret) => return node,
                    None => ()
                }
                Error(_) => return node,
            }
        }

        return None
    }

    pub fn parse_memberaccess(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Ref<MemberAccess>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        let start: Position = self.lexer.get_previous_position();

        let successDot1: bool = self.lexer.parse_punctuation(".");
        match successDot1 {
            Some (x) =>
                lexer.advance(),
            None =>
                return None
        }

        let member: String = self.lexer.parse_identifier();
        match (member != null) && isIdentifier(member) {
            Some (x) =>
                lexer.advance(),
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

        let ret: MemberAccessSyntax = Ref::new(_rp, MemberAccessSyntax { start: start, end: end, member: member});

        ret
    }

    pub fn parse_as(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Ref<As>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        let start: Position = self.lexer.get_previous_position();

        let successAs1: bool = self.lexer.parse_keyword("as");
        match successAs1 {
            Some (x) =>
                lexer.advance(),
            None =>
                return None
        }

        let typeSpec: Ref<TypeSpecSyntax> = parse_typeSpec();
        match typeSpec {
            None => {
                return Err(Ref::new(_ep, parser_exception(String::copy(_ep, self.file_name), lexer.line, lexer.column))
            }

        let end: Position = self.lexer.get_position();

        let ret: AsSyntax = Ref::new(_rp, AsSyntax { start: start, end: end, typeSpec: typeSpec});

            typeSpec.parent = ret;

        ret
    }

    pub fn parse_is(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Ref<Is>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        let start: Position = self.lexer.get_previous_position();

        let successIs1: bool = self.lexer.parse_keyword("is");
        match successIs1 {
            Some (x) =>
                lexer.advance(),
            None =>
                return None
        }

        let typeSpec: Ref<TypeSpecSyntax> = parse_typeSpec();
        match typeSpec {
            None => {
                return Err(Ref::new(_ep, parser_exception(String::copy(_ep, self.file_name), lexer.line, lexer.column))
            }

        let end: Position = self.lexer.get_position();

        let ret: IsSyntax = Ref::new(_rp, IsSyntax { start: start, end: end, typeSpec: typeSpec});

            typeSpec.parent = ret;

        ret
    }

    pub fn parse_unwrap(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Ref<Unwrap>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        let start: Position = self.lexer.get_previous_position();

        let successExclamation1: bool = self.lexer.parse_punctuation("!");
        match successExclamation1 {
            Some (x) =>
                lexer.advance(),
            None =>
                return None
        }

        let end: Position = self.lexer.get_position();

        let ret: UnwrapSyntax = Ref::new(_rp, UnwrapSyntax { start: start, end: end});

        ret
    }

    pub fn parse_catch(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Ref<Catch>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        let start: Position = self.lexer.get_previous_position();

        let successCatch1: bool = self.lexer.parse_keyword("catch");
        match successCatch1 {
            Some (x) =>
                lexer.advance(),
            None =>
                return None
        }

        let typeSpec: Ref<CatchPatternSyntax> = parse_catchPattern();
        match typeSpec {
            None => {
                return Err(Ref::new(_ep, parser_exception(String::copy(_ep, self.file_name), lexer.line, lexer.column))
            }

        let handler: Ref<BlockSyntax> = parse_block();
        match handler {
            None => {
                return Err(Ref::new(_ep, parser_exception(String::copy(_ep, self.file_name), lexer.line, lexer.column))
            }

        let end: Position = self.lexer.get_position();

        let ret: CatchSyntax = Ref::new(_rp, CatchSyntax { start: start, end: end, typeSpec: typeSpec, handler: handler});

            typeSpec.parent = ret;

            handler.parent = ret;

        ret
    }

    pub fn parse_catchpattern(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Ref<CatchPattern>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        {
            let node = parseWildCardCatchPattern(&_r, _rp);
            match node {
                OK(node_ok) => match node_ok {
                    Some(node_ret) => return node,
                    None => ()
                }
                Error(_) => return node,
            }
        }

        {
            let node = parseTypeCatchPattern(&_r, _rp);
            match node {
                OK(node_ok) => match node_ok {
                    Some(node_ret) => return node,
                    None => ()
                }
                Error(_) => return node,
            }
        }

        return None
    }

    pub fn parse_wildcardcatchpattern(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Ref<WildCardCatchPattern>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        let start: Position = self.lexer.get_previous_position();

        let pattern: Ref<WildcardPatternSyntax> = parse_wildcardPattern();
        match pattern {
            None => {
                return Ok(None)
            }

        let end: Position = self.lexer.get_position();

        let ret: WildCardCatchPatternSyntax = Ref::new(_rp, WildCardCatchPatternSyntax { start: start, end: end, pattern: pattern});

            pattern.parent = ret;

        ret
    }

    pub fn parse_typecatchpattern(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Ref<TypeCatchPattern>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        let start: Position = self.lexer.get_previous_position();

        let typeSpec: Ref<TypeSpecSyntax> = parse_typeSpec();
        match typeSpec {
            None => {
                return Ok(None)
            }

        let errorName: String = self.lexer.parse_identifier();
        match (errorName != null) && isIdentifier(errorName) {
            Some (x) =>
                lexer.advance(),
            None =>
()        }

        let end: Position = self.lexer.get_position();

        let ret: TypeCatchPatternSyntax = Ref::new(_rp, TypeCatchPatternSyntax { start: start, end: end, typeSpec: typeSpec, errorName: errorName});

            typeSpec.parent = ret;

        ret
    }

    pub fn parseExpressionList(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
    ) -> Option<Ref<Vector<Ref<ExpressionSyntax>>>> {
        let _r = Region::create(_pr);
        let mut ret: Option<Ref<Array<Ref<ExpressionSyntax>>>> = Option::None;
        loop {
            let node = self.parse_Expression(&_r, _rp);
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
            Some(ret) => Some(Ref::new(_rp, Vector::from_array(_rp, ret))),
            None => None,
        }
    }

    pub fn parse_expression(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Ref<Expression>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        {
            let node = parseBlock(&_r, _rp);
            match node {
                OK(node_ok) => match node_ok {
                    Some(node_ret) => return node,
                    None => ()
                }
                Error(_) => return node,
            }
        }

        {
            let node = parseName(&_r, _rp);
            match node {
                OK(node_ok) => match node_ok {
                    Some(node_ret) => return node,
                    None => ()
                }
                Error(_) => return node,
            }
        }

        {
            let node = parseConstant(&_r, _rp);
            match node {
                OK(node_ok) => match node_ok {
                    Some(node_ret) => return node,
                    None => ()
                }
                Error(_) => return node,
            }
        }

        {
            let node = parseIf(&_r, _rp);
            match node {
                OK(node_ok) => match node_ok {
                    Some(node_ret) => return node,
                    None => ()
                }
                Error(_) => return node,
            }
        }

        {
            let node = parseSwitch(&_r, _rp);
            match node {
                OK(node_ok) => match node_ok {
                    Some(node_ret) => return node,
                    None => ()
                }
                Error(_) => return node,
            }
        }

        {
            let node = parseFor(&_r, _rp);
            match node {
                OK(node_ok) => match node_ok {
                    Some(node_ret) => return node,
                    None => ()
                }
                Error(_) => return node,
            }
        }

        {
            let node = parseWhile(&_r, _rp);
            match node {
                OK(node_ok) => match node_ok {
                    Some(node_ret) => return node,
                    None => ()
                }
                Error(_) => return node,
            }
        }

        {
            let node = parseDo(&_r, _rp);
            match node {
                OK(node_ok) => match node_ok {
                    Some(node_ret) => return node,
                    None => ()
                }
                Error(_) => return node,
            }
        }

        {
            let node = parseThis(&_r, _rp);
            match node {
                OK(node_ok) => match node_ok {
                    Some(node_ret) => return node,
                    None => ()
                }
                Error(_) => return node,
            }
        }

        {
            let node = parseNew(&_r, _rp);
            match node {
                OK(node_ok) => match node_ok {
                    Some(node_ret) => return node,
                    None => ()
                }
                Error(_) => return node,
            }
        }

        {
            let node = parseObject(&_r, _rp);
            match node {
                OK(node_ok) => match node_ok {
                    Some(node_ret) => return node,
                    None => ()
                }
                Error(_) => return node,
            }
        }

        {
            let node = parseArray(&_r, _rp);
            match node {
                OK(node_ok) => match node_ok {
                    Some(node_ret) => return node,
                    None => ()
                }
                Error(_) => return node,
            }
        }

        {
            let node = parseSizeOf(&_r, _rp);
            match node {
                OK(node_ok) => match node_ok {
                    Some(node_ret) => return node,
                    None => ()
                }
                Error(_) => return node,
            }
        }

        return None
    }

    pub fn parse_block(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Ref<Block>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        let start: Position = self.lexer.get_previous_position();

        let successLeftCurly1: bool = self.lexer.parse_punctuation("{");
        match successLeftCurly1 {
            Some (x) =>
                lexer.advance(),
            None =>
                return None
        }

        let statements: Option<Ref<Vector<StatementSyntax>>> = parse_statement_list();

        let successRightCurly3: bool = self.lexer.parse_punctuation("}");
        match successRightCurly3 {
            Some (x) =>
                lexer.advance(),
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

        let ret: BlockSyntax = Ref::new(_rp, BlockSyntax { start: start, end: end, statements: statements});

        match statements {
            Some(x) => for item in x {
                item.parent = ret;
            }
            None => ()
        }

        ret
    }

    pub fn parse_constant(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Ref<Constant>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        let start: Position = self.lexer.get_previous_position();

        let literal: Literal = self.lexer.parse_literal();
        match literal != null {
            Some (x) =>
                lexer.advance(),
            None =>
                return None
        }

        let end: Position = self.lexer.get_position();

        let ret: ConstantSyntax = Ref::new(_rp, ConstantSyntax { start: start, end: end, literal: literal});

        ret
    }

    pub fn parse_if(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Ref<If>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        let start: Position = self.lexer.get_previous_position();

        let successIf1: bool = self.lexer.parse_keyword("if");
        match successIf1 {
            Some (x) =>
                lexer.advance(),
            None =>
                return None
        }

        let successLeftParen2: bool = self.lexer.parse_punctuation("(");
        match successLeftParen2 {
            Some (x) =>
                lexer.advance(),
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

        let condition: Ref<OperationSyntax> = parse_operation();
        match condition {
            None => {
                return Err(Ref::new(_ep, parser_exception(String::copy(_ep, self.file_name), lexer.line, lexer.column))
            }

        let successRightParen4: bool = self.lexer.parse_punctuation(")");
        match successRightParen4 {
            Some (x) =>
                lexer.advance(),
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

        let consequent: Ref<BlockSyntax> = parse_block();
        match consequent {
            None => {
                return Err(Ref::new(_ep, parser_exception(String::copy(_ep, self.file_name), lexer.line, lexer.column))
            }

        let elseClause: Option<Ref<ElseSyntax>> = parse_else();

        let end: Position = self.lexer.get_position();

        let ret: IfSyntax = Ref::new(_rp, IfSyntax { start: start, end: end, condition: condition, consequent: consequent, elseClause: elseClause});

            condition.parent = ret;

            consequent.parent = ret;

        match elseClause {
            Some(x) => 
                x.parent = ret;
            None => ()
        }

        ret
    }

    pub fn parse_else(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Ref<Else>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        let start: Position = self.lexer.get_previous_position();

        let successElse1: bool = self.lexer.parse_keyword("else");
        match successElse1 {
            Some (x) =>
                lexer.advance(),
            None =>
                return None
        }

        let alternative: Ref<BlockSyntax> = parse_block();
        match alternative {
            None => {
                return Err(Ref::new(_ep, parser_exception(String::copy(_ep, self.file_name), lexer.line, lexer.column))
            }

        let end: Position = self.lexer.get_position();

        let ret: ElseSyntax = Ref::new(_rp, ElseSyntax { start: start, end: end, alternative: alternative});

            alternative.parent = ret;

        ret
    }

    pub fn parse_switch(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Ref<Switch>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        let start: Position = self.lexer.get_previous_position();

        let successSwitch1: bool = self.lexer.parse_keyword("switch");
        match successSwitch1 {
            Some (x) =>
                lexer.advance(),
            None =>
                return None
        }

        let successLeftParen2: bool = self.lexer.parse_punctuation("(");
        match successLeftParen2 {
            Some (x) =>
                lexer.advance(),
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

        let condition: Ref<OperationSyntax> = parse_operation();
        match condition {
            None => {
                return Err(Ref::new(_ep, parser_exception(String::copy(_ep, self.file_name), lexer.line, lexer.column))
            }

        let successRightParen4: bool = self.lexer.parse_punctuation(")");
        match successRightParen4 {
            Some (x) =>
                lexer.advance(),
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

        let successLeftCurly5: bool = self.lexer.parse_punctuation("{");
        match successLeftCurly5 {
            Some (x) =>
                lexer.advance(),
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

        let cases: Ref<Vector<SwitchCaseSyntax>> = parse_switchCase_list();
        match cases {
            None => {
                return Err(Ref::new(_ep, parser_exception(String::copy(_ep, self.file_name), lexer.line, lexer.column))
            }

        let successRightCurly7: bool = self.lexer.parse_punctuation("}");
        match successRightCurly7 {
            Some (x) =>
                lexer.advance(),
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

        let ret: SwitchSyntax = Ref::new(_rp, SwitchSyntax { start: start, end: end, condition: condition, cases: cases});

            condition.parent = ret;

        match cases {
            Some(x) => for item in x {
                item.parent = ret;
            }
            None => ()
        }

        ret
    }

    pub fn parseSwitchCaseList(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
    ) -> Option<Ref<Vector<Ref<SwitchCaseSyntax>>>> {
        let _r = Region::create(_pr);
        let mut ret: Option<Ref<Array<Ref<SwitchCaseSyntax>>>> = Option::None;
        loop {
            let node = self.parse_SwitchCase(&_r, _rp);
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
            Some(ret) => Some(Ref::new(_rp, Vector::from_array(_rp, ret))),
            None => None,
        }
    }

    pub fn parse_switchcase(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Ref<SwitchCase>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        let start: Position = self.lexer.get_previous_position();

        let label: Ref<CaseLabelSyntax> = parse_caseLabel();
        match label {
            None => {
                return Ok(None)
            }

        let content: Ref<BlockSyntax> = parse_block();
        match content {
            None => {
                return Err(Ref::new(_ep, parser_exception(String::copy(_ep, self.file_name), lexer.line, lexer.column))
            }

        let end: Position = self.lexer.get_position();

        let ret: SwitchCaseSyntax = Ref::new(_rp, SwitchCaseSyntax { start: start, end: end, label: label, content: content});

            label.parent = ret;

            content.parent = ret;

        ret
    }

    pub fn parse_caselabel(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Ref<CaseLabel>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        {
            let node = parseItemCaseLabel(&_r, _rp);
            match node {
                OK(node_ok) => match node_ok {
                    Some(node_ret) => return node,
                    None => ()
                }
                Error(_) => return node,
            }
        }

        {
            let node = parseDefaultCaseLabel(&_r, _rp);
            match node {
                OK(node_ok) => match node_ok {
                    Some(node_ret) => return node,
                    None => ()
                }
                Error(_) => return node,
            }
        }

        return None
    }

    pub fn parse_itemcaselabel(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Ref<ItemCaseLabel>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        let start: Position = self.lexer.get_previous_position();

        let successCase1: bool = self.lexer.parse_keyword("case");
        match successCase1 {
            Some (x) =>
                lexer.advance(),
            None =>
                return None
        }

        let items: Ref<Vector<CaseItemSyntax>> = parse_caseItem_list();
        match items {
            None => {
                return Err(Ref::new(_ep, parser_exception(String::copy(_ep, self.file_name), lexer.line, lexer.column))
            }

        let end: Position = self.lexer.get_position();

        let ret: ItemCaseLabelSyntax = Ref::new(_rp, ItemCaseLabelSyntax { start: start, end: end, items: items});

        match items {
            Some(x) => for item in x {
                item.parent = ret;
            }
            None => ()
        }

        ret
    }

    pub fn parseCaseItemList(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
    ) -> Option<Ref<Vector<Ref<CaseItemSyntax>>>> {
        let _r = Region::create(_pr);
        let mut ret: Option<Ref<Array<Ref<CaseItemSyntax>>>> = Option::None;
        loop {
            let node = self.parse_CaseItem(&_r, _rp);
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
            Some(ret) => Some(Ref::new(_rp, Vector::from_array(_rp, ret))),
            None => None,
        }
    }

    pub fn parse_caseitem(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Ref<CaseItem>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        let start: Position = self.lexer.get_previous_position();

        let pattern: Ref<CasePatternSyntax> = parse_casePattern();
        match pattern {
            None => {
                return Ok(None)
            }

        let successComma2: bool = self.lexer.parse_punctuation(",");
        match successComma2 {
            Some (x) =>
                lexer.advance(),
            None =>
()        }

        let end: Position = self.lexer.get_position();

        let ret: CaseItemSyntax = Ref::new(_rp, CaseItemSyntax { start: start, end: end, pattern: pattern});

            pattern.parent = ret;

        ret
    }

    pub fn parseCasePatternList(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
    ) -> Option<Ref<Vector<Ref<CasePatternSyntax>>>> {
        let _r = Region::create(_pr);
        let mut ret: Option<Ref<Array<Ref<CasePatternSyntax>>>> = Option::None;
        loop {
            let node = self.parse_CasePattern(&_r, _rp);
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
            Some(ret) => Some(Ref::new(_rp, Vector::from_array(_rp, ret))),
            None => None,
        }
    }

    pub fn parse_casepattern(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Ref<CasePattern>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        {
            let node = parseConstantPattern(&_r, _rp);
            match node {
                OK(node_ok) => match node_ok {
                    Some(node_ret) => return node,
                    None => ()
                }
                Error(_) => return node,
            }
        }

        {
            let node = parseWildcardPattern(&_r, _rp);
            match node {
                OK(node_ok) => match node_ok {
                    Some(node_ret) => return node,
                    None => ()
                }
                Error(_) => return node,
            }
        }

        {
            let node = parseNamePattern(&_r, _rp);
            match node {
                OK(node_ok) => match node_ok {
                    Some(node_ret) => return node,
                    None => ()
                }
                Error(_) => return node,
            }
        }

        return None
    }

    pub fn parse_constantpattern(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Ref<ConstantPattern>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        let start: Position = self.lexer.get_previous_position();

        let constant: Ref<ConstantSyntax> = parse_constant();
        match constant {
            None => {
                return Ok(None)
            }

        let end: Position = self.lexer.get_position();

        let ret: ConstantPatternSyntax = Ref::new(_rp, ConstantPatternSyntax { start: start, end: end, constant: constant});

            constant.parent = ret;

        ret
    }

    pub fn parse_wildcardpattern(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Ref<WildcardPattern>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        let start: Position = self.lexer.get_previous_position();

        let successUnderscore1: bool = self.lexer.parse_punctuation("_");
        match successUnderscore1 {
            Some (x) =>
                lexer.advance(),
            None =>
                return None
        }

        let end: Position = self.lexer.get_position();

        let ret: WildcardPatternSyntax = Ref::new(_rp, WildcardPatternSyntax { start: start, end: end});

        ret
    }

    pub fn parse_namepattern(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Ref<NamePattern>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        let start: Position = self.lexer.get_previous_position();

        let name: Ref<NameSyntax> = parse_name();
        match name {
            None => {
                return Ok(None)
            }

        let end: Position = self.lexer.get_position();

        let ret: NamePatternSyntax = Ref::new(_rp, NamePatternSyntax { start: start, end: end, name: name});

            name.parent = ret;

        ret
    }

    pub fn parse_defaultcaselabel(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Ref<DefaultCaseLabel>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        let start: Position = self.lexer.get_previous_position();

        let successDefault1: bool = self.lexer.parse_keyword("default");
        match successDefault1 {
            Some (x) =>
                lexer.advance(),
            None =>
                return None
        }

        let end: Position = self.lexer.get_position();

        let ret: DefaultCaseLabelSyntax = Ref::new(_rp, DefaultCaseLabelSyntax { start: start, end: end});

        ret
    }

    pub fn parse_for(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Ref<For>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        let start: Position = self.lexer.get_previous_position();

        let successFor1: bool = self.lexer.parse_keyword("for");
        match successFor1 {
            Some (x) =>
                lexer.advance(),
            None =>
                return None
        }

        let successLeftParen2: bool = self.lexer.parse_punctuation("(");
        match successLeftParen2 {
            Some (x) =>
                lexer.advance(),
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

        let index: String = self.lexer.parse_identifier();
        match (index != null) && isIdentifier(index) {
            Some (x) =>
                lexer.advance(),
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

        let typeAnnotation: Option<Ref<TypeAnnotationSyntax>> = parse_typeAnnotation();

        let successIn5: bool = self.lexer.parse_keyword("in");
        match successIn5 {
            Some (x) =>
                lexer.advance(),
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

        let operation: Ref<OperationSyntax> = parse_operation();
        match operation {
            None => {
                return Err(Ref::new(_ep, parser_exception(String::copy(_ep, self.file_name), lexer.line, lexer.column))
            }

        let successRightParen7: bool = self.lexer.parse_punctuation(")");
        match successRightParen7 {
            Some (x) =>
                lexer.advance(),
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

        let iteration: Ref<LoopSyntax> = parse_loop();
        match iteration {
            None => {
                return Err(Ref::new(_ep, parser_exception(String::copy(_ep, self.file_name), lexer.line, lexer.column))
            }

        let end: Position = self.lexer.get_position();

        let ret: ForSyntax = Ref::new(_rp, ForSyntax { start: start, end: end, index: index, typeAnnotation: typeAnnotation, operation: operation, iteration: iteration});

        match typeAnnotation {
            Some(x) => 
                x.parent = ret;
            None => ()
        }

            operation.parent = ret;

            iteration.parent = ret;

        ret
    }

    pub fn parse_while(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Ref<While>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        let start: Position = self.lexer.get_previous_position();

        let successWhile1: bool = self.lexer.parse_keyword("while");
        match successWhile1 {
            Some (x) =>
                lexer.advance(),
            None =>
                return None
        }

        let successLeftParen2: bool = self.lexer.parse_punctuation("(");
        match successLeftParen2 {
            Some (x) =>
                lexer.advance(),
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

        let condition: Ref<OperationSyntax> = parse_operation();
        match condition {
            None => {
                return Err(Ref::new(_ep, parser_exception(String::copy(_ep, self.file_name), lexer.line, lexer.column))
            }

        let successRightParen4: bool = self.lexer.parse_punctuation(")");
        match successRightParen4 {
            Some (x) =>
                lexer.advance(),
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

        let iteration: Ref<LoopSyntax> = parse_loop();
        match iteration {
            None => {
                return Err(Ref::new(_ep, parser_exception(String::copy(_ep, self.file_name), lexer.line, lexer.column))
            }

        let end: Position = self.lexer.get_position();

        let ret: WhileSyntax = Ref::new(_rp, WhileSyntax { start: start, end: end, condition: condition, iteration: iteration});

            condition.parent = ret;

            iteration.parent = ret;

        ret
    }

    pub fn parse_do(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Ref<Do>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        let start: Position = self.lexer.get_previous_position();

        let successDo1: bool = self.lexer.parse_keyword("do");
        match successDo1 {
            Some (x) =>
                lexer.advance(),
            None =>
                return None
        }

        let iteration: Ref<LoopSyntax> = parse_loop();
        match iteration {
            None => {
                return Err(Ref::new(_ep, parser_exception(String::copy(_ep, self.file_name), lexer.line, lexer.column))
            }

        let successWhile3: bool = self.lexer.parse_keyword("while");
        match successWhile3 {
            Some (x) =>
                lexer.advance(),
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

        let successLeftParen4: bool = self.lexer.parse_punctuation("(");
        match successLeftParen4 {
            Some (x) =>
                lexer.advance(),
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

        let condition: Ref<OperationSyntax> = parse_operation();
        match condition {
            None => {
                return Err(Ref::new(_ep, parser_exception(String::copy(_ep, self.file_name), lexer.line, lexer.column))
            }

        let successRightParen6: bool = self.lexer.parse_punctuation(")");
        match successRightParen6 {
            Some (x) =>
                lexer.advance(),
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

        let ret: DoSyntax = Ref::new(_rp, DoSyntax { start: start, end: end, iteration: iteration, condition: condition});

            iteration.parent = ret;

            condition.parent = ret;

        ret
    }

    pub fn parse_loop(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Ref<Loop>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        {
            let node = parseSimpleLoop(&_r, _rp);
            match node {
                OK(node_ok) => match node_ok {
                    Some(node_ret) => return node,
                    None => ()
                }
                Error(_) => return node,
            }
        }

        {
            let node = parseNamedLoop(&_r, _rp);
            match node {
                OK(node_ok) => match node_ok {
                    Some(node_ret) => return node,
                    None => ()
                }
                Error(_) => return node,
            }
        }

        return None
    }

    pub fn parse_simpleloop(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Ref<SimpleLoop>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        let start: Position = self.lexer.get_previous_position();

        let code: Ref<BlockSyntax> = parse_block();
        match code {
            None => {
                return Ok(None)
            }

        let end: Position = self.lexer.get_position();

        let ret: SimpleLoopSyntax = Ref::new(_rp, SimpleLoopSyntax { start: start, end: end, code: code});

            code.parent = ret;

        ret
    }

    pub fn parse_namedloop(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Ref<NamedLoop>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        let start: Position = self.lexer.get_previous_position();

        let successLoop1: bool = self.lexer.parse_keyword("loop");
        match successLoop1 {
            Some (x) =>
                lexer.advance(),
            None =>
                return None
        }

        let name: String = self.lexer.parse_identifier();
        match (name != null) && isIdentifier(name) {
            Some (x) =>
                lexer.advance(),
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

        let code: Ref<BlockSyntax> = parse_block();
        match code {
            None => {
                return Err(Ref::new(_ep, parser_exception(String::copy(_ep, self.file_name), lexer.line, lexer.column))
            }

        let end: Position = self.lexer.get_position();

        let ret: NamedLoopSyntax = Ref::new(_rp, NamedLoopSyntax { start: start, end: end, name: name, code: code});

            code.parent = ret;

        ret
    }

    pub fn parse_this(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Ref<This>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        let start: Position = self.lexer.get_previous_position();

        let successThis1: bool = self.lexer.parse_keyword("this");
        match successThis1 {
            Some (x) =>
                lexer.advance(),
            None =>
                return None
        }

        let end: Position = self.lexer.get_position();

        let ret: ThisSyntax = Ref::new(_rp, ThisSyntax { start: start, end: end});

        ret
    }

    pub fn parse_new(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Ref<New>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        let start: Position = self.lexer.get_previous_position();

        let successNew1: bool = self.lexer.parse_keyword("new");
        match successNew1 {
            Some (x) =>
                lexer.advance(),
            None =>
                return None
        }

        let typeSpec: Ref<TypeSyntax> = parse_type();
        match typeSpec {
            None => {
                return Err(Ref::new(_ep, parser_exception(String::copy(_ep, self.file_name), lexer.line, lexer.column))
            }

        let end: Position = self.lexer.get_position();

        let ret: NewSyntax = Ref::new(_rp, NewSyntax { start: start, end: end, typeSpec: typeSpec});

            typeSpec.parent = ret;

        ret
    }

    pub fn parse_object(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Ref<Object>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        let start: Position = self.lexer.get_previous_position();

        let successLeftParen1: bool = self.lexer.parse_punctuation("(");
        match successLeftParen1 {
            Some (x) =>
                lexer.advance(),
            None =>
                return None
        }

        let firstOp: Option<Ref<OperationSyntax>> = parse_operation();

        let additionalOps: Option<Ref<Vector<ItemSyntax>>> = parse_item_list();

        let successRightParen4: bool = self.lexer.parse_punctuation(")");
        match successRightParen4 {
            Some (x) =>
                lexer.advance(),
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

        let ret: ObjectSyntax = Ref::new(_rp, ObjectSyntax { start: start, end: end, firstOp: firstOp, additionalOps: additionalOps});

        match firstOp {
            Some(x) => 
                x.parent = ret;
            None => ()
        }

        match additionalOps {
            Some(x) => for item in x {
                item.parent = ret;
            }
            None => ()
        }

        ret
    }

    pub fn parse_array(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Ref<Array>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        let start: Position = self.lexer.get_previous_position();

        let successLeftBracket1: bool = self.lexer.parse_punctuation("[");
        match successLeftBracket1 {
            Some (x) =>
                lexer.advance(),
            None =>
                return None
        }

        let firstOp: Option<Ref<OperationSyntax>> = parse_operation();

        let additionalOps: Option<Ref<Vector<ItemSyntax>>> = parse_item_list();

        let successRightBracket4: bool = self.lexer.parse_punctuation("]");
        match successRightBracket4 {
            Some (x) =>
                lexer.advance(),
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

        let ret: ArraySyntax = Ref::new(_rp, ArraySyntax { start: start, end: end, firstOp: firstOp, additionalOps: additionalOps});

        match firstOp {
            Some(x) => 
                x.parent = ret;
            None => ()
        }

        match additionalOps {
            Some(x) => for item in x {
                item.parent = ret;
            }
            None => ()
        }

        ret
    }

    pub fn parseItemList(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
    ) -> Option<Ref<Vector<Ref<ItemSyntax>>>> {
        let _r = Region::create(_pr);
        let mut ret: Option<Ref<Array<Ref<ItemSyntax>>>> = Option::None;
        loop {
            let node = self.parse_Item(&_r, _rp);
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
            Some(ret) => Some(Ref::new(_rp, Vector::from_array(_rp, ret))),
            None => None,
        }
    }

    pub fn parse_item(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Ref<Item>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        let start: Position = self.lexer.get_previous_position();

        let successComma1: bool = self.lexer.parse_punctuation(",");
        match successComma1 {
            Some (x) =>
                lexer.advance(),
            None =>
                return None
        }

        let operation: Ref<OperationSyntax> = parse_operation();
        match operation {
            None => {
                return Err(Ref::new(_ep, parser_exception(String::copy(_ep, self.file_name), lexer.line, lexer.column))
            }

        let end: Position = self.lexer.get_position();

        let ret: ItemSyntax = Ref::new(_rp, ItemSyntax { start: start, end: end, operation: operation});

            operation.parent = ret;

        ret
    }

    pub fn parse_sizeof(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Ref<SizeOf>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        let start: Position = self.lexer.get_previous_position();

        let successSizeof1: bool = self.lexer.parse_keyword("sizeof");
        match successSizeof1 {
            Some (x) =>
                lexer.advance(),
            None =>
                return None
        }

        let typeSpec: Ref<TypeSyntax> = parse_type();
        match typeSpec {
            None => {
                return Err(Ref::new(_ep, parser_exception(String::copy(_ep, self.file_name), lexer.line, lexer.column))
            }

        let end: Position = self.lexer.get_position();

        let ret: SizeOfSyntax = Ref::new(_rp, SizeOfSyntax { start: start, end: end, typeSpec: typeSpec});

            typeSpec.parent = ret;

        ret
    }

    pub fn parse_break(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Ref<Break>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        let start: Position = self.lexer.get_previous_position();

        let successBreak1: bool = self.lexer.parse_keyword("break");
        match successBreak1 {
            Some (x) =>
                lexer.advance(),
            None =>
                return None
        }

        let iteration: String = self.lexer.parse_identifier();
        match (iteration != null) && isIdentifier(iteration) {
            Some (x) =>
                lexer.advance(),
            None =>
()        }

        let successSemicolon3: bool = self.lexer.parse_punctuation(";");
        match successSemicolon3 {
            Some (x) =>
                lexer.advance(),
            None =>
()        }

        let end: Position = self.lexer.get_position();

        let ret: BreakSyntax = Ref::new(_rp, BreakSyntax { start: start, end: end, iteration: iteration});

        ret
    }

    pub fn parse_continue(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Ref<Continue>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        let start: Position = self.lexer.get_previous_position();

        let successContinue1: bool = self.lexer.parse_keyword("continue");
        match successContinue1 {
            Some (x) =>
                lexer.advance(),
            None =>
                return None
        }

        let iteration: String = self.lexer.parse_identifier();
        match (iteration != null) && isIdentifier(iteration) {
            Some (x) =>
                lexer.advance(),
            None =>
()        }

        let successSemicolon3: bool = self.lexer.parse_punctuation(";");
        match successSemicolon3 {
            Some (x) =>
                lexer.advance(),
            None =>
()        }

        let end: Position = self.lexer.get_position();

        let ret: ContinueSyntax = Ref::new(_rp, ContinueSyntax { start: start, end: end, iteration: iteration});

        ret
    }

    pub fn parse_return(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Ref<Return>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        let start: Position = self.lexer.get_previous_position();

        let successReturn1: bool = self.lexer.parse_keyword("return");
        match successReturn1 {
            Some (x) =>
                lexer.advance(),
            None =>
                return None
        }

        let result: Option<Ref<CalculationSyntax>> = parse_calculation();

        let end: Position = self.lexer.get_position();

        let ret: ReturnSyntax = Ref::new(_rp, ReturnSyntax { start: start, end: end, result: result});

        match result {
            Some(x) => 
                x.parent = ret;
            None => ()
        }

        ret
    }

    pub fn parse_throw(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Ref<Throw>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        let start: Position = self.lexer.get_previous_position();

        let successThrow1: bool = self.lexer.parse_keyword("throw");
        match successThrow1 {
            Some (x) =>
                lexer.advance(),
            None =>
                return None
        }

        let exception: Ref<CalculationSyntax> = parse_calculation();
        match exception {
            None => {
                return Err(Ref::new(_ep, parser_exception(String::copy(_ep, self.file_name), lexer.line, lexer.column))
            }

        let end: Position = self.lexer.get_position();

        let ret: ThrowSyntax = Ref::new(_rp, ThrowSyntax { start: start, end: end, exception: exception});

            exception.parent = ret;

        ret
    }

    pub fn parse_class(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Ref<Class>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        let start: Position = self.lexer.get_previous_position();

        let successClass1: bool = self.lexer.parse_keyword("class");
        match successClass1 {
            Some (x) =>
                lexer.advance(),
            None =>
                return None
        }

        let name: Ref<NameSyntax> = parse_name();
        match name {
            None => {
                return Err(Ref::new(_ep, parser_exception(String::copy(_ep, self.file_name), lexer.line, lexer.column))
            }

        let generics: Option<Ref<GenericParametersSyntax>> = parse_genericParameters();

        let baseClass: Option<Ref<ExtendsSyntax>> = parse_extends();

        let contents: Option<Ref<StructureSyntax>> = parse_structure();

        let body: Option<Ref<ClassBodySyntax>> = parse_classBody();

        let end: Position = self.lexer.get_position();

        let ret: ClassSyntax = Ref::new(_rp, ClassSyntax { start: start, end: end, name: name, generics: generics, baseClass: baseClass, contents: contents, body: body});

            name.parent = ret;

        match generics {
            Some(x) => 
                x.parent = ret;
            None => ()
        }

        match baseClass {
            Some(x) => 
                x.parent = ret;
            None => ()
        }

        match contents {
            Some(x) => 
                x.parent = ret;
            None => ()
        }

        match body {
            Some(x) => 
                x.parent = ret;
            None => ()
        }

        ret
    }

    pub fn parse_genericparameters(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Ref<GenericParameters>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        let start: Position = self.lexer.get_previous_position();

        let successLeftBracket1: bool = self.lexer.parse_punctuation("[");
        match successLeftBracket1 {
            Some (x) =>
                lexer.advance(),
            None =>
                return None
        }

        let name: String = self.lexer.parse_identifier();
        match (name != null) && isIdentifier(name) {
            Some (x) =>
                lexer.advance(),
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

        let additionalGenerics: Option<Ref<Vector<GenericParameterSyntax>>> = parse_genericParameter_list();

        let successRightBracket4: bool = self.lexer.parse_punctuation("]");
        match successRightBracket4 {
            Some (x) =>
                lexer.advance(),
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

        let ret: GenericParametersSyntax = Ref::new(_rp, GenericParametersSyntax { start: start, end: end, name: name, additionalGenerics: additionalGenerics});

        match additionalGenerics {
            Some(x) => for item in x {
                item.parent = ret;
            }
            None => ()
        }

        ret
    }

    pub fn parseGenericParameterList(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
    ) -> Option<Ref<Vector<Ref<GenericParameterSyntax>>>> {
        let _r = Region::create(_pr);
        let mut ret: Option<Ref<Array<Ref<GenericParameterSyntax>>>> = Option::None;
        loop {
            let node = self.parse_GenericParameter(&_r, _rp);
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
            Some(ret) => Some(Ref::new(_rp, Vector::from_array(_rp, ret))),
            None => None,
        }
    }

    pub fn parse_genericparameter(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Ref<GenericParameter>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        let start: Position = self.lexer.get_previous_position();

        let successComma1: bool = self.lexer.parse_punctuation(",");
        match successComma1 {
            Some (x) =>
                lexer.advance(),
            None =>
                return None
        }

        let name: String = self.lexer.parse_identifier();
        match (name != null) && isIdentifier(name) {
            Some (x) =>
                lexer.advance(),
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

        let ret: GenericParameterSyntax = Ref::new(_rp, GenericParameterSyntax { start: start, end: end, name: name});

        ret
    }

    pub fn parse_extends(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Ref<Extends>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        let start: Position = self.lexer.get_previous_position();

        let successExtends1: bool = self.lexer.parse_keyword("extends");
        match successExtends1 {
            Some (x) =>
                lexer.advance(),
            None =>
                return None
        }

        let name: Ref<NameSyntax> = parse_name();
        match name {
            None => {
                return Err(Ref::new(_ep, parser_exception(String::copy(_ep, self.file_name), lexer.line, lexer.column))
            }

        let end: Position = self.lexer.get_position();

        let ret: ExtendsSyntax = Ref::new(_rp, ExtendsSyntax { start: start, end: end, name: name});

            name.parent = ret;

        ret
    }

    pub fn parse_structure(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Ref<Structure>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        let start: Position = self.lexer.get_previous_position();

        let successLeftParen1: bool = self.lexer.parse_punctuation("(");
        match successLeftParen1 {
            Some (x) =>
                lexer.advance(),
            None =>
                return None
        }

        let components: Option<Ref<Vector<ComponentSyntax>>> = parse_component_list();

        let successRightParen3: bool = self.lexer.parse_punctuation(")");
        match successRightParen3 {
            Some (x) =>
                lexer.advance(),
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

        let ret: StructureSyntax = Ref::new(_rp, StructureSyntax { start: start, end: end, components: components});

        match components {
            Some(x) => for item in x {
                item.parent = ret;
            }
            None => ()
        }

        ret
    }

    pub fn parseComponentList(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
    ) -> Option<Ref<Vector<Ref<ComponentSyntax>>>> {
        let _r = Region::create(_pr);
        let mut ret: Option<Ref<Array<Ref<ComponentSyntax>>>> = Option::None;
        loop {
            let node = self.parse_Component(&_r, _rp);
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
            Some(ret) => Some(Ref::new(_rp, Vector::from_array(_rp, ret))),
            None => None,
        }
    }

    pub fn parse_component(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Ref<Component>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        let start: Position = self.lexer.get_previous_position();

        let name: String = self.lexer.parse_identifier();
        match (name != null) && isIdentifier(name) {
            Some (x) =>
                lexer.advance(),
            None =>
                return None
        }

        let typeAnnotation: Option<Ref<TypeAnnotationSyntax>> = parse_typeAnnotation();

        let successComma3: bool = self.lexer.parse_punctuation(",");
        match successComma3 {
            Some (x) =>
                lexer.advance(),
            None =>
()        }

        let end: Position = self.lexer.get_position();

        let ret: ComponentSyntax = Ref::new(_rp, ComponentSyntax { start: start, end: end, name: name, typeAnnotation: typeAnnotation});

        match typeAnnotation {
            Some(x) => 
                x.parent = ret;
            None => ()
        }

        ret
    }

    pub fn parse_classbody(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Ref<ClassBody>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        let start: Position = self.lexer.get_previous_position();

        let successLeftCurly1: bool = self.lexer.parse_punctuation("{");
        match successLeftCurly1 {
            Some (x) =>
                lexer.advance(),
            None =>
                return None
        }

        let members: Option<Ref<Vector<ClassMemberSyntax>>> = parse_classMember_list();

        let successRightCurly3: bool = self.lexer.parse_punctuation("}");
        match successRightCurly3 {
            Some (x) =>
                lexer.advance(),
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

        let ret: ClassBodySyntax = Ref::new(_rp, ClassBodySyntax { start: start, end: end, members: members});

        match members {
            Some(x) => for item in x {
                item.parent = ret;
            }
            None => ()
        }

        ret
    }

    pub fn parseClassMemberList(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
    ) -> Option<Ref<Vector<Ref<ClassMemberSyntax>>>> {
        let _r = Region::create(_pr);
        let mut ret: Option<Ref<Array<Ref<ClassMemberSyntax>>>> = Option::None;
        loop {
            let node = self.parse_ClassMember(&_r, _rp);
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
            Some(ret) => Some(Ref::new(_rp, Vector::from_array(_rp, ret))),
            None => None,
        }
    }

    pub fn parse_classmember(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Ref<ClassMember>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        {
            let node = parseLetMember(&_r, _rp);
            match node {
                OK(node_ok) => match node_ok {
                    Some(node_ret) => return node,
                    None => ()
                }
                Error(_) => return node,
            }
        }

        {
            let node = parseVarMember(&_r, _rp);
            match node {
                OK(node_ok) => match node_ok {
                    Some(node_ret) => return node,
                    None => ()
                }
                Error(_) => return node,
            }
        }

        {
            let node = parseMutableMember(&_r, _rp);
            match node {
                OK(node_ok) => match node_ok {
                    Some(node_ret) => return node,
                    None => ()
                }
                Error(_) => return node,
            }
        }

        {
            let node = parseSetInitialization(&_r, _rp);
            match node {
                OK(node_ok) => match node_ok {
                    Some(node_ret) => return node,
                    None => ()
                }
                Error(_) => return node,
            }
        }

        {
            let node = parseMethod(&_r, _rp);
            match node {
                OK(node_ok) => match node_ok {
                    Some(node_ret) => return node,
                    None => ()
                }
                Error(_) => return node,
            }
        }

        {
            let node = parseStaticFunction(&_r, _rp);
            match node {
                OK(node_ok) => match node_ok {
                    Some(node_ret) => return node,
                    None => ()
                }
                Error(_) => return node,
            }
        }

        {
            let node = parseOperator(&_r, _rp);
            match node {
                OK(node_ok) => match node_ok {
                    Some(node_ret) => return node,
                    None => ()
                }
                Error(_) => return node,
            }
        }

        {
            let node = parseInitializer(&_r, _rp);
            match node {
                OK(node_ok) => match node_ok {
                    Some(node_ret) => return node,
                    None => ()
                }
                Error(_) => return node,
            }
        }

        {
            let node = parseAllocator(&_r, _rp);
            match node {
                OK(node_ok) => match node_ok {
                    Some(node_ret) => return node,
                    None => ()
                }
                Error(_) => return node,
            }
        }

        return None
    }

    pub fn parse_letmember(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Ref<LetMember>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        let start: Position = self.lexer.get_previous_position();

        let declaration: Ref<LetSyntax> = parse_let();
        match declaration {
            None => {
                return Ok(None)
            }

        let end: Position = self.lexer.get_position();

        let ret: LetMemberSyntax = Ref::new(_rp, LetMemberSyntax { start: start, end: end, declaration: declaration});

            declaration.parent = ret;

        ret
    }

    pub fn parse_varmember(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Ref<VarMember>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        let start: Position = self.lexer.get_previous_position();

        let declaration: Ref<VarSyntax> = parse_var();
        match declaration {
            None => {
                return Ok(None)
            }

        let end: Position = self.lexer.get_position();

        let ret: VarMemberSyntax = Ref::new(_rp, VarMemberSyntax { start: start, end: end, declaration: declaration});

            declaration.parent = ret;

        ret
    }

    pub fn parse_mutablemember(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Ref<MutableMember>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        let start: Position = self.lexer.get_previous_position();

        let declaration: Ref<MutableSyntax> = parse_mutable();
        match declaration {
            None => {
                return Ok(None)
            }

        let end: Position = self.lexer.get_position();

        let ret: MutableMemberSyntax = Ref::new(_rp, MutableMemberSyntax { start: start, end: end, declaration: declaration});

            declaration.parent = ret;

        ret
    }

    pub fn parse_setinitialization(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Ref<SetInitialization>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        let start: Position = self.lexer.get_previous_position();

        let definition: Ref<SetSyntax> = parse_set();
        match definition {
            None => {
                return Ok(None)
            }

        let end: Position = self.lexer.get_position();

        let ret: SetInitializationSyntax = Ref::new(_rp, SetInitializationSyntax { start: start, end: end, definition: definition});

            definition.parent = ret;

        ret
    }

    pub fn parse_method(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Ref<Method>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        let start: Position = self.lexer.get_previous_position();

        let successMethod1: bool = self.lexer.parse_keyword("method");
        match successMethod1 {
            Some (x) =>
                lexer.advance(),
            None =>
                return None
        }

        let procedure: Ref<ProcedureSyntax> = parse_procedure();
        match procedure {
            None => {
                return Err(Ref::new(_ep, parser_exception(String::copy(_ep, self.file_name), lexer.line, lexer.column))
            }

        let end: Position = self.lexer.get_position();

        let ret: MethodSyntax = Ref::new(_rp, MethodSyntax { start: start, end: end, procedure: procedure});

            procedure.parent = ret;

        ret
    }

    pub fn parse_staticfunction(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Ref<StaticFunction>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        let start: Position = self.lexer.get_previous_position();

        let procedure: Ref<FunctionSyntax> = parse_function();
        match procedure {
            None => {
                return Ok(None)
            }

        let end: Position = self.lexer.get_position();

        let ret: StaticFunctionSyntax = Ref::new(_rp, StaticFunctionSyntax { start: start, end: end, procedure: procedure});

            procedure.parent = ret;

        ret
    }

    pub fn parse_operator(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Ref<Operator>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        let start: Position = self.lexer.get_previous_position();

        let successOperator1: bool = self.lexer.parse_keyword("operator");
        match successOperator1 {
            Some (x) =>
                lexer.advance(),
            None =>
                return None
        }

        let routine: Ref<RoutineSyntax> = parse_routine();
        match routine {
            None => {
                return Err(Ref::new(_ep, parser_exception(String::copy(_ep, self.file_name), lexer.line, lexer.column))
            }

        let end: Position = self.lexer.get_position();

        let ret: OperatorSyntax = Ref::new(_rp, OperatorSyntax { start: start, end: end, routine: routine});

            routine.parent = ret;

        ret
    }

    pub fn parse_initializer(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Ref<Initializer>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        let start: Position = self.lexer.get_previous_position();

        let successInitializer1: bool = self.lexer.parse_keyword("initializer");
        match successInitializer1 {
            Some (x) =>
                lexer.advance(),
            None =>
                return None
        }

        let input: Option<Ref<StructureSyntax>> = parse_structure();

        let body: Ref<BlockSyntax> = parse_block();
        match body {
            None => {
                return Err(Ref::new(_ep, parser_exception(String::copy(_ep, self.file_name), lexer.line, lexer.column))
            }

        let end: Position = self.lexer.get_position();

        let ret: InitializerSyntax = Ref::new(_rp, InitializerSyntax { start: start, end: end, input: input, body: body});

        match input {
            Some(x) => 
                x.parent = ret;
            None => ()
        }

            body.parent = ret;

        ret
    }

    pub fn parse_allocator(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Ref<Allocator>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        let start: Position = self.lexer.get_previous_position();

        let successAllocator1: bool = self.lexer.parse_keyword("allocator");
        match successAllocator1 {
            Some (x) =>
                lexer.advance(),
            None =>
                return None
        }

        let input: Option<Ref<StructureSyntax>> = parse_structure();

        let body: Ref<BlockSyntax> = parse_block();
        match body {
            None => {
                return Err(Ref::new(_ep, parser_exception(String::copy(_ep, self.file_name), lexer.line, lexer.column))
            }

        let end: Position = self.lexer.get_position();

        let ret: AllocatorSyntax = Ref::new(_rp, AllocatorSyntax { start: start, end: end, input: input, body: body});

        match input {
            Some(x) => 
                x.parent = ret;
            None => ()
        }

            body.parent = ret;

        ret
    }

    pub fn parse_typeannotation(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Ref<TypeAnnotation>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        let start: Position = self.lexer.get_previous_position();

        let successColon1: bool = self.lexer.parse_punctuation(":");
        match successColon1 {
            Some (x) =>
                lexer.advance(),
            None =>
                return None
        }

        let typeSpec: Ref<TypeSpecSyntax> = parse_typeSpec();
        match typeSpec {
            None => {
                return Err(Ref::new(_ep, parser_exception(String::copy(_ep, self.file_name), lexer.line, lexer.column))
            }

        let end: Position = self.lexer.get_position();

        let ret: TypeAnnotationSyntax = Ref::new(_rp, TypeAnnotationSyntax { start: start, end: end, typeSpec: typeSpec});

            typeSpec.parent = ret;

        ret
    }

    pub fn parseTypeSpecList(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
    ) -> Option<Ref<Vector<Ref<TypeSpecSyntax>>>> {
        let _r = Region::create(_pr);
        let mut ret: Option<Ref<Array<Ref<TypeSpecSyntax>>>> = Option::None;
        loop {
            let node = self.parse_TypeSpec(&_r, _rp);
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
            Some(ret) => Some(Ref::new(_rp, Vector::from_array(_rp, ret))),
            None => None,
        }
    }

    pub fn parse_typespec(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Ref<TypeSpec>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        {
            let node = parseType(&_r, _rp);
            match node {
                OK(node_ok) => match node_ok {
                    Some(node_ret) => return node,
                    None => ()
                }
                Error(_) => return node,
            }
        }

        {
            let node = parseVariant(&_r, _rp);
            match node {
                OK(node_ok) => match node_ok {
                    Some(node_ret) => return node,
                    None => ()
                }
                Error(_) => return node,
            }
        }

        return None
    }

    pub fn parseTypeList(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
    ) -> Option<Ref<Vector<Ref<TypeSyntax>>>> {
        let _r = Region::create(_pr);
        let mut ret: Option<Ref<Array<Ref<TypeSyntax>>>> = Option::None;
        loop {
            let node = self.parse_Type(&_r, _rp);
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
            Some(ret) => Some(Ref::new(_rp, Vector::from_array(_rp, ret))),
            None => None,
        }
    }

    pub fn parse_type(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Ref<Type>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        let start: Position = self.lexer.get_previous_position();

        let name: Ref<NameSyntax> = parse_name();
        match name {
            None => {
                return Ok(None)
            }

        let generics: Option<Ref<GenericArgumentsSyntax>> = parse_genericArguments();

        let optional: Option<Ref<OptionalSyntax>> = parse_optional();

        let lifeTime: Option<Ref<LifeTimeSyntax>> = parse_lifeTime();

        let end: Position = self.lexer.get_position();

        let ret: TypeSyntax = Ref::new(_rp, TypeSyntax { start: start, end: end, name: name, generics: generics, optional: optional, lifeTime: lifeTime});

            name.parent = ret;

        match generics {
            Some(x) => 
                x.parent = ret;
            None => ()
        }

        match optional {
            Some(x) => 
                x.parent = ret;
            None => ()
        }

        match lifeTime {
            Some(x) => 
                x.parent = ret;
            None => ()
        }

        ret
    }

    pub fn parse_variant(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Ref<Variant>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        let start: Position = self.lexer.get_previous_position();

        let successLeftParen1: bool = self.lexer.parse_punctuation("(");
        match successLeftParen1 {
            Some (x) =>
                lexer.advance(),
            None =>
                return None
        }

        let types: Option<Ref<Vector<TypeSyntax>>> = parse_type_list();

        let successRightParen3: bool = self.lexer.parse_punctuation(")");
        match successRightParen3 {
            Some (x) =>
                lexer.advance(),
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

        let ret: VariantSyntax = Ref::new(_rp, VariantSyntax { start: start, end: end, types: types});

        match types {
            Some(x) => for item in x {
                item.parent = ret;
            }
            None => ()
        }

        ret
    }

    pub fn parse_throws(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Ref<Throws>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        let start: Position = self.lexer.get_previous_position();

        let successThrows1: bool = self.lexer.parse_keyword("throws");
        match successThrows1 {
            Some (x) =>
                lexer.advance(),
            None =>
                return None
        }

        let throwsType: Ref<TypeSpecSyntax> = parse_typeSpec();
        match throwsType {
            None => {
                return Err(Ref::new(_ep, parser_exception(String::copy(_ep, self.file_name), lexer.line, lexer.column))
            }

        let end: Position = self.lexer.get_position();

        let ret: ThrowsSyntax = Ref::new(_rp, ThrowsSyntax { start: start, end: end, throwsType: throwsType});

            throwsType.parent = ret;

        ret
    }

    pub fn parse_genericarguments(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Ref<GenericArguments>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        let start: Position = self.lexer.get_previous_position();

        let successLeftBracket1: bool = self.lexer.parse_punctuation("[");
        match successLeftBracket1 {
            Some (x) =>
                lexer.advance(),
            None =>
                return None
        }

        let generic: Ref<TypeSyntax> = parse_type();
        match generic {
            None => {
                return Err(Ref::new(_ep, parser_exception(String::copy(_ep, self.file_name), lexer.line, lexer.column))
            }

        let additionalGenerics: Option<Ref<Vector<GenericArgumentSyntax>>> = parse_genericArgument_list();

        let successRightBracket4: bool = self.lexer.parse_punctuation("]");
        match successRightBracket4 {
            Some (x) =>
                lexer.advance(),
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

        let ret: GenericArgumentsSyntax = Ref::new(_rp, GenericArgumentsSyntax { start: start, end: end, generic: generic, additionalGenerics: additionalGenerics});

            generic.parent = ret;

        match additionalGenerics {
            Some(x) => for item in x {
                item.parent = ret;
            }
            None => ()
        }

        ret
    }

    pub fn parseGenericArgumentList(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
    ) -> Option<Ref<Vector<Ref<GenericArgumentSyntax>>>> {
        let _r = Region::create(_pr);
        let mut ret: Option<Ref<Array<Ref<GenericArgumentSyntax>>>> = Option::None;
        loop {
            let node = self.parse_GenericArgument(&_r, _rp);
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
            Some(ret) => Some(Ref::new(_rp, Vector::from_array(_rp, ret))),
            None => None,
        }
    }

    pub fn parse_genericargument(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Ref<GenericArgument>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        let start: Position = self.lexer.get_previous_position();

        let successComma1: bool = self.lexer.parse_punctuation(",");
        match successComma1 {
            Some (x) =>
                lexer.advance(),
            None =>
                return None
        }

        let typeSpec: Ref<TypeSyntax> = parse_type();
        match typeSpec {
            None => {
                return Err(Ref::new(_ep, parser_exception(String::copy(_ep, self.file_name), lexer.line, lexer.column))
            }

        let end: Position = self.lexer.get_position();

        let ret: GenericArgumentSyntax = Ref::new(_rp, GenericArgumentSyntax { start: start, end: end, typeSpec: typeSpec});

            typeSpec.parent = ret;

        ret
    }

    pub fn parse_optional(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Ref<Optional>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        let start: Position = self.lexer.get_previous_position();

        let successQuestion1: bool = self.lexer.parse_punctuation("?");
        match successQuestion1 {
            Some (x) =>
                lexer.advance(),
            None =>
                return None
        }

        let end: Position = self.lexer.get_position();

        let ret: OptionalSyntax = Ref::new(_rp, OptionalSyntax { start: start, end: end});

        ret
    }

    pub fn parse_lifetime(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Ref<LifeTime>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        {
            let node = parseRoot(&_r, _rp);
            match node {
                OK(node_ok) => match node_ok {
                    Some(node_ret) => return node,
                    None => ()
                }
                Error(_) => return node,
            }
        }

        {
            let node = parseLocal(&_r, _rp);
            match node {
                OK(node_ok) => match node_ok {
                    Some(node_ret) => return node,
                    None => ()
                }
                Error(_) => return node,
            }
        }

        {
            let node = parseReference(&_r, _rp);
            match node {
                OK(node_ok) => match node_ok {
                    Some(node_ret) => return node,
                    None => ()
                }
                Error(_) => return node,
            }
        }

        {
            let node = parseThrown(&_r, _rp);
            match node {
                OK(node_ok) => match node_ok {
                    Some(node_ret) => return node,
                    None => ()
                }
                Error(_) => return node,
            }
        }

        return None
    }

    pub fn parse_root(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Ref<Root>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        let start: Position = self.lexer.get_previous_position();

        let successDollar1: bool = self.lexer.parse_punctuation("$");
        match successDollar1 {
            Some (x) =>
                lexer.advance(),
            None =>
                return None
        }

        let end: Position = self.lexer.get_position();

        let ret: RootSyntax = Ref::new(_rp, RootSyntax { start: start, end: end});

        ret
    }

    pub fn parse_local(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Ref<Local>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        let start: Position = self.lexer.get_previous_position();

        let successAt1: bool = self.lexer.parse_punctuation("@");
        match successAt1 {
            Some (x) =>
                lexer.advance(),
            None =>
                return None
        }

        let location: String = self.lexer.parse_identifier();
        match (location != null) && isIdentifier(location) {
            Some (x) =>
                lexer.advance(),
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

        let ret: LocalSyntax = Ref::new(_rp, LocalSyntax { start: start, end: end, location: location});

        ret
    }

    pub fn parse_reference(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Ref<Reference>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        let start: Position = self.lexer.get_previous_position();

        let successBacktick1: bool = self.lexer.parse_punctuation("`");
        match successBacktick1 {
            Some (x) =>
                lexer.advance(),
            None =>
                return None
        }

        let age: Literal = self.lexer.parse_literal();
        match age != null {
            Some (x) =>
                lexer.advance(),
            None =>
()        }

        let end: Position = self.lexer.get_position();

        let ret: ReferenceSyntax = Ref::new(_rp, ReferenceSyntax { start: start, end: end, age: age});

        ret
    }

    pub fn parse_thrown(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<Ref<Thrown>, Ref<ParserError>> {
        let _r = Region::create(_pr);

        let start: Position = self.lexer.get_previous_position();

        let successHash1: bool = self.lexer.parse_punctuation("#");
        match successHash1 {
            Some (x) =>
                lexer.advance(),
            None =>
                return None
        }

        let end: Position = self.lexer.get_position();

        let ret: ThrownSyntax = Ref::new(_rp, ThrownSyntax { start: start, end: end});

        ret
    }

    fn is_at_end() -> bool {
        self.lexer.is_at_end()
    }

    fn is_identifier(id: Ref<String>) {
        if keywords.contains(id) {
            false
        }
        else {
            true;
        }
    }
}
