#include "FileRunner.h"
#include "Evaluator.h"
#include "Lexer.h"
#include "Object.h"
#include "Parser.h"
#include "REPL.h"

#include <fstream>
#include <iostream>
#include <sstream>

int FileRunner::run(const std::string &path) {
  std::ifstream in(path);
  if (!in) {
    std::cerr << "monkey: could not open file: " << path << std::endl;
    return 1;
  }
  std::stringstream buf;
  buf << in.rdbuf();

  auto environment = std::make_shared<Environment>();
  Evaluator evaluator(environment);

  auto lexer = new Lexer(buf.str());
  auto parser = new Parser(lexer);
  auto program = parser->parseProgram();
  if (!parser->errors().empty()) {
    REPL::printParseErrors(parser->errors());
    return 1;
  }

  auto evaluated = evaluator.evaluate(program);
  if (evaluated != nullptr && evaluated->type() == ERROR_OBJ) {
    std::cerr << "runtime error: " << evaluated->inspect() << std::endl;
    return 1;
  }
  return 0;
}
