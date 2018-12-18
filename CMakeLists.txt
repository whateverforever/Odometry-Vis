cmake_minimum_required(VERSION 3.12)
project(Visualization)

set(CMAKE_CXX_STANDARD 14)

add_library(Visualization library.cpp library.h)

add_executable(main main.cpp main.h)