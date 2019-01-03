#pragma once

#include <vector>

#include <nanogui/nanogui.h>
#include <opencv2/opencv.hpp>

#include "Vis.h"

class DataGenerator {
public:
  DataGenerator();
  void updateValue();
  friend class Vis;

private:
  int m_someValue;

  std::vector<nanogui::Vector3f> m_trajectoryPoints;

  cv::Mat *m_activeImage;
  cv::Mat m_image_1;
  cv::Mat m_image_2;
};

DataGenerator::DataGenerator() {
  m_someValue = 555;

  m_image_1 = cv::imread(
      "../data/rgbd_dataset_freiburg3_teddy/rgb/1341841873.273798.png",
      CV_LOAD_IMAGE_COLOR); // Read the file
  m_image_2 = cv::imread(
      "../data/rgbd_dataset_freiburg3_teddy/rgb/1341841879.367159.png",
      CV_LOAD_IMAGE_COLOR); // Read the file

  m_activeImage = &m_image_1;
}

float RandomFloat(float a, float b) {
    float random = ((float) rand()) / (float) RAND_MAX;
    float diff = b - a;
    float r = random * diff;
    return a + r;
}

void DataGenerator::updateValue() {
  m_someValue += 10;

  m_trajectoryPoints.push_back(nanogui::Vector3f(
    RandomFloat(-2,2),
    RandomFloat(-2,2),
    RandomFloat(-2,2)));

  if (m_activeImage == &m_image_1) {
    m_activeImage = &m_image_2;
  } else {
    m_activeImage = &m_image_1;
  }
}