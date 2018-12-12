using System;
using System.Collections.Generic;
using System.Text;

namespace scalyc
{
    class Parser
    {
        Lexer lexer;
        string fileName;
        HashSet<string> keywords = new HashSet<string>(new string[] {
            "using",
            "namespace",
            "typedef",
            "let",
            "mutable",
            "threadlocal",
            "var",
            "set",
            "class",
            "extends",
            "initializer",
            "allocator",
            "method",
            "function",
            "operator",
            "this",
            "new",
            "sizeof",
            "catch",
            "throws",
            "as",
            "is",
            "if",
            "else",
            "switch",
            "case",
            "default",
            "for",
            "in",
            "while",
            "do",
            "loop",
            "break",
            "continue",
            "return",
            "throw",
            "intrinsic",
            "define",
       });

        public Parser(string theFileName, string text)
        {
            lexer = new Lexer(text);
            fileName = theFileName;
        }

        public FileSyntax parseFile()
        {
            Position start = lexer.getPreviousPosition();

            IntrinsicSyntax[] intrinsics = parseIntrinsicList();

            UsingSyntax[] usings = parseUsingList();

            DefineSyntax[] defines = parseDefineList();

            DeclarationSyntax[] declarations = parseDeclarationList();

            StatementSyntax[] statements = parseStatementList();
            if (statements != null)
            {
                if (!isAtEnd())
                {
                    Position errorPos = lexer.getPreviousPosition();
                    throw new ParserException(fileName, errorPos.line, errorPos.column);
                }
            }

            Position end = lexer.getPosition();

            FileSyntax ret = new FileSyntax(start, end, intrinsics, usings, defines, declarations, statements);

            if (intrinsics != null)
            {
                foreach (IntrinsicSyntax item in intrinsics)
                    item.parent = ret;
            }
            if (usings != null)
            {
                foreach (UsingSyntax item in usings)
                    item.parent = ret;
            }
            if (defines != null)
            {
                foreach (DefineSyntax item in defines)
                    item.parent = ret;
            }
            if (declarations != null)
            {
                foreach (DeclarationSyntax item in declarations)
                    item.parent = ret;
            }
            if (statements != null)
            {
                foreach (StatementSyntax item in statements)
                    item.parent = ret;
            }

            return ret;
        }

        public IntrinsicSyntax[] parseIntrinsicList()
        {
            List<IntrinsicSyntax> ret = null;
            while (true)
            {
                IntrinsicSyntax node = parseIntrinsic();
                if (node == null)
                    break;

                if (ret == null)
                    ret = new List<IntrinsicSyntax>();

                ret.Add(node);
            }

            if (ret != null)
                return ret.ToArray();
            else
                return null;
        }

        public IntrinsicSyntax parseIntrinsic()
        {
            Position start = lexer.getPreviousPosition();

            bool successIntrinsic1 = lexer.parseKeyword("intrinsic");
            if (successIntrinsic1)
                lexer.advance();
            else
                return null;

            string name = lexer.parseIdentifier();
            if ((name != null) && isIdentifier(name))
                lexer.advance();
            else
                throw new ParserException(fileName, lexer.line, lexer.column);

            Position end = lexer.getPosition();

            IntrinsicSyntax ret = new IntrinsicSyntax(start, end, name);


            return ret;
        }

        public UsingSyntax[] parseUsingList()
        {
            List<UsingSyntax> ret = null;
            while (true)
            {
                UsingSyntax node = parseUsing();
                if (node == null)
                    break;

                if (ret == null)
                    ret = new List<UsingSyntax>();

                ret.Add(node);
            }

            if (ret != null)
                return ret.ToArray();
            else
                return null;
        }

        public UsingSyntax parseUsing()
        {
            Position start = lexer.getPreviousPosition();

            bool successUsing1 = lexer.parseKeyword("using");
            if (successUsing1)
                lexer.advance();
            else
                return null;

            NameSyntax name = parseName();
            if (name == null)
                throw new ParserException(fileName, lexer.line, lexer.column);

            Position end = lexer.getPosition();

            UsingSyntax ret = new UsingSyntax(start, end, name);

            name.parent = ret;

            return ret;
        }

        public DefineSyntax[] parseDefineList()
        {
            List<DefineSyntax> ret = null;
            while (true)
            {
                DefineSyntax node = parseDefine();
                if (node == null)
                    break;

                if (ret == null)
                    ret = new List<DefineSyntax>();

                ret.Add(node);
            }

            if (ret != null)
                return ret.ToArray();
            else
                return null;
        }

        public DefineSyntax parseDefine()
        {
            Position start = lexer.getPreviousPosition();

            bool successDefine1 = lexer.parseKeyword("define");
            if (successDefine1)
                lexer.advance();
            else
                return null;

            NameSyntax name = parseName();
            if (name == null)
                throw new ParserException(fileName, lexer.line, lexer.column);

            TypeSyntax typeSpec = parseType();
            if (typeSpec == null)
                throw new ParserException(fileName, lexer.line, lexer.column);

            Position end = lexer.getPosition();

            DefineSyntax ret = new DefineSyntax(start, end, name, typeSpec);

            name.parent = ret;
            typeSpec.parent = ret;

            return ret;
        }

        public NameSyntax parseName()
        {
            Position start = lexer.getPreviousPosition();

            string name = lexer.parseIdentifier();
            if ((name != null) && isIdentifier(name))
                lexer.advance();
            else
                return null;

            ExtensionSyntax[] extensions = parseExtensionList();

            Position end = lexer.getPosition();

            NameSyntax ret = new NameSyntax(start, end, name, extensions);

            if (extensions != null)
            {
                foreach (ExtensionSyntax item in extensions)
                    item.parent = ret;
            }

            return ret;
        }

        public ExtensionSyntax[] parseExtensionList()
        {
            List<ExtensionSyntax> ret = null;
            while (true)
            {
                ExtensionSyntax node = parseExtension();
                if (node == null)
                    break;

                if (ret == null)
                    ret = new List<ExtensionSyntax>();

                ret.Add(node);
            }

            if (ret != null)
                return ret.ToArray();
            else
                return null;
        }

        public ExtensionSyntax parseExtension()
        {
            Position start = lexer.getPreviousPosition();

            bool successDot1 = lexer.parsePunctuation(".");
            if (successDot1)
                lexer.advance();
            else
                return null;

            string name = lexer.parseIdentifier();
            if ((name != null) && isIdentifier(name))
                lexer.advance();
            else
                throw new ParserException(fileName, lexer.line, lexer.column);

            Position end = lexer.getPosition();

            ExtensionSyntax ret = new ExtensionSyntax(start, end, name);


            return ret;
        }

        public DeclarationSyntax[] parseDeclarationList()
        {
            List<DeclarationSyntax> ret = null;
            while (true)
            {
                DeclarationSyntax node = parseDeclaration();
                if (node == null)
                    break;

                if (ret == null)
                    ret = new List<DeclarationSyntax>();

                ret.Add(node);
            }

            if (ret != null)
                return ret.ToArray();
            else
                return null;
        }

        public DeclarationSyntax parseDeclaration()
        {
            {
                NamespaceSyntax node = parseNamespace();
                if (node != null)
                    return node;
            }

            {
                FunctionSyntax node = parseFunction();
                if (node != null)
                    return node;
            }

            {
                ClassSyntax node = parseClass();
                if (node != null)
                    return node;
            }

            {
                LetDeclarationSyntax node = parseLetDeclaration();
                if (node != null)
                    return node;
            }

            {
                VarDeclarationSyntax node = parseVarDeclaration();
                if (node != null)
                    return node;
            }

            {
                MutableDeclarationSyntax node = parseMutableDeclaration();
                if (node != null)
                    return node;
            }

            {
                ThreadLocalDeclarationSyntax node = parseThreadLocalDeclaration();
                if (node != null)
                    return node;
            }

            return null;
        }

        public NamespaceSyntax parseNamespace()
        {
            Position start = lexer.getPreviousPosition();

            bool successNamespace1 = lexer.parseKeyword("namespace");
            if (successNamespace1)
                lexer.advance();
            else
                return null;

            NameSyntax name = parseName();
            if (name == null)
                throw new ParserException(fileName, lexer.line, lexer.column);

            bool successLeftCurly3 = lexer.parsePunctuation("{");
            if (successLeftCurly3)
                lexer.advance();
            else
                throw new ParserException(fileName, lexer.line, lexer.column);

            UsingSyntax[] usings = parseUsingList();

            DefineSyntax[] defines = parseDefineList();

            DeclarationSyntax[] declarations = parseDeclarationList();

            bool successRightCurly7 = lexer.parsePunctuation("}");
            if (successRightCurly7)
                lexer.advance();
            else
                throw new ParserException(fileName, lexer.line, lexer.column);

            Position end = lexer.getPosition();

            NamespaceSyntax ret = new NamespaceSyntax(start, end, name, usings, defines, declarations);

            name.parent = ret;
            if (usings != null)
            {
                foreach (UsingSyntax item in usings)
                    item.parent = ret;
            }
            if (defines != null)
            {
                foreach (DefineSyntax item in defines)
                    item.parent = ret;
            }
            if (declarations != null)
            {
                foreach (DeclarationSyntax item in declarations)
                    item.parent = ret;
            }

            return ret;
        }

        public FunctionSyntax parseFunction()
        {
            Position start = lexer.getPreviousPosition();

            bool successFunction1 = lexer.parseKeyword("function");
            if (successFunction1)
                lexer.advance();
            else
                return null;

            ProcedureSyntax procedure = parseProcedure();
            if (procedure == null)
                throw new ParserException(fileName, lexer.line, lexer.column);

            Position end = lexer.getPosition();

            FunctionSyntax ret = new FunctionSyntax(start, end, procedure);

            procedure.parent = ret;

            return ret;
        }

        public ProcedureSyntax parseProcedure()
        {
            Position start = lexer.getPreviousPosition();

            string name = lexer.parseIdentifier();
            if ((name != null) && isIdentifier(name))
                lexer.advance();
            else
                return null;

            RoutineSyntax routine = parseRoutine();
            if (routine == null)
                throw new ParserException(fileName, lexer.line, lexer.column);

            Position end = lexer.getPosition();

            ProcedureSyntax ret = new ProcedureSyntax(start, end, name, routine);

            routine.parent = ret;

            return ret;
        }

        public RoutineSyntax parseRoutine()
        {
            Position start = lexer.getPreviousPosition();

            StructureSyntax input = parseStructure();

            TypeAnnotationSyntax output = parseTypeAnnotation();

            ThrowsSyntax throwsClause = parseThrows();

            BlockSyntax body = parseBlock();
            if (body == null)
                throw new ParserException(fileName, lexer.line, lexer.column);

            Position end = lexer.getPosition();

            RoutineSyntax ret = new RoutineSyntax(start, end, input, output, throwsClause, body);

            if (input != null)
                input.parent = ret;
            if (output != null)
                output.parent = ret;
            if (throwsClause != null)
                throwsClause.parent = ret;
            body.parent = ret;

            return ret;
        }

        public LetDeclarationSyntax parseLetDeclaration()
        {
            Position start = lexer.getPreviousPosition();

            LetSyntax declaration = parseLet();
            if (declaration == null)
                return null;

            Position end = lexer.getPosition();

            LetDeclarationSyntax ret = new LetDeclarationSyntax(start, end, declaration);

            declaration.parent = ret;

            return ret;
        }

        public VarDeclarationSyntax parseVarDeclaration()
        {
            Position start = lexer.getPreviousPosition();

            VarSyntax declaration = parseVar();
            if (declaration == null)
                return null;

            Position end = lexer.getPosition();

            VarDeclarationSyntax ret = new VarDeclarationSyntax(start, end, declaration);

            declaration.parent = ret;

            return ret;
        }

        public MutableDeclarationSyntax parseMutableDeclaration()
        {
            Position start = lexer.getPreviousPosition();

            MutableSyntax declaration = parseMutable();
            if (declaration == null)
                return null;

            Position end = lexer.getPosition();

            MutableDeclarationSyntax ret = new MutableDeclarationSyntax(start, end, declaration);

            declaration.parent = ret;

            return ret;
        }

        public ThreadLocalDeclarationSyntax parseThreadLocalDeclaration()
        {
            Position start = lexer.getPreviousPosition();

            ThreadLocalSyntax declaration = parseThreadLocal();
            if (declaration == null)
                return null;

            Position end = lexer.getPosition();

            ThreadLocalDeclarationSyntax ret = new ThreadLocalDeclarationSyntax(start, end, declaration);

            declaration.parent = ret;

            return ret;
        }

        public StatementSyntax[] parseStatementList()
        {
            List<StatementSyntax> ret = null;
            while (true)
            {
                StatementSyntax node = parseStatement();
                if (node == null)
                    break;

                if (ret == null)
                    ret = new List<StatementSyntax>();

                ret.Add(node);
            }

            if (ret != null)
                return ret.ToArray();
            else
                return null;
        }

        public StatementSyntax parseStatement()
        {
            {
                LetSyntax node = parseLet();
                if (node != null)
                    return node;
            }

            {
                VarSyntax node = parseVar();
                if (node != null)
                    return node;
            }

            {
                MutableSyntax node = parseMutable();
                if (node != null)
                    return node;
            }

            {
                ThreadLocalSyntax node = parseThreadLocal();
                if (node != null)
                    return node;
            }

            {
                SetSyntax node = parseSet();
                if (node != null)
                    return node;
            }

            {
                CalculationSyntax node = parseCalculation();
                if (node != null)
                    return node;
            }

            {
                BreakSyntax node = parseBreak();
                if (node != null)
                    return node;
            }

            {
                ContinueSyntax node = parseContinue();
                if (node != null)
                    return node;
            }

            {
                ReturnSyntax node = parseReturn();
                if (node != null)
                    return node;
            }

            {
                ThrowSyntax node = parseThrow();
                if (node != null)
                    return node;
            }

            return null;
        }

        public LetSyntax parseLet()
        {
            Position start = lexer.getPreviousPosition();

            bool successLet1 = lexer.parseKeyword("let");
            if (successLet1)
                lexer.advance();
            else
                return null;

            BindingSyntax binding = parseBinding();
            if (binding == null)
                throw new ParserException(fileName, lexer.line, lexer.column);

            Position end = lexer.getPosition();

            LetSyntax ret = new LetSyntax(start, end, binding);

            binding.parent = ret;

            return ret;
        }

        public VarSyntax parseVar()
        {
            Position start = lexer.getPreviousPosition();

            bool successVar1 = lexer.parseKeyword("var");
            if (successVar1)
                lexer.advance();
            else
                return null;

            BindingSyntax binding = parseBinding();
            if (binding == null)
                throw new ParserException(fileName, lexer.line, lexer.column);

            Position end = lexer.getPosition();

            VarSyntax ret = new VarSyntax(start, end, binding);

            binding.parent = ret;

            return ret;
        }

        public MutableSyntax parseMutable()
        {
            Position start = lexer.getPreviousPosition();

            bool successMutable1 = lexer.parseKeyword("mutable");
            if (successMutable1)
                lexer.advance();
            else
                return null;

            BindingSyntax binding = parseBinding();
            if (binding == null)
                throw new ParserException(fileName, lexer.line, lexer.column);

            Position end = lexer.getPosition();

            MutableSyntax ret = new MutableSyntax(start, end, binding);

            binding.parent = ret;

            return ret;
        }

