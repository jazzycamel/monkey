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
  STRING_LITERAL,
  PREFIX_EXPRESSION,
  INFIX_EXPRESSION,
  BOOLEAN_LITERAL,
  BLOCK_STATEMENT,
  IF_EXPRESSION,
  FUNCTION_LITERAL,
  CALL_EXPRESSION,
  ARRAY_LITERAL,
  INDEX_EXPRESSION,
};

class Node {
public:
  virtual std::string tokenLiteral() = 0;
  virtual std::string string() = 0;
  virtual NodeType nodeType() = 0;
};
typedef std::shared_ptr<Node> NodePtr;

class Statement : public Node {
public:
  virtual void statementNode() = 0;
};
typedef std::shared_ptr<Statement> StatementPtr;
typedef std::vector<StatementPtr> StatementPtrVec;

class Expression : public Node {
public:
  virtual void expressionNode() = 0;
};
typedef std::shared_ptr<Expression> ExpressionPtr;
typedef std::vector<ExpressionPtr> ExpressionPtrVec;

class Identifier : public Expression {
public:
  std::string tokenLiteral() override;
  void expressionNode() override;
  std::string string() override;
  NodeType nodeType() override;

  Token token;
  std::string value;
};
typedef std::shared_ptr<Identifier> IdentifierPtr;
typedef std::vector<IdentifierPtr> IdentifierPtrVec;

class Program : public Node {
public:
  std::string tokenLiteral() override;
  StatementPtrVec statements;
  std::string string() override;
  NodeType nodeType() override;
};
typedef std::shared_ptr<Program> ProgramPtr;

class LetStatement : public Statement {
public:
  std::string tokenLiteral() override;
  void statementNode() override;
  std::string string() override;
  NodeType nodeType() override;

  Token token;
  Identifier *name;
  ExpressionPtr value;
};
typedef std::shared_ptr<LetStatement> LetStatementPtr;

class ReturnStatement : public Statement {
public:
  std::string tokenLiteral() override;
  void statementNode() override;
  std::string string() override;
  NodeType nodeType() override;

  Token token;
  ExpressionPtr returnValue;
};
typedef std::shared_ptr<ReturnStatement> ReturnStatementPtr;

class ExpressionStatement : public Statement {
public:
  std::string tokenLiteral() override;
  void statementNode() override;
  std::string string() override;
  NodeType nodeType() override;

  Token token;
  ExpressionPtr expression;
};
typedef std::shared_ptr<ExpressionStatement> ExpressionStatementPtr;

class IntegerLiteralExpression : public Expression {
public:
  std::string tokenLiteral() override;
  void expressionNode() override;
  std::string string() override;
  NodeType nodeType() override;

  Token token;
  int64_t value;
};

class StringLiteralExpression : public Expression {
public:
  std::string tokenLiteral() override;
  void expressionNode() override;
  std::string string() override;
  NodeType nodeType() override;

  Token token;
  std::string value;
};

class ArrayLiteralExpression : public Expression {
public:  
  std::string tokenLiteral() override;
  void expressionNode() override;
  std::string string() override;
  NodeType nodeType() override;

  Token token;
  ExpressionPtrVec elements;
};


class PrefixExpression : public Expression {
public:
  std::string tokenLiteral() override;
  void expressionNode() override;
  std::string string() override;
  NodeType nodeType() override;

  Token token;
  std::string operator_;
  ExpressionPtr right;
};

class InfixExpression : public Expression {
public:
  std::string tokenLiteral() override;
  void expressionNode() override;
  std::string string() override;
  NodeType nodeType() override;

  Token token;
  ExpressionPtr left;
  std::string operator_;
  ExpressionPtr right;
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
  StatementPtrVec statements;
};
typedef std::shared_ptr<BlockStatement> BlockStatementPtr;

class IfExpression : public Expression {
public:
  std::string tokenLiteral() override;
  void expressionNode() override;
  std::string string() override;
  NodeType nodeType() override;

  Token token;
  ExpressionPtr condition;
  BlockStatementPtr consequence;
  BlockStatementPtr alternative;
};
typedef std::shared_ptr<IfExpression> IfExpressionPtr;

class FunctionLiteralExpression : public Expression {
public:
  std::string tokenLiteral() override;
  void expressionNode() override;
  std::string string() override;
  NodeType nodeType() override;

  Token token;
  IdentifierPtrVec parameters;
  BlockStatementPtr body;
};
typedef std::shared_ptr<FunctionLiteralExpression> FunctionLiteralExpressionPtr;

class CallExpression : public Expression {
public:
  std::string tokenLiteral() override;
  void expressionNode() override;
  std::string string() override;
  NodeType nodeType() override;

  Token token;
  ExpressionPtr function;
  ExpressionPtrVec arguments;
};
typedef std::shared_ptr<CallExpression> CallExpressionPtr;

class IndexExpression : public Expression {
public:
  std::string tokenLiteral() override;
  void expressionNode() override;
  std::string string() override;
  NodeType nodeType() override;

  Token token;
  ExpressionPtr left;
  ExpressionPtr index;
};

#endif // MONKEY_AST_H
