cmake_minimum_required(VERSION 3.4)
project(python_mincut)
add_subdirectory(extern/pybind11)
pybind11_add_module(mincut src/main.cpp)

