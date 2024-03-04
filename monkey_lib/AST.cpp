#include "AST.h"

std::string Identifier::tokenLiteral() { return token.literal; }
void Identifier::expressionNode() {}
std::string Identifier::string() { return value; }

std::string Program::tokenLiteral() {
  if (!statements.empty()) {
    return statements[0]->tokenLiteral();
  }
  return "";
}

std::string Program::string() {
  std::string out;
  for (const auto &statement : statements) {
    out += statement->string();
  }
  return out;
}

std::string LetStatement::tokenLiteral() { return token.literal; }
void LetStatement::statementNode() {}
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
std::string ReturnStatement::string() {
  std::string out = tokenLiteral() + " ";
  if (returnValue != nullptr)
    out += returnValue->string();
  out += ";";
  return out;
}

std::string ExpressionStatement::tokenLiteral() { return token.literal; }
void ExpressionStatement::statementNode() {}
std::string ExpressionStatement::string() {
  if (expression != nullptr)
    return expression->string();
  return "";
}

std::string IntegerLiteralExpression::tokenLiteral() { return token.literal; }
void IntegerLiteralExpression::expressionNode() {}
std::string IntegerLiteralExpression::string() { return token.literal; }

std::string PrefixExpression::tokenLiteral() { return token.literal; }
void PrefixExpression::expressionNode() {}
std::string PrefixExpression::string() {
  std::string out = "(";
  out += operator_;
  out += right->string();
  out += ")";
  return out;
}

std::string InfixExpression::tokenLiteral() { return token.literal; }
void InfixExpression::expressionNode() {}
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
std::string BooleanLiteralExpression::string() { return token.literal; }

std::string IfExpression::tokenLiteral() { return token.literal; }
void IfExpression::expressionNode() {}
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
std::string BlockStatement::string() {
  std::string out;
  for (const auto &statement : statements) {
    out += statement->string();
  }
  return out;
}

std::string FunctionLiteralExpression::tokenLiteral() { return token.literal; }
void FunctionLiteralExpression::expressionNode() {}
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
std::string CallExpression::string() {
  std::string out = function->string() + "(";
  for(auto i=0; i<arguments.size(); i++) {
    out += arguments[i]->string();
    if(i < arguments.size() - 1) {
      out += ", ";
    }
  }
  out += ")";
  return out;
}