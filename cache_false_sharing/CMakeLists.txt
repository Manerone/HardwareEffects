cmake_minimum_required(VERSION 3.14)

SET(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(false_sharing)

find_package(benchmark REQUIRED)

add_executable(false_sharing experiment.cpp)
target_link_libraries(false_sharing benchmark::benchmark)