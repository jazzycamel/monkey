#ifndef MONKEY_REPL_H
#define MONKEY_REPL_H

#include <iostream>
#include <string>

const auto PROMPT = ">> ";

class REPL {
public:
  static void start();

private:
  static void _printParseErrors(const std::vector<std::string> &errors);
};

#endif // MONKEY_REPL_H
