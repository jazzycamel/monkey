#include "REPL.h"
#include "Lexer.h"
#include "Token.h"
#include <iostream>
#include <string>

void REPL::start() {
  while (true) {
    std::cout << PROMPT;
    std::string line;
    std::getline(std::cin, line);

    auto lexer = new Lexer(line);
    auto token = lexer->nextToken();
    do {
      std::cout << token << std::endl;
      token = lexer->nextToken();
    } while (token.type != EOF_);
  }
}