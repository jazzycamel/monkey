#ifndef MONKEY_AST_H
#define MONKEY_AST_H

#include "Token.h"
#include <memory>
#include <string>
#include <vector>

enum NodeType {
  PROGRAM,
  LET_STATEMENT,
  RETURN_STATEMENT,
  EXPRESSION_STATEMENT,
  IDENTIFIER,
  INTEGER_LITERAL,
  PREFIX_EXPRESSION,
  INFIX_EXPRESSION,
  BOOLEAN_LITERAL,
  BLOCK_STATEMENT,
  IF_EXPRESSION,
  FUNCTION_LITERAL,
  CALL_EXPRESSION
};

class Node {
public:
  virtual std::string tokenLiteral() = 0;
  virtual std::string string() = 0;
  virtual NodeType nodeType() = 0;
};

class Statement : public Node {
public:
  virtual void statementNode() = 0;
};

class Expression : public Node {
public:
  virtual void expressionNode() = 0;
};

class Identifier : public Expression {
public:
  std::string tokenLiteral() override;
  void expressionNode() override;
  std::string string() override;
  NodeType nodeType() override;

  Token token;
  std::string value;
};

class Program : public Node {
public:
  std::string tokenLiteral() override;
  std::vector<std::shared_ptr<Statement>> statements;
  std::string string() override;
  NodeType nodeType() override;
};

class LetStatement : public Statement {
public:
  std::string tokenLiteral() override;
  void statementNode() override;
  std::string string() override;
  NodeType nodeType() override;

  Token token;
  Identifier *name;
  std::shared_ptr<Expression> value;
};

class ReturnStatement : public Statement {
public:
  std::string tokenLiteral() override;
  void statementNode() override;
  std::string string() override;
  NodeType nodeType() override;

  Token token;
  std::shared_ptr<Expression> returnValue;
};

class ExpressionStatement : public Statement {
public:
  std::string tokenLiteral() override;
  void statementNode() override;
  std::string string() override;
  NodeType nodeType() override;

  Token token;
  std::shared_ptr<Expression> expression;
};

class IntegerLiteralExpression : public Expression {
public:
  std::string tokenLiteral() override;
  void expressionNode() override;
  std::string string() override;
  NodeType nodeType() override;

  Token token;
  int64_t value;
};

class PrefixExpression : public Expression {
public:
  std::string tokenLiteral() override;
  void expressionNode() override;
  std::string string() override;
  NodeType nodeType() override;

  Token token;
  std::string operator_;
  std::shared_ptr<Expression> right;
};

class InfixExpression : public Expression {
public:
  std::string tokenLiteral() override;
  void expressionNode() override;
  std::string string() override;
  NodeType nodeType() override;

  Token token;
  std::shared_ptr<Expression> left;
  std::string operator_;
  std::shared_ptr<Expression> right;
};

class BooleanLiteralExpression : public Expression {
public:
  std::string tokenLiteral() override;
  void expressionNode() override;
  std::string string() override;
  NodeType nodeType() override;

  Token token;
  bool value;
};

class BlockStatement : public Statement {
public:
  std::string tokenLiteral() override;
  void statementNode() override;
  std::string string() override;
  NodeType nodeType() override;

  Token token;
  std::vector<std::shared_ptr<Statement>> statements;
};

class IfExpression : public Expression {
public:
  std::string tokenLiteral() override;
  void expressionNode() override;
  std::string string() override;
  NodeType nodeType() override;

  Token token;
  std::shared_ptr<Expression> condition;
  std::shared_ptr<BlockStatement> consequence;
  std::shared_ptr<BlockStatement> alternative;
};

class FunctionLiteralExpression : public Expression {
public:
  std::string tokenLiteral() override;
  void expressionNode() override;
  std::string string() override;
  NodeType nodeType() override;

  Token token;
  std::vector<std::shared_ptr<Identifier>> parameters;
  std::shared_ptr<BlockStatement> body;
};

class CallExpression : public Expression {
public:
  std::string tokenLiteral() override;
  void expressionNode() override;
  std::string string() override;
  NodeType nodeType() override;

  Token token;
  std::shared_ptr<Expression> function;
  std::vector<std::shared_ptr<Expression>> arguments;
};

#endif // MONKEY_AST_H
