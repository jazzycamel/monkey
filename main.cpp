#include "FileRunner.h"
#include "REPL.h"
#include "utilities.h"
#include <iostream>

int main(int argc, char *argv[]) {
  if (argc >= 2) {
    return FileRunner::run(argv[1]);
  }
  std::cout << "Hello " << getCurrentUser()
            << "! This is the monkey programming language!" << std::endl;
  std::cout << "Feel free to type in commands" << std::endl;
  REPL::start();
  return 0;
}
