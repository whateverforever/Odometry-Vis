#pragma once

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include <Eigen/Core>
#include <Eigen/Geometry>

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>

#include <DataGen.h> //for random float
#include <Keyframe.h>

#include <Constants.h>
#include <data_types.h>

#ifndef N_FRAMES
#define N_FRAMES 32 // 200
#endif

class MockCameraInput {
public:
  MockCameraInput();
  odometry::KeyFrame getLatestKeyframe();

private:
  void load_data(std::string, std::vector<cv::Mat> &, std::vector<cv::Mat> &,
                 Eigen::MatrixXf &, int);

  int m_lastFrameIdx;
  Eigen::MatrixXf m_poses;
  std::vector<cv::Mat> m_gray;
  std::vector<cv::Mat> m_depth;
};
