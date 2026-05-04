#ifndef MONKEY_PARSER_H
#define MONKEY_PARSER_H

#include <map>
#include <string>
#include <vector>

#include "AST.h"
#include "Lexer.h"
#include "Token.h"

class Parser;

typedef ExpressionPtr (Parser::*prefixParseFn)();

typedef ExpressionPtr (Parser::*infixParseFn)(ExpressionPtr);

enum { LOWEST = 1, EQUALS, LESS_GREATER, SUM, PRODUCT, PREFIX, CALL, INDEX };

const std::map<TokenType, int> precedences = {
    {EQ, EQUALS},       {NOT_EQ, EQUALS},    {LT, LESS_GREATER},
    {GT, LESS_GREATER}, {PLUS, SUM},         {MINUS, SUM},
    {SLASH, PRODUCT},   {ASTERISK, PRODUCT}, {LPAREN, CALL}, {LBRACKET, INDEX} };

class Parser {
public:
  explicit Parser(Lexer *lexer);

  ProgramPtr parseProgram();

  std::vector<std::string> errors();

private:
  void _nextToken();

  StatementPtr _parseStatement();

  LetStatementPtr _parseLetStatement();

  ReturnStatementPtr _parseReturnStatement();

  ExpressionStatementPtr _parseExpressionStatement();

  ExpressionPtr _parseExpression(int precedence);

  ExpressionPtrVec _parseExpressionList(TokenType end);

  ExpressionPtr _parseIdentifier();

  ExpressionPtr _parseIntegerLiteralExpression();

  ExpressionPtr _parseStringLiteralExpression();

  ExpressionPtr _parsePrefixExpression();

  ExpressionPtr _parseInfixExpression(ExpressionPtr left);

  ExpressionPtr _parseBooleanLiteralExpression();

  ExpressionPtr _parseGroupedExpression();

  ExpressionPtr _parseIfExpression();

  BlockStatementPtr _parseBlockStatement();

  ExpressionPtr _parseFunctionLiteralExpression();

  IdentifierPtrVec _parseFunctionParameters();

  ExpressionPtr _parseCallExpression(ExpressionPtr function);

  ExpressionPtrVec _parseCallArguments();

  ExpressionPtr _parseArrayLiteral();

  ExpressionPtr _parseIndexExpression(ExpressionPtr left);

  void _noPrefixParseFnError(const TokenType &t);

  void _registerPrefix(const TokenType &tokenType, prefixParseFn fn);

  void _registerInfix(const TokenType &tokenType, infixParseFn fn);

  [[nodiscard]] bool _currentTokenIs(const TokenType &t) const;

  [[nodiscard]] bool _peekTokenIs(const TokenType &t) const;

  bool _expectPeek(const TokenType &t);

  void _peekError(const TokenType &t);

  [[nodiscard]] int _peekPrecedence() const;

  [[nodiscard]] int _currentPrecedence() const;

  Lexer *_lexer;
  Token _currentToken;
  Token _peekToken;
  std::vector<std::string> _errors;
  std::map<TokenType, prefixParseFn> _prefixParseFns;
  std::map<TokenType, infixParseFn> _infixParseFns;
};

#endif // MONKEY_PARSER_H
