#include <catch2/catch_test_macros.hpp>

#include "Lexer.h"
#include "Parser.h"

#include <any>
#include <iostream>

void checkErrors(const std::vector<std::string> &errors) {
  if (!errors.empty()) {
    for (const auto &error : errors) {
      UNSCOPED_INFO("Parser error: " << error);
    }
  }
  REQUIRE(errors.empty());
}

bool testIntegerLiteral(const std::shared_ptr<Expression> &expression,
                        const int64_t value) {
  auto literal = dynamic_cast<IntegerLiteralExpression *>(expression.get());
  REQUIRE(literal != nullptr);
  REQUIRE(literal->value == value);
  return true;
}

bool testIdentifier(const std::shared_ptr<Expression> &expression,
                    const std::string &value) {
  auto identifier = dynamic_cast<Identifier *>(expression.get());
  REQUIRE(identifier != nullptr);
  REQUIRE(identifier->value == value);
  REQUIRE(identifier->tokenLiteral() == value);
  return true;
}

bool testBooleanLiteral(const std::shared_ptr<Expression> &expression,
                        const bool value) {
  auto boolean = dynamic_cast<BooleanLiteralExpression *>(expression.get());
  REQUIRE(boolean != nullptr);
  REQUIRE(boolean->value == value);
  REQUIRE(boolean->tokenLiteral() == (value ? "true" : "false"));
  return true;
}

bool testLiteralExpression(const std::shared_ptr<Expression> &expression,
                           const std::any &value) {
  if (value.type() == typeid(int)) {
    return testIntegerLiteral(expression, (int64_t)std::any_cast<int>(value));
  } else if (value.type() == typeid(int64_t)) {
    return testIntegerLiteral(expression, std::any_cast<int64_t>(value));
  } else if (value.type() == typeid(bool)) {
    return testBooleanLiteral(expression, std::any_cast<bool>(value));
  } else if (value.type() == typeid(std::string)) {
    return testIdentifier(expression, std::any_cast<std::string>(value));
  }
  return false;
}

bool testInfixExpression(const std::shared_ptr<Expression> &expression,
                         const std::any &left, std::string operator_,
                         const std::any &right) {
  auto infix = dynamic_cast<InfixExpression *>(expression.get());
  REQUIRE(infix != nullptr);
  REQUIRE(testLiteralExpression(infix->left, left));
  REQUIRE(infix->operator_ == operator_);
  REQUIRE(testLiteralExpression(infix->right, right));
  return true;
}

bool testLetStatement(const std::shared_ptr<Statement> &statement,
                      const std::string &name) {
  REQUIRE(statement->tokenLiteral() == "let");
  auto letStatement = dynamic_cast<LetStatement *>(statement.get());
  REQUIRE(letStatement != nullptr);
  REQUIRE(letStatement->name->value == name);
  REQUIRE(letStatement->name->tokenLiteral() == name);
  return true;
}

TEST_CASE("Parser: test let statements") {
  typedef struct {
    std::string input;
    std::string expectedIdentifier;
    std::any expectedValue;
  } LetStatementTest;

  LetStatementTest tests[] = {
      {"let x = 5;", "x", 5},
      {"let y = true;", "y", true},
      {"let foobar = y;", "foobar", std::string("y")},
  };

  for (const auto &tt : tests) {
    auto lexer = new Lexer(tt.input);
    auto parser = new Parser(lexer);
    auto program = parser->parseProgram();
    checkErrors(parser->errors());

    REQUIRE(program != nullptr);
    REQUIRE(program->statements.size() == 1);
    auto statement = program->statements[0];
    REQUIRE(testLetStatement(statement, tt.expectedIdentifier));
    auto letStatement = dynamic_cast<LetStatement *>(statement.get());
    REQUIRE(letStatement != nullptr);
    REQUIRE(testLiteralExpression(letStatement->value, tt.expectedValue));
  }
}

TEST_CASE("Parser: test let statement errors") {
  std::string input = R"(
let x 5;
let = 10;
let 838383;
)";

  auto lexer = new Lexer(input);
  auto parser = new Parser(lexer);
  parser->parseProgram();

  REQUIRE_FALSE(parser->errors().empty());
  REQUIRE(parser->errors().size() == 4);
  REQUIRE(parser->errors().at(0) ==
          "Expected next token to be =, got INT instead");
  REQUIRE(parser->errors().at(1) ==
          "Expected next token to be IDENT, got = instead");
  REQUIRE(parser->errors().at(2) == "No prefix parse function for '=' found");
  REQUIRE(parser->errors().at(3) ==
          "Expected next token to be IDENT, got INT instead");
}

