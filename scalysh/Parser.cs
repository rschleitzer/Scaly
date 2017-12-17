using System;
using System.Collections.Generic;
using System.Text;

namespace scalysh
{
    class Parser
    {
        Lexer lexer;
        string fileName;

        string usingKeyword = "using";
        string letKeyword = "let";
        string mutableKeyword = "mutable";
        string varKeyword = "var";
        string threadKeyword = "thread";
        string setKeyword = "set";
        string classKeyword = "class";
        string extendsKeyword = "extends";
        string functionKeyword = "function";
        string methodKeyword = "method";
        string thisKeyword = "this";
        string sizeofKeyword = "sizeof";
        string catchKeyword = "catch";
        string asKeyword = "as";
        string isKeyword = "is";
        string ifKeyword = "if";
        string elseKeyword = "else";
        string forKeyword = "for";
        string inKeyword = "in";
        string whileKeyword = "while";
        string doKeyword = "do";
        string breakKeyword = "break";
        string returnKeyword = "return";

        string leftCurly = "{";
        string rightCurly = "}";
        string leftParen = "(";
        string rightParen = ")";
        string leftBracket = "[";
        string rightBracket = "]";
        string dot = ".";
        string comma = ",";
        string colon = ":";
        string question = "?";
        string exclamation = "!";
        string at = "@";
        string hash = "#";
        string dollar = "$";
        string underscore = "_";
        string backtick = "`";

        public Parser(string theFileName, string text)
        {
            lexer = new Lexer(text);
            fileName = theFileName;
        }

        public File parseFile()
        {
            Position start = lexer.getPreviousPosition();

            Statement[] statements = parseStatementList();
            if (statements != null)
            {
                if (!isAtEnd())
                {
                    Position errorPos = lexer.getPreviousPosition();
                    throw new ParserException(fileName, errorPos.line, errorPos.column);
                }
            }

            Position end = lexer.getPosition();

            File ret = new File(start, end, statements);

            if (statements != null)
            {
                foreach (Statement item in statements)
                    item.parent = ret;
            }

            return ret;
        }

        public Block parseBlock()
        {
            Position start = lexer.getPreviousPosition();

            bool successLeftCurly1 = lexer.parsePunctuation(leftCurly);
            if (successLeftCurly1)
                lexer.advance();
            else
                return null;

            Statement[] statements = parseStatementList();

            bool successRightCurly3 = lexer.parsePunctuation(rightCurly);
            if (successRightCurly3)
                lexer.advance();
            else
                return null;

            Position end = lexer.getPosition();

            Block ret = new Block(start, end, statements);

            if (statements != null)
            {
                foreach (Statement item in statements)
                    item.parent = ret;
            }

            return ret;
        }

        public Statement[] parseStatementList()
        {
            List<Statement> ret = null;
            while (true)
            {
                Statement node = parseStatement();
                if (node == null)
                    break;

                if (ret == null)
                    ret = new List<Statement>();

                ret.Add(node);
            }

            if (ret != null)
                return ret.ToArray();
            else
                return null;
        }

        public Statement parseStatement()
        {
            {
                Using node = parseUsing();
                if (node != null)
                    return node;
            }

            {
                Declaration node = parseDeclaration();
                if (node != null)
                    return node;
            }

            {
                Expression node = parseExpression();
                if (node != null)
                    return node;
            }

            {
                Set node = parseSet();
                if (node != null)
                    return node;
            }

            {
                Break node = parseBreak();
                if (node != null)
                    return node;
            }

            {
                Return node = parseReturn();
                if (node != null)
                    return node;
            }

            return null;
        }

        public Using parseUsing()
        {
            Position start = lexer.getPreviousPosition();

            bool successUsing1 = lexer.parseKeyword(usingKeyword);
            if (successUsing1)
                lexer.advance();
            else
                return null;

            Name name = parseName();
            if (name == null)
                return null;

            Position end = lexer.getPosition();

            Using ret = new Using(start, end, name);

            name.parent = ret;

            return ret;
        }

        public Declaration[] parseDeclarationList()
        {
            List<Declaration> ret = null;
            while (true)
            {
                Declaration node = parseDeclaration();
                if (node == null)
                    break;

                if (ret == null)
                    ret = new List<Declaration>();

                ret.Add(node);
            }

            if (ret != null)
                return ret.ToArray();
            else
                return null;
        }

        public Declaration parseDeclaration()
        {
            {
                Let node = parseLet();
                if (node != null)
                    return node;
            }

            {
                Mutable node = parseMutable();
                if (node != null)
                    return node;
            }

            {
                Var node = parseVar();
                if (node != null)
                    return node;
            }

            {
                Thread node = parseThread();
                if (node != null)
                    return node;
            }

            {
                Class node = parseClass();
                if (node != null)
                    return node;
            }

            {
                Method node = parseMethod();
                if (node != null)
                    return node;
            }

            {
                Function node = parseFunction();
                if (node != null)
                    return node;
            }

            return null;
        }

        public Let parseLet()
        {
            Position start = lexer.getPreviousPosition();

            bool successLet1 = lexer.parseKeyword(letKeyword);
            if (successLet1)
                lexer.advance();
            else
                return null;

            Binding binding = parseBinding();
            if (binding == null)
                return null;

            Position end = lexer.getPosition();

            Let ret = new Let(start, end, binding);

            binding.parent = ret;

            return ret;
        }

        public Mutable parseMutable()
        {
            Position start = lexer.getPreviousPosition();

            bool successMutable1 = lexer.parseKeyword(mutableKeyword);
            if (successMutable1)
                lexer.advance();
            else
                return null;

            Binding binding = parseBinding();
            if (binding == null)
                return null;

            Position end = lexer.getPosition();

            Mutable ret = new Mutable(start, end, binding);

            binding.parent = ret;

            return ret;
        }

        public Var parseVar()
        {
            Position start = lexer.getPreviousPosition();

            bool successVar1 = lexer.parseKeyword(varKeyword);
            if (successVar1)
                lexer.advance();
            else
                return null;

            Binding binding = parseBinding();
            if (binding == null)
                return null;

            Position end = lexer.getPosition();

            Var ret = new Var(start, end, binding);

            binding.parent = ret;

            return ret;
        }

        public Thread parseThread()
        {
            Position start = lexer.getPreviousPosition();

            bool successThread1 = lexer.parseKeyword(threadKeyword);
            if (successThread1)
                lexer.advance();
            else
                return null;

            Binding binding = parseBinding();
            if (binding == null)
                return null;

            Position end = lexer.getPosition();

            Thread ret = new Thread(start, end, binding);

            binding.parent = ret;

            return ret;
        }

        public Binding parseBinding()
        {
            Position start = lexer.getPreviousPosition();

            Pattern pattern = parsePattern();
            if (pattern == null)
                return null;

            TypeAnnotation typeAnnotation = parseTypeAnnotation();

            Expression[] expressions = parseExpressionList();

            Position end = lexer.getPosition();

            Binding ret = new Binding(start, end, pattern, typeAnnotation, expressions);

            pattern.parent = ret;
            if (typeAnnotation != null)
                typeAnnotation.parent = ret;
            if (expressions != null)
            {
                foreach (Expression item in expressions)
                    item.parent = ret;
            }

            return ret;
        }

        public Pattern parsePattern()
        {
            {
                WildcardPattern node = parseWildcardPattern();
                if (node != null)
                    return node;
            }

            {
                IdentifierPattern node = parseIdentifierPattern();
                if (node != null)
                    return node;
            }

            return null;
        }

        public IdentifierPattern parseIdentifierPattern()
        {
            Position start = lexer.getPreviousPosition();

            Name name = parseName();
            if (name == null)
                return null;

            TypeAnnotation annotationForType = parseTypeAnnotation();

            Position end = lexer.getPosition();

            IdentifierPattern ret = new IdentifierPattern(start, end, name, annotationForType);

            name.parent = ret;
            if (annotationForType != null)
                annotationForType.parent = ret;

            return ret;
        }

        public WildcardPattern parseWildcardPattern()
        {
            Position start = lexer.getPreviousPosition();

            bool successUnderscore1 = lexer.parsePunctuation(underscore);
            if (successUnderscore1)
                lexer.advance();
            else
                return null;

            Position end = lexer.getPosition();

            WildcardPattern ret = new WildcardPattern(start, end);


            return ret;
        }

        public Expression[] parseExpressionList()
        {
            List<Expression> ret = null;
            while (true)
            {
                Expression node = parseExpression();
                if (node == null)
                    break;

                if (ret == null)
                    ret = new List<Expression>();

                ret.Add(node);
            }

            if (ret != null)
                return ret.ToArray();
            else
                return null;
        }

