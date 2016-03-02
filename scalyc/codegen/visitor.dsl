(define (visitor-scaly concrete) ($
    (if concrete "" ($
        (apply-to-selected-children "syntax" (lambda (syntax-node) ($
"class "(id syntax-node)";
"
        )))
"
"   ))

"class "(if concrete "MyVisitor : " "")"SyntaxVisitor {"
    (apply-to-selected-children "syntax" (lambda (syntax) (if (abstract? syntax) "" ($
        (if (has-syntax-children? syntax)
            ($ "
    function open"(id syntax)"("(string-firstchar-downcase (id syntax))": "(id syntax)") -> bool"
                (if concrete
                    ($ " {
"                       (apply-to-children-of syntax (lambda (identifier)
                            (if (identifier? identifier) ($
"        "(visitor-property syntax identifier)" = "(string-firstchar-downcase (id syntax))"."(property identifier)"
"                           )"")
                        ))
"        return true; }
"              )
                    ";"
                )

"
    function close"(id syntax)"("(string-firstchar-downcase (id syntax))": "(id syntax)")"
                (if concrete
                    ($ " {"
                        (apply-to-children-of syntax (lambda (identifier)
                            (if (identifier? identifier) ($
"
        "(visitor-property syntax identifier)" = null"
                            )"")
                        ))
" }"                 )
                    ";"
                )
            )
            ($ "
    function visit"(id syntax)"("(string-firstchar-downcase (id syntax))": "(id syntax)")"
                (if concrete " { }"                  ";"
                )
            )
        )
        (if (and concrete (has-syntax-children? syntax)) ($
"
"            (apply-to-children-of syntax (lambda (identifier)
                (if (identifier? identifier) ($
"
    mutable "(visitor-property syntax identifier)": String"
                )"")
             ))
"
"        )"")
    ))))
"}"
))

(define (visitor-h concrete) ($
"#ifndef __scalyc__"(if concrete "My" "")"Visitor__
#define __scalyc__"(if concrete "My" "")"Visitor__
#include \"scalyc.h\"
using namespace scaly;
namespace scalyc {
"
    (if concrete "" ($
        (apply-to-selected-children "syntax" (lambda (syntax-node) ($
"
class "(id syntax-node)";
"
        )))
   ))
"
class "(if concrete "MyVisitor : public " "")"SyntaxVisitor "(if concrete "" ": public Object ")"{
public:"
    (apply-to-selected-children "syntax" (lambda (syntax) (if (abstract? syntax) "" ($
        (if (has-syntax-children? syntax)
            ($ "
    virtual bool open"(id syntax)"("(id syntax)"* "(string-firstchar-downcase (id syntax))")"(if concrete "" " = 0")";
    virtual void close"(id syntax)"("(id syntax)"* "(string-firstchar-downcase (id syntax))")"(if concrete "" " = 0")";"
            )
            ($ "
    virtual void visit"(id syntax)"("(id syntax)"* "(string-firstchar-downcase (id syntax))")"(if concrete "" " = 0")";"
            )
        )
        (if (and concrete (has-syntax-children? syntax))
            (apply-to-children-of syntax (lambda (identifier)
                (if (identifier? identifier) ($
"
    _VarString* "(visitor-property syntax identifier)";"
                )"")
            ))
        "")
    ))))
"
"       (if (not concrete)
"    virtual bool _isSyntaxVisitor();
"       "")
"    virtual bool _isMyVisitor();
"       (if (not concrete)
"    virtual bool _isCppVisitor();
"       "")
"};

}
#endif // __scalyc__"(if concrete "My" "")"Visitor__
"
))

(define (visitor-property syntax identifier)
    ($ (string-firstchar-downcase (id syntax))(string-firstchar-upcase (property identifier))))

(define (visitor-cpp) ($
"#include \"scalyc.h\"
using namespace scaly;
namespace scalyc {

"
    (apply-to-selected-children "syntax" (lambda (syntax) (if (abstract? syntax) ""
        (if (has-syntax-children? syntax)
            ($
"
bool MyVisitor::open"(id syntax)"("(id syntax)"* "(string-firstchar-downcase (id syntax))") {
"                (apply-to-children-of syntax (lambda (identifier)
                    (if (identifier? identifier) ($
"    *"(visitor-property syntax identifier)" = *"(string-firstchar-downcase (id syntax))"->"(property identifier)";
"                   )"")
                ))
"    return true;
}

void MyVisitor::close"(id syntax)"("(id syntax)"* "(string-firstchar-downcase (id syntax))") {
"                (apply-to-children-of syntax (lambda (identifier)
                    (if (identifier? identifier) ($
"    "(visitor-property syntax identifier)" = 0;
"                   )"")
                 ))
"}
"           )
            ($
"
void MyVisitor::visit"(id syntax)"("(id syntax)"* "(string-firstchar-downcase (id syntax))") {
}
"           )
        )
    )))
"
bool SyntaxVisitor::_isSyntaxVisitor() { return true; }
bool SyntaxVisitor::_isMyVisitor() { return false; }
bool SyntaxVisitor::_isCppVisitor() { return false; }
bool MyVisitor::_isMyVisitor() { return true; }
}
"
))
