#include <catch2/catch_test_macros.hpp>
#include <utility>

#include "Evaluator.h"
#include "Lexer.h"
#include "Parser.h"
#include <any>

std::shared_ptr<Object> testEval(std::string input) {
  auto lexer = new Lexer(std::move(input));
  auto parser = new Parser(lexer);
  auto program = parser->parseProgram();
  auto evaluator = Evaluator();
  return evaluator.evaluate(program);
}

bool testNullObject(const std::shared_ptr<Object> &obj) {
  auto result = std::dynamic_pointer_cast<NullObject>(obj);
  REQUIRE(result != nullptr);
  return true;
}

bool testIntegerObject(const std::shared_ptr<Object> &obj, int expected) {
  auto result = std::dynamic_pointer_cast<IntegerObject>(obj);
  REQUIRE(result != nullptr);
  REQUIRE(result->value == expected);
  return true;
}

bool testBooleanObject(const std::shared_ptr<Object> &obj, bool expected) {
  auto result = std::dynamic_pointer_cast<BooleanObject>(obj);
  REQUIRE(result != nullptr);
  REQUIRE(result->value == expected);
  return true;
}

TEST_CASE("Evaluator: integer expression") {
  typedef struct {
    std::string input;
    int64_t expected;
  } IntegerObjectTest;

  IntegerObjectTest tests[] = {{"5", 5},
                               {"10", 10},
                               {"-5", -5},
                               {"-10", -10},
                               {"5 + 5 + 5 + 5 - 10", 10},
                               {"2 * 2 * 2 * 2 * 2", 32},
                               {"-50 + 100 + -50", 0},
                               {"5 * 2 + 10", 20},
                               {"5 + 2 * 10", 25},
                               {"20 + 2 * -10", 0},
                               {"50 / 2 * 2 + 10", 60},
                               {"2 * (5 + 10)", 30},
                               {"3 * 3 * 3 + 10", 37},
                               {"3 * (3 * 3) + 10", 37},
                               {"(5 + 10 * 2 + 15 / 3) * 2 + -10", 50}};

  for (const auto &test : tests) {
    auto evaluated = testEval(test.input);
    REQUIRE(testIntegerObject(evaluated, test.expected));
  }
}

TEST_CASE("Evaluator: boolean expression") {
  typedef struct {
    std::string input;
    bool expected;
  } BooleanObjectTest;

  BooleanObjectTest tests[] = {
      {"true", true},    {"false", false}, {"1 < 2", true},  {"1 > 2", false},
      {"1 < 1", false},  {"1 > 1", false}, {"1 == 1", true}, {"1 != 1", false},
      {"1 == 2", false}, {"1 != 2", true}};

  for (const auto &test : tests) {
    auto evaluated = testEval(test.input);
    REQUIRE(testBooleanObject(evaluated, test.expected));
  }
}

TEST_CASE("Evaluator: bang operator") {
  typedef struct {
    std::string input;
    bool expected;
  } BangOperatorTest;

  BangOperatorTest tests[] = {{"!true", false},   {"!false", true},
                              {"!5", false},      {"!!true", true},
                              {"!!false", false}, {"!!5", true}};

  for (const auto &test : tests) {
    auto evaluated = testEval(test.input);
    REQUIRE(testBooleanObject(evaluated, test.expected));
  }
}

TEST_CASE("Evaluator: if else expressions") {
  typedef struct {
    std::string input;
    std::any expected;
  } IfElseExpressionTest;

  IfElseExpressionTest tests[] = {{"if (true) { 10 }", 10},
                                  {"if (false) { 10 }", nullptr},
                                  {"if (1) { 10 }", 10},
                                  {"if (1 < 2) { 10 }", 10},
                                  {"if (1 > 2) { 10 }", nullptr},
                                  {"if (1 > 2) { 10 } else { 20 }", 20},
                                  {"if (1 < 2) { 10 } else { 20 }", 10}};

  for (const auto &test : tests) {
    auto evaluated = testEval(test.input);
    if (test.expected.type() == typeid(int)) {
      REQUIRE(testIntegerObject(evaluated, std::any_cast<int>(test.expected)));
    } else {
      REQUIRE(testNullObject(evaluated));
    }
  }
}

TEST_CASE("Evaluator: return statements") {
  typedef struct {
    std::string input;
    int expected;
  } ReturnStatementTest;

  ReturnStatementTest tests[] = {
      {"return 10;", 10},
      {"return 10; 9;", 10},
      {"return 2 * 5; 9;", 10},
      {"9; return 2 * 5; 9;", 10},
      {"if (10 > 1) { if (10 > 1) { return 10; } return 1; }", 10}};

  for (const auto &test : tests) {
    auto evaluated = testEval(test.input);
    REQUIRE(testIntegerObject(evaluated, test.expected));
  }
}