        public Expression parseExpression()
        {
            Position start = lexer.getPreviousPosition();

            PrimaryExpression primary = parsePrimaryExpression();
            if (primary == null)
                return null;

            Postfix[] postfixes = parsePostfixList();

            Position end = lexer.getPosition();

            Expression ret = new Expression(start, end, primary, postfixes);

            primary.parent = ret;
            if (postfixes != null)
            {
                foreach (Postfix item in postfixes)
                    item.parent = ret;
            }

            return ret;
        }

        public PrimaryExpression[] parsePrimaryExpressionList()
        {
            List<PrimaryExpression> ret = null;
            while (true)
            {
                PrimaryExpression node = parsePrimaryExpression();
                if (node == null)
                    break;

                if (ret == null)
                    ret = new List<PrimaryExpression>();

                ret.Add(node);
            }

            if (ret != null)
                return ret.ToArray();
            else
                return null;
        }

        public PrimaryExpression parsePrimaryExpression()
        {
            {
                NameExpression node = parseNameExpression();
                if (node != null)
                    return node;
            }

            {
                LiteralExpression node = parseLiteralExpression();
                if (node != null)
                    return node;
            }

            {
                If node = parseIf();
                if (node != null)
                    return node;
            }

            {
                For node = parseFor();
                if (node != null)
                    return node;
            }

            {
                While node = parseWhile();
                if (node != null)
                    return node;
            }

            {
                Do node = parseDo();
                if (node != null)
                    return node;
            }

            {
                This node = parseThis();
                if (node != null)
                    return node;
            }

            {
                ObjectExpression node = parseObjectExpression();
                if (node != null)
                    return node;
            }

            {
                Block node = parseBlock();
                if (node != null)
                    return node;
            }

            {
                SizeOf node = parseSizeOf();
                if (node != null)
                    return node;
            }

            return null;
        }

        public NameExpression parseNameExpression()
        {
            Position start = lexer.getPreviousPosition();

            Name name = parseName();
            if (name == null)
                return null;

            GenericArguments generics = parseGenericArguments();

            LifeTime lifeTime = parseLifeTime();

            Position end = lexer.getPosition();

            NameExpression ret = new NameExpression(start, end, name, generics, lifeTime);

            name.parent = ret;
            if (generics != null)
                generics.parent = ret;
            if (lifeTime != null)
                lifeTime.parent = ret;

            return ret;
        }

        public LiteralExpression parseLiteralExpression()
        {
            Position start = lexer.getPreviousPosition();

            Literal literal = lexer.parseLiteral();
            if (literal != null)
                lexer.advance();
            else
                return null;

            Position end = lexer.getPosition();

            LiteralExpression ret = new LiteralExpression(start, end, literal);


            return ret;
        }

        public If parseIf()
        {
            Position start = lexer.getPreviousPosition();

            bool successIf1 = lexer.parseKeyword(ifKeyword);
            if (successIf1)
                lexer.advance();
            else
                return null;

            Expression condition = parseExpression();
            if (condition == null)
                return null;

            Expression consequent = parseExpression();
            if (consequent == null)
                return null;

            Else elseClause = parseElse();

            Position end = lexer.getPosition();

            If ret = new If(start, end, condition, consequent, elseClause);

            condition.parent = ret;
            consequent.parent = ret;
            if (elseClause != null)
                elseClause.parent = ret;

            return ret;
        }

        public Else parseElse()
        {
            Position start = lexer.getPreviousPosition();

            bool successElse1 = lexer.parseKeyword(elseKeyword);
            if (successElse1)
                lexer.advance();
            else
                return null;

            Expression alternative = parseExpression();
            if (alternative == null)
                return null;

            Position end = lexer.getPosition();

            Else ret = new Else(start, end, alternative);

            alternative.parent = ret;

            return ret;
        }

        public For parseFor()
        {
            Position start = lexer.getPreviousPosition();

            bool successFor1 = lexer.parseKeyword(forKeyword);
            if (successFor1)
                lexer.advance();
            else
                return null;

            string index = lexer.parseIdentifier();
            if ((index != null) && isIdentifier(index))
                lexer.advance();
            else
                return null;

            TypeAnnotation typeAnnotation = parseTypeAnnotation();

            bool successIn4 = lexer.parseKeyword(inKeyword);
            if (successIn4)
                lexer.advance();
            else
                return null;

            Expression expression = parseExpression();
            if (expression == null)
                return null;

            Expression code = parseExpression();
            if (code == null)
                return null;

            Position end = lexer.getPosition();

            For ret = new For(start, end, index, typeAnnotation, expression, code);

            if (typeAnnotation != null)
                typeAnnotation.parent = ret;
            expression.parent = ret;
            code.parent = ret;

            return ret;
        }

        public While parseWhile()
        {
            Position start = lexer.getPreviousPosition();

            bool successWhile1 = lexer.parseKeyword(whileKeyword);
            if (successWhile1)
                lexer.advance();
            else
                return null;

            Expression condition = parseExpression();
            if (condition == null)
                return null;

            Expression code = parseExpression();
            if (code == null)
                return null;

            Position end = lexer.getPosition();

            While ret = new While(start, end, condition, code);

            condition.parent = ret;
            code.parent = ret;

            return ret;
        }

        public Do parseDo()
        {
            Position start = lexer.getPreviousPosition();

            bool successDo1 = lexer.parseKeyword(doKeyword);
            if (successDo1)
                lexer.advance();
            else
                return null;

            Expression code = parseExpression();
            if (code == null)
                return null;

            bool successWhile3 = lexer.parseKeyword(whileKeyword);
            if (successWhile3)
                lexer.advance();
            else
                return null;

            Expression condition = parseExpression();
            if (condition == null)
                return null;

            Position end = lexer.getPosition();

            Do ret = new Do(start, end, code, condition);

            code.parent = ret;
            condition.parent = ret;

            return ret;
        }

        public This parseThis()
        {
            Position start = lexer.getPreviousPosition();

            bool successThis1 = lexer.parseKeyword(thisKeyword);
            if (successThis1)
                lexer.advance();
            else
                return null;

            Position end = lexer.getPosition();

            This ret = new This(start, end);


            return ret;
        }

        public Postfix[] parsePostfixList()
        {
            List<Postfix> ret = null;
            while (true)
            {
                Postfix node = parsePostfix();
                if (node == null)
                    break;

                if (ret == null)
                    ret = new List<Postfix>();

                ret.Add(node);
            }

            if (ret != null)
                return ret.ToArray();
            else
                return null;
        }

        public Postfix parsePostfix()
        {
            {
                Catch node = parseCatch();
                if (node != null)
                    return node;
            }

            {
                MemberAccess node = parseMemberAccess();
                if (node != null)
                    return node;
            }

            {
                Subscript node = parseSubscript();
                if (node != null)
                    return node;
            }

            {
                As node = parseAs();
                if (node != null)
                    return node;
            }

            {
                Is node = parseIs();
                if (node != null)
                    return node;
            }

            return null;
        }

        public Catch parseCatch()
        {
            Position start = lexer.getPreviousPosition();

            bool successCatch1 = lexer.parseKeyword(catchKeyword);
            if (successCatch1)
                lexer.advance();
            else
                return null;

            Position end = lexer.getPosition();

            Catch ret = new Catch(start, end);


            return ret;
        }

        public MemberAccess parseMemberAccess()
        {
            Position start = lexer.getPreviousPosition();

            bool successDot1 = lexer.parsePunctuation(dot);
            if (successDot1)
                lexer.advance();
            else
                return null;

            string member = lexer.parseIdentifier();
            if ((member != null) && isIdentifier(member))
                lexer.advance();
            else
                return null;

            Position end = lexer.getPosition();

            MemberAccess ret = new MemberAccess(start, end, member);


            return ret;
        }

        public Subscript parseSubscript()
        {
            Position start = lexer.getPreviousPosition();

            bool successLeftBracket1 = lexer.parsePunctuation(leftBracket);
            if (successLeftBracket1)
                lexer.advance();
            else
                return null;

            Expression[] firstItems = parseExpressionList();

            ObjectItem[] additionalItemses = parseObjectItemList();

            bool successRightBracket4 = lexer.parsePunctuation(rightBracket);
            if (successRightBracket4)
                lexer.advance();
            else
                return null;

            Position end = lexer.getPosition();

            Subscript ret = new Subscript(start, end, firstItems, additionalItemses);

            if (firstItems != null)
            {
                foreach (Expression item in firstItems)
                    item.parent = ret;
            }
            if (additionalItemses != null)
            {
                foreach (ObjectItem item in additionalItemses)
                    item.parent = ret;
            }

            return ret;
        }

