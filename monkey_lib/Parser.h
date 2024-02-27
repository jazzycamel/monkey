#ifndef MONKEY_PARSER_H
#define MONKEY_PARSER_H

#include <memory>
#include <string>
#include <vector>
#include <map>

#include "Lexer.h"
#include "Token.h"
#include "AST.h"

class Parser;

typedef std::shared_ptr<Expression> (Parser::*prefixParseFn)();

typedef std::shared_ptr<Expression> (Parser::*infixParseFn)(std::shared_ptr<Expression>);

enum {
    LOWEST = 1,
    EQUALS,
    LESS_GREATER,
    SUM,
    PRODUCT,
    PREFIX,
    CALL
};

const std::map<TokenType, int> precedences = {
        {EQ, EQUALS},
        {NOT_EQ, EQUALS},
        {LT, LESS_GREATER},
        {GT, LESS_GREATER},
        {PLUS, SUM},
        {MINUS, SUM},
        {SLASH, PRODUCT},
        {ASTERISK, PRODUCT}
};

class Parser {
public:
    explicit Parser(Lexer *lexer);

    Program *parseProgram();

    std::vector<std::string> errors();

private:
    void _nextToken();

    std::shared_ptr<Statement> _parseStatement();

    std::shared_ptr<LetStatement> _parseLetStatement();

    std::shared_ptr<ReturnStatement> _parseReturnStatement();

    std::shared_ptr<ExpressionStatement> _parseExpressionStatement();

    std::shared_ptr<Expression> _parseExpression(int precedence);

    std::shared_ptr<Expression> _parseIdentifier();

    std::shared_ptr<Expression> _parseIntegerLiteralExpression();

    std::shared_ptr<Expression> _parsePrefixExpression();

    std::shared_ptr<Expression> _parseInfixExpression(std::shared_ptr<Expression> left);

    void _noPrefixParseFnError(const TokenType &t);

    void _registerPrefix(const TokenType &tokenType, prefixParseFn fn);

    void _registerInfix(const TokenType &tokenType, infixParseFn fn);

    [[nodiscard]] bool _currentTokenIs(const TokenType &t) const;

    [[nodiscard]] bool _peekTokenIs(const TokenType &t) const;

    bool _expectPeek(const TokenType &t);

    void _peekError(const TokenType &t);

    [[nodiscard]] int _peekPrecedence() const;

    [[nodiscard]] int _currentPrecedence() const;

    Lexer *_lexer;
    Token _currentToken;
    Token _peekToken;
    std::vector<std::string> _errors;
    std::map<TokenType, prefixParseFn> _prefixParseFns;
    std::map<TokenType, infixParseFn> _infixParseFns;
};

#endif //MONKEY_PARSER_H
