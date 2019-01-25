cmake_minimum_required(VERSION 3.12)
project(Visualization)

set(CMAKE_CXX_STANDARD 14)
set(APP_EXE main)
set(APP_LIB Visualization)
set(CMAKE_CXX_FLAGS "-O3")

# Nanogui
add_subdirectory(lib/nanogui)
add_definitions(${NANOGUI_EXTRA_DEFS})

# OpenCV
find_package(OpenCV REQUIRED)
include_directories(${OPENCV_INCLUDE_DIR})

# My Lib
include_directories(include)

add_library(${APP_LIB} STATIC
    src/Vis.cpp
    src/VisScreen.cpp
    src/Keyframe.cpp
    src/DataGen.cpp
    )
add_executable(${APP_EXE} src/main.cpp)

target_link_libraries(${APP_LIB} nanogui ${NANOGUI_EXTRA_LIBS} opencv_imgcodecs)
target_link_libraries(${APP_EXE} ${APP_LIB})

# Absolutely needed for the parent not to forget nanogui, eigen, etc...
target_include_directories(${APP_LIB} PUBLIC lib/nanogui/include)
target_include_directories(${APP_LIB} PUBLIC ${NANOGUI_EXTRA_INCS})