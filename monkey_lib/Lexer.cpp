#include "Lexer.h"

#include <utility>

Lexer::Lexer(std::string input) : _input(std::move(input)) {
  _position = 0;
  _readPosition = 0;
  _ch = 0;

  _readChar();
}

Token Lexer::nextToken() {
  _skipWhiteSpace();

  Token token;
  token.literal = std::string{_ch};

  switch (_ch) {
  case '=':
    if (_peekChar() == '=') {
      auto ch = _ch;
      _readChar();
      token = {.type = EQ, .literal = std::string() + ch + _ch};
    } else
      token.type = ASSIGN;
    break;
  case ';':
    token.type = SEMICOLON;
    break;
  case '(':
    token.type = LPAREN;
    break;
  case ')':
    token.type = RPAREN;
    break;
  case '{':
    token.type = LBRACE;
    break;
  case '}':
    token.type = RBRACE;
    break;
  case ',':
    token.type = COMMA;
    break;
  case '+':
    token.type = PLUS;
    break;
  case '-':
    token.type = MINUS;
    break;
  case '!':
    if (_peekChar() == '=') {
      auto ch = _ch;
      _readChar();
      token = {.type = NOT_EQ, .literal = std::string() + ch + _ch};
    } else
      token.type = BANG;
    break;
  case '/':
    token.type = SLASH;
    break;
  case '*':
    token.type = ASTERISK;
    break;
  case '<':
    token.type = LT;
    break;
  case '>':
    token.type = GT;
    break;
  case '"':
    token.type = STRING;
    token.literal = _readString();
    break;
  case 0:
    token.literal = "";
    token.type = EOF_;
    break;
  default:
    if (_isLetter(_ch)) {
      token.literal = _readIdentifier();
      token.type = lookupIdent(token.literal);
      return token;
    } else if (_isDigit(_ch)) {
      token.type = INT;
      token.literal = _readNumber();
      return token;
    } else
      token.type = ILLEGAL;
  }

  _readChar();
  return token;
}

void Lexer::_readChar() {
  if (_readPosition >= _input.length())
    _ch = 0;
  else
    _ch = _input[_readPosition];
  _position = _readPosition;
  _readPosition++;
}

char Lexer::_peekChar() {
  if (_readPosition >= _input.length())
    return 0;
  return _input[_readPosition];
}

std::string Lexer::_readNumber() {
  auto position = _position;
  while (_isDigit(_ch))
    _readChar();
  return _input.substr(position, _position - position);
}

std::string Lexer::_readIdentifier() {
  auto position = _position;
  while (_isLetter(_ch))
    _readChar();
  return _input.substr(position, _position - position);
}

std::string Lexer::_readString() {
  auto position = _position + 1;
  do {
    _readChar();
  } while (_ch != '"' && _ch != 0);
  return _input.substr(position, _position - position);
}

bool Lexer::_isDigit(char ch) { return '0' <= ch && ch <= '9'; }

bool Lexer::_isLetter(char ch) {
  return ('a' <= ch && ch <= 'z') || ('A' <= ch && ch <= 'Z') || ch == '_';
}

void Lexer::_skipWhiteSpace() {
  while (_ch == ' ' || _ch == '\t' || _ch == '\n' || _ch == '\r')
    _readChar();
}