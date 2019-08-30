(define (parser-rs) ($
"use scaly::containers::{Array, HashSet, Ref, String, Vector};
use scaly::io::Stream;
use scaly::memory::Region;
use scaly::Page;
use scalyc::errors::ParserError;
use scalyc::lexer::{Lexer, Literal, Position};

pub struct Parser {
    lexer: Ref<""Lexer>,
    file_name: String,
    keywords: Ref<""HashSet<""String>>,
}

impl Parser {
    pub fn new(_pr: &""Region, _rp: *mut Page, file_name: String, stream: *mut Stream) -> Parser {
        let _r = Region::create(_pr);
        let keywords = HashSet::from_vector(
            &""_r,
            _rp,
            Ref::new(
                _rp,
                Vector::from_raw_array(
                    _rp,
                    &[
"   (apply-to-selected-children "keyword" (lambda (keyword) ($
"                        String::from_string_slice(_rp, \""(id keyword)"\"),
"   )))
"                    ],
                ),
            ),
        );
        Parser {
            lexer: Lexer::new(&""_r,_rp, stream),
            file_name: file_name,
            keywords: keywords,
        }
    }
"
    (apply-to-selected-children "syntax" (lambda (syntax) (if (program? syntax) "" ($
        (if (multiple? syntax) ($
"
    pub fn parse_"(downcase-string (id syntax))"_list(
        &""mut self,
        _pr: &""Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<""Option<""Ref<""Vector<""Ref<"(id syntax)"Syntax>>>>, Ref<""ParserError>> {
        let _r = Region::create(_pr);
        let mut ret: Option<""Ref<""Array<""Ref<"(id syntax)"Syntax>>>> = Option::None;
        loop {
            let node = self.parse_"(downcase-string (id syntax))"(&""_r, _rp, _ep)?;
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
"       )"")
"
    pub fn parse_"(downcase-string (id syntax))"(
        &""mut self,
        _pr: &""Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<""Option<""Ref<"(id syntax)"Syntax>>, Ref<""ParserError>> {
        let _r = Region::create(_pr);
"
        (if (abstract? syntax)
            ($
                (apply-to-children-of syntax (lambda (content) ($
"
        {
            let node = self.parse_"(downcase-string (link content))"(&""_r, _rp, _ep)?;
            if let Some(node) = node {
                return Ok(Some(Ref::new(_rp, "(id syntax)"Syntax::"(link content)"(*node))))
            }
        }
"
                )))
"        return Ok(None)
"
            )
            ($ ; non-abstract syntax
                (if (top? syntax) ""
"
        let start: Position = self.lexer.get_previous_position();
"               )
                (apply-to-children-of syntax (lambda (content) ($
                   (if (string=? "syntax" (type content))
                        ($ ; non-terminals
"
        let "(property content)" = self.parse_"(downcase-string (link content))(if (multiple? content) "_list" "")"(&""_r, _rp, _ep)?;
"
                            (if (optional? content) "" ($
"        if let None = "(property content)" {
"                               (if (equal? 1 (child-number content))
                                    ($
"            return Ok(None)
"                                   )
                                    ($
"            return Err(Ref::new(_ep, ParserError { file_name: String::copy(_ep, self.file_name), line: self.lexer.line, column: self.lexer.column }))
"                                   )
                                )
"        }
"                           ))
                            (if (and (top? syntax) (last-sibling? content)) ($
"        if let Some(_) = "(property content)" {
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
"                           )"")
                        )
                        ($ ; terminals
"
        let "
                            (case (type content)
                                (("keyword" "punctuation") ($ "success_"(if (property content) (property content) ($ (link content)"_"(number->string (child-number content))))))
                                (else (property content))
                            )
            " = self.lexer.parse_"(type content)"("
                            (case (type content)
                                (("keyword")     ($ "&""_r, String::from_string_slice(_r.page, \""(link content)"\")"))
                                (("punctuation") ($ "&""_r, String::from_string_slice(_r.page, \""(value (element-with-id (link content)))"\")"))
                                (else ($"&""_r, _rp")))");
"
                            (let
                                ((null-handler
                                    (if (optional? content) 
"
            ()
"
                                        ($
                                            (if (equal? 1 (child-number content))
                                                ($
"            return Ok(None)
"                                               )
                                                ($
"
            return Result::Err(Ref::new(
                _ep,
                ParserError {
                    file_name: self.file_name,
                    line: self.lexer.line,
                    column: self.lexer.column,
                },
            ))
"                                              )
                                            )
                                        )
                                    )
                                ))
                                (case (type content)
                                    (("keyword" "punctuation") ($ 
"        if !success_"(if (property content) (property content) ($ (link content)"_"(number->string (child-number content))))" {
"                                       null-handler
"        }
"
                                    ))
                                    (("identifier") ($ 
"        if let Some("(property content)") = "(property content)" {
            if !self.is_identifier("(property content)") {
"                   null-handler
"           }
        } else {"
                                        null-handler
"        }
"                                   ))
                                    (else ($ 
"        if let None = "(property content)" {"
                                        null-handler
"        }
"                                   ))
                                )
                            )
                        ) ; terminal
                    ) ; syntax or terminal
                ))) ; apply to children of syntax

                (if (top? syntax) ""
"
        let end: Position = self.lexer.get_position();
"               )
"
        let ret = Ref::new(_rp, "(id syntax)"Syntax { "(if (top? syntax) "file_name: self.file_name" "parent: None, start: start, end: end")
                (apply-to-property-children-of syntax (lambda (content) ($
                    ", "(property content)": "(property content)(if (optional? content) "" ".unwrap()")
                )))
                " });
"                (apply-to-property-children-of syntax (lambda (content)
                    (if (multiple? content)
                        ($
"
        if let Some(mut "(property content)") = "(property content)" {
            for item in "(property content)".iter_mut() {
"                           (if (abstract? (element-with-id (link content)))
                                ($
"                match **item {
"                                   (apply-to-children-of (element-with-id (link content)) (lambda (variant) ($
"                    "(link content)"Syntax::"(link variant)"(mut y) => y.parent = Some(ParentNode::"(id syntax)"(ret)),
"                                   )))
"                }
"                               )
                                ($
"                item.parent = Some(ParentNode::"(id syntax)"(ret));
"                               )
                            )
"            }
        }
"                       )
                        (if (string=? "syntax" (type content)) ($
                            (if (optional? content)
                                ($
"
        if let Some("(if (abstract? (element-with-id (link content))) "" "mut ")"x) = "(property content)" {
"                                   (if (abstract? (element-with-id (link content)))
                                        ($
"            match *x {
"                                   (apply-to-children-of (element-with-id (link content)) (lambda (variant) ($
"                "(link content)"Syntax::"(link variant)"(mut y) => y.parent = Some(ParentNode::"(id syntax)"(ret)),
"                                   )))
"            }
"                                        )
                                        ($
"            x.parent = Some(ParentNode::"(id syntax)"(ret))
"                                       )
                                    )
"        }
"                               )
                                (if (abstract? (element-with-id (link content)))
                                    ($
"
        match *"(property content)".unwrap() {
"                                       (apply-to-children-of (element-with-id (link content)) (lambda (variant) ($
"            "(link content)"Syntax::"(link variant)"(mut x) => x.parent = Some(ParentNode::"(id syntax)"(ret)),
"                                       )))
"        }
"                                    )
                                    ($
"
        "(property content)".unwrap().parent = Some(ParentNode::"(id syntax)"(ret));
"                                   )
                                )
                            )
                        )"")
                    )
                ))
"
        Ok(Some(ret))
"
            ) ; $
        ) ; abstract or not
"    }
"   ))))
"
    fn is_at_end(&""self) -> bool {
        self.lexer.is_at_end()
    }

    fn is_identifier(&""self, id: String) -> bool {
        if self.keywords.contains(id) {
            false
        } else {
            true
        }
    }
}
"

))

(define (parser) ($
"namespace scalyc {

class Parser(

    fileName: string,
    text: string

    ) {

    mutable lexer: Lexer new Lexer(text)

"   (apply-to-selected-children "keyword" (lambda (keyword) ($
"    let "(name keyword)": string = new string(\""(id keyword)"\")
"   )))
    (apply-to-selected-children "punctuation" (lambda (punctuation) ($
"    let "(id punctuation)": string = new string(\""(value punctuation)"\")
"   )))

    (apply-to-selected-children "syntax" (lambda (syntax) (if (program? syntax) "" ($
        (if (multiple? syntax) ($
"
    method parse"(id syntax)"List(): Array["(id syntax)"] {
        mutable ret: Array["(id syntax)"]$ = null
        while (true) {

            let node: "(id syntax)" parse"(id syntax)"()
            if (node = ()) {
                break }

            if (ret = ()) {
                ret = new Array["(id syntax)"]() }

            ret.add(node)
        }

        ret
    }
"       )"")
"
    function parse"(id syntax)"(): "(id syntax)(if (top? syntax) " throws ParserError" "")" {"
        (if (abstract? syntax)
            ($
                (apply-to-children-of syntax (lambda (content) ($
"
        {
            let node: "(link content)" = parse"(link content)"()
            if (node <""> ()) {
                return node }
        }
"
                )))
"
        null
"
            )
            ($ ; non-abstract syntax
"
        let start: Position$ = lexer.getPreviousPosition()
"                (apply-to-children-of syntax (lambda (content) ($
                   (if (string=? "syntax" (type content))
                        ($ ; non-terminals
"
        let "(property content)": "(if (multiple? content) "Array[" "")(link content)(if (multiple? content) "]" "")" = parse"(link content)(if (multiple? content) "List" "")"()
"
                    (if (or (optional? content) (multiple? content)) "" ($
"        if ("(property content)" = ()) {
            return () }
"
                    ))
                           (if (top? syntax) ($
"        if ("(property content)" <""> null) {
            if (isAtEnd() = false) {
                let errorPos: Position$ = lexer.getPreviousPosition()
                throw syntax(errorPos.line, errorPos.column)
            }
        }
"                           )"")
                        )
                        ($ ; terminals
"
        let "(case (type content) (("keyword" "punctuation") ($ "success_"(if (property content) (property content) ($ (link content)"_"(number->string (child-number content))))": bool"))(("literal") ($ (property content)": Literal"))(else ($ (property content)": string")))
            " = lexer.parse"(case (type content)(("identifier") "Identifier")(("literal") "Literal")(("keyword") "Keyword")(("punctuation") "Punctuation"))
            "("(case (type content)(("keyword") (name-of-link content)) (("punctuation") (link content)) (else ""))")
        if ("(case (type content) (("keyword" "punctuation") ($ "success_"(if (property content) (property content) ($ (link content)"_"(number->string (child-number content))))))(("identifier") ($ "("(property content)" <""> ()) && isIdentifier("(property content)")")) (else ($ (property content)" <""> ()")))") {
            lexer.advance() }
"                           (if (optional? content) "" ($
"        else {
            return () }
"
                           ))
                        )
                    ) ; syntax or terminal
                ))) ; apply to children of syntax
"
        let end: Position$ lexer.getPosition()

        let ret: "(id syntax)" new "(id syntax)"("
                (apply-to-property-children-of syntax (lambda (content) ($
                    (property content)(if (properties-remaining? content syntax) ", " "")
                )))
                (if (node-list-empty? (properties syntax)) "" ", ")
                "new Position(start), new Position(end))
"                (apply-to-property-children-of syntax (lambda (content)
                    (if (multiple? content)
                        ($
"        if ("(property content)" <""> ()) {
            for (item: "(link content)" in "(property content)") {
                item.parent = ret }
        }
"                       )
                        (if (string=? "syntax" (type content)) ($
                            (if (optional? content) ($
"        if ("(property content)" <""> ()) {
    "                       )"")
"        "(property content)".parent = ret"(if (optional? content) " }" "")"
"                       )"")
                    )
                ))
"
        ret
"
            ) ; $
        ) ; abstract or not
"    }
"   ))))
"
    function isAtEnd(): bool {
        lexer.isAtEnd()
    }

    function isIdentifier(id: string): bool {"
   (apply-to-selected-children "keyword" (lambda (keyword) ($
"
        if (id.equals("(name keyword)")) {
            return false }
"   )))
"
        true
    }
}
"

))
