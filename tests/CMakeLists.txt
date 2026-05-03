find_package(Catch2 3 QUIET)
if(NOT Catch2_FOUND)
    include(FetchContent)
    FetchContent_Declare(
            Catch2
            GIT_REPOSITORY https://github.com/catchorg/Catch2.git
            GIT_TAG        v3.5.4
    )
    FetchContent_MakeAvailable(Catch2)
    list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
endif()

add_executable(Catch_tests_run Lexer_tests.cpp Parser_tests.cpp
        AST_tests.cpp
        Evaluator_tests.cpp)

target_link_libraries(Catch_tests_run PRIVATE Monkey_lib)
target_link_libraries(Catch_tests_run PRIVATE Catch2::Catch2WithMain)

include(Catch)
catch_discover_tests(Catch_tests_run)