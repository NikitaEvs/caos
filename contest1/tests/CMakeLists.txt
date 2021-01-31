cmake_minimum_required(VERSION 3.15)
project(tests_build)

find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-g -Wall -Wextra -Werror")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ../bin)

add_executable(run src/${TASK_NUMBER}.cpp)
target_link_libraries(run ${GTEST_LIBRARIES} pthread)