TEST_CASE("Parser: test return statement") {
  typedef struct {
    std::string input;
    std::any expectedValue;
  } ReturnStatementTest;

  ReturnStatementTest tests[] = {
      {"return 5;", 5},
      {"return true;", true},
      {"return foobar;", std::string("foobar")},
  };

  for (const auto &tt : tests) {
    auto lexer = new Lexer(tt.input);
    auto parser = new Parser(lexer);
    auto program = parser->parseProgram();
    checkErrors(parser->errors());

    REQUIRE(program != nullptr);
    REQUIRE(program->statements.size() == 1);
    auto statement = program->statements[0];
    REQUIRE(statement->tokenLiteral() == "return");
    auto returnStatement = dynamic_cast<ReturnStatement *>(statement.get());
    REQUIRE(returnStatement != nullptr);
    REQUIRE(
        testLiteralExpression(returnStatement->returnValue, tt.expectedValue));
  }
}

TEST_CASE("Parser: identifier expressions") {
  std::string input = "foobar;";

  auto lexer = new Lexer(input);
  auto parser = new Parser(lexer);
  auto program = parser->parseProgram();
  checkErrors(parser->errors());

  REQUIRE(program->statements.size() == 1);
  auto expressionStatement =
      dynamic_cast<ExpressionStatement *>(program->statements[0].get());
  REQUIRE(expressionStatement != nullptr);

  auto identifier =
      dynamic_cast<Identifier *>(expressionStatement->expression.get());
  REQUIRE(identifier != nullptr);
  REQUIRE(identifier->value == "foobar");
  REQUIRE(identifier->tokenLiteral() == "foobar");
}

TEST_CASE("Parser: integer literal expression") {
  std::string input = "5;";

  auto lexer = new Lexer(input);
  auto parser = new Parser(lexer);
  auto program = parser->parseProgram();
  checkErrors(parser->errors());

  REQUIRE(program->statements.size() == 1);
  auto expressionStatement =
      dynamic_cast<ExpressionStatement *>(program->statements[0].get());
  REQUIRE(expressionStatement != nullptr);

  auto literal = dynamic_cast<IntegerLiteralExpression *>(
      expressionStatement->expression.get());
  REQUIRE(literal != nullptr);
  REQUIRE(literal->value == 5);
  REQUIRE(literal->tokenLiteral() == "5");
}

TEST_CASE("Parser: prefix expressions") {
  typedef struct {
    std::string input;
    std::string operator_;
    std::any value;
  } PrefixTest;

  PrefixTest prefixTests[] = {
      {"!5", "!", 5},
      {"-15", "-", 15},
      {"!true;", "!", true},
      {"!false;", "!", false},
  };

  for (const auto &tt : prefixTests) {
    auto lexer = new Lexer(tt.input);
    auto parser = new Parser(lexer);
    auto program = parser->parseProgram();
    checkErrors(parser->errors());

    REQUIRE(program->statements.size() == 1);
    auto expressionStatement =
        dynamic_cast<ExpressionStatement *>(program->statements[0].get());
    REQUIRE(expressionStatement != nullptr);

    auto prefix =
        dynamic_cast<PrefixExpression *>(expressionStatement->expression.get());
    REQUIRE(prefix != nullptr);
    REQUIRE(prefix->operator_ == tt.operator_);
    REQUIRE(testLiteralExpression(prefix->right, tt.value));
  }
}

TEST_CASE("Parser: infix expressions") {
  typedef struct {
    std::string input;
    std::any leftValue;
    std::string operator_;
    std::any rightValue;
  } InfixTest;

  InfixTest infixTests[] = {
      {"5 + 5", 5, "+", 5},
      {"5 - 5", 5, "-", 5},
      {"5 * 5", 5, "*", 5},
      {"5 / 5", 5, "/", 5},
      {"5 > 5", 5, ">", 5},
      {"5 < 5", 5, "<", 5},
      {"5 == 5", 5, "==", 5},
      {"5 != 5", 5, "!=", 5},
      {"true == true", true, "==", true},
      {"true != false", true, "!=", false},
      {"false == false", false, "==", false},
  };

  for (const auto &tt : infixTests) {
    auto lexer = new Lexer(tt.input);
    auto parser = new Parser(lexer);
    auto program = parser->parseProgram();
    checkErrors(parser->errors());

    REQUIRE(program->statements.size() == 1);
    auto expressionStatement =
        dynamic_cast<ExpressionStatement *>(program->statements[0].get());
    REQUIRE(expressionStatement != nullptr);

    testInfixExpression(expressionStatement->expression, tt.leftValue,
                        tt.operator_, tt.rightValue);
  }
}

