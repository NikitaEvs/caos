cmake_minimum_required(VERSION 2.6.0)

project(contest6 C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -lm -g -Wall -Wextra -fsanitize=undefined -fno-sanitize-recover -fstack-protector")

add_executable(4 4.c)
target_link_libraries(4 m)
