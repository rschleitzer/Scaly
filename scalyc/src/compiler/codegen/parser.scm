(define (parser) ($
"
pub struct ParserError {
    pub file_name: String,
    pub line: usize,
    pub column: usize,
}

pub struct Parser<'a> {
    lexer: Lexer<'a>,
    _keywords: HashSet<""String>,
}

impl<'a> Parser<'a> {
    pub fn new(deck: &'a str) -> Parser {
        let mut keywords = HashSet::new();
"   (apply-to-selected-children "keyword" (lambda (keyword) ($
"        keywords.insert(String::from(\""(id keyword)"\"));
"   )))
"        Parser {
            lexer: Lexer::new(deck),
            _keywords: keywords,
        }
    }
"
    (apply-to-selected-children "syntax" (lambda (syntax) (if (program? syntax) "" ($
        (if (multiple? syntax) ($
"
    pub fn parse_"(downcase-string (id syntax))"_list(&""mut self) -> Result<""Option<""Vec<"(id syntax)"Syntax>>, ParserError> {
        let mut array: Option<""Vec<"(id syntax)"Syntax>> = Option::None;
        loop {
            let node = self.parse_"(downcase-string (id syntax))"()?;
            if let Some(node) = node {
                if let None = array {
                    array = Some(Vec::new())
                };
                match &""mut array {
                    Some(a) => a.push(node),
                    None => (),
                }
            } else {
                break;
            }
        }

        Ok(array)
    }
"       )"")
"
    pub fn parse_"(downcase-string (id syntax))"(&""mut self) -> Result<""Option<"(id syntax)"Syntax>, ParserError> {
"
        (if (abstract? syntax)
            ($
                (apply-to-children-of syntax (lambda (content) ($
"        {
            let node = self.parse_"(downcase-string (link content))"()?;
            if let Some(node) = node {
                return Ok(Some("(id syntax)"Syntax::"(link content)"(node)));
            }
        }
"
                )))
"        return Ok(None)
"
            )
            ($ ; non-abstract syntax
                (if (top? syntax) ""
"        let start: Position = self.lexer.get_previous_position();
"               )
                (apply-to-children-of syntax (lambda (content) ($
                   (if (string=? "syntax" (type content))
                        ($ ; non-terminals
"
        let "(property content)" = self.parse_"(downcase-string (link content))(if (multiple? content) "_list" "")"()?;
"
                            (if (optional? content) "" ($
"        if let None = "(property content)" {
"                               (if (equal? 1 (child-number content))
                                    ($
"            return Ok(None);
"                                   )
                                    ($
"            return Err(ParserError {
                file_name: \"\".to_string(),
                line: self.lexer.line,
                column: self.lexer.column,
            });
"                                   )
                                )
"        }
"                           ))
                            (if (and (top? syntax) (last-sibling? content)) ($
"        if let Some(_) = "(property content)" {
            if !self.is_at_end() {
                let error_pos = self.lexer.get_previous_position();
                return Result::Err(
                    ParserError {
                        file_name: \"\".to_string(),
                        line: error_pos.line,
                        column: error_pos.column,
                    },
                )
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
                                (("keyword")     ($ "\""(link content)"\".to_string()"))
                                (("punctuation") ($ "\""(value (element-with-id (link content)))"\".to_string()"))
                                (else "")
                            )");
"
                            (let
                                ((null-handler
                                    (if (optional? content) 
"            ()
"
                                        ($
                                            (if (equal? 1 (child-number content))
                                                ($
"
                return Ok(None)
"                                               )
                                                ($
"
            return Result::Err(
                ParserError {
                    file_name: \"\".to_string(),
                    line: self.lexer.line,
                    column: self.lexer.column,
                },
            )"
                                                )
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
"        match &"(property content)" {
            Some("(property content)") =>
                if !self.is_identifier("(property content)") {"
                                        null-handler
"
           },
           _ =>"
                                        null-handler
            ",
        }
"                                   ))
                                    (else ($ 
"        if let None = "(property content)" {
"
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
        let ret = "(id syntax)"Syntax {
"
                (if (top? syntax) 
"            file_name: \"\".to_string()"
"            start: start,
            end: end"
                )
                (apply-to-property-children-of syntax (lambda (content) ($
                    ",
            "(property content)": "(property content)
                    (if (optional? content)
                        ""
                        ".unwrap()"
                    )
                )))
                ",
        };

        Ok(Some(ret))
"
            ) ; $
        ) ; abstract or not
"    }
"   ))))
"
    pub fn is_at_end(&""mut self) -> bool {
        self.lexer.is_at_end()
    }

    pub fn get_current_line(&""self) -> usize {
        self.lexer.get_position().line
    }

    pub fn get_current_column(&""self) -> usize {
        self.lexer.get_position().column
    }

    pub fn get_previous_line(&""self) -> usize {
        self.lexer.get_previous_position().line
    }

    pub fn get_previous_column(&""self) -> usize {
        self.lexer.get_previous_position().column
    }
    fn is_identifier(&""self, id: &""String) -> bool {
        if self._keywords.contains(id) {
            false
        } else {
            true
        }
    }
}
"

))
