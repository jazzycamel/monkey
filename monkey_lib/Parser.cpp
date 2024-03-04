#include "Parser.h"

#include <utility>

#include "utilities.h"

Parser::Parser(Lexer *lexer) : _lexer(lexer) {
  _nextToken();
  _nextToken();

  _registerPrefix(IDENT, &Parser::_parseIdentifier);
  _registerPrefix(INT, &Parser::_parseIntegerLiteralExpression);
  _registerPrefix(BANG, &Parser::_parsePrefixExpression);
  _registerPrefix(MINUS, &Parser::_parsePrefixExpression);
  _registerPrefix(TRUE, &Parser::_parseBooleanLiteralExpression);
  _registerPrefix(FALSE, &Parser::_parseBooleanLiteralExpression);
  _registerPrefix(LPAREN, &Parser::_parseGroupedExpression);
  _registerPrefix(IF, &Parser::_parseIfExpression);
  _registerPrefix(FUNCTION, &Parser::_parseFunctionLiteralExpression);

  _registerInfix(PLUS, &Parser::_parseInfixExpression);
  _registerInfix(MINUS, &Parser::_parseInfixExpression);
  _registerInfix(SLASH, &Parser::_parseInfixExpression);
  _registerInfix(ASTERISK, &Parser::_parseInfixExpression);
  _registerInfix(EQ, &Parser::_parseInfixExpression);
  _registerInfix(NOT_EQ, &Parser::_parseInfixExpression);
  _registerInfix(LT, &Parser::_parseInfixExpression);
  _registerInfix(GT, &Parser::_parseInfixExpression);
  _registerInfix(LPAREN, &Parser::_parseCallExpression);
}

Program *Parser::parseProgram() {
  auto program = new Program();
  do {
    auto statement = _parseStatement();
    if (statement != nullptr) {
      program->statements.push_back(statement);
    }
    _nextToken();
  } while (_currentToken.type != EOF_);

  return program;
}

std::vector<std::string> Parser::errors() { return _errors; }

void Parser::_nextToken() {
  _currentToken = _peekToken;
  _peekToken = _lexer->nextToken();
}

std::shared_ptr<Statement> Parser::_parseStatement() {
  if (_currentToken.type == LET) {
    return _parseLetStatement();
  } else if (_currentToken.type == RETURN) {
    return _parseReturnStatement();
  } else {
    return _parseExpressionStatement();
  }
}

std::shared_ptr<LetStatement> Parser::_parseLetStatement() {
  LetStatement statement;
  statement.token = _currentToken;

  if (!_expectPeek(IDENT))
    return nullptr;

  statement.name = new Identifier();
  statement.name->token = _currentToken;
  statement.name->value = _currentToken.literal;

  if (!_expectPeek(ASSIGN))
    return nullptr;
  
  _nextToken();
  statement.value = _parseExpression(LOWEST);
  if (_peekTokenIs(SEMICOLON))
    _nextToken();

  return std::make_shared<LetStatement>(statement);
}

std::shared_ptr<ReturnStatement> Parser::_parseReturnStatement() {
  ReturnStatement statement;
  statement.token = _currentToken;

  _nextToken();
  statement.returnValue = _parseExpression(LOWEST);
  if (_peekTokenIs(SEMICOLON))
    _nextToken();

  return std::make_shared<ReturnStatement>(statement);
}

std::shared_ptr<ExpressionStatement> Parser::_parseExpressionStatement() {
  ExpressionStatement statement;
  statement.expression = _parseExpression(LOWEST);

  if (_peekTokenIs(SEMICOLON))
    _nextToken();

  return std::make_shared<ExpressionStatement>(statement);
}

std::shared_ptr<Expression> Parser::_parseExpression(int precedence) {
  auto prefix = _prefixParseFns[_currentToken.type];
  if (prefix == nullptr) {
    _noPrefixParseFnError(_currentToken.type);
    return nullptr;
  }
  auto leftExpression = (this->*prefix)();

  while (!_peekTokenIs(SEMICOLON) && precedence < _peekPrecedence()) {
    auto infix = _infixParseFns[_peekToken.type];
    if (infix == nullptr)
      return leftExpression;
    _nextToken();
    leftExpression = (this->*infix)(leftExpression);
  }

  return leftExpression;
}

std::shared_ptr<Expression> Parser::_parseIdentifier() {
  Identifier identifier;
  identifier.token = _currentToken;
  identifier.value = _currentToken.literal;
  return std::make_shared<Identifier>(identifier);
}

std::shared_ptr<Expression> Parser::_parseIntegerLiteralExpression() {
  IntegerLiteralExpression literal;
  literal.token = _currentToken;
  try {
    literal.value = std::stoll(_currentToken.literal);
  } catch (const std::invalid_argument &ia) {
    auto msg = string_format("Could not parse %s as integer",
                             _currentToken.literal.c_str());
    _errors.push_back(msg);
    return nullptr;
  }
  return std::make_shared<IntegerLiteralExpression>(literal);
}

std::shared_ptr<Expression> Parser::_parsePrefixExpression() {
  PrefixExpression prefix;
  prefix.token = _currentToken;
  prefix.operator_ = _currentToken.literal;
  _nextToken();
  prefix.right = _parseExpression(PREFIX);
  return std::make_shared<PrefixExpression>(prefix);
}

