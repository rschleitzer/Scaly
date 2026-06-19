<![CDATA[
;; Rules for generating idiomatic C++ from scaly.sgm

(element grammar
    (sosofo-append
        (file "scalyc/Syntax.h"
            (generate-syntax-cpp)
        )
        (file "scalyc/Parser.h"
            (generate-parser-h)
        )
        (file "scalyc/Parser.cpp"
            (generate-parser-cpp)
        )
        (file "scalyc/SyntaxDump.h"
            (generate-syntax-dump-cpp)
        )
        (file "packages/scalyc/0.1.0/scalyc/compiler/Syntax.scaly"
            (generate-syntax-scaly)
        )
        (file "packages/scalyc/0.1.0/scalyc/compiler/parser.scaly"
            (generate-parser-scaly)
        )
        (file "packages/scalyls/0.1.0/scalyls/grammar.scaly"
            (generate-highlight-scaly)
        )
        (file "editors/vscode/syntaxes/scaly.tmLanguage.json"
            (generate-textmate)
        )
    )
)
]]>