        public ThreadLocalSyntax parseThreadLocal()
        {
            Position start = lexer.getPreviousPosition();

            bool successThreadlocal1 = lexer.parseKeyword("threadlocal");
            if (successThreadlocal1)
                lexer.advance();
            else
                return null;

            BindingSyntax binding = parseBinding();
            if (binding == null)
                throw new ParserException(fileName, lexer.line, lexer.column);

            Position end = lexer.getPosition();

            ThreadLocalSyntax ret = new ThreadLocalSyntax(start, end, binding);

            binding.parent = ret;

            return ret;
        }

        public BindingSyntax parseBinding()
        {
            Position start = lexer.getPreviousPosition();

            string name = lexer.parseIdentifier();
            if ((name != null) && isIdentifier(name))
                lexer.advance();
            else
                return null;

            TypeAnnotationSyntax typeAnnotation = parseTypeAnnotation();

            CalculationSyntax calculation = parseCalculation();
            if (calculation == null)
                throw new ParserException(fileName, lexer.line, lexer.column);

            Position end = lexer.getPosition();

            BindingSyntax ret = new BindingSyntax(start, end, name, typeAnnotation, calculation);

            if (typeAnnotation != null)
                typeAnnotation.parent = ret;
            calculation.parent = ret;

            return ret;
        }

        public SetSyntax parseSet()
        {
            Position start = lexer.getPreviousPosition();

            bool successSet1 = lexer.parseKeyword("set");
            if (successSet1)
                lexer.advance();
            else
                return null;

            OperationSyntax lValue = parseOperation();
            if (lValue == null)
                throw new ParserException(fileName, lexer.line, lexer.column);

            bool successColon3 = lexer.parsePunctuation(":");
            if (successColon3)
                lexer.advance();
            else
                throw new ParserException(fileName, lexer.line, lexer.column);

            CalculationSyntax rValue = parseCalculation();
            if (rValue == null)
                throw new ParserException(fileName, lexer.line, lexer.column);

            Position end = lexer.getPosition();

            SetSyntax ret = new SetSyntax(start, end, lValue, rValue);

            lValue.parent = ret;
            rValue.parent = ret;

            return ret;
        }

        public CalculationSyntax parseCalculation()
        {
            Position start = lexer.getPreviousPosition();

            OperationSyntax operation = parseOperation();
            if (operation == null)
                return null;

            bool successSemicolon2 = lexer.parsePunctuation(";");
            if (successSemicolon2)
                lexer.advance();

            Position end = lexer.getPosition();

            CalculationSyntax ret = new CalculationSyntax(start, end, operation);

            operation.parent = ret;

            return ret;
        }

        public OperationSyntax parseOperation()
        {
            Position start = lexer.getPreviousPosition();

            OperandSyntax[] op = parseOperandList();
            if (op == null)
                return null;

            Position end = lexer.getPosition();

            OperationSyntax ret = new OperationSyntax(start, end, op);

            if (op != null)
            {
                foreach (OperandSyntax item in op)
                    item.parent = ret;
            }

            return ret;
        }

        public OperandSyntax[] parseOperandList()
        {
            List<OperandSyntax> ret = null;
            while (true)
            {
                OperandSyntax node = parseOperand();
                if (node == null)
                    break;

                if (ret == null)
                    ret = new List<OperandSyntax>();

                ret.Add(node);
            }

            if (ret != null)
                return ret.ToArray();
            else
                return null;
        }

        public OperandSyntax parseOperand()
        {
            Position start = lexer.getPreviousPosition();

            ExpressionSyntax primary = parseExpression();
            if (primary == null)
                return null;

            PostfixSyntax[] postfixes = parsePostfixList();

            Position end = lexer.getPosition();

            OperandSyntax ret = new OperandSyntax(start, end, primary, postfixes);

            primary.parent = ret;
            if (postfixes != null)
            {
                foreach (PostfixSyntax item in postfixes)
                    item.parent = ret;
            }

            return ret;
        }

        public PostfixSyntax[] parsePostfixList()
        {
            List<PostfixSyntax> ret = null;
            while (true)
            {
                PostfixSyntax node = parsePostfix();
                if (node == null)
                    break;

                if (ret == null)
                    ret = new List<PostfixSyntax>();

                ret.Add(node);
            }

            if (ret != null)
                return ret.ToArray();
            else
                return null;
        }

        public PostfixSyntax parsePostfix()
        {
            {
                MemberAccessSyntax node = parseMemberAccess();
                if (node != null)
                    return node;
            }

            {
                AsSyntax node = parseAs();
                if (node != null)
                    return node;
            }

            {
                IsSyntax node = parseIs();
                if (node != null)
                    return node;
            }

            {
                UnwrapSyntax node = parseUnwrap();
                if (node != null)
                    return node;
            }

            {
                CatchSyntax node = parseCatch();
                if (node != null)
                    return node;
            }

            return null;
        }

        public MemberAccessSyntax parseMemberAccess()
        {
            Position start = lexer.getPreviousPosition();

            bool successDot1 = lexer.parsePunctuation(".");
            if (successDot1)
                lexer.advance();
            else
                return null;

            string member = lexer.parseIdentifier();
            if ((member != null) && isIdentifier(member))
                lexer.advance();
            else
                throw new ParserException(fileName, lexer.line, lexer.column);

            Position end = lexer.getPosition();

            MemberAccessSyntax ret = new MemberAccessSyntax(start, end, member);


            return ret;
        }

        public AsSyntax parseAs()
        {
            Position start = lexer.getPreviousPosition();

            bool successAs1 = lexer.parseKeyword("as");
            if (successAs1)
                lexer.advance();
            else
                return null;

            TypeSpecSyntax typeSpec = parseTypeSpec();
            if (typeSpec == null)
                throw new ParserException(fileName, lexer.line, lexer.column);

            Position end = lexer.getPosition();

            AsSyntax ret = new AsSyntax(start, end, typeSpec);

            typeSpec.parent = ret;

            return ret;
        }

        public IsSyntax parseIs()
        {
            Position start = lexer.getPreviousPosition();

            bool successIs1 = lexer.parseKeyword("is");
            if (successIs1)
                lexer.advance();
            else
                return null;

            TypeSpecSyntax typeSpec = parseTypeSpec();
            if (typeSpec == null)
                throw new ParserException(fileName, lexer.line, lexer.column);

            Position end = lexer.getPosition();

            IsSyntax ret = new IsSyntax(start, end, typeSpec);

            typeSpec.parent = ret;

            return ret;
        }

        public UnwrapSyntax parseUnwrap()
        {
            Position start = lexer.getPreviousPosition();

            bool successExclamation1 = lexer.parsePunctuation("!");
            if (successExclamation1)
                lexer.advance();
            else
                return null;

            Position end = lexer.getPosition();

            UnwrapSyntax ret = new UnwrapSyntax(start, end);


            return ret;
        }

        public CatchSyntax parseCatch()
        {
            Position start = lexer.getPreviousPosition();

            bool successCatch1 = lexer.parseKeyword("catch");
            if (successCatch1)
                lexer.advance();
            else
                return null;

            CatchPatternSyntax typeSpec = parseCatchPattern();
            if (typeSpec == null)
                throw new ParserException(fileName, lexer.line, lexer.column);

            BlockSyntax handler = parseBlock();
            if (handler == null)
                throw new ParserException(fileName, lexer.line, lexer.column);

            Position end = lexer.getPosition();

            CatchSyntax ret = new CatchSyntax(start, end, typeSpec, handler);

            typeSpec.parent = ret;
            handler.parent = ret;

            return ret;
        }

        public CatchPatternSyntax parseCatchPattern()
        {
            {
                WildCardCatchPatternSyntax node = parseWildCardCatchPattern();
                if (node != null)
                    return node;
            }

            {
                TypeCatchPatternSyntax node = parseTypeCatchPattern();
                if (node != null)
                    return node;
            }

            return null;
        }

        public WildCardCatchPatternSyntax parseWildCardCatchPattern()
        {
            Position start = lexer.getPreviousPosition();

            WildcardPatternSyntax pattern = parseWildcardPattern();
            if (pattern == null)
                return null;

            Position end = lexer.getPosition();

            WildCardCatchPatternSyntax ret = new WildCardCatchPatternSyntax(start, end, pattern);

            pattern.parent = ret;

            return ret;
        }

        public TypeCatchPatternSyntax parseTypeCatchPattern()
        {
            Position start = lexer.getPreviousPosition();

            TypeSpecSyntax typeSpec = parseTypeSpec();
            if (typeSpec == null)
                return null;

            string errorName = lexer.parseIdentifier();
            if ((errorName != null) && isIdentifier(errorName))
                lexer.advance();

            Position end = lexer.getPosition();

            TypeCatchPatternSyntax ret = new TypeCatchPatternSyntax(start, end, typeSpec, errorName);

            typeSpec.parent = ret;

            return ret;
        }

        public ExpressionSyntax[] parseExpressionList()
        {
            List<ExpressionSyntax> ret = null;
            while (true)
            {
                ExpressionSyntax node = parseExpression();
                if (node == null)
                    break;

                if (ret == null)
                    ret = new List<ExpressionSyntax>();

                ret.Add(node);
            }

            if (ret != null)
                return ret.ToArray();
            else
                return null;
        }

        public ExpressionSyntax parseExpression()
        {
            {
                BlockSyntax node = parseBlock();
                if (node != null)
                    return node;
            }

            {
                NameSyntax node = parseName();
                if (node != null)
                    return node;
            }

            {
                ConstantSyntax node = parseConstant();
                if (node != null)
                    return node;
            }

            {
                IfSyntax node = parseIf();
                if (node != null)
                    return node;
            }

            {
                SwitchSyntax node = parseSwitch();
                if (node != null)
                    return node;
            }

            {
                ForSyntax node = parseFor();
                if (node != null)
                    return node;
            }

            {
                WhileSyntax node = parseWhile();
                if (node != null)
                    return node;
            }

            {
                DoSyntax node = parseDo();
                if (node != null)
                    return node;
            }

            {
                ThisSyntax node = parseThis();
                if (node != null)
                    return node;
            }

            {
                NewSyntax node = parseNew();
                if (node != null)
                    return node;
            }

            {
                ObjectSyntax node = parseObject();
                if (node != null)
                    return node;
            }

            {
                ArraySyntax node = parseArray();
                if (node != null)
                    return node;
            }

            {
                SizeOfSyntax node = parseSizeOf();
                if (node != null)
                    return node;
            }

            return null;
        }

        public BlockSyntax parseBlock()
        {
            Position start = lexer.getPreviousPosition();

            bool successLeftCurly1 = lexer.parsePunctuation("{");
            if (successLeftCurly1)
                lexer.advance();
            else
                return null;

            StatementSyntax[] statements = parseStatementList();

            bool successRightCurly3 = lexer.parsePunctuation("}");
            if (successRightCurly3)
                lexer.advance();
            else
                throw new ParserException(fileName, lexer.line, lexer.column);

            Position end = lexer.getPosition();

            BlockSyntax ret = new BlockSyntax(start, end, statements);

            if (statements != null)
            {
                foreach (StatementSyntax item in statements)
                    item.parent = ret;
            }

            return ret;
        }

        public ConstantSyntax parseConstant()
        {
            Position start = lexer.getPreviousPosition();

            Literal literal = lexer.parseLiteral();
            if (literal != null)
                lexer.advance();
            else
                return null;

            Position end = lexer.getPosition();

            ConstantSyntax ret = new ConstantSyntax(start, end, literal);


            return ret;
        }

        public IfSyntax parseIf()
        {
            Position start = lexer.getPreviousPosition();

            bool successIf1 = lexer.parseKeyword("if");
            if (successIf1)
                lexer.advance();
            else
                return null;

            bool successLeftParen2 = lexer.parsePunctuation("(");
            if (successLeftParen2)
                lexer.advance();
            else
                throw new ParserException(fileName, lexer.line, lexer.column);

            OperationSyntax condition = parseOperation();
            if (condition == null)
                throw new ParserException(fileName, lexer.line, lexer.column);

            bool successRightParen4 = lexer.parsePunctuation(")");
            if (successRightParen4)
                lexer.advance();
            else
                throw new ParserException(fileName, lexer.line, lexer.column);

            BlockSyntax consequent = parseBlock();
            if (consequent == null)
                throw new ParserException(fileName, lexer.line, lexer.column);

            ElseSyntax elseClause = parseElse();

            Position end = lexer.getPosition();

            IfSyntax ret = new IfSyntax(start, end, condition, consequent, elseClause);

            condition.parent = ret;
            consequent.parent = ret;
            if (elseClause != null)
                elseClause.parent = ret;

            return ret;
        }

        public ElseSyntax parseElse()
        {
            Position start = lexer.getPreviousPosition();

            bool successElse1 = lexer.parseKeyword("else");
            if (successElse1)
                lexer.advance();
            else
                return null;

            BlockSyntax alternative = parseBlock();
            if (alternative == null)
                throw new ParserException(fileName, lexer.line, lexer.column);

            Position end = lexer.getPosition();

            ElseSyntax ret = new ElseSyntax(start, end, alternative);

            alternative.parent = ret;

            return ret;
        }

        public SwitchSyntax parseSwitch()
        {
            Position start = lexer.getPreviousPosition();

            bool successSwitch1 = lexer.parseKeyword("switch");
            if (successSwitch1)
                lexer.advance();
            else
                return null;

            bool successLeftParen2 = lexer.parsePunctuation("(");
            if (successLeftParen2)
                lexer.advance();
            else
                throw new ParserException(fileName, lexer.line, lexer.column);

            OperationSyntax condition = parseOperation();
            if (condition == null)
                throw new ParserException(fileName, lexer.line, lexer.column);

            bool successRightParen4 = lexer.parsePunctuation(")");
            if (successRightParen4)
                lexer.advance();
            else
                throw new ParserException(fileName, lexer.line, lexer.column);

            bool successLeftCurly5 = lexer.parsePunctuation("{");
            if (successLeftCurly5)
                lexer.advance();
            else
                throw new ParserException(fileName, lexer.line, lexer.column);

            SwitchCaseSyntax[] cases = parseSwitchCaseList();
            if (cases == null)
                throw new ParserException(fileName, lexer.line, lexer.column);

            bool successRightCurly7 = lexer.parsePunctuation("}");
            if (successRightCurly7)
                lexer.advance();
            else
                throw new ParserException(fileName, lexer.line, lexer.column);

            Position end = lexer.getPosition();

            SwitchSyntax ret = new SwitchSyntax(start, end, condition, cases);

            condition.parent = ret;
            if (cases != null)
            {
                foreach (SwitchCaseSyntax item in cases)
                    item.parent = ret;
            }

            return ret;
        }

        public SwitchCaseSyntax[] parseSwitchCaseList()
        {
            List<SwitchCaseSyntax> ret = null;
            while (true)
            {
                SwitchCaseSyntax node = parseSwitchCase();
                if (node == null)
                    break;

                if (ret == null)
                    ret = new List<SwitchCaseSyntax>();

                ret.Add(node);
            }

            if (ret != null)
                return ret.ToArray();
            else
                return null;
        }

        public SwitchCaseSyntax parseSwitchCase()
        {
            Position start = lexer.getPreviousPosition();

            CaseLabelSyntax label = parseCaseLabel();
            if (label == null)
                return null;

            BlockSyntax content = parseBlock();
            if (content == null)
                throw new ParserException(fileName, lexer.line, lexer.column);

            Position end = lexer.getPosition();

            SwitchCaseSyntax ret = new SwitchCaseSyntax(start, end, label, content);

            label.parent = ret;
            content.parent = ret;

            return ret;
        }

        public CaseLabelSyntax parseCaseLabel()
        {
            {
                ItemCaseLabelSyntax node = parseItemCaseLabel();
                if (node != null)
                    return node;
            }

            {
                DefaultCaseLabelSyntax node = parseDefaultCaseLabel();
                if (node != null)
                    return node;
            }

            return null;
        }

