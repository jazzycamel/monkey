#include "AST.h"

std::string Identifier::tokenLiteral() { return token.literal; }
void Identifier::expressionNode() {}
std::string Identifier::string() {
    return value;
}

std::string Program::tokenLiteral() {
    if(!statements.empty()) {
        return statements[0]->tokenLiteral();
    }
    return "";
}

std::string Program::string() {
    std::string out;
    for(const auto& statement: statements){
        out += statement->string();
    }
    return out;
}

std::string LetStatement::tokenLiteral() { return token.literal; }
void LetStatement::statementNode() {}
std::string LetStatement::string() {
    std::string out = tokenLiteral() + " ";
    out += name->string() + " = ";

    if(value != nullptr) out += value->string();

    out += ";";
    return out;
}

std::string ReturnStatement::tokenLiteral() { return token.literal; }
void ReturnStatement::statementNode() {}
std::string ReturnStatement::string() {
    std::string out = tokenLiteral() + " ";
    if (returnValue != nullptr) out += returnValue->string();
    out += ";";
    return out;
}

std::string ExpressionStatement::tokenLiteral() { return token.literal; }
void ExpressionStatement::statementNode() {}
std::string ExpressionStatement::string() {
    if(expression != nullptr) return expression->string();
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