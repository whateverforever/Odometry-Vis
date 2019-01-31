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
#define N_FRAMES 23
#endif

class KittiImport {
public:
  KittiImport();
  odometry::KeyFrame getLatestKeyframe();

private:
  void load_data(std::string, std::vector<cv::Mat> &, std::vector<cv::Mat> &,
                 std::vector<cv::Mat> &, Eigen::MatrixXf &, int);

  int m_lastFrameIdx;
  Eigen::MatrixXf m_poses;
  std::vector<cv::Mat> m_gray;
  std::vector<cv::Mat> m_depth;
  std::vector<cv::Mat> m_mask;
};

KittiImport::KittiImport()
    : m_poses(7, N_FRAMES), m_gray(N_FRAMES), m_mask(N_FRAMES),
      m_depth(N_FRAMES) {

  m_lastFrameIdx = 0;
  load_data(DATA_PATH_KITTI + "/associated.txt", m_gray, m_depth, m_mask,
            m_poses, N_FRAMES);

  std::cout << "Load data done: " << N_FRAMES << " frames" << std::endl;
}

odometry::KeyFrame KittiImport::getLatestKeyframe() {
  if (m_lastFrameIdx == m_gray.size()) {
    m_lastFrameIdx = 0;
  }

  cv::Mat rgbRight;
  cv::Mat rgbLeft = cv::Mat::zeros(376, 1241, CV_8UC3);

  // convert to rgb
  std::cout << "gray:\n" << m_gray[m_lastFrameIdx] << std::endl;

  cv::cvtColor(m_gray[m_lastFrameIdx], rgbLeft, cv::COLOR_GRAY2BGR);
  rgbLeft.convertTo(rgbLeft, CV_8UC3);

  rgbLeft.copyTo(rgbRight);

  auto p_leftRGB = std::make_shared<cv::Mat>(rgbLeft);
  auto p_rightRGB = std::make_shared<cv::Mat>(rgbRight);
  auto p_leftDepth = std::make_shared<cv::Mat>(m_depth[m_lastFrameIdx]);
  auto p_depthMask = std::make_shared<cv::Mat>(m_mask[m_lastFrameIdx]);

  Eigen::VectorXf pose_raw = m_poses.col(m_lastFrameIdx);
  Eigen::Quaternionf pose_quat(pose_raw[0], pose_raw[1], pose_raw[2],
                               pose_raw[3]);
  Eigen::Vector3f pose_translation = pose_raw.block<3, 1>(4, 0);

  odometry::Affine4f pose;
  pose.block<3, 3>(0, 0) = pose_quat.normalized().toRotationMatrix();
  pose.block<3, 1>(0, 3) = pose_translation;

  m_lastFrameIdx += 1;

  return odometry::KeyFrame(p_leftRGB, p_rightRGB, p_leftDepth, p_depthMask,
                            pose);
}

void KittiImport::load_data(std::string filename, std::vector<cv::Mat> &gray,
                            std::vector<cv::Mat> &depth,
                            std::vector<cv::Mat> &mask, Eigen::MatrixXf &poses,
                            int n_frames) {
  std::string line;
  std::ifstream file(filename);

  int counter = 0;
  if (file.is_open()) {
    while (std::getline(file, line) && counter < n_frames) {
      std::vector<std::string> items;
      std::string item;
      std::stringstream ss(line);

      while (std::getline(ss, item, '	')) {
        items.push_back(item);
      }

      std::string nameDepth = items[1];
      std::string nameGray = items[2];
      std::string nameMask = items[3];
      // from here on we have the pose matrix -> 15

      // -> load gray
      std::string filename_gray = std::string(DATA_PATH_KITTI + "/") + nameGray;
      cv::Mat gray_8u = cv::imread(filename_gray, cv::IMREAD_GRAYSCALE);

      if (gray_8u.empty()) {
        std::cout << "read img failed for: " << counter << filename_gray
                  << std::endl;
        std::exit(-1);
      }

      gray_8u.convertTo(gray[counter], PixelType);
      // <-

      // -> load depth
      std::string filename_depth =
          std::string(DATA_PATH_KITTI + "/") + nameDepth;
      cv::Mat depth_img = cv::imread(filename_depth, cv::IMREAD_UNCHANGED);

      if (depth_img.empty()) {
        std::cout << "read depth img failed for: " << counter << std::endl;
        std::exit(-1);
      }

      depth_img.convertTo(depth[counter], PixelType, 1.0f / 386.1448f);
      // <-

      // -> load mask
      std::string filename_mask = std::string(DATA_PATH_KITTI + "/") + nameMask;
      cv::Mat mask_16u = cv::imread(filename_mask, CV_16U);

      if (mask_16u.empty()) {
        std::cout << "read mask failed for: " << counter << std::endl;
        std::exit(-1);
      }

      mask_16u.convertTo(mask[counter], CV_8UC1); // one channel for binary vals
      // <-

      // -> pose
      // clang-format off
      Eigen::Vector3f t(
        std::stof(items[7]),
        std::stof(items[11]),
        std::stof(items[15])
      ); // <- translation T

      Eigen::Matrix3f rotationMat;
      rotationMat << std::stof(items[4]), std::stof(items[5]), std::stof(items[6]),
                     std::stof(items[8]), std::stof(items[9]), std::stof(items[10]),
                     std::stof(items[12]), std::stof(items[13]), std::stof(items[14]);
      // clang-format on

      Eigen::Quaternionf q(rotationMat);

      // wxyz

      // Eigen::Quaternionf q(std::stof(items[7]), std::stof(items[4]),
      // std::stof(items[5]), std::stof(items[6])); // <- rotation in Eigen:
      // w,x,y,z Eigen::Vector3f a =
      // Eigen::AngleAxisf(q).angle()*Eigen::AngleAxisf(q).axis(); // <--
      // convert to axis angle

      // clang-format off
      poses.col(counter) << q.w(),
                            q.x(),
                            q.y(),
                            q.z(),
                            t(0),
                            t(1),
                            t(2);
      // clang-format on

      counter++;
    }
    file.close();
  }
  assert(counter == n_frames);
};
