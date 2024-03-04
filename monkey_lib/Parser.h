#ifndef MONKEY_PARSER_H
#define MONKEY_PARSER_H

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "AST.h"
#include "Lexer.h"
#include "Token.h"

class Parser;

typedef std::shared_ptr<Expression> (Parser::*prefixParseFn)();

typedef std::shared_ptr<Expression> (Parser::*infixParseFn)(
    std::shared_ptr<Expression>);

enum { LOWEST = 1, EQUALS, LESS_GREATER, SUM, PRODUCT, PREFIX, CALL };

const std::map<TokenType, int> precedences = {
    {EQ, EQUALS},       {NOT_EQ, EQUALS},    {LT, LESS_GREATER},
    {GT, LESS_GREATER}, {PLUS, SUM},         {MINUS, SUM},
    {SLASH, PRODUCT},   {ASTERISK, PRODUCT}, {LPAREN, CALL}};

class Parser {
public:
  explicit Parser(Lexer *lexer);

  Program *parseProgram();

  std::vector<std::string> errors();

private:
  void _nextToken();

  std::shared_ptr<Statement> _parseStatement();

  std::shared_ptr<LetStatement> _parseLetStatement();

  std::shared_ptr<ReturnStatement> _parseReturnStatement();

  std::shared_ptr<ExpressionStatement> _parseExpressionStatement();

  std::shared_ptr<Expression> _parseExpression(int precedence);

  std::shared_ptr<Expression> _parseIdentifier();

  std::shared_ptr<Expression> _parseIntegerLiteralExpression();

  std::shared_ptr<Expression> _parsePrefixExpression();

  std::shared_ptr<Expression>
  _parseInfixExpression(std::shared_ptr<Expression> left);

  std::shared_ptr<Expression> _parseBooleanLiteralExpression();

  std::shared_ptr<Expression> _parseGroupedExpression();

  std::shared_ptr<Expression> _parseIfExpression();

  std::shared_ptr<BlockStatement> _parseBlockStatement();

  std::shared_ptr<Expression> _parseFunctionLiteralExpression();

  std::vector<std::shared_ptr<Identifier>> _parseFunctionParameters();

  std::shared_ptr<Expression>
  _parseCallExpression(std::shared_ptr<Expression> function);

  std::vector<std::shared_ptr<Expression>> _parseCallArguments();

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
