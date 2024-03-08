#ifndef MONKEY_LEXER_H
#define MONKEY_LEXER_H

#include "Token.h"
#include <string>

class Lexer {
public:
  explicit Lexer(std::string input);

  Token nextToken();

private:
  void _readChar();

  char _peekChar();

  std::string _readNumber();

  std::string _readIdentifier();

  std::string _readString();

  static bool _isDigit(char ch);

  static bool _isLetter(char ch);

  void _skipWhiteSpace();

  std::string _input;
  int _position;
  int _readPosition;
  char _ch;
};

#endif // MONKEY_LEXER_H