        public As parseAs()
        {
            Position start = lexer.getPreviousPosition();

            bool successAs1 = lexer.parseKeyword(asKeyword);
            if (successAs1)
                lexer.advance();
            else
                return null;

            Type typeSpec = parseType();
            if (typeSpec == null)
                return null;

            Position end = lexer.getPosition();

            As ret = new As(start, end, typeSpec);

            typeSpec.parent = ret;

            return ret;
        }

        public Is parseIs()
        {
            Position start = lexer.getPreviousPosition();

            bool successIs1 = lexer.parseKeyword(isKeyword);
            if (successIs1)
                lexer.advance();
            else
                return null;

            Type typeSpec = parseType();
            if (typeSpec == null)
                return null;

            Position end = lexer.getPosition();

            Is ret = new Is(start, end, typeSpec);

            typeSpec.parent = ret;

            return ret;
        }

        public ObjectExpression parseObjectExpression()
        {
            Position start = lexer.getPreviousPosition();

            bool successLeftParen1 = lexer.parsePunctuation(leftParen);
            if (successLeftParen1)
                lexer.advance();
            else
                return null;

            Expression[] firstItems = parseExpressionList();

            ObjectItem[] additionalItemses = parseObjectItemList();

            bool successRightParen4 = lexer.parsePunctuation(rightParen);
            if (successRightParen4)
                lexer.advance();
            else
                return null;

            Position end = lexer.getPosition();

            ObjectExpression ret = new ObjectExpression(start, end, firstItems, additionalItemses);

            if (firstItems != null)
            {
                foreach (Expression item in firstItems)
                    item.parent = ret;
            }
            if (additionalItemses != null)
            {
                foreach (ObjectItem item in additionalItemses)
                    item.parent = ret;
            }

            return ret;
        }

        public ObjectItem[] parseObjectItemList()
        {
            List<ObjectItem> ret = null;
            while (true)
            {
                ObjectItem node = parseObjectItem();
                if (node == null)
                    break;

                if (ret == null)
                    ret = new List<ObjectItem>();

                ret.Add(node);
            }

            if (ret != null)
                return ret.ToArray();
            else
                return null;
        }

        public ObjectItem parseObjectItem()
        {
            Position start = lexer.getPreviousPosition();

            bool successComma1 = lexer.parsePunctuation(comma);
            if (successComma1)
                lexer.advance();
            else
                return null;

            Expression[] expression = parseExpressionList();

            Position end = lexer.getPosition();

            ObjectItem ret = new ObjectItem(start, end, expression);

            if (expression != null)
            {
                foreach (Expression item in expression)
                    item.parent = ret;
            }

            return ret;
        }

        public SizeOf parseSizeOf()
        {
            Position start = lexer.getPreviousPosition();

            bool successSizeof1 = lexer.parseKeyword(sizeofKeyword);
            if (successSizeof1)
                lexer.advance();
            else
                return null;

            Type typeSpec = parseType();
            if (typeSpec == null)
                return null;

            Position end = lexer.getPosition();

            SizeOf ret = new SizeOf(start, end, typeSpec);

            typeSpec.parent = ret;

            return ret;
        }

        public Set parseSet()
        {
            Position start = lexer.getPreviousPosition();

            bool successSet1 = lexer.parseKeyword(setKeyword);
            if (successSet1)
                lexer.advance();
            else
                return null;

            Expression[] lValue = parseExpressionList();

            bool successColon3 = lexer.parsePunctuation(colon);
            if (successColon3)
                lexer.advance();
            else
                return null;

            Expression[] rValue = parseExpressionList();

            Position end = lexer.getPosition();

            Set ret = new Set(start, end, lValue, rValue);

            if (lValue != null)
            {
                foreach (Expression item in lValue)
                    item.parent = ret;
            }
            if (rValue != null)
            {
                foreach (Expression item in rValue)
                    item.parent = ret;
            }

            return ret;
        }

        public Break parseBreak()
        {
            Position start = lexer.getPreviousPosition();

            bool successBreak1 = lexer.parseKeyword(breakKeyword);
            if (successBreak1)
                lexer.advance();
            else
                return null;

            Expression[] lValue = parseExpressionList();

            Position end = lexer.getPosition();

            Break ret = new Break(start, end, lValue);

            if (lValue != null)
            {
                foreach (Expression item in lValue)
                    item.parent = ret;
            }

            return ret;
        }

        public Return parseReturn()
        {
            Position start = lexer.getPreviousPosition();

            bool successReturn1 = lexer.parseKeyword(returnKeyword);
            if (successReturn1)
                lexer.advance();
            else
                return null;

            Expression[] lValue = parseExpressionList();

            Position end = lexer.getPosition();

            Return ret = new Return(start, end, lValue);

            if (lValue != null)
            {
                foreach (Expression item in lValue)
                    item.parent = ret;
            }

            return ret;
        }

        public Class parseClass()
        {
            Position start = lexer.getPreviousPosition();

            bool successClass1 = lexer.parseKeyword(classKeyword);
            if (successClass1)
                lexer.advance();
            else
                return null;

            Name name = parseName();
            if (name == null)
                return null;

            GenericParameters generics = parseGenericParameters();

            Extends baseClass = parseExtends();

            Object contents = parseObject();

            Expression body = parseExpression();

            Position end = lexer.getPosition();

            Class ret = new Class(start, end, name, generics, baseClass, contents, body);

            name.parent = ret;
            if (generics != null)
                generics.parent = ret;
            if (baseClass != null)
                baseClass.parent = ret;
            if (contents != null)
                contents.parent = ret;
            if (body != null)
                body.parent = ret;

            return ret;
        }

        public Name parseName()
        {
            Position start = lexer.getPreviousPosition();

            string name = lexer.parseIdentifier();
            if ((name != null) && isIdentifier(name))
                lexer.advance();
            else
                return null;

            Extension[] extensions = parseExtensionList();

            Position end = lexer.getPosition();

            Name ret = new Name(start, end, name, extensions);

            if (extensions != null)
            {
                foreach (Extension item in extensions)
                    item.parent = ret;
            }

            return ret;
        }

        public Extension[] parseExtensionList()
        {
            List<Extension> ret = null;
            while (true)
            {
                Extension node = parseExtension();
                if (node == null)
                    break;

                if (ret == null)
                    ret = new List<Extension>();

                ret.Add(node);
            }

            if (ret != null)
                return ret.ToArray();
            else
                return null;
        }

        public Extension parseExtension()
        {
            Position start = lexer.getPreviousPosition();

            bool successDot1 = lexer.parsePunctuation(dot);
            if (successDot1)
                lexer.advance();
            else
                return null;

            string name = lexer.parseIdentifier();
            if ((name != null) && isIdentifier(name))
                lexer.advance();
            else
                return null;

            Position end = lexer.getPosition();

            Extension ret = new Extension(start, end, name);


            return ret;
        }

        public GenericParameters parseGenericParameters()
        {
            Position start = lexer.getPreviousPosition();

            bool successBacktick1 = lexer.parsePunctuation(backtick);
            if (successBacktick1)
                lexer.advance();
            else
                return null;

            string name = lexer.parseIdentifier();
            if ((name != null) && isIdentifier(name))
                lexer.advance();
            else
                return null;

            GenericParameter[] additionalGenerics = parseGenericParameterList();

            bool successBacktick4 = lexer.parsePunctuation(backtick);
            if (successBacktick4)
                lexer.advance();
            else
                return null;

            Position end = lexer.getPosition();

            GenericParameters ret = new GenericParameters(start, end, name, additionalGenerics);

            if (additionalGenerics != null)
            {
                foreach (GenericParameter item in additionalGenerics)
                    item.parent = ret;
            }

            return ret;
        }

        public GenericParameter[] parseGenericParameterList()
        {
            List<GenericParameter> ret = null;
            while (true)
            {
                GenericParameter node = parseGenericParameter();
                if (node == null)
                    break;

                if (ret == null)
                    ret = new List<GenericParameter>();

                ret.Add(node);
            }

            if (ret != null)
                return ret.ToArray();
            else
                return null;
        }

        public GenericParameter parseGenericParameter()
        {
            Position start = lexer.getPreviousPosition();

            bool successComma1 = lexer.parsePunctuation(comma);
            if (successComma1)
                lexer.advance();
            else
                return null;

            string name = lexer.parseIdentifier();
            if ((name != null) && isIdentifier(name))
                lexer.advance();
            else
                return null;

            Position end = lexer.getPosition();

            GenericParameter ret = new GenericParameter(start, end, name);


            return ret;
        }

