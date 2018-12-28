#pragma once

#include <iostream>
#include <map>
#include <string>
#include <thread>

#include <opencv2/core/core.hpp>

#include <nanogui/nanogui.h>
#include <nanogui/imagepanel.h>
#include <nanogui/imageview.h>

#include "trajectoryView.h"
#include "DataGen.h"

class Vis {
public:
    Vis();
    void initUI();
    void startUI();
    void setDataSource(DataGenerator*);
private:
    std::map<std::string, cv::Mat> m_frames;
    TrajectoryView *m_view;
    DataGenerator *m_dataSource;
};