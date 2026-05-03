@_:
    just --list

[group('scripts')]
build:
    cmake -S . -B build
    cmake --build build

[group('scripts')]
repl:
    just build
    cd build && ./monkey

[group('scripts')]
test:
    just build
    cd build/tests && ./Catch_tests_run