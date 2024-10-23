(define (syntax) ($
"
struct IdentifierLiteral {
    IdentifierLiteral(String name) : name(name) {}
    String name;
};

struct StringLiteral {
    StringLiteral(String value) : value(value) {}
    String value;
};

struct CharacterLiteral {
    CharacterLiteral(String value) : value(value) {}
    String value;
};

struct FragmentLiteral {
    FragmentLiteral(String value) : value(value) {}
    String value;
};

struct IntegerLiteral {
    IntegerLiteral(String value) : value(value) {}
    String value;
};

struct BooleanLiteral {
    BooleanLiteral(bool value) : value(value) {}
    bool value;
};

struct FloatingPointLiteral {
    FloatingPointLiteral(String value) : value(value) {}
    String value;
};

struct HexLiteral {
    HexLiteral(String value) : value(value) {}
    String value;
};

struct Literal : Object {
    Literal(StringLiteral string) : _tag(String), _String(string) {}
    Literal(CharacterLiteral string) : _tag(Character), _Character(string) {}
    Literal(FragmentLiteral fragment) : _tag(Fragment), _Fragment(fragment) {}
    Literal(IntegerLiteral integer) : _tag(Integer), _Integer(integer) {}
    Literal(BooleanLiteral boolean) : _tag(Boolean), _Boolean(boolean) {}
    Literal(FloatingPointLiteral floating_point) : _tag(FloatingPoint), _FloatingPoint(floating_point) {}
    Literal(HexLiteral hex) : _tag(Hex), _Hex(hex) {}
    enum {
        String,
        Character,
        Fragment,
        Integer,
        Boolean,
        FloatingPoint,
        Hex,
    } _tag;
    union {
        StringLiteral _String;
        CharacterLiteral _Character;
        FragmentLiteral _Fragment;
        IntegerLiteral _Integer;
        BooleanLiteral _Boolean;
        FloatingPointLiteral _FloatingPoint;
        HexLiteral _Hex;

    };
};

"
    (apply-to-selected-children "syntax" (lambda (syntax-node) ($
"struct "(id syntax-node)"Syntax; 
"   )))

    (apply-to-nodelist (node-list-reverse (select-elements (children (current-node)) "syntax")) (lambda (syntax-node)
        (if (not (abstract? syntax-node))
            ($
"
struct "(id syntax-node)"Syntax : Object {
    "           (id syntax-node)"Syntax(size_t start, size_t end"
                (apply-to-children-of syntax-node (lambda (content) ($
                    (if (property content) ($
                            ", "
                            (case (type content)
                                (("syntax") ($ (if (multiple? content) ($ "Vector<") "")(link content)"Syntax"(if (multiple? content)">" "")(if (or (optional? content) (multiple? content))"*" "")))
                                (("identifier" "attribute") ($ "String"(if (optional? content)"*" "")))
                                (("literal") "Literal")
                                (("keyword" "punctuation") "bool")
                            )
                            " "
                            (property content)
                    )"")
                )))
    ") : start(start), end(end)"
                (apply-to-children-of syntax-node (lambda (content) ($
                    (if (property content) ($
                ", "
                (property content)"("(property content)")"
                    )"")
                )))
        " {}
    size_t start;
    size_t end;"
                (apply-to-children-of syntax-node (lambda (content) ($
                    (if (property content) ($
"
    "
                        (case (type content)
                            (("syntax") ($ (if (multiple? content) ($ "Vector<") "")(link content)"Syntax"(if (multiple? content)">" "")(if (or (optional? content) (multiple? content))"*" "")))
                            (("identifier" "attribute") ($ "String"(if (optional? content)"*" "")))
                            (("literal") "Literal")
                            (("keyword" "punctuation") "bool")
                        )
" "
                        (property content)";"
                    )"")
                )))
"
};
"           )
            ($
"
struct "(id syntax-node)"Syntax : Object {
"               (apply-to-children-of syntax-node (lambda (content) ($
"    "          (id syntax-node)"Syntax("(link content)"Syntax _"(link content)"Syntax) : _tag("(link content)") { _"(link content)" = _"(link content)"Syntax; }
"               )))

"    enum {"
                (apply-to-children-of syntax-node (lambda (content) ($
"
        "(link content)","
                )))
"
    } _tag;
    union {"
                (apply-to-children-of syntax-node (lambda (content) ($
"
        "(link content)"Syntax _"(link content)";"
                )))
"
    };
};
"           )
        )
    ))
))
