#include "REPL.h"
#include "Evaluator.h"
#include "Lexer.h"
#include "Parser.h"

void REPL::start() {
  auto environment = std::make_shared<Environment>();
  auto evaluator = Evaluator(environment);

  while (true) {
    std::cout << PROMPT;
    std::string line;
    std::getline(std::cin, line);

    auto lexer = new Lexer(line);
    auto parser = new Parser(lexer);
    auto program = parser->parseProgram();
    if (!parser->errors().empty()) {
      _printParseErrors(parser->errors());
      continue;
    }

    auto evaluated = evaluator.evaluate(program);
    if (evaluated != nullptr) {
      std::cout << evaluated->inspect() << std::endl;
    }
  }
}

const auto MONKEY_FACE = R"(
           __,__
  .--.  .-"     "-.  .--.
 / .. \/  .-. .-.  \/ .. \
| |  '|  /   Y   \  |'  | |
| \   \  \ 0 | 0 /  /   / |
 \ '- ,\.-"""""""-./, -' /
  ''-' /_   ^ ^   _\ '-''
      |  \._   _./  |
      \   \ '~' /   /
       '._ '-=-' _.'
          '-----'
)";

void REPL::_printParseErrors(const std::vector<std::string> &errors) {
  std::cout << MONKEY_FACE << std::endl;
  std::cout << "Whoops! We ran into some monkey business here!" << std::endl;
  std::cout << " parser errors:" << std::endl;
  for (const auto &error : errors) {
    std::cout << "\t" << error << std::endl;
  }
}