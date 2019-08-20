(define (parser-rs) ($
"use scaly::containers::{Array, HashSet, Ref, String, Vector};
use scaly::io::Stream;
use scaly::memory::Region;
use scaly::Page;
use scalyc::errors::ParserError;
use scalyc::lexer::Lexer;
use scalyc::lexer::Position;

pub struct Parser {
    lexer: Ref<""Lexer>,
    file_name: String,
    _keywords: Ref<""HashSet<""String>>,
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
            _keywords: keywords,
        }
    }
"
    (apply-to-selected-children "syntax" (lambda (syntax) (if (program? syntax) "" ($
        (if (multiple? syntax) ($
"
    pub fn parse"(id syntax)"List(
        &""mut self,
        _pr: &""Region,
        _rp: *mut Page,
    ) -> Option<""Ref<""Vector<""Ref"(id syntax)"Syntax>>>> {
        let _r = Region::create(_pr);
        let mut ret: Option<""Ref<""Array<""Ref<"(id syntax)"Syntax>>>> = Option::None;
        loop {
            let node = self.parse_"(id syntax)"(&""_r, _rp);
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

            if (ret == null)
                ret = new List<"(id syntax)"Syntax>();

            ret.Add(node);
        }

        match ret {
            Some(ret) => Some(Ref::new(_rp, Vector::from_array(_rp, ret))),
            None => None,
        }
    }
"       )"")
"
    pub fn parse_"(downcase-string (id syntax))"(
        &""mut self,
        _pr: &""Region,
        _rp: *mut Page,
        _ep: *mut Page,
    ) -> Result<""Ref<"(id syntax)">, Ref<""ParserError>> {
        let _r = Region::create(_pr);
        let start: Position = self.lexer.get_previous_position();
"
        (if (abstract? syntax)
            ($
                (apply-to-children-of syntax (lambda (content) ($
"
        {
            let node = parse"(link content)"(&""_r, _rp);
            match node {
                OK(node_ok) => match node_ok {
                    Some(node_ret) => return node,
                    None => ()
                }
                Error(_) => return node,
            }
        }
"
                )))
"
        return None
"
            )
            ($ ; non-abstract syntax
"
            Position start = lexer.getPreviousPosition();
"                (apply-to-children-of syntax (lambda (content) ($
                   (if (string=? "syntax" (type content))
                        ($ ; non-terminals
"
            "(link content)"Syntax"(if (multiple? content) "[]" "")" "(property content)" = parse"(link content)(if (multiple? content) "List" "")"();
"
                    (if (optional? content) "" ($
"            if ("(property content)" == null)
"
                        (if (equal? 1 (child-number content))
                            ($
"                return null;
"                           )
                            ($
"                throw new ParserException(fileName, lexer.line, lexer.column);
"                           )
                        )
                    ))
                           (if (and (top? syntax) (last-sibling? content)) ($
"            if ("(property content)" != null)
            {
                if (!isAtEnd())
                {
                    Position errorPos = lexer.getPreviousPosition();
                    throw new ParserException(fileName, errorPos.line, errorPos.column);
                }
            }
"                           )"")
                        )
                        ($ ; terminals
"
            "(case (type content)
                (("keyword" "punctuation") ($ "bool success"(if (property content) (string-firstchar-upcase (property content)) ($ (string-firstchar-upcase (link content))(number->string (child-number content))))))
                (("literal") ($ "Literal "(property content)))
                (else ($ "string "(property content)))
            )
            " = lexer.parse"
            (case (type content)(("identifier") "Identifier")(("literal") "Literal")(("keyword") "Keyword")(("punctuation") "Punctuation"))
            "("(case (type content)(("keyword") ($ "\""(link content)"\"")) (("punctuation") ($ "\""(value (element-with-id (link content)))"\"")) (else ""))");
            if ("(case (type content) (("keyword" "punctuation") ($ "success"(if (property content) (string-firstchar-upcase (property content)) ($ (string-firstchar-upcase (link content))(number->string (child-number content))))))(("identifier") ($ "("(property content)" != null) && isIdentifier("(property content)")")) (else ($ (property content)" != null")))")
                lexer.advance();
"                           (if (optional? content) "" ($
"            else
"                               (if (equal? 1 (child-number content))
                                    ($
"                return null;
"                                   )
                                    ($
"                throw new ParserException(fileName, lexer.line, lexer.column);
"                                    )
                                )
                           ))
                        )
                    ) ; syntax or terminal
                ))) ; apply to children of syntax
"
            Position end = lexer.getPosition();

            "(id syntax)"Syntax ret = new "(id syntax)"Syntax(start, end"
                (apply-to-property-children-of syntax (lambda (content) ($
                    ", "(property content)
                )))
                ");

"                (apply-to-property-children-of syntax (lambda (content)
                    (if (multiple? content)
                        ($
"            if ("(property content)" != null)
            {
                foreach ("(link content)"Syntax item in "(property content)")
                    item.parent = ret;
            }
"                       )
                        (if (string=? "syntax" (type content)) ($
                            (if (optional? content) ($
"            if ("(property content)" != null)
    "                       )"")
"            "(property content)".parent = ret;
"                       )"")
                    )
                ))
"
            return ret;
"
            ) ; $
        ) ; abstract or not
"        }
"   ))))
"
        bool isAtEnd()
        {
            return lexer.isAtEnd();
        }

        bool isIdentifier(string id)
        {
            if (keywords.Contains(id))
                return false;

            return true;
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
        let "(case (type content) (("keyword" "punctuation") ($ "success"(if (property content) (string-firstchar-upcase (property content)) ($ (string-firstchar-upcase (link content))(number->string (child-number content))))": bool"))(("literal") ($ (property content)": Literal"))(else ($ (property content)": string")))
            " = lexer.parse"(case (type content)(("identifier") "Identifier")(("literal") "Literal")(("keyword") "Keyword")(("punctuation") "Punctuation"))
            "("(case (type content)(("keyword") (name-of-link content)) (("punctuation") (link content)) (else ""))")
        if ("(case (type content) (("keyword" "punctuation") ($ "success"(if (property content) (string-firstchar-upcase (property content)) ($ (string-firstchar-upcase (link content))(number->string (child-number content))))))(("identifier") ($ "("(property content)" <""> ()) && isIdentifier("(property content)")")) (else ($ (property content)" <""> ()")))") {
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
