#ifndef MONKEY_REPL_H
#define MONKEY_REPL_H

#include <iostream>
#include <string>
#include <vector>

const auto PROMPT = ">> ";

class REPL {
public:
  static void start();
  static void printParseErrors(const std::vector<std::string> &errors);
};

#endif // MONKEY_REPL_H