        public Extends parseExtends()
        {
            Position start = lexer.getPreviousPosition();

            bool successExtends1 = lexer.parseKeyword(extendsKeyword);
            if (successExtends1)
                lexer.advance();
            else
                return null;

            Name name = parseName();
            if (name == null)
                return null;

            Position end = lexer.getPosition();

            Extends ret = new Extends(start, end, name);

            name.parent = ret;

            return ret;
        }

        public Object parseObject()
        {
            Position start = lexer.getPreviousPosition();

            bool successLeftParen1 = lexer.parsePunctuation(leftParen);
            if (successLeftParen1)
                lexer.advance();
            else
                return null;

            Component[] components = parseComponentList();

            bool successRightParen3 = lexer.parsePunctuation(rightParen);
            if (successRightParen3)
                lexer.advance();
            else
                return null;

            Position end = lexer.getPosition();

            Object ret = new Object(start, end, components);

            if (components != null)
            {
                foreach (Component item in components)
                    item.parent = ret;
            }

            return ret;
        }

        public Component[] parseComponentList()
        {
            List<Component> ret = null;
            while (true)
            {
                Component node = parseComponent();
                if (node == null)
                    break;

                if (ret == null)
                    ret = new List<Component>();

                ret.Add(node);
            }

            if (ret != null)
                return ret.ToArray();
            else
                return null;
        }

        public Component parseComponent()
        {
            Position start = lexer.getPreviousPosition();

            string name = lexer.parseIdentifier();
            if ((name != null) && isIdentifier(name))
                lexer.advance();
            else
                return null;

            TypeAnnotation typeAnnotation = parseTypeAnnotation();

            bool successComma3 = lexer.parsePunctuation(comma);
            if (successComma3)
                lexer.advance();

            Position end = lexer.getPosition();

            Component ret = new Component(start, end, name, typeAnnotation);

            if (typeAnnotation != null)
                typeAnnotation.parent = ret;

            return ret;
        }

        public Function parseFunction()
        {
            Position start = lexer.getPreviousPosition();

            bool successFunction1 = lexer.parseKeyword(functionKeyword);
            if (successFunction1)
                lexer.advance();
            else
                return null;

            Procedure procedure = parseProcedure();
            if (procedure == null)
                return null;

            Position end = lexer.getPosition();

            Function ret = new Function(start, end, procedure);

            procedure.parent = ret;

            return ret;
        }

        public Method parseMethod()
        {
            Position start = lexer.getPreviousPosition();

            bool successMethod1 = lexer.parseKeyword(methodKeyword);
            if (successMethod1)
                lexer.advance();
            else
                return null;

            Procedure procedure = parseProcedure();
            if (procedure == null)
                return null;

            Position end = lexer.getPosition();

            Method ret = new Method(start, end, procedure);

            procedure.parent = ret;

            return ret;
        }

        public Procedure parseProcedure()
        {
            Position start = lexer.getPreviousPosition();

            string name = lexer.parseIdentifier();
            if ((name != null) && isIdentifier(name))
                lexer.advance();
            else
                return null;

            Object input = parseObject();

            TypeAnnotation output = parseTypeAnnotation();

            Expression[] body = parseExpressionList();

            Position end = lexer.getPosition();

            Procedure ret = new Procedure(start, end, name, input, output, body);

            if (input != null)
                input.parent = ret;
            if (output != null)
                output.parent = ret;
            if (body != null)
            {
                foreach (Expression item in body)
                    item.parent = ret;
            }

            return ret;
        }

        public TypeAnnotation parseTypeAnnotation()
        {
            Position start = lexer.getPreviousPosition();

            bool successColon1 = lexer.parsePunctuation(colon);
            if (successColon1)
                lexer.advance();
            else
                return null;

            Type typeSpec = parseType();
            if (typeSpec == null)
                return null;

            Position end = lexer.getPosition();

            TypeAnnotation ret = new TypeAnnotation(start, end, typeSpec);

            typeSpec.parent = ret;

            return ret;
        }

        public Type parseType()
        {
            Position start = lexer.getPreviousPosition();

            string name = lexer.parseIdentifier();
            if ((name != null) && isIdentifier(name))
                lexer.advance();
            else
                return null;

            GenericArguments generics = parseGenericArguments();

            TypePostfix[] postfixes = parseTypePostfixList();

            LifeTime lifeTime = parseLifeTime();

            Position end = lexer.getPosition();

            Type ret = new Type(start, end, name, generics, postfixes, lifeTime);

            if (generics != null)
                generics.parent = ret;
            if (postfixes != null)
            {
                foreach (TypePostfix item in postfixes)
                    item.parent = ret;
            }
            if (lifeTime != null)
                lifeTime.parent = ret;

            return ret;
        }

        public GenericArguments parseGenericArguments()
        {
            Position start = lexer.getPreviousPosition();

            bool successBacktick1 = lexer.parsePunctuation(backtick);
            if (successBacktick1)
                lexer.advance();
            else
                return null;

            Type typeSpec = parseType();
            if (typeSpec == null)
                return null;

            GenericArgument[] additionalGenerics = parseGenericArgumentList();

            bool successBacktick4 = lexer.parsePunctuation(backtick);
            if (successBacktick4)
                lexer.advance();
            else
                return null;

            Position end = lexer.getPosition();

            GenericArguments ret = new GenericArguments(start, end, typeSpec, additionalGenerics);

            typeSpec.parent = ret;
            if (additionalGenerics != null)
            {
                foreach (GenericArgument item in additionalGenerics)
                    item.parent = ret;
            }

            return ret;
        }

        public GenericArgument[] parseGenericArgumentList()
        {
            List<GenericArgument> ret = null;
            while (true)
            {
                GenericArgument node = parseGenericArgument();
                if (node == null)
                    break;

                if (ret == null)
                    ret = new List<GenericArgument>();

                ret.Add(node);
            }

            if (ret != null)
                return ret.ToArray();
            else
                return null;
        }

        public GenericArgument parseGenericArgument()
        {
            Position start = lexer.getPreviousPosition();

            bool successComma1 = lexer.parsePunctuation(comma);
            if (successComma1)
                lexer.advance();
            else
                return null;

            Type typeSpec = parseType();
            if (typeSpec == null)
                return null;

            Position end = lexer.getPosition();

            GenericArgument ret = new GenericArgument(start, end, typeSpec);

            typeSpec.parent = ret;

            return ret;
        }

        public TypePostfix[] parseTypePostfixList()
        {
            List<TypePostfix> ret = null;
            while (true)
            {
                TypePostfix node = parseTypePostfix();
                if (node == null)
                    break;

                if (ret == null)
                    ret = new List<TypePostfix>();

                ret.Add(node);
            }

            if (ret != null)
                return ret.ToArray();
            else
                return null;
        }

        public TypePostfix parseTypePostfix()
        {
            {
                Optional node = parseOptional();
                if (node != null)
                    return node;
            }

            {
                IndexedType node = parseIndexedType();
                if (node != null)
                    return node;
            }

            return null;
        }

        public Optional parseOptional()
        {
            Position start = lexer.getPreviousPosition();

            bool successQuestion1 = lexer.parsePunctuation(question);
            if (successQuestion1)
                lexer.advance();
            else
                return null;

            Position end = lexer.getPosition();

            Optional ret = new Optional(start, end);


            return ret;
        }

        public IndexedType parseIndexedType()
        {
            Position start = lexer.getPreviousPosition();

            bool successLeftBracket1 = lexer.parsePunctuation(leftBracket);
            if (successLeftBracket1)
                lexer.advance();
            else
                return null;

            Type typeSpec = parseType();

            bool successRightBracket3 = lexer.parsePunctuation(rightBracket);
            if (successRightBracket3)
                lexer.advance();
            else
                return null;

            Position end = lexer.getPosition();

            IndexedType ret = new IndexedType(start, end, typeSpec);

            if (typeSpec != null)
                typeSpec.parent = ret;

            return ret;
        }

        public LifeTime parseLifeTime()
        {
            {
                Root node = parseRoot();
                if (node != null)
                    return node;
            }

            {
                Local node = parseLocal();
                if (node != null)
                    return node;
            }

            {
                Reference node = parseReference();
                if (node != null)
                    return node;
            }

            {
                Thrown node = parseThrown();
                if (node != null)
                    return node;
            }

            return null;
        }

