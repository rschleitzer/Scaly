using System;
using System.Collections.Generic;
using System.Text;

namespace scalysh
{
    class Parser
    {
        Lexer lexer;
        string fileName;

        string setKeyword = "set";

        string leftCurly = "{";
        string rightCurly = "}";

        public Parser(string theFileName, string text)
        {
            lexer = new Lexer(text);
            fileName = theFileName;
        }

        public File parseFile()
        {
            Position start = lexer.getPreviousPosition();

            Expression[] expressions = parseExpressionList();
            if (expressions != null)
            {
                if (!isAtEnd())
                {
                    Position errorPos = lexer.getPreviousPosition();
                    throw new ParserException(errorPos.line, errorPos.column);
                }
            }

            Position end = lexer.getPosition();

            File ret = new File(start, end, expressions);

            if (expressions != null)
            {
                foreach (Expression item in expressions)
                    item.parent = ret;
            }

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

            return ret.ToArray();
        }

        public Expression parseExpression()
        {
            {
                Block node = parseBlock();
                if (node != null)
                    return node;
            }

            {
                Assignment node = parseAssignment();
                if (node != null)
                    return node;
            }

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

            Expression[] statements = parseExpressionList();

            bool successRightCurly3 = lexer.parsePunctuation(rightCurly);
            if (successRightCurly3)
                lexer.advance();
            else
                return null;

            Position end = lexer.getPosition();

            Block ret = new Block(start, end, statements);

            if (statements != null)
            {
                foreach (Expression item in statements)
                    item.parent = ret;
            }

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

            Expression[] statements = parseExpressionList();

            Position end = lexer.getPosition();

            Assignment ret = new Assignment(start, end, statements);

            if (statements != null)
            {
                foreach (Expression item in statements)
                    item.parent = ret;
            }

            return ret;
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

        bool isAtEnd()
        {
            return lexer.isAtEnd();
        }

        bool isIdentifier(string id)
        {
            if (id == setKeyword)
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

        public virtual bool openAssignment(Assignment assignment)
        {
            return true;
        }

        public virtual void closeAssignment(Assignment assignment)
        {
        }

        public virtual void visitIdentifierExpression(IdentifierExpression identifierExpression)
        {
        }

        public virtual void visitLiteralExpression(LiteralExpression literalExpression)
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
        public Expression[] expressions;
        public File(Position start, Position end, Expression[] expressions)
        {
            this.start = start;
            this.end = end;
            this.expressions = expressions;
        }

        public override void accept(Visitor visitor)
        {
            if (!visitor.openFile(this))
                return;

            if (expressions != null)
            {
                foreach (Expression node in expressions)
                    node.accept(visitor);
            }
            visitor.closeFile(this);
        }
    }

    public class Expression : SyntaxNode
    {
        public override void accept(Visitor visitor)
        {
        }
    }

    public class Block : Expression
    {
        public Expression[] statements;
        public Block(Position start, Position end, Expression[] statements)
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
                foreach (Expression node in statements)
                    node.accept(visitor);
            }
            visitor.closeBlock(this);
        }
    }

    public class Assignment : Expression
    {
        public Expression[] statements;
        public Assignment(Position start, Position end, Expression[] statements)
        {
            this.start = start;
            this.end = end;
            this.statements = statements;
        }

        public override void accept(Visitor visitor)
        {
            if (!visitor.openAssignment(this))
                return;

            if (statements != null)
            {
                foreach (Expression node in statements)
                    node.accept(visitor);
            }
            visitor.closeAssignment(this);
        }
    }

    public class IdentifierExpression : Expression
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

    public class LiteralExpression : Expression
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
}
