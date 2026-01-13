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
        (file "packages/scalyc/0.1.0/scalyc/compiler/Syntax.scaly"
            (generate-syntax-scaly)
        )
    )
)
]]>