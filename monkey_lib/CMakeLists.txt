project(monkey_lib)

set(HEADER_FILES Lexer.h Token.h REPL.h utilities.h AST.h Parser.h)
set(SOURCE_FILES Lexer.cpp Token.cpp REPL.cpp utilities.cpp AST.cpp Parser.cpp)

add_library(Monkey_lib STATIC ${SOURCE_FILES} ${HEADER_FILES})