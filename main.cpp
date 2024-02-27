#include <iostream>
#include "REPL.h"
#include "utilities.h"

int main() {
    std::cout << "Hello " << getCurrentUser() << "! This is the monkey programming language!" << std::endl;
    std::cout << "Feel free to type in commands" << std::endl;
    auto repl = new REPL();
    repl->start();
    return 0;
}
