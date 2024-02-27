#ifndef MONKEY_AST_H
#define MONKEY_AST_H

#include <memory>
#include <string>
#include <vector>
#include "Token.h"

class Node {
public:
    virtual std::string tokenLiteral() = 0;
    virtual std::string string() = 0;
};

class Statement: public Node {
public:
    virtual void statementNode() = 0;
};

class Expression: public Node {
public:
    virtual void expressionNode() = 0;
};

class Identifier: public Expression {
public:
    std::string tokenLiteral() override;
    void expressionNode() override;
    std::string string() override;

    Token token;
    std::string value;
};

class Program: public Node {
public:
    std::string tokenLiteral() override;
    std::vector<std::shared_ptr<Statement>> statements;
    std::string string() override;
};

class LetStatement: public Statement {
public:
    std::string tokenLiteral() override;
    void statementNode() override;
    std::string string() override;

    Token token;
    Identifier *name;
    std::shared_ptr<Expression> value;
};

class ReturnStatement: public Statement {
public:
    std::string tokenLiteral() override;
    void statementNode() override;
    std::string string() override;

    Token token;
    std::shared_ptr<Expression> returnValue;
};

class ExpressionStatement: public Statement {
public:
    std::string tokenLiteral() override;
    void statementNode() override;
    std::string string() override;

    Token token;
    std::shared_ptr<Expression> expression;
};

class IntegerLiteralExpression: public Expression {
public:
    std::string tokenLiteral() override;
    void expressionNode() override;
    std::string string() override;

    Token token;
    int64_t value;
};

class PrefixExpression: public Expression {
public:
    std::string tokenLiteral() override;
    void expressionNode() override;
    std::string string() override;

    Token token;
    std::string operator_;
    std::shared_ptr<Expression> right;
};

class InfixExpression: public Expression {
public:
    std::string tokenLiteral() override;
    void expressionNode() override;
    std::string string() override;

    Token token;
    std::shared_ptr<Expression> left;
    std::string operator_;
    std::shared_ptr<Expression> right;
};

#endif //MONKEY_AST_H
