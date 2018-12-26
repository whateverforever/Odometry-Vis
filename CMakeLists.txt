cmake_minimum_required(VERSION 3.12)
project(Visualization)

set(CMAKE_CXX_STANDARD 14)
set(APP_EXE main)
set(APP_LIB Visualization)

# Nanogui
add_subdirectory(nanogui)
include_directories(nanogui/include)
include_directories(${NANOGUI_EXTRA_INCS})
add_definitions(${NANOGUI_EXTRA_DEFS})

# OpenCV
find_package(OpenCV REQUIRED)
include_directories(${OPENCV_INCLUDE_DIR})

# VTK
find_package(VTK 8.1.2 REQUIRED)
include(${VTK_USE_FILE})

add_library(${APP_LIB} library.cpp library.h)
add_executable(${APP_EXE} main.cpp main.h trajectoryView.h)

target_link_libraries(${APP_EXE} ${APP_LIB} nanogui opencv_core opencv_highgui opencv_features2d opencv_imgcodecs)
target_link_libraries(${APP_LIB} nanogui opencv_core opencv_highgui opencv_features2d opencv_imgcodecs)

if(VTK_LIBRARIES)
    target_link_libraries(${APP_EXE} ${VTK_LIBRARIES})
else()
    target_link_libraries(${APP_EXE} vtkHybrid vtkWidgets)
endif()