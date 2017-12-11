using System;
using System.Collections.Generic;
using System.Text;

namespace scalysh
{
    class Parser
    {
        Lexer lexer;
        string fileName;

        string letKeyword = "let";
        string mutableKeyword = "mutable";
        string varKeyword = "var";
        string setKeyword = "set";
        string classKeyword = "class";
        string extendsKeyword = "extends";
        string functionKeyword = "function";
        string methodKeyword = "method";
        string thisKeyword = "this";

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
        string at = "@";
        string hash = "#";
        string dollar = "$";
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
                    throw new ParserException(errorPos.line, errorPos.column);
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
                Declaration node = parseDeclaration();
                if (node != null)
                    return node;
            }

            {
                Expression node = parseExpression();
                if (node != null)
                    return node;
            }

            return null;
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
                ConstantDeclaration node = parseConstantDeclaration();
                if (node != null)
                    return node;
            }

            {
                VariableDeclaration node = parseVariableDeclaration();
                if (node != null)
                    return node;
            }

            {
                InferredDeclaration node = parseInferredDeclaration();
                if (node != null)
                    return node;
            }

            {
                Assignment node = parseAssignment();
                if (node != null)
                    return node;
            }

            {
                ClassDeclaration node = parseClassDeclaration();
                if (node != null)
                    return node;
            }

            {
                MethodDeclaration node = parseMethodDeclaration();
                if (node != null)
                    return node;
            }

            {
                FunctionDeclaration node = parseFunctionDeclaration();
                if (node != null)
                    return node;
            }

            return null;
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
            {
                Block node = parseBlock();
                if (node != null)
                    return node;
            }

            {
                PostfixExpression node = parsePostfixExpression();
                if (node != null)
                    return node;
            }

            return null;
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

        public ConstantDeclaration parseConstantDeclaration()
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

            ConstantDeclaration ret = new ConstantDeclaration(start, end, binding);

            binding.parent = ret;

            return ret;
        }

        public VariableDeclaration parseVariableDeclaration()
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

            VariableDeclaration ret = new VariableDeclaration(start, end, binding);

            binding.parent = ret;

            return ret;
        }

        public InferredDeclaration parseInferredDeclaration()
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

            InferredDeclaration ret = new InferredDeclaration(start, end, binding);

            binding.parent = ret;

            return ret;
        }

        public Binding parseBinding()
        {
            Position start = lexer.getPreviousPosition();

            string name = lexer.parseIdentifier();
            if ((name != null) && isIdentifier(name))
                lexer.advance();
            else
                return null;

            TypeAnnotation typeAnnotation = parseTypeAnnotation();

            Expression expression = parseExpression();
            if (expression == null)
                return null;

            Position end = lexer.getPosition();

            Binding ret = new Binding(start, end, name, typeAnnotation, expression);

            if (typeAnnotation != null)
                typeAnnotation.parent = ret;
            expression.parent = ret;

            return ret;
        }

        public Assignment parseAssignment()
        {
            Position start = lexer.getPreviousPosition();

            bool successSet1 = lexer.parseKeyword(setKeyword);
            if (successSet1)
                lexer.advance();
            else
                return null;

            Expression lvalue = parseExpression();
            if (lvalue == null)
                return null;

            bool successColon3 = lexer.parsePunctuation(colon);
            if (successColon3)
                lexer.advance();
            else
                return null;

            Expression rvalue = parseExpression();
            if (rvalue == null)
                return null;

            Position end = lexer.getPosition();

            Assignment ret = new Assignment(start, end, lvalue, rvalue);

            lvalue.parent = ret;
            rvalue.parent = ret;

            return ret;
        }

        public PostfixExpression parsePostfixExpression()
        {
            Position start = lexer.getPreviousPosition();

            PrimaryExpression primary = parsePrimaryExpression();
            if (primary == null)
                return null;

            Postfix[] postfixes = parsePostfixList();

            Position end = lexer.getPosition();

            PostfixExpression ret = new PostfixExpression(start, end, primary, postfixes);

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
                IdentifierExpression node = parseIdentifierExpression();
                if (node != null)
                    return node;
            }

            {
                LiteralExpression node = parseLiteralExpression();
                if (node != null)
                    return node;
            }

            {
                ThisExpression node = parseThisExpression();
                if (node != null)
                    return node;
            }

            {
                ObjectExpression node = parseObjectExpression();
                if (node != null)
                    return node;
            }

            return null;
        }

        public IdentifierExpression parseIdentifierExpression()
        {
            Position start = lexer.getPreviousPosition();

            string name = lexer.parseIdentifier();
            if ((name != null) && isIdentifier(name))
                lexer.advance();
            else
                return null;

            Position end = lexer.getPosition();

            IdentifierExpression ret = new IdentifierExpression(start, end, name);


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

        public ThisExpression parseThisExpression()
        {
            Position start = lexer.getPreviousPosition();

            bool successThis1 = lexer.parseKeyword(thisKeyword);
            if (successThis1)
                lexer.advance();
            else
                return null;

            Position end = lexer.getPosition();

            ThisExpression ret = new ThisExpression(start, end);


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
                Operation node = parseOperation();
                if (node != null)
                    return node;
            }

            return null;
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

            ExpressionElement[] expressions = parseExpressionElementList();

            bool successRightBracket3 = lexer.parsePunctuation(rightBracket);
            if (successRightBracket3)
                lexer.advance();
            else
                return null;

            Position end = lexer.getPosition();

            Subscript ret = new Subscript(start, end, expressions);

            if (expressions != null)
            {
                foreach (ExpressionElement item in expressions)
                    item.parent = ret;
            }

            return ret;
        }

        public Operation parseOperation()
        {
            Position start = lexer.getPreviousPosition();

            Expression Expression = parseExpression();
            if (Expression == null)
                return null;

            Position end = lexer.getPosition();

            Operation ret = new Operation(start, end, Expression);

            Expression.parent = ret;

            return ret;
        }

        public ExpressionElement[] parseExpressionElementList()
        {
            List<ExpressionElement> ret = null;
            while (true)
            {
                ExpressionElement node = parseExpressionElement();
                if (node == null)
                    break;

                if (ret == null)
                    ret = new List<ExpressionElement>();

                ret.Add(node);
            }

            if (ret != null)
                return ret.ToArray();
            else
                return null;
        }

        public ExpressionElement parseExpressionElement()
        {
            Position start = lexer.getPreviousPosition();

            Expression expression = parseExpression();
            if (expression == null)
                return null;

            bool successComma2 = lexer.parsePunctuation(comma);
            if (successComma2)
                lexer.advance();

            Position end = lexer.getPosition();

            ExpressionElement ret = new ExpressionElement(start, end, expression);

            expression.parent = ret;

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

            ObjectItem[] items = parseObjectItemList();

            bool successRightParen3 = lexer.parsePunctuation(rightParen);
            if (successRightParen3)
                lexer.advance();
            else
                return null;

            Position end = lexer.getPosition();

            ObjectExpression ret = new ObjectExpression(start, end, items);

            if (items != null)
            {
                foreach (ObjectItem item in items)
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

            Expression expression = parseExpression();
            if (expression == null)
                return null;

            bool successComma2 = lexer.parsePunctuation(comma);
            if (successComma2)
                lexer.advance();

            Position end = lexer.getPosition();

            ObjectItem ret = new ObjectItem(start, end, expression);

            expression.parent = ret;

            return ret;
        }

        public ClassDeclaration parseClassDeclaration()
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

            TypeInheritanceClause typeInheritanceClause = parseTypeInheritanceClause();

            ObjectDeclaration contents = parseObjectDeclaration();

            Expression body = parseExpression();

            Position end = lexer.getPosition();

            ClassDeclaration ret = new ClassDeclaration(start, end, name, typeInheritanceClause, contents, body);

            name.parent = ret;
            if (typeInheritanceClause != null)
                typeInheritanceClause.parent = ret;
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

        public TypeInheritanceClause parseTypeInheritanceClause()
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

            TypeInheritanceClause ret = new TypeInheritanceClause(start, end, name);

            name.parent = ret;

            return ret;
        }

        public ObjectDeclaration parseObjectDeclaration()
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

            ObjectDeclaration ret = new ObjectDeclaration(start, end, components);

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

        public FunctionDeclaration parseFunctionDeclaration()
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

            FunctionDeclaration ret = new FunctionDeclaration(start, end, procedure);

            procedure.parent = ret;

            return ret;
        }

        public MethodDeclaration parseMethodDeclaration()
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

            MethodDeclaration ret = new MethodDeclaration(start, end, procedure);

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

            ObjectDeclaration input = parseObjectDeclaration();

            TypeAnnotation output = parseTypeAnnotation();

            Statement body = parseStatement();
            if (body == null)
                return null;

            Position end = lexer.getPosition();

            Procedure ret = new Procedure(start, end, name, input, output, body);

            if (input != null)
                input.parent = ret;
            if (output != null)
                output.parent = ret;
            body.parent = ret;

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

            Type name = parseType();
            if (name == null)
                return null;

            Position end = lexer.getPosition();

            TypeAnnotation ret = new TypeAnnotation(start, end, name);

            name.parent = ret;

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

            TypePostfix[] postfixes = parseTypePostfixList();

            LifeTime lifeTime = parseLifeTime();

            Position end = lexer.getPosition();

            Type ret = new Type(start, end, name, postfixes, lifeTime);

            if (postfixes != null)
            {
                foreach (TypePostfix item in postfixes)
                    item.parent = ret;
            }
            if (lifeTime != null)
                lifeTime.parent = ret;

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

            Type key = parseType();

            bool successRightBracket3 = lexer.parsePunctuation(rightBracket);
            if (successRightBracket3)
                lexer.advance();
            else
                return null;

            Position end = lexer.getPosition();

            IndexedType ret = new IndexedType(start, end, key);

            if (key != null)
                key.parent = ret;

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

            bool successBacktick1 = lexer.parsePunctuation(backtick);
            if (successBacktick1)
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

            bool successHash1 = lexer.parsePunctuation(hash);
            if (successHash1)
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
            if (id == letKeyword)
                return false;

            if (id == mutableKeyword)
                return false;

            if (id == varKeyword)
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

            return true;
        }
    }

    public class Visitor
    {
        public virtual bool openProgram(Program program)
        {
            return true;
        }

        public virtual void closeProgram(Program program)
        {
        }

        public virtual bool openFile(File file)
        {
            return true;
        }

        public virtual void closeFile(File file)
        {
        }

        public virtual bool openBlock(Block block)
        {
            return true;
        }

        public virtual void closeBlock(Block block)
        {
        }

        public virtual bool openConstantDeclaration(ConstantDeclaration constantDeclaration)
        {
            return true;
        }

        public virtual void closeConstantDeclaration(ConstantDeclaration constantDeclaration)
        {
        }

        public virtual bool openVariableDeclaration(VariableDeclaration variableDeclaration)
        {
            return true;
        }

        public virtual void closeVariableDeclaration(VariableDeclaration variableDeclaration)
        {
        }

        public virtual bool openInferredDeclaration(InferredDeclaration inferredDeclaration)
        {
            return true;
        }

        public virtual void closeInferredDeclaration(InferredDeclaration inferredDeclaration)
        {
        }

        public virtual bool openBinding(Binding binding)
        {
            return true;
        }

        public virtual void closeBinding(Binding binding)
        {
        }

        public virtual bool openAssignment(Assignment assignment)
        {
            return true;
        }

        public virtual void closeAssignment(Assignment assignment)
        {
        }

        public virtual bool openPostfixExpression(PostfixExpression postfixExpression)
        {
            return true;
        }

        public virtual void closePostfixExpression(PostfixExpression postfixExpression)
        {
        }

        public virtual void visitIdentifierExpression(IdentifierExpression identifierExpression)
        {
        }

        public virtual void visitLiteralExpression(LiteralExpression literalExpression)
        {
        }

        public virtual void visitThisExpression(ThisExpression thisExpression)
        {
        }

        public virtual void visitMemberAccess(MemberAccess memberAccess)
        {
        }

        public virtual bool openSubscript(Subscript subscript)
        {
            return true;
        }

        public virtual void closeSubscript(Subscript subscript)
        {
        }

        public virtual bool openOperation(Operation operation)
        {
            return true;
        }

        public virtual void closeOperation(Operation operation)
        {
        }

        public virtual bool openExpressionElement(ExpressionElement expressionElement)
        {
            return true;
        }

        public virtual void closeExpressionElement(ExpressionElement expressionElement)
        {
        }

        public virtual bool openObjectExpression(ObjectExpression objectExpression)
        {
            return true;
        }

        public virtual void closeObjectExpression(ObjectExpression objectExpression)
        {
        }

        public virtual bool openObjectItem(ObjectItem objectItem)
        {
            return true;
        }

        public virtual void closeObjectItem(ObjectItem objectItem)
        {
        }

        public virtual bool openClassDeclaration(ClassDeclaration classDeclaration)
        {
            return true;
        }

        public virtual void closeClassDeclaration(ClassDeclaration classDeclaration)
        {
        }

        public virtual bool openName(Name name)
        {
            return true;
        }

        public virtual void closeName(Name name)
        {
        }

        public virtual void visitExtension(Extension extension)
        {
        }

        public virtual bool openTypeInheritanceClause(TypeInheritanceClause typeInheritanceClause)
        {
            return true;
        }

        public virtual void closeTypeInheritanceClause(TypeInheritanceClause typeInheritanceClause)
        {
        }

        public virtual bool openObjectDeclaration(ObjectDeclaration objectDeclaration)
        {
            return true;
        }

        public virtual void closeObjectDeclaration(ObjectDeclaration objectDeclaration)
        {
        }

        public virtual bool openComponent(Component component)
        {
            return true;
        }

        public virtual void closeComponent(Component component)
        {
        }

        public virtual bool openFunctionDeclaration(FunctionDeclaration functionDeclaration)
        {
            return true;
        }

        public virtual void closeFunctionDeclaration(FunctionDeclaration functionDeclaration)
        {
        }

        public virtual bool openMethodDeclaration(MethodDeclaration methodDeclaration)
        {
            return true;
        }

        public virtual void closeMethodDeclaration(MethodDeclaration methodDeclaration)
        {
        }

        public virtual bool openProcedure(Procedure procedure)
        {
            return true;
        }

        public virtual void closeProcedure(Procedure procedure)
        {
        }

        public virtual bool openTypeAnnotation(TypeAnnotation typeAnnotation)
        {
            return true;
        }

        public virtual void closeTypeAnnotation(TypeAnnotation typeAnnotation)
        {
        }

        public virtual bool openType(Type type)
        {
            return true;
        }

        public virtual void closeType(Type type)
        {
        }

        public virtual void visitOptional(Optional optional)
        {
        }

        public virtual bool openIndexedType(IndexedType indexedType)
        {
            return true;
        }

        public virtual void closeIndexedType(IndexedType indexedType)
        {
        }

        public virtual void visitRoot(Root root)
        {
        }

        public virtual void visitLocal(Local local)
        {
        }

        public virtual void visitReference(Reference reference)
        {
        }

        public virtual void visitThrown(Thrown thrown)
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

    public class Statement : SyntaxNode
    {
        public override void accept(Visitor visitor)
        {
        }
    }

    public class Declaration : Statement
    {
        public override void accept(Visitor visitor)
        {
        }
    }

    public class Expression : Statement
    {
        public override void accept(Visitor visitor)
        {
        }
    }

    public class Block : Expression
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

    public class ConstantDeclaration : Declaration
    {
        public Binding binding;
        public ConstantDeclaration(Position start, Position end, Binding binding)
        {
            this.start = start;
            this.end = end;
            this.binding = binding;
        }

        public override void accept(Visitor visitor)
        {
            if (!visitor.openConstantDeclaration(this))
                return;

        binding.accept(visitor);
            visitor.closeConstantDeclaration(this);
        }
    }

    public class VariableDeclaration : Declaration
    {
        public Binding binding;
        public VariableDeclaration(Position start, Position end, Binding binding)
        {
            this.start = start;
            this.end = end;
            this.binding = binding;
        }

        public override void accept(Visitor visitor)
        {
            if (!visitor.openVariableDeclaration(this))
                return;

        binding.accept(visitor);
            visitor.closeVariableDeclaration(this);
        }
    }

    public class InferredDeclaration : Declaration
    {
        public Binding binding;
        public InferredDeclaration(Position start, Position end, Binding binding)
        {
            this.start = start;
            this.end = end;
            this.binding = binding;
        }

        public override void accept(Visitor visitor)
        {
            if (!visitor.openInferredDeclaration(this))
                return;

        binding.accept(visitor);
            visitor.closeInferredDeclaration(this);
        }
    }

    public class Binding : SyntaxNode
    {
        public string name;
        public TypeAnnotation typeAnnotation;
        public Expression expression;
        public Binding(Position start, Position end, string name, TypeAnnotation typeAnnotation, Expression expression)
        {
            this.start = start;
            this.end = end;
            this.name = name;
            this.typeAnnotation = typeAnnotation;
            this.expression = expression;
        }

        public override void accept(Visitor visitor)
        {
            if (!visitor.openBinding(this))
                return;

        if (typeAnnotation != null)
            typeAnnotation.accept(visitor);
        expression.accept(visitor);
            visitor.closeBinding(this);
        }
    }

    public class Assignment : Declaration
    {
        public Expression lvalue;
        public Expression rvalue;
        public Assignment(Position start, Position end, Expression lvalue, Expression rvalue)
        {
            this.start = start;
            this.end = end;
            this.lvalue = lvalue;
            this.rvalue = rvalue;
        }

        public override void accept(Visitor visitor)
        {
            if (!visitor.openAssignment(this))
                return;

        lvalue.accept(visitor);
        rvalue.accept(visitor);
            visitor.closeAssignment(this);
        }
    }

    public class PostfixExpression : Expression
    {
        public PrimaryExpression primary;
        public Postfix[] postfixes;
        public PostfixExpression(Position start, Position end, PrimaryExpression primary, Postfix[] postfixes)
        {
            this.start = start;
            this.end = end;
            this.primary = primary;
            this.postfixes = postfixes;
        }

        public override void accept(Visitor visitor)
        {
            if (!visitor.openPostfixExpression(this))
                return;

        primary.accept(visitor);
            if (postfixes != null)
            {
                foreach (Postfix node in postfixes)
                    node.accept(visitor);
            }
            visitor.closePostfixExpression(this);
        }
    }

    public class PrimaryExpression : SyntaxNode
    {
        public override void accept(Visitor visitor)
        {
        }
    }

    public class IdentifierExpression : PrimaryExpression
    {
        public string name;
        public IdentifierExpression(Position start, Position end, string name)
        {
            this.start = start;
            this.end = end;
            this.name = name;
        }

        public override void accept(Visitor visitor)
        {
            visitor.visitIdentifierExpression(this);
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

    public class ThisExpression : PrimaryExpression
    {
        public ThisExpression(Position start, Position end)
        {
            this.start = start;
            this.end = end;
        }

        public override void accept(Visitor visitor)
        {
            visitor.visitThisExpression(this);
        }
    }

    public class Postfix : SyntaxNode
    {
        public override void accept(Visitor visitor)
        {
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
        public ExpressionElement[] expressions;
        public Subscript(Position start, Position end, ExpressionElement[] expressions)
        {
            this.start = start;
            this.end = end;
            this.expressions = expressions;
        }

        public override void accept(Visitor visitor)
        {
            if (!visitor.openSubscript(this))
                return;

            if (expressions != null)
            {
                foreach (ExpressionElement node in expressions)
                    node.accept(visitor);
            }
            visitor.closeSubscript(this);
        }
    }

    public class Operation : Postfix
    {
        public Expression Expression;
        public Operation(Position start, Position end, Expression Expression)
        {
            this.start = start;
            this.end = end;
            this.Expression = Expression;
        }

        public override void accept(Visitor visitor)
        {
            if (!visitor.openOperation(this))
                return;

        Expression.accept(visitor);
            visitor.closeOperation(this);
        }
    }

    public class ExpressionElement : SyntaxNode
    {
        public Expression expression;
        public ExpressionElement(Position start, Position end, Expression expression)
        {
            this.start = start;
            this.end = end;
            this.expression = expression;
        }

        public override void accept(Visitor visitor)
        {
            if (!visitor.openExpressionElement(this))
                return;

        expression.accept(visitor);
            visitor.closeExpressionElement(this);
        }
    }

    public class ObjectExpression : PrimaryExpression
    {
        public ObjectItem[] items;
        public ObjectExpression(Position start, Position end, ObjectItem[] items)
        {
            this.start = start;
            this.end = end;
            this.items = items;
        }

        public override void accept(Visitor visitor)
        {
            if (!visitor.openObjectExpression(this))
                return;

            if (items != null)
            {
                foreach (ObjectItem node in items)
                    node.accept(visitor);
            }
            visitor.closeObjectExpression(this);
        }
    }

    public class ObjectItem : SyntaxNode
    {
        public Expression expression;
        public ObjectItem(Position start, Position end, Expression expression)
        {
            this.start = start;
            this.end = end;
            this.expression = expression;
        }

        public override void accept(Visitor visitor)
        {
            if (!visitor.openObjectItem(this))
                return;

        expression.accept(visitor);
            visitor.closeObjectItem(this);
        }
    }

    public class ClassDeclaration : Declaration
    {
        public Name name;
        public TypeInheritanceClause typeInheritanceClause;
        public ObjectDeclaration contents;
        public Expression body;
        public ClassDeclaration(Position start, Position end, Name name, TypeInheritanceClause typeInheritanceClause, ObjectDeclaration contents, Expression body)
        {
            this.start = start;
            this.end = end;
            this.name = name;
            this.typeInheritanceClause = typeInheritanceClause;
            this.contents = contents;
            this.body = body;
        }

        public override void accept(Visitor visitor)
        {
            if (!visitor.openClassDeclaration(this))
                return;

        name.accept(visitor);
        if (typeInheritanceClause != null)
            typeInheritanceClause.accept(visitor);
        if (contents != null)
            contents.accept(visitor);
        if (body != null)
            body.accept(visitor);
            visitor.closeClassDeclaration(this);
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

    public class TypeInheritanceClause : SyntaxNode
    {
        public Name name;
        public TypeInheritanceClause(Position start, Position end, Name name)
        {
            this.start = start;
            this.end = end;
            this.name = name;
        }

        public override void accept(Visitor visitor)
        {
            if (!visitor.openTypeInheritanceClause(this))
                return;

        name.accept(visitor);
            visitor.closeTypeInheritanceClause(this);
        }
    }

    public class ObjectDeclaration : SyntaxNode
    {
        public Component[] components;
        public ObjectDeclaration(Position start, Position end, Component[] components)
        {
            this.start = start;
            this.end = end;
            this.components = components;
        }

        public override void accept(Visitor visitor)
        {
            if (!visitor.openObjectDeclaration(this))
                return;

            if (components != null)
            {
                foreach (Component node in components)
                    node.accept(visitor);
            }
            visitor.closeObjectDeclaration(this);
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

    public class FunctionDeclaration : Declaration
    {
        public Procedure procedure;
        public FunctionDeclaration(Position start, Position end, Procedure procedure)
        {
            this.start = start;
            this.end = end;
            this.procedure = procedure;
        }

        public override void accept(Visitor visitor)
        {
            if (!visitor.openFunctionDeclaration(this))
                return;

        procedure.accept(visitor);
            visitor.closeFunctionDeclaration(this);
        }
    }

    public class MethodDeclaration : Declaration
    {
        public Procedure procedure;
        public MethodDeclaration(Position start, Position end, Procedure procedure)
        {
            this.start = start;
            this.end = end;
            this.procedure = procedure;
        }

        public override void accept(Visitor visitor)
        {
            if (!visitor.openMethodDeclaration(this))
                return;

        procedure.accept(visitor);
            visitor.closeMethodDeclaration(this);
        }
    }

    public class Procedure : SyntaxNode
    {
        public string name;
        public ObjectDeclaration input;
        public TypeAnnotation output;
        public Statement body;
        public Procedure(Position start, Position end, string name, ObjectDeclaration input, TypeAnnotation output, Statement body)
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
        body.accept(visitor);
            visitor.closeProcedure(this);
        }
    }

    public class TypeAnnotation : SyntaxNode
    {
        public Type name;
        public TypeAnnotation(Position start, Position end, Type name)
        {
            this.start = start;
            this.end = end;
            this.name = name;
        }

        public override void accept(Visitor visitor)
        {
            if (!visitor.openTypeAnnotation(this))
                return;

        name.accept(visitor);
            visitor.closeTypeAnnotation(this);
        }
    }

    public class Type : SyntaxNode
    {
        public string name;
        public TypePostfix[] postfixes;
        public LifeTime lifeTime;
        public Type(Position start, Position end, string name, TypePostfix[] postfixes, LifeTime lifeTime)
        {
            this.start = start;
            this.end = end;
            this.name = name;
            this.postfixes = postfixes;
            this.lifeTime = lifeTime;
        }

        public override void accept(Visitor visitor)
        {
            if (!visitor.openType(this))
                return;

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
        public Type key;
        public IndexedType(Position start, Position end, Type key)
        {
            this.start = start;
            this.end = end;
            this.key = key;
        }

        public override void accept(Visitor visitor)
        {
            if (!visitor.openIndexedType(this))
                return;

        if (key != null)
            key.accept(visitor);
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
