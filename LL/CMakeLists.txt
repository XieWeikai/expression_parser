cmake_minimum_required(VERSION 3.23)
project(LL C)

set(CMAKE_C_STANDARD 99)

add_executable(LL main.c lex.yy.c parser.c)
