cmake_minimum_required(VERSION 3.13)
project(hold VERSION 0.0.1 LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories(include)

set(SOURCES src/vm.c)

add_library(libhold STATIC ${SOURCES})

set(APP_SOURCES src_app/main.c)

add_executable(hold ${APP_SOURCES})

target_link_libraries(hold PRIVATE libhold)
