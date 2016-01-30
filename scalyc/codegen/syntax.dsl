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

    init("(if (program? syntax-node) "" "start: Position, end: Position")") {
        super.init("(if (program? syntax-node) "Position(0, 0), Position(0, 0)" "start, end")")
    }

    "(if (base syntax-node) "override " "")"function Accept(mutable visitor: SyntaxVisitor) {
        visitor.Open"(id syntax-node)"(this)
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
"(if (optional? content) "    " "")"            node.Accept(visitor)
"(if (optional? content) "    " "")"        }
"
                            )
                            ($
(if (optional? content) "    " "")"        "(property content)"!.Accept(visitor)
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
"        visitor.Close"(id syntax-node)"(this)
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
    SyntaxNode(Position start, Position end);
    Position start;
    Position end;
};

"
    (apply-to-selected-children "syntax" (lambda (syntax-node) ($
"
class "(id syntax-node)" : public "(if (base syntax-node) (base syntax-node) "SyntaxNode")" {
public:
    "(id syntax-node)"("(if (program? syntax-node) "" "Position start, Position end")");

    virtual void Accept(SyntaxVisitor& visitor);
"
        (apply-to-children-of syntax-node (lambda (content) ($
            (case (type content)
                (("syntax") ($
                    (if (abstract? syntax-node) "" ($
"    "
                        (if (multiple? content)"Array<" "")
                        (link content)
                        (if (multiple? content)">" "")"*"
" "(property content)";
"
                    ))
                ))
                (("identifier" "operator" "prefixoperator" "binaryoperator" "postfixoperator")
                    (if (property content) ($
"    String* "(property content)";
"
                    )"")
                )
                (("literal") ($
"    Literal* "(property content)";
"
                ))
                (("keyword" "punctuation") (if (property content) ($
"    bool "(property content)";
"
                )""))
                (else "")
            )
            (if (top? syntax-node)
"    String* fileName;
"
            "")
        )))
"};
"   )))
"
}
#endif // __scalyc__Syntax__
"))

(define (syntax-cpp) ($

"#include \"scalyc.h\"
using namespace scaly;
namespace scalyc {

SyntaxNode::SyntaxNode(Position start, Position end)
: start(start), end(end) {
}
"
    (apply-to-selected-children "syntax" (lambda (syntax-node) ($
"
"(id syntax-node)"::"(id syntax-node)"("(if (program? syntax-node) "" "Position start, Position end")")
: "(if (base syntax-node) (base syntax-node) "SyntaxNode")"("(if (program? syntax-node) "Position(0, 0), Position(0, 0)" "start, end")") {
}

void "(id syntax-node)"::Accept(SyntaxVisitor& visitor) {
    visitor.Open"(id syntax-node)"(*this);
"
        (apply-to-children-of syntax-node (lambda (content)
            (case (type content)
                (("syntax") ($
                    (if (abstract? syntax-node) "" ($
                        (if (optional? content) ($
"    if ("(property content)") {
"
                        )"")
                        (if (multiple? content)
                            ($
(if (optional? content) "    " "")"    {
"(if (optional? content) "    " "")"        "(link content)"* node = 0;
"(if (optional? content) "    " "")"        size_t _alength = "(property content)"->length();
"(if (optional? content) "    " "")"        for (size_t _a = 0; _a < _alength; _a++) {
"(if (optional? content) "    " "")"            node = *(*"(property content)")[_a];
"(if (optional? content) "    " "")"            node->Accept(visitor);
"(if (optional? content) "    " "")"        }
"(if (optional? content) "    " "")"    }
"
                            )
                            ($
(if (optional? content) "    " "")"    "(property content)"->Accept(visitor);
"
                            )
                        )
                        (if (optional? content) "    }
" ""                    )
                   ))
                ))
                (else "")
            )
        ))
"    visitor.Close"(id syntax-node)"(*this);
}
"
   )))
"
}
"
))
