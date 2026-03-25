cmake_minimum_required(VERSION 3.13)
project(Hold VERSION 0.0.1 LANGUAGES C)

set(CMAKE_C_COMPILER "gcc")
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_compile_options(-Wall -Wextra -Wunused-function)
include_directories(include)

set(SOURCES src/vm.c src/gc.c src/arena.c src/str_table.c src/lexer.c)

add_library(libhold STATIC ${SOURCES})
set_target_properties(libhold PROPERTIES OUTPUT_NAME "hold")

set(APP_SOURCES src_app/main.c)

add_executable(hold ${APP_SOURCES})

target_link_libraries(hold PRIVATE libhold)
