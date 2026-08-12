<![CDATA[
;; Rules for generating the self-hosted parser/syntax sources from scaly.sgm.
;; The C++ stage-0 outputs (scalyc/Syntax.h, Parser.h/cpp, SyntaxDump.h) were
;; retired with the C++ compiler (sources frozen under retired/scalyc0/); the
;; generator functions remain loaded but unused.

(element grammar
    (sosofo-append
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
        (file "editors/vscode/language-configuration.json"
            (generate-language-configuration)
        )
    )
)
]]>