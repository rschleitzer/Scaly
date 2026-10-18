<![CDATA[
;; Rules for generating the parser, the syntax tree, the highlighter inputs
;; and the editor configuration from scaly.sgm.

(element grammar
    (sosofo-append
        (file "packages/scalyc/0.2.0/scalyc/compiler/Syntax.scaly"
            (generate-syntax)
        )
        (file "packages/scalyc/0.2.0/scalyc/compiler/parser.scaly"
            (generate-parser)
        )
        (file "packages/scalyls/0.2.0/scalyls/grammar.scaly"
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