TEST_CASE("Parser: operator precedence") {
  typedef struct {
    std::string input;
    std::string expected;
  } PrecedenceTest;

  PrecedenceTest precedenceTests[] = {
      {"-a * b", "((-a) * b)"},
      {"!-a", "(!(-a))"},
      {"a + b + c", "((a + b) + c)"},
      {"a + b - c", "((a + b) - c)"},
      {"a * b * c", "((a * b) * c)"},
      {"a * b / c", "((a * b) / c)"},
      {"a + b / c", "(a + (b / c))"},
      {"a + b * c + d / e - f", "(((a + (b * c)) + (d / e)) - f)"},
      {"3 + 4; -5 * 5", "(3 + 4)((-5) * 5)"},
      {"5 > 4 == 3 < 4", "((5 > 4) == (3 < 4))"},
      {"5 < 4 != 3 > 4", "((5 < 4) != (3 > 4))"},
      {"3 + 4 * 5 == 3 * 1 + 4 * 5", "((3 + (4 * 5)) == ((3 * 1) + (4 * 5)))"},
      {"true", "true"},
      {"false", "false"},
      {"3 > 5 == false", "((3 > 5) == false)"},
      {"3 < 5 == true", "((3 < 5) == true)"},
      {"1 + (2 + 3) + 4", "((1 + (2 + 3)) + 4)"},
      {"(5 + 5) * 2", "((5 + 5) * 2)"},
      {"2 / (5 + 5)", "(2 / (5 + 5))"},
      {"-(5 + 5)", "(-(5 + 5))"},
      {"!(true == true)", "(!(true == true))"},
      {"a + add(b * c) + d", "((a + add((b * c))) + d)"},
      {"add(a, b, 1, 2 * 3, 4 + 5, add(6, 7 * 8))",
       "add(a, b, 1, (2 * 3), (4 + 5), add(6, (7 * 8)))"},
      {"add(a + b + c * d / f + g)", "add((((a + b) + ((c * d) / f)) + g))"},
  };

  for (const auto &tt : precedenceTests) {
    auto lexer = new Lexer(tt.input);
    auto parser = new Parser(lexer);
    auto program = parser->parseProgram();
    checkErrors(parser->errors());

    auto actual = program->string();
    REQUIRE(actual == tt.expected);
  }
}

TEST_CASE("Parser: boolean expression") {
  typedef struct {
    std::string input;
    bool booleanValue;
  } BooleanTest;

  BooleanTest booleanTests[] = {
      {"true;", true},
      {"false;", false},
  };

  for (const auto &tt : booleanTests) {
    auto lexer = new Lexer(tt.input);
    auto parser = new Parser(lexer);
    auto program = parser->parseProgram();
    checkErrors(parser->errors());

    REQUIRE(program->statements.size() == 1);
    auto expressionStatement =
        dynamic_cast<ExpressionStatement *>(program->statements[0].get());
    REQUIRE(expressionStatement != nullptr);

    auto boolean = dynamic_cast<BooleanLiteralExpression *>(
        expressionStatement->expression.get());
    REQUIRE(boolean != nullptr);
    REQUIRE(boolean->value == tt.booleanValue);
  }
}

TEST_CASE("Parser: if expression") {
  std::string input = "if (x < y) { x }";

  auto lexer = new Lexer(input);
  auto parser = new Parser(lexer);
  auto program = parser->parseProgram();
  checkErrors(parser->errors());

  REQUIRE(program->statements.size() == 1);
  auto expressionStatement =
      dynamic_cast<ExpressionStatement *>(program->statements[0].get());
  REQUIRE(expressionStatement != nullptr);

  auto ifExpression =
      dynamic_cast<IfExpression *>(expressionStatement->expression.get());
  REQUIRE(ifExpression != nullptr);
  REQUIRE(testInfixExpression(ifExpression->condition, std::string("x"), "<",
                              std::string("y")));

  REQUIRE(ifExpression->consequence->statements.size() == 1);
  auto consequence = dynamic_cast<ExpressionStatement *>(
      ifExpression->consequence->statements[0].get());
  REQUIRE(consequence != nullptr);
  REQUIRE(testIdentifier(consequence->expression, "x"));
  REQUIRE(ifExpression->alternative == nullptr);
}