        public ItemCaseLabelSyntax parseItemCaseLabel()
        {
            Position start = lexer.getPreviousPosition();

            bool successCase1 = lexer.parseKeyword("case");
            if (successCase1)
                lexer.advance();
            else
                return null;

            CaseItemSyntax[] items = parseCaseItemList();
            if (items == null)
                throw new ParserException(fileName, lexer.line, lexer.column);

            Position end = lexer.getPosition();

            ItemCaseLabelSyntax ret = new ItemCaseLabelSyntax(start, end, items);

            if (items != null)
            {
                foreach (CaseItemSyntax item in items)
                    item.parent = ret;
            }

            return ret;
        }

        public CaseItemSyntax[] parseCaseItemList()
        {
            List<CaseItemSyntax> ret = null;
            while (true)
            {
                CaseItemSyntax node = parseCaseItem();
                if (node == null)
                    break;

                if (ret == null)
                    ret = new List<CaseItemSyntax>();

                ret.Add(node);
            }

            if (ret != null)
                return ret.ToArray();
            else
                return null;
        }

        public CaseItemSyntax parseCaseItem()
        {
            Position start = lexer.getPreviousPosition();

            CasePatternSyntax pattern = parseCasePattern();
            if (pattern == null)
                return null;

            bool successComma2 = lexer.parsePunctuation(",");
            if (successComma2)
                lexer.advance();

            Position end = lexer.getPosition();

            CaseItemSyntax ret = new CaseItemSyntax(start, end, pattern);

            pattern.parent = ret;

            return ret;
        }

        public CasePatternSyntax[] parseCasePatternList()
        {
            List<CasePatternSyntax> ret = null;
            while (true)
            {
                CasePatternSyntax node = parseCasePattern();
                if (node == null)
                    break;

                if (ret == null)
                    ret = new List<CasePatternSyntax>();

                ret.Add(node);
            }

            if (ret != null)
                return ret.ToArray();
            else
                return null;
        }

        public CasePatternSyntax parseCasePattern()
        {
            {
                ConstantPatternSyntax node = parseConstantPattern();
                if (node != null)
                    return node;
            }

            {
                WildcardPatternSyntax node = parseWildcardPattern();
                if (node != null)
                    return node;
            }

            {
                NamePatternSyntax node = parseNamePattern();
                if (node != null)
                    return node;
            }

            return null;
        }

        public ConstantPatternSyntax parseConstantPattern()
        {
            Position start = lexer.getPreviousPosition();

            ConstantSyntax constant = parseConstant();
            if (constant == null)
                return null;

            Position end = lexer.getPosition();

            ConstantPatternSyntax ret = new ConstantPatternSyntax(start, end, constant);

            constant.parent = ret;

            return ret;
        }

        public WildcardPatternSyntax parseWildcardPattern()
        {
            Position start = lexer.getPreviousPosition();

            bool successUnderscore1 = lexer.parsePunctuation("_");
            if (successUnderscore1)
                lexer.advance();
            else
                return null;

            Position end = lexer.getPosition();

            WildcardPatternSyntax ret = new WildcardPatternSyntax(start, end);


            return ret;
        }

        public NamePatternSyntax parseNamePattern()
        {
            Position start = lexer.getPreviousPosition();

            NameSyntax name = parseName();
            if (name == null)
                return null;

            Position end = lexer.getPosition();

            NamePatternSyntax ret = new NamePatternSyntax(start, end, name);

            name.parent = ret;

            return ret;
        }

        public DefaultCaseLabelSyntax parseDefaultCaseLabel()
        {
            Position start = lexer.getPreviousPosition();

            bool successDefault1 = lexer.parseKeyword("default");
            if (successDefault1)
                lexer.advance();
            else
                return null;

            Position end = lexer.getPosition();

            DefaultCaseLabelSyntax ret = new DefaultCaseLabelSyntax(start, end);


            return ret;
        }

        public ForSyntax parseFor()
        {
            Position start = lexer.getPreviousPosition();

            bool successFor1 = lexer.parseKeyword("for");
            if (successFor1)
                lexer.advance();
            else
                return null;

            bool successLeftParen2 = lexer.parsePunctuation("(");
            if (successLeftParen2)
                lexer.advance();
            else
                throw new ParserException(fileName, lexer.line, lexer.column);

            string index = lexer.parseIdentifier();
            if ((index != null) && isIdentifier(index))
                lexer.advance();
            else
                throw new ParserException(fileName, lexer.line, lexer.column);

            TypeAnnotationSyntax typeAnnotation = parseTypeAnnotation();

            bool successIn5 = lexer.parseKeyword("in");
            if (successIn5)
                lexer.advance();
            else
                throw new ParserException(fileName, lexer.line, lexer.column);

            OperationSyntax operation = parseOperation();
            if (operation == null)
                throw new ParserException(fileName, lexer.line, lexer.column);

            bool successRightParen7 = lexer.parsePunctuation(")");
            if (successRightParen7)
                lexer.advance();
            else
                throw new ParserException(fileName, lexer.line, lexer.column);

            LoopSyntax iteration = parseLoop();
            if (iteration == null)
                throw new ParserException(fileName, lexer.line, lexer.column);

            Position end = lexer.getPosition();

            ForSyntax ret = new ForSyntax(start, end, index, typeAnnotation, operation, iteration);

            if (typeAnnotation != null)
                typeAnnotation.parent = ret;
            operation.parent = ret;
            iteration.parent = ret;

            return ret;
        }

        public WhileSyntax parseWhile()
        {
            Position start = lexer.getPreviousPosition();

            bool successWhile1 = lexer.parseKeyword("while");
            if (successWhile1)
                lexer.advance();
            else
                return null;

            bool successLeftParen2 = lexer.parsePunctuation("(");
            if (successLeftParen2)
                lexer.advance();
            else
                throw new ParserException(fileName, lexer.line, lexer.column);

            OperationSyntax condition = parseOperation();
            if (condition == null)
                throw new ParserException(fileName, lexer.line, lexer.column);

            bool successRightParen4 = lexer.parsePunctuation(")");
            if (successRightParen4)
                lexer.advance();
            else
                throw new ParserException(fileName, lexer.line, lexer.column);

            LoopSyntax iteration = parseLoop();
            if (iteration == null)
                throw new ParserException(fileName, lexer.line, lexer.column);

            Position end = lexer.getPosition();

            WhileSyntax ret = new WhileSyntax(start, end, condition, iteration);

            condition.parent = ret;
            iteration.parent = ret;

            return ret;
        }

        public DoSyntax parseDo()
        {
            Position start = lexer.getPreviousPosition();

            bool successDo1 = lexer.parseKeyword("do");
            if (successDo1)
                lexer.advance();
            else
                return null;

            LoopSyntax iteration = parseLoop();
            if (iteration == null)
                throw new ParserException(fileName, lexer.line, lexer.column);

            bool successWhile3 = lexer.parseKeyword("while");
            if (successWhile3)
                lexer.advance();
            else
                throw new ParserException(fileName, lexer.line, lexer.column);

            bool successLeftParen4 = lexer.parsePunctuation("(");
            if (successLeftParen4)
                lexer.advance();
            else
                throw new ParserException(fileName, lexer.line, lexer.column);

            OperationSyntax condition = parseOperation();
            if (condition == null)
                throw new ParserException(fileName, lexer.line, lexer.column);

            bool successRightParen6 = lexer.parsePunctuation(")");
            if (successRightParen6)
                lexer.advance();
            else
                throw new ParserException(fileName, lexer.line, lexer.column);

            Position end = lexer.getPosition();

            DoSyntax ret = new DoSyntax(start, end, iteration, condition);

            iteration.parent = ret;
            condition.parent = ret;

            return ret;
        }

        public LoopSyntax parseLoop()
        {
            {
                SimpleLoopSyntax node = parseSimpleLoop();
                if (node != null)
                    return node;
            }

            {
                NamedLoopSyntax node = parseNamedLoop();
                if (node != null)
                    return node;
            }

            return null;
        }

        public SimpleLoopSyntax parseSimpleLoop()
        {
            Position start = lexer.getPreviousPosition();

            BlockSyntax code = parseBlock();
            if (code == null)
                return null;

            Position end = lexer.getPosition();

            SimpleLoopSyntax ret = new SimpleLoopSyntax(start, end, code);

            code.parent = ret;

            return ret;
        }

        public NamedLoopSyntax parseNamedLoop()
        {
            Position start = lexer.getPreviousPosition();

            bool successLoop1 = lexer.parseKeyword("loop");
            if (successLoop1)
                lexer.advance();
            else
                return null;

            string name = lexer.parseIdentifier();
            if ((name != null) && isIdentifier(name))
                lexer.advance();
            else
                throw new ParserException(fileName, lexer.line, lexer.column);

            BlockSyntax code = parseBlock();
            if (code == null)
                throw new ParserException(fileName, lexer.line, lexer.column);

            Position end = lexer.getPosition();

            NamedLoopSyntax ret = new NamedLoopSyntax(start, end, name, code);

            code.parent = ret;

            return ret;
        }

        public ThisSyntax parseThis()
        {
            Position start = lexer.getPreviousPosition();

            bool successThis1 = lexer.parseKeyword("this");
            if (successThis1)
                lexer.advance();
            else
                return null;

            Position end = lexer.getPosition();

            ThisSyntax ret = new ThisSyntax(start, end);


            return ret;
        }

        public NewSyntax parseNew()
        {
            Position start = lexer.getPreviousPosition();

            bool successNew1 = lexer.parseKeyword("new");
            if (successNew1)
                lexer.advance();
            else
                return null;

            TypeSyntax typeSpec = parseType();
            if (typeSpec == null)
                throw new ParserException(fileName, lexer.line, lexer.column);

            Position end = lexer.getPosition();

            NewSyntax ret = new NewSyntax(start, end, typeSpec);

            typeSpec.parent = ret;

            return ret;
        }

        public ObjectSyntax parseObject()
        {
            Position start = lexer.getPreviousPosition();

            bool successLeftParen1 = lexer.parsePunctuation("(");
            if (successLeftParen1)
                lexer.advance();
            else
                return null;

            OperationSyntax firstOp = parseOperation();

            ItemSyntax[] additionalOps = parseItemList();

            bool successRightParen4 = lexer.parsePunctuation(")");
            if (successRightParen4)
                lexer.advance();
            else
                throw new ParserException(fileName, lexer.line, lexer.column);

            Position end = lexer.getPosition();

            ObjectSyntax ret = new ObjectSyntax(start, end, firstOp, additionalOps);

            if (firstOp != null)
                firstOp.parent = ret;
            if (additionalOps != null)
            {
                foreach (ItemSyntax item in additionalOps)
                    item.parent = ret;
            }

            return ret;
        }

        public ArraySyntax parseArray()
        {
            Position start = lexer.getPreviousPosition();

            bool successLeftBracket1 = lexer.parsePunctuation("[");
            if (successLeftBracket1)
                lexer.advance();
            else
                return null;

            OperationSyntax firstOp = parseOperation();

            ItemSyntax[] additionalOps = parseItemList();

            bool successRightBracket4 = lexer.parsePunctuation("]");
            if (successRightBracket4)
                lexer.advance();
            else
                throw new ParserException(fileName, lexer.line, lexer.column);

            Position end = lexer.getPosition();

            ArraySyntax ret = new ArraySyntax(start, end, firstOp, additionalOps);

            if (firstOp != null)
                firstOp.parent = ret;
            if (additionalOps != null)
            {
                foreach (ItemSyntax item in additionalOps)
                    item.parent = ret;
            }

            return ret;
        }

        public ItemSyntax[] parseItemList()
        {
            List<ItemSyntax> ret = null;
            while (true)
            {
                ItemSyntax node = parseItem();
                if (node == null)
                    break;

                if (ret == null)
                    ret = new List<ItemSyntax>();

                ret.Add(node);
            }

            if (ret != null)
                return ret.ToArray();
            else
                return null;
        }

        public ItemSyntax parseItem()
        {
            Position start = lexer.getPreviousPosition();

            bool successComma1 = lexer.parsePunctuation(",");
            if (successComma1)
                lexer.advance();
            else
                return null;

            OperationSyntax operation = parseOperation();
            if (operation == null)
                throw new ParserException(fileName, lexer.line, lexer.column);

            Position end = lexer.getPosition();

            ItemSyntax ret = new ItemSyntax(start, end, operation);

            operation.parent = ret;

            return ret;
        }

        public SizeOfSyntax parseSizeOf()
        {
            Position start = lexer.getPreviousPosition();

            bool successSizeof1 = lexer.parseKeyword("sizeof");
            if (successSizeof1)
                lexer.advance();
            else
                return null;

            TypeSyntax typeSpec = parseType();
            if (typeSpec == null)
                throw new ParserException(fileName, lexer.line, lexer.column);

            Position end = lexer.getPosition();

            SizeOfSyntax ret = new SizeOfSyntax(start, end, typeSpec);

            typeSpec.parent = ret;

            return ret;
        }

        public BreakSyntax parseBreak()
        {
            Position start = lexer.getPreviousPosition();

            bool successBreak1 = lexer.parseKeyword("break");
            if (successBreak1)
                lexer.advance();
            else
                return null;

            string iteration = lexer.parseIdentifier();
            if ((iteration != null) && isIdentifier(iteration))
                lexer.advance();

            bool successSemicolon3 = lexer.parsePunctuation(";");
            if (successSemicolon3)
                lexer.advance();

            Position end = lexer.getPosition();

            BreakSyntax ret = new BreakSyntax(start, end, iteration);


            return ret;
        }

        public ContinueSyntax parseContinue()
        {
            Position start = lexer.getPreviousPosition();

            bool successContinue1 = lexer.parseKeyword("continue");
            if (successContinue1)
                lexer.advance();
            else
                return null;

            string iteration = lexer.parseIdentifier();
            if ((iteration != null) && isIdentifier(iteration))
                lexer.advance();

            bool successSemicolon3 = lexer.parsePunctuation(";");
            if (successSemicolon3)
                lexer.advance();

            Position end = lexer.getPosition();

            ContinueSyntax ret = new ContinueSyntax(start, end, iteration);


            return ret;
        }

        public ReturnSyntax parseReturn()
        {
            Position start = lexer.getPreviousPosition();

            bool successReturn1 = lexer.parseKeyword("return");
            if (successReturn1)
                lexer.advance();
            else
                return null;

            CalculationSyntax result = parseCalculation();

            Position end = lexer.getPosition();

            ReturnSyntax ret = new ReturnSyntax(start, end, result);

            if (result != null)
                result.parent = ret;

            return ret;
        }

        public ThrowSyntax parseThrow()
        {
            Position start = lexer.getPreviousPosition();

            bool successThrow1 = lexer.parseKeyword("throw");
            if (successThrow1)
                lexer.advance();
            else
                return null;

            CalculationSyntax exception = parseCalculation();
            if (exception == null)
                throw new ParserException(fileName, lexer.line, lexer.column);

            Position end = lexer.getPosition();

            ThrowSyntax ret = new ThrowSyntax(start, end, exception);

            exception.parent = ret;

            return ret;
        }

