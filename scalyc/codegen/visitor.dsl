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
class "(id syntax-node)";"
        )))
"
"   ))
"
class "(if concrete "MyVisitor : public " "")"SyntaxVisitor "(if concrete "" ": public Object")"{
public:"
    (apply-to-selected-children "syntax" (lambda (syntax) (if (abstract? syntax) "" ($
        (if (has-syntax-children? syntax)
            ($ "
    virtual bool open"(id syntax)"("(id syntax)"& "(string-firstchar-downcase (id syntax))")"(if concrete "" "= 0")";
    virtual void close"(id syntax)"("(id syntax)"& "(string-firstchar-downcase (id syntax))")"(if concrete "" "= 0")";"
            )
            ($ "
    virtual void visit"(id syntax)"("(id syntax)"& "(string-firstchar-downcase (id syntax))")"(if concrete "" "= 0")";"
            )
        )
        (if concrete
            (apply-to-children-of syntax (lambda (identifier)
                (if (identifier? identifier) ($
"
    String* "(visitor-property syntax identifier)";"
                )"")
            ))
        "")
    ))))
"
};

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
bool MyVisitor::open"(id syntax)"("(id syntax)"& "(string-firstchar-downcase (id syntax))") {
"                (apply-to-children-of syntax (lambda (identifier)
                    (if (identifier? identifier) ($
"    "(visitor-property syntax identifier)" = "(string-firstchar-downcase (id syntax))"."(property identifier)";
"                   )"")
                ))
"    return true;
}

void MyVisitor::close"(id syntax)"("(id syntax)"& "(string-firstchar-downcase (id syntax))") {
"                (apply-to-children-of syntax (lambda (identifier)
                    (if (identifier? identifier) ($
"    "(visitor-property syntax identifier)" = 0;
"                   )"")
                 ))
"}
"           )
            ($
"
void MyVisitor::visit"(id syntax)"("(id syntax)"& "(string-firstchar-downcase (id syntax))") {
}
"           )
        )
    )))
"
}
"
))
