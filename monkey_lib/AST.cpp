#include "AST.h"

std::string Identifier::tokenLiteral() { return token.literal; }
void Identifier::expressionNode() {}
NodeType Identifier::nodeType() { return IDENTIFIER; }
std::string Identifier::string() { return value; }

std::string Program::tokenLiteral() {
  if (!statements.empty()) {
    return statements[0]->tokenLiteral();
  }
  return "";
}

NodeType Program::nodeType() { return PROGRAM; }
std::string Program::string() {
  std::string out;
  for (const auto &statement : statements) {
    out += statement->string();
  }
  return out;
}

std::string LetStatement::tokenLiteral() { return token.literal; }
void LetStatement::statementNode() {}
NodeType LetStatement::nodeType() { return LET_STATEMENT; }
std::string LetStatement::string() {
  std::string out = tokenLiteral() + " ";
  out += name->string() + " = ";

  if (value != nullptr)
    out += value->string();

  out += ";";
  return out;
}

std::string ReturnStatement::tokenLiteral() { return token.literal; }
void ReturnStatement::statementNode() {}
NodeType ReturnStatement::nodeType() { return RETURN_STATEMENT; }
std::string ReturnStatement::string() {
  std::string out = tokenLiteral() + " ";
  if (returnValue != nullptr)
    out += returnValue->string();
  out += ";";
  return out;
}

std::string ExpressionStatement::tokenLiteral() { return token.literal; }
void ExpressionStatement::statementNode() {}
NodeType ExpressionStatement::nodeType() { return EXPRESSION_STATEMENT; }
std::string ExpressionStatement::string() {
  if (expression != nullptr)
    return expression->string();
  return "";
}

std::string IntegerLiteralExpression::tokenLiteral() { return token.literal; }
void IntegerLiteralExpression::expressionNode() {}
NodeType IntegerLiteralExpression::nodeType() { return INTEGER_LITERAL; }
std::string IntegerLiteralExpression::string() { return token.literal; }

std::string PrefixExpression::tokenLiteral() { return token.literal; }
void PrefixExpression::expressionNode() {}
NodeType PrefixExpression::nodeType() { return PREFIX_EXPRESSION; }
std::string PrefixExpression::string() {
  std::string out = "(";
  out += operator_;
  out += right->string();
  out += ")";
  return out;
}

std::string InfixExpression::tokenLiteral() { return token.literal; }
void InfixExpression::expressionNode() {}
NodeType InfixExpression::nodeType() { return INFIX_EXPRESSION; }
std::string InfixExpression::string() {
  std::string out = "(";
  out += left->string();
  out += " " + operator_ + " ";
  out += right->string();
  out += ")";
  return out;
}

std::string BooleanLiteralExpression::tokenLiteral() { return token.literal; }
void BooleanLiteralExpression::expressionNode() {}
NodeType BooleanLiteralExpression::nodeType() { return BOOLEAN_LITERAL; }
std::string BooleanLiteralExpression::string() { return token.literal; }

std::string IfExpression::tokenLiteral() { return token.literal; }
void IfExpression::expressionNode() {}
NodeType IfExpression::nodeType() { return IF_EXPRESSION; }
std::string IfExpression::string() {
  std::string out = "if";
  out += condition->string();
  out += " ";
  out += consequence->string();

  if (alternative != nullptr) {
    out += "else ";
    out += alternative->string();
  }

  return out;
}

std::string BlockStatement::tokenLiteral() { return token.literal; }
void BlockStatement::statementNode() {}
NodeType BlockStatement::nodeType() { return BLOCK_STATEMENT; }
std::string BlockStatement::string() {
  std::string out;
  for (const auto &statement : statements) {
    out += statement->string();
  }
  return out;
}

std::string FunctionLiteralExpression::tokenLiteral() { return token.literal; }
void FunctionLiteralExpression::expressionNode() {}
NodeType FunctionLiteralExpression::nodeType() { return FUNCTION_LITERAL; }
std::string FunctionLiteralExpression::string() {
  std::string out = tokenLiteral() + "(";
  for (const auto &param : parameters) {
    out += param->string();
  }
  out += ") ";
  out += body->string();
  return out;
}

std::string CallExpression::tokenLiteral() { return token.literal; }
void CallExpression::expressionNode() {}
NodeType CallExpression::nodeType() { return CALL_EXPRESSION; }
std::string CallExpression::string() {
  std::string out = function->string() + "(";
  for (auto i = 0; i < arguments.size(); i++) {
    out += arguments[i]->string();
    if (i < arguments.size() - 1) {
      out += ", ";
    }
  }
  out += ")";
  return out;
}