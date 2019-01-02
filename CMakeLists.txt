cmake_minimum_required(VERSION 3.12)
project(Visualization)

set(CMAKE_CXX_STANDARD 14)
set(APP_EXE main)
set(APP_LIB Visualization)

# Nanogui
add_subdirectory(lib/nanogui)
include_directories(lib/nanogui/include)
include_directories(${NANOGUI_EXTRA_INCS})
add_definitions(${NANOGUI_EXTRA_DEFS})

# OpenCV
find_package(OpenCV REQUIRED)
include_directories(${OPENCV_INCLUDE_DIR})

# My Lib
add_subdirectory(src)