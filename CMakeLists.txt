cmake_minimum_required(VERSION 3.14)

SET(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(atomic_operations)

find_package(benchmark REQUIRED)

add_executable(main main.cpp)
target_link_libraries(main benchmark::benchmark)