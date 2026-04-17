cmake_minimum_required(VERSION 4.1)
project(ZabavaEngine VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


set(ENGINE_LOGIC
        src/board.cpp
        src/movegen.cpp
        src/eval.cpp
        src/search.cpp
        src/uci.cpp
)


add_executable(ZabavaEngine src/main.cpp ${ENGINE_LOGIC})
target_include_directories(ZabavaEngine PRIVATE include)


add_executable(ZabavaTests tests/tests.cpp ${ENGINE_LOGIC})
target_include_directories(ZabavaTests PRIVATE include tests)


if(MSVC)
    target_compile_options(ZabavaEngine PRIVATE /O2 /Oi)
else()
    target_compile_options(ZabavaEngine PRIVATE -O3 -march=native)
endif()