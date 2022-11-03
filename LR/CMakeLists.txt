cmake_minimum_required(VERSION 3.23)
project(LR C)

set(CMAKE_C_STANDARD 99)

add_executable(LR main.c lex.yy.c parser.c)
