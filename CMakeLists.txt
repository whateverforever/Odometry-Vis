cmake_minimum_required(VERSION 3.12)
project(Visualization)

set(CMAKE_CXX_STANDARD 14)
set(APP_EXE main)

add_subdirectory(nanogui)
include_directories(nanogui/include)
include_directories(${NANOGUI_EXTRA_INCS})
add_definitions(${NANOGUI_EXTRA_DEFS})

add_library(Visualization library.cpp library.h)
add_executable(${APP_EXE} main.cpp main.h)

target_link_libraries(${APP_EXE} Visualization nanogui)