std::shared_ptr<Expression>
Parser::_parseInfixExpression(std::shared_ptr<Expression> left) {
  InfixExpression infix;
  infix.token = _currentToken;
  infix.operator_ = _currentToken.literal;
  infix.left = std::move(left);
  auto precedence = _currentPrecedence();
  _nextToken();
  infix.right = _parseExpression(precedence);
  return std::make_shared<InfixExpression>(infix);
}

std::shared_ptr<Expression> Parser::_parseBooleanLiteralExpression() {
  BooleanLiteralExpression boolean;
  boolean.token = _currentToken;
  boolean.value = _currentTokenIs(TRUE);
  return std::make_shared<BooleanLiteralExpression>(boolean);
}

std::shared_ptr<Expression> Parser::_parseGroupedExpression() {
  _nextToken();
  auto expression = _parseExpression(LOWEST);
  if (!_expectPeek(RPAREN))
    return nullptr;
  return expression;
}

std::shared_ptr<Expression> Parser::_parseIfExpression() {
  IfExpression expression;
  expression.token = _currentToken;

  if (!_expectPeek(LPAREN))
    return nullptr;

  _nextToken();
  expression.condition = _parseExpression(LOWEST);

  if (!_expectPeek(RPAREN))
    return nullptr;

  if (!_expectPeek(LBRACE))
    return nullptr;

  expression.consequence = _parseBlockStatement();

  if (_peekTokenIs(ELSE)) {
    _nextToken();
    if (!_expectPeek(LBRACE))
      return nullptr;
    expression.alternative = _parseBlockStatement();
  }

  return std::make_shared<IfExpression>(expression);
}

std::shared_ptr<BlockStatement> Parser::_parseBlockStatement() {
  BlockStatement block;
  block.token = _currentToken;
  block.statements = {};

  _nextToken();

  while (!_currentTokenIs(RBRACE) && !_currentTokenIs(EOF_)) {
    auto statement = _parseStatement();
    if (statement != nullptr) {
      block.statements.push_back(statement);
    }
    _nextToken();
  }

  return std::make_shared<BlockStatement>(block);
}

std::shared_ptr<Expression> Parser::_parseFunctionLiteralExpression() {
  FunctionLiteralExpression expression;
  expression.token = _currentToken;

  if (!_expectPeek(LPAREN))
    return nullptr;

  expression.parameters = _parseFunctionParameters();

  if (!_expectPeek(LBRACE))
    return nullptr;

  expression.body = _parseBlockStatement();

  return std::make_shared<FunctionLiteralExpression>(expression);
}

std::vector<std::shared_ptr<Identifier>> Parser::_parseFunctionParameters() {
  std::vector<std::shared_ptr<Identifier>> identifiers;

  if (_peekTokenIs(RPAREN)) {
    _nextToken();
    return identifiers;
  }

  _nextToken();

  auto identifier = std::make_shared<Identifier>();
  identifier->token = _currentToken;
  identifier->value = _currentToken.literal;
  identifiers.push_back(identifier);

  while (_peekTokenIs(COMMA)) {
    _nextToken();
    _nextToken();
    identifier = std::make_shared<Identifier>();
    identifier->token = _currentToken;
    identifier->value = _currentToken.literal;
    identifiers.push_back(identifier);
  }

  if (!_expectPeek(RPAREN))
    return {};
  return identifiers;
}

std::shared_ptr<Expression>
Parser::_parseCallExpression(std::shared_ptr<Expression> function) {
  CallExpression expression;
  expression.token = _currentToken;
  expression.function = std::move(function);
  expression.arguments = _parseCallArguments();
  return std::make_shared<CallExpression>(expression);
}

std::vector<std::shared_ptr<Expression>> Parser::_parseCallArguments() {
  std::vector<std::shared_ptr<Expression>> arguments;

  if (_peekTokenIs(RPAREN)) {
    _nextToken();
    return arguments;
  }

  _nextToken();
  arguments.push_back(_parseExpression(LOWEST));

  while (_peekTokenIs(COMMA)) {
    _nextToken();
    _nextToken();
    arguments.push_back(_parseExpression(LOWEST));
  }

  if (!_expectPeek(RPAREN))
    return {};
  return arguments;
}

void Parser::_noPrefixParseFnError(const TokenType &t) {
  auto msg =
      string_format("No prefix parse function for '%s' found", t.c_str());
  _errors.push_back(msg);
}

void Parser::_registerPrefix(const TokenType &tokenType, prefixParseFn fn) {
  _prefixParseFns.insert({tokenType, fn});
}

void Parser::_registerInfix(const TokenType &tokenType, infixParseFn fn) {
  _infixParseFns.insert({tokenType, fn});
}

bool Parser::_currentTokenIs(const TokenType &t) const {
  return _currentToken.type == t;
}

bool Parser::_peekTokenIs(const TokenType &t) const {
  return _peekToken.type == t;
}

bool Parser::_expectPeek(const TokenType &t) {
  if (_peekTokenIs(t)) {
    _nextToken();
    return true;
  }
  _peekError(t);
  return false;
}

void Parser::_peekError(const TokenType &t) {
  auto msg = string_format("Expected next token to be %s, got %s instead",
                           t.c_str(), _peekToken.type.c_str());
  _errors.push_back(msg);
}

int Parser::_peekPrecedence() const {
  try {
    return precedences.at(_peekToken.type);
  } catch (const std::out_of_range &oor) {
    return LOWEST;
  }
}

int Parser::_currentPrecedence() const {
  try {
    return precedences.at(_currentToken.type);
  } catch (const std::out_of_range &oor) {
    return LOWEST;
  }
}