cmake_minimum_required(VERSION 3.13 FATAL_ERROR)

project(small_test CXX)

add_executable(main main_tuple.cpp)
target_compile_options(main PRIVATE -std=c++17)
