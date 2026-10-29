<![CDATA[
;; Rules for generating the parser, the syntax tree, the protocol of an
;; operation, the highlighter inputs and the editor configuration from
;; scaly.sgm.

(element grammar
    (sosofo-append
        (file "packages/scalyc/0.2.1/scalyc/compiler/Syntax.scaly"
            (generate-syntax)
        )
        (file "packages/scalyc/0.2.1/scalyc/compiler/parser.scaly"
            (generate-parser)
        )
        (file "packages/scaly/0.2.1/scaly/operation.scaly"
            (generate-operation)
        )
        (file "packages/scalyls/0.2.0/scalyls/grammar.scaly"
            (generate-highlight)
        )
        (file "editors/vscode/syntaxes/scaly.tmLanguage.json"
            (generate-textmate)
        )
        (file "editors/vscode/language-configuration.json"
            (generate-language-configuration)
        )
        (file "docs/book/words.js"
            (generate-words)
        )
    )
)
]]>