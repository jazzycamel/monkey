cmake_minimum_required(VERSION 3.27)
project(monkey)

set(CMAKE_CXX_STANDARD 23)

add_executable(monkey main.cpp)

include_directories(monkey_lib)
add_subdirectory(monkey_lib)

target_link_libraries(monkey Monkey_lib)

add_subdirectory(tests)