        public Root parseRoot()
        {
            Position start = lexer.getPreviousPosition();

            bool successDollar1 = lexer.parsePunctuation(dollar);
            if (successDollar1)
                lexer.advance();
            else
                return null;

            Position end = lexer.getPosition();

            Root ret = new Root(start, end);


            return ret;
        }

        public Local parseLocal()
        {
            Position start = lexer.getPreviousPosition();

            bool successAt1 = lexer.parsePunctuation(at);
            if (successAt1)
                lexer.advance();
            else
                return null;

            string location = lexer.parseIdentifier();
            if ((location != null) && isIdentifier(location))
                lexer.advance();
            else
                return null;

            Position end = lexer.getPosition();

            Local ret = new Local(start, end, location);


            return ret;
        }

        public Reference parseReference()
        {
            Position start = lexer.getPreviousPosition();

            bool successHash1 = lexer.parsePunctuation(hash);
            if (successHash1)
                lexer.advance();
            else
                return null;

            Literal age = lexer.parseLiteral();
            if (age != null)
                lexer.advance();

            Position end = lexer.getPosition();

            Reference ret = new Reference(start, end, age);


            return ret;
        }

        public Thrown parseThrown()
        {
            Position start = lexer.getPreviousPosition();

            bool successExclamation1 = lexer.parsePunctuation(exclamation);
            if (successExclamation1)
                lexer.advance();
            else
                return null;

            Position end = lexer.getPosition();

            Thrown ret = new Thrown(start, end);


            return ret;
        }

        bool isAtEnd()
        {
            return lexer.isAtEnd();
        }

        bool isIdentifier(string id)
        {
            if (id == usingKeyword)
                return false;

            if (id == letKeyword)
                return false;

            if (id == mutableKeyword)
                return false;

            if (id == varKeyword)
                return false;

            if (id == threadKeyword)
                return false;

            if (id == setKeyword)
                return false;

            if (id == classKeyword)
                return false;

            if (id == extendsKeyword)
                return false;

            if (id == functionKeyword)
                return false;

            if (id == methodKeyword)
                return false;

            if (id == thisKeyword)
                return false;

            if (id == sizeofKeyword)
                return false;

            if (id == catchKeyword)
                return false;

            if (id == asKeyword)
                return false;

            if (id == isKeyword)
                return false;

            if (id == ifKeyword)
                return false;

            if (id == elseKeyword)
                return false;

            if (id == forKeyword)
                return false;

            if (id == inKeyword)
                return false;

            if (id == whileKeyword)
                return false;

            if (id == doKeyword)
                return false;

            if (id == breakKeyword)
                return false;

            if (id == returnKeyword)
                return false;

            return true;
        }
    }

    public class Visitor
    {
        public virtual bool openProgram(Program theProgram)
        {
            return true;
        }

        public virtual void closeProgram(Program theProgram)
        {
        }

        public virtual bool openFile(File theFile)
        {
            return true;
        }

        public virtual void closeFile(File theFile)
        {
        }

        public virtual bool openBlock(Block theBlock)
        {
            return true;
        }

        public virtual void closeBlock(Block theBlock)
        {
        }

        public virtual bool openUsing(Using theUsing)
        {
            return true;
        }

        public virtual void closeUsing(Using theUsing)
        {
        }

        public virtual bool openLet(Let theLet)
        {
            return true;
        }

        public virtual void closeLet(Let theLet)
        {
        }

        public virtual bool openMutable(Mutable theMutable)
        {
            return true;
        }

        public virtual void closeMutable(Mutable theMutable)
        {
        }

        public virtual bool openVar(Var theVar)
        {
            return true;
        }

        public virtual void closeVar(Var theVar)
        {
        }

        public virtual bool openThread(Thread theThread)
        {
            return true;
        }

        public virtual void closeThread(Thread theThread)
        {
        }

        public virtual bool openBinding(Binding theBinding)
        {
            return true;
        }

        public virtual void closeBinding(Binding theBinding)
        {
        }

        public virtual bool openIdentifierPattern(IdentifierPattern theIdentifierPattern)
        {
            return true;
        }

        public virtual void closeIdentifierPattern(IdentifierPattern theIdentifierPattern)
        {
        }

        public virtual void visitWildcardPattern(WildcardPattern theWildcardPattern)
        {
        }

        public virtual bool openExpression(Expression theExpression)
        {
            return true;
        }

        public virtual void closeExpression(Expression theExpression)
        {
        }

        public virtual bool openNameExpression(NameExpression theNameExpression)
        {
            return true;
        }

        public virtual void closeNameExpression(NameExpression theNameExpression)
        {
        }

        public virtual void visitLiteralExpression(LiteralExpression theLiteralExpression)
        {
        }

        public virtual bool openIf(If theIf)
        {
            return true;
        }

        public virtual void closeIf(If theIf)
        {
        }

        public virtual bool openElse(Else theElse)
        {
            return true;
        }

        public virtual void closeElse(Else theElse)
        {
        }

        public virtual bool openFor(For theFor)
        {
            return true;
        }

        public virtual void closeFor(For theFor)
        {
        }

        public virtual bool openWhile(While theWhile)
        {
            return true;
        }

        public virtual void closeWhile(While theWhile)
        {
        }

        public virtual bool openDo(Do theDo)
        {
            return true;
        }

        public virtual void closeDo(Do theDo)
        {
        }

        public virtual void visitThis(This theThis)
        {
        }

        public virtual void visitCatch(Catch theCatch)
        {
        }

        public virtual void visitMemberAccess(MemberAccess theMemberAccess)
        {
        }

        public virtual bool openSubscript(Subscript theSubscript)
        {
            return true;
        }

        public virtual void closeSubscript(Subscript theSubscript)
        {
        }

        public virtual bool openAs(As theAs)
        {
            return true;
        }

        public virtual void closeAs(As theAs)
        {
        }

        public virtual bool openIs(Is theIs)
        {
            return true;
        }

        public virtual void closeIs(Is theIs)
        {
        }

        public virtual bool openObjectExpression(ObjectExpression theObjectExpression)
        {
            return true;
        }

        public virtual void closeObjectExpression(ObjectExpression theObjectExpression)
        {
        }

        public virtual bool openObjectItem(ObjectItem theObjectItem)
        {
            return true;
        }

        public virtual void closeObjectItem(ObjectItem theObjectItem)
        {
        }

        public virtual bool openSizeOf(SizeOf theSizeOf)
        {
            return true;
        }

        public virtual void closeSizeOf(SizeOf theSizeOf)
        {
        }

        public virtual bool openSet(Set theSet)
        {
            return true;
        }

        public virtual void closeSet(Set theSet)
        {
        }

        public virtual bool openBreak(Break theBreak)
        {
            return true;
        }

        public virtual void closeBreak(Break theBreak)
        {
        }

        public virtual bool openReturn(Return theReturn)
        {
            return true;
        }

        public virtual void closeReturn(Return theReturn)
        {
        }

        public virtual bool openClass(Class theClass)
        {
            return true;
        }

        public virtual void closeClass(Class theClass)
        {
        }

        public virtual bool openName(Name theName)
        {
            return true;
        }

        public virtual void closeName(Name theName)
        {
        }

        public virtual void visitExtension(Extension theExtension)
        {
        }

        public virtual bool openGenericParameters(GenericParameters theGenericParameters)
        {
            return true;
        }

        public virtual void closeGenericParameters(GenericParameters theGenericParameters)
        {
        }

        public virtual void visitGenericParameter(GenericParameter theGenericParameter)
        {
        }

        public virtual bool openExtends(Extends theExtends)
        {
            return true;
        }

        public virtual void closeExtends(Extends theExtends)
        {
        }

        public virtual bool openObject(Object theObject)
        {
            return true;
        }

        public virtual void closeObject(Object theObject)
        {
        }

        public virtual bool openComponent(Component theComponent)
        {
            return true;
        }

        public virtual void closeComponent(Component theComponent)
        {
        }

        public virtual bool openFunction(Function theFunction)
        {
            return true;
        }

        public virtual void closeFunction(Function theFunction)
        {
        }

        public virtual bool openMethod(Method theMethod)
        {
            return true;
        }

        public virtual void closeMethod(Method theMethod)
        {
        }

        public virtual bool openProcedure(Procedure theProcedure)
        {
            return true;
        }

        public virtual void closeProcedure(Procedure theProcedure)
        {
        }

        public virtual bool openTypeAnnotation(TypeAnnotation theTypeAnnotation)
        {
            return true;
        }

        public virtual void closeTypeAnnotation(TypeAnnotation theTypeAnnotation)
        {
        }

        public virtual bool openType(Type theType)
        {
            return true;
        }

        public virtual void closeType(Type theType)
        {
        }

