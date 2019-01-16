#pragma once

#include <vector>

#include <nanogui/nanogui.h>
#include <opencv2/opencv.hpp>

#include "Keyframe.h"
#include "Vis.h"

class DataGenerator {
 public:
  DataGenerator();
  odometry::KeyFrame getLatestKeyframe();

 private:
  cv::Mat m_image_1;
  cv::Mat m_image_2;
};

DataGenerator::DataGenerator() {
  m_image_1 = cv::imread(
      "../data/rgbd_dataset_freiburg3_teddy/rgb/1341841873.273798.png",
      CV_LOAD_IMAGE_COLOR);  // Read the file
  m_image_2 = cv::imread(
      "../data/rgbd_dataset_freiburg3_teddy/rgb/1341841879.367159.png",
      CV_LOAD_IMAGE_COLOR);  // Read the file
}

float RandomFloat(float a, float b) {
  float random = ((float)rand()) / (float)RAND_MAX;
  float diff = b - a;
  float r = random * diff;
  return a + r;
}

odometry::KeyFrame DataGenerator::getLatestKeyframe() {
  auto p_leftRGB = std::make_shared<cv::Mat>(m_image_1);
  auto p_rightRGB = std::make_shared<cv::Mat>(m_image_2);
  auto p_leftDepth = std::make_shared<cv::Mat>(m_image_1);
  auto p_leftValue = std::make_shared<cv::Mat>(m_image_2);
  // clang-format off
  odometry::Affine4f origin;
  origin << 0, 0, 0, 0,
            0, 0, 0, 0,
            0, 0, 0, 0,
            0, 0, 0, 0;
  // clang-format on

  auto kframe = odometry::KeyFrame(p_leftRGB, p_rightRGB, p_leftDepth,
                                   p_leftValue, origin);
}