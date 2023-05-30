cmake_minimum_required(VERSION 3.0.0)
project(lotto-645-simulator VERSION 0.1.0)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_HAVE_THREADS_LIBRARY 1)
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -DDEBUG")

include(CTest)
enable_testing()

add_executable(lotto-645-simulator main.c)
target_sources(lotto-645-simulator PRIVATE console/console.c)
target_sources(lotto-645-simulator PRIVATE console/menu.c)
target_sources(lotto-645-simulator PRIVATE chacha20/chacha20.c)

add_executable(simple_console tests/simple_console.c)
target_sources(simple_console PRIVATE console/console.c)
target_sources(simple_console PRIVATE console/menu.c)
target_sources(simple_console PRIVATE chacha20/chacha20.c)

add_test(NAME simple_console COMMAND simple_console)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
