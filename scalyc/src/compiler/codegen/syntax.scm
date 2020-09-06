(define (syntax) ($

"
pub trait SyntaxNode {
    fn accept(&""mut self, _pr: &""Region, visitor: *mut SyntaxVisitor);
}

#[derive(Copy, Clone)]
pub enum ParentNode {
"    (apply-to-nodelist (node-list-filter can-be-parent-syntax? (select-elements (children (current-node)) "syntax")) (lambda (syntax-node) ($
"    "(id syntax-node)"(Ref<"(id syntax-node)"Syntax>),
"   )))
"}
"
    (apply-to-selected-children "syntax" (lambda (syntax-node) ($
"
#[derive(Copy, Clone)]
pub "(if (abstract? syntax-node) "enum" "struct")" "(id syntax-node)"Syntax {"
        (if (abstract? syntax-node)
            (apply-to-children-of syntax-node (lambda (content) ($
"
   "(link content)"("(link content)"Syntax),"
            )))
            ($
                (if (top? syntax-node)
                    ($
"
    pub file_name: String,"
                    )
                    (if (program? syntax-node) "" ($
"
    pub start: Position,
    pub end: Position,
    pub parent: Option<""ParentNode>,"
                    ))
                )

                (apply-to-children-of syntax-node (lambda (content) ($
                    (if (property content) ($
"
    pub "(property content)": "
                (case (type content)
                    (("syntax") ($ (if (optional? content)"Option<" "")"Ref<"(if (multiple? content) ($ "Vector<""Ref<") "")(link content)"Syntax>"(if (multiple? content)">>" "")(if (optional? content)">" "")))
                    (("identifier") ($ (if (optional? content)"Option<" "")"String"(if (optional? content)">" "")))
                    (("literal") ($ (if (optional? content)"Option<" "")"Literal"(if (optional? content)">" "")))
                    (("keyword" "punctuation") "bool")
                )
                ","
                    )"")
                )))
            )
        )
"
}

impl SyntaxNode for "(id syntax-node)"Syntax {
    fn accept(&""mut self, _pr: &""Region, visitor: *mut SyntaxVisitor) {
"       (if (abstract? syntax-node)
            ($
"        match self {"
                (apply-to-children-of syntax-node (lambda (content) ($
"
            "(id syntax-node)"Syntax::"(link content)"(syntax) => syntax.accept(_pr, visitor),"
                )))

"
        }
"           )
            ($
                (if (has-syntax-children? syntax-node)
                    ($
"        unsafe {
            if !(*visitor).open_"(downcase-string (id syntax-node))"(_pr, Ref::from(self as *mut "(id syntax-node)"Syntax)) {
                return
            }
        }"
                        (apply-to-children-of syntax-node (lambda (content)
                            (case (type content)
                                (("syntax") ($
                                    (if (abstract? syntax-node) "" ($
                                        (if (multiple? content)
                                            ($
                                                (if (optional? content)
                                                    ($
"
        if let Some(mut "(property content)") = self."(property content)" {
            for node in "(property content)".iter_mut() {
                node.accept(_pr, visitor)
            };
        };
"                                                   )
                                                    ($
"
        for node in self."(property content)".iter_mut() {
            node.accept(_pr, visitor)
        };
"                                                   )
                                                )
                                            )
                                            (if (optional? content)
                                                ($
"
        if let Some(mut "(property content)") = self."(property content)" {
            "(property content)".accept(_pr, visitor);
        };
"                                               )
                                                ($
"
        self."(property content)".accept(_pr, visitor);
    "                                            )
                                            )
                                        )
                                    ))
                                ))
                                (else "")
                            )
                        ))
"        unsafe {
            (*visitor).close_"(downcase-string (id syntax-node))"(_pr, Ref::from(self as *mut "(id syntax-node)"Syntax))
        }
"
                    )
                    ($
"        unsafe {
            (*visitor).visit_"(downcase-string (id syntax-node))"(_pr, Ref::from(self as *mut "(id syntax-node)"Syntax))
        }
"
                    )
                )
            )
        )
"    }
}
"   )))
))

(define (can-be-parent-syntax? syntax)
    (and
        (not (abstract? syntax))
        (not
            (node-list-empty?
                (node-list-filter
                    (lambda (node)
                        (and
                            (link node) 
                            (let ((target (element-with-id (link node)))) 
                                (and 
                                    (string=? "syntax" (gi target))
                                    (not (top? target))
                                )
                            )
                        )
                    )
                    (children syntax)
                )
            )
        )
    )
)