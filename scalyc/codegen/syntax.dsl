(define (syntax-scaly) ($

"class SyntaxNode {

    init(start: Position, end: Position) {
        start = start
        end = end
    }

    let start: Position
    let end: Position
}
"
    (apply-to-selected-children "syntax" (lambda (syntax-node) ($
"
class "(id syntax-node)" : "(if (base syntax-node) (base syntax-node) "SyntaxNode")" {

    init("(scaly-syntax-constructor-parameters syntax-node)") {
        super.init("(if (program? syntax-node) "Position(0, 0), Position(0, 0)" "start, end")")
    }

    "(if (base syntax-node) "override " "")"function accept(mutable visitor: SyntaxVisitor) {
        visitor.open"(id syntax-node)"(this)
"
        (apply-to-children-of syntax-node (lambda (content)
            (case (type content)
                (("syntax") ($
                    (if (abstract? syntax-node) "" ($
                        (if (optional? content) ($
"        if "(property content)" != nil {
"
                        )"")
                        (if (multiple? content)
                            ($
(if (optional? content) "    " "")"        for node in "(property content)"! {
"(if (optional? content) "    " "")"            node.accept(visitor)
"(if (optional? content) "    " "")"        }
"
                            )
                            ($
(if (optional? content) "    " "")"        "(property content)"!.accept(visitor)
"
                            )
                        )
                        (if (optional? content) "        }
" ""                    )
                   ))
                ))
                (else "")
            )
        ))
"        visitor.close"(id syntax-node)"(this)
    }

"
        (apply-to-children-of syntax-node (lambda (content) ($
            (if (property content) ($
"    let "(property content)": "
            (case (type content)
                (("syntax") ($ (if (multiple? content)"[" "")(link content)(if (multiple? content)"]" "")))
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

(define (syntax-h) ($

"#ifndef __scalyc__Syntax__
#define __scalyc__Syntax__
#include \"scalyc.h\"
using namespace scaly;
namespace scalyc {

class SyntaxNode : public Object {
public:
    SyntaxNode(Position* start, Position* end);
    Position* start;
    Position* end;
    virtual bool _isSyntaxNode();
"   (apply-to-selected-children "syntax" (lambda (syntax-node) ($
"    virtual bool _is"(id syntax-node)"();
"   )))
"};
"
    (apply-to-selected-children "syntax" (lambda (syntax-node) ($
"
class "(id syntax-node)" : public "(if (base syntax-node) (base syntax-node) "SyntaxNode")" {
public:
    "(id syntax-node)"("(constructor-parameters syntax-node)");
    virtual void accept(SyntaxVisitor* visitor);
"
        (apply-to-property-children-of syntax-node (lambda (content) ($
"    "(property-declaration content)";
"       )))
        (if (top? syntax-node)
"    _LetString* fileName;
"
        "")
"
    virtual bool _is"(id syntax-node)"();
"       (apply-to-nodelist (inheritors syntax-node) (lambda (inheritor) ($
"    virtual bool _is"(attribute-string "link" inheritor)"();
"       )))
"};
"   )))
"
}
#endif // __scalyc__Syntax__
"))

(define (constructor-parameters syntax-node)
    ($
        (apply-to-property-children-of syntax-node (lambda (content) ($
            (property-declaration content)(if (properties-remaining? content syntax-node) ", " "")
        )))
        (if (program? syntax-node) "" ($ (if (node-list-empty? (properties syntax-node)) "" ", ") "Position* start, Position* end"))))

(define (property-declaration content)
    (case (type content)
        (("syntax") ($ (if (multiple? content)"_Vector<" "")(link content)(if (multiple? content)">" "")"* "(property content)))
        (("identifier" "operator" "prefixoperator" "binaryoperator" "postfixoperator") ($ "_LetString* "(property content)))
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
        (("syntax") ($ (property content)": "(if (multiple? content)"[" "")(link content)(if (multiple? content)"]" "")))
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
        (if (base syntax-node) (base syntax-node) "SyntaxNode") "(start, end)" (if (node-list-empty? (properties syntax-node)) "" ", ")
        (apply-to-property-children-of syntax-node (lambda (content) ($
            (property content)"("(property content)")"(if (properties-remaining? content syntax-node) ", " "")
        )))))

(define (syntax-cpp) ($

"#include \"scalyc.h\"
using namespace scaly;
namespace scalyc {

SyntaxNode::SyntaxNode(Position* start, Position* end)
: start(start), end(end) {
}

bool SyntaxNode::_isSyntaxNode() { return true; }
"   (apply-to-selected-children "syntax" (lambda (syntax-node) ($
"bool SyntaxNode::_is"(id syntax-node)"() { return false; }
"   )))
    (apply-to-selected-children "syntax" (lambda (syntax-node) ($
"
"(id syntax-node)"::"(id syntax-node)"("(constructor-parameters syntax-node)")
: "     (if (program? syntax-node)
            "SyntaxNode(new(getPage()) Position(0, 0), new(getPage()) Position(0, 0)), name(name), directory(directory), compilationUnits(compilationUnits)"
            (constructor-initializers syntax-node)
        )
" {}

"       (apply-to-nodelist (inheritors syntax-node) (lambda (inheritor) ($
"bool "(id syntax-node)"::_is"(attribute-string "link" inheritor)"() { return false; }
"       )))
"bool "(id syntax-node)"::_is"(id syntax-node)"() { return true; }

void "(id syntax-node)"::accept(SyntaxVisitor* visitor) {
"       (if (abstract? syntax-node) "" ($
            (if (has-syntax-children? syntax-node)
                ($
"    if (!visitor->open"(id syntax-node)"(this))
        return;
"
                    (apply-to-children-of syntax-node (lambda (content)
                        (case (type content)
                            (("syntax") ($
                                (if (abstract? syntax-node) "" ($
                                    (if (and (optional? content) (not (multiple? content))) ($
"    if ("(property content)")
"
                                    )"")
                                    (if (multiple? content)
                                        ($
"    if ("(property content)") {
        "(link content)"* node = 0;
        size_t _alength = "(property content)"->length();
        for (size_t _a = 0; _a < _alength; _a++) {
            node = *(*"(property content)")[_a];
            node->accept(visitor);
        }
    }
"
                                        )
                                        ($
(if (optional? content) "    " "")"    "(property content)"->accept(visitor);
"
                                        )
                                    )
                                ))
                            ))
                            (else "")
                        )
                    ))
"    visitor->close"(id syntax-node)"(this);
"
                )
                ($
"    visitor->visit"(id syntax-node)"(this);
"
                )
            )
       ))
"}
"   )))
"
}
"
))
