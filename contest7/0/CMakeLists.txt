cmake_minimum_required(VERSION 2.6.0)

project(contest6 C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -Wall -Wextra -fsanitize=address -fsanitize=undefined -fno-sanitize-recover -fstack-protector -lm")

add_executable(0 0.c)
