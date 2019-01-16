#pragma once

#include <iostream>
#include <map>
#include <string>
#include <thread>

#include <opencv2/core/core.hpp>

#include <nanogui/imagepanel.h>
#include <nanogui/imageview.h>
#include <nanogui/nanogui.h>

#include "DataGen.h"
#include "TrajectoryView.h"
#include "VisScreen.h"

class Vis {
public:
  Vis();
  void initUI();
  void startUI();
  void addTrajectoryPoint(nanogui::Vector3f);
  void loadNewestKeyframe(const odometry::KeyFrame &);
  void someFunc();

private:
  double m_lastFrameTime;
  int m_numElapsedFrames;
  double m_fps;

  std::map<std::string, cv::Mat> m_frames;
  std::vector<nanogui::Vector3f> m_pointBuffer;
  std::vector<odometry::KeyFrame> m_keyframeBuffer;
  TrajectoryView *m_view;
};