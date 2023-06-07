cmake_minimum_required(VERSION 3.0.0)
project(lotto-645-simulator VERSION 0.1.0)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_HAVE_THREADS_LIBRARY 1)
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -DDEBUG")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")

include(CTest)
enable_testing()

add_executable(lotto-645-simulator main.c)
target_sources(lotto-645-simulator PRIVATE 
    console/console.c
    console/menu.c
    console/input.c
    console/page.c
    console/dialog.c
    chacha20/chacha20.c
    util/parse_string.c
    util/split_whitespace.c
    pages/main_menu.c
    pages/about_page.c
    pages/single_game.c
    pages/single_game_result.c
    pages/single_game_file.c
    pages/multi_game.c
    pages/multi_game_result.c
)

add_executable(simple_console tests/simple_console.c)
target_sources(simple_console PRIVATE 
console/console.c
console/menu.c
console/input.c
console/page.c
console/dialog.c
chacha20/chacha20.c
util/parse_string.c
util/split_whitespace.c
pages/main_menu.c
pages/about_page.c
pages/single_game.c
pages/single_game_result.c
pages/single_game_file.c
pages/multi_game.c
pages/multi_game_result.c
)

add_test(NAME simple_console COMMAND simple_console)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