        public ClassSyntax parseClass()
        {
            Position start = lexer.getPreviousPosition();

            bool successClass1 = lexer.parseKeyword("class");
            if (successClass1)
                lexer.advance();
            else
                return null;

            NameSyntax name = parseName();
            if (name == null)
                throw new ParserException(fileName, lexer.line, lexer.column);

            GenericParametersSyntax generics = parseGenericParameters();

            ExtendsSyntax baseClass = parseExtends();

            StructureSyntax contents = parseStructure();

            ClassBodySyntax body = parseClassBody();

            Position end = lexer.getPosition();

            ClassSyntax ret = new ClassSyntax(start, end, name, generics, baseClass, contents, body);

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

        public GenericParametersSyntax parseGenericParameters()
        {
            Position start = lexer.getPreviousPosition();

            bool successLeftBracket1 = lexer.parsePunctuation("[");
            if (successLeftBracket1)
                lexer.advance();
            else
                return null;

            string name = lexer.parseIdentifier();
            if ((name != null) && isIdentifier(name))
                lexer.advance();
            else
                throw new ParserException(fileName, lexer.line, lexer.column);

            GenericParameterSyntax[] additionalGenerics = parseGenericParameterList();

            bool successRightBracket4 = lexer.parsePunctuation("]");
            if (successRightBracket4)
                lexer.advance();
            else
                throw new ParserException(fileName, lexer.line, lexer.column);

            Position end = lexer.getPosition();

            GenericParametersSyntax ret = new GenericParametersSyntax(start, end, name, additionalGenerics);

            if (additionalGenerics != null)
            {
                foreach (GenericParameterSyntax item in additionalGenerics)
                    item.parent = ret;
            }

            return ret;
        }

        public GenericParameterSyntax[] parseGenericParameterList()
        {
            List<GenericParameterSyntax> ret = null;
            while (true)
            {
                GenericParameterSyntax node = parseGenericParameter();
                if (node == null)
                    break;

                if (ret == null)
                    ret = new List<GenericParameterSyntax>();

                ret.Add(node);
            }

            if (ret != null)
                return ret.ToArray();
            else
                return null;
        }

        public GenericParameterSyntax parseGenericParameter()
        {
            Position start = lexer.getPreviousPosition();

            bool successComma1 = lexer.parsePunctuation(",");
            if (successComma1)
                lexer.advance();
            else
                return null;

            string name = lexer.parseIdentifier();
            if ((name != null) && isIdentifier(name))
                lexer.advance();
            else
                throw new ParserException(fileName, lexer.line, lexer.column);

            Position end = lexer.getPosition();

            GenericParameterSyntax ret = new GenericParameterSyntax(start, end, name);


            return ret;
        }

        public ExtendsSyntax parseExtends()
        {
            Position start = lexer.getPreviousPosition();

            bool successExtends1 = lexer.parseKeyword("extends");
            if (successExtends1)
                lexer.advance();
            else
                return null;

            NameSyntax name = parseName();
            if (name == null)
                throw new ParserException(fileName, lexer.line, lexer.column);

            Position end = lexer.getPosition();

            ExtendsSyntax ret = new ExtendsSyntax(start, end, name);

            name.parent = ret;

            return ret;
        }

        public StructureSyntax parseStructure()
        {
            Position start = lexer.getPreviousPosition();

            bool successLeftParen1 = lexer.parsePunctuation("(");
            if (successLeftParen1)
                lexer.advance();
            else
                return null;

            ComponentSyntax[] components = parseComponentList();

            bool successRightParen3 = lexer.parsePunctuation(")");
            if (successRightParen3)
                lexer.advance();
            else
                throw new ParserException(fileName, lexer.line, lexer.column);

            Position end = lexer.getPosition();

            StructureSyntax ret = new StructureSyntax(start, end, components);

            if (components != null)
            {
                foreach (ComponentSyntax item in components)
                    item.parent = ret;
            }

            return ret;
        }

        public ComponentSyntax[] parseComponentList()
        {
            List<ComponentSyntax> ret = null;
            while (true)
            {
                ComponentSyntax node = parseComponent();
                if (node == null)
                    break;

                if (ret == null)
                    ret = new List<ComponentSyntax>();

                ret.Add(node);
            }

            if (ret != null)
                return ret.ToArray();
            else
                return null;
        }

        public ComponentSyntax parseComponent()
        {
            Position start = lexer.getPreviousPosition();

            string name = lexer.parseIdentifier();
            if ((name != null) && isIdentifier(name))
                lexer.advance();
            else
                return null;

            TypeAnnotationSyntax typeAnnotation = parseTypeAnnotation();

            bool successComma3 = lexer.parsePunctuation(",");
            if (successComma3)
                lexer.advance();

            Position end = lexer.getPosition();

            ComponentSyntax ret = new ComponentSyntax(start, end, name, typeAnnotation);

            if (typeAnnotation != null)
                typeAnnotation.parent = ret;

            return ret;
        }

        public ClassBodySyntax parseClassBody()
        {
            Position start = lexer.getPreviousPosition();

            bool successLeftCurly1 = lexer.parsePunctuation("{");
            if (successLeftCurly1)
                lexer.advance();
            else
                return null;

            ClassMemberSyntax[] members = parseClassMemberList();

            bool successRightCurly3 = lexer.parsePunctuation("}");
            if (successRightCurly3)
                lexer.advance();
            else
                throw new ParserException(fileName, lexer.line, lexer.column);

            Position end = lexer.getPosition();

            ClassBodySyntax ret = new ClassBodySyntax(start, end, members);

            if (members != null)
            {
                foreach (ClassMemberSyntax item in members)
                    item.parent = ret;
            }

            return ret;
        }

        public ClassMemberSyntax[] parseClassMemberList()
        {
            List<ClassMemberSyntax> ret = null;
            while (true)
            {
                ClassMemberSyntax node = parseClassMember();
                if (node == null)
                    break;

                if (ret == null)
                    ret = new List<ClassMemberSyntax>();

                ret.Add(node);
            }

            if (ret != null)
                return ret.ToArray();
            else
                return null;
        }

        public ClassMemberSyntax parseClassMember()
        {
            {
                LetMemberSyntax node = parseLetMember();
                if (node != null)
                    return node;
            }

            {
                VarMemberSyntax node = parseVarMember();
                if (node != null)
                    return node;
            }

            {
                MutableMemberSyntax node = parseMutableMember();
                if (node != null)
                    return node;
            }

            {
                SetInitializationSyntax node = parseSetInitialization();
                if (node != null)
                    return node;
            }

            {
                MethodSyntax node = parseMethod();
                if (node != null)
                    return node;
            }

            {
                StaticFunctionSyntax node = parseStaticFunction();
                if (node != null)
                    return node;
            }

            {
                OperatorSyntax node = parseOperator();
                if (node != null)
                    return node;
            }

            {
                InitializerSyntax node = parseInitializer();
                if (node != null)
                    return node;
            }

            {
                AllocatorSyntax node = parseAllocator();
                if (node != null)
                    return node;
            }

            return null;
        }

        public LetMemberSyntax parseLetMember()
        {
            Position start = lexer.getPreviousPosition();

            LetSyntax declaration = parseLet();
            if (declaration == null)
                return null;

            Position end = lexer.getPosition();

            LetMemberSyntax ret = new LetMemberSyntax(start, end, declaration);

            declaration.parent = ret;

            return ret;
        }

        public VarMemberSyntax parseVarMember()
        {
            Position start = lexer.getPreviousPosition();

            VarSyntax declaration = parseVar();
            if (declaration == null)
                return null;

            Position end = lexer.getPosition();

            VarMemberSyntax ret = new VarMemberSyntax(start, end, declaration);

            declaration.parent = ret;

            return ret;
        }

        public MutableMemberSyntax parseMutableMember()
        {
            Position start = lexer.getPreviousPosition();

            MutableSyntax declaration = parseMutable();
            if (declaration == null)
                return null;

            Position end = lexer.getPosition();

            MutableMemberSyntax ret = new MutableMemberSyntax(start, end, declaration);

            declaration.parent = ret;

            return ret;
        }

        public SetInitializationSyntax parseSetInitialization()
        {
            Position start = lexer.getPreviousPosition();

            SetSyntax definition = parseSet();
            if (definition == null)
                return null;

            Position end = lexer.getPosition();

            SetInitializationSyntax ret = new SetInitializationSyntax(start, end, definition);

            definition.parent = ret;

            return ret;
        }

        public MethodSyntax parseMethod()
        {
            Position start = lexer.getPreviousPosition();

            bool successMethod1 = lexer.parseKeyword("method");
            if (successMethod1)
                lexer.advance();
            else
                return null;

            ProcedureSyntax procedure = parseProcedure();
            if (procedure == null)
                throw new ParserException(fileName, lexer.line, lexer.column);

            Position end = lexer.getPosition();

            MethodSyntax ret = new MethodSyntax(start, end, procedure);

            procedure.parent = ret;

            return ret;
        }

        public StaticFunctionSyntax parseStaticFunction()
        {
            Position start = lexer.getPreviousPosition();

            FunctionSyntax procedure = parseFunction();
            if (procedure == null)
                return null;

            Position end = lexer.getPosition();

            StaticFunctionSyntax ret = new StaticFunctionSyntax(start, end, procedure);

            procedure.parent = ret;

            return ret;
        }

        public OperatorSyntax parseOperator()
        {
            Position start = lexer.getPreviousPosition();

            bool successOperator1 = lexer.parseKeyword("operator");
            if (successOperator1)
                lexer.advance();
            else
                return null;

            RoutineSyntax routine = parseRoutine();
            if (routine == null)
                throw new ParserException(fileName, lexer.line, lexer.column);

            Position end = lexer.getPosition();

            OperatorSyntax ret = new OperatorSyntax(start, end, routine);

            routine.parent = ret;

            return ret;
        }

        public InitializerSyntax parseInitializer()
        {
            Position start = lexer.getPreviousPosition();

            bool successInitializer1 = lexer.parseKeyword("initializer");
            if (successInitializer1)
                lexer.advance();
            else
                return null;

            StructureSyntax input = parseStructure();

            BlockSyntax body = parseBlock();
            if (body == null)
                throw new ParserException(fileName, lexer.line, lexer.column);

            Position end = lexer.getPosition();

            InitializerSyntax ret = new InitializerSyntax(start, end, input, body);

            if (input != null)
                input.parent = ret;
            body.parent = ret;

            return ret;
        }

        public AllocatorSyntax parseAllocator()
        {
            Position start = lexer.getPreviousPosition();

            bool successAllocator1 = lexer.parseKeyword("allocator");
            if (successAllocator1)
                lexer.advance();
            else
                return null;

            StructureSyntax input = parseStructure();

            BlockSyntax body = parseBlock();
            if (body == null)
                throw new ParserException(fileName, lexer.line, lexer.column);

            Position end = lexer.getPosition();

            AllocatorSyntax ret = new AllocatorSyntax(start, end, input, body);

            if (input != null)
                input.parent = ret;
            body.parent = ret;

            return ret;
        }

        public TypeAnnotationSyntax parseTypeAnnotation()
        {
            Position start = lexer.getPreviousPosition();

            bool successColon1 = lexer.parsePunctuation(":");
            if (successColon1)
                lexer.advance();
            else
                return null;

            TypeSpecSyntax typeSpec = parseTypeSpec();
            if (typeSpec == null)
                throw new ParserException(fileName, lexer.line, lexer.column);

            Position end = lexer.getPosition();

            TypeAnnotationSyntax ret = new TypeAnnotationSyntax(start, end, typeSpec);

            typeSpec.parent = ret;

            return ret;
        }

        public TypeSpecSyntax[] parseTypeSpecList()
        {
            List<TypeSpecSyntax> ret = null;
            while (true)
            {
                TypeSpecSyntax node = parseTypeSpec();
                if (node == null)
                    break;

                if (ret == null)
                    ret = new List<TypeSpecSyntax>();

                ret.Add(node);
            }

            if (ret != null)
                return ret.ToArray();
            else
                return null;
        }

        public TypeSpecSyntax parseTypeSpec()
        {
            {
                TypeSyntax node = parseType();
                if (node != null)
                    return node;
            }

            {
                VariantSyntax node = parseVariant();
                if (node != null)
                    return node;
            }

            return null;
        }

        public TypeSyntax[] parseTypeList()
        {
            List<TypeSyntax> ret = null;
            while (true)
            {
                TypeSyntax node = parseType();
                if (node == null)
                    break;

                if (ret == null)
                    ret = new List<TypeSyntax>();

                ret.Add(node);
            }

            if (ret != null)
                return ret.ToArray();
            else
                return null;
        }

        public TypeSyntax parseType()
        {
            Position start = lexer.getPreviousPosition();

            NameSyntax name = parseName();
            if (name == null)
                return null;

            GenericArgumentsSyntax generics = parseGenericArguments();

            OptionalSyntax optional = parseOptional();

            LifeTimeSyntax lifeTime = parseLifeTime();

            Position end = lexer.getPosition();

            TypeSyntax ret = new TypeSyntax(start, end, name, generics, optional, lifeTime);

            name.parent = ret;
            if (generics != null)
                generics.parent = ret;
            if (optional != null)
                optional.parent = ret;
            if (lifeTime != null)
                lifeTime.parent = ret;

            return ret;
        }

        public VariantSyntax parseVariant()
        {
            Position start = lexer.getPreviousPosition();

            bool successLeftParen1 = lexer.parsePunctuation("(");
            if (successLeftParen1)
                lexer.advance();
            else
                return null;

            TypeSyntax[] types = parseTypeList();

            bool successRightParen3 = lexer.parsePunctuation(")");
            if (successRightParen3)
                lexer.advance();
            else
                throw new ParserException(fileName, lexer.line, lexer.column);

            Position end = lexer.getPosition();

            VariantSyntax ret = new VariantSyntax(start, end, types);

            if (types != null)
            {
                foreach (TypeSyntax item in types)
                    item.parent = ret;
            }

            return ret;
        }

        public ThrowsSyntax parseThrows()
        {
            Position start = lexer.getPreviousPosition();

            bool successThrows1 = lexer.parseKeyword("throws");
            if (successThrows1)
                lexer.advance();
            else
                return null;

            TypeSpecSyntax throwsType = parseTypeSpec();
            if (throwsType == null)
                throw new ParserException(fileName, lexer.line, lexer.column);

            Position end = lexer.getPosition();

            ThrowsSyntax ret = new ThrowsSyntax(start, end, throwsType);

            throwsType.parent = ret;

            return ret;
        }

        public GenericArgumentsSyntax parseGenericArguments()
        {
            Position start = lexer.getPreviousPosition();

            bool successLeftBracket1 = lexer.parsePunctuation("[");
            if (successLeftBracket1)
                lexer.advance();
            else
                return null;

            TypeSyntax generic = parseType();
            if (generic == null)
                throw new ParserException(fileName, lexer.line, lexer.column);

            GenericArgumentSyntax[] additionalGenerics = parseGenericArgumentList();

            bool successRightBracket4 = lexer.parsePunctuation("]");
            if (successRightBracket4)
                lexer.advance();
            else
                throw new ParserException(fileName, lexer.line, lexer.column);

            Position end = lexer.getPosition();

            GenericArgumentsSyntax ret = new GenericArgumentsSyntax(start, end, generic, additionalGenerics);

            generic.parent = ret;
            if (additionalGenerics != null)
            {
                foreach (GenericArgumentSyntax item in additionalGenerics)
                    item.parent = ret;
            }

            return ret;
        }

        public GenericArgumentSyntax[] parseGenericArgumentList()
        {
            List<GenericArgumentSyntax> ret = null;
            while (true)
            {
                GenericArgumentSyntax node = parseGenericArgument();
                if (node == null)
                    break;

                if (ret == null)
                    ret = new List<GenericArgumentSyntax>();

                ret.Add(node);
            }

            if (ret != null)
                return ret.ToArray();
            else
                return null;
        }

        public GenericArgumentSyntax parseGenericArgument()
        {
            Position start = lexer.getPreviousPosition();

            bool successComma1 = lexer.parsePunctuation(",");
            if (successComma1)
                lexer.advance();
            else
                return null;

            TypeSyntax typeSpec = parseType();
            if (typeSpec == null)
                throw new ParserException(fileName, lexer.line, lexer.column);

            Position end = lexer.getPosition();

            GenericArgumentSyntax ret = new GenericArgumentSyntax(start, end, typeSpec);

            typeSpec.parent = ret;

            return ret;
        }

        public OptionalSyntax parseOptional()
        {
            Position start = lexer.getPreviousPosition();

            bool successQuestion1 = lexer.parsePunctuation("?");
            if (successQuestion1)
                lexer.advance();
            else
                return null;

            Position end = lexer.getPosition();

            OptionalSyntax ret = new OptionalSyntax(start, end);


            return ret;
        }

        public LifeTimeSyntax parseLifeTime()
        {
            {
                RootSyntax node = parseRoot();
                if (node != null)
                    return node;
            }

            {
                LocalSyntax node = parseLocal();
                if (node != null)
                    return node;
            }

            {
                ReferenceSyntax node = parseReference();
                if (node != null)
                    return node;
            }

            {
                ThrownSyntax node = parseThrown();
                if (node != null)
                    return node;
            }

            return null;
        }

        public RootSyntax parseRoot()
        {
            Position start = lexer.getPreviousPosition();

            bool successDollar1 = lexer.parsePunctuation("$");
            if (successDollar1)
                lexer.advance();
            else
                return null;

            Position end = lexer.getPosition();

            RootSyntax ret = new RootSyntax(start, end);


            return ret;
        }

        public LocalSyntax parseLocal()
        {
            Position start = lexer.getPreviousPosition();

            bool successAt1 = lexer.parsePunctuation("@");
            if (successAt1)
                lexer.advance();
            else
                return null;

            string location = lexer.parseIdentifier();
            if ((location != null) && isIdentifier(location))
                lexer.advance();
            else
                throw new ParserException(fileName, lexer.line, lexer.column);

            Position end = lexer.getPosition();

            LocalSyntax ret = new LocalSyntax(start, end, location);


            return ret;
        }

        public ReferenceSyntax parseReference()
        {
            Position start = lexer.getPreviousPosition();

            bool successBacktick1 = lexer.parsePunctuation("`");
            if (successBacktick1)
                lexer.advance();
            else
                return null;

            Literal age = lexer.parseLiteral();
            if (age != null)
                lexer.advance();

            Position end = lexer.getPosition();

            ReferenceSyntax ret = new ReferenceSyntax(start, end, age);


            return ret;
        }

        public ThrownSyntax parseThrown()
        {
            Position start = lexer.getPreviousPosition();

            bool successHash1 = lexer.parsePunctuation("#");
            if (successHash1)
                lexer.advance();
            else
                return null;

            Position end = lexer.getPosition();

            ThrownSyntax ret = new ThrownSyntax(start, end);


            return ret;
        }

        bool isAtEnd()
        {
            return lexer.isAtEnd();
        }

        bool isIdentifier(string id)
        {
            if (keywords.Contains(id))
                return false;

            return true;
        }
    }

