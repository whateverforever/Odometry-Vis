#ifndef VISUALIZATION_LIBRARY_H
#define VISUALIZATION_LIBRARY_H

#include <iostream>
#include <map>
#include <string>
#include <thread>

#include <opencv2/core/core.hpp>

#include <nanogui/nanogui.h>
#include <nanogui/imagepanel.h>
#include <nanogui/imageview.h>

#include "trajectoryView.h"

class Vis {
public:
    Vis();
    void initUI();
    void startUI();
    void addPoint();
    void calledFromOutSide();
private:
    std::map<std::string, cv::Mat> m_frames;
    nanogui::MatrixXf m_trajectoryPoints;
    nanogui::Screen *m_screen;
    TrajectoryView *m_view;
};

#endif