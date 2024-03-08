#include <catch2/catch_test_macros.hpp>
#include <utility>

#include "Evaluator.h"
#include "Lexer.h"
#include "Parser.h"
#include <any>
#include <iostream>

std::shared_ptr<Object> testEval(std::string input) {
  auto lexer = new Lexer(std::move(input));
  auto parser = new Parser(lexer);
  auto program = parser->parseProgram();
  auto environment = std::make_shared<Environment>();
  auto evaluator = Evaluator(environment);
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

TEST_CASE("Evaluator: errors") {
  typedef struct {
    std::string input;
    std::string expectedMessage;
  } ErrorTest;

  ErrorTest tests[] = {
      {"5 + true;", "type mismatch: INTEGER + BOOLEAN"},
      {"5 + true; 5;", "type mismatch: INTEGER + BOOLEAN"},
      {"-true", "unknown operator: -BOOLEAN"},
      {"true + false;", "unknown operator: BOOLEAN + BOOLEAN"},
      {"5; true + false; 5", "unknown operator: BOOLEAN + BOOLEAN"},
      {"if (10 > 1) { true + false; }", "unknown operator: BOOLEAN + BOOLEAN"},
      {"if (10 > 1) { if (10 > 1) { return true + false; } return 1; }",
       "unknown operator: BOOLEAN + BOOLEAN"},
      {"foobar", "identifier not found: foobar"}};

  for (const auto &test : tests) {
    auto evaluated = testEval(test.input);
    auto result = std::dynamic_pointer_cast<ErrorObject>(evaluated);
    REQUIRE(result != nullptr);
    REQUIRE(result->message == test.expectedMessage);
  }
}

TEST_CASE("Evaluator: let statements") {
  typedef struct {
    std::string input;
    int expected;
  } LetStatementTest;

  LetStatementTest tests[] = {
      {"let a = 5; a;", 5},
      {"let a = 5 * 5; a;", 25},
      {"let a = 5; let b = a; b;", 5},
      {"let a = 5; let b = a; let c = a + b + 5; c;", 15}};

  for (const auto &test : tests) {
    auto evaluated = testEval(test.input);
    REQUIRE(testIntegerObject(evaluated, test.expected));
  }
}

TEST_CASE("Evaluator: function object") {
  auto input = "fn(x) { x + 2; };";
  auto evaluated = testEval(input);
  auto result = std::dynamic_pointer_cast<FunctionObject>(evaluated);
  REQUIRE(result != nullptr);
  REQUIRE(result->parameters.size() == 1);
  REQUIRE(result->parameters[0]->string() == "x");
  REQUIRE(result->body->string() == "(x + 2)");
}

TEST_CASE("Evaluator: function application") {
  typedef struct {
    std::string input;
    int64_t expected;
  } FunctionTest;

  FunctionTest tests[] = {
      {"let identity = fn(x) { x; }; identity(5);", 5},
      {"let identity = fn(x) { return x; }; identity(5);", 5},
      {"let double = fn(x) { x * 2; }; double(5);", 10},
      {"let add = fn(x, y) { x + y; }; add(5, 5);", 10},
      {"let add = fn(x, y) { x + y; }; add(5 + 5, add(5, 5));", 20},
      {"fn(x) { x; }(5)", 5}};

  for (auto &test : tests) {
    auto evaluated = testEval(test.input);
    REQUIRE(testIntegerObject(evaluated, test.expected));
  }
}

TEST_CASE("Evaluator: closures") {
  auto input = "let newAdder = fn(x) { fn(y) { x + y }; }; let addTwo = "
               "newAdder(2); addTwo(x);";
  auto evaluated = testEval(input);
  REQUIRE(testIntegerObject(evaluated, 4));
}

TEST_CASE("Evaluator: string literal") {
  auto input = R"("hello world")";
  auto evaluated = testEval(input);
  auto result = std::dynamic_pointer_cast<StringObject>(evaluated);
  REQUIRE(result != nullptr);
  REQUIRE(result->value == "hello world");
}