    public class SyntaxVisitor
    {
        public virtual bool openProgram(ProgramSyntax programSyntax)
        {
            return true;
        }

        public virtual void closeProgram(ProgramSyntax programSyntax)
        {
        }

        public virtual bool openFile(FileSyntax fileSyntax)
        {
            return true;
        }

        public virtual void closeFile(FileSyntax fileSyntax)
        {
        }

        public virtual void visitIntrinsic(IntrinsicSyntax intrinsicSyntax)
        {
        }

        public virtual bool openUsing(UsingSyntax usingSyntax)
        {
            return true;
        }

        public virtual void closeUsing(UsingSyntax usingSyntax)
        {
        }

        public virtual bool openDefine(DefineSyntax defineSyntax)
        {
            return true;
        }

        public virtual void closeDefine(DefineSyntax defineSyntax)
        {
        }

        public virtual bool openName(NameSyntax nameSyntax)
        {
            return true;
        }

        public virtual void closeName(NameSyntax nameSyntax)
        {
        }

        public virtual void visitExtension(ExtensionSyntax extensionSyntax)
        {
        }

        public virtual bool openNamespace(NamespaceSyntax namespaceSyntax)
        {
            return true;
        }

        public virtual void closeNamespace(NamespaceSyntax namespaceSyntax)
        {
        }

        public virtual bool openFunction(FunctionSyntax functionSyntax)
        {
            return true;
        }

        public virtual void closeFunction(FunctionSyntax functionSyntax)
        {
        }

        public virtual bool openProcedure(ProcedureSyntax procedureSyntax)
        {
            return true;
        }

        public virtual void closeProcedure(ProcedureSyntax procedureSyntax)
        {
        }

        public virtual bool openRoutine(RoutineSyntax routineSyntax)
        {
            return true;
        }

        public virtual void closeRoutine(RoutineSyntax routineSyntax)
        {
        }

        public virtual bool openLetDeclaration(LetDeclarationSyntax letDeclarationSyntax)
        {
            return true;
        }

        public virtual void closeLetDeclaration(LetDeclarationSyntax letDeclarationSyntax)
        {
        }

        public virtual bool openVarDeclaration(VarDeclarationSyntax varDeclarationSyntax)
        {
            return true;
        }

        public virtual void closeVarDeclaration(VarDeclarationSyntax varDeclarationSyntax)
        {
        }

        public virtual bool openMutableDeclaration(MutableDeclarationSyntax mutableDeclarationSyntax)
        {
            return true;
        }

        public virtual void closeMutableDeclaration(MutableDeclarationSyntax mutableDeclarationSyntax)
        {
        }

        public virtual bool openThreadLocalDeclaration(ThreadLocalDeclarationSyntax threadLocalDeclarationSyntax)
        {
            return true;
        }

        public virtual void closeThreadLocalDeclaration(ThreadLocalDeclarationSyntax threadLocalDeclarationSyntax)
        {
        }

        public virtual bool openLet(LetSyntax letSyntax)
        {
            return true;
        }

        public virtual void closeLet(LetSyntax letSyntax)
        {
        }

        public virtual bool openVar(VarSyntax varSyntax)
        {
            return true;
        }

        public virtual void closeVar(VarSyntax varSyntax)
        {
        }

        public virtual bool openMutable(MutableSyntax mutableSyntax)
        {
            return true;
        }

        public virtual void closeMutable(MutableSyntax mutableSyntax)
        {
        }

        public virtual bool openThreadLocal(ThreadLocalSyntax threadLocalSyntax)
        {
            return true;
        }

        public virtual void closeThreadLocal(ThreadLocalSyntax threadLocalSyntax)
        {
        }

        public virtual bool openBinding(BindingSyntax bindingSyntax)
        {
            return true;
        }

        public virtual void closeBinding(BindingSyntax bindingSyntax)
        {
        }

        public virtual bool openSet(SetSyntax setSyntax)
        {
            return true;
        }

        public virtual void closeSet(SetSyntax setSyntax)
        {
        }

        public virtual bool openCalculation(CalculationSyntax calculationSyntax)
        {
            return true;
        }

        public virtual void closeCalculation(CalculationSyntax calculationSyntax)
        {
        }

        public virtual bool openOperation(OperationSyntax operationSyntax)
        {
            return true;
        }

        public virtual void closeOperation(OperationSyntax operationSyntax)
        {
        }

        public virtual bool openOperand(OperandSyntax operandSyntax)
        {
            return true;
        }

        public virtual void closeOperand(OperandSyntax operandSyntax)
        {
        }

        public virtual void visitMemberAccess(MemberAccessSyntax memberAccessSyntax)
        {
        }

        public virtual bool openAs(AsSyntax asSyntax)
        {
            return true;
        }

        public virtual void closeAs(AsSyntax asSyntax)
        {
        }

        public virtual bool openIs(IsSyntax isSyntax)
        {
            return true;
        }

        public virtual void closeIs(IsSyntax isSyntax)
        {
        }

        public virtual void visitUnwrap(UnwrapSyntax unwrapSyntax)
        {
        }

        public virtual bool openCatch(CatchSyntax catchSyntax)
        {
            return true;
        }

        public virtual void closeCatch(CatchSyntax catchSyntax)
        {
        }

        public virtual bool openWildCardCatchPattern(WildCardCatchPatternSyntax wildCardCatchPatternSyntax)
        {
            return true;
        }

        public virtual void closeWildCardCatchPattern(WildCardCatchPatternSyntax wildCardCatchPatternSyntax)
        {
        }

        public virtual bool openTypeCatchPattern(TypeCatchPatternSyntax typeCatchPatternSyntax)
        {
            return true;
        }

        public virtual void closeTypeCatchPattern(TypeCatchPatternSyntax typeCatchPatternSyntax)
        {
        }

        public virtual bool openBlock(BlockSyntax blockSyntax)
        {
            return true;
        }

        public virtual void closeBlock(BlockSyntax blockSyntax)
        {
        }

        public virtual void visitConstant(ConstantSyntax constantSyntax)
        {
        }

        public virtual bool openIf(IfSyntax ifSyntax)
        {
            return true;
        }

        public virtual void closeIf(IfSyntax ifSyntax)
        {
        }

        public virtual bool openElse(ElseSyntax elseSyntax)
        {
            return true;
        }

        public virtual void closeElse(ElseSyntax elseSyntax)
        {
        }

        public virtual bool openSwitch(SwitchSyntax switchSyntax)
        {
            return true;
        }

        public virtual void closeSwitch(SwitchSyntax switchSyntax)
        {
        }

        public virtual bool openSwitchCase(SwitchCaseSyntax switchCaseSyntax)
        {
            return true;
        }

        public virtual void closeSwitchCase(SwitchCaseSyntax switchCaseSyntax)
        {
        }

        public virtual bool openItemCaseLabel(ItemCaseLabelSyntax itemCaseLabelSyntax)
        {
            return true;
        }

        public virtual void closeItemCaseLabel(ItemCaseLabelSyntax itemCaseLabelSyntax)
        {
        }

        public virtual bool openCaseItem(CaseItemSyntax caseItemSyntax)
        {
            return true;
        }

        public virtual void closeCaseItem(CaseItemSyntax caseItemSyntax)
        {
        }

        public virtual bool openConstantPattern(ConstantPatternSyntax constantPatternSyntax)
        {
            return true;
        }

        public virtual void closeConstantPattern(ConstantPatternSyntax constantPatternSyntax)
        {
        }

        public virtual void visitWildcardPattern(WildcardPatternSyntax wildcardPatternSyntax)
        {
        }

        public virtual bool openNamePattern(NamePatternSyntax namePatternSyntax)
        {
            return true;
        }

        public virtual void closeNamePattern(NamePatternSyntax namePatternSyntax)
        {
        }

        public virtual void visitDefaultCaseLabel(DefaultCaseLabelSyntax defaultCaseLabelSyntax)
        {
        }

        public virtual bool openFor(ForSyntax forSyntax)
        {
            return true;
        }

        public virtual void closeFor(ForSyntax forSyntax)
        {
        }

        public virtual bool openWhile(WhileSyntax whileSyntax)
        {
            return true;
        }

        public virtual void closeWhile(WhileSyntax whileSyntax)
        {
        }

        public virtual bool openDo(DoSyntax doSyntax)
        {
            return true;
        }

        public virtual void closeDo(DoSyntax doSyntax)
        {
        }

        public virtual bool openSimpleLoop(SimpleLoopSyntax simpleLoopSyntax)
        {
            return true;
        }

        public virtual void closeSimpleLoop(SimpleLoopSyntax simpleLoopSyntax)
        {
        }

        public virtual bool openNamedLoop(NamedLoopSyntax namedLoopSyntax)
        {
            return true;
        }

        public virtual void closeNamedLoop(NamedLoopSyntax namedLoopSyntax)
        {
        }

        public virtual void visitThis(ThisSyntax thisSyntax)
        {
        }

        public virtual bool openNew(NewSyntax newSyntax)
        {
            return true;
        }

        public virtual void closeNew(NewSyntax newSyntax)
        {
        }

        public virtual bool openObject(ObjectSyntax objectSyntax)
        {
            return true;
        }

        public virtual void closeObject(ObjectSyntax objectSyntax)
        {
        }

        public virtual bool openArray(ArraySyntax arraySyntax)
        {
            return true;
        }

        public virtual void closeArray(ArraySyntax arraySyntax)
        {
        }

        public virtual bool openItem(ItemSyntax itemSyntax)
        {
            return true;
        }

        public virtual void closeItem(ItemSyntax itemSyntax)
        {
        }

        public virtual bool openSizeOf(SizeOfSyntax sizeOfSyntax)
        {
            return true;
        }

        public virtual void closeSizeOf(SizeOfSyntax sizeOfSyntax)
        {
        }

        public virtual void visitBreak(BreakSyntax breakSyntax)
        {
        }

        public virtual void visitContinue(ContinueSyntax continueSyntax)
        {
        }

        public virtual bool openReturn(ReturnSyntax returnSyntax)
        {
            return true;
        }

        public virtual void closeReturn(ReturnSyntax returnSyntax)
        {
        }

        public virtual bool openThrow(ThrowSyntax throwSyntax)
        {
            return true;
        }

        public virtual void closeThrow(ThrowSyntax throwSyntax)
        {
        }

        public virtual bool openClass(ClassSyntax classSyntax)
        {
            return true;
        }

        public virtual void closeClass(ClassSyntax classSyntax)
        {
        }

        public virtual bool openGenericParameters(GenericParametersSyntax genericParametersSyntax)
        {
            return true;
        }

        public virtual void closeGenericParameters(GenericParametersSyntax genericParametersSyntax)
        {
        }

        public virtual void visitGenericParameter(GenericParameterSyntax genericParameterSyntax)
        {
        }

        public virtual bool openExtends(ExtendsSyntax extendsSyntax)
        {
            return true;
        }

        public virtual void closeExtends(ExtendsSyntax extendsSyntax)
        {
        }

        public virtual bool openStructure(StructureSyntax structureSyntax)
        {
            return true;
        }

        public virtual void closeStructure(StructureSyntax structureSyntax)
        {
        }

        public virtual bool openComponent(ComponentSyntax componentSyntax)
        {
            return true;
        }

        public virtual void closeComponent(ComponentSyntax componentSyntax)
        {
        }

        public virtual bool openClassBody(ClassBodySyntax classBodySyntax)
        {
            return true;
        }

        public virtual void closeClassBody(ClassBodySyntax classBodySyntax)
        {
        }

        public virtual bool openLetMember(LetMemberSyntax letMemberSyntax)
        {
            return true;
        }

        public virtual void closeLetMember(LetMemberSyntax letMemberSyntax)
        {
        }

        public virtual bool openVarMember(VarMemberSyntax varMemberSyntax)
        {
            return true;
        }

        public virtual void closeVarMember(VarMemberSyntax varMemberSyntax)
        {
        }

        public virtual bool openMutableMember(MutableMemberSyntax mutableMemberSyntax)
        {
            return true;
        }

        public virtual void closeMutableMember(MutableMemberSyntax mutableMemberSyntax)
        {
        }

        public virtual bool openSetInitialization(SetInitializationSyntax setInitializationSyntax)
        {
            return true;
        }

        public virtual void closeSetInitialization(SetInitializationSyntax setInitializationSyntax)
        {
        }

        public virtual bool openMethod(MethodSyntax methodSyntax)
        {
            return true;
        }

        public virtual void closeMethod(MethodSyntax methodSyntax)
        {
        }

        public virtual bool openStaticFunction(StaticFunctionSyntax staticFunctionSyntax)
        {
            return true;
        }