        public virtual bool openGenericArguments(GenericArguments theGenericArguments)
        {
            return true;
        }

        public virtual void closeGenericArguments(GenericArguments theGenericArguments)
        {
        }

        public virtual bool openGenericArgument(GenericArgument theGenericArgument)
        {
            return true;
        }

        public virtual void closeGenericArgument(GenericArgument theGenericArgument)
        {
        }

        public virtual void visitOptional(Optional theOptional)
        {
        }

        public virtual bool openIndexedType(IndexedType theIndexedType)
        {
            return true;
        }

        public virtual void closeIndexedType(IndexedType theIndexedType)
        {
        }

        public virtual void visitRoot(Root theRoot)
        {
        }

        public virtual void visitLocal(Local theLocal)
        {
        }

        public virtual void visitReference(Reference theReference)
        {
        }

        public virtual void visitThrown(Thrown theThrown)
        {
        }
}

    public abstract class SyntaxNode
    {
        public Position start;
        public Position end;
        public SyntaxNode parent;

        public abstract void accept(Visitor visitor);
    }

    public class Program : SyntaxNode
    {
        public string name;
        public File[] files;
        public Program(string name, File[] files)
        {
            start = new Position(0, 0);
            end = new Position(0, 0);
            this.name = name;
            this.files = files;
            this.parent = null;
        }

        public override void accept(Visitor visitor)
        {
            if (!visitor.openProgram(this))
                return;

            if (files != null)
            {
                foreach (File node in files)
                    node.accept(visitor);
            }
            visitor.closeProgram(this);
        }
    }

    public class File : SyntaxNode
    {
        public Statement[] statements;
        public File(Position start, Position end, Statement[] statements)
        {
            this.start = start;
            this.end = end;
            this.statements = statements;
        }

        public override void accept(Visitor visitor)
        {
            if (!visitor.openFile(this))
                return;

            if (statements != null)
            {
                foreach (Statement node in statements)
                    node.accept(visitor);
            }
            visitor.closeFile(this);
        }
    }

    public class Block : PrimaryExpression
    {
        public Statement[] statements;
        public Block(Position start, Position end, Statement[] statements)
        {
            this.start = start;
            this.end = end;
            this.statements = statements;
        }

        public override void accept(Visitor visitor)
        {
            if (!visitor.openBlock(this))
                return;

            if (statements != null)
            {
                foreach (Statement node in statements)
                    node.accept(visitor);
            }
            visitor.closeBlock(this);
        }
    }

    public class Statement : SyntaxNode
    {
        public override void accept(Visitor visitor)
        {
        }
    }

    public class Using : Statement
    {
        public Name name;
        public Using(Position start, Position end, Name name)
        {
            this.start = start;
            this.end = end;
            this.name = name;
        }

        public override void accept(Visitor visitor)
        {
            if (!visitor.openUsing(this))
                return;

        name.accept(visitor);
            visitor.closeUsing(this);
        }
    }

    public class Declaration : Statement
    {
        public override void accept(Visitor visitor)
        {
        }
    }

    public class Let : Declaration
    {
        public Binding binding;
        public Let(Position start, Position end, Binding binding)
        {
            this.start = start;
            this.end = end;
            this.binding = binding;
        }

        public override void accept(Visitor visitor)
        {
            if (!visitor.openLet(this))
                return;

        binding.accept(visitor);
            visitor.closeLet(this);
        }
    }

    public class Mutable : Declaration
    {
        public Binding binding;
        public Mutable(Position start, Position end, Binding binding)
        {
            this.start = start;
            this.end = end;
            this.binding = binding;
        }

        public override void accept(Visitor visitor)
        {
            if (!visitor.openMutable(this))
                return;

        binding.accept(visitor);
            visitor.closeMutable(this);
        }
    }

    public class Var : Declaration
    {
        public Binding binding;
        public Var(Position start, Position end, Binding binding)
        {
            this.start = start;
            this.end = end;
            this.binding = binding;
        }

        public override void accept(Visitor visitor)
        {
            if (!visitor.openVar(this))
                return;

        binding.accept(visitor);
            visitor.closeVar(this);
        }
    }

    public class Thread : Declaration
    {
        public Binding binding;
        public Thread(Position start, Position end, Binding binding)
        {
            this.start = start;
            this.end = end;
            this.binding = binding;
        }

        public override void accept(Visitor visitor)
        {
            if (!visitor.openThread(this))
                return;

        binding.accept(visitor);
            visitor.closeThread(this);
        }
    }

    public class Binding : SyntaxNode
    {
        public Pattern pattern;
        public TypeAnnotation typeAnnotation;
        public Expression[] expressions;
        public Binding(Position start, Position end, Pattern pattern, TypeAnnotation typeAnnotation, Expression[] expressions)
        {
            this.start = start;
            this.end = end;
            this.pattern = pattern;
            this.typeAnnotation = typeAnnotation;
            this.expressions = expressions;
        }

        public override void accept(Visitor visitor)
        {
            if (!visitor.openBinding(this))
                return;

        pattern.accept(visitor);
        if (typeAnnotation != null)
            typeAnnotation.accept(visitor);
            if (expressions != null)
            {
                foreach (Expression node in expressions)
                    node.accept(visitor);
            }
            visitor.closeBinding(this);
        }
    }

    public class Pattern : SyntaxNode
    {
        public override void accept(Visitor visitor)
        {
        }
    }

    public class IdentifierPattern : Pattern
    {
        public Name name;
        public TypeAnnotation annotationForType;
        public IdentifierPattern(Position start, Position end, Name name, TypeAnnotation annotationForType)
        {
            this.start = start;
            this.end = end;
            this.name = name;
            this.annotationForType = annotationForType;
        }

        public override void accept(Visitor visitor)
        {
            if (!visitor.openIdentifierPattern(this))
                return;

        name.accept(visitor);
        if (annotationForType != null)
            annotationForType.accept(visitor);
            visitor.closeIdentifierPattern(this);
        }
    }

    public class WildcardPattern : Pattern
    {
        public WildcardPattern(Position start, Position end)
        {
            this.start = start;
            this.end = end;
        }

        public override void accept(Visitor visitor)
        {
            visitor.visitWildcardPattern(this);
        }
    }

    public class Expression : Statement
    {
        public PrimaryExpression primary;
        public Postfix[] postfixes;
        public Expression(Position start, Position end, PrimaryExpression primary, Postfix[] postfixes)
        {
            this.start = start;
            this.end = end;
            this.primary = primary;
            this.postfixes = postfixes;
        }

        public override void accept(Visitor visitor)
        {
            if (!visitor.openExpression(this))
                return;

        primary.accept(visitor);
            if (postfixes != null)
            {
                foreach (Postfix node in postfixes)
                    node.accept(visitor);
            }
            visitor.closeExpression(this);
        }
    }

    public class PrimaryExpression : SyntaxNode
    {
        public override void accept(Visitor visitor)
        {
        }
    }

    public class NameExpression : PrimaryExpression
    {
        public Name name;
        public GenericArguments generics;
        public LifeTime lifeTime;
        public NameExpression(Position start, Position end, Name name, GenericArguments generics, LifeTime lifeTime)
        {
            this.start = start;
            this.end = end;
            this.name = name;
            this.generics = generics;
            this.lifeTime = lifeTime;
        }

        public override void accept(Visitor visitor)
        {
            if (!visitor.openNameExpression(this))
                return;

        name.accept(visitor);
        if (generics != null)
            generics.accept(visitor);
        if (lifeTime != null)
            lifeTime.accept(visitor);
            visitor.closeNameExpression(this);
        }
    }

    public class LiteralExpression : PrimaryExpression
    {
        public Literal literal;
        public LiteralExpression(Position start, Position end, Literal literal)
        {
            this.start = start;
            this.end = end;
            this.literal = literal;
        }

        public override void accept(Visitor visitor)
        {
            visitor.visitLiteralExpression(this);
        }
    }

    public class If : PrimaryExpression
    {
        public Expression condition;
        public Expression consequent;
        public Else elseClause;
        public If(Position start, Position end, Expression condition, Expression consequent, Else elseClause)
        {
            this.start = start;
            this.end = end;
            this.condition = condition;
            this.consequent = consequent;
            this.elseClause = elseClause;
        }

        public override void accept(Visitor visitor)
        {
            if (!visitor.openIf(this))
                return;

        condition.accept(visitor);
        consequent.accept(visitor);
        if (elseClause != null)
            elseClause.accept(visitor);
            visitor.closeIf(this);
        }
    }

