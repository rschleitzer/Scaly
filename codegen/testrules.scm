(element category
    (sosofo-append
        (file "packages/scaly-lang/tests/generated.test.ts"
            (generate-testjs)
        )
        (file "docs/scaly/generated-tests.xml"
            (generate-testdoc)
        )
    )
)