        public virtual void closeStaticFunction(StaticFunctionSyntax staticFunctionSyntax)
        {
        }

        public virtual bool openOperator(OperatorSyntax operatorSyntax)
        {
            return true;
        }

        public virtual void closeOperator(OperatorSyntax operatorSyntax)
        {
        }

        public virtual bool openInitializer(InitializerSyntax initializerSyntax)
        {
            return true;
        }

        public virtual void closeInitializer(InitializerSyntax initializerSyntax)
        {
        }

        public virtual bool openAllocator(AllocatorSyntax allocatorSyntax)
        {
            return true;
        }

        public virtual void closeAllocator(AllocatorSyntax allocatorSyntax)
        {
        }

        public virtual bool openTypeAnnotation(TypeAnnotationSyntax typeAnnotationSyntax)
        {
            return true;
        }

        public virtual void closeTypeAnnotation(TypeAnnotationSyntax typeAnnotationSyntax)
        {
        }

        public virtual bool openType(TypeSyntax typeSyntax)
        {
            return true;
        }

        public virtual void closeType(TypeSyntax typeSyntax)
        {
        }

        public virtual bool openVariant(VariantSyntax variantSyntax)
        {
            return true;
        }

        public virtual void closeVariant(VariantSyntax variantSyntax)
        {
        }

        public virtual bool openThrows(ThrowsSyntax throwsSyntax)
        {
            return true;
        }

        public virtual void closeThrows(ThrowsSyntax throwsSyntax)
        {
        }

        public virtual bool openGenericArguments(GenericArgumentsSyntax genericArgumentsSyntax)
        {
            return true;
        }

        public virtual void closeGenericArguments(GenericArgumentsSyntax genericArgumentsSyntax)
        {
        }

        public virtual bool openGenericArgument(GenericArgumentSyntax genericArgumentSyntax)
        {
            return true;
        }

        public virtual void closeGenericArgument(GenericArgumentSyntax genericArgumentSyntax)
        {
        }

        public virtual void visitOptional(OptionalSyntax optionalSyntax)
        {
        }

        public virtual void visitRoot(RootSyntax rootSyntax)
        {
        }

        public virtual void visitLocal(LocalSyntax localSyntax)
        {
        }

        public virtual void visitReference(ReferenceSyntax referenceSyntax)
        {
        }

        public virtual void visitThrown(ThrownSyntax thrownSyntax)
        {
        }
    }

    public abstract class SyntaxNode
    {
        public Position start;
        public Position end;
        public SyntaxNode parent;

        public abstract void accept(SyntaxVisitor visitor);
    }

    public class ProgramSyntax : SyntaxNode
    {
        public string name;
        public FileSyntax[] files;
        public ProgramSyntax(string name, FileSyntax[] files)
        {
            start = new Position(0, 0);
            end = new Position(0, 0);
            this.name = name;
            this.files = files;
            this.parent = null;
        }

        public override void accept(SyntaxVisitor visitor)
        {
            if (!visitor.openProgram(this))
                return;

            if (files != null)
            {
                foreach (FileSyntax node in files)
                    node.accept(visitor);
            }

            visitor.closeProgram(this);
        }
    }

    public class FileSyntax : SyntaxNode
    {
        public IntrinsicSyntax[] intrinsics;
        public UsingSyntax[] usings;
        public DefineSyntax[] defines;
        public DeclarationSyntax[] declarations;
        public StatementSyntax[] statements;
        public string fileName;
        public FileSyntax(Position start, Position end, IntrinsicSyntax[] intrinsics, UsingSyntax[] usings, DefineSyntax[] defines, DeclarationSyntax[] declarations, StatementSyntax[] statements)
        {
            this.start = start;
            this.end = end;
            this.intrinsics = intrinsics;
            this.usings = usings;
            this.defines = defines;
            this.declarations = declarations;
            this.statements = statements;
        }

        public override void accept(SyntaxVisitor visitor)
        {
            if (!visitor.openFile(this))
                return;

            if (intrinsics != null)
            {
                foreach (IntrinsicSyntax node in intrinsics)
                    node.accept(visitor);
            }

            if (usings != null)
            {
                foreach (UsingSyntax node in usings)
                    node.accept(visitor);
            }

            if (defines != null)
            {
                foreach (DefineSyntax node in defines)
                    node.accept(visitor);
            }

            if (declarations != null)
            {
                foreach (DeclarationSyntax node in declarations)
                    node.accept(visitor);
            }

            if (statements != null)
            {
                foreach (StatementSyntax node in statements)
                    node.accept(visitor);
            }

            visitor.closeFile(this);
        }
    }

    public class IntrinsicSyntax : SyntaxNode
    {
        public string name;
        public IntrinsicSyntax(Position start, Position end, string name)
        {
            this.start = start;
            this.end = end;
            this.name = name;
        }

        public override void accept(SyntaxVisitor visitor)
        {
            visitor.visitIntrinsic(this);
        }
    }

    public class UsingSyntax : SyntaxNode
    {
        public NameSyntax name;
        public UsingSyntax(Position start, Position end, NameSyntax name)
        {
            this.start = start;
            this.end = end;
            this.name = name;
        }

        public override void accept(SyntaxVisitor visitor)
        {
            if (!visitor.openUsing(this))
                return;

            name.accept(visitor);

            visitor.closeUsing(this);
        }
    }

    public class DefineSyntax : SyntaxNode
    {
        public NameSyntax name;
        public TypeSyntax typeSpec;
        public DefineSyntax(Position start, Position end, NameSyntax name, TypeSyntax typeSpec)
        {
            this.start = start;
            this.end = end;
            this.name = name;
            this.typeSpec = typeSpec;
        }

        public override void accept(SyntaxVisitor visitor)
        {
            if (!visitor.openDefine(this))
                return;

            name.accept(visitor);

            typeSpec.accept(visitor);

            visitor.closeDefine(this);
        }
    }

    public class NameSyntax : ExpressionSyntax
    {
        public string name;
        public ExtensionSyntax[] extensions;
        public NameSyntax(Position start, Position end, string name, ExtensionSyntax[] extensions)
        {
            this.start = start;
            this.end = end;
            this.name = name;
            this.extensions = extensions;
        }

        public override void accept(SyntaxVisitor visitor)
        {
            if (!visitor.openName(this))
                return;

            if (extensions != null)
            {
                foreach (ExtensionSyntax node in extensions)
                    node.accept(visitor);
            }

            visitor.closeName(this);
        }
    }

    public class ExtensionSyntax : SyntaxNode
    {
        public string name;
        public ExtensionSyntax(Position start, Position end, string name)
        {
            this.start = start;
            this.end = end;
            this.name = name;
        }

        public override void accept(SyntaxVisitor visitor)
        {
            visitor.visitExtension(this);
        }
    }

    public class DeclarationSyntax : SyntaxNode
    {
        public override void accept(SyntaxVisitor visitor)
        {
        }
    }

    public class NamespaceSyntax : DeclarationSyntax
    {
        public NameSyntax name;
        public UsingSyntax[] usings;
        public DefineSyntax[] defines;
        public DeclarationSyntax[] declarations;
        public NamespaceSyntax(Position start, Position end, NameSyntax name, UsingSyntax[] usings, DefineSyntax[] defines, DeclarationSyntax[] declarations)
        {
            this.start = start;
            this.end = end;
            this.name = name;
            this.usings = usings;
            this.defines = defines;
            this.declarations = declarations;
        }

        public override void accept(SyntaxVisitor visitor)
        {
            if (!visitor.openNamespace(this))
                return;

            name.accept(visitor);

            if (usings != null)
            {
                foreach (UsingSyntax node in usings)
                    node.accept(visitor);
            }

            if (defines != null)
            {
                foreach (DefineSyntax node in defines)
                    node.accept(visitor);
            }

            if (declarations != null)
            {
                foreach (DeclarationSyntax node in declarations)
                    node.accept(visitor);
            }

            visitor.closeNamespace(this);
        }
    }

    public class FunctionSyntax : DeclarationSyntax
    {
        public ProcedureSyntax procedure;
        public FunctionSyntax(Position start, Position end, ProcedureSyntax procedure)
        {
            this.start = start;
            this.end = end;
            this.procedure = procedure;
        }

        public override void accept(SyntaxVisitor visitor)
        {
            if (!visitor.openFunction(this))
                return;

            procedure.accept(visitor);

            visitor.closeFunction(this);
        }
    }

    public class ProcedureSyntax : SyntaxNode
    {
        public string name;
        public RoutineSyntax routine;
        public ProcedureSyntax(Position start, Position end, string name, RoutineSyntax routine)
        {
            this.start = start;
            this.end = end;
            this.name = name;
            this.routine = routine;
        }

        public override void accept(SyntaxVisitor visitor)
        {
            if (!visitor.openProcedure(this))
                return;

            routine.accept(visitor);

            visitor.closeProcedure(this);
        }
    }

    public class RoutineSyntax : SyntaxNode
    {
        public StructureSyntax input;
        public TypeAnnotationSyntax output;
        public ThrowsSyntax throwsClause;
        public BlockSyntax body;
        public RoutineSyntax(Position start, Position end, StructureSyntax input, TypeAnnotationSyntax output, ThrowsSyntax throwsClause, BlockSyntax body)
        {
            this.start = start;
            this.end = end;
            this.input = input;
            this.output = output;
            this.throwsClause = throwsClause;
            this.body = body;
        }

        public override void accept(SyntaxVisitor visitor)
        {
            if (!visitor.openRoutine(this))
                return;

            if (input != null)
                input.accept(visitor);

            if (output != null)
                output.accept(visitor);

            if (throwsClause != null)
                throwsClause.accept(visitor);

            body.accept(visitor);

            visitor.closeRoutine(this);
        }
    }

    public class LetDeclarationSyntax : DeclarationSyntax
    {
        public LetSyntax declaration;
        public LetDeclarationSyntax(Position start, Position end, LetSyntax declaration)
        {
            this.start = start;
            this.end = end;
            this.declaration = declaration;
        }

        public override void accept(SyntaxVisitor visitor)
        {
            if (!visitor.openLetDeclaration(this))
                return;

            declaration.accept(visitor);

            visitor.closeLetDeclaration(this);
        }
    }

    public class VarDeclarationSyntax : DeclarationSyntax
    {
        public VarSyntax declaration;
        public VarDeclarationSyntax(Position start, Position end, VarSyntax declaration)
        {
            this.start = start;
            this.end = end;
            this.declaration = declaration;
        }

        public override void accept(SyntaxVisitor visitor)
        {
            if (!visitor.openVarDeclaration(this))
                return;

            declaration.accept(visitor);

            visitor.closeVarDeclaration(this);
        }
    }

    public class MutableDeclarationSyntax : DeclarationSyntax
    {
        public MutableSyntax declaration;
        public MutableDeclarationSyntax(Position start, Position end, MutableSyntax declaration)
        {
            this.start = start;
            this.end = end;
            this.declaration = declaration;
        }

        public override void accept(SyntaxVisitor visitor)
        {
            if (!visitor.openMutableDeclaration(this))
                return;

            declaration.accept(visitor);

            visitor.closeMutableDeclaration(this);
        }
    }

    public class ThreadLocalDeclarationSyntax : DeclarationSyntax
    {
        public ThreadLocalSyntax declaration;
        public ThreadLocalDeclarationSyntax(Position start, Position end, ThreadLocalSyntax declaration)
        {
            this.start = start;
            this.end = end;
            this.declaration = declaration;
        }

        public override void accept(SyntaxVisitor visitor)
        {
            if (!visitor.openThreadLocalDeclaration(this))
                return;

            declaration.accept(visitor);

            visitor.closeThreadLocalDeclaration(this);
        }
    }

    public class StatementSyntax : SyntaxNode
    {
        public override void accept(SyntaxVisitor visitor)
        {
        }
    }

    public class LetSyntax : StatementSyntax
    {
        public BindingSyntax binding;
        public LetSyntax(Position start, Position end, BindingSyntax binding)
        {
            this.start = start;
            this.end = end;
            this.binding = binding;
        }

        public override void accept(SyntaxVisitor visitor)
        {
            if (!visitor.openLet(this))
                return;

            binding.accept(visitor);

            visitor.closeLet(this);
        }
    }

    public class VarSyntax : StatementSyntax
    {
        public BindingSyntax binding;
        public VarSyntax(Position start, Position end, BindingSyntax binding)
        {
            this.start = start;
            this.end = end;
            this.binding = binding;
        }

        public override void accept(SyntaxVisitor visitor)
        {
            if (!visitor.openVar(this))
                return;

            binding.accept(visitor);

            visitor.closeVar(this);
        }
    }

    public class MutableSyntax : StatementSyntax
    {
        public BindingSyntax binding;
        public MutableSyntax(Position start, Position end, BindingSyntax binding)
        {
            this.start = start;
            this.end = end;
            this.binding = binding;
        }

        public override void accept(SyntaxVisitor visitor)
        {
            if (!visitor.openMutable(this))
                return;

            binding.accept(visitor);

            visitor.closeMutable(this);
        }
    }

    public class ThreadLocalSyntax : StatementSyntax
    {
        public BindingSyntax binding;
        public ThreadLocalSyntax(Position start, Position end, BindingSyntax binding)
        {
            this.start = start;
            this.end = end;
            this.binding = binding;
        }

        public override void accept(SyntaxVisitor visitor)
        {
            if (!visitor.openThreadLocal(this))
                return;

            binding.accept(visitor);

            visitor.closeThreadLocal(this);
        }
    }

    public class BindingSyntax : SyntaxNode
    {
        public string name;
        public TypeAnnotationSyntax typeAnnotation;
        public CalculationSyntax calculation;
        public BindingSyntax(Position start, Position end, string name, TypeAnnotationSyntax typeAnnotation, CalculationSyntax calculation)
        {
            this.start = start;
            this.end = end;
            this.name = name;
            this.typeAnnotation = typeAnnotation;
            this.calculation = calculation;
        }

        public override void accept(SyntaxVisitor visitor)
        {
            if (!visitor.openBinding(this))
                return;

            if (typeAnnotation != null)
                typeAnnotation.accept(visitor);

            calculation.accept(visitor);

            visitor.closeBinding(this);
        }
    }

    public class SetSyntax : StatementSyntax
    {
        public OperationSyntax lValue;
        public CalculationSyntax rValue;
        public SetSyntax(Position start, Position end, OperationSyntax lValue, CalculationSyntax rValue)
        {
            this.start = start;
            this.end = end;
            this.lValue = lValue;
            this.rValue = rValue;
        }

        public override void accept(SyntaxVisitor visitor)
        {
            if (!visitor.openSet(this))
                return;

            lValue.accept(visitor);

            rValue.accept(visitor);

            visitor.closeSet(this);
        }
    }

    public class CalculationSyntax : StatementSyntax
    {
        public OperationSyntax operation;
        public CalculationSyntax(Position start, Position end, OperationSyntax operation)
        {
            this.start = start;
            this.end = end;
            this.operation = operation;
        }

        public override void accept(SyntaxVisitor visitor)
        {
            if (!visitor.openCalculation(this))
                return;

            operation.accept(visitor);

            visitor.closeCalculation(this);
        }
    }

    public class OperationSyntax : SyntaxNode
    {
        public OperandSyntax[] op;
        public OperationSyntax(Position start, Position end, OperandSyntax[] op)
        {
            this.start = start;
            this.end = end;
            this.op = op;
        }

        public override void accept(SyntaxVisitor visitor)
        {
            if (!visitor.openOperation(this))
                return;

            if (op != null)
            {
                foreach (OperandSyntax node in op)
                    node.accept(visitor);
            }

            visitor.closeOperation(this);
        }
    }

