#include <iostream>
#include "Token.h"

TokenType lookupIdent(const std::string &ident) {
    if (keywords.contains(ident)) return keywords.at(ident);
    return IDENT;
}

std::ostream &operator<<(std::ostream &os, Token const &token){
    return os << "{Type:"
              << token.type
              << " Literal:"
              << token.literal
              << "}"
              << std::endl;
}