project(monkey_lib)

set(HEADER_FILES
        Lexer.h
        Token.h
        REPL.h
        utilities.h
        AST.h
        Parser.h
        Object.h
        Evaluator.h
        Environment.h)
set(SOURCE_FILES
        Lexer.cpp
        Token.cpp
        REPL.cpp
        utilities.cpp
        AST.cpp
        Parser.cpp
        Object.cpp
        Evaluator.cpp
        Environment.cpp
        )

add_library(Monkey_lib STATIC ${SOURCE_FILES} ${HEADER_FILES})