    public class Else : SyntaxNode
    {
        public Expression alternative;
        public Else(Position start, Position end, Expression alternative)
        {
            this.start = start;
            this.end = end;
            this.alternative = alternative;
        }

        public override void accept(Visitor visitor)
        {
            if (!visitor.openElse(this))
                return;

        alternative.accept(visitor);
            visitor.closeElse(this);
        }
    }

    public class For : PrimaryExpression
    {
        public string index;
        public TypeAnnotation typeAnnotation;
        public Expression expression;
        public Expression code;
        public For(Position start, Position end, string index, TypeAnnotation typeAnnotation, Expression expression, Expression code)
        {
            this.start = start;
            this.end = end;
            this.index = index;
            this.typeAnnotation = typeAnnotation;
            this.expression = expression;
            this.code = code;
        }

        public override void accept(Visitor visitor)
        {
            if (!visitor.openFor(this))
                return;

        if (typeAnnotation != null)
            typeAnnotation.accept(visitor);
        expression.accept(visitor);
        code.accept(visitor);
            visitor.closeFor(this);
        }
    }

    public class While : PrimaryExpression
    {
        public Expression condition;
        public Expression code;
        public While(Position start, Position end, Expression condition, Expression code)
        {
            this.start = start;
            this.end = end;
            this.condition = condition;
            this.code = code;
        }

        public override void accept(Visitor visitor)
        {
            if (!visitor.openWhile(this))
                return;

        condition.accept(visitor);
        code.accept(visitor);
            visitor.closeWhile(this);
        }
    }

    public class Do : PrimaryExpression
    {
        public Expression code;
        public Expression condition;
        public Do(Position start, Position end, Expression code, Expression condition)
        {
            this.start = start;
            this.end = end;
            this.code = code;
            this.condition = condition;
        }

        public override void accept(Visitor visitor)
        {
            if (!visitor.openDo(this))
                return;

        code.accept(visitor);
        condition.accept(visitor);
            visitor.closeDo(this);
        }
    }

    public class This : PrimaryExpression
    {
        public This(Position start, Position end)
        {
            this.start = start;
            this.end = end;
        }

        public override void accept(Visitor visitor)
        {
            visitor.visitThis(this);
        }
    }

    public class Postfix : SyntaxNode
    {
        public override void accept(Visitor visitor)
        {
        }
    }

    public class Catch : Postfix
    {
        public Catch(Position start, Position end)
        {
            this.start = start;
            this.end = end;
        }

        public override void accept(Visitor visitor)
        {
            visitor.visitCatch(this);
        }
    }

    public class MemberAccess : Postfix
    {
        public string member;
        public MemberAccess(Position start, Position end, string member)
        {
            this.start = start;
            this.end = end;
            this.member = member;
        }

        public override void accept(Visitor visitor)
        {
            visitor.visitMemberAccess(this);
        }
    }

    public class Subscript : Postfix
    {
        public Expression[] firstItems;
        public ObjectItem[] additionalItemses;
        public Subscript(Position start, Position end, Expression[] firstItems, ObjectItem[] additionalItemses)
        {
            this.start = start;
            this.end = end;
            this.firstItems = firstItems;
            this.additionalItemses = additionalItemses;
        }

        public override void accept(Visitor visitor)
        {
            if (!visitor.openSubscript(this))
                return;

            if (firstItems != null)
            {
                foreach (Expression node in firstItems)
                    node.accept(visitor);
            }
            if (additionalItemses != null)
            {
                foreach (ObjectItem node in additionalItemses)
                    node.accept(visitor);
            }
            visitor.closeSubscript(this);
        }
    }

    public class As : Postfix
    {
        public Type typeSpec;
        public As(Position start, Position end, Type typeSpec)
        {
            this.start = start;
            this.end = end;
            this.typeSpec = typeSpec;
        }

        public override void accept(Visitor visitor)
        {
            if (!visitor.openAs(this))
                return;

        typeSpec.accept(visitor);
            visitor.closeAs(this);
        }
    }

    public class Is : Postfix
    {
        public Type typeSpec;
        public Is(Position start, Position end, Type typeSpec)
        {
            this.start = start;
            this.end = end;
            this.typeSpec = typeSpec;
        }

        public override void accept(Visitor visitor)
        {
            if (!visitor.openIs(this))
                return;

        typeSpec.accept(visitor);
            visitor.closeIs(this);
        }
    }

    public class ObjectExpression : PrimaryExpression
    {
        public Expression[] firstItems;
        public ObjectItem[] additionalItemses;
        public ObjectExpression(Position start, Position end, Expression[] firstItems, ObjectItem[] additionalItemses)
        {
            this.start = start;
            this.end = end;
            this.firstItems = firstItems;
            this.additionalItemses = additionalItemses;
        }

        public override void accept(Visitor visitor)
        {
            if (!visitor.openObjectExpression(this))
                return;

            if (firstItems != null)
            {
                foreach (Expression node in firstItems)
                    node.accept(visitor);
            }
            if (additionalItemses != null)
            {
                foreach (ObjectItem node in additionalItemses)
                    node.accept(visitor);
            }
            visitor.closeObjectExpression(this);
        }
    }

    public class ObjectItem : SyntaxNode
    {
        public Expression[] expression;
        public ObjectItem(Position start, Position end, Expression[] expression)
        {
            this.start = start;
            this.end = end;
            this.expression = expression;
        }

        public override void accept(Visitor visitor)
        {
            if (!visitor.openObjectItem(this))
                return;

            if (expression != null)
            {
                foreach (Expression node in expression)
                    node.accept(visitor);
            }
            visitor.closeObjectItem(this);
        }
    }

    public class SizeOf : PrimaryExpression
    {
        public Type typeSpec;
        public SizeOf(Position start, Position end, Type typeSpec)
        {
            this.start = start;
            this.end = end;
            this.typeSpec = typeSpec;
        }

        public override void accept(Visitor visitor)
        {
            if (!visitor.openSizeOf(this))
                return;

        typeSpec.accept(visitor);
            visitor.closeSizeOf(this);
        }
    }

    public class Set : Statement
    {
        public Expression[] lValue;
        public Expression[] rValue;
        public Set(Position start, Position end, Expression[] lValue, Expression[] rValue)
        {
            this.start = start;
            this.end = end;
            this.lValue = lValue;
            this.rValue = rValue;
        }

        public override void accept(Visitor visitor)
        {
            if (!visitor.openSet(this))
                return;

            if (lValue != null)
            {
                foreach (Expression node in lValue)
                    node.accept(visitor);
            }
            if (rValue != null)
            {
                foreach (Expression node in rValue)
                    node.accept(visitor);
            }
            visitor.closeSet(this);
        }
    }

    public class Break : Statement
    {
        public Expression[] lValue;
        public Break(Position start, Position end, Expression[] lValue)
        {
            this.start = start;
            this.end = end;
            this.lValue = lValue;
        }

        public override void accept(Visitor visitor)
        {
            if (!visitor.openBreak(this))
                return;

            if (lValue != null)
            {
                foreach (Expression node in lValue)
                    node.accept(visitor);
            }
            visitor.closeBreak(this);
        }
    }

    public class Return : Statement
    {
        public Expression[] lValue;
        public Return(Position start, Position end, Expression[] lValue)
        {
            this.start = start;
            this.end = end;
            this.lValue = lValue;
        }

        public override void accept(Visitor visitor)
        {
            if (!visitor.openReturn(this))
                return;

            if (lValue != null)
            {
                foreach (Expression node in lValue)
                    node.accept(visitor);
            }
            visitor.closeReturn(this);
        }
    }

    public class Class : Declaration
    {
        public Name name;
        public GenericParameters generics;
        public Extends baseClass;
        public Object contents;
        public Expression body;
        public Class(Position start, Position end, Name name, GenericParameters generics, Extends baseClass, Object contents, Expression body)
        {
            this.start = start;
            this.end = end;
            this.name = name;
            this.generics = generics;
            this.baseClass = baseClass;
            this.contents = contents;
            this.body = body;
        }

        public override void accept(Visitor visitor)
        {
            if (!visitor.openClass(this))
                return;

        name.accept(visitor);
        if (generics != null)
            generics.accept(visitor);
        if (baseClass != null)
            baseClass.accept(visitor);
        if (contents != null)
            contents.accept(visitor);
        if (body != null)
            body.accept(visitor);
            visitor.closeClass(this);
        }
    }

    public class Name : SyntaxNode
    {
        public string name;
        public Extension[] extensions;
        public Name(Position start, Position end, string name, Extension[] extensions)
        {
            this.start = start;
            this.end = end;
            this.name = name;
            this.extensions = extensions;
        }

