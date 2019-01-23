include_directories(../include)

add_library(${APP_LIB} Vis.cpp VisScreen.cpp Keyframe.cpp)
add_executable(${APP_EXE} main.cpp)

target_link_libraries(${APP_LIB} nanogui opencv_imgcodecs)
target_link_libraries(${APP_EXE} ${APP_LIB})