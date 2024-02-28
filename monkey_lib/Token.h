#ifndef MONKEY_TOKEN_H
#define MONKEY_TOKEN_H

#include <map>
#include <string>

typedef std::string TokenType;

typedef struct {
  TokenType type;
  std::string literal;
} Token;

const TokenType ILLEGAL = "ILLEGAL", EOF_ = "EOF",

                // Identifiers and literals
    IDENT = "IDENT", INT = "INT",

                // Operators
    ASSIGN = "=", PLUS = "+", MINUS = "-", BANG = "!", ASTERISK = "*",
                SLASH = "/", LT = "<", GT = ">", EQ = "==", NOT_EQ = "!=",

                // Delimiters
    COMMA = ",", SEMICOLON = ";", LPAREN = "(", RPAREN = ")", LBRACE = "{",
                RBRACE = "}",

                // Keywords
    FUNCTION = "FUNCTION", LET = "LET", TRUE = "TRUE", FALSE = "FALSE",
                IF = "IF", ELSE = "ELSE", RETURN = "RETURN";

const std::map<std::string, TokenType> keywords = {
    {"fn", FUNCTION}, {"let", LET},   {"true", TRUE},     {"false", FALSE},
    {"if", IF},       {"else", ELSE}, {"return", RETURN},
};

TokenType lookupIdent(const std::string &ident);

std::ostream &operator<<(std::ostream &os, Token const &token);

#endif // MONKEY_TOKEN_H