        public override void accept(Visitor visitor)
        {
            if (!visitor.openName(this))
                return;

            if (extensions != null)
            {
                foreach (Extension node in extensions)
                    node.accept(visitor);
            }
            visitor.closeName(this);
        }
    }

    public class Extension : SyntaxNode
    {
        public string name;
        public Extension(Position start, Position end, string name)
        {
            this.start = start;
            this.end = end;
            this.name = name;
        }

        public override void accept(Visitor visitor)
        {
            visitor.visitExtension(this);
        }
    }

    public class GenericParameters : SyntaxNode
    {
        public string name;
        public GenericParameter[] additionalGenerics;
        public GenericParameters(Position start, Position end, string name, GenericParameter[] additionalGenerics)
        {
            this.start = start;
            this.end = end;
            this.name = name;
            this.additionalGenerics = additionalGenerics;
        }

        public override void accept(Visitor visitor)
        {
            if (!visitor.openGenericParameters(this))
                return;

            if (additionalGenerics != null)
            {
                foreach (GenericParameter node in additionalGenerics)
                    node.accept(visitor);
            }
            visitor.closeGenericParameters(this);
        }
    }

    public class GenericParameter : SyntaxNode
    {
        public string name;
        public GenericParameter(Position start, Position end, string name)
        {
            this.start = start;
            this.end = end;
            this.name = name;
        }

        public override void accept(Visitor visitor)
        {
            visitor.visitGenericParameter(this);
        }
    }

    public class Extends : SyntaxNode
    {
        public Name name;
        public Extends(Position start, Position end, Name name)
        {
            this.start = start;
            this.end = end;
            this.name = name;
        }

        public override void accept(Visitor visitor)
        {
            if (!visitor.openExtends(this))
                return;

        name.accept(visitor);
            visitor.closeExtends(this);
        }
    }

    public class Object : SyntaxNode
    {
        public Component[] components;
        public Object(Position start, Position end, Component[] components)
        {
            this.start = start;
            this.end = end;
            this.components = components;
        }

        public override void accept(Visitor visitor)
        {
            if (!visitor.openObject(this))
                return;

            if (components != null)
            {
                foreach (Component node in components)
                    node.accept(visitor);
            }
            visitor.closeObject(this);
        }
    }

    public class Component : SyntaxNode
    {
        public string name;
        public TypeAnnotation typeAnnotation;
        public Component(Position start, Position end, string name, TypeAnnotation typeAnnotation)
        {
            this.start = start;
            this.end = end;
            this.name = name;
            this.typeAnnotation = typeAnnotation;
        }

        public override void accept(Visitor visitor)
        {
            if (!visitor.openComponent(this))
                return;

        if (typeAnnotation != null)
            typeAnnotation.accept(visitor);
            visitor.closeComponent(this);
        }
    }

    public class Function : Declaration
    {
        public Procedure procedure;
        public Function(Position start, Position end, Procedure procedure)
        {
            this.start = start;
            this.end = end;
            this.procedure = procedure;
        }

        public override void accept(Visitor visitor)
        {
            if (!visitor.openFunction(this))
                return;

        procedure.accept(visitor);
            visitor.closeFunction(this);
        }
    }

    public class Method : Declaration
    {
        public Procedure procedure;
        public Method(Position start, Position end, Procedure procedure)
        {
            this.start = start;
            this.end = end;
            this.procedure = procedure;
        }

        public override void accept(Visitor visitor)
        {
            if (!visitor.openMethod(this))
                return;

        procedure.accept(visitor);
            visitor.closeMethod(this);
        }
    }

    public class Procedure : SyntaxNode
    {
        public string name;
        public Object input;
        public TypeAnnotation output;
        public Expression[] body;
        public Procedure(Position start, Position end, string name, Object input, TypeAnnotation output, Expression[] body)
        {
            this.start = start;
            this.end = end;
            this.name = name;
            this.input = input;
            this.output = output;
            this.body = body;
        }

        public override void accept(Visitor visitor)
        {
            if (!visitor.openProcedure(this))
                return;

        if (input != null)
            input.accept(visitor);
        if (output != null)
            output.accept(visitor);
            if (body != null)
            {
                foreach (Expression node in body)
                    node.accept(visitor);
            }
            visitor.closeProcedure(this);
        }
    }

    public class TypeAnnotation : SyntaxNode
    {
        public Type typeSpec;
        public TypeAnnotation(Position start, Position end, Type typeSpec)
        {
            this.start = start;
            this.end = end;
            this.typeSpec = typeSpec;
        }

        public override void accept(Visitor visitor)
        {
            if (!visitor.openTypeAnnotation(this))
                return;

        typeSpec.accept(visitor);
            visitor.closeTypeAnnotation(this);
        }
    }

    public class Type : SyntaxNode
    {
        public string name;
        public GenericArguments generics;
        public TypePostfix[] postfixes;
        public LifeTime lifeTime;
        public Type(Position start, Position end, string name, GenericArguments generics, TypePostfix[] postfixes, LifeTime lifeTime)
        {
            this.start = start;
            this.end = end;
            this.name = name;
            this.generics = generics;
            this.postfixes = postfixes;
            this.lifeTime = lifeTime;
        }

        public override void accept(Visitor visitor)
        {
            if (!visitor.openType(this))
                return;

        if (generics != null)
            generics.accept(visitor);
            if (postfixes != null)
            {
                foreach (TypePostfix node in postfixes)
                    node.accept(visitor);
            }
        if (lifeTime != null)
            lifeTime.accept(visitor);
            visitor.closeType(this);
        }
    }

    public class GenericArguments : SyntaxNode
    {
        public Type typeSpec;
        public GenericArgument[] additionalGenerics;
        public GenericArguments(Position start, Position end, Type typeSpec, GenericArgument[] additionalGenerics)
        {
            this.start = start;
            this.end = end;
            this.typeSpec = typeSpec;
            this.additionalGenerics = additionalGenerics;
        }

        public override void accept(Visitor visitor)
        {
            if (!visitor.openGenericArguments(this))
                return;

        typeSpec.accept(visitor);
            if (additionalGenerics != null)
            {
                foreach (GenericArgument node in additionalGenerics)
                    node.accept(visitor);
            }
            visitor.closeGenericArguments(this);
        }
    }

    public class GenericArgument : SyntaxNode
    {
        public Type typeSpec;
        public GenericArgument(Position start, Position end, Type typeSpec)
        {
            this.start = start;
            this.end = end;
            this.typeSpec = typeSpec;
        }

        public override void accept(Visitor visitor)
        {
            if (!visitor.openGenericArgument(this))
                return;

        typeSpec.accept(visitor);
            visitor.closeGenericArgument(this);
        }
    }

    public class TypePostfix : SyntaxNode
    {
        public override void accept(Visitor visitor)
        {
        }
    }

    public class Optional : TypePostfix
    {
        public Optional(Position start, Position end)
        {
            this.start = start;
            this.end = end;
        }

        public override void accept(Visitor visitor)
        {
            visitor.visitOptional(this);
        }
    }

    public class IndexedType : TypePostfix
    {
        public Type typeSpec;
        public IndexedType(Position start, Position end, Type typeSpec)
        {
            this.start = start;
            this.end = end;
            this.typeSpec = typeSpec;
        }

        public override void accept(Visitor visitor)
        {
            if (!visitor.openIndexedType(this))
                return;

        if (typeSpec != null)
            typeSpec.accept(visitor);
            visitor.closeIndexedType(this);
        }
    }

    public class LifeTime : SyntaxNode
    {
        public override void accept(Visitor visitor)
        {
        }
    }

    public class Root : LifeTime
    {
        public Root(Position start, Position end)
        {
            this.start = start;
            this.end = end;
        }

        public override void accept(Visitor visitor)
        {
            visitor.visitRoot(this);
        }
    }

    public class Local : LifeTime
    {
        public string location;
        public Local(Position start, Position end, string location)
        {
            this.start = start;
            this.end = end;
            this.location = location;
        }

        public override void accept(Visitor visitor)
        {
            visitor.visitLocal(this);
        }
    }

    public class Reference : LifeTime
    {
        public Literal age;
        public Reference(Position start, Position end, Literal age)
        {
            this.start = start;
            this.end = end;
            this.age = age;
        }

        public override void accept(Visitor visitor)
        {
            visitor.visitReference(this);
        }
    }

    public class Thrown : LifeTime
    {
        public Thrown(Position start, Position end)
        {
            this.start = start;
            this.end = end;
        }

        public override void accept(Visitor visitor)
        {
            visitor.visitThrown(this);
        }
    }
}