TEST_CASE("Parser: if else expression") {
  std::string input = "if (x < y) { x } else { y }";

  auto lexer = new Lexer(input);
  auto parser = new Parser(lexer);
  auto program = parser->parseProgram();
  checkErrors(parser->errors());

  REQUIRE(program->statements.size() == 1);
  auto expressionStatement =
      dynamic_cast<ExpressionStatement *>(program->statements[0].get());
  REQUIRE(expressionStatement != nullptr);

  auto ifExpression =
      dynamic_cast<IfExpression *>(expressionStatement->expression.get());
  REQUIRE(ifExpression != nullptr);
  REQUIRE(testInfixExpression(ifExpression->condition, std::string("x"), "<",
                              std::string("y")));

  REQUIRE(ifExpression->consequence->statements.size() == 1);
  auto consequence = dynamic_cast<ExpressionStatement *>(
      ifExpression->consequence->statements[0].get());
  REQUIRE(consequence != nullptr);
  REQUIRE(testIdentifier(consequence->expression, "x"));

  REQUIRE(ifExpression->alternative->statements.size() == 1);
  auto alternative = dynamic_cast<ExpressionStatement *>(
      ifExpression->alternative->statements[0].get());
  REQUIRE(alternative != nullptr);
  REQUIRE(testIdentifier(alternative->expression, "y"));
}

TEST_CASE("Parser: function literal") {
  std::string input = "fn(x, y) { x + y; }";

  auto lexer = new Lexer(input);
  auto parser = new Parser(lexer);
  auto program = parser->parseProgram();
  checkErrors(parser->errors());

  REQUIRE(program->statements.size() == 1);
  auto expressionStatement =
      dynamic_cast<ExpressionStatement *>(program->statements[0].get());
  REQUIRE(expressionStatement != nullptr);

  auto function = dynamic_cast<FunctionLiteralExpression *>(
      expressionStatement->expression.get());
  REQUIRE(function != nullptr);
  REQUIRE(function->parameters.size() == 2);
  REQUIRE(testLiteralExpression(function->parameters[0], std::string("x")));
  REQUIRE(testLiteralExpression(function->parameters[1], std::string("y")));

  REQUIRE(function->body->statements.size() == 1);
  auto body =
      dynamic_cast<ExpressionStatement *>(function->body->statements[0].get());
  REQUIRE(body != nullptr);
  REQUIRE(testInfixExpression(body->expression, std::string("x"), "+",
                              std::string("y")));
}

TEST_CASE("Parser: function parameters") {
  typedef struct {
    std::string input;
    std::vector<std::string> expectedParams;
  } FunctionParameterTest;

  FunctionParameterTest functionParameterTests[] = {
      {"fn() {};", {}},
      {"fn(x) {};", {"x"}},
      {"fn(x, y, z) {};", {"x", "y", "z"}}};

  for (auto tt : functionParameterTests) {
    auto lexer = new Lexer(tt.input);
    auto parser = new Parser(lexer);
    auto program = parser->parseProgram();
    checkErrors(parser->errors());

    REQUIRE(program->statements.size() == 1);
    auto expressionStatement =
        dynamic_cast<ExpressionStatement *>(program->statements[0].get());
    REQUIRE(expressionStatement != nullptr);

    auto function = dynamic_cast<FunctionLiteralExpression *>(
        expressionStatement->expression.get());
    REQUIRE(function != nullptr);
    REQUIRE(function->parameters.size() == tt.expectedParams.size());
    for (int i = 0; i < tt.expectedParams.size(); i++) {
      REQUIRE(
          testLiteralExpression(function->parameters[i], tt.expectedParams[i]));
    }
  }
}

TEST_CASE("Parser: call expression") {
  std::string input = "add(1, 2 * 3, 4 + 5);";

  auto lexer = new Lexer(input);
  auto parser = new Parser(lexer);
  auto program = parser->parseProgram();
  checkErrors(parser->errors());

  REQUIRE(program->statements.size() == 1);
  auto expressionStatement =
      dynamic_cast<ExpressionStatement *>(program->statements[0].get());
  REQUIRE(expressionStatement != nullptr);

  auto call =
      dynamic_cast<CallExpression *>(expressionStatement->expression.get());
  REQUIRE(call != nullptr);
  REQUIRE(testIdentifier(call->function, "add"));
  REQUIRE(call->arguments.size() == 3);
  REQUIRE(testLiteralExpression(call->arguments[0], 1));
  REQUIRE(testInfixExpression(call->arguments[1], 2, "*", 3));
  REQUIRE(testInfixExpression(call->arguments[2], 4, "+", 5));
}