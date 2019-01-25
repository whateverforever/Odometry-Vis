#pragma once

#include <vector>

#include <nanogui/nanogui.h>
#include <opencv2/opencv.hpp>

#include <Eigen/Geometry>

#include "Keyframe.h"

class DataGenerator {
public:
  DataGenerator();
  odometry::KeyFrame getLatestKeyframe();

private:
  cv::Mat m_image_1;
  cv::Mat m_image_2;
};

float RandomFloat(float, float);