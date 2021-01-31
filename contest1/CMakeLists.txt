cmake_minimum_required(VERSION 2.6.0)

project(contest1 C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -Wall -Wextra -fsanitize=address -fsanitize=undefined -fno-sanitize-recover -fstack-protector -lm")
set(SOURCE_ROOT src)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "bin")

include_directories(
	src
	include
	)

if(BUILD_TEST)
	add_subdirectory(tests)
endif()

add_executable(${TASK_NUMBER} ${SOURCE_ROOT}/${TASK_NUMBER}.c)
