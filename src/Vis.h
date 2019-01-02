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
  void setDataSource(DataGenerator *);
  void someFunc();

private:
  double m_lastFrameTime;
  int m_numElapsedFrames;
  double m_fps;

  std::map<std::string, cv::Mat> m_frames;
  TrajectoryView *m_view;
  DataGenerator *m_dataSource;
};