    public class OperandSyntax : StatementSyntax
    {
        public ExpressionSyntax primary;
        public PostfixSyntax[] postfixes;
        public OperandSyntax(Position start, Position end, ExpressionSyntax primary, PostfixSyntax[] postfixes)
        {
            this.start = start;
            this.end = end;
            this.primary = primary;
            this.postfixes = postfixes;
        }

        public override void accept(SyntaxVisitor visitor)
        {
            if (!visitor.openOperand(this))
                return;

            primary.accept(visitor);

            if (postfixes != null)
            {
                foreach (PostfixSyntax node in postfixes)
                    node.accept(visitor);
            }

            visitor.closeOperand(this);
        }
    }

    public class PostfixSyntax : SyntaxNode
    {
        public override void accept(SyntaxVisitor visitor)
        {
        }
    }

    public class MemberAccessSyntax : PostfixSyntax
    {
        public string member;
        public MemberAccessSyntax(Position start, Position end, string member)
        {
            this.start = start;
            this.end = end;
            this.member = member;
        }

        public override void accept(SyntaxVisitor visitor)
        {
            visitor.visitMemberAccess(this);
        }
    }

    public class AsSyntax : PostfixSyntax
    {
        public TypeSpecSyntax typeSpec;
        public AsSyntax(Position start, Position end, TypeSpecSyntax typeSpec)
        {
            this.start = start;
            this.end = end;
            this.typeSpec = typeSpec;
        }

        public override void accept(SyntaxVisitor visitor)
        {
            if (!visitor.openAs(this))
                return;

            typeSpec.accept(visitor);

            visitor.closeAs(this);
        }
    }

    public class IsSyntax : PostfixSyntax
    {
        public TypeSpecSyntax typeSpec;
        public IsSyntax(Position start, Position end, TypeSpecSyntax typeSpec)
        {
            this.start = start;
            this.end = end;
            this.typeSpec = typeSpec;
        }

        public override void accept(SyntaxVisitor visitor)
        {
            if (!visitor.openIs(this))
                return;

            typeSpec.accept(visitor);

            visitor.closeIs(this);
        }
    }

    public class UnwrapSyntax : PostfixSyntax
    {
        public UnwrapSyntax(Position start, Position end)
        {
            this.start = start;
            this.end = end;
        }

        public override void accept(SyntaxVisitor visitor)
        {
            visitor.visitUnwrap(this);
        }
    }

    public class CatchSyntax : PostfixSyntax
    {
        public CatchPatternSyntax typeSpec;
        public BlockSyntax handler;
        public CatchSyntax(Position start, Position end, CatchPatternSyntax typeSpec, BlockSyntax handler)
        {
            this.start = start;
            this.end = end;
            this.typeSpec = typeSpec;
            this.handler = handler;
        }

        public override void accept(SyntaxVisitor visitor)
        {
            if (!visitor.openCatch(this))
                return;

            typeSpec.accept(visitor);

            handler.accept(visitor);

            visitor.closeCatch(this);
        }
    }

    public class CatchPatternSyntax : SyntaxNode
    {
        public override void accept(SyntaxVisitor visitor)
        {
        }
    }

    public class WildCardCatchPatternSyntax : CatchPatternSyntax
    {
        public WildcardPatternSyntax pattern;
        public WildCardCatchPatternSyntax(Position start, Position end, WildcardPatternSyntax pattern)
        {
            this.start = start;
            this.end = end;
            this.pattern = pattern;
        }

        public override void accept(SyntaxVisitor visitor)
        {
            if (!visitor.openWildCardCatchPattern(this))
                return;

            pattern.accept(visitor);

            visitor.closeWildCardCatchPattern(this);
        }
    }

    public class TypeCatchPatternSyntax : CatchPatternSyntax
    {
        public TypeSpecSyntax typeSpec;
        public string errorName;
        public TypeCatchPatternSyntax(Position start, Position end, TypeSpecSyntax typeSpec, string errorName)
        {
            this.start = start;
            this.end = end;
            this.typeSpec = typeSpec;
            this.errorName = errorName;
        }

        public override void accept(SyntaxVisitor visitor)
        {
            if (!visitor.openTypeCatchPattern(this))
                return;

            typeSpec.accept(visitor);

            visitor.closeTypeCatchPattern(this);
        }
    }

    public class ExpressionSyntax : SyntaxNode
    {
        public override void accept(SyntaxVisitor visitor)
        {
        }
    }

    public class BlockSyntax : ExpressionSyntax
    {
        public StatementSyntax[] statements;
        public BlockSyntax(Position start, Position end, StatementSyntax[] statements)
        {
            this.start = start;
            this.end = end;
            this.statements = statements;
        }

        public override void accept(SyntaxVisitor visitor)
        {
            if (!visitor.openBlock(this))
                return;

            if (statements != null)
            {
                foreach (StatementSyntax node in statements)
                    node.accept(visitor);
            }

            visitor.closeBlock(this);
        }
    }

    public class ConstantSyntax : ExpressionSyntax
    {
        public Literal literal;
        public ConstantSyntax(Position start, Position end, Literal literal)
        {
            this.start = start;
            this.end = end;
            this.literal = literal;
        }

        public override void accept(SyntaxVisitor visitor)
        {
            visitor.visitConstant(this);
        }
    }

    public class IfSyntax : ExpressionSyntax
    {
        public OperationSyntax condition;
        public BlockSyntax consequent;
        public ElseSyntax elseClause;
        public IfSyntax(Position start, Position end, OperationSyntax condition, BlockSyntax consequent, ElseSyntax elseClause)
        {
            this.start = start;
            this.end = end;
            this.condition = condition;
            this.consequent = consequent;
            this.elseClause = elseClause;
        }

        public override void accept(SyntaxVisitor visitor)
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

    public class ElseSyntax : SyntaxNode
    {
        public BlockSyntax alternative;
        public ElseSyntax(Position start, Position end, BlockSyntax alternative)
        {
            this.start = start;
            this.end = end;
            this.alternative = alternative;
        }

        public override void accept(SyntaxVisitor visitor)
        {
            if (!visitor.openElse(this))
                return;

            alternative.accept(visitor);

            visitor.closeElse(this);
        }
    }

    public class SwitchSyntax : ExpressionSyntax
    {
        public OperationSyntax condition;
        public SwitchCaseSyntax[] cases;
        public SwitchSyntax(Position start, Position end, OperationSyntax condition, SwitchCaseSyntax[] cases)
        {
            this.start = start;
            this.end = end;
            this.condition = condition;
            this.cases = cases;
        }

        public override void accept(SyntaxVisitor visitor)
        {
            if (!visitor.openSwitch(this))
                return;

            condition.accept(visitor);

            if (cases != null)
            {
                foreach (SwitchCaseSyntax node in cases)
                    node.accept(visitor);
            }

            visitor.closeSwitch(this);
        }
    }

    public class SwitchCaseSyntax : SyntaxNode
    {
        public CaseLabelSyntax label;
        public BlockSyntax content;
        public SwitchCaseSyntax(Position start, Position end, CaseLabelSyntax label, BlockSyntax content)
        {
            this.start = start;
            this.end = end;
            this.label = label;
            this.content = content;
        }

        public override void accept(SyntaxVisitor visitor)
        {
            if (!visitor.openSwitchCase(this))
                return;

            label.accept(visitor);

            content.accept(visitor);

            visitor.closeSwitchCase(this);
        }
    }

    public class CaseLabelSyntax : SyntaxNode
    {
        public override void accept(SyntaxVisitor visitor)
        {
        }
    }

    public class ItemCaseLabelSyntax : CaseLabelSyntax
    {
        public CaseItemSyntax[] items;
        public ItemCaseLabelSyntax(Position start, Position end, CaseItemSyntax[] items)
        {
            this.start = start;
            this.end = end;
            this.items = items;
        }

        public override void accept(SyntaxVisitor visitor)
        {
            if (!visitor.openItemCaseLabel(this))
                return;

            if (items != null)
            {
                foreach (CaseItemSyntax node in items)
                    node.accept(visitor);
            }

            visitor.closeItemCaseLabel(this);
        }
    }

    public class CaseItemSyntax : SyntaxNode
    {
        public CasePatternSyntax pattern;
        public CaseItemSyntax(Position start, Position end, CasePatternSyntax pattern)
        {
            this.start = start;
            this.end = end;
            this.pattern = pattern;
        }

        public override void accept(SyntaxVisitor visitor)
        {
            if (!visitor.openCaseItem(this))
                return;

            pattern.accept(visitor);

            visitor.closeCaseItem(this);
        }
    }

    public class CasePatternSyntax : SyntaxNode
    {
        public override void accept(SyntaxVisitor visitor)
        {
        }
    }

    public class ConstantPatternSyntax : CasePatternSyntax
    {
        public ConstantSyntax constant;
        public ConstantPatternSyntax(Position start, Position end, ConstantSyntax constant)
        {
            this.start = start;
            this.end = end;
            this.constant = constant;
        }

        public override void accept(SyntaxVisitor visitor)
        {
            if (!visitor.openConstantPattern(this))
                return;

            constant.accept(visitor);

            visitor.closeConstantPattern(this);
        }
    }

    public class WildcardPatternSyntax : CasePatternSyntax
    {
        public WildcardPatternSyntax(Position start, Position end)
        {
            this.start = start;
            this.end = end;
        }

        public override void accept(SyntaxVisitor visitor)
        {
            visitor.visitWildcardPattern(this);
        }
    }

    public class NamePatternSyntax : CasePatternSyntax
    {
        public NameSyntax name;
        public NamePatternSyntax(Position start, Position end, NameSyntax name)
        {
            this.start = start;
            this.end = end;
            this.name = name;
        }

        public override void accept(SyntaxVisitor visitor)
        {
            if (!visitor.openNamePattern(this))
                return;

            name.accept(visitor);

            visitor.closeNamePattern(this);
        }
    }

    public class DefaultCaseLabelSyntax : CaseLabelSyntax
    {
        public DefaultCaseLabelSyntax(Position start, Position end)
        {
            this.start = start;
            this.end = end;
        }

        public override void accept(SyntaxVisitor visitor)
        {
            visitor.visitDefaultCaseLabel(this);
        }
    }

    public class ForSyntax : ExpressionSyntax
    {
        public string index;
        public TypeAnnotationSyntax typeAnnotation;
        public OperationSyntax operation;
        public LoopSyntax iteration;
        public ForSyntax(Position start, Position end, string index, TypeAnnotationSyntax typeAnnotation, OperationSyntax operation, LoopSyntax iteration)
        {
            this.start = start;
            this.end = end;
            this.index = index;
            this.typeAnnotation = typeAnnotation;
            this.operation = operation;
            this.iteration = iteration;
        }

        public override void accept(SyntaxVisitor visitor)
        {
            if (!visitor.openFor(this))
                return;

            if (typeAnnotation != null)
                typeAnnotation.accept(visitor);

            operation.accept(visitor);

            iteration.accept(visitor);

            visitor.closeFor(this);
        }
    }

    public class WhileSyntax : ExpressionSyntax
    {
        public OperationSyntax condition;
        public LoopSyntax iteration;
        public WhileSyntax(Position start, Position end, OperationSyntax condition, LoopSyntax iteration)
        {
            this.start = start;
            this.end = end;
            this.condition = condition;
            this.iteration = iteration;
        }

        public override void accept(SyntaxVisitor visitor)
        {
            if (!visitor.openWhile(this))
                return;

            condition.accept(visitor);

            iteration.accept(visitor);

            visitor.closeWhile(this);
        }
    }

    public class DoSyntax : ExpressionSyntax
    {
        public LoopSyntax iteration;
        public OperationSyntax condition;
        public DoSyntax(Position start, Position end, LoopSyntax iteration, OperationSyntax condition)
        {
            this.start = start;
            this.end = end;
            this.iteration = iteration;
            this.condition = condition;
        }

        public override void accept(SyntaxVisitor visitor)
        {
            if (!visitor.openDo(this))
                return;

            iteration.accept(visitor);

            condition.accept(visitor);

            visitor.closeDo(this);
        }
    }

    public class LoopSyntax : SyntaxNode
    {
        public override void accept(SyntaxVisitor visitor)
        {
        }
    }

    public class SimpleLoopSyntax : LoopSyntax
    {
        public BlockSyntax code;
        public SimpleLoopSyntax(Position start, Position end, BlockSyntax code)
        {
            this.start = start;
            this.end = end;
            this.code = code;
        }

        public override void accept(SyntaxVisitor visitor)
        {
            if (!visitor.openSimpleLoop(this))
                return;

            code.accept(visitor);

            visitor.closeSimpleLoop(this);
        }
    }

    public class NamedLoopSyntax : LoopSyntax
    {
        public string name;
        public BlockSyntax code;
        public NamedLoopSyntax(Position start, Position end, string name, BlockSyntax code)
        {
            this.start = start;
            this.end = end;
            this.name = name;
            this.code = code;
        }

        public override void accept(SyntaxVisitor visitor)
        {
            if (!visitor.openNamedLoop(this))
                return;

            code.accept(visitor);

            visitor.closeNamedLoop(this);
        }
    }

    public class ThisSyntax : ExpressionSyntax
    {
        public ThisSyntax(Position start, Position end)
        {
            this.start = start;
            this.end = end;
        }

        public override void accept(SyntaxVisitor visitor)
        {
            visitor.visitThis(this);
        }
    }

    public class NewSyntax : ExpressionSyntax
    {
        public TypeSyntax typeSpec;
        public NewSyntax(Position start, Position end, TypeSyntax typeSpec)
        {
            this.start = start;
            this.end = end;
            this.typeSpec = typeSpec;
        }

        public override void accept(SyntaxVisitor visitor)
        {
            if (!visitor.openNew(this))
                return;

            typeSpec.accept(visitor);

            visitor.closeNew(this);
        }
    }

    public class ObjectSyntax : ExpressionSyntax
    {
        public OperationSyntax firstOp;
        public ItemSyntax[] additionalOps;
        public ObjectSyntax(Position start, Position end, OperationSyntax firstOp, ItemSyntax[] additionalOps)
        {
            this.start = start;
            this.end = end;
            this.firstOp = firstOp;
            this.additionalOps = additionalOps;
        }

        public override void accept(SyntaxVisitor visitor)
        {
            if (!visitor.openObject(this))
                return;

            if (firstOp != null)
                firstOp.accept(visitor);

            if (additionalOps != null)
            {
                foreach (ItemSyntax node in additionalOps)
                    node.accept(visitor);
            }

            visitor.closeObject(this);
        }
    }

    public class ArraySyntax : ExpressionSyntax
    {
        public OperationSyntax firstOp;
        public ItemSyntax[] additionalOps;
        public ArraySyntax(Position start, Position end, OperationSyntax firstOp, ItemSyntax[] additionalOps)
        {
            this.start = start;
            this.end = end;
            this.firstOp = firstOp;
            this.additionalOps = additionalOps;
        }

        public override void accept(SyntaxVisitor visitor)
        {
            if (!visitor.openArray(this))
                return;

            if (firstOp != null)
                firstOp.accept(visitor);

            if (additionalOps != null)
            {
                foreach (ItemSyntax node in additionalOps)
                    node.accept(visitor);
            }

            visitor.closeArray(this);
        }
    }

    public class ItemSyntax : SyntaxNode
    {
        public OperationSyntax operation;
        public ItemSyntax(Position start, Position end, OperationSyntax operation)
        {
            this.start = start;
            this.end = end;
            this.operation = operation;
        }

