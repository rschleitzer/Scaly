(define (syntax) ($

"class SyntaxNode {
    let start: Position
    let end: Position
    var parent: SyntaxNode
}
"
    (apply-to-selected-children "syntax" (lambda (syntax-node) ($
"
class "(id syntax-node)" : "(if (base syntax-node) (base syntax-node) "SyntaxNode")" {
"       (if (abstract? syntax-node) "" ($
"
    constructor("(scaly-syntax-constructor-parameters syntax-node)") {
"     (if (program? syntax-node)
"        start = new Position(0, 0)
        end = new Position(0, 0)
        this.name = name
        this.directory = directory
        this.compilationUnits = compilationUnits
        this.parent = null
"
            (scaly-constructor-initializers syntax-node)
        )
"    }
"       ))
"
    "(if (base syntax-node) "override " "")"function accept(mutable visitor: SyntaxVisitor) {
"       (if (abstract? syntax-node) "" ($
            (if (has-syntax-children? syntax-node)
                ($
"        if !visitor.open"(id syntax-node)"(this)
            return;
"
                    (apply-to-children-of syntax-node (lambda (content)
                        (case (type content)
                            (("syntax") ($
                                (if (abstract? syntax-node) "" ($
                                    (if (and (optional? content) (not (multiple? content))) ($
"        if "(property content)" != null
"
                                    )"")
                                    (if (multiple? content)
                                        ($
"        if "(property content)" != null {
            for node: "(link content)" in "(property content)"
                node.accept(visitor)
        }
"
                                        )
                                        ($
(if (optional? content) "    " "")"        "(property content)".accept(visitor)
"
                                        )
                                    )
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

"
        (apply-to-children-of syntax-node (lambda (content) ($
            (if (property content) ($
"    let "(property content)": "
            (case (type content)
                (("syntax") ($ (link content)(if (multiple? content)"[]" "")))
                (("identifier" "operator" "prefixoperator" "binaryoperator" "postfixoperator") "String")
                (("literal") "Literal")
                (("keyword" "punctuation") "bool")
            )
"?
"           )"")
        )))
        (if (top? syntax-node)
"    let fileName: String
"
        "")
"}
"   )))
))

(define (constructor-parameters syntax-node)
    ($
        (apply-to-property-children-of syntax-node (lambda (content) ($
            (property-declaration content)(if (properties-remaining? content syntax-node) ", " "")
        )))
        (if (program? syntax-node) "" ($ (if (node-list-empty? (properties syntax-node)) "" ", ") "Position* start, Position* end"))))

(define (property-declaration content)
    (case (type content)
        (("syntax") ($ (if (multiple? content)"_Vector<" "")(link content)(if (multiple? content)">" "")"* "(property content)))
        (("identifier" "operator" "prefixoperator" "binaryoperator" "postfixoperator") ($ "String* "(property content)))
        (("literal") ($ "Literal* "(property content)))
        (("keyword" "punctuation") ($ "bool "(property content)))))

(define (scaly-syntax-constructor-parameters syntax-node)
    ($
        (apply-to-property-children-of syntax-node (lambda (content) ($
            (scaly-property-declaration content)(if (properties-remaining? content syntax-node) ", " "")
        )))
        (if (program? syntax-node) "" ($ (if (node-list-empty? (properties syntax-node)) "" ", ") "start: Position, end: Position"))))

(define (scaly-property-declaration content)
    (case (type content)
        (("syntax") ($ (property content)": "(link content)(if (multiple? content)"[]" "")))
        (("identifier" "operator" "prefixoperator" "binaryoperator" "postfixoperator") ($ (property content)": String"))
        (("literal") ($ (property content)": Literal"))
        (("keyword" "punctuation") ($ (property content)": bool"))))

(define (inheritors node)
    (if (abstract? node) (node-list-union
        (children node)
        (apply node-list-union
            (map inheritors
                (node-list->list
                    (node-list-map
                        (lambda (content-node)
                            (element-with-id (link content-node))
                        )
                        (children node)))))
    )(empty-node-list))
)

(define (constructor-initializers syntax-node)
    ($
"    this->start = start;
    this->end = end;
"       (apply-to-property-children-of syntax-node (lambda (content) ($
"    this->"(property content)" = "(property content)";
"
)))))

(define (scaly-constructor-initializers syntax-node)
    ($
"        this.start = start;
        this.end = end;
"       (apply-to-property-children-of syntax-node (lambda (content) ($
"        this."(property content)" = "(property content)";
"
)))))
