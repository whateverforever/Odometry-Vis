#ifndef VISUALIZATION_LIBRARY_H
#define VISUALIZATION_LIBRARY_H

#include <iostream>
#include <vector>

#include <opencv2/core/core.hpp>

#include <nanogui/nanogui.h>
#include <nanogui/imagepanel.h>
#include <nanogui/imageview.h>

class Vis {
public:
    Vis();
    void hello();
    void what();
private:
    std::vector<&cv::Mat> m_frames;
    nanogui::MatrixXf m_trajectoryPoints;
};

#endif