(define (syntax-rs) ($

"
pub trait SyntaxNode {
    fn accept(visitor: Ref<""SyntaxVisitor>);
}
"
    (apply-to-selected-children "syntax" (lambda (syntax-node) ($
"
pub struct "(id syntax-node)"Syntax {"
        (if (top? syntax-node)
"
    pub file_name: String;"
"
    pub start: Position;
    pub end: Position;
    pub parent: SyntaxNode;"
        )
        (apply-to-children-of syntax-node (lambda (content) ($
            (if (property content) ($
"
    pub "(property content)": "
                (case (type content)
                    (("syntax") ($ "Ref<"(if (multiple? content)"Vector<" "")(link content)"Syntax"(if (multiple? content)">" "")">"))
                    (("identifier") "string")
                    (("literal") "Literal")
                    (("keyword" "punctuation") "bool")
                )
                ";"
          )"")
        )))
"
}

impl SyntaxNode for "(id syntax-node)"Syntax {
    accept(visitor: Ref<""SyntaxVisitor>) {
"       (if (abstract? syntax-node) "" ($
            (if (has-syntax-children? syntax-node)
                ($
"        if !visitor.open"(id syntax-node)"(this) {
            return
        }"
                    (apply-to-children-of syntax-node (lambda (content)
                        (case (type content)
                            (("syntax") ($
                                (if (abstract? syntax-node) "" ($
                                    (if (multiple? content)
                                        ($
"
        match self."(property content)" {
            Some(x) => for node in "(property content)" {
                node.accept(visitor)
            },
            None => ()
        };
"                                       )
                                        (if (optional? content)
                                            ($
"
        match self."(property content)" {
            Some(x) => "(property content)".accept(visitor),
            None => ()
        };
"                                           )
                                            ($
"
        "(property content)".accept(visitor);
"                                            )
                                        )
                                    )
                                    (if (and (optional? content) (not (multiple? content))) ($
"        match self."(property content)" {
            Some(x) =>
"
                                    )"")
                                ))
                            ))
                            (else "")
                        )
                    ))
"        visitor.close"(id syntax-node)"(this)
"
                )
                ($
"        visitor.visit"(id syntax-node)"(this)
"
                )
            )
       ))
"    }
}
"   )))
))

(define (syntax) ($

"
class SyntaxNode (

    start: Position,
    end: Position

    ) {

    var parent: SyntaxNode? ()
}
"
    (apply-to-selected-children "syntax" (lambda (syntax-node) ($
"
class "(id syntax-node)" extends "(if (base syntax-node) (base syntax-node) "SyntaxNode")
        (if (abstract? syntax-node) "" ($
" (

"           (apply-to-children-of syntax-node (lambda (content) ($
                (if (property content) ($
"    "(property content)": "
                    (case (type content)
                        (("syntax") ($ (if (multiple? content)"Array[" "")(link content)(if (multiple? content)"]" "")))
                        (("identifier") "string")
                        (("literal") "Literal")
                        (("keyword" "punctuation") "bool")
                    )
                    (if (last-sibling? content) "" ", ")
                    "
"
                )"")
            )))
"
    )"       
        ))
" {
    function accept(visitor: Visitor) {
"       (if (abstract? syntax-node) "" ($
            (if (has-syntax-children? syntax-node)
                ($
"
        if (visitor.open"(id syntax-node)"(this) = false) {
            return }
"
                    (apply-to-children-of syntax-node (lambda (content)
                        (case (type content)
                            (("syntax") ($
                                (if (abstract? syntax-node) "" ($
                                    (if (and (optional? content) (not (multiple? content))) ($
"
        if ("(property content)" <""> ()) {
"
                                    )"")
                                    (if (multiple? content)
                                        ($
"
        if ("(property content)" <""> ()) {
            for (node: "(link content)" in "(property content)") {
                node.accept(visitor) } }
"
                                        )
                                        ($
(if (optional? content) "    " "")"        "(property content)".accept(visitor)"(if (optional? content) " }" "")"
"
                                        )
                                    )
                                ))
                            ))
                            (else "")
                        )
                    ))
"
        visitor.close"(id syntax-node)"(this)
"
                )
                ($
"
        visitor.visit"(id syntax-node)"(this)
"
                )
            )
       ))
"    }
}
"   )))
"
} // namespace scalyc
"))
