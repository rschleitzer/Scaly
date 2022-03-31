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
    ) -> Result<Ref<FileSyntax>, Ref<ParserError>> {
        let _r = Region::create(_pr);
        let start: Position = self.lexer.get_previous_position();

        // IntrinsicSyntax[] intrinsics = parseIntrinsicList();

        // UsingSyntax[] usings = parseUsingList();

        // DefineSyntax[] defines = parseDefineList();

        // DeclarationSyntax[] declarations = parseDeclarationList();

        let statements = self.parse_statement_list(&_r, _rp);
        match statements {
            Some(_) => {
                if !self.is_at_end() {
                    let error_pos = self.lexer.get_previous_position();
                    return Result::Err(Ref::new(
                        _ep,
                        ParserError {
                            file_name: self.file_name,
                            line: error_pos.line,
                            column: error_pos.column,
                        },
                    ));
                }
            }
            None => (),
        }

        let end: Position = self.lexer.get_position();

        let ret: Ref<FileSyntax> = Ref::new(
            _rp,
            FileSyntax {
                start: start,
                end: end,
            },
        );

        // if (intrinsics != null)
        // {
        //     foreach (IntrinsicSyntax item in intrinsics)
        //         item.parent = ret;
        // }
        // if (usings != null)
        // {
        //     foreach (UsingSyntax item in usings)
        //         item.parent = ret;
        // }
        // if (defines != null)
        // {
        //     foreach (DefineSyntax item in defines)
        //         item.parent = ret;
        // }
        // if (declarations != null)
        // {
        //     foreach (DeclarationSyntax item in declarations)
        //         item.parent = ret;
        // }
        // if (statements != null)
        // {
        //     foreach (StatementSyntax item in statements)
        //         item.parent = ret;
        // }

        Ok(ret)
    }

    pub fn parse_statement_list(
        &mut self,
        _pr: &Region,
        _rp: *mut Page,
    ) -> Option<Ref<Vector<Ref<StatementSyntax>>>> {
        let _r = Region::create(_pr);
        let mut ret: Option<Ref<Array<Ref<StatementSyntax>>>> = Option::None;
        loop {
            let node = self.parse_statement(&_r, _rp);
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
    ) -> Option<Ref<StatementSyntax>> {
        let _r = Region::create(_pr);
        let start: Position = self.lexer.get_previous_position();
        let end: Position = self.lexer.get_position();
        let ret: Ref<StatementSyntax> = Ref::new(
            _rp,
            StatementSyntax {
                start: start,
                end: end,
            },
        );
        Some(ret)
    }

    fn is_at_end(&self) -> bool {
        self.lexer.is_at_end()
    }

    fn _is_identifier(&self, id: String) -> bool {
        if self._keywords.contains(id) {
            false
        } else {
            true
        }
    }
}

#[derive(Copy, Clone)]
pub struct FileSyntax {
    pub start: Position,
    pub end: Position,
}

#[derive(Copy, Clone)]
pub struct StatementSyntax {
    pub start: Position,
    pub end: Position,
}
