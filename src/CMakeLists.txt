add_library(${APP_LIB} TrajectoryView.h Vis.cpp Vis.h VisScreen.cpp VisScreen.h Keyframe.h Keyframe.cpp)
add_executable(${APP_EXE} main.cpp)

target_link_libraries(${APP_LIB} nanogui opencv_core opencv_highgui opencv_features2d opencv_imgcodecs)
target_link_libraries(${APP_EXE} ${APP_LIB})