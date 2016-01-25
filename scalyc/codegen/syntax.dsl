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

    init(start: Position, end: Position) {
        super.init(start, end)
    }

"
"    "(if (base syntax-node) "override " "")"function Accept(mutable visitor: SyntaxVisitor) {
        visitor.Visit"(id syntax-node)"(this)
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
"        for node in "(property content)"! {
"(if (optional? content) "    " "")
"            node.Accept(visitor)
            }
"
                            )
                            ($
"        "(property content)"!.Accept(visitor)
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
"    }

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
class "(id syntax-node)";"
    )))
"
"
    (apply-to-selected-children "syntax" (lambda (syntax-node) ($
"
class "(id syntax-node)" : public "(if (base syntax-node) (base syntax-node) "SyntaxNode")" {
public:
    "(id syntax-node)"(Position start, Position end);

"
<!--"    "(if (base syntax-node) "override " "")"func Accept(var visitor:SyntaxVisitor) {
        visitor.Visit"(id syntax-node)"(self)
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
"        for node in "(property content)"! {
"(if (optional? content) "    " "")
"            node.Accept(visitor) }
"
                            )
                            ($
"        "(property content)"!.Accept(visitor)
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
"    }
"
-->
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
"(id syntax-node)"::"(id syntax-node)"(Position start, Position end)
: "(if (base syntax-node) (base syntax-node) "SyntaxNode")"(start, end) {
}
"
<!--
"    "(if (base syntax-node) "override " "")"func Accept(var visitor:SyntaxVisitor) {
        visitor.Visit"(id syntax-node)"(self)
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
"        for node in "(property content)"! {
"(if (optional? content) "    " "")
"            node.Accept(visitor) }
"
                            )
                            ($
"        "(property content)"!.Accept(visitor)
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
"    }
"
-->
   )))
"
}
"
))
