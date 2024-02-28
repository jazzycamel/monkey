#include <catch2/catch_test_macros.hpp>

#include "AST.h"

TEST_CASE("AST: string()") {
  Identifier name;
  name.token = {.type = IDENT, .literal = "myVar"};
  name.value = "myVar";

  Identifier value;
  value.token = {.type = IDENT, .literal = "anotherVar"};
  value.value = "anotherVar";

  LetStatement statement;
  statement.token = {.type = LET, .literal = "let"};
  statement.name = &name;
  statement.value = std::make_shared<Identifier>(value);

  Program program;
  program.statements.push_back(std::make_shared<LetStatement>(statement));
  REQUIRE(program.string() == "let myVar = anotherVar;");
}