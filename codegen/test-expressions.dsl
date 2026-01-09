<!DOCTYPE STYLE-SHEET PUBLIC "-//James Clark//DTD DSSSL Style Sheet//EN" [
<!ENTITY testcpp   SYSTEM "testcpp.scm">
<!ENTITY testdoc   SYSTEM "testdoc.scm">
<!ENTITY helpers   SYSTEM "helpers.scm">
<!ENTITY fodeclare SYSTEM "fodeclare.scm">
]>

<STYLE-SHEET>
<STYLE-SPECIFICATION>

&fodeclare;
&helpers;
&testcpp;
&testdoc;

<![CDATA[
(element suite
    (sosofo-append
        (file "scalyc/ExpressionTests.h"
            (generate-tests-h "Expression")
        )
        (file "scalyc/ExpressionTests.cpp"
            (generate-tests-cpp "Expression")
        )
        (file "docs/scaly/generated-expressions.xml"
            (generate-testdoc)
        )
    )
)
]]>

</STYLE-SPECIFICATION>
</STYLE-SHEET>
