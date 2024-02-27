#include <catch2/catch_test_macros.hpp>

#include "Lexer.h"
#include "Parser.h"

void checkErrors(const std::vector<std::string> &errors) {
    if (!errors.empty()) {
        for (const auto &error: errors) {
            UNSCOPED_INFO("Parser error: " << error);
        }
    }
    REQUIRE(errors.empty());
}

TEST_CASE("Parser: test let statements") {
    std::string input = R"(
let x = 5;
let y = 10;
let foobar = 838383;
)";

    auto lexer = new Lexer(input);
    auto parser = new Parser(lexer);
    auto program = parser->parseProgram();
    checkErrors(parser->errors());

    REQUIRE(program != nullptr);
    REQUIRE(program->statements.size() == 3);

    std::string tests[] = {
            "x",
            "y",
            "foobar"
    };

    int counter = 0;
    for (auto tt: tests) {
        auto statement = program->statements[counter];
        REQUIRE(statement->tokenLiteral() == "let");
        auto letStatement = dynamic_cast<LetStatement *>(statement.get());
        REQUIRE(letStatement != nullptr);
        REQUIRE(letStatement->name->value == tt);
        REQUIRE(letStatement->name->tokenLiteral() == tt);
        counter++;
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
    REQUIRE(parser->errors().at(0) == "Expected next token to be =, got INT instead");
    REQUIRE(parser->errors().at(1) == "Expected next token to be IDENT, got = instead");
    REQUIRE(parser->errors().at(2) == "No prefix parse function for '=' found");
    REQUIRE(parser->errors().at(3) == "Expected next token to be IDENT, got INT instead");
}

TEST_CASE("Parser: test return statement") {
    std::string input = R"(
return 5;
return 10;
return 993322;
)";

    auto lexer = new Lexer(input);
    auto parser = new Parser(lexer);
    auto program = parser->parseProgram();
    checkErrors(parser->errors());

    REQUIRE(program != nullptr);
    REQUIRE(program->statements.size() == 3);
    for (const auto &statement: program->statements) {
        REQUIRE(statement->tokenLiteral() == "return");
    }
}

TEST_CASE("Parser: identifier expressions") {
    std::string input = "foobar;";

    auto lexer = new Lexer(input);
    auto parser = new Parser(lexer);
    auto program = parser->parseProgram();
    checkErrors(parser->errors());

    REQUIRE(program->statements.size() == 1);
    auto expressionStatement = dynamic_cast<ExpressionStatement *>(program->statements[0].get());
    REQUIRE(expressionStatement != nullptr);

    auto identifier = dynamic_cast<Identifier *>(expressionStatement->expression.get());
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
    auto expressionStatement = dynamic_cast<ExpressionStatement *>(program->statements[0].get());
    REQUIRE(expressionStatement != nullptr);

    auto literal = dynamic_cast<IntegerLiteralExpression *>(expressionStatement->expression.get());
    REQUIRE(literal != nullptr);
    REQUIRE(literal->value == 5);
    REQUIRE(literal->tokenLiteral() == "5");
}

bool testIntegerLiteral(const std::shared_ptr<Expression> &expression, const int64_t value) {
    auto literal = dynamic_cast<IntegerLiteralExpression *>(expression.get());
    REQUIRE(literal != nullptr);
    REQUIRE(literal->value == value);
    return true;
}

TEST_CASE("Parser: prefix expressions") {
    typedef struct {
        std::string input;
        std::string operator_;
        int64_t integerValue;
    } PrefixTest;

    PrefixTest prefixTests[] = {
            {"!5",  "!", 5},
            {"-15", "-", 15}
    };

    for (const auto &tt: prefixTests) {
        auto lexer = new Lexer(tt.input);
        auto parser = new Parser(lexer);
        auto program = parser->parseProgram();
        checkErrors(parser->errors());

        REQUIRE(program->statements.size() == 1);
        auto expressionStatement = dynamic_cast<ExpressionStatement *>(program->statements[0].get());
        REQUIRE(expressionStatement != nullptr);

        auto prefix = dynamic_cast<PrefixExpression *>(expressionStatement->expression.get());
        REQUIRE(prefix != nullptr);
        REQUIRE(prefix->operator_ == tt.operator_);
        REQUIRE(testIntegerLiteral(prefix->right, tt.integerValue));
    }
}

TEST_CASE("Parser: infix expressions") {
    typedef struct {
        std::string input;
        int64_t leftValue;
        std::string operator_;
        int64_t rightValue;
    } InfixTest;

    InfixTest infixTests[] = {
            {"5 + 5",  5, "+",  5},
            {"5 - 5",  5, "-",  5},
            {"5 * 5",  5, "*",  5},
            {"5 / 5",  5, "/",  5},
            {"5 > 5",  5, ">",  5},
            {"5 < 5",  5, "<",  5},
            {"5 == 5", 5, "==", 5},
            {"5 != 5", 5, "!=", 5},
    };

    for (const auto &tt: infixTests) {
        auto lexer = new Lexer(tt.input);
        auto parser = new Parser(lexer);
        auto program = parser->parseProgram();
        checkErrors(parser->errors());

        REQUIRE(program->statements.size() == 1);
        auto expressionStatement = dynamic_cast<ExpressionStatement *>(program->statements[0].get());
        REQUIRE(expressionStatement != nullptr);

        auto infix = dynamic_cast<InfixExpression *>(expressionStatement->expression.get());
        REQUIRE(infix != nullptr);
        REQUIRE(testIntegerLiteral(infix->left, tt.leftValue));
        REQUIRE(infix->operator_ == tt.operator_);
        REQUIRE(testIntegerLiteral(infix->right, tt.rightValue));
    }
}

TEST_CASE("Parser: operator precedence") {
    typedef struct {
        std::string input;
        std::string expected;
    } PrecedenceTest;

    PrecedenceTest precedenceTests[] = {
            {"-a * b",    "((-a) * b)"},
            {"!-a",       "(!(-a))"},
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
    };

    for (const auto &tt: precedenceTests) {
        auto lexer = new Lexer(tt.input);
        auto parser = new Parser(lexer);
        auto program = parser->parseProgram();
        checkErrors(parser->errors());

        auto actual = program->string();
        REQUIRE(actual == tt.expected);
    }
}