        public override void accept(SyntaxVisitor visitor)
        {
            if (!visitor.openItem(this))
                return;

            operation.accept(visitor);

            visitor.closeItem(this);
        }
    }

    public class SizeOfSyntax : ExpressionSyntax
    {
        public TypeSyntax typeSpec;
        public SizeOfSyntax(Position start, Position end, TypeSyntax typeSpec)
        {
            this.start = start;
            this.end = end;
            this.typeSpec = typeSpec;
        }

        public override void accept(SyntaxVisitor visitor)
        {
            if (!visitor.openSizeOf(this))
                return;

            typeSpec.accept(visitor);

            visitor.closeSizeOf(this);
        }
    }

    public class BreakSyntax : StatementSyntax
    {
        public string iteration;
        public BreakSyntax(Position start, Position end, string iteration)
        {
            this.start = start;
            this.end = end;
            this.iteration = iteration;
        }

        public override void accept(SyntaxVisitor visitor)
        {
            visitor.visitBreak(this);
        }
    }

    public class ContinueSyntax : StatementSyntax
    {
        public string iteration;
        public ContinueSyntax(Position start, Position end, string iteration)
        {
            this.start = start;
            this.end = end;
            this.iteration = iteration;
        }

        public override void accept(SyntaxVisitor visitor)
        {
            visitor.visitContinue(this);
        }
    }

    public class ReturnSyntax : StatementSyntax
    {
        public CalculationSyntax result;
        public ReturnSyntax(Position start, Position end, CalculationSyntax result)
        {
            this.start = start;
            this.end = end;
            this.result = result;
        }

        public override void accept(SyntaxVisitor visitor)
        {
            if (!visitor.openReturn(this))
                return;

            if (result != null)
                result.accept(visitor);

            visitor.closeReturn(this);
        }
    }

    public class ThrowSyntax : StatementSyntax
    {
        public CalculationSyntax exception;
        public ThrowSyntax(Position start, Position end, CalculationSyntax exception)
        {
            this.start = start;
            this.end = end;
            this.exception = exception;
        }

        public override void accept(SyntaxVisitor visitor)
        {
            if (!visitor.openThrow(this))
                return;

            exception.accept(visitor);

            visitor.closeThrow(this);
        }
    }

    public class ClassSyntax : DeclarationSyntax
    {
        public NameSyntax name;
        public GenericParametersSyntax generics;
        public ExtendsSyntax baseClass;
        public StructureSyntax contents;
        public ClassBodySyntax body;
        public ClassSyntax(Position start, Position end, NameSyntax name, GenericParametersSyntax generics, ExtendsSyntax baseClass, StructureSyntax contents, ClassBodySyntax body)
        {
            this.start = start;
            this.end = end;
            this.name = name;
            this.generics = generics;
            this.baseClass = baseClass;
            this.contents = contents;
            this.body = body;
        }

        public override void accept(SyntaxVisitor visitor)
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

    public class GenericParametersSyntax : SyntaxNode
    {
        public string name;
        public GenericParameterSyntax[] additionalGenerics;
        public GenericParametersSyntax(Position start, Position end, string name, GenericParameterSyntax[] additionalGenerics)
        {
            this.start = start;
            this.end = end;
            this.name = name;
            this.additionalGenerics = additionalGenerics;
        }

        public override void accept(SyntaxVisitor visitor)
        {
            if (!visitor.openGenericParameters(this))
                return;

            if (additionalGenerics != null)
            {
                foreach (GenericParameterSyntax node in additionalGenerics)
                    node.accept(visitor);
            }

            visitor.closeGenericParameters(this);
        }
    }

    public class GenericParameterSyntax : SyntaxNode
    {
        public string name;
        public GenericParameterSyntax(Position start, Position end, string name)
        {
            this.start = start;
            this.end = end;
            this.name = name;
        }

        public override void accept(SyntaxVisitor visitor)
        {
            visitor.visitGenericParameter(this);
        }
    }

    public class ExtendsSyntax : SyntaxNode
    {
        public NameSyntax name;
        public ExtendsSyntax(Position start, Position end, NameSyntax name)
        {
            this.start = start;
            this.end = end;
            this.name = name;
        }

        public override void accept(SyntaxVisitor visitor)
        {
            if (!visitor.openExtends(this))
                return;

            name.accept(visitor);

            visitor.closeExtends(this);
        }
    }

    public class StructureSyntax : SyntaxNode
    {
        public ComponentSyntax[] components;
        public StructureSyntax(Position start, Position end, ComponentSyntax[] components)
        {
            this.start = start;
            this.end = end;
            this.components = components;
        }

        public override void accept(SyntaxVisitor visitor)
        {
            if (!visitor.openStructure(this))
                return;

            if (components != null)
            {
                foreach (ComponentSyntax node in components)
                    node.accept(visitor);
            }

            visitor.closeStructure(this);
        }
    }

    public class ComponentSyntax : SyntaxNode
    {
        public string name;
        public TypeAnnotationSyntax typeAnnotation;
        public ComponentSyntax(Position start, Position end, string name, TypeAnnotationSyntax typeAnnotation)
        {
            this.start = start;
            this.end = end;
            this.name = name;
            this.typeAnnotation = typeAnnotation;
        }

        public override void accept(SyntaxVisitor visitor)
        {
            if (!visitor.openComponent(this))
                return;

            if (typeAnnotation != null)
                typeAnnotation.accept(visitor);

            visitor.closeComponent(this);
        }
    }

    public class ClassBodySyntax : SyntaxNode
    {
        public ClassMemberSyntax[] members;
        public ClassBodySyntax(Position start, Position end, ClassMemberSyntax[] members)
        {
            this.start = start;
            this.end = end;
            this.members = members;
        }

        public override void accept(SyntaxVisitor visitor)
        {
            if (!visitor.openClassBody(this))
                return;

            if (members != null)
            {
                foreach (ClassMemberSyntax node in members)
                    node.accept(visitor);
            }

            visitor.closeClassBody(this);
        }
    }

    public class ClassMemberSyntax : SyntaxNode
    {
        public override void accept(SyntaxVisitor visitor)
        {
        }
    }

    public class LetMemberSyntax : ClassMemberSyntax
    {
        public LetSyntax declaration;
        public LetMemberSyntax(Position start, Position end, LetSyntax declaration)
        {
            this.start = start;
            this.end = end;
            this.declaration = declaration;
        }

        public override void accept(SyntaxVisitor visitor)
        {
            if (!visitor.openLetMember(this))
                return;

            declaration.accept(visitor);

            visitor.closeLetMember(this);
        }
    }

    public class VarMemberSyntax : ClassMemberSyntax
    {
        public VarSyntax declaration;
        public VarMemberSyntax(Position start, Position end, VarSyntax declaration)
        {
            this.start = start;
            this.end = end;
            this.declaration = declaration;
        }

        public override void accept(SyntaxVisitor visitor)
        {
            if (!visitor.openVarMember(this))
                return;

            declaration.accept(visitor);

            visitor.closeVarMember(this);
        }
    }

    public class MutableMemberSyntax : ClassMemberSyntax
    {
        public MutableSyntax declaration;
        public MutableMemberSyntax(Position start, Position end, MutableSyntax declaration)
        {
            this.start = start;
            this.end = end;
            this.declaration = declaration;
        }

        public override void accept(SyntaxVisitor visitor)
        {
            if (!visitor.openMutableMember(this))
                return;

            declaration.accept(visitor);

            visitor.closeMutableMember(this);
        }
    }

    public class SetInitializationSyntax : ClassMemberSyntax
    {
        public SetSyntax definition;
        public SetInitializationSyntax(Position start, Position end, SetSyntax definition)
        {
            this.start = start;
            this.end = end;
            this.definition = definition;
        }

        public override void accept(SyntaxVisitor visitor)
        {
            if (!visitor.openSetInitialization(this))
                return;

            definition.accept(visitor);

            visitor.closeSetInitialization(this);
        }
    }

    public class MethodSyntax : ClassMemberSyntax
    {
        public ProcedureSyntax procedure;
        public MethodSyntax(Position start, Position end, ProcedureSyntax procedure)
        {
            this.start = start;
            this.end = end;
            this.procedure = procedure;
        }

        public override void accept(SyntaxVisitor visitor)
        {
            if (!visitor.openMethod(this))
                return;

            procedure.accept(visitor);

            visitor.closeMethod(this);
        }
    }

    public class StaticFunctionSyntax : ClassMemberSyntax
    {
        public FunctionSyntax procedure;
        public StaticFunctionSyntax(Position start, Position end, FunctionSyntax procedure)
        {
            this.start = start;
            this.end = end;
            this.procedure = procedure;
        }

        public override void accept(SyntaxVisitor visitor)
        {
            if (!visitor.openStaticFunction(this))
                return;

            procedure.accept(visitor);

            visitor.closeStaticFunction(this);
        }
    }

    public class OperatorSyntax : ClassMemberSyntax
    {
        public RoutineSyntax routine;
        public OperatorSyntax(Position start, Position end, RoutineSyntax routine)
        {
            this.start = start;
            this.end = end;
            this.routine = routine;
        }

        public override void accept(SyntaxVisitor visitor)
        {
            if (!visitor.openOperator(this))
                return;

            routine.accept(visitor);

            visitor.closeOperator(this);
        }
    }

    public class InitializerSyntax : ClassMemberSyntax
    {
        public StructureSyntax input;
        public BlockSyntax body;
        public InitializerSyntax(Position start, Position end, StructureSyntax input, BlockSyntax body)
        {
            this.start = start;
            this.end = end;
            this.input = input;
            this.body = body;
        }

        public override void accept(SyntaxVisitor visitor)
        {
            if (!visitor.openInitializer(this))
                return;

            if (input != null)
                input.accept(visitor);

            body.accept(visitor);

            visitor.closeInitializer(this);
        }
    }

    public class AllocatorSyntax : ClassMemberSyntax
    {
        public StructureSyntax input;
        public BlockSyntax body;
        public AllocatorSyntax(Position start, Position end, StructureSyntax input, BlockSyntax body)
        {
            this.start = start;
            this.end = end;
            this.input = input;
            this.body = body;
        }

        public override void accept(SyntaxVisitor visitor)
        {
            if (!visitor.openAllocator(this))
                return;

            if (input != null)
                input.accept(visitor);

            body.accept(visitor);

            visitor.closeAllocator(this);
        }
    }

    public class TypeAnnotationSyntax : SyntaxNode
    {
        public TypeSpecSyntax typeSpec;
        public TypeAnnotationSyntax(Position start, Position end, TypeSpecSyntax typeSpec)
        {
            this.start = start;
            this.end = end;
            this.typeSpec = typeSpec;
        }

        public override void accept(SyntaxVisitor visitor)
        {
            if (!visitor.openTypeAnnotation(this))
                return;

            typeSpec.accept(visitor);

            visitor.closeTypeAnnotation(this);
        }
    }

    public class TypeSpecSyntax : SyntaxNode
    {
        public override void accept(SyntaxVisitor visitor)
        {
        }
    }

    public class TypeSyntax : TypeSpecSyntax
    {
        public NameSyntax name;
        public GenericArgumentsSyntax generics;
        public OptionalSyntax optional;
        public LifeTimeSyntax lifeTime;
        public TypeSyntax(Position start, Position end, NameSyntax name, GenericArgumentsSyntax generics, OptionalSyntax optional, LifeTimeSyntax lifeTime)
        {
            this.start = start;
            this.end = end;
            this.name = name;
            this.generics = generics;
            this.optional = optional;
            this.lifeTime = lifeTime;
        }

        public override void accept(SyntaxVisitor visitor)
        {
            if (!visitor.openType(this))
                return;

            name.accept(visitor);

            if (generics != null)
                generics.accept(visitor);

            if (optional != null)
                optional.accept(visitor);

            if (lifeTime != null)
                lifeTime.accept(visitor);

            visitor.closeType(this);
        }
    }

    public class VariantSyntax : TypeSpecSyntax
    {
        public TypeSyntax[] types;
        public VariantSyntax(Position start, Position end, TypeSyntax[] types)
        {
            this.start = start;
            this.end = end;
            this.types = types;
        }

        public override void accept(SyntaxVisitor visitor)
        {
            if (!visitor.openVariant(this))
                return;

            if (types != null)
            {
                foreach (TypeSyntax node in types)
                    node.accept(visitor);
            }

            visitor.closeVariant(this);
        }
    }

    public class ThrowsSyntax : SyntaxNode
    {
        public TypeSpecSyntax throwsType;
        public ThrowsSyntax(Position start, Position end, TypeSpecSyntax throwsType)
        {
            this.start = start;
            this.end = end;
            this.throwsType = throwsType;
        }

        public override void accept(SyntaxVisitor visitor)
        {
            if (!visitor.openThrows(this))
                return;

            throwsType.accept(visitor);

            visitor.closeThrows(this);
        }
    }

    public class GenericArgumentsSyntax : SyntaxNode
    {
        public TypeSyntax generic;
        public GenericArgumentSyntax[] additionalGenerics;
        public GenericArgumentsSyntax(Position start, Position end, TypeSyntax generic, GenericArgumentSyntax[] additionalGenerics)
        {
            this.start = start;
            this.end = end;
            this.generic = generic;
            this.additionalGenerics = additionalGenerics;
        }

        public override void accept(SyntaxVisitor visitor)
        {
            if (!visitor.openGenericArguments(this))
                return;

            generic.accept(visitor);

            if (additionalGenerics != null)
            {
                foreach (GenericArgumentSyntax node in additionalGenerics)
                    node.accept(visitor);
            }

            visitor.closeGenericArguments(this);
        }
    }

    public class GenericArgumentSyntax : SyntaxNode
    {
        public TypeSyntax typeSpec;
        public GenericArgumentSyntax(Position start, Position end, TypeSyntax typeSpec)
        {
            this.start = start;
            this.end = end;
            this.typeSpec = typeSpec;
        }

        public override void accept(SyntaxVisitor visitor)
        {
            if (!visitor.openGenericArgument(this))
                return;

            typeSpec.accept(visitor);

            visitor.closeGenericArgument(this);
        }
    }

    public class OptionalSyntax : SyntaxNode
    {
        public OptionalSyntax(Position start, Position end)
        {
            this.start = start;
            this.end = end;
        }

        public override void accept(SyntaxVisitor visitor)
        {
            visitor.visitOptional(this);
        }
    }

    public class LifeTimeSyntax : SyntaxNode
    {
        public override void accept(SyntaxVisitor visitor)
        {
        }
    }

    public class RootSyntax : LifeTimeSyntax
    {
        public RootSyntax(Position start, Position end)
        {
            this.start = start;
            this.end = end;
        }

        public override void accept(SyntaxVisitor visitor)
        {
            visitor.visitRoot(this);
        }
    }

    public class LocalSyntax : LifeTimeSyntax
    {
        public string location;
        public LocalSyntax(Position start, Position end, string location)
        {
            this.start = start;
            this.end = end;
            this.location = location;
        }

        public override void accept(SyntaxVisitor visitor)
        {
            visitor.visitLocal(this);
        }
    }

    public class ReferenceSyntax : LifeTimeSyntax
    {
        public Literal age;
        public ReferenceSyntax(Position start, Position end, Literal age)
        {
            this.start = start;
            this.end = end;
            this.age = age;
        }

        public override void accept(SyntaxVisitor visitor)
        {
            visitor.visitReference(this);
        }
    }

    public class ThrownSyntax : LifeTimeSyntax
    {
        public ThrownSyntax(Position start, Position end)
        {
            this.start = start;
            this.end = end;
        }

        public override void accept(SyntaxVisitor visitor)
        {
            visitor.visitThrown(this